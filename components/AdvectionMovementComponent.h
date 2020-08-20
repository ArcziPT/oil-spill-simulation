//
// Created by Mateusz Święszek on 11/08/2020.
//
#include "OilPointComponent.h"

#ifndef OILSPILL_ADVECTIONMOVEMENTCOMPONENT_H
#define OILSPILL_ADVECTIONMOVEMENTCOMPONENT_H

class AdvectionMovementComponent: public OilPointComponent {
private:
    static const long serialVersionUID = -1653011884245706960L;
    double cellSize;
    Configurations config;

public:
    AdvectionMovementComponent(std::shared_ptr<Configurations> config);
    void update(std::shared_ptr<Cell> cell, std::vector<OilPoint>::iterator it,const int &timestep) override;


};


#endif //OILSPILL_ADVECTIONMOVEMENTCOMPONENT_H
