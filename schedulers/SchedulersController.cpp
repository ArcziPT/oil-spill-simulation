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
    return schedulers;
}

void SchedulersController::update(int iteration)
{
    for (std::unique_ptr<Scheduler> scheduler : this->schedulersMap) {
        scheduler.update(iteration);
    }
}

void SchedulersController::initializeSchedulersMap()
{
    schedulersMap[EditItem.CURRENT] = std::unique_ptr<CurrentScheduler>(new CurrentScheduler);
    schedulersMap[EditItem.WIND] = std::unique_ptr<WindScheduler>(new WindScheduler);
    schedulersMap[EditItem.OIL] = std::unique_ptr<OilScheduler>(new OilScheduler);
    schedulersMap[EditItem.TEMPERATURE] = std::unique_ptr<TemperatureScheduler>(new TemperatureScheduler);
}
