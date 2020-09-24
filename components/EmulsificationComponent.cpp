//
// Created by Mateusz Święszek on 25/08/2020.
//

#include "EmulsificationComponent.h"

EmulsificationComponent::EmulsificationComponent(Configurations &config) : config(config) {}

void EmulsificationComponent::update(sycl::queue& queue, CellGrid& cells,
                                     sycl::buffer<Cell::Params, 1>& cellParamsBuf,
                                     sycl::buffer<OilPoint::Params, 1>& opParamsBuf,
                                     sycl::buffer<OilComponent, 2>& opCompBuf,
                                     int timestep) {
    sycl::buffer<double, 1> windBuf(sycl::range<1>(cellParamsBuf.get_count()));
    {
        auto windO = windBuf.get_access<sycl::access::mode::write>();
        auto cellParamsI = cellParamsBuf.get_access<sycl::access::mode::read>();

        for(int i=0; i<cellParamsBuf.get_count(); i++){
            windO[i] = cellParamsI[i].wind.len();
        }
    }

    queue.submit([&](sycl::handler& cgh) {
        auto opParamsIO = opParamsBuf.get_access<sycl::access::mode::read_write>(cgh);
        auto windI = windBuf.get_access<sycl::access::mode::read>(cgh);

        auto emulsification = [](const OilPoint::Params& op) -> double{
            return (op.massOfEmulsion - op.mass) / op.massOfEmulsion;
        };
        int col = cells.getCol();
        auto id = [col](OilPoint::Params::CellPos pos) -> int{
            return pos.x * col + pos.y;
        };

        auto mousseViscosity = config.mousseViscosity;
        cgh.parallel_for<class EmuCUpdate>(sycl::range<1>(opParamsBuf.get_count()), [=](sycl::id<1> i){
            auto& op = opParamsIO[i];

            if(op.removed)
                return;

            double windSpeed = windI[id(op.cellPos)];
            double Y = emulsification(op);
            double deltaY = 2E-6 * (windSpeed + 1) * (windSpeed + 1) * (1 - Y / mousseViscosity)
                            * timestep;

            op.massOfEmulsion = op.mass / (1 - (Y + deltaY));
            op.lastDeltaY = deltaY;
        });
    });
//    auto& cellParams = cells.getCellParams();
//    auto& opParams = cells.getOilPointsParams();
//
//    for (int i=0; i<cellParams.size(); i++) {
//        auto& cell = cellParams[i];
//        for (auto &op : opParams[i]){
//            double windSpeed = cell.wind.len();
//            double Y = op.getEmulsification();
//            double deltaY = 2E-6 * (windSpeed + 1) * (windSpeed + 1) * (1 - Y / config.mousseViscosity)
//                            * timestep;
//
//            op.massOfEmulsion = op.mass / (1 - (Y + deltaY));
//            op.lastDeltaY = deltaY;
//        }
//    }
}
