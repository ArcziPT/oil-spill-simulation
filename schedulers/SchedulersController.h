//
// Created by Mateusz Raczynski on 8/16/2020.
//

#ifndef OILSPILL_SCHEDULERSCONTROLLER_H
#define OILSPILL_SCHEDULERSCONTROLLER_H

#include <unordered_map>
#include <memory>
#include "core/Sea.h"
#include "core/EditItem.h"
#include "Scheduler.h"

class Sea;
class Scheduler;

typedef std::unordered_map<EditItem, std::unique_ptr<Scheduler>> SchedulerMap;

class SchedulersController
{
private:
    std::shared_ptr<Sea> sea;
    std::unordered_map<EditItem, std::unique_ptr<Scheduler>> schedulersMap;

public:
    SchedulersController(std::shared_ptr<Sea> sea);
    SchedulerMap& getSchedulersMap();
    void update(int iteration);
private:
    void initializeSchedulersMap();
    
};


#endif //OILSPILL_SCHEDULERSCONTROLLER_H
