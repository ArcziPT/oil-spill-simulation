//
// Created by Mateusz Święszek on 25/08/2020.
//
#include "core/Configurations.h"
#include "core/Cell.h"
#include "core/OilPoint.h"
#include "OilPointComponent.h"
#ifndef OILSPILL_EMULSIFICATIONCOMPONENT_H
#define OILSPILL_EMULSIFICATIONCOMPONENT_H


class EmulsificationComponent: public OilPointComponent {
private:
    static const long serialVersionUID = 8902362502821648198L;
    std::shared_ptr<Configurations> config;

public:
    EmulsificationComponent(std::shared_ptr<Configurations> config);
    void update(std::shared_ptr<Cell> cell, std::vector<OilPoint>::iterator it,const int &timestep) override;

};


#endif //OILSPILL_EMULSIFICATIONCOMPONENT_H
