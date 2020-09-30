//
// Created by Mateusz Święszek on 17/08/2020.
//

#include "DensityUpdateComponent.h"
#include <iostream>
#include <cmath>

DensityUpdateComponent::DensityUpdateComponent(Configurations &config) : config(config) {
    this->densityAt15K = config.initialDensityOfOilPoint;
    this->salinity = config.salinity;
}

double DensityUpdateComponent::calculateDensity(double tempAtK) {
    double deltaT = tempAtK - 288.15;
    double alpha = 613.97226 / (densityAt15K * densityAt15K);
    double Kt = std::exp(-alpha * deltaT * (1 + 0.8 * alpha * deltaT));
    double result = densityAt15K * Kt;
    return result;
}

double DensityUpdateComponent::calculateWaterDensity(double tempAtK) {
    tempAtK -= 273.15;
    double rho = 1000 * (1.0 - (tempAtK + 288.9414)
                               / (508929.2 * (tempAtK + 68.12963))
                               * (std::pow(tempAtK - 3.9863, 2)));

    double rhos, A, B;
    A = 0.824493 - 0.0040899 * tempAtK + 0.000076438 * std::pow(tempAtK, 2)
        - 0.00000082467 * std::pow(tempAtK, 3) + 0.0000000053675
                                                 * std::pow(tempAtK, 4);
    B = -0.005724 + 0.00010227 * tempAtK - 0.0000016546
                                           *
                                           std::pow(tempAtK, 2);
    rhos = rho + A * salinity + B * std::pow(salinity, (3 / 2))
           + 0.00048314 * std::pow(salinity, 2);

    return rhos;
}

void
DensityUpdateComponent::update(sycl::queue& queue, CellGrid& cells,
                               sycl::buffer<Cell::Params, 1>& cellParamsBuf,
                               sycl::buffer<OilPoint::Params, 1>& opParamsBuf,
                               sycl::buffer<OilComponent, 2>& opCompBuf,
                               int timestep) {
    sycl::buffer<double, 1> oilDensityBuf(sycl::range<1>(cellParamsBuf.get_count()));
    sycl::buffer<double, 1> waterDensityBuf(sycl::range<1>(cellParamsBuf.get_count()));

    {
        auto oilDensityO = oilDensityBuf.get_access<sycl::access::mode::write>();
        auto waterDensityO = waterDensityBuf.get_access<sycl::access::mode::write>();
        auto cellParamsI = cellParamsBuf.get_access<sycl::access::mode::read>();

        for(int i=0; i<cellParamsBuf.get_count(); i++){
            oilDensityO[i] = calculateDensity(cellParamsI[i].temperature);
            waterDensityO[i] = calculateWaterDensity(cellParamsI[i].temperature);
        }
    }

    queue.submit([&](sycl::handler& cgh) {
        auto opParamsIO = opParamsBuf.get_access<sycl::access::mode::read_write>(cgh);
        auto oilDensityI = oilDensityBuf.get_access<sycl::access::mode::read>(cgh);
        auto waterDensityI = waterDensityBuf.get_access<sycl::access::mode::read>(cgh);

        auto calcEmulsification = [](const OilPoint::Params& op) -> double{
            return (op.massOfEmulsion - op.mass) / op.massOfEmulsion;
        };
        auto initialMassOfOilPoint = config.initialMassOfOilPoint;
        auto evaporatedRatio = [initialMassOfOilPoint](const OilPoint::Params& op) -> double{
            return op.evaporatedMass / initialMassOfOilPoint;
        };
        int col = cells.getCol();
        auto id = [col](OilPoint::Params::CellPos pos) -> int{
            return pos.x * col + pos.y;
        };

        cgh.parallel_for<class DUCUpdate>(sycl::range<1>(opParamsBuf.get_count()), [=](sycl::id<1> i){
            auto& op = opParamsIO[i];
            auto cell_i = id(op.cellPos);

            if(op.removed)
                return;

            double emulsification = calcEmulsification(op);
            double evaporationRatio = evaporatedRatio(op);
            double initialOilDensity = oilDensityI[cell_i];
            op.density = (1 - emulsification)
                         * ((0.6 * initialOilDensity - 340) * evaporationRatio + initialOilDensity) +
                         emulsification * waterDensityI[cell_i];
        });
    });
}
