//
// Created by arczipt on 23.08.2020.
//

#include "OilPointComponentSystem.h"

#include "components/EvaporationComponent.h"
#include "components/EmulsificationComponent.h"
#include "components/DispersionComponent.h"
#include "components/DensityUpdateComponent.h"
#include "components/ViscosityUpdateComponent.h"

OilPointComponentsSystem::OilPointComponentsSystem(std::shared_ptr<Sea> sea, Configurations &config): cells(sea->getCells()) {
    oilComponents.push_back(std::unique_ptr<OilPointComponent>(new EvaporationComponent(config)));
    oilComponents.push_back(std::unique_ptr<OilPointComponent>(new EmulsificationComponent(config)));
    oilComponents.push_back(std::unique_ptr<OilPointComponent>(new DispersionComponent(config)));
    oilComponents.push_back(std::unique_ptr<OilPointComponent>(new DensityUpdateComponent(config)));
    oilComponents.push_back(std::unique_ptr<OilPointComponent>(new ViscosityUpdateComponent(config)));
}

//TODO
void OilPointComponentsSystem::update(int timestep) {
    int rows = cells.size();
    int cols = cells[0].size();

    for (auto& com : oilComponents) {
        for (int i = 1; i < rows - 1; i++) {
            for (int j = 1; j < cols - 1; j++) {
                auto& cell = cells[i][j];
                for (auto it = cell.oilPoints.begin(); it != cell.oilPoints.end(); it++) {
                    com->update(cell, it, timestep);
                }
            }
        }
    }
}
