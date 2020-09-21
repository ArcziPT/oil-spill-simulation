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

    for (auto &cell : cells) {
        for (auto &op : cell.oilPoints) {
            double temp = cell.temperature;
            auto &components = op.components;
            std::vector<double> lossMassArray;

            double totalMole = 0;
            for (auto &comp : components) {
                totalMole += (comp.getX() / comp.getMolecularWeight());
            }

            double x = 0;
            for (auto &comp : components) {
                x = comp.getX() / (comp.getMolecularWeight() * totalMole);

                if (x != 0) {
                    double Tb = comp.getTb();
                    double molecularWeigth = comp.getMolecularWeight();

                    double A = -(4.4 + std::log(Tb))
                               * (1.803 * (Tb / temp - 1) - 0.803 * std::log(Tb / temp));
                    double P = (double) (100000 * std::exp(A)); // Vapor pressure
                    // [Pa]
                    double lossmass = (K * molecularWeigth * P * x / (R * temp)
                                       * timestep * (config.cellSize * config.cellSize)) / cell.oilPoints.size();
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
            int i = 0;
            for (auto &comp : components) {
                double lossMass = lossMassArray[i++];
                if (lossMass > 0) {
                    double newX = (comp.getX() * actualMass - lossMass) / newMass;
                    if (newX < 0.00001) {
                        newX = 0;
                    }
                    comp.setX(newX);
                }

            }

            if (newMass > 0.00001) {
                double lastEvaporatedRatio = op.getEvaporatedRatio();
                op.mass = newMass;

                op.evaporatedMass = op.evaporatedMass + totalLossMass;
                op.massOfEmulsion = op.massOfEmulsion - totalLossMass;
                op.lastDeltaF = op.getEvaporatedRatio() - lastEvaporatedRatio;


            } else {
                cell.deletedOilPoints.push_back(op);
                op.removed = true;
            }
        }
    }
}
