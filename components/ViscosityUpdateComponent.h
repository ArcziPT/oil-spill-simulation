//
// Created by Mateusz Święszek on 09/09/2020.
//
#include "core/Configurations.h"
#include "components//OilPointComponent.h"
#ifndef OILSPILL_VISCOSITYUPDATECOMPONENT_H
#define OILSPILL_VISCOSITYUPDATECOMPONENT_H


class ViscosityUpdateComponent: public OilPointComponent {
private:
    static const long serialVersionUID = 6301560017261177982L;
    Configurations config;
public:
    ViscosityUpdateComponent(Configurations config);
    void update(std::shared_ptr<Cell> cell, std::vector<OilPoint>::iterator it,const int &timestep) override;

};


#endif //OILSPILL_VISCOSITYUPDATECOMPONENT_H
