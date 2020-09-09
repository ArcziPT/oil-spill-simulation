//
// Created by Mateusz Święszek on 09/09/2020.
//

#ifndef OILSPILL_TEMPERATUREDEPENDENCY_H
#define OILSPILL_TEMPERATUREDEPENDENCY_H


class TemperatureDependency {
public:
    static double calculateOilDensity(double densityAt15, double tempAtK);
    static double calculateWaterDensity(double temp, double conc);
    static double calculateWaterDensity2(double T, double s);
    static double calculateWaterDynamicViscosity(double T, double S);
};


#endif //OILSPILL_TEMPERATUREDEPENDENCY_H
