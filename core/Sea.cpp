#include "Sea.h"
#include <memory>
#include "systems/SPreadingSystem.h"
#include "systems/ReEntairedSystem.h"
#include "systems/OilPointComponentSystem.h"
#include "systems/ChangeSquareSystem.h"

Sea::Sea(Configurations &config) : config(config), rows(config.rows), cols(config.cols),
                                   timeCounter(), statistics(config)
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

void Sea::setOil(const GridValuesType& array)
{
    for (int i = 1; i < rows - 1; i++)
    {
        for (int j = 1; j < cols - 1; j++)
        {
            cells[i][j].setOil(array[i - 1][j - 1]);
        }
    }
}

GridValuesType Sea::getOil()
{
    GridValuesType array{};
    for (int i = 0; i < rows; i++)
    {
        array.push_back(std::vector<double>());
        for (int j = 0; j < cols; j++)
        {
            array[i].push_back(cells[i][j].getOil());
        }
    }
    return array;
}

void Sea::setTemperature(const GridValuesType& array)
{
    for (int i = 1; i < rows - 1; i++)
    {
        for (int j = 1; j < cols - 1; j++)
        {
            cells[i][j].temperature = (array[i - 1][j - 1]);
        }
    }
}

void Sea::setWind(const GridValuesType& array)
{
    for (int i = 1; i < rows - 1; i++)
    {
        for (int j = 1; j < cols - 1; j++)
        {
            cells[i][j].wind = (Vector2(array[i - 1][2 * j - 2], array[i - 1][2 * j - 1]));
        }
    }
}

void Sea::setCurrent(const GridValuesType& array)
{
    for (int i = 1; i < rows - 1; i++)
    {
        for (int j = 1; j < cols - 1; j++)
        {
            if (cells[i][j].type == CellType::SEA)
            {
                cells[i][j].current = (Vector2(array[i - 1][2 * j - 2], array[i - 1][2 * j - 1]));
            }
        }
    }
}

void Sea::initialize()
{
    timeCounter.reset();

    for (int i = 0; i < rows; i++)
    {
        cells.push_back(std::vector<Cell>());
        for (int j = 0; j < cols; j++)
        {
            cells[i].push_back(Cell(i, j, config));
            if (i == 0 || i == rows - 1 || j == 0 || j == cols - 1)
            {
                cells[i][j].type = (CellType::FRAME);
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
