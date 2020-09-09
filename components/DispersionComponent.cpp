//
// Created by Mateusz Święszek on 18/08/2020.
//

#include "DispersionComponent.h"



DispersionComponent::DispersionComponent(std::shared_ptr<Configurations> config) {
    this->config = config;
};

void DispersionComponent::update(std::shared_ptr<Cell> cell, std::vector<OilPoint>::iterator it, const int &timestep) {
    it += 1;
    double tension = config->oilWaterTension * (1 + it->getEvaporatedRatio());
    double Da = 0.11 * std::pow(cell->getWind().getR()+1,2);
    double Db = 1 / (1 + 50 * std::sqrt(it->viscosity) * cell->getThickness() * 100 * tension);
    double mass = it->mass * Da * Db / 3600 * timestep;

    it->mass = it->mass * Da * Db / 3600 * timestep;
    it->dispersedMass = it->dispersedMass + mass;
    it->massOfEmulsion = it->massOfEmulsion - mass;
}