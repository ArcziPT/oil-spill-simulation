//
// Created by arczipt on 23.08.2020.
//

#ifndef OILSPILL_OILPOINTCOMPONENTSYSTEM_H
#define OILSPILL_OILPOINTCOMPONENTSYSTEM_H


#include <core/Sea.h>
#include "OilSystem.h"

class OilPointComponentsSystem: public OilSystem {
public:
    OilPointComponentsSystem(Sea& sea, Configurations& config);

    void update(int timestep) override;

private:
    std::vector<OilPointComponent> oilComponents{};
    CellGrid cells;
}


#endif //OILSPILL_OILPOINTCOMPONENTSYSTEM_H
