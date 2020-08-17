//
// Created by Mateusz Raczynski on 8/16/2020.
//

#include "WindScheduler.h"

void WindScheduler::add(int iteration, GridValuesType &array)
{
    if ((array.size() + 2) != sea->getCells().size() || (array[0].size() != 2 * (sea->getCells().size() - 2))) {
        throw InconsistentSizeException();
    }
    bool present = (map.find(iteration) != map.end());
    if (present) {
        throw FilePresentedException();
    }
    map[iteration] = array;
}

void WindScheduler::update(int iteration)
{
    auto it = map.find(iteration);
    if(it != map.end())
    {
        GridValuesType &array = it->second;
        sea->setWind(array);
    }
}

WindScheduler::WindScheduler(SeaPtr sea) : Scheduler(sea)
{

}
