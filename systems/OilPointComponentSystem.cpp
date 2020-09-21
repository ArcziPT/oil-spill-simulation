//
// Created by arczipt on 23.08.2020.
//

#include "OilPointComponentSystem.h"

#include "components/EvaporationComponent.h"
#include "components/EmulsificationComponent.h"
#include "components/DispersionComponent.h"
#include "components/DensityUpdateComponent.h"
#include "components/ViscosityUpdateComponent.h"

OilPointComponentsSystem::OilPointComponentsSystem(std::shared_ptr<Sea> sea, Configurations &config) : cells(
        sea->getCells()) {
    oilComponents.push_back(std::unique_ptr<OilPointComponent>(new EvaporationComponent(config)));
    oilComponents.push_back(std::unique_ptr<OilPointComponent>(new EmulsificationComponent(config)));
    oilComponents.push_back(std::unique_ptr<OilPointComponent>(new DispersionComponent(config)));
    oilComponents.push_back(std::unique_ptr<OilPointComponent>(new DensityUpdateComponent(config)));
    oilComponents.push_back(std::unique_ptr<OilPointComponent>(new ViscosityUpdateComponent(config)));
}

//TODO
void OilPointComponentsSystem::update(int timestep) {
    for (auto &com : oilComponents) {
        com->update(cells, timestep);

        //some oil points might be removed by component
        for(auto& cell : cells){
            std::vector<OilPoint> oilPoints{};
            for (auto &op : cell.oilPoints) {
                if (!op.removed)
                    oilPoints.push_back(op);
            }
            cell.oilPoints = std::move(oilPoints);
        }
    }
}
