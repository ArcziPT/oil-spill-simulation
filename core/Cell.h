#ifndef CELL_H
#define CELL_H

#include "Configurations.h"
#include "CellType.h"
#include "Vector2.h"
#include "OilPoint.h"

class Cell
{
public:
    Cell(int row, int col, Configurations &config) : row(row), col(col), config(config) {}

    double getOil();
    double getDensity();
    double getViscosity();
    double getMassOfEmulsion();
    double getEmulsification();
    double getEvaporationRatio();
    double getDispersedMass();
    double getEvaporatedMass();
    double getTotalEvaporatedMass();
    double getTotalDispersedMass();
    double getVolume();
    std::string getComponentsString();
    std::vector<double> getComponentsFraction();
    double getThickness();
    void setOil(double mass);
    void addOilPoints(int n);
    void addMass(double mass);
    int getRow();
    int getCol();
    TypeInfo getType();
    void setType(TypeInfo type);
    Vector2 getWind();
    void setWind(Vector2 wind);
    Vector2 getCurrent();
    void setCurrent(Vector2 current);
    std::vector<OilPoint> getOilPoints();
    double getTemperature();
    void setTemperature(double temperature);
    std::vector<OilPoint> getDeletedOilPoints();
    void setDeletedOilPoints(const std::vector<OilPoint> &deletedOilPoints);

private:
    std::vector<OilPoint> oilPoints{};
    std::vector<OilPoint> deletedOilPoints{};
    int row;
    int col;
    TypeInfo type = CellType::SEA;
    Vector2 wind{0, 0};
    Vector2 current{0, 0};
    double temperature = 293.0;

    Configurations &config;
};

#endif