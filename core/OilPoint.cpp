#include "OilPoint.h"

double OilPoint::Params::getEvaporatedRatio() const
{
    return evaporatedMass / initialMassOfOilPoint;
}

double OilPoint::Params::getEmulsification() const {
    return (massOfEmulsion - mass) / massOfEmulsion;
}
