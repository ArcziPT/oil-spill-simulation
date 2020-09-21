#ifndef CELL_H
#define CELL_H

#include "Configurations.h"
#include "CellType.h"
#include "Vector2.h"
#include "OilPoint.h"

struct Cell
{
    Cell(int row, int col, Configurations &config) : row(row), col(col), config(config) {}

    Cell& operator=(Cell&& cell);
    Cell(const Cell&) = default;

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
    std::vector<double> getComponentsFraction();
    double getThickness();
    void setOil(double mass);
    void addOilPoints(int n);
    void addMass(double mass);

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

class CellGrid{
public:

    CellGrid(int row, int col, Configurations& config);
    CellGrid() = default;

    struct CellPos{
        int x;
        int y;
    };

    Cell& operator[](CellPos cellPos);

    std::vector<Cell>::iterator begin();
    std::vector<Cell>::iterator end();

    int getRow() const;
    int getCol() const;

private:
    int row = 0;
    int col = 0;
    std::vector<Cell> cells;
};

typedef std::vector<std::vector<double>> GridValuesType;

#endif
