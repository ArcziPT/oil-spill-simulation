//
// Created by Mateusz Święszek on 18/08/2020.
//
#include "core/Configurations.h"
#include "OilPointComponent.h"
#include <cmath>

#ifndef OILSPILL_DISPERSIONCOMPONENT_H
#define OILSPILL_DISPERSIONCOMPONENT_H


class DispersionComponent : OilPointComponent {
private:
    static const long serialVersionUID = -246278455590877923L;
    std::shared_ptr<Configurations> config;
public:
    DispersionComponent(std::shared_ptr<Configurations> config);

    void update(std::shared_ptr<Cell> cell, std::vector<OilPoint>::iterator it, const int &timestep) override;
};


#endif //OILSPILL_DISPERSIONCOMPONENT_H
