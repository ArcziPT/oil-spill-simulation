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
        double mass;
        double density;
        double massOfEmulsion;
        double evaporatedMass = 0;
        double lastDeltaF = 0;
        double lastDeltaY = 0;
        double viscosity;
        double dispersedMass = 0;

        double initialMassOfOilPoint;

        struct CellPos{
            int x;
            int y;
            CellPos(int x, int y): x(x), y(y) {}
        };
        CellPos cellPos;
        bool removed = false;

        Params(double mass, double massOfEmulsion, double initialMassOfOilPoint,
               const Vector2& position, double density,
               double viscosity, CellPos pos)
               : mass(mass), massOfEmulsion(massOfEmulsion), initialMassOfOilPoint(initialMassOfOilPoint),
               density(density), position(position), viscosity(viscosity), cellPos(pos) {}

        double getEvaporatedRatio() const;
        double getEmulsification() const;
	};
};

#endif
