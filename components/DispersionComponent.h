//
// Created by Mateusz Święszek on 18/08/2020.
//
#include "core/Configurations.h"
#include "OilPointComponent.h"
#include <cmath>

#ifndef OILSPILL_DISPERSIONCOMPONENT_H
#define OILSPILL_DISPERSIONCOMPONENT_H


class DispersionComponent : public OilPointComponent {
private:
    static const long serialVersionUID = -246278455590877923L;
    Configurations& config;
public:
    DispersionComponent(Configurations& config);

    void update(CellGrid& cells, int timestep) override;
};


#endif //OILSPILL_DISPERSIONCOMPONENT_H
