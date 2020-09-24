//
// Created by arczipt on 23.08.2020.
//

#ifndef OILSPILL_REENTAIREDSYSTEM_H
#define OILSPILL_REENTAIREDSYSTEM_H


#include "OilSystem.h"
#include "ChangeSquareSystem.h"

class ReEntairedSystem: public OilSystem {
public:

    ReEntairedSystem(CellGrid& cells, Configurations& config): cells(cells), config(config) {}
    void update(sycl::queue& queue,
                sycl::buffer<Cell::Params, 1>& cellParamsBuf,
                sycl::buffer<OilPoint::Params, 1>& opParamsBuf,
                sycl::buffer<OilComponent, 2>& opCompBuf,
                int timestep) override;

private:
    CellGrid &cells;
    Configurations& config;

    void updateCell(int x, int y, int timestep);
};


#endif //OILSPILL_REENTAIREDSYSTEM_H
