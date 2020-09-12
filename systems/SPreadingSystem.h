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

    void update(int timestep) override;

private:
    void updatePair(Cell& cell1, Cell& cell2, int timestep, double volume);
    Vector2 determineVector2(Cell& cell1, Cell& cell2, double deltaMass);
    double calculateTotalVolume();

    double size;
    CellGrid &cells;
    Configurations &config;
    TimeCounter &timeSystem;
    constexpr static const double g = 9.81;
};

#endif //OILSPILL_SPREADINGSYSTEM_H
