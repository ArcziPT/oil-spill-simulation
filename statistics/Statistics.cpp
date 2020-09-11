//
// Created by arczipt on 31.08.2020.
//

#include "Statistics.h"

Statistics::Statistics(Configurations& config): rows(config.rows), cols(config.cols),
                                    numberOfComponents(config.oilComponents.size()),
                                    cellArea(config.cellSize*config.cellSize) {
    initializeTypeListMap();
}

int Statistics::getItNumber() {
    return itTimeMap.size() - 1;
}

long Statistics::getTime() {
    return actualTime;
}

double Statistics::getLastValue(AxisInfo& cT) {
    auto& list = *typeListMap[cT];
    return list[list.size() - 1];
}

std::vector<double> Statistics::getAllValues(AxisInfo& cT) {
    auto list = typeListMap[cT];
    return *list;
}

std::vector<long> Statistics::getTimeList() {
    auto result = std::vector<long>(itTimeMap.begin(), itTimeMap.end());
    std::sort(result.begin(), result.end());
    return result;
}

void Statistics::initializeTypeListMap() {
    typeListMap.insert({StatisticsType::MASS, &oilMassList});
    typeListMap.insert({StatisticsType::DENSITY, &densityList});
    typeListMap.insert({StatisticsType::EMULSIFICATION, &emulsificationList});
    typeListMap.insert({StatisticsType::EVAPORATION, &evaporatedMassList});
    typeListMap.insert({StatisticsType::VISCOSITY, &viscosityList});
    typeListMap.insert({StatisticsType::DISPERSED_MASS, &dispersedMassList});
    typeListMap.insert({StatisticsType::AREA, &areaList});
}

void Statistics::initialize(TimeCounter timeCounters, CellGrid& cellGrid) {
    clearAll();
    update(timeCounters, cellGrid);
}

void Statistics::clearAll() {
    oilPointsNumberList.clear();
    oilMassList.clear();
    emulsionMassList.clear();
    evaporatedMassList.clear();
    emulsificationList.clear();
    itTimeMap.clear();
    densityList.clear();
    componentsFractionList.clear();
    dispersedMassList.clear();
    viscosityList.clear();
    actualTime = 0;
    areaList.clear();
}

void Statistics::update(TimeCounter timeCounter, CellGrid& cellGrid) {
    if (itTimeMap.count(timeCounter.iteration) == 0) {
        itTimeMap.insert({timeCounter.iteration, timeCounter.totalTime});

        int oilPoints = 0;
        double oilMass = 0;
        double emulsionMass = 0;
        double density = 0;
        double evaporatedMass = 0;
        double viscosity = 0;
        double dispersedMass = 0;
        double area = 0;

        auto listOfFraction = std::vector<double>(numberOfComponents, 0);
        for (int i = 1; i < rows - 1; i++) {
            for (int j = 1; j < cols - 1; j++) {
                auto &cell = cellGrid[i][j];

                oilPoints += cell.oilPoints.size();

                double actualMass = cell.getOil();
                oilMass += actualMass;
                if (actualMass > 0) {
                    area++;
                }

                emulsionMass += cell.getMassOfEmulsion();
                density += cell.getDensity() * cell.getMassOfEmulsion();
                evaporatedMass += cell.getTotalEvaporatedMass();
                dispersedMass += cell.getTotalDispersedMass();
                viscosity += cell.getViscosity() * cell.getMassOfEmulsion();

                auto actualListOfFraction = cell.getComponentsFraction();
                if (!actualListOfFraction.empty()) {
                    for (int k = 0; k < numberOfComponents; k++) {
                        listOfFraction[k] = listOfFraction[k] + actualListOfFraction[k] * actualMass;
                    }
                }
            }

        }
        for (int k = 0; k < numberOfComponents; k++) {
            listOfFraction[k] = listOfFraction[k] / oilMass;
        }

        oilPointsNumberList.push_back(oilPoints);
        oilMassList.push_back(oilMass);
        emulsionMassList.push_back(emulsionMass);

        actualTime = timeCounter.totalTime;

        emulsificationList.push_back((emulsionMass - oilMass) / emulsionMass * 100); // procenty
        densityList.push_back(density / emulsionMass);
        viscosityList.push_back(viscosity / emulsionMass);
        componentsFractionList.push_back(listOfFraction);
        evaporatedMassList.push_back(evaporatedMass);
        dispersedMassList.push_back(dispersedMass);
        areaList.push_back(area * cellArea);
    }
}