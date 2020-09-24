//
// Created by arczipt on 24.08.2020.
//

#ifndef OILSPILL_SPREADINGSYSTEM_H
#define OILSPILL_SPREADINGSYSTEM_H

#include <core/Configurations.h>
#include <cmath>
#include <random>
#include "OilSystem.h"
#include "TimeCounter.h"
#include "ChangeSquareSystem.h"

class SpreadingSystem : public OilSystem {
public:
    SpreadingSystem(CellGrid &cells, Configurations &config, TimeCounter &timeSystem) : size(config.cellSize),
                                                                                                    cells(cells),
                                                                                                    config(config),
                                                                                                    timeSystem(timeSystem) {}

    void update(sycl::queue& queue,
                sycl::buffer<Cell::Params, 1>& cellParamsBuf,
                sycl::buffer<OilPoint::Params, 1>& opParamsBuf,
                sycl::buffer<OilComponent, 2>& opCompBuf,
                int timestep) override;

private:
    void updatePair(int x1, int y1, int x2, int y2, int timestep, double volume);
    Vector2 determineVector2(int x1, int y1, int x2, int y2, double deltaMass);
    double calculateTotalVolume();

    double size;
    CellGrid &cells;
    Configurations &config;
    TimeCounter &timeSystem;
    constexpr static const double g = 9.81;
};

#endif //OILSPILL_SPREADINGSYSTEM_H
