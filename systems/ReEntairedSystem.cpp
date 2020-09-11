//
// Created by arczipt on 23.08.2020.
//

#include <random>
#include "ReEntairedSystem.h"

void ReEntairedSystem::update(int timestep) {
    int row = cells.size();
    int col = cells[0].size();
    for (int i = 1; i < row - 1; i++) {

        for (int j = 1; j < col - 1; j++) {
            auto &cell = cells[i][j];
            updateCell(cell, timestep);
        }
    }
}

void ReEntairedSystem::updateCell(Cell &cell, int timestep) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist(1.0, std::numeric_limits<double>::max());

    TypeInfo type = cell.type;
    if (!(type == CellType::SEA) && !(type == CellType::FRAME)) {
        int row = cell.row;
        int col = cell.col;
        double mass = cell.getOil();
        auto& ops = cell.oilPoints;
        if (mass > 0) {
            bool tab[4];
            for (bool & i : tab) {
                i = false;
            }

            int count = 0;
            if (cells[row - 1][col].type == CellType::SEA) {
                tab[0] = true;
                count++;
            }
            if (cells[row][col + 1].type == CellType::SEA) {
                tab[1] = true;
                count++;
            }
            if (cells[row + 1][col].type == CellType::SEA) {
                tab[2] = true;
                count++;
            }
            if (cells[row][col - 1].type == CellType::SEA) {
                tab[3] = true;
                count++;
            }

            if (count > 0) {
                double ratio = type.rateConstant() * timestep;
                for (auto& op : ops) {
                    if (ratio > dist(mt)) {
                        while (true) {
                            int choice = (int)dist(mt) % 4;
                            if (tab[choice]) {
                                if (choice == 0) {
                                    op.position += Vector2(0, -config.cellSize);
                                } else if (choice == 1) {
                                    op.position += Vector2(config.cellSize, 0);
                                } else if (choice == 2) {
                                    op.position += Vector2(0, config.cellSize);
                                } else {
                                    op.position += Vector2(-config.cellSize, 0);
                                }
                                break;
                            }
                        }

                    }
                }
            }
        }
    }
