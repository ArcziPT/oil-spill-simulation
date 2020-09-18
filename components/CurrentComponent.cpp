//
// Created by Mateusz Święszek on 16/08/2020.
//
#include "core/Cell.h"
#include "core/Configurations.h"
#include "core/OilPoint.h"
#include "core/Vector2.h"
#include "CurrentComponent.h"

CurrentComponent::CurrentComponent(Configurations& config): config(config) {}

void CurrentComponent::update(Cell& cell, OilPoint& op, const int &timestep) {
    Vector2 velocityOfCurrent = cell.current;
//nie jestem pewien tego
    if (velocityOfCurrent.x != 0 && velocityOfCurrent.y != 0) {
        op.velocity += Vector2(velocityOfCurrent.x * currentParameter, (-velocityOfCurrent.y) * currentParameter);
    }
}

