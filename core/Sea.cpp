#include "Sea.h"
#include <memory>
#include "systems/SPreadingSystem.h"
#include "systems/ReEntairedSystem.h"
#include "systems/OilPointComponentSystem.h"
#include "systems/ChangeSquareSystem.h"

Sea::Sea(Configurations &config) : config(config), rows(config.rows), cols(config.cols),
                                   timeCounter(), statistics(config), cells(config)
{
    initialize();
}

std::shared_ptr<Sea> Sea::getSea() { 
	return shared_from_this(); 
}

void Sea::init(){
     systems.push_back(std::unique_ptr<OilSystem>(new SpreadingSystem(cells, config, timeCounter)));
     systems.push_back(std::unique_ptr<OilSystem>(new ReEntairedSystem(cells, config)));
     systems.push_back(std::unique_ptr<OilSystem>(new OilPointComponentsSystem(getSea(), config)));
     systems.push_back(std::unique_ptr<OilSystem>(new ChangeSquareSystem(cells, config)));

     schedulersController = std::make_shared<SchedulersController>(getSea());
 }

std::shared_ptr<SchedulersController> Sea::getSchedulersController()
{
    return schedulersController;
}

TimeCounter Sea::getTimeCounter()
{
    return timeCounter;
}

CellGrid& Sea::getCells()
{
    return cells;
}

Statistics Sea::getStatistics()
{
    return statistics;
}

void Sea::setOil(const GridValuesType<double>& array)
{
    cells.setOil(array);
}

GridValuesType<double> Sea::getOil()
{
    return cells.getOil();
}

void Sea::setTemperature(const GridValuesType<double>& array)
{
    cells.setTemperature(array);
}

void Sea::setWind(const GridValuesType<double>& array)
{
    cells.setWind(array);
}

void Sea::setCurrent(const GridValuesType<double>& array)
{
    cells.setCurrent(array);
}

void Sea::initialize()
{
    timeCounter.reset();
    cells.init(rows, cols);
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            if (i == 0 || i == rows - 1 || j == 0 || j == cols - 1)
            {
                cells.getCellParams(i,j).type = (CellType::FRAME);
            }
        }
    }
    statistics.initialize(timeCounter, cells);
}

void Sea::reset()
{
    initialize();
}

void Sea::update()
{
    if (timeCounter.totalTime > config.simulationTime)
    {
        setFinished(true);
        return;
    }

    int timestep = config.timestep;

    if (timeCounter.iteration == 0)
    {
        statistics.initialize(timeCounter, cells);
    }

    for (auto& system : systems)
    {
        system->update(timestep);
    }
    timeCounter.update(timestep);
    statistics.update(timeCounter, cells);
    schedulersController->update(timeCounter.iteration);
}

bool Sea::isFinished()
{
    return finished;
}

void Sea::setFinished(bool finished)
{
    this->finished = finished;
}

Configurations Sea::getConfig()
{
    return config;
}
