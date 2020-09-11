//
// Created by arczipt on 17.08.2020.
//


#include <algorithm>

#include "ChangeSquareSystem.h"

void ChangeSquareSystem::update(int timestep) {
    int row = cells.size();
    int col = cells[0].size();
    for (int i = 1; i < row - 1; i++) {
        for (int j = 1; j < col - 1; j++) {
            auto& cell = cells[i][j];
            std::vector<bool> remove(cell.oilPoints.size(), false);

            for(int k=0; k<cell.oilPoints.size(); k++){
                remove[k] = update(cell, k);
            }

            //remove oilPoints, which now are not in this cell
            std::vector<OilPoint> newPoints{};
            for(int k=0; k<cell.oilPoints.size(); k++){
                if(remove[k])
                    continue;

                newPoints.push_back(cell.oilPoints[k]);
            }

            cell.oilPoints = newPoints;
        }
    }
}

/**
 * Return true if OilPoint should be removed.
 *
 * @param cell
 * @param i
 */
bool ChangeSquareSystem::update(Cell &cell, int i) {
    auto& op = cell.oilPoints[i];
    Vector2 position = op.position;
    double x = position.getX();
    double y = position.getY();
    int row = cell.row;
    int col = cell.col;

    int newRow = (int) (y / config.cellSize);
    int newCol = (int) (x / config.cellSize);

    if (newRow != row || newCol != col) {
        if (newRow < cells.size() && newRow > 0 && newCol < cells[0].size() && newCol > 0) {
            cells[newRow][newCol].oilPoints.push_back(op);
        }

        return true;
    }

    return false;
}
