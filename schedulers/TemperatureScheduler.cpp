//
// Created by Mateusz Raczynski on 8/16/2020.
//

#include "TemperatureScheduler.h"

void TemperatureScheduler::add(int iteration, GridType &array)
{
    if ((array.size() + 2) != sea.getCells().length || (array[0].size() + 2) != sea.getCells().length) {
        throw InconsistentSizeException();
    }
    bool present = (map.find(iteration) != map.end());
    if (present) {
        throw FilePresentedException();
    }
    map[iteration] = array;
}

void TemperatureScheduler::update(int iteration)
{
    auto it = map.find(iteration);
    if(it != map.end())
    {
        GridType &array = it->second;
        sea.setTemperature(array);
    }
}
