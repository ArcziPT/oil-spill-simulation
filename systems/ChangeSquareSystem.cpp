//
// Created by arczipt on 17.08.2020.
//


#include <algorithm>
#include <iostream>

#include "ChangeSquareSystem.h"

void ChangeSquareSystem::update(int timestep) {
    int row = cells.getRow();
    int col = cells.getCol();
    for (int i = 1; i < row - 1; i++) {
        for (int j = 1; j < col - 1; j++) {
            std::vector<bool> remove(cells.size(i, j), false);

            for(int k=0; k<cells.size(i, j); k++){
                remove[k] = update(i, j, k);
            }

            //remove oilPoints, which now are not in this cell
            cells.removeOilPoints(i, j, remove);
        }
    }
}

/**
 * Return true if OilPoint should be removed.
 *
 * @param cell
 * @param i
 */
bool ChangeSquareSystem::update(int x, int y, int i) {
    auto& op = cells.getOilPointsParams(x,y)[i];
    Vector2 position = op.position;
    double posX = position.getX();
    double posY = position.getY();
    int row = x;
    int col = y;

    int newRow = (int) (posY / config.cellSize);
    int newCol = (int) (posX / config.cellSize);

    if (newRow != row || newCol != col) {
        if (newRow < cells.getRow() && newRow > 0 && newCol < cells.getCol() && newCol > 0) {
            cells.getOilPointsParams(x,y).push_back(op);
            cells.copyOilPoint(x, y, i, newRow, newCol);
        }
        return true;
    }
    return false;
}
