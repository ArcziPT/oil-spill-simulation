//
// Created by arczipt on 31.08.2020.
//

#ifndef OILSPILL_STATISTICS_H
#define OILSPILL_STATISTICS_H


#include <map>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <core/Configurations.h>
#include <systems/TimeCounter.h>
#include "StatisticsType.h"

using CellGrid = std::vector<std::vector<Cell>>;

class Statistics {
public:

    Statistics(Configurations& config);

    int getItNumber();
    long getTime();
    double getLastValue(AxisInfo& cT);
    std::vector<double> getAllValues(AxisInfo& cT);
    std::vector<long> getTimeList();
    void initializeTypeListMap();
    void initialize(TimeCounter timeCounters, CellGrid& cellGrid);
    void clearAll();
    void update(TimeCounter timeCounter, CellGrid& cellGrid);

private:

    std::map<AxisInfo, std::vector<double>*> typeListMap{};// = new EnumMap<>(StatisticType.class);
    std::unordered_map<int, long> itTimeMap{};
    std::vector<double> oilPointsNumberList{};
    std::vector<double> oilMassList{};
    std::vector<double> emulsionMassList{};
    std::vector<double> emulsificationList{};
    std::vector<double> evaporatedMassList{};
    std::vector<double> dispersedMassList{};
    std::vector<double> densityList{};
    std::vector<double> viscosityList{};
    std::vector<std::vector<double>> componentsFractionList{};
    std::vector<double> areaList{};
    long actualTime = 0;
    int rows, cols;
    int numberOfComponents = 0;
    double cellArea;
};


#endif //OILSPILL_STATISTICS_H
