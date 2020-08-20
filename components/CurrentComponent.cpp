//
// Created by Mateusz Święszek on 16/08/2020.
//
#include "Cell.h"
#include "Configurations.h"
#include "OilPoint.h"
#include "Vector2.h"
#include "CurrentComponent.h"

CurrentComponent::CurrentComponent(std::shared_ptr<Configurations> config){
    this->config = config;
};

void CurrentComponent::update(std::shared_ptr<Cell> cell, std::vector<OilPoint>::iterator it, const int &timestep) {

    Vector2 velocityOfCurrent = cell->getCurrent();

    if (velocityOfCurrent != NULL) {
        Vector2 tempVector2(velocityOfCurrent.getX() * currentParameter,(-velocityOfCurrent.getY())*currentParameter);
        velocityOfCurrent +=tempVector2;
        *(it + 1)->setCurrent(velocityOfCurrent);
    }
}

