//
// Created by Mateusz Święszek on 11/08/2020.
//
#include <iostream>
#include <vector>
#include "core/OilPoint.h"

#ifndef OILSPILL_OILPOINTCOMPONENT_H
#define OILSPILL_OILPOINTCOMPONENT_H


class OilPointComponent {
public:
    virtual void update(std::shared_ptr<Cell> cell, std::vector<OilPoint>::iterator it,const int &timestep);
};


#endif //OILSPILL_OILPOINTCOMPONENT_H
