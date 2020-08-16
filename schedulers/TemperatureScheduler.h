//
// Created by Mateusz Raczynski on 8/16/2020.
//

#ifndef OILSPILL_TEMPERATURESCHEDULER_H
#define OILSPILL_TEMPERATURESCHEDULER_H


#include "Scheduler.h"

class TemperatureScheduler: public Scheduler
{
public:
    void add(int iteration, GridValuesType &array) override;
    void update(int iteration) override;
};


#endif //OILSPILL_TEMPERATURESCHEDULER_H
