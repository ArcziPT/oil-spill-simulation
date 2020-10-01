//
// Created by arczipt on 23.08.2020.
//

#include <random>
#include <iostream>
#include "ReEntairedSystem.h"

void ReEntairedSystem::update(sycl::queue &queue,
                              sycl::buffer<Cell::Params, 1> &cellParamsBuf,
                              sycl::buffer<OilPoint::Params, 1> &opParamsBuf,
                              sycl::buffer<OilComponent, 2> &opCompBuf,
                              int timestep) {
    //mass of oil in every cell
    sycl::buffer<double, 1> oilMassBuf(sycl::range<1>(cellParamsBuf.get_size()));

    //caluclate mass of oilMass
    {
        auto opParamsI = opParamsBuf.get_access<sycl::access::mode::read>();
        auto oilMassO = oilMassBuf.get_access<sycl::access::mode::write>();

        for (int i = 0; i < opParamsBuf.get_count(); i++) {
            oilMassO[cells.id(opParamsI[i].cellPos)] += opParamsI[i].mass;
        }
    }

    sycl::buffer<bool, 2> tabBuf(sycl::range<2>(cellParamsBuf.get_count(), 4));

    queue.submit([&](sycl::handler &cgh) {
        auto cellParamsI = cellParamsBuf.get_access<sycl::access::mode::read>(cgh);
        auto opParamsIO = opParamsBuf.get_access<sycl::access::mode::read_write>(cgh);

        auto oilMassI = oilMassBuf.get_access<sycl::access::mode::read>(cgh);
        auto tabIO = tabBuf.get_access<sycl::access::mode::read_write>(cgh);

        int row = cells.getRow();
        int col = cells.getCol();

        /*
         * Function returning index of the next cell
         */
        auto id_up = [col](int i) -> int {
            return i - col;
        };
        auto id_down = [col](int i) -> int {
            return i + col;
        };
        auto id_left = [](int i) -> int {
            return i - 1;
        };
        auto id_right = [](int i) -> int {
            return i + 1;
        };

        auto equals = [](const TypeInfo &t1, const TypeInfo &t2) -> bool {
            return t1.halfTime == t2.halfTime &&
                   (t1.color.r == t2.color.r && t1.color.g == t2.color.g && t1.color.b == t2.color.b);
        };

        auto seaType = CellType::SEA;
        //For every cell's up,down,left and right neighbours
        cgh.parallel_for<class RESUpdateTab>(sycl::range<2>(cellParamsBuf.get_count(), 4), [=](sycl::id<2> i) {
            if (oilMassI[i[0]] <= 0)
                return;

            if (i[1] == 0)
                tabIO[i[0]][0] = (i[1] / row != 0) ? equals(cellParamsI[id_up(i[0])].type, seaType) : false;
            else if (i[1] == 1)
                tabIO[i[0]][1] = (i[1] % col != 0) ? equals(cellParamsI[id_right(i[0])].type, seaType) : false;
            else if (i[1] == 2)
                tabIO[i[0]][2] = (i[1] / row != row - 1) ? equals(cellParamsI[id_down(i[0])].type, seaType) : false;
            else if (i[1] == 3)
                tabIO[i[0]][3] = (i[1] % col != col - 1) ? equals(cellParamsI[id_left(i[0])].type, seaType) : false;
        });
    });

    //init random values
    sycl::buffer<int, 1> randomChoiceBuf(sycl::range<1>(opParamsBuf.get_count()));
    sycl::buffer<double, 1> randomRatioBuf(sycl::range<1>(opParamsBuf.get_count()));
    {
        std::random_device rd1;
        std::mt19937 mt1(rd1());
        std::uniform_int_distribution<int> dist1(0, 4);

        std::random_device rd2;
        std::mt19937 mt2(rd2());
        std::uniform_real_distribution<double> dist2(0, 1);

        auto randomChoiceO = randomChoiceBuf.get_access<sycl::access::mode::write>();
        auto randomRatioO = randomRatioBuf.get_access<sycl::access::mode::write>();

        for (int i = 0; i < opParamsBuf.get_count(); i++) {
            randomChoiceO[i] = dist1(mt1);
            randomRatioO[i] = dist2(mt2);
        }
    }


    {
        queue.submit([&](sycl::handler &cgh) {
            auto cellParamsI = cellParamsBuf.get_access<sycl::access::mode::read>(cgh);
            auto opParamsIO = opParamsBuf.get_access<sycl::access::mode::read_write>(cgh);

            auto oilMassI = oilMassBuf.get_access<sycl::access::mode::read>(cgh);
            auto tabI = tabBuf.get_access<sycl::access::mode::read>(cgh);
            auto randomRatioI = randomRatioBuf.get_access<sycl::access::mode::read>(cgh);
            auto randomChoiceI = randomChoiceBuf.get_access<sycl::access::mode::read>(cgh);

            int col = cells.getCol();
            auto id = [col](OilPoint::Params::CellPos pos) -> int {
                return pos.x * col + pos.y;
            };
            double log2 = std::log(2);
            auto rateConstant = [log2](const TypeInfo &t) -> double {
                return (double) (log2 / (t.halfTime * 3600));
            };
            auto addVec = [](Vector2& v1, const Vector2& v2) -> void{
                v1.x += v2.x;
                v1.y += v2.y;
            };
            auto equals = [](const TypeInfo &t1, const TypeInfo &t2) -> bool {
                return t1.halfTime == t2.halfTime &&
                       (t1.color.r == t2.color.r && t1.color.g == t2.color.g && t1.color.b == t2.color.b);
            };

            auto seaType = CellType::SEA;
            auto frameType = CellType::FRAME;

            int cellSize = config.cellSize;
            cgh.parallel_for<class RESUpdateChoice>(sycl::range<1>(opParamsBuf.get_count()), [=](sycl::id<1> i) {
                if (oilMassI[i] <= 0 || opParamsIO[i].removed)
                    return;

                auto cell_index = id(opParamsIO[i].cellPos);

                if(equals(cellParamsI[cell_index].type, seaType) || equals(cellParamsI[cell_index].type, frameType))
                    return;

                if (tabI[i[0]][0] || tabI[i[0]][1] || tabI[i[0]][2] || tabI[i[0]][3]) {
                    double ratio = rateConstant(cellParamsI[cell_index].type) * timestep;
                    if (ratio > randomRatioI[cell_index]) {
                        auto choice = randomChoiceI[i];

                        while (!tabI[i[0]][choice]) {
                            choice = (choice + 1) % 4;
                        }

                        if (choice == 0) {
                            addVec(opParamsIO[i].position, Vector2(0, -cellSize));
                        } else if (choice == 1) {
                            addVec(opParamsIO[i].position, Vector2(cellSize, 0));
                        } else if (choice == 2) {
                            addVec(opParamsIO[i].position, Vector2(0, cellSize));
                        } else if (choice == 3){
                            addVec(opParamsIO[i].position, Vector2(-cellSize, 0));
                        }
                    }
                }
            });
        });
    }
}
