//
// Created by Mateusz Święszek on 26/08/2020.
//
#include "core/Configurations.h"
#include "OilPointComponent.h"
#include "core/Cell.h"
#ifndef OILSPILL_EVAPORATIONCOMPONENT_H
#define OILSPILL_EVAPORATIONCOMPONENT_H


class EvaporationComponent: public OilPointComponent {
private:
    constexpr static const double R = 8.31;
    constexpr static const double K = 1.25E-3;
    static const long serialVersionUID = -6961288679362087433L;
    Configurations& config;
public:
    EvaporationComponent(Configurations& config);
    void update(Cell& cell, OilPoint& op, const int &timestep) override;

};


#endif //OILSPILL_EVAPORATIONCOMPONENT_H
