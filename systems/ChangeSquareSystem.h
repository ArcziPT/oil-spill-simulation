//
// Created by arczipt on 17.08.2020.
//

#ifndef OILSPILL_CHANGESQUARESYSTEM_H
#define OILSPILL_CHANGESQUARESYSTEM_H

#include <core/Cell.h>
#include "OilSystem.h"

class ChangeSquareSystem: public OilSystem{
public:
    ChangeSquareSystem(CellGrid& cells, Configurations& config): cells(cells), config(config) {}

    void update(sycl::queue& queue,
                sycl::buffer<Cell::Params, 1>& cellParamsBuf,
                sycl::buffer<OilPoint::Params, 1>& opParamsBuf,
                sycl::buffer<OilComponent, 2>& opCompBuf,
                int timestep) override;

private:
    Configurations& config;
    CellGrid& cells;

    bool update(int x, int y, int i);
};


#endif //OILSPILL_CHANGESQUARESYSTEM_H
