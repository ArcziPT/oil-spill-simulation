#ifndef OIL_POINT_H
#define OIL_POINT_H

#include "../components/TemperatureDependency.h"
#include "OilComponent.h"
#include "Configurations.h"
#include "Vector2.h"

#include <vector>

namespace OilPoint
{
	struct Params {
        Vector2 position;
        Vector2 velocity{0, 0};
        bool removed = false;
        double mass;
        double density;
        double massOfEmulsion;
        double evaporatedMass = 0;
        double lastDeltaF = 0;
        double lastDeltaY = 0;
        double viscosity;
        double dispersedMass = 0;

        double initialMassOfOilPoint;

        Params(double mass, double massOfEmulsion, double initialMassOfOilPoint,
               const Vector2& position, double density,
               double viscosity)
               : mass(mass), massOfEmulsion(massOfEmulsion), initialMassOfOilPoint(initialMassOfOilPoint),
               density(density), position(position), viscosity(viscosity) {}

        double getEvaporatedRatio() const;
        double getEmulsification() const;
	};

	struct Components{
	    std::vector<OilComponent> components;

	    Components(const std::vector<OilComponent>& oilComponents): components(oilComponents) {}
	};
};

#endif
