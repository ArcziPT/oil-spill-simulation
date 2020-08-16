#ifndef CONFIGURATIONS_H
#define CONFIGURATIONS_H

#include "OilComponent.h"

#include <vector>

struct Configurations
{
	int rows;
	int cols;
	double initialMassOfOilPoint;
	double cellSize;
	std::vector<OilComponent> oilComponents;
	int timestep;
	double initialDensityOfOilPoint;
	double salinity; // mg/l
	long simulationTime;
	double spreadingCoefficient;
	double mousseViscosity = 2;
	double viscosity;
	double viscosityParameter; // C4 sebastiano równanie 13
	double oilWaterTension;
	double minSlickThickness; // m
};

#endif
