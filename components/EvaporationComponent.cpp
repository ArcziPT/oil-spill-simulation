//
// Created by Mateusz Święszek on 26/08/2020.
//

#include "EvaporationComponent.h"
#include <cmath>
#include <vector>

#include <CL/sycl.hpp>

using namespace cl;

EvaporationComponent::EvaporationComponent(Configurations &config) : config(config) {}

void EvaporationComponent::update(CellGrid &cells, int timestep) {
    for (int i = 0; i < cellsParams.size(); i++) {
        auto &cell = cellsParams[i];
        for (int j = 0; j < opComponents[i].size(); j++) {
            auto &op = opParams[i][j];
            auto &components = opComponents[i][j];

            double temp = cell.temperature;
            std::vector<double> lossMassArray;

            double totalMole = 0;
            for (auto &comp : components.components) {
                totalMole += (comp.x / comp.molecularWeigth);
            }

            double x = 0;
            for (auto &comp : components.components) {
                x = comp.x / (comp.molecularWeigth * totalMole);

                if (x != 0) {
                    double Tb = comp.tb;
                    double molecularWeigth = comp.molecularWeigth;

                    double A = -(4.4 + std::log(Tb))
                               * (1.803 * (Tb / temp - 1) - 0.803 * std::log(Tb / temp));
                    double P = (double) (100000 * std::exp(A)); // Vapor pressure
                    // [Pa]
                    double lossmass = (K * molecularWeigth * P * x / (R * temp)
                                       * timestep * (config.cellSize * config.cellSize)) /
                                      opParams[i].size(); //number of oilPoints
                    lossMassArray.push_back(lossmass);
                } else {
                    lossMassArray.push_back(0);
                }
            }

            double actualMass = op.mass;
            double totalLossMass = 0;
            for (double n : lossMassArray) {
                totalLossMass += n;
            }

            double newMass = actualMass - totalLossMass;
            int k = 0;
            for (auto &comp : components.components) {
                double lossMass = lossMassArray[k++];
                if (lossMass > 0) {
                    double newX = (comp.x * actualMass - lossMass) / newMass;
                    if (newX < 0.00001) {
                        newX = 0;
                    }
                    comp.x = newX;
                }

            }

            if (newMass > 0.00001) {
                double lastEvaporatedRatio = op.getEvaporatedRatio();
                op.mass = newMass;

                op.evaporatedMass = op.evaporatedMass + totalLossMass;
                op.massOfEmulsion = op.massOfEmulsion - totalLossMass;
                op.lastDeltaF = op.getEvaporatedRatio() - lastEvaporatedRatio;


            } else {
                cells.removeOilPoint(i, j);
                op.removed = true;
            }
        }
    }
}
