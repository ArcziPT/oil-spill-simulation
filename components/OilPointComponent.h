//
// Created by Mateusz Święszek on 11/08/2020.
//
#include <iostream>
#include <vector>
#include "core/OilPoint.h"
#include "core/Cell.h"

#ifndef OILSPILL_OILPOINTCOMPONENT_H
#define OILSPILL_OILPOINTCOMPONENT_H


class OilPointComponent {
public:
    virtual void update(Cell&cell, std::vector<OilPoint>::iterator it,const int &timestep);
};


#endif //OILSPILL_OILPOINTCOMPONENT_H
