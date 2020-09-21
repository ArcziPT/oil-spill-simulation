#ifndef OIL_COMPONENT_H
#define OIL_COMPONENT_H

struct OilComponent
{
    OilComponent(double x, double molecularWeigth, double tb) : x(x), molecularWeigth(molecularWeigth), tb(tb) {}

    double x;               // ulamek masowy
    double molecularWeigth; // masa molowa kg/moll
    double tb;              // temperatura wrzenia w K
};

#endif