#include "OilPoint.h"

double OilPoint::getEvaporatedRatio()
{
    return evaporatedMass / config.initialMassOfOilPoint;
}

double OilPoint::getEmulsification() const {
    return (massOfEmulsion - mass) / massOfEmulsion;
}
