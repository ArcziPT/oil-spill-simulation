//
// Created by Mateusz Raczynski on 8/16/2020.
//

#ifndef OILSPILL_OILSCHEDULER_H
#define OILSPILL_OILSCHEDULER_H


#include "Scheduler.h"

class OilScheduler: public Scheduler
{
public:
    OilScheduler(SeaPtr sea);
    void update(int iteration) override;
    void add(int iteration, GridValuesType &array) override;
};


#endif //OILSPILL_OILSCHEDULER_H
