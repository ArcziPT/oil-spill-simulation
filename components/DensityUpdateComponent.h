//
// Created by Mateusz Święszek on 17/08/2020.
//
#include "OilPointComponent.h"
#include "core/Configurations.h"
#include <iostream>

#ifndef OILSPILL_DENSITYUPDATECOMPONENT_H
#define OILSPILL_DENSITYUPDATECOMPONENT_H


class DensityUpdateComponent : public OilPointComponent {
private:
    double densityAt15K;
    double salinity;
    Configurations& config;

public:
    DensityUpdateComponent(Configurations& config);
    void update(sycl::queue& queue, CellGrid& cells,
                sycl::buffer<Cell::Params, 1>& cellParamsBuf,
                sycl::buffer<OilPoint::Params, 1>& opParamsBuf,
                sycl::buffer<OilComponent, 2>& opCompBuf,
                int timestep) override;
    double calculateWaterDensity(double tempAtk);

private:
    double calculateDensity(double tempAtK);
};


#endif //OILSPILL_DENSITYUPDATECOMPONENT_H
