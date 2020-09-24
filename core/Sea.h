#ifndef SEA_H
#define SEA_H

#include "Cell.h"
#include "Configurations.h"

#include <memory>
#include <vector>
#include <vector>
#include "systems/TimeCounter.h"
//#include "statistics/Statistics.h"
#include "schedulers/SchedulersController.h"
#include "systems/OilSystem.h"

class SchedulersController;

/**
 * Before using call init()
 */
class Sea : public std::enable_shared_from_this<Sea>
{
public:
    Sea(Configurations &config);
    void init();
    
    std::shared_ptr<Sea> getSea();

    std::shared_ptr<SchedulersController> getSchedulersController();
    TimeCounter getTimeCounter();
    CellGrid& getCells();
    //Statistics getStatistics();

    void setOil(const GridValuesType<double>& array);
    GridValuesType<double> getOil();

    void setTemperature(const GridValuesType<double>& array);
    void setWind(const GridValuesType<double >& array);
    void setCurrent(const GridValuesType<double >& array);

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
    //Statistics statistics;
    std::vector<std::unique_ptr<OilSystem>> systems;
    std::shared_ptr<SchedulersController> schedulersController;
    int cols;
    int rows;
    bool finished = false;
};

#endif
