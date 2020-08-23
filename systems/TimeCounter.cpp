//
// Created by arczipt on 23.08.2020.
//

#include "TimeCounter.h"

void TimeCounter::update(int timestep) {
    totalTime += timestep;
    iteration++;
}

void TimeCounter::reset() {
    totalTime = 0L;
    iteration = 0;
}
