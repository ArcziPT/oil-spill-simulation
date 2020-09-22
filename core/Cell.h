#ifndef CELL_H
#define CELL_H

#include "Configurations.h"
#include "CellType.h"
#include "Vector2.h"
#include "OilPoint.h"

namespace Cell
{
    struct Params{
        int row;
        int col;
        TypeInfo type = CellType::SEA;
        Vector2 wind{0, 0};
        Vector2 current{0, 0};
        double temperature = 293.0;

        Params(int row, int col): row(row), col(col) {}
    };
};

class CellGrid{
public:
    CellGrid(Configurations& config): config(config) {}
    void init(int row, int col);

    Cell::Params& getCellParams(int x, int y);
    std::vector<OilPoint::Params> & getOilPointsParams(int x, int y);
    std::vector<OilPoint::Params> & getDeletedOilPointsParams(int x, int y);
    std::vector<OilPoint::Components> & getOilPointComponents(int x, int y);

    std::vector<Cell::Params>& getCellParams();
    std::vector<std::vector<OilPoint::Params>>& getOilPointsParams();
    std::vector<std::vector<OilPoint::Params>>& getDeletedOilPointsParams();
    std::vector<std::vector<OilPoint::Components>> & getOilPointComponents();

    void copyOilPoint(int sx, int sy, int si, int dx, int dy);
    void removeOilPoints(int x, int y, const std::vector<bool>& doRemove);
    void removeOilPoint(int i, int j);

    void setOil(std::vector<std::vector<double>> oil);
    std::vector<std::vector<double>> getOil();
    void setTemperature(std::vector<std::vector<double>> temperature);
    void setCurrent(std::vector<std::vector<double>> current);
    void setWind(std::vector<std::vector<double>> wind);

    double getOil(int i);
    double getThickness(int i);
    double getDensity(int i);
    double getViscosity(int i);
    double getMassOfEmulsion(int i);
    double getEmulsification(int i);
    double getEvaporationRatio(int i);
    double getDispersedMass(int i);
    double getEvaporatedMass(int i);
    double getTotalEvaporatedMass(int i);
    double getTotalDispersedMass(int i);
    double getVolume(int i);
    int size(int x, int y);
    std::vector<double> getComponentsFraction(int x, int y);

    int getRow() const;
    int getCol() const;
    int size() const;

    constexpr inline int id(int x, int y) const{
        return x*col + y;
    };

private:
    Configurations& config;
    int row = 0;
    int col = 0;

    std::vector<Cell::Params> cellsParams;
    std::vector<std::vector<OilPoint::Params>> cellsOilPointsParams;
    std::vector<std::vector<OilPoint::Params>> cellDeletedOilPointsParams;
    std::vector<std::vector<OilPoint::Components>> cellsOilPointsComponents;
    std::vector<std::vector<OilPoint::Components>> cellsDeletedOilPointsComponents;

    void setTemperature(int i, double temperature);
    void setWind(int i, const Vector2& wind);
    void setCurrent(int i, const Vector2& current);
    void setOil(int i, double mass);
    void addOilPoints(int i, int n);
    void addMass(int i, double mass);
};

template <typename T>
using GridValuesType = std::vector<std::vector<T>>;

#endif
