//
// Created by Mateusz Raczynski on 8/16/2020.
//

#include <core/Configurations.h>
#include <memory>
#include <core/Sea.h>
#include <iostream>

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
    config.simulationTime = 600;
    config.oilComponents = {
            OilComponent(0.25, 45.02/1000, 313),
            OilComponent(0.25, 45.02/1000, 313),
            OilComponent(0.25, 45.02/1000, 313),
            OilComponent(0.25, 45.02/1000, 313)
    };

    auto sea = std::make_shared<Sea>(config);
    sea->init();

    auto oil = GridValuesType<double>(148, std::vector<double>(148, 0));
    for(int i=0; i<148; i++){
        for(int j=0; j<148; j++){
            oil[i][j] = 1000;
        }
    }
    sea->setOil(oil);

    while(!sea->isFinished()){
        auto begin = std::chrono::high_resolution_clock::now();
        sea->update();
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end-begin).count()<< "ms" << std::endl;
    }

    std::cout<<"##############################################################################################################################\n";
    for(auto& row : sea->getOil()){
        for(auto& el : row){
            std::cout<<el<<" ";
        }
        std::cout<<std::endl;
    }

    return 0;
}
