//
// Created by Mateusz Raczynski on 8/16/2020.
//

#ifndef OIL_SPILL_SIMULATION_CURRENTSCHEDULER_H
#define OIL_SPILL_SIMULATION_CURRENTSCHEDULER_H


#include "Scheduler.h"

class CurrentScheduler: public Scheduler
{
public:
    CurrentScheduler(Sea sea);
    void add(int iteration, GridType &array) override;
    void update(int iteration) override;
};


#endif //OIL_SPILL_SIMULATION_CURRENTSCHEDULER_H
