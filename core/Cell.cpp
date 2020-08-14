#include "Cell.h"

#include <cmath>

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

// std::string Cell::getComponentsString()
// {

// 	std::vector<double> xValues = getComponentsFraction();
// 	if (xValues.size() != 0)
// 	{
// 		StringBuilder builder = new StringBuilder();
// 		for (int i = 0; i < xValues.size(); i++)
// 		{
// 			ComponentOfOil comp = config
// 									  .getComponentsOfOil()
// 									  .get(i);
// 			builder.append("component " + (i + 1) + " : x=" + xValues.get(i) + " M=" + comp.getMolecularWeigth() + " Tb=" + comp.getTb() + "  <br/>");
// 		}
// 		return builder.toString();
// 	}
// 	else
// 		return "";
// }

std::vector<double> Cell::getComponentsFraction()
{
	if (oilPoints.size() > 0)
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
			xValues.add(x / oilMass);
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
	int n = (int)(mass / config.initialMassOfOilPoint;
	addOilPoints(n);
}

// void Cell::addOilPoints(int n)
// {
// 	Random rand = new Random();

// 	for (int i = 0; i < n; i++)
// 	{
// 		double x = (double)((col + rand.nextDouble()) * config
// 															.getCellSize());
// 		double y = (double)((row + rand.nextDouble()) * config
// 															.getCellSize());

// 		oilPoints.add(new OilPoint(new Vector2(x, y),
// 								   Incrementator.autoIncrementId++, config, this));
// 	}
// }

void Cell::addMass(double mass)
{
	int n = (int)(mass / config.initialMassOfOilPoint);
	addOilPoints(n);
}

int Cell::getRow()
{
	return row;
}

int Cell::getCol()
{
	return col;
}

CellType Cell::getType()
{
	return type;
}

void Cell::setType(CellType type)
{
	this->type = type;
}

Vector2 Cell::getWind()
{
	return wind;
}

void Cell::setWind(Vector2 wind)
{
	this->wind = wind;
}

Vector2 Cell::getCurrent()
{
	return current;
}

void Cell::setCurrent(Vector2 current)
{

	this->current = current;
}

std::vector<OilPoint> Cell::getOilPoints()
{
	return oilPoints;
}

double Cell::getTemperature()
{
	return temperature;
}

void Cell::setTemperature(double temperature)
{
	this->temperature = temperature;
}

std::vector<OilPoint> Cell::getDeletedOilPoints()
{
	return deletedOilPoints;
}

void Cell::setDeletedOilPoints(const std::vector<OilPoint> &deletedOilPoints)
{
	this->deletedOilPoints = deletedOilPoints;
}