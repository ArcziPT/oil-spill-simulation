//
// Created by Mateusz Święszek on 18/08/2020.
//

#include "DispersionComponent.h"


DispersionComponent::DispersionComponent(Configurations &config) : config(config) {}

void DispersionComponent::update(CellGrid &cells, int timestep) {
    auto& cellParams = cells.getCellParams();
    auto& opParams = cells.getOilPointsParams();

    for (int i=0; i<cellParams.size(); i++) {
        auto& cell = cellParams[i];
        for (auto &op : opParams[i].oilPointsParams)  {
            double tension = config.oilWaterTension * (1 + op.getEvaporatedRatio());
            double Da = 0.11 * std::pow(cell.wind.getR() + 1, 2);
            double Db = 1 / (1 + 50 * std::sqrt(op.viscosity) * cells.getThickness(i) * 100 * tension);
            double mass = op.mass * Da * Db / 3600 * timestep;

            op.mass = op.mass - mass;
            op.dispersedMass = op.dispersedMass + mass;
            op.massOfEmulsion = op.massOfEmulsion - mass;
        }
    }
}
