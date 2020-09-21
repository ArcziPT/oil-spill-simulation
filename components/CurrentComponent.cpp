//
// Created by Mateusz Święszek on 16/08/2020.
//
#include "core/Cell.h"
#include "core/Configurations.h"
#include "core/OilPoint.h"
#include "core/Vector2.h"
#include "CurrentComponent.h"

CurrentComponent::CurrentComponent(Configurations &config) : config(config) {}

void CurrentComponent::update(CellGrid &cells, int timestep) {
    auto& cellParams = cells.getCellParams();
    auto& opParams = cells.getOilPointsParams();

    for (int i=0; i<cellParams.size(); i++) {
        auto& cell = cellParams[i];
        for (auto &op : opParams[i].oilPointsParams) {
            Vector2 velocityOfCurrent = cell.current;
            //nie jestem pewien tego
            if (velocityOfCurrent.x != 0 && velocityOfCurrent.y != 0) {
                op.velocity += Vector2(velocityOfCurrent.x * currentParameter,
                                       (-velocityOfCurrent.y) * currentParameter);
            }
        }
    }
}

