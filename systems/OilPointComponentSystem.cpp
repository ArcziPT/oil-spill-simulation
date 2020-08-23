//
// Created by arczipt on 23.08.2020.
//

#include "OilPointComponentSystem.h"

OilPointComponentsSystem::OilPointComponentsSystem(Sea &sea, Configurations &config) {
    oilcomponents.push_back(EvaporationComponent(config));
    oilcomponents.push_back(EmulsificationComponent(config));
    oilcomponents.push_back(DispersionComponent(config));
    oilcomponents.push_back(DensityUpdateComponent(config));
    oilcomponents.push_back(ViscosityUpdateComponent(config));

    cells = sea.getCells();
}

//TODO
void OilPointComponentsSystem::update(int timestep) {
    int rows = cells.size();
    int cols = cells[0].size();

    for (auto& com : oilcomponents) {
        for (int i = 1; i < rows - 1; i++) {
            for (int j = 1; j < cols - 1; j++) {
                auto& cell = cells[i][j];
                for (auto& op : cell.oilPoints) {
                    com.update(cell, op, timestep);
                }
            }
        }
    }
}
