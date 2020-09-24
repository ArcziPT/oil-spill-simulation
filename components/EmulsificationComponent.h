//
// Created by Mateusz Święszek on 25/08/2020.
//
#include "core/Configurations.h"
#include "core/Cell.h"
#include "core/OilPoint.h"
#include "OilPointComponent.h"
#ifndef OILSPILL_EMULSIFICATIONCOMPONENT_H
#define OILSPILL_EMULSIFICATIONCOMPONENT_H


class EmulsificationComponent: public OilPointComponent {
private:
    static const long serialVersionUID = 8902362502821648198L;
    Configurations& config;

public:
    EmulsificationComponent(Configurations& config);
    void update(sycl::queue& queue, CellGrid& cells,
                sycl::buffer<Cell::Params, 1>& cellParamsBuf,
                sycl::buffer<OilPoint::Params, 1>& opParamsBuf,
                sycl::buffer<OilComponent, 2>& opCompBuf,
                int timestep) override;

};


#endif //OILSPILL_EMULSIFICATIONCOMPONENT_H
