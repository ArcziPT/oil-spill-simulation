//
// Created by Mateusz Święszek on 25/08/2020.
//

#include "EmulsificationComponent.h"

EmulsificationComponent::EmulsificationComponent(Configurations &config) : config(config) {}

void EmulsificationComponent::update(CellGrid &cells, int timestep) {
    auto& cellParams = cells.getCellParams();
    auto& opParams = cells.getOilPointsParams();

    for (int i=0; i<cellParams.size(); i++) {
        auto& cell = cellParams[i];
        for (auto &op : opParams[i]){
            double windSpeed = cell.wind.len();
            double Y = op.getEmulsification();
            double deltaY = 2E-6 * (windSpeed + 1) * (windSpeed + 1) * (1 - Y / config.mousseViscosity)
                            * timestep;

            op.massOfEmulsion = op.mass / (1 - (Y + deltaY));
            op.lastDeltaY = deltaY;
        }
    }
}
