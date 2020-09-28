//
// Created by Mateusz Święszek on 26/08/2020.
//

#include "EvaporationComponent.h"
#include <cmath>

using namespace cl;

EvaporationComponent::EvaporationComponent(Configurations &config) : config(config) {}

void EvaporationComponent::update(sycl::queue& queue, CellGrid& cells,
                                  sycl::buffer<Cell::Params, 1>& cellParamsBuf,
                                  sycl::buffer<OilPoint::Params, 1>& opParamsBuf,
                                  sycl::buffer<OilComponent, 2>& opCompBuf,
                                  int timestep) {
    //mass loss of every component in oil point
    sycl::buffer<double, 2> lossMassArrayBuf(opCompBuf.get_range());
    //number of oil points in every cell
    sycl::buffer<int, 1> cellOpNumBuf(sycl::range<1>(cellParamsBuf.get_count()));
    {
        auto cellOpNumO = cellOpNumBuf.get_access<sycl::access::mode::write>();
        auto opParamsI = opParamsBuf.get_access<sycl::access::mode::read>();

        int col = cells.getCol();
        auto id = [col](OilPoint::Params::CellPos pos) -> int{
            return pos.x * col + pos.y;
        };

        for(int i=0; i<cellParamsBuf.get_count(); i++){
            cellOpNumO[i] = 0;
        }

        for(int i=0; i<opParamsBuf.get_count(); i++){
            auto& op = opParamsI[i];
            cellOpNumO[id(op.cellPos)]++;
        }
    }

    queue.submit([&](sycl::handler& cgh){
        auto cellParamsIO = cellParamsBuf.get_access<sycl::access::mode::read_write>(cgh);
        auto opParamsIO = opParamsBuf.get_access<sycl::access::mode::read_write>(cgh);
        auto opCompIO = opCompBuf.get_access<sycl::access::mode::read_write>(cgh);
        auto lossMassArrayIO = lossMassArrayBuf.get_access<sycl::access::mode::read_write>(cgh);
        auto cellOpNumI = cellOpNumBuf.get_access<sycl::access::mode::read>(cgh);

        int col = cells.getCol();
        auto id = [col](OilPoint::Params::CellPos pos) -> int{
            return pos.x * col + pos.y;
        };
        auto evaporatedRatio = [](const OilPoint::Params& op) -> double{
            return op.evaporatedMass / op.initialMassOfOilPoint;
        };

        int cellSize = config.cellSize;
        int comp_len = config.oilComponents.size();
        cgh.parallel_for<class ECUpdate>(sycl::range<1>(opParamsBuf.get_count()), [=](sycl::id<1> i){
            auto& op = opParamsIO[i];

            if(op.removed)
                return;

            double temp = cellParamsIO[id(op.cellPos)].temperature;

            double totalMole = 0;
            for (int j=0; j<comp_len; j++) {
                totalMole += (opCompIO[i[0]][j].x / opCompIO[i[0]][j].molecularWeigth);
            }

            double x = 0;
            for (int j=0; j<comp_len; j++) {
                x = opCompIO[i[0]][j].x / (opCompIO[i[0]][j].molecularWeigth * totalMole);

                if (x != 0) {
                    double molecularWeigth = opCompIO[i[0]][j].molecularWeigth;
                    double A = -(4.4 + sycl::log(opCompIO[i[0]][j].tb)) * (1.803 * (opCompIO[i[0]][j].tb / temp - 1) - 0.803 * sycl::log(opCompIO[i[0]][j].tb / temp));
                    double P = (double) (100000 * sycl::exp(A)); // Vapor pressure
                    double lossmass = (K * molecularWeigth * P * x / (R * temp)
                                       * timestep * (cellSize * cellSize)) / cellOpNumI[id(op.cellPos)]; //number of oilPoints in cell
                    lossMassArrayIO[i[0]][j] = lossmass;
                } else {
                    lossMassArrayIO[i[0]][j] = 0;
                }
            }

            double actualMass = op.mass;
            double totalLossMass = 0;
            for (int j=0; j<comp_len; j++) {
                totalLossMass += lossMassArrayIO[i[0]][j];
            }

            double newMass = actualMass - totalLossMass;
            for(int j=0; j<comp_len; j++) {
                double lossMass = lossMassArrayIO[i[0]][j];
                if (lossMass > 0) {
                    double newX = (opCompIO[i[0]][j].x * actualMass - lossMass) / newMass;
                    if (newX < 0.00001) {
                        newX = 0;
                    }
                    opCompIO[i[0]][j].x = newX;
                }

            }

            if (newMass > 0.00001) {
                double lastEvaporatedRatio = evaporatedRatio(op);
                op.mass = newMass;

                op.evaporatedMass = op.evaporatedMass + totalLossMass;
                op.massOfEmulsion = op.massOfEmulsion - totalLossMass;
                op.lastDeltaF = evaporatedRatio(op) - lastEvaporatedRatio;
            } else {
                op.removed = true;
            }
        });
    });
}
