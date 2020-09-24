//
// Created by Mateusz Święszek on 11/08/2020.
//
#include <iostream>
#include <vector>
#include <SYCL/sycl.hpp>
using namespace cl;

#include "core/OilPoint.h"
#include "core/Cell.h"

#ifndef OILSPILL_OILPOINTCOMPONENT_H
#define OILSPILL_OILPOINTCOMPONENT_H


class OilPointComponent {
public:
    virtual void update(sycl::queue& queue, CellGrid& cells,
                        sycl::buffer<Cell::Params, 1>& cellParamsBuf,
                        sycl::buffer<OilPoint::Params, 1>& opParamsBuf,
                        sycl::buffer<OilComponent, 2>& opCompBuf,
                        int timestep) = 0;
};


#endif //OILSPILL_OILPOINTCOMPONENT_H
