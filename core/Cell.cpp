#include "Cell.h"

#include <cmath>
#include <random>
#include <iostream>
#include <numeric>

//double CellGrid::getDensity(int i)
//{
//	double result = 0;
//	for (auto &op :  cellsOilPointsParams[i])
//	{
//		result += op.density * op.massOfEmulsion;
//	}
//	if (!cellsOilPointsParams[i].empty())
//	{
//		return result / getMassOfEmulsion(i);
//	}
//	else
//	{
//		return 0;
//	}
//}
//
//double CellGrid::getViscosity(int i)
//{
//	double result = 0;
//	for (auto &op :  cellsOilPointsParams[i])
//	{
//		result += op.viscosity * op.massOfEmulsion;
//	}
//	if (!cellsOilPointsParams[i].empty())
//	{
//		return result / getMassOfEmulsion(i);
//	}
//	else
//	{
//		return 0;
//	}
//}
//
//double CellGrid::getMassOfEmulsion(int i)
//{
//	double result = 0;
//	for (auto &op :  cellsOilPointsParams[i])
//	{
//		result += op.massOfEmulsion;
//	}
//
//	return result;
//}
//
//double CellGrid::getEmulsification(int i)
//{
//	double mass = getOil(i);
//	double emulsionMass = getMassOfEmulsion(i);
//	if (emulsionMass > 0)
//	{
//		return (emulsionMass - mass) / emulsionMass;
//	}
//	else
//	{
//		return 0;
//	}
//}
//
//// ułamek odparowania dla kawałków ropy które istnieją w kwadracie
//double CellGrid::getEvaporationRatio(int i)
//{
//	int size =  cellsOilPointsParams[i].size();
//	if (size != 0)
//	{
//		return getEvaporatedMass(i) / (config.initialMassOfOilPoint * size);
//	}
//	else
//	{
//		return 0;
//	}
//}
//
//double CellGrid::getDispersedMass(int i)
//{
//	double result = 0;
//	for (auto &op :  cellsOilPointsParams[i])
//	{
//		result += op.dispersedMass;
//	}
//	return result;
//}
//
//// masa odparowana z z kawałków ktore aktualnie są w kwadracie
//double CellGrid::getEvaporatedMass(int i)
//{
//	double result = 0;
//	for (auto &op :  cellsOilPointsParams[i])
//	{
//		result += op.evaporatedMass;
//	}
//	return result;
//}
//
//// masa odparowana z z kawałków ktore aktualnie są w kwadracie+ te które
//// zostały calkowicie usuniete
//double CellGrid::getTotalEvaporatedMass(int i)
//{
//	double result = 0;
//	for (auto &op : cellsOilPointsParams[i])
//	{
//		result += op.evaporatedMass;
//	}
//
//	for (auto &op : cellDeletedOilPointsParams[i])
//	{
//		result += op.evaporatedMass;
//	}
//
//	return result;
//}
//
//double CellGrid::getTotalDispersedMass(int i)
//{
//	double result = 0;
//	for (auto &op : cellsOilPointsParams[i])
//	{
//		result += op.dispersedMass;
//	}
//
//	for (auto &op : cellDeletedOilPointsParams[i])
//	{
//		result += op.dispersedMass;
//	}
//
//	return result;
//}
//
//double CellGrid::getVolume(int i)
//{
//	double density = getDensity(i);
//	if (density != 0)
//	{
//		return getMassOfEmulsion(i) / getDensity(i);
//	}
//	else
//	{
//		return 0;
//	}
//}
//
//std::vector<double> CellGrid::getComponentsFraction(OilPoint::Params::CellPos pos)
//{
//    int i = id(x,y);
//	if (!cellsOilPointsParams[i].empty()){
//		int numberOfComponents = config.oilComponents.size();
//		std::vector<double> xValues(numberOfComponents, 0);
//
//		double oilMass = getOil(i);
//
//		for (int j = 0; j < numberOfComponents; j++)
//		{
//			double a = 0;
//
//			for(int k=0; k<cellsOilPointsParams[i].size(); k++){
//			    a += cellsOilPointsParams[i][k].mass *
//			            cellsOilPointsComponents[i][k].components[j].x;
//			}
//			xValues.push_back(a / oilMass);
//		}
//		return xValues;
//	}
//
//	return {};
//}

void CellGrid::setTemperature(int x, int y, double temperature)
{
    int i = id(OilPoint::Params::CellPos(x,y));
    cellsParams[i].temperature = temperature;
}

void CellGrid::setWind(int x, int y, const Vector2& wind)
{
    int i = id(OilPoint::Params::CellPos(x,y));
    cellsParams[i].wind = wind;
}

void CellGrid::setCurrent(int x, int y, const Vector2& current)
{
    int i = id(OilPoint::Params::CellPos(x,y));
    cellsParams[i].current = current;
}

void CellGrid::setOil(int x, int y, double mass)
{
    int n = (int)(mass / config.initialMassOfOilPoint);
    addOilPoints(x, y, n);
}

 void CellGrid::addOilPoints(int x, int y, int n)
 {
     std::random_device rd;
     std::mt19937 mt(rd());
     std::uniform_real_distribution<double> dist(0, config.cellSize);

     int i = id(OilPoint::Params::CellPos(x,y));

 	for (int j = 0; j < n; j++){
 		auto px = (double)(cellsParams[i].row * config.cellSize + dist(mt));
 		auto py = (double)(cellsParams[i].col * config.cellSize + dist(mt));

 		cellsOilPointsParams.emplace_back(OilPoint::Params(
 		        config.initialMassOfOilPoint,
 		        config.initialMassOfOilPoint,
 		        Vector2(px, py),
 		        config.initialDensityOfOilPoint,
 		        config.viscosity,
 		        OilPoint::Params::CellPos(cellsParams[i].row, cellsParams[i].col)));
 		
 		for(auto& comp : config.oilComponents){
            cellsOilPointsComponents.push_back(comp);
        }
 	}
 }

void CellGrid::addMass(int x, int y, double mass)
{
	int n = (int)(mass / config.initialMassOfOilPoint);
	addOilPoints(x, y, n);
}

void CellGrid::init(int row, int col, int compNum) {
    this->row = row;
    this->col = col;
    this->compNum = compNum;
    cellsParams.reserve(row*col);
    cellsOilPointsParams.reserve(row*col);
    cellsOilPointsComponents.reserve(row*col*compNum);

    for(int i=0; i<row; i++){
        for(int j=0; j<col; j++){
            cellsParams.push_back(Cell::Params(i, j));
        }
    }
}

void CellGrid::setOil(std::vector<std::vector<double>> oil) {
    for(int i=1; i<row-1; i++){
        for(int j=1; j<col-1; j++){
            setOil(i, j, oil[i-1][j-1]);
        }
    }
}

std::vector<std::vector<double>> CellGrid::getOil() {
    auto ret = std::vector<std::vector<double>>(row-2, std::vector<double>(col-2, 0));
    for(auto& opParam : cellsOilPointsParams){
        if(!opParam.removed)
            ret[opParam.cellPos.x-1][opParam.cellPos.y-1] += opParam.mass;
    }

    return ret;
}

void CellGrid::setTemperature(std::vector<std::vector<double>> temperature) {
    for(int i=1; i<row-1; i++){
        for(int j=1; j<col-1; j++){
            setTemperature(i, j, temperature[i-1][j-1]);
        }
    }
}

void CellGrid::setCurrent(std::vector<std::vector<double>> current) {
    for(int i=1; i<row-1; i++){
        for(int j=1; j<col-1; j++){
            setCurrent(i, j, Vector2(current[i - 1][2 * j - 2], current[i - 1][2 * j - 1]));
        }
    }
}

void CellGrid::setWind(std::vector<std::vector<double>> wind) {
    for(int i=1; i<row-1; i++){
        for(int j=1; j<col-1; j++){
            setWind(i, j, Vector2(wind[i - 1][2 * j - 2], wind[i - 1][2 * j - 1]));
        }
    }
}

int CellGrid::getRow() const {
    return row;
}

int CellGrid::getCol() const {
    return col;
}

int CellGrid::size() const {
    return row*col;
}

std::vector<Cell::Params> &CellGrid::getCellParams() {
    return cellsParams;
}

std::vector<OilPoint::Params> &CellGrid::getOilPointsParams() {
    //remove unused oilPoints
    std::vector<OilPoint::Params> newParams;
    std::vector<OilComponent> newComp;

    for(int i=0; i<cellsOilPointsParams.size(); i++){
        auto& op = cellsOilPointsParams[i];

        if(!op.removed){
            newParams.emplace_back(std::move(op));

            for(int j=i; j<i+config.oilComponents.size(); j++){
                newComp.emplace_back(cellsOilPointsComponents[j]);
            }
        }
    }

    cellsOilPointsParams = newParams;
    cellsOilPointsComponents = newComp;

    return cellsOilPointsParams;
}

std::vector<OilComponent> &CellGrid::getOilPointsComponents() {
    return cellsOilPointsComponents;
}
