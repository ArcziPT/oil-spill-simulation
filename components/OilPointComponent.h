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
    virtual void update(CellGrid& cells, int timestep) = 0;
};


#endif //OILSPILL_OILPOINTCOMPONENT_H
