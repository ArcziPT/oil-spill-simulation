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

    void update(int timestep) override;

private:
    Configurations& config;
    CellGrid& cells;

    bool update(Cell& cell, int i);
};


#endif //OILSPILL_CHANGESQUARESYSTEM_H
