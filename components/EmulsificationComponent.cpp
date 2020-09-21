//
// Created by Mateusz Święszek on 25/08/2020.
//

#include "EmulsificationComponent.h"

EmulsificationComponent::EmulsificationComponent(Configurations &config) : config(config) {}

void EmulsificationComponent::update(CellGrid &cells, int timestep) {
    for (auto &cell : cells) {
        for (auto &op : cell.oilPoints) {
            double windSpeed = cell.wind.len();
            double Y = op.getEmulsification();
            double deltaY = 2E-6 * (windSpeed + 1) * (windSpeed + 1) * (1 - Y / config.mousseViscosity)
                            * timestep;

            op.massOfEmulsion = op.mass / (1 - (Y + deltaY));
            op.lastDeltaY = deltaY;
        }
    }
}
