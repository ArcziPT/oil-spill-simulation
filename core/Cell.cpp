#include "Cell.h"

#include <cmath>
#include <random>
#include <iostream>
#include <numeric>

double CellGrid::getDensity(int i)
{
	double result = 0;
	for (auto &op :  cellsOilPointsParams[i].oilPointsParams)
	{
		result += op.density * op.massOfEmulsion;
	}
	if (!cellsOilPointsParams[i].oilPointsParams.empty())
	{
		return result / getMassOfEmulsion(i);
	}
	else
	{
		return 0;
	}
}

double CellGrid::getViscosity(int i)
{
	double result = 0;
	for (auto &op :  cellsOilPointsParams[i].oilPointsParams)
	{
		result += op.viscosity * op.massOfEmulsion;
	}
	if (!cellsOilPointsParams[i].oilPointsParams.empty())
	{
		return result / getMassOfEmulsion(i);
	}
	else
	{
		return 0;
	}
}

double CellGrid::getMassOfEmulsion(int i)
{
	double result = 0;
	for (auto &op :  cellsOilPointsParams[i].oilPointsParams)
	{
		result += op.massOfEmulsion;
	}

	return result;
}

double CellGrid::getEmulsification(int i)
{
	double mass = getOil(i);
	double emulsionMass = getMassOfEmulsion(i);
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
double CellGrid::getEvaporationRatio(int i)
{
	int size =  cellsOilPointsParams[i].oilPointsParams.size();
	if (size != 0)
	{
		return getEvaporatedMass(i) / (config.initialMassOfOilPoint * size);
	}
	else
	{
		return 0;
	}
}

double CellGrid::getDispersedMass(int i)
{
	double result = 0;
	for (auto &op :  cellsOilPointsParams[i].oilPointsParams)
	{
		result += op.dispersedMass;
	}
	return result;
}

// masa odparowana z z kawałków ktore aktualnie są w kwadracie
double CellGrid::getEvaporatedMass(int i)
{
	double result = 0;
	for (auto &op :  cellsOilPointsParams[i].oilPointsParams)
	{
		result += op.evaporatedMass;
	}
	return result;
}

// masa odparowana z z kawałków ktore aktualnie są w kwadracie+ te które
// zostały calkowicie usuniete
double CellGrid::getTotalEvaporatedMass(int i)
{
	double result = 0;
	for (auto &op : cellsOilPointsParams[i].oilPointsParams)
	{
		result += op.evaporatedMass;
	}

	for (auto &op : cellDeletedOilPointsParams[i].oilPointsParams)
	{
		result += op.evaporatedMass;
	}

	return result;
}

double CellGrid::getTotalDispersedMass(int i)
{
	double result = 0;
	for (auto &op : cellsOilPointsParams[i].oilPointsParams)
	{
		result += op.dispersedMass;
	}

	for (auto &op : cellDeletedOilPointsParams[i].oilPointsParams)
	{
		result += op.dispersedMass;
	}

	return result;
}

double CellGrid::getVolume(int i)
{
	double density = getDensity(i);
	if (density != 0)
	{
		return getMassOfEmulsion(i) / getDensity(i);
	}
	else
	{
		return 0;
	}
}

std::vector<double> CellGrid::getComponentsFraction(int x, int y)
{
    int i = id(x,y);
	if (!cellsOilPointsParams[i].oilPointsParams.empty()){
		int numberOfComponents = config.oilComponents.size();
		std::vector<double> xValues(numberOfComponents, 0);

		double oilMass = getOil(i);

		for (int j = 0; j < numberOfComponents; j++)
		{
			double a = 0;

			for(int k=0; k<cellsOilPointsParams[i].oilPointsParams.size(); k++){
			    a += cellsOilPointsParams[i].oilPointsParams[k].mass *
			            cellsOilPointsComponents[i].oilPointComponents[k].components[j].x;
			}
			xValues.push_back(a / oilMass);
		}
		return xValues;
	}

	return {};
}

void CellGrid::setTemperature(int i, double temperature)
{
    cellsParams[i].temperature = temperature;
}

void CellGrid::setWind(int i, const Vector2& wind)
{
    cellsParams[i].wind = wind;
}

void CellGrid::setCurrent(int i, const Vector2& current)
{
    cellsParams[i].current = current;
}

void CellGrid::setOil(int i, double mass)
{
    int n = (int)(mass / config.initialMassOfOilPoint);
    addOilPoints(i, n);
}

 void CellGrid::addOilPoints(int i, int n)
 {
     std::random_device rd;
     std::mt19937 mt(rd());
     std::uniform_real_distribution<double> dist(0, config.cellSize);

 	for (int j = 0; j < n; j++){
 		auto x = (double)(cellsParams[i].col * config.cellSize + dist(mt));
 		auto y = (double)(cellsParams[i].row * config.cellSize + dist(mt));

 		cellsOilPointsParams[i].oilPointsParams.push_back(OilPoint::Params(
 		        config.initialMassOfOilPoint,
 		        config.initialMassOfOilPoint,
                config.initialMassOfOilPoint,
 		        Vector2(x, y),
 		        config.initialDensityOfOilPoint,
 		        config.viscosity));
 		cellsOilPointsComponents[i].oilPointComponents.push_back(config.oilComponents);
 	}
 }

void CellGrid::addMass(int i, double mass)
{
	int n = (int)(mass / config.initialMassOfOilPoint);
	addOilPoints(i, n);
}

void CellGrid::init(int row, int col) {
    this->row = row;
    this->col = col;
    cellsParams.reserve(row*col);
    cellsOilPointsParams = std::vector<Cell::OilPointsParams>(row*col);
    cellDeletedOilPointsParams = std::vector<Cell::OilPointsParams>(row*col);
    cellsOilPointsComponents = std::vector<Cell::OilPointsComponents>(row*col);
    cellsDeletedOilPointsComponents = std::vector<Cell::OilPointsComponents>(row*col);

    for(int i=0; i<row; i++){
        for(int j=0; j<col; j++){
            cellsParams.push_back(Cell::Params(i, j));
        }
    }
}

void CellGrid::setOil(std::vector<std::vector<double>> oil) {
    for(int i=1; i<row-1; i++){
        for(int j=1; j<col-1; j++){
            setOil(id(i,j), oil[i-1][j-1]);
        }
    }
}

std::vector<std::vector<double>> CellGrid::getOil() {
    auto ret = std::vector<std::vector<double>>(row, std::vector<double>(col, 0));
    for(int i=0; i<row; i++){
        for(int j=0; j<col; j++){
            for(auto& opParam : cellsOilPointsParams[id(i,j)].oilPointsParams){
                ret[i][j] += opParam.mass;
            }
            std::cout<<ret[i][j]<<" ";
        }
        std::cout<<std::endl;
    }
    return ret;
}

void CellGrid::setTemperature(std::vector<std::vector<double>> temperature) {
    for(int i=1; i<row-1; i++){
        for(int j=1; j<col-1; j++){
            setTemperature(id(i,j), temperature[i-1][j-1]);
        }
    }
}

void CellGrid::setCurrent(std::vector<std::vector<double>> current) {
    for(int i=1; i<row-1; i++){
        for(int j=1; j<col-1; j++){
            setCurrent(id(i,j), Vector2(current[i - 1][2 * j - 2], current[i - 1][2 * j - 1]));
        }
    }
}

void CellGrid::setWind(std::vector<std::vector<double>> wind) {
    for(int i=1; i<row-1; i++){
        for(int j=1; j<col-1; j++){
            setWind(id(i,j), Vector2(wind[i - 1][2 * j - 2], wind[i - 1][2 * j - 1]));
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

double CellGrid::getOil(int i) {
    double ret = 0;
    for(auto& opParam : cellsOilPointsParams[i].oilPointsParams){
        ret += opParam.mass;
    }
    return ret;
}

double CellGrid::getThickness(int i) {
    return getVolume(i) / (config.cellSize * config.cellSize);
}

int CellGrid::size(int x, int y) {
    return cellsOilPointsParams[id(x,y)].oilPointsParams.size();
}

Cell::Params &CellGrid::getCellParams(int x, int y) {
    return cellsParams[id(x,y)];
}

Cell::OilPointsParams& CellGrid::getOilPointsParams(int x, int y) {
    return cellsOilPointsParams[id(x,y)];
}

Cell::OilPointsParams& CellGrid::getDeletedOilPointsParams(int x, int y) {
    return cellDeletedOilPointsParams[id(x,y)];
}

Cell::OilPointsComponents& CellGrid::getOilPointComponents(int x, int y) {
    return cellsOilPointsComponents[id(x,y)];
}

void CellGrid::copyOilPoint(int sx, int sy, int si, int dx, int dy) {
    cellsOilPointsParams[id(dx, dy)].oilPointsParams.push_back(cellsOilPointsParams[id(sx, sy)].oilPointsParams[si]);
    cellsOilPointsComponents[id(dx, dy)].oilPointComponents.push_back(cellsOilPointsComponents[id(sx, sy)].oilPointComponents[si]);
}

void CellGrid::removeOilPoints(int x, int y, const std::vector<bool> &doRemove) {
    Cell::OilPointsParams params{};
    Cell::OilPointsComponents components{};

    int i = id(x,y);
    for(int j=0; j<cellsOilPointsParams[i].oilPointsParams.size(); j++){
        if(!doRemove[j]){
            params.oilPointsParams.push_back(cellsOilPointsParams[i].oilPointsParams[j]);
            components.oilPointComponents.push_back(cellsOilPointsComponents[i].oilPointComponents[j]);
        }else{
            cellDeletedOilPointsParams[i].oilPointsParams.push_back(cellsOilPointsParams[i].oilPointsParams[j]);
            cellsDeletedOilPointsComponents[i].oilPointComponents.push_back(cellsOilPointsComponents[i].oilPointComponents[j]);
        }
    }

    cellsOilPointsParams[i] = params;
    cellsOilPointsComponents[i] = components;
}

std::vector<Cell::Params> &CellGrid::getCellParams() {
    return cellsParams;
}

std::vector<Cell::OilPointsParams> &CellGrid::getOilPointsParams() {
    return cellsOilPointsParams;
}

std::vector<Cell::OilPointsParams> &CellGrid::getDeletedOilPointsParams() {
    return cellDeletedOilPointsParams;
}

std::vector<Cell::OilPointsComponents> &CellGrid::getOilPointComponents() {
    return cellsOilPointsComponents;
}

void CellGrid::removeOilPoint(int i, int j) {
    cellDeletedOilPointsParams[i].oilPointsParams.push_back(cellsOilPointsParams[i].oilPointsParams[j]);
    cellsDeletedOilPointsComponents[i].oilPointComponents.push_back(cellsOilPointsComponents[i].oilPointComponents[j]);
    cellsOilPointsComponents[i].oilPointComponents.erase(cellsOilPointsComponents[i].oilPointComponents.begin() + j);
    cellsOilPointsParams[i].oilPointsParams.erase(cellsOilPointsParams[i].oilPointsParams.begin() + j);
}
