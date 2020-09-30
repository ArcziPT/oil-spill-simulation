//
// Created by arczipt on 29.09.2020.
//

#include <iostream>
#include <util/ArrayReader.h>
#include "Simulation.h"

Simulation::Simulation(const Configurations& configurations) : config(configurations) {
    sea = std::shared_ptr<Sea>(new Sea(config));
    sea->init();
}

void Simulation::start(bool bench) {
    while(!sea->isFinished()){
        if(bench){
            auto beg = std::chrono::high_resolution_clock::now();
            sea->update();
            auto end = std::chrono::high_resolution_clock::now();
            std::cout<<std::chrono::duration_cast<std::chrono::milliseconds>(end-beg).count()<<"ms"<<std::endl;
        }else {
            sea->update();
        }
    }
}

void Simulation::setOil(const std::string &path, int it) {
    auto arr = ArrayReader::read(path);

    sea->getSchedulersController()->getSchedulersMap()[EditItem::OIL]->add(it, arr);
}

void Simulation::addOil(const std::string &path, int it) {

}

void Simulation::setCurrent(const std::string &path, int it) {
    auto arr = ArrayReader::read(path, false);
    auto& s = sea->getSchedulersController()->getSchedulersMap()[EditItem::CURRENT];
    s->add(it, arr);
}

void Simulation::setWind(const std::string &path, int it) {
    auto arr = ArrayReader::read(path, false);
    auto& s = sea->getSchedulersController()->getSchedulersMap()[EditItem::WIND];
    s->add(it, arr);
}

void Simulation::printOil() {
    //sea->getCells().getOilPointsParams();
    auto oil = sea->getOil();

    for(auto& row : oil){
        for(auto& el : row){
            std::cout<<el<<" ";
        }
        std::cout<<std::endl;
    }
}
