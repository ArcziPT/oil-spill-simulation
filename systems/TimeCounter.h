//
// Created by arczipt on 23.08.2020.
//

#ifndef OILSPILL_TIMECOUNTER_H
#define OILSPILL_TIMECOUNTER_H


struct TimeCounter{
    long totalTime = 0L;
    int iteration = 0;

    void update(int timestep);
    void reset();
}


#endif //OILSPILL_TIMECOUNTER_H
