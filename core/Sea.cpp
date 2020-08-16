#include "Sea.h"

Sea::Sea(Configurations &config) : config(config), rows(config.rows), cols(config.cols),
                                   cells(rows, std::vector<Cell>(cols)),
                                   timeCounter(), statistics(config), schedulersController(this)
{
    initSystems();
    initialize();
}

// void Sea::initSystems(){
//     systems.add(new SpreadingSystem(cells, config, timeCounter));
//     systems.add(new ReEntairedSystem(cells, config));
//     systems.add(new OilPointComponentsSystem(this, config));
//     systems.add(new ChangeSquareSystem(cells, config));
// }

SchedulersController Sea::getSchedulersController()
{
    return schedulersController;
}

TimeCounter Sea::getTimeCounter()
{
    return timeCounter;
}

CellGrid Sea::getCells()
{
    return cells;
}

Statistics Sea::getStatistics()
{
    return statistics;
}

// void Sea::setOil(double[][] array)
// {
//     for (int i = 1; i < rows - 1; i++)
//     {
//         for (int j = 1; j < cols - 1; j++)
//         {
//             cells[i][j].setOil(array[i - 1][j - 1]);
//         }
//     }
// }

// double[][] Sea::getOil()
// {
//     double[][] array = new double[rows - 2][cols - 2];
//     for (int i = 1; i < rows - 1; i++)
//     {
//         for (int j = 1; j < cols - 1; j++)
//         {
//             array[i - 1][j - 1] = cells[i][j].getOil();
//         }
//     }
//     return array;
// }

// void Sea::setTemperature(double[][] array)
// {
//     for (int i = 1; i < rows - 1; i++)
//     {
//         for (int j = 1; j < cols - 1; j++)
//         {
//             cells[i][j].setTemperature(array[i - 1][j - 1]);
//         }
//     }
// }

// void Sea::setWind(double[][] array)
// {
//     for (int i = 1; i < rows - 1; i++)
//     {
//         for (int j = 1; j < cols - 1; j++)
//         {
//             cells[i][j].setWind(new Vector2(array[i - 1][2 * j - 2],
//                                             array[i - 1][2 * j - 1]));
//         }
//     }
// }

// void Sea::setCurrent(double[][] array)
// {
//     for (int i = 1; i < rows - 1; i++)
//     {
//         for (int j = 1; j < cols - 1; j++)
//         {
//             if (cells[i][j].getType() == CellType.SEA)
//             {
//                 cells[i][j].setCurrent(new Vector2(array[i - 1][2 * j - 2],
//                                                    array[i - 1][2 * j - 1]));
//             }
//         }
//     }
// }

void Sea::initialize()
{
    timeCounter.reset();

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {

            cells[i][j] = Cell(i, j, config);
            if (i == 0 || i == rows - 1 || j == 0 || j == cols - 1)
            {
                cells[i][j].setType(CellType::FRAME);
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
    if (timeCounter.getTotalTime() > config.getSimulationTime())
    {
        setFinished(true);
        return;
    }

    int timestep = config.getTimestep();

    if (timeCounter.getIteration() == 0)
    {
        statistics.initialize(timeCounter, cells);
    }

    for (OilSystem system : systems)
    {
        system.update(timestep);
    }
    timeCounter.update(timestep);
    statistics.update(timeCounter, cells);
    schedulersController.update(timeCounter.getIteration());
}

bool Sea::isFinished()
{
    return finished;
}

void Sea::setFinished(bool finished)
{
    this.finished = finished;
}

Configurations Sea::getConfig()
{
    return config;
}