//
// Created by Mateusz Święszek on 18/08/2020.
//

#include "DispersionComponent.h"


DispersionComponent::DispersionComponent(Configurations &config) : config(config) {}

void DispersionComponent::update(sycl::queue& queue, CellGrid& cells,
                                 sycl::buffer<Cell::Params, 1>& cellParamsBuf,
                                 sycl::buffer<OilPoint::Params, 1>& opParamsBuf,
                                 sycl::buffer<OilComponent, 2>& opCompBuf,
                                 int timestep) {
    sycl::buffer<double, 1> cellVolBuf(sycl::range<1>(cellParamsBuf.get_count()));
    sycl::buffer<double, 1> meBuf(sycl::range<1>(cellParamsBuf.get_count()));
    sycl::buffer<double, 1> dBuf(sycl::range<1>(cellParamsBuf.get_count()));

    {
        auto cellVolO = cellVolBuf.get_access<sycl::access::mode::write>();
        auto meIO = meBuf.get_access<sycl::access::mode::read_write>();
        auto dIO = dBuf.get_access<sycl::access::mode::read_write>();
        auto opParamsI = opParamsBuf.get_access<sycl::access::mode::read>();

        for(int i=0; i<opParamsBuf.get_count(); i++){
            dIO[cells.id(opParamsI[i].cellPos)] += opParamsI[i].density * opParamsI[i].massOfEmulsion;
            meIO[cells.id(opParamsI[i].cellPos)] += opParamsI[i].massOfEmulsion;
        }
        for(int i=0; i<cellParamsBuf.get_count(); i++){
            cellVolO[i] = (meIO[i]*meIO[i])/dIO[i];
        }
    }

    queue.submit([&](sycl::handler& cgh){
        auto opParamsIO = opParamsBuf.get_access<sycl::access::mode::read_write>(cgh);
        auto cellParamsI = cellParamsBuf.get_access<sycl::access::mode::read>(cgh);
        auto cellVolI = cellVolBuf.get_access<sycl::access::mode::read>(cgh);

        auto evaporatedRatio = [](const OilPoint::Params& op) -> double{
            return op.evaporatedMass / op.initialMassOfOilPoint;
        };

        int col = cells.getCol();
        auto id = [col](OilPoint::Params::CellPos pos) -> int {
            return pos.x * col + pos.y;
        };

        auto getR = [](const Vector2& v) -> double{
            return sycl::sqrt(v.x * v.x + v.y * v.y);
        };

        double oilWaterTension = config.oilWaterTension;
        double cellSize = config.cellSize;
        cgh.parallel_for<class DPUpdate>(sycl::range<1>(opParamsBuf.get_count()), [=](sycl::id<1> i){
            auto& op = opParamsIO[i];

            if(op.removed)
                return;

            double tension = oilWaterTension * (1 + evaporatedRatio(op));
            double Da = 0.11 * (getR(cellParamsI[id(opParamsIO[i].cellPos)].wind) + 1) * (getR(cellParamsI[id(opParamsIO[i].cellPos)].wind) + 1);
            double Db = 1 / (1 + 50 * sycl::sqrt(opParamsIO[i].viscosity) * cellVolI[id(op.cellPos)]/(cellSize*cellSize) * 100 * tension);
            double mass = op.mass * Da * Db / 3600 * timestep;

            op.mass = op.mass - mass;
            op.dispersedMass = op.dispersedMass + mass;
            op.massOfEmulsion = op.massOfEmulsion - mass;
        });
    });
}
