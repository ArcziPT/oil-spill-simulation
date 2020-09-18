//
// Created by Mateusz Święszek on 11/08/2020.
//

#include "AdvectionMovementComponent.h"
#include "OilPointComponent.h"
#include "core/Vector2.h"
#include "core/OilPoint.h"

AdvectionMovementComponent::AdvectionMovementComponent(Configurations& config): config(config) {
    this->cellSize = config.cellSize;
}

void AdvectionMovementComponent::update(Cell& cell, OilPoint& op, const int &timestep) {
    auto offsetX = op.velocity.x * timestep;
    auto offsetY = op.velocity.x * timestep;

    if (offsetX > cellSize || offsetY > cellSize) {
        std::cout << "Za du�y krok!!!!!!" << std::endl;
    }
    op.position += Vector2(offsetX, offsetY);
    op.velocity.zero();
}
