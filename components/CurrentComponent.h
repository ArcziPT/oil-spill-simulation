//
// Created by Mateusz Święszek on 16/08/2020.
//
#include "core/Cell.h"
#include "core/Configurations.h"
#include "OilPointComponent.h"
#include "core/Vector2.h"

#ifndef OILSPILL_CURRENTCOMPONENT_H
#define OILSPILL_CURRENTCOMPONENT_H


class CurrentComponent: public OilPointComponent {

private:
    static const long serialVersionUID = -1447627012567202701L;
    double currentParameter = 1.1;
    Configurations& config;
public:
    CurrentComponent(Configurations& config);
    void update(sycl::queue& queue, CellGrid& cells,
                sycl::buffer<Cell::Params, 1>& cellParamsBuf,
                sycl::buffer<OilPoint::Params, 1>& opParamsBuf,
                sycl::buffer<OilComponent, 2>& opCompBuf,
                int timestep) override;
};


#endif //OILSPILL_CURRENTCOMPONENT_H
