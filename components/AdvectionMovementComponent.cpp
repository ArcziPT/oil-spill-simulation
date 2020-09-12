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

void AdvectionMovementComponent::update(Cell& cell, std::vector<OilPoint>::iterator it,
                                        const int &timestep) {
    it += 1;
    it->velocity *= timestep;

    if (it->velocity.x > cellSize || it->velocity.y > cellSize) {
        std::cout << "Za du�y krok!!!!!!" << std::endl;
    }
    it->position += it->velocity;
    it->velocity.zero();

}
