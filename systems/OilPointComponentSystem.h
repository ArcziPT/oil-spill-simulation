//
// Created by arczipt on 23.08.2020.
//

#ifndef OILSPILL_OILPOINTCOMPONENTSYSTEM_H
#define OILSPILL_OILPOINTCOMPONENTSYSTEM_H


#include <core/Sea.h>
#include "OilSystem.h"
#include "components/OilPointComponent.h"

class OilPointComponentsSystem : public OilSystem {
public:
    OilPointComponentsSystem(std::shared_ptr<Sea> sea, Configurations& config);

    void update(int timestep) override;

private:
    std::vector<std::unique_ptr<OilPointComponent>> oilComponents{};
    CellGrid &cells;
};


#endif //OILSPILL_OILPOINTCOMPONENTSYSTEM_H
