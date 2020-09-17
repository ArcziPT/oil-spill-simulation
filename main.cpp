//
// Created by Mateusz Raczynski on 8/16/2020.
//

#include <core/Configurations.h>
#include <memory>
#include <core/Sea.h>

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
    config.minSlickThickness = 0.03;
    config.simulationTime = 1000;
    config.oilComponents = {
            OilComponent(0.02, 45.02, 313),
            OilComponent(0.02, 45.02, 313),
            OilComponent(0.02, 45.02, 313),
            OilComponent(0.02, 45.02, 313)
    };

    auto sea = std::make_shared<Sea>(config);
    sea->init();
    sea->update();

    return 0;
}