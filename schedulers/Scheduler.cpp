//
// Created by Mateusz Raczynski on 8/16/2020.
//

#include "Scheduler.h"

Scheduler::Scheduler(std::shared_ptr<Sea> _sea): sea(_sea)
{

}

void Scheduler::remove(int iteration)
{
    map.erase(iteration);
}

bool Scheduler::isThereUpdate(int iteration) {
    return map.count(iteration) != 0;
}
