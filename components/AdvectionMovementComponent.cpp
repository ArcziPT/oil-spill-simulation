//
// Created by Mateusz Święszek on 11/08/2020.
//

#include "AdvectionMovementComponent.h"
#include "OilPointComponent.h"
#include "Vector2.h"

AdvectionMovementComponent::AdvectionMovementComponent(std::shared_ptr<Configurations> config) : config(config) {
    this->cellSize = config->cellSize;
    this->config = config;
}

void AdvectionMovementComponent::update(std::shared_ptr<Cell> cell, std::vector<OilPoint>::iterator it,
                                        const int &timestep) {
    OilPoint oilPoint = *(it + 1);
    Vector2 offset(timestep, timestep);


    offset = offset * oilPoint.velocity;

    if (offset.getX() > cellSize || offset.getY() > cellSize) {
        std::cout << "Za du�y krok!!!!!!" << std::endl;
    }

    oilPoint.position = oilPoint.position + offset;


    oilPoint.velocity.zero();
    *(it + 1) = oilPoint;

}