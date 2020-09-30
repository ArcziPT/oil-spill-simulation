//
// Created by Mateusz Raczynski on 8/16/2020.
//

#include "SchedulersController.h"
#include "Scheduler.h"
#include "CurrentScheduler.h"
#include "WindScheduler.h"
#include "OilScheduler.h"
#include "TemperatureScheduler.h"

SchedulersController::SchedulersController(std::shared_ptr<Sea> _sea): sea(_sea)
{
    initializeSchedulersMap();
}

SchedulerMap &SchedulersController::getSchedulersMap()
{
    return schedulersMap;
}

void SchedulersController::update(int iteration)
{
    for (auto& it : this->schedulersMap) {
        it.second->update(iteration);
    }
}

std::vector<UpdateSubject> SchedulersController::getUpdateSubjects(int iteration) {
    std::vector<UpdateSubject> subjects;

    for(auto& it : this->schedulersMap){
        if(it.second->isThereUpdate(iteration)){
            subjects.push_back(it.second->getUpdateSubject(iteration));
        }
    }
    return subjects;
}

void SchedulersController::initializeSchedulersMap()
{
    schedulersMap[EditItem::CURRENT] = std::unique_ptr<CurrentScheduler>(new CurrentScheduler(sea));
    schedulersMap[EditItem::WIND] = std::unique_ptr<WindScheduler>(new WindScheduler(sea));
    schedulersMap[EditItem::OIL] = std::unique_ptr<OilScheduler>(new OilScheduler(sea));
    schedulersMap[EditItem::TEMPERATURE] = std::unique_ptr<TemperatureScheduler>(new TemperatureScheduler(sea));
}
