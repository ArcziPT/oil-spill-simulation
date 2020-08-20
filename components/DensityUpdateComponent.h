//
// Created by Mateusz Święszek on 17/08/2020.
//
#include "OilPointComponent.h"
#include "Configurations.h"
#include <iostream>

#ifndef OILSPILL_DENSITYUPDATECOMPONENT_H
#define OILSPILL_DENSITYUPDATECOMPONENT_H


class DensityUpdateComponent : public OilPointComponent {
private:

    static const long serialVersionUID = 7611614141616125901L;
    double densityAt15K;
    double salinity;
    std::shared_ptr<Configurations> config;

public:
    DensityUpdateComponent(std::shared_ptr<Configurations> config);
    void update(std::shared_ptr<Cell> cell, std::vector<OilPoint>::iterator it,const int &timestep) override;
    double &calculateWaterDensity(const double &tempAtk);

private:
    double &calculateDensity(const double &tempAtK);
};


#endif //OILSPILL_DENSITYUPDATECOMPONENT_H
