//
// Created by Mateusz Święszek on 25/08/2020.
//

#include "EmulsificationComponent.h"

EmulsificationComponent::EmulsificationComponent(std::shared_ptr<Configurations> config) {
    this->config = config;
}

void EmulsificationComponent::update(std::shared_ptr<Cell> cell, std::vector<OilPoint>::iterator it,
                                     const int &timestep) {
    it += 1;
    double windSpeed = cell->getWind().len();
    double Y = (it->massOfEmulsion - it->mass) / it->massOfEmulsion; //getEmulsification
    double deltaY = 2E-6 * (windSpeed + 1) * (windSpeed + 1) * (1 - Y / config->mousseViscosity)
                    * timestep;

    it->massOfEmulsion = it->mass / (1 - (Y + deltaY));
    it->lastDeltaY = deltaY;

}