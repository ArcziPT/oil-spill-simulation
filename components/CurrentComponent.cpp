//
// Created by Mateusz Święszek on 16/08/2020.
//
#include "core/Cell.h"
#include "core/Configurations.h"
#include "core/OilPoint.h"
#include "core/Vector2.h"
#include "CurrentComponent.h"

CurrentComponent::CurrentComponent(std::shared_ptr<Configurations> config) {
    this->config = config;
};

void CurrentComponent::update(std::shared_ptr<Cell> cell, std::vector<OilPoint>::iterator it, const int &timestep) {
    it += 1;
    Vector2 velocityOfCurrent = cell->getCurrent();
//nie jestem pewien tego
    if (velocityOfCurrent.x != 0 && velocityOfCurrent.y != 0) {
        Vector2 tempVector2(velocityOfCurrent.x * currentParameter, (-velocityOfCurrent.y) * currentParameter);
        it->velocity = tempVector2;
    }
}

