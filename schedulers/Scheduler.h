//
// Created by Mateusz Raczynski on 8/16/2020.
//

#ifndef OIL_SPILL_SIMULATION_SCHEDULER_H
#define OIL_SPILL_SIMULATION_SCHEDULER_H

#include <unordered_map>
#include <vector>
#include "core/Sea.h"
#include "exceptions.h"

using GridValuesType = std::vector<std::vector<double>>;
class Sea;

class Scheduler
{
protected:
    std::unordered_map<int, GridValuesType> map;
    std::shared_ptr<Sea> sea;
    
public:
    Scheduler(std::shared_ptr<Sea> _sea);
    virtual void add(int iteration, GridValuesType& array) = 0;
    void remove(int iteration);
    virtual void update(int iteration) = 0;
};

#endif //OIL_SPILL_SIMULATION_SCHEDULER_H
