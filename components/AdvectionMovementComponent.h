//
// Created by Mateusz Święszek on 11/08/2020.
//
#include "OilPointComponent.h"
#include "core/Configurations.h"
#include "core/Cell.h"

#ifndef OILSPILL_ADVECTIONMOVEMENTCOMPONENT_H
#define OILSPILL_ADVECTIONMOVEMENTCOMPONENT_H

class AdvectionMovementComponent: public OilPointComponent {
private:
    static const long serialVersionUID = -1653011884245706960L;
    double cellSize;
    Configurations& config;

public:
    AdvectionMovementComponent(Configurations& config);
    void update(Cell& cell, OilPoint& op, const int &timestep) override;


};


#endif //OILSPILL_ADVECTIONMOVEMENTCOMPONENT_H
