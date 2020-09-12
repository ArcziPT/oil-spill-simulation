//
// Created by arczipt on 31.08.2020.
//

#ifndef OILSPILL_STATISTICSTYPE_H
#define OILSPILL_STATISTICSTYPE_H

#include <string>

struct AxisInfo{
    long id;
    std::string xAxis;
    std::string yAxis;

    AxisInfo(long id, const std::string& xAxis, const std::string& yAxis): id(id), xAxis(xAxis), yAxis(yAxis) {}
};

struct StatisticsType{
    static AxisInfo MASS;
    static AxisInfo DENSITY;
    static AxisInfo EMULSIFICATION;
    static AxisInfo EVAPORATION;
    static AxisInfo VISCOSITY;
    static AxisInfo DISPERSED_MASS;
    static AxisInfo AREA;
};


#endif //OILSPILL_STATISTICSTYPE_H
