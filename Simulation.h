//
// Created by arczipt on 29.09.2020.
//

#ifndef OILSPILL_SIMULATION_H
#define OILSPILL_SIMULATION_H


#include <core/Sea.h>

class Simulation {
public:
    Simulation(const Configurations& configurations);

    void start(bool bench = false);

    void setOil(const std::string& path, int it);
    void addOil(const std::string& path, int it);

    void setCurrent(const std::string& path, int it);
    void setWind(const std::string& path, int it);

    void printOil();

private:
    std::shared_ptr<Sea> sea;
    Configurations config;
};


#endif //OILSPILL_SIMULATION_H
