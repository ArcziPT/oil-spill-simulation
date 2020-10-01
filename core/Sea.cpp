#include "Sea.h"
#include <memory>
#include "systems/SPreadingSystem.h"
#include "systems/ReEntairedSystem.h"
#include "systems/OilPointComponentSystem.h"
#include "systems/ChangeSquareSystem.h"

Sea::Sea(Configurations &config) : config(config), rows(config.rows), cols(config.cols),
                                   timeCounter(), cells(config)//, statistics(config)
{
    initialize();
}

std::shared_ptr<Sea> Sea::getSea() { 
	return shared_from_this(); 
}

void Sea::init(){
    //init systems and schedulers
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

//Statistics Sea::getStatistics()
//{
//    return statistics;
//}

void Sea::setOil(const GridValuesType<double>& array)
{
    cells.setOil(array);
}

GridValuesType<double> Sea::getOil()
{
    componentBufferPtr.reset(nullptr);
    opBufferPtr.reset(nullptr);

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
    cells.init(rows, cols, config.oilComponents.size());

    auto& cellParams = cells.getCellParams();
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            if (i == 0 || i == rows - 1 || j == 0 || j == cols - 1)
            {
                int id = cells.id(OilPoint::Params::CellPos(i, j));
                cellParams[id].type = (CellType::FRAME);
            }
        }
    }
    //statistics.initialize(timeCounter, cells);
}

void Sea::reset()
{
    initialize();
}

void Sea::update()
{
    //if there is an update destroy right buffer (make sycl write data back to origin) and push new data
    auto subjects = schedulersController->getUpdateSubjects(timeCounter.iteration);
    if(!subjects.empty()){
        //destroy buffers
        for(auto& subject : subjects){
            switch (subject) {
                case UpdateSubject::OIL:
                    std::cout<<"Update OP\n";
                    opBufferPtr.reset(nullptr);
                    componentBufferPtr.reset(nullptr);
                    break;
                case UpdateSubject::WIND:
                case UpdateSubject::TEMP:
                case UpdateSubject::CURRENT:
                    std::cout<<"Update Params\n";
                    cellBufferPtr.reset(nullptr);
                    break;
            }
        }

        //update
        schedulersController->update(timeCounter.iteration);
    }

    createCellBuffer();
    createOpBuffer();
    createCompBuffer();
    createQueue();

    if (timeCounter.totalTime > config.simulationTime)
    {
        setFinished(true);

        opBufferPtr.reset(nullptr);
        cellBufferPtr.reset(nullptr);
        componentBufferPtr.reset(nullptr);
        queuePtr.reset(nullptr);

        return;
    }

    int timestep = config.timestep;

    if (timeCounter.iteration == 0)
    {
        //statistics.initialize(timeCounter, cells);
    }

    for (auto& system : systems)
    {
        system->update(*queuePtr, *cellBufferPtr, *opBufferPtr, *componentBufferPtr, timestep);
        queuePtr->wait();
    }
    timeCounter.update(timestep);
    //statistics.update(timeCounter, cells);
}

void Sea::createCellBuffer(){
    if(cellBufferPtr != nullptr)
        return;

    auto& cellParams = cells.getCellParams();
    cellBufferPtr = CellBufferPtr(new sycl::buffer<Cell::Params, 1>(cellParams.data(), sycl::range<1>(cellParams.size())));
}

void Sea::createOpBuffer(){
    if(opBufferPtr != nullptr)
        return;

    auto& opParams = cells.getOilPointsParams();
    opBufferPtr = OpBufferPtr(new sycl::buffer<OilPoint::Params, 1>(opParams.data(), sycl::range<1>(opParams.size())));
}

void Sea::createCompBuffer(){
    if(componentBufferPtr != nullptr)
        return;

    auto& opComponents = cells.getOilPointsComponents();
    componentBufferPtr = ComponentBufferPtr(new sycl::buffer<OilComponent, 2>(opComponents.data(), sycl::range<2>(opComponents.size()/config.oilComponents.size(), config.oilComponents.size())));
}

void Sea::createQueue() {
    if(queuePtr != nullptr)
        return;

    queuePtr = QueuePtr(new sycl::queue);
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
