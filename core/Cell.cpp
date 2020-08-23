#include "Cell.h"

#include <cmath>
#include <random>


Cell& Cell::operator=(Cell &&cell) {
    this->col = cell.col;
    this->config = cell.config;
    this->current = cell.current;
    this->deletedOilPoints = std::move(cell.deletedOilPoints);
    this->row = cell.row;
    this->temperature = cell.temperature;
    this->type = cell.type;
    this->wind = cell.wind;

    return *this;
}

double Cell::getOil()
{
	double result = 0;
	for (auto &op : oilPoints)
	{
		result += op.mass;
	}
	return result;
}

double Cell::getDensity()
{
	double result = 0;
	for (auto &op : oilPoints)
	{
		result += op.density * op.massOfEmulsion;
	}
	if (oilPoints.size() != 0)
	{
		return result / getMassOfEmulsion();
	}
	else
	{
		return 0;
	}
}

double Cell::getViscosity()
{
	double result = 0;
	for (auto &op : oilPoints)
	{
		result += op.viscosity * op.massOfEmulsion;
	}
	if (oilPoints.size() != 0)
	{
		return result / getMassOfEmulsion();
	}
	else
	{
		return 0;
	}
}

double Cell::getMassOfEmulsion()
{
	double result = 0;
	for (auto &op : oilPoints)
	{
		result += op.massOfEmulsion;
	}

	return result;
}

double Cell::getEmulsification()
{
	double mass = getOil();
	double emulsionMass = getMassOfEmulsion();
	if (emulsionMass > 0)
	{
		return (emulsionMass - mass) / emulsionMass;
	}
	else
	{
		return 0;
	}
}

// ułamek odparowania dla kawałków ropy które istnieją w kwadracie
double Cell::getEvaporationRatio()
{
	int size = oilPoints.size();
	if (size != 0)
	{
		return getEvaporatedMass() / (config.initialMassOfOilPoint * size);
	}
	else
	{
		return 0;
	}
}

double Cell::getDispersedMass()
{
	double result = 0;
	for (auto &op : oilPoints)
	{
		result += op.dispersedMass;
	}
	return result;
}

// masa odparowana z z kawałków ktore aktualnie są w kwadracie
double Cell::getEvaporatedMass()
{
	double result = 0;
	for (auto &op : oilPoints)
	{
		result += op.evaporatedMass;
	}
	return result;
}

// masa odparowana z z kawałków ktore aktualnie są w kwadracie+ te które
// zostały calkowicie usuniete
double Cell::getTotalEvaporatedMass()
{
	double result = 0;
	for (auto &op : oilPoints)
	{
		result += op.evaporatedMass;
	}

	for (auto &op : deletedOilPoints)
	{
		result += op.evaporatedMass;
	}

	return result;
}

double Cell::getTotalDispersedMass()
{
	double result = 0;
	for (auto &op : oilPoints)
	{
		result += op.dispersedMass;
	}

	for (auto &op : deletedOilPoints)
	{
		result += op.dispersedMass;
	}

	return result;
}

double Cell::getVolume()
{
	double density = getDensity();
	if (density != 0)
	{
		return getMassOfEmulsion() / getDensity();
	}
	else
	{
		return 0;
	}
}

std::vector<double> Cell::getComponentsFraction()
{
	if (!oilPoints.empty())
	{
		int numberOfComponents = config.oilComponents.size();
		std::vector<double> xValues(numberOfComponents, 0);

		double oilMass = getOil();

		for (int i = 0; i < numberOfComponents; i++)
		{
			double x = 0;
			for (auto &op : oilPoints)
			{
				x += op.components[i].getX() * op.mass;
			}
			xValues.push_back(x / oilMass);
		}
		return xValues;
	}

	return {};
}

double Cell::getThickness()
{
	return getVolume() / (config.cellSize * config.cellSize);
}

void Cell::setOil(double mass)
{
	oilPoints.clear();
	int n = (int)(mass / config.initialMassOfOilPoint);
	addOilPoints(n);
}

 void Cell::addOilPoints(int n)
 {
     std::random_device rd;
     std::mt19937 mt(rd());
     std::uniform_real_distribution<double> dist(1.0, 10.0);

 	for (int i = 0; i < n; i++)
 	{
 	    //TODO????: replace '*' with '%'
 		auto x = (double)((col + dist(mt)) * config.cellSize);
 		auto y = (double)((row + dist(mt)) * config.cellSize);

 		oilPoints.push_back(OilPoint(Vector2(x, y), config, *this));
 	}
 }

void Cell::addMass(double mass)
{
	int n = (int)(mass / config.initialMassOfOilPoint);
	addOilPoints(n);
}