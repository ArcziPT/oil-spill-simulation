#ifndef SEA_H
#define SEA_H

#include "Cell.h"
#include "Configurations.h"

#include <memory>
#include <vector>
#include <schedulers/SchedulersController.h>

using CellGrid = std::vector<std::vector<Cell>>;
typedef std::vector<std::vector<double>> GridValuesType;

class Sea
{
public:
    Sea(Configurations &config);

    SchedulersController getSchedulersController();
    TimeCounter getTimeCounter();
    CellGrid getCells();
    Statistics getStatistics();

    //void setOil(double[][] array);
    //double[][] getOil();

    // void setTemperature(double[][] array);
    // void setWind(double[][] array);
    // void setCurrent(double[][] array);

    void initialize();
    void reset();
    void update();

    bool isFinished();
    void setFinished(bool finished);

    Configurations getConfig();

private:
    CellGrid cells;
    Configurations &config;
    TimeCounter timeCounter;
    Statistics statistics;
    //Set<OilSystem> systems = new LinkedHashSet<>();
    SchedulersController schedulersController;
    int cols;
    int rows;
    bool finished = false;

    void initSystems();
};

typedef shared_ptr<Sea> SeaPtr;

#endif