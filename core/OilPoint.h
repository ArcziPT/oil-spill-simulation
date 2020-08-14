#ifndef OIL_POINT_H
#define OIL_POINT_H

#include "../components/TemperatureDependency.h"
#include "OilComponent.h"
#include "Configurations.h"
#include "Vector2.h"

#include <vector>

struct OilPoint{
	OilPoint(const Vector2& position, Long id, Configurations config, Cell cell): 
		mass(config.initialMassOfOilPoint), massOfEmulsion(config.initialMassOfOilPoint),
		id(id), position(position), density(TemperatureDependency.calculateOilDensity(config.initialDensityOfOilPoint, cell.getTemperature())),
		viscosity(config.viscosity), components(config.oilComponents), config(config) {}


	double getEvaporatedRatio();

    Vector2 position;
	Vector2 velocity{0, 0};
	long id;
	bool removed = false;
	double mass;
	double density;
	double massOfEmulsion;
	double evaporatedMass = 0;
	double lastDeltaF = 0;
	double lastDeltaY = 0;
	double viscosity;
	double dispersedMass = 0;
	std::vector<OilComponent> components;

	Configurations& config;
};

#endif