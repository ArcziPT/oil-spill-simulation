#include "OilPoint.h"

double OilPoint::getEvaporatedRatio()
{
    return evaporatedMass / config.initialMassOfOilPoint;
}