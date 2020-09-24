//
// Created by arczipt on 23.08.2020.
//

#ifndef OILSPILL_OILSYSTEM_H
#define OILSPILL_OILSYSTEM_H

#include <CL/sycl.hpp>
#include "core/OilComponent.h"
#include "core/Cell.h"
using namespace cl;

class OilSystem{
public:
    virtual void update(sycl::queue& queue,
                        sycl::buffer<Cell::Params, 1>& cellParamsBuf,
                        sycl::buffer<OilPoint::Params, 1>& opParamsBuf,
                        sycl::buffer<OilComponent, 2>& opCompBuf,
                        int timestep) = 0;
};

#endif //OILSPILL_OILSYSTEM_H
