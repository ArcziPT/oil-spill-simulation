//
// Created by arczipt on 24.08.2020.
//

#include "SPreadingSystem.h"

//double SpreadingSystem::calculateTotalVolume() {
//    double result = 0;
//    int rows = cells.getRow();
//    int cols = cells.getCol();
//
//    auto& cellsParams = cells.getCellParams();
//
//    for (int i = 1; i < rows - 1; i++) {
//        for (int j = 1; j < cols - 1; j++) {
//            result += cells.getVolume(cells.id(CellGrid::Pos(i,j)));
//        }
//    }
//    return result;
//}
//
//Vector2 SpreadingSystem::determineVector2(int x1, int y1, int x2, int y2, double deltaMass) {
//    int i1 = x1;
//    int i2 = x2;
//    double length = size;
//
//    if (deltaMass > 0) {
//        length = -length;
//    }
//    double x, y;
//    if (i1 == i2) {
//        x = length;
//        y = 0;
//    } else {
//        x = 0;
//        y = length;
//    }
//    return Vector2(x, y);
//}
//
//void SpreadingSystem::updatePair(int x1, int y1, int x2, int y2, int timestep, double volume) {
//    double mass1 = cells.getMassOfEmulsion(cells.id(CellGrid::Pos(x1, y1)));
//    double mass2 = cells.getMassOfEmulsion(cells.id(CellGrid::Pos(x2, y2)));
//
//    if (mass1 == 0 && mass2 == 0) {
//        return;
//    }
//
//    double oilDensity = (mass1 * cells.getDensity(cells.id(CellGrid::Pos(x2, y2))) + mass2 * cells.getDensity(cells.id(CellGrid::Pos(x2, y2)))) / (mass1 + mass2);
//    double temperature = (cells.getCellParams(x2, y2).temperature + cells.getCellParams(x2, y2).temperature) / 2;
//    double salinity = config.salinity;
//    double waterDensity = TemperatureDependency::calculateWaterDensity(temperature, salinity);
//    double kinematicWaterViscosity = TemperatureDependency::calculateWaterDynamicViscosity(temperature, salinity) / waterDensity;
//    double time = timeSystem.totalTime + timestep / 2.;
//    double delta = (waterDensity - oilDensity) / waterDensity;
//    double base = (g * delta * volume * volume) / (std::sqrt(kinematicWaterViscosity));
//    double diffusion = 0.49 / std::pow(config.spreadingCoefficient, 2) * std::pow(base, 1.0 / 3) * 1 / std::sqrt(time);
//
//    if(!(cells.getCellParams(x2, y2).type == CellType::SEA) && !(cells.getCellParams(x2, y2).type == CellType::SEA))
//        return;
//
//    double deltaMass = (0.5 * (mass2 - mass1) * (1 - std::exp(-2 * diffusion / (size * size) * timestep)));
//
//    if (deltaMass == 0)
//        return;
//
//    int tx, ty;
//    int fx, fy;
//
//    double ratio = 0;
//    if (deltaMass < 0) {
//
//        ratio = -deltaMass / mass1;
//        tx = x2;
//        ty = y2;
//        fx = x1;
//        fy = y1;
//    } else {
//
//        ratio = deltaMass / mass2;
//        tx = x1;
//        ty = y1;
//        fx = x2;
//        fy = y2;
//    }
//    if (!(cells.getCellParams(fx, fy).type == CellType::SEA)) {
//        return;
//    }
//
//    if (cells.getThickness(cells.id(CellGrid::Pos(fx, fy))) > config.minSlickThickness && cells.getOilPointsParams(fx, fy).size() > 1) {
//
//        std::random_device rd;
//        std::mt19937 mt(rd());
//        std::uniform_real_distribution<double> dist(1.0, std::numeric_limits<double>::max());
//
//        Vector2 vector = determineVector2(x1, y1, x2, y2, deltaMass);
//        std::vector<bool> toRemove(cells.getOilPointsParams(fx, fy).size(), false);
//        for (int i=0; i<cells.getOilPointsParams(fx, fy).size(); i++) {
//            auto& op = cells.getOilPointsParams(fx, fy)[i];
//            if (ratio > dist(mt)) {
//                op.position += (vector);
//                toRemove[i] = true;
//                cells.copyOilPoint(fx,fy, i, tx, ty);
//            }
//        }
//
//        cells.removeOilPoints(fx, fy, toRemove);
//    }
//}

void SpreadingSystem::update(sycl::queue& queue,
                             sycl::buffer<Cell::Params, 1>& cellParamsBuf,
                             sycl::buffer<OilPoint::Params, 1>& opParamsBuf,
                             sycl::buffer<OilComponent, 2>& opCompBuf,
                             int timestep) {
//    double volume = 0;
//    auto len = opParamsBuf.get_size();
//
//    auto num_groups = queue.get_device().get_info<cl::sycl::info::device::max_compute_units>();
//    // getting the maximum work group size per thread
//    auto work_group_size = queue.get_device().get_info<cl::sycl::info::device::max_work_group_size>();
//    // building the best number of global thread
//    auto total_threads = num_groups * work_group_size;
//    queue.submit([&](sycl::handler& cgh){
//
//
//        cgh.parallel_for(sycl::range<1>(total_threads, work_group_size), [=](sycl::item<1> item_id){
//            // in_size is equivalent of total number of thread
//            auto global_id = item_id.get_global_id(0);
//            auto local_id = item_id.get_local_id(0);
//            local_acc[local_id] = (global_id < len) ? in_acc[global_id] : 0;
//            for (auto i = item_id.get_local_range(0) / 2; i > 0; i >>= 1) {
//                // wait for all thread to put the data in the local memory
//                item_id.barrier(cl::sycl::access::fence_space::local_space);
//                // replacing odd threads with contiguous threads
//                if (local_id < i) local_acc[local_id] += local_acc[local_id + i];
//            }
//            if (item_id.get_local_id(0) == 0) {
//                out_acc[item_id.get_group(0)] = local_acc[0];
//            }
//        });
//    });

//    double volume = calculateTotalVolume();
//    int row = cells.getRow();
//    int col = cells.getCol();
//    for (int i = 1; i < row - 1; i++) {
//        for (int j = 1; j < col - 2; j += 2) {
//            updatePair(i , j, i, j+1, timestep, volume);
//        }
//        for (int j = 2; j < col - 2; j += 2) {
//            updatePair(i , j, i, j+1, timestep, volume);
//        }
//    }
//
//    for (int j = 1; j < col - 1; j++) {
//        for (int i = 1; i < row - 2; i += 2) {
//            updatePair(i , j, i+1, j, timestep, volume);
//        }
//        for (int i = 2; i < row - 2; i += 2) {
//            updatePair(i , j, i+1, j, timestep, volume);
//        }
//    }
}
