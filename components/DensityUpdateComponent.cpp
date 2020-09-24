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
//    auto& cellParams = cells.getCellParams();
//    auto& opParams = cells.getOilPointsParams();
//
//    for (int i=0; i<cellParams.size(); i++) {
//        auto& cell = cellParams[i];
//        for (auto &op : opParams[i]) {
//            double emulsification = op.getEmulsification();
//            double evaporationRatio = op.getEvaporatedRatio();
//            double initialOilDensity = calculateDensity(cell.temperature);
//            op.density = (1 - emulsification)
//                         * ((0.6 * initialOilDensity - 340) * evaporationRatio + initialOilDensity) +
//                         emulsification * calculateWaterDensity(cell.temperature);
//        }
//    }
}
