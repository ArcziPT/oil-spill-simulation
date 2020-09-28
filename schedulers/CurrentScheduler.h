//
// Created by Mateusz Raczynski on 8/16/2020.
//

#ifndef OIL_SPILL_SIMULATION_CURRENTSCHEDULER_H
#define OIL_SPILL_SIMULATION_CURRENTSCHEDULER_H


#include "Scheduler.h"

class CurrentScheduler: public Scheduler
{
public:
    CurrentScheduler(std::shared_ptr<Sea> sea);
    void add(int iteration, GridValuesType<double> &array) override;
    void update(int iteration) override;
    UpdateSubject getUpdateSubject(int iteration) override;
};


#endif //OIL_SPILL_SIMULATION_CURRENTSCHEDULER_H
