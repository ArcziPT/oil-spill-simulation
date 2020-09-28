//
// Created by Mateusz Raczynski on 8/16/2020.
//

#ifndef OILSPILL_OILSCHEDULER_H
#define OILSPILL_OILSCHEDULER_H


#include "Scheduler.h"

class OilScheduler: public Scheduler
{
public:
    OilScheduler(std::shared_ptr<Sea> sea);
    void update(int iteration) override;
    void add(int iteration, GridValuesType<double> &array) override;
    UpdateSubject getUpdateSubject(int iteration) override;
};


#endif //OILSPILL_OILSCHEDULER_H
