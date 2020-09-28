//
// Created by Mateusz Raczynski on 8/16/2020.
//

#ifndef OILSPILL_TEMPERATURESCHEDULER_H
#define OILSPILL_TEMPERATURESCHEDULER_H


#include "Scheduler.h"

class TemperatureScheduler: public Scheduler
{
public:
    TemperatureScheduler(std::shared_ptr<Sea>sea);
    void add(int iteration, GridValuesType<double>& array) override;
    void update(int iteration) override;
    UpdateSubject getUpdateSubject(int iteration) override;
};


#endif //OILSPILL_TEMPERATURESCHEDULER_H
