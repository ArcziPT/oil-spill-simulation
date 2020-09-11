//
// Created by Mateusz Święszek on 09/09/2020.
//

#include "ViscosityUpdateComponent.h"
#include <cmath>

ViscosityUpdateComponent::ViscosityUpdateComponent(Configurations& config): config(config) {}

void ViscosityUpdateComponent::update(Cell& cell, std::vector<OilPoint>::iterator it,
                                      const int &timestep) {
    it += 1;
    double actualVis = it->viscosity;
    double deltaVis = config.viscosity * actualVis * it->lastDeltaF +
                      2.5 * actualVis * it->lastDeltaY /
                      std::pow(1 + config.viscosity * ((it->massOfEmulsion - it->mass) / it->massOfEmulsion), 2);
    it->viscosity = actualVis + deltaVis;
}
