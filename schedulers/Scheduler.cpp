//
// Created by Mateusz Raczynski on 8/16/2020.
//

#include "Scheduler.h"

Scheduler::Scheduler(Sea _sea): sea(_sea)
{

}

void Scheduler::remove(int iteration)
{
    map.erase(iteration);
}
