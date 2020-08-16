//
// Created by Mateusz Raczynski on 8/16/2020.
//

#include "CurrentScheduler.h"


void CurrentScheduler::update(int iteration)
{
    auto it = map.find(iteration);
    if(it != map.end())
    {
        GridValuesType &array = it->second;
        sea.setCurrent(array);
    }
}

CurrentScheduler::CurrentScheduler(Sea sea) : Scheduler(sea)
{

}

void CurrentScheduler::add(int iteration, GridValuesType &array)
{
    if((array.size() + 2) != sea.getCells().size() ||
       array[0].size() != 2 * (sea.getCells().size() - 2))
    {
        throw InconsistentSizeException();
    }
    bool present = (map.find(iteration) != map.end());
    if(present)
    {
        throw FilePresentedException();
    }
    map[iteration] = array;
}
