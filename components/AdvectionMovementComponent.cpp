//
// Created by Mateusz Święszek on 11/08/2020.
//

#include "AdvectionMovementComponent.h"
#include "OilPointComponent.h"
#include "core/Vector2.h"
#include "core/OilPoint.h"

AdvectionMovementComponent::AdvectionMovementComponent(Configurations &config) : config(config) {
    this->cellSize = config.cellSize;
}

void AdvectionMovementComponent::update(sycl::queue& queue, CellGrid& cells,
                                        sycl::buffer<Cell::Params, 1>& cellParamsBuf,
                                        sycl::buffer<OilPoint::Params, 1>& opParamsBuf,
                                        sycl::buffer<OilComponent, 2>& opCompBuf,
                                        int timestep) {
//    for (auto &cell : cells.getOilPointsParams()) {
//        for (auto &op : cell) {
//            auto offsetX = op.velocity.x * timestep;
//            auto offsetY = op.velocity.x * timestep;
//
//            if (offsetX > cellSize || offsetY > cellSize) {
//                std::cout << "Za du�y krok!!!!!!" << std::endl;
//            }
//            op.position += Vector2(offsetX, offsetY);
//            op.velocity.zero();
//        }
//    }
}
