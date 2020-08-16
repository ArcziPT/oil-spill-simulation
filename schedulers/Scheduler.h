//
// Created by Mateusz Raczynski on 8/16/2020.
//

#ifndef OIL_SPILL_SIMULATION_SCHEDULER_H
#define OIL_SPILL_SIMULATION_SCHEDULER_H

#include <unordered_map>
#include <vector>

typedef std::vector<std::vector<double>> GridType;

class Scheduler
{
protected:
    std::unordered_map<int, GridType> map;
    Sea sea;
    
public:
    Scheduler(Sea _sea);
    virtual void add(int iteration, GridType& array) = 0;
    void remove(int iteration);
    virtual void update(int iteration) = 0;
};


#endif //OIL_SPILL_SIMULATION_SCHEDULER_H
