//
// Created by Mateusz Święszek on 18/08/2020.
//

#include "DispersionComponent.h"



DispersionComponent::DispersionComponent(Configurations& config): config(config) {}

void DispersionComponent::update(Cell& cell, OilPoint& op, const int &timestep) {
    double tension = config.oilWaterTension * (1 + op.getEvaporatedRatio());
    double Da = 0.11 * std::pow(cell.wind.getR()+1,2);
    double Db = 1 / (1 + 50 * std::sqrt(op.viscosity) * cell.getThickness() * 100 * tension);
    double mass = op.mass * Da * Db / 3600 * timestep;

    op.mass = op.mass - mass;
    op.dispersedMass = op.dispersedMass + mass;
    op.massOfEmulsion = op.massOfEmulsion - mass;
}
