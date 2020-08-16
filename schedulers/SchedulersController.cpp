//
// Created by Mateusz Raczynski on 8/16/2020.
//

#include "SchedulersController.h"
#include "Scheduler.h"
#include "CurrentScheduler.h"
#include "WindScheduler.h"
#include "OilScheduler.h"
#include "TemperatureScheduler.h"

SchedulersController::SchedulersController(Sea _sea): sea(_sea)
{

}

SchedulerMap &SchedulersController::getSchedulersMap()
{
    return schedulersMap;
}

void SchedulersController::update(int iteration)
{
    for (auto& it : this->schedulersMap) {
        std::unique_ptr<Scheduler>& scheduler = it.second;
        scheduler->update(iteration);
    }
}

void SchedulersController::initializeSchedulersMap()
{
    schedulersMap[EditItem::CURRENT] = std::unique_ptr<CurrentScheduler>(new CurrentScheduler(sea));
    schedulersMap[EditItem::WIND] = std::unique_ptr<WindScheduler>(new WindScheduler(sea));
    schedulersMap[EditItem::OIL] = std::unique_ptr<OilScheduler>(new OilScheduler(sea));
    schedulersMap[EditItem::TEMPERATURE] = std::unique_ptr<TemperatureScheduler>(new TemperatureScheduler(sea));
}
