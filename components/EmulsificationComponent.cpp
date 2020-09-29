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
    queue.submit([&](sycl::handler &cgh) {
        auto opParamsIO = opParamsBuf.get_access<sycl::access::mode::read_write>(cgh);
        auto cellParamsI = cellParamsBuf.get_access<sycl::access::mode::read>(cgh);

        auto emulsification = [](const OilPoint::Params &op) -> double {
            return (op.massOfEmulsion - op.mass) / op.massOfEmulsion;
        };
        int col = cells.getCol();
        auto id = [col](OilPoint::Params::CellPos pos) -> int {
            return pos.x * col + pos.y;
        };

        auto getLen = [](const Vector2 &v) -> double {
            return sycl::sqrt(v.x * v.x + v.y * v.y);
        };

        auto mousseViscosity = config.mousseViscosity;
        cgh.parallel_for<class EmuCUpdate>(sycl::range<1>(opParamsBuf.get_count()), [=](sycl::id<1> i) {
            auto &op = opParamsIO[i];

            if (op.removed)
                return;

            double windSpeed = getLen(cellParamsI[id(op.cellPos)].wind);
            double Y = emulsification(op);
            double deltaY = 2E-6 * (windSpeed + 1) * (windSpeed + 1) * (1 - Y / mousseViscosity)
                            * timestep;

            op.massOfEmulsion = op.mass / (1 - (Y + deltaY));
            op.lastDeltaY = deltaY;
        });
    });
}
