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
    sycl::buffer<double, 1> cellVolBuf(sycl::range<1>(opParamsBuf.get_count()));
    sycl::buffer<double, 1> sqrtBuf(sycl::range<1>(opParamsBuf.get_count()));
    sycl::buffer<double, 1> windBuf(sycl::range<1>(cellParamsBuf.get_count()));

    {
        auto cellVolO = cellVolBuf.get_access<sycl::access::mode::write>();
        auto sqrtO = sqrtBuf.get_access<sycl::access::mode::write>();
        auto opParamsI = opParamsBuf.get_access<sycl::access::mode::read>();
        auto cellParamsI = cellParamsBuf.get_access<sycl::access::mode::read>();
        auto windO = windBuf.get_access<sycl::access::mode::write>();

        for(int i=0; i<opParamsBuf.get_count(); i++){
            sqrtO[i] = std::sqrt(opParamsI[i].viscosity);
            cellVolO[cells.id(opParamsI[i].cellPos)] += opParamsI[i].massOfEmulsion / opParamsI[i].density;
        }
        for(int i=0; i<cellParamsBuf.get_count(); i++){
            windO[i] = cellParamsI[i].wind.getR();
        }
    }

    queue.submit([&](sycl::handler& cgh){
        auto opParamsIO = opParamsBuf.get_access<sycl::access::mode::read_write>(cgh);
        auto cellParamsI = cellParamsBuf.get_access<sycl::access::mode::read>(cgh);
        auto cellVolI = cellVolBuf.get_access<sycl::access::mode::read>(cgh);
        auto sqrtI = sqrtBuf.get_access<sycl::access::mode::read>(cgh);
        auto windI = windBuf.get_access<sycl::access::mode::read>(cgh);

        auto evaporatedRatio = [](const OilPoint::Params& op) -> double{
            return op.evaporatedMass / op.initialMassOfOilPoint;
        };

        int col = cells.getCol();
        auto id = [col](OilPoint::Params::CellPos pos) -> int {
            return pos.x * col + pos.y;
        };

        double oilWaterTension = config.oilWaterTension;
        double cellSize = config.cellSize;
        cgh.parallel_for<class DPUpdate>(sycl::range<1>(opParamsBuf.get_count()), [=](sycl::id<1> i){
            auto& op = opParamsIO[i];

            if(op.removed)
                return;

            double tension = oilWaterTension * (1 + evaporatedRatio(op));
            double Da = 0.11 * (windI[id(opParamsIO[i].cellPos)] + 1) * (windI[id(opParamsIO[i].cellPos)] + 1);
            double Db = 1 / (1 + 50 * sqrtI[i] * cellVolI[id(op.cellPos)]/(cellSize*cellSize) * 100 * tension);
            double mass = op.mass * Da * Db / 3600 * timestep;

            op.mass = op.mass - mass;
            op.dispersedMass = op.dispersedMass + mass;
            op.massOfEmulsion = op.massOfEmulsion - mass;
        });
    });
//    auto& cellParams = cells.getCellParams();
//    auto& opParams = cells.getOilPointsParams();
//
//    for (int i=0; i<cellParams.size(); i++) {
//        auto& cell = cellParams[i];
//        for (auto &op : opParams[i])  {
//            double tension = config.oilWaterTension * (1 + op.getEvaporatedRatio());
//            double Da = 0.11 * std::pow(cell.wind.getR() + 1, 2);
//            double Db = 1 / (1 + 50 * std::sqrt(op.viscosity) * cells.getThickness(i) * 100 * tension);
//            double mass = op.mass * Da * Db / 3600 * timestep;
//
//            op.mass = op.mass - mass;
//            op.dispersedMass = op.dispersedMass + mass;
//            op.massOfEmulsion = op.massOfEmulsion - mass;
//        }
//    }
}
