#ifndef CELL_H
#define CELL_H

#include "Configurations.h"
#include "CellType.h"
#include "Vector2.h"
#include "OilPoint.h"

#include <CL/sycl.hpp>

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

/**
 * Simulation data proxy.
 *
 * Manages cells', oil points' and theirs components' data.
 * Storing it continuously in vector enables better cooperation with SyCL.
 */
class CellGrid{
public:
    CellGrid(Configurations& config): config(config) {}
    void init(int row, int col, int compNum);

    std::vector<Cell::Params>& getCellParams();
    std::vector<OilPoint::Params>& getOilPointsParams();
    std::vector<OilComponent>& getOilPointsComponents();

    void setOil(std::vector<std::vector<double>> oil);
    std::vector<std::vector<double>> getOil();
    void setTemperature(std::vector<std::vector<double>> temperature);
    void setCurrent(std::vector<std::vector<double>> current);
    void setWind(std::vector<std::vector<double>> wind);

    int getRow() const;
    int getCol() const;
    int size() const;

    constexpr inline int id(const OilPoint::Params::CellPos& pos) const{
        return pos.x*col + pos.y;
    };

private:
    Configurations& config;
    int row = 0;
    int col = 0;
    int compNum = 0;

    std::vector<Cell::Params> cellsParams;
    std::vector<OilPoint::Params> cellsOilPointsParams;
    std::vector<OilComponent> cellsOilPointsComponents;

    void setTemperature(int x, int y, double temperature);
    void setWind(int x, int y, const Vector2& wind);
    void setCurrent(int x, int y, const Vector2& current);
    void setOil(int x, int y, double mass);
    void addOilPoints(int x, int y, int n);
    void addMass(int x, int y, double mass);
};

template <typename T>
using GridValuesType = std::vector<std::vector<T>>;

#endif
