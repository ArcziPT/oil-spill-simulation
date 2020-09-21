//
// Created by arczipt on 24.08.2020.
//

#include "SPreadingSystem.h"

double SpreadingSystem::calculateTotalVolume() {
    double result = 0;
    int rows = cells.getRow();
    int cols = cells.getCol();
    for (int i = 1; i < rows - 1; i++) {
        for (int j = 1; j < cols - 1; j++) {
            auto& cell = cells[{i, j}];
            result += cell.getVolume();
        }
    }
    return result;
}

Vector2 SpreadingSystem::determineVector2(Cell &cell1, Cell &cell2, double deltaMass) {
    int i1 = cell1.row;
    int i2 = cell2.row;
    double length = size;

    if (deltaMass > 0) {
        length = -length;
    }
    double x, y;
    if (i1 == i2) {
        x = length;
        y = 0;
    } else {
        x = 0;
        y = length;
    }
    return Vector2(x, y);
}

void SpreadingSystem::updatePair(Cell &cell1, Cell &cell2, int timestep, double volume) {
    double mass1 = cell1.getMassOfEmulsion();
    double mass2 = cell2.getMassOfEmulsion();

    if (mass1 == 0 && mass2 == 0) {
        return;
    }

    double oilDensity = (mass1 * cell1.getDensity() + mass2 * cell2.getDensity()) / (mass1 + mass2);
    double temperature = (cell1.temperature + cell2.temperature) / 2;
    double salinity = config.salinity;
    double waterDensity = TemperatureDependency::calculateWaterDensity(temperature, salinity);
    double kinematicWaterViscosity = TemperatureDependency::calculateWaterDynamicViscosity(temperature, salinity) / waterDensity;
    double time = timeSystem.totalTime + timestep / 2.;
    double delta = (waterDensity - oilDensity) / waterDensity;
    double base = (g * delta * volume * volume) / (std::sqrt(kinematicWaterViscosity));
    double diffusion = 0.49 / std::pow(config.spreadingCoefficient, 2) * std::pow(base, 1.0 / 3) * 1 / std::sqrt(time);

    if(!(cell1.type == CellType::SEA) && !(cell2.type == CellType::SEA))
        return;

    double deltaMass = (0.5 * (mass2 - mass1) * (1 - std::exp(-2 * diffusion / (size * size) * timestep)));

    if (deltaMass == 0)
        return;

    Cell* to;
    Cell* from;

    double ratio = 0;
    if (deltaMass < 0) {

        ratio = -deltaMass / mass1;
        to = &cell2;
        from = &cell1;
    } else {

        ratio = deltaMass / mass2;
        to = &cell1;
        from = &cell2;
    }
    if (!(from->type == CellType::SEA)) {
        return;
    }

    if (from->getThickness() > config.minSlickThickness && from->oilPoints.size() > 1) {

        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_real_distribution<double> dist(1.0, std::numeric_limits<double>::max());

        Vector2 vector = determineVector2(cell1, cell2, deltaMass);
        std::vector<bool> toRemove(from->oilPoints.size(), false);
        for (int i=0; i<from->oilPoints.size(); i++) {
            auto& op = from->oilPoints[i];
            if (ratio > dist(mt)) {
                op.position += (vector);
                toRemove[i] = true;
                to->oilPoints.push_back(op);
            }
        }

        std::vector<OilPoint> points{};
        for(int i=0; i<from->oilPoints.size(); i++){
            if(!toRemove[i])
                points.push_back(from->oilPoints[i]);
        }

        from->oilPoints = std::move(points);
    }
}

void SpreadingSystem::update(int timestep) {
    double volume = calculateTotalVolume();
    int row = cells.getRow();
    int col = cells.getCol();
    for (int i = 1; i < row - 1; i++) {
        for (int j = 1; j < col - 2; j += 2) {
            auto& cell1 = cells[{i, j}];
            auto& cell2 = cells[{i, j + 1}];
            updatePair(cell1, cell2, timestep, volume);
        }
        for (int j = 2; j < col - 2; j += 2) {
            auto& cell1 = cells[{i, j}];
            auto& cell2 = cells[{i, j + 1}];
            updatePair(cell1, cell2, timestep, volume);
        }
    }

    for (int j = 1; j < col - 1; j++) {
        for (int i = 1; i < row - 2; i += 2) {
            auto& cell1 = cells[{i, j}];
            auto& cell2 = cells[{i + 1, j}];
            updatePair(cell1, cell2, timestep, volume);
        }
        for (int i = 2; i < row - 2; i += 2) {
            auto& cell1 = cells[{i, j}];
            auto& cell2 = cells[{i + 1, j}];
            updatePair(cell1, cell2, timestep, volume);
        }
    }
}
