//
// Created by Mateusz Święszek on 09/09/2020.
//
#include "core/Configurations.h"
#include "components//OilPointComponent.h"
#ifndef OILSPILL_VISCOSITYUPDATECOMPONENT_H
#define OILSPILL_VISCOSITYUPDATECOMPONENT_H


class ViscosityUpdateComponent: public OilPointComponent {
private:
    static const long serialVersionUID = 6301560017261177982L;
    Configurations& config;
public:
    ViscosityUpdateComponent(Configurations& config);
    void update(sycl::queue& queue, CellGrid& cells,
                sycl::buffer<Cell::Params, 1>& cellParamsBuf,
                sycl::buffer<OilPoint::Params, 1>& opParamsBuf,
                sycl::buffer<OilComponent, 2>& opCompBuf,
                int timestep) override;

};


#endif //OILSPILL_VISCOSITYUPDATECOMPONENT_H
