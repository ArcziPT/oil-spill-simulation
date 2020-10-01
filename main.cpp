//
// Created by Mateusz Raczynski on 8/16/2020.
//

#include <core/Configurations.h>
#include <memory>
#include <core/Sea.h>
#include <iostream>
#include "Simulation.h"

int main(int argc, char** argv)
{
    Configurations config;
    config.rows = 150;
    config.cols = 150;
    config.cellSize = 20;
    config.initialMassOfOilPoint = 10;
    config.timestep = 100;
    config.salinity = 32;
    config.initialDensityOfOilPoint = 835;
    config.spreadingCoefficient = 3;
    config.mousseViscosity = 0.7;
    config.viscosity = 19.4;
    config.viscosityParameter = 10;
    config.oilWaterTension = 30;
    config.minSlickThickness = 0.03/1000;
    config.simulationTime = 2600;
    config.oilComponents = {
            OilComponent(0.25, 45.02/1000, 313),
            OilComponent(0.25, 45.02/1000, 313),
            OilComponent(0.25, 45.02/1000, 313),
            OilComponent(0.25, 45.02/1000, 313)
    };

    Simulation sim(config);
    sim.setOil("/home/arczipt/Desktop/oil/mockdata/oil0", 0);
    sim.setWind("/home/arczipt/Desktop/oil/mockdata/wind0", 0);
    sim.setWind("/home/arczipt/Desktop/oil/mockdata/wind5", 5);
    sim.setWind("/home/arczipt/Desktop/oil/mockdata/wind10", 10);
    sim.setWind("/home/arczipt/Desktop/oil/mockdata/wind15", 15);
    sim.setWind("/home/arczipt/Desktop/oil/mockdata/wind20", 20);
    sim.setWind("/home/arczipt/Desktop/oil/mockdata/wind25", 25);
    sim.setCurrent("/home/arczipt/Desktop/oil/mockdata/cur0", 0);
    sim.setCurrent("/home/arczipt/Desktop/oil/mockdata/cur5", 5);
    sim.setCurrent("/home/arczipt/Desktop/oil/mockdata/cur10", 10);
    sim.setCurrent("/home/arczipt/Desktop/oil/mockdata/cur15", 15);
    sim.setCurrent("/home/arczipt/Desktop/oil/mockdata/cur20", 20);
    sim.setCurrent("/home/arczipt/Desktop/oil/mockdata/cur25", 25);

    sim.start(true);
    sim.printOil();

    return 0;
}
