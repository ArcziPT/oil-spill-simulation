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
    sycl::buffer<double, 2> lossMassArrayBuf(opCompBuf.get_range());
    sycl::buffer<double, 2> vpBuf(opCompBuf.get_range());

    {
        auto vpO = vpBuf.get_access<sycl::access::mode::write>();
        auto cellParamsI = cellParamsBuf.get_access<sycl::access::mode::read>();
        auto opParamsI = opParamsBuf.get_access<sycl::access::mode::read>();
        auto opCompI = opCompBuf.get_access<sycl::access::mode::read>();

        int col = cells.getCol();
        auto id = [col](OilPoint::Params::CellPos pos) -> int{
            return pos.x * col + pos.y;
        };

        for(int i=0; i<opParamsBuf.get_count(); i++){
            auto& op = opParamsI[i];
            auto& cell = cellParamsI[id(op.cellPos)];
            for(int j=0; j<config.oilComponents.size(); j++){
                double A = -(4.4 + std::log(opCompI[i][j].tb)) * (1.803 * (opCompI[i][j].tb / cell.temperature - 1) - 0.803 * std::log(opCompI[i][j].tb / cell.temperature));
                vpO[i][j] = (double) (100000 * std::exp(A)); // Vapor pressure
            }
        }
    }

    queue.submit([&](sycl::handler& cgh){
        auto cellParamsIO = cellParamsBuf.get_access<sycl::access::mode::read_write>(cgh);
        auto opParamsIO = opParamsBuf.get_access<sycl::access::mode::read_write>(cgh);
        auto opCompIO = opCompBuf.get_access<sycl::access::mode::read_write>(cgh);
        auto lossMassArrayIO = lossMassArrayBuf.get_access<sycl::access::mode::read_write>(cgh);
        auto vpI = vpBuf.get_access<sycl::access::mode::read>(cgh);

        int col = cells.getCol();
        auto id = [col](OilPoint::Params::CellPos pos) -> int{
            return pos.x * col + pos.y;
        };
        auto evaporatedRatio = [](const OilPoint::Params& op) -> double{
            return op.evaporatedMass / op.initialMassOfOilPoint;
        };

        int cellSize = config.cellSize;
        int comp_len = config.oilComponents.size();
        int op_num = opParamsBuf.get_count();
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
                    double lossmass = (K * molecularWeigth * vpI[i[0]][j] * x / (R * temp)
                                       * timestep * (cellSize * cellSize)) / op_num; //number of oilPoints
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
