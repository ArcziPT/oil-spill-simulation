//
// Created by Mateusz Raczynski on 8/16/2020.
//

#ifndef OILSPILL_SCHEDULERSCONTROLLER_H
#define OILSPILL_SCHEDULERSCONTROLLER_H


#include <unordered_map>
#include <memory>
#include <core/Sea.h>
#include <core/EditItem.h>
#include "Scheduler.h"

typedef std::unordered_map<EditItem, std::unique_ptr<Scheduler>> SchedulerMap;

class SchedulersController
{
private:
    SeaPtr sea;
    std::unordered_map<EditItem, std::unique_ptr<Scheduler>> schedulersMap;

public:
    SchedulersController(SeaPtr sea);
    SchedulerMap& getSchedulersMap();
    void update(int iteration);
private:
    void initializeSchedulersMap();
    
};


#endif //OILSPILL_SCHEDULERSCONTROLLER_H
