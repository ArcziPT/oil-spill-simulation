//
// Created by Mateusz Święszek on 17/08/2020.
//
#include "OilPointComponent.h"
#include "core/Configurations.h"
#include <iostream>

#ifndef OILSPILL_DENSITYUPDATECOMPONENT_H
#define OILSPILL_DENSITYUPDATECOMPONENT_H


class DensityUpdateComponent : public OilPointComponent {
private:

    static const long serialVersionUID = 7611614141616125901L;
    double densityAt15K;
    double salinity;
    Configurations& config;

public:
    DensityUpdateComponent(Configurations& config);
    void update(Cell& cell, std::vector<OilPoint>::iterator it,const int &timestep) override;
    double calculateWaterDensity(const double &tempAtk);

private:
    double calculateDensity(const double &tempAtK);
};


#endif //OILSPILL_DENSITYUPDATECOMPONENT_H
