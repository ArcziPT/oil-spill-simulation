//
// Created by arczipt on 24.08.2020.
//

#include <iostream>
#include "SPreadingSystem.h"

struct data{
    int ti;
    int fi;
    double ratio;
    double deltaMass;

    data(int ti, int fi, double ratio, double deltaMass): ti(ti), fi(fi), ratio(ratio), deltaMass(deltaMass) {}
};

//TODO: refactor
void SpreadingSystem::update(sycl::queue& queue,
                             sycl::buffer<Cell::Params, 1>& cellParamsBuf,
                             sycl::buffer<OilPoint::Params, 1>& opParamsBuf,
                             sycl::buffer<OilComponent, 2>& opCompBuf,
                             int timestep) {
    //mass of emulsion in every cell
    sycl::buffer<double, 1> massOfEmulsionBuf(sycl::range<1>(cellParamsBuf.get_count()));
    //oil density for every cell
    sycl::buffer<double, 1> oilDensityBuf(sycl::range<1>(cellParamsBuf.get_count()));
    //volume of emulsion in every cell
    sycl::buffer<double, 1> volumeBuf(sycl::range<1>(cellParamsBuf.get_count()));
    //random ratio for every oil point used later
    sycl::buffer<double, 1> randomRatioBuf(sycl::range<1>(opParamsBuf.get_count()));
    double volume = 0;

    //Calculate massOfEmulsion, oilDensity and init randomRatio, because porting to GPU code is not that simple
    {
        auto massOfEmulsionO = massOfEmulsionBuf.get_access<sycl::access::mode::write>();
        auto oilDensityO = oilDensityBuf.get_access<sycl::access::mode::write>();
        auto volumeO = volumeBuf.get_access<sycl::access::mode::write>();
        auto randomRatioO = randomRatioBuf.get_access<sycl::access::mode::write>();
        auto opParamsI = opParamsBuf.get_access<sycl::access::mode::read>();

        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_real_distribution<double> dist(1.0, std::numeric_limits<double>::max());

        for(int i=0; i<opParamsBuf.get_count(); i++){
            massOfEmulsionO[cells.id(opParamsI[i].cellPos)] += opParamsI[i].massOfEmulsion;
            oilDensityO[cells.id(opParamsI[i].cellPos)] += opParamsI[i].density * opParamsI[i].massOfEmulsion;
            randomRatioO[i] = dist(mt);
        }
        for(int i=0; i<cellParamsBuf.get_count(); i++){
            oilDensityO[i] = oilDensityO[i] / massOfEmulsionO[i];
            volumeO[i] = massOfEmulsionO[i] / oilDensityO[i];
            volume += volumeO[i];
        }
    }

    //temporary
    sycl::buffer<data, 1> dataBuf(sycl::range<1>(cellParamsBuf.get_count()));

    //functions used on gpu cores
    auto calcWaterDynamicViscosity = [](double T, double S) -> double{
        S = S / 1000;
        T -= 273.15;

        double a[] = {
                0.0,
                1.5700386464E-01,
                6.4992620050E+01,
                -9.1296496657E+01,
                4.2844324477E-05,
                1.5409136040E+00,
                1.9981117208E-02,
                -9.5203865864E-05,
                7.9739318223E+00,
                -7.5614568881E-02,
                4.7237011074E-04,
        };

        double mu_w = a[4] + 1.0 / (a[1] * sycl::pow((T + a[2]), 2.0) + a[3]);


        double A = a[5] + a[6] * T + a[7] * T * T;
        double B = a[8] + a[9] * T + a[10] * T * T;
        double mu = mu_w * (1 + A * S + B * S * S);

        return mu;
    };

    auto calculateWaterDensity = [](double temp, double conc) -> double{
        // salinity in mg/L
        temp = temp - 273.15;
        double rho = 1000 * (1.0 - (temp + 288.9414)
                                   / (508929.2 * (temp + 68.12963)) * (sycl::pow(temp - 3.9863, 2.0)));

        double rhos, A, B;
        A = 0.824493 - 0.0040899 * temp + 0.000076438 * sycl::pow(temp, 2.0)
            - 0.00000082467 * sycl::pow(temp, 3.0) + 0.0000000053675
                                                  * sycl::pow(temp, 4.0);
        B = -0.005724 + 0.00010227 * temp - 0.0000016546 * sycl::pow(temp, 2.0);
        rhos = rho + A * conc + B * sycl::pow(conc, 3.0/2.0) + 0.00048314
                                                              * sycl::pow(conc, 2.0);
        return rhos;
    };

    double time = timeSystem.totalTime + timestep/ 2;
    /*
     * TODO: refactor
     * For every direction (right and down) submit two jobs:
     *  - 0->1 2->3 4->5 .... and op update
     *  - wait to finish
     *  - 1->2 3->4 5->6 .... and op update
     *  - wait to finish
     */

    /**
     * dx=1
     * dy=0
     * s=0
     */
     try {
         queue.submit([&](sycl::handler &cgh) {
             auto cellParamsI = cellParamsBuf.get_access<sycl::access::mode::read>(cgh);
             auto oilDensityI = oilDensityBuf.get_access<sycl::access::mode::read>(cgh);
             auto massOfEmulsionI = massOfEmulsionBuf.get_access<sycl::access::mode::read>(cgh);
             auto dataO = dataBuf.get_access<sycl::access::mode::write>(cgh);

             auto equals = [](const TypeInfo &t1, const TypeInfo &t2) -> bool {
                 return t1.halfTime == t2.halfTime &&
                        (t1.color.r == t2.color.r && t1.color.g == t2.color.g && t1.color.b == t2.color.b);
             };

             int cols = cells.getCol();
             int rows = cells.getRow();

             auto seaType = CellType::SEA;
             double salinity = config.salinity;
             double cellSize = config.cellSize;
             double spreadingCoefficient = config.spreadingCoefficient;

             cgh.parallel_for<class SSCalc1>(sycl::range<1>(cellParamsBuf.get_count()), [=](sycl::id<1> i) {
                 auto &cell = cellParamsI[i];

                 if (cell.row == rows - 1 || cell.row % 2 != 0)
                     return;

                 //second cell
                 auto n_i = i[0] + cols;
                 auto &cell2 = cellParamsI[n_i];

                 double mass1 = massOfEmulsionI[i];
                 double mass2 = massOfEmulsionI[n_i];

                 if (mass1 == 0 || mass2 == 0)
                     return;

                 if (equals(cell.type, seaType) && equals(cell2.type, seaType))
                     return;

                 double oilDensity = (mass1 * oilDensityI[i] + mass2 * oilDensityI[n_i]) / (mass1 + mass2);
                 double temperature = (cell.temperature + cell2.temperature) / 2;
                 double waterDensity = calculateWaterDensity(temperature, salinity);
                 double kinematicWaterViscosity = calcWaterDynamicViscosity(temperature, salinity) / waterDensity;
                 double delta = (waterDensity - oilDensity) / waterDensity;
                 double base = (g * delta * volume * volume) / (sycl::sqrt(kinematicWaterViscosity));
                 double diffusion =
                         0.49 / sycl::pow(spreadingCoefficient, 2.0) * sycl::pow(base, 1.0 / 3) * 1 / sycl::sqrt(time);

                 double deltaMass = (0.5 * (mass2 - mass1) *
                                     (1 - sycl::exp(-2 * diffusion / (cellSize * cellSize) * timestep)));

                 if (deltaMass == 0)
                     return;

                 double ratio = (deltaMass < 0) * (-deltaMass / mass1) + (deltaMass > 0) * (deltaMass / mass2);
                 int ti = (deltaMass < 0) * (n_i) + (deltaMass > 0) * (i[0]);
                 int fi = (deltaMass < 0) * (i[0]) + (deltaMass > 0) * (n_i);

                 dataO[i] = data(fi, ti, ratio, deltaMass);
             });
         });
         queue.wait();
         /**
          * Update OP
          */
         queue.submit([&](sycl::handler &cgh) {
             auto cellParamsI = cellParamsBuf.get_access<sycl::access::mode::read>(cgh);
             auto opParamsIO = opParamsBuf.get_access<sycl::access::mode::read_write>(cgh);
             auto oilDensityI = oilDensityBuf.get_access<sycl::access::mode::read>(cgh);
             auto massOfEmulsionI = massOfEmulsionBuf.get_access<sycl::access::mode::read>(cgh);
             auto dataI = dataBuf.get_access<sycl::access::mode::read>(cgh);
             auto volumeI = volumeBuf.get_access<sycl::access::mode::read>(cgh);
             auto randomRatioI = randomRatioBuf.get_access<sycl::access::mode::read>(cgh);

             int cols = cells.getCol();
             int rows = cells.getRow();
             auto id = [cols](OilPoint::Params::CellPos pos) -> int {
                 return pos.x * cols + pos.y;
             };
             auto posFromId = [cols](int i) -> OilPoint::Params::CellPos {
                 return OilPoint::Params::CellPos(i / cols, i % cols);
             };

             auto seaType = CellType::SEA;
             auto equals = [](const TypeInfo &t1, const TypeInfo &t2) -> bool {
                 return t1.halfTime == t2.halfTime &&
                        (t1.color.r == t2.color.r && t1.color.g == t2.color.g && t1.color.b == t2.color.b);
             };

             auto addVec = [](Vector2 &v1, const Vector2 &v2) -> void {
                 v1.x += v2.x;
                 v1.y += v2.y;
             };

             auto cellSize = config.cellSize;
             auto minSlickThickness = config.minSlickThickness;
             cgh.parallel_for<class SSUpdate1>(sycl::range<1>(opParamsBuf.get_count()), [=](sycl::id<1> i) {
                 auto &op = opParamsIO[i];
                 if (op.removed)
                     return;

                 auto cell_i = id(op.cellPos);
                 auto &cell = cellParamsI[cell_i];
                 if (cell.row == rows - 1 || cell.row % 2 != 0 || equals(cell.type, seaType))
                     return;

                 auto thickness = volumeI[cell_i] / (cellSize * cellSize);
                 auto deltaMass = dataI[cell_i].deltaMass;
                 if (thickness > minSlickThickness) {
                     Vector2 v1(0, (deltaMass > 0) * (-cellSize) + (deltaMass < 0) * (cellSize));
                     if (dataI[cell_i].ratio > randomRatioI[i]) {
                         addVec(op.position, v1);
                         op.cellPos = posFromId(dataI[cell_i].ti);
                     }
                 }
             });
         });
     } catch (sycl::exception& e) {
        std::cout<<e.get_description();
        exit(1);
     }
    queue.wait();

    /**
     * dx=1
     * dy=0
     * s=1
     */
    queue.submit([&](sycl::handler& cgh){
        auto cellParamsI = cellParamsBuf.get_access<sycl::access::mode::read>(cgh);
        auto oilDensityI = oilDensityBuf.get_access<sycl::access::mode::read>(cgh);
        auto massOfEmulsionI = massOfEmulsionBuf.get_access<sycl::access::mode::read>(cgh);
        auto dataO = dataBuf.get_access<sycl::access::mode::write>(cgh);

        auto equals = [](const TypeInfo &t1, const TypeInfo &t2) -> bool {
            return t1.halfTime == t2.halfTime &&
                   (t1.color.r == t2.color.r && t1.color.g == t2.color.g && t1.color.b == t2.color.b);
        };

        int cols = cells.getCol();
        int rows = cells.getRow();

        auto seaType = CellType::SEA;
        double salinity = config.salinity;
        double cellSize = config.cellSize;
        double spreadingCoefficient = config.spreadingCoefficient;

        cgh.parallel_for<class SSCalc2>(sycl::range<1>(cellParamsBuf.get_count()), [=](sycl::id<1> i){
            auto& cell = cellParamsI[i];

            if(cell.row == rows-1 || cell.row%2 != 1)
                return;

            auto n_i = i[0] + cols;
            auto& cell2 = cellParamsI[n_i];

            double mass1 = massOfEmulsionI[i];
            double mass2 = massOfEmulsionI[n_i];

            if (mass1 == 0 || mass2 == 0)
                return;

            if (equals(cell.type, seaType) && equals(cell2.type, seaType))
                return;

            double oilDensity = (mass1 * oilDensityI[i] + mass2 * oilDensityI[n_i]) / (mass1 + mass2);
            double temperature = (cell.temperature + cell2.temperature) / 2;
            double waterDensity = calculateWaterDensity(temperature, salinity);
            double kinematicWaterViscosity = calcWaterDynamicViscosity(temperature, salinity) / waterDensity;
            double delta = (waterDensity - oilDensity) / waterDensity;
            double base = (g * delta * volume * volume) / (sycl::sqrt(kinematicWaterViscosity));
            double diffusion = 0.49 / sycl::pow(spreadingCoefficient, 2.0) * sycl::pow(base, 1.0 / 3) * 1 / sycl::sqrt(time);

            double deltaMass = (0.5 * (mass2 - mass1) * (1 - sycl::exp(-2 * diffusion / (cellSize * cellSize) * timestep)));

            if (deltaMass == 0)
                return;

            double ratio = (deltaMass < 0)*(-deltaMass/mass1) + (deltaMass>0)*(deltaMass/mass2);
            int ti = (deltaMass < 0)*(n_i) + (deltaMass>0)*(i[0]);
            int fi = (deltaMass < 0)*(i[0]) + (deltaMass>0)*(n_i);

            dataO[i] = data(fi, ti, ratio, deltaMass);
        });
    });
    queue.wait();
    /**
     * Update OP
     */
    queue.submit([&](sycl::handler& cgh){
        auto cellParamsI = cellParamsBuf.get_access<sycl::access::mode::read>(cgh);
        auto opParamsIO = opParamsBuf.get_access<sycl::access::mode::read_write>(cgh);
        auto oilDensityI = oilDensityBuf.get_access<sycl::access::mode::read>(cgh);
        auto massOfEmulsionI = massOfEmulsionBuf.get_access<sycl::access::mode::read>(cgh);
        auto dataI = dataBuf.get_access<sycl::access::mode::read>(cgh);
        auto volumeI = volumeBuf.get_access<sycl::access::mode::read>(cgh);
        auto randomRatioI = randomRatioBuf.get_access<sycl::access::mode::read>(cgh);

        int cols = cells.getCol();
        int rows = cells.getRow();
        auto id = [cols](OilPoint::Params::CellPos pos) -> int {
            return pos.x * cols + pos.y;
        };
        auto posFromId = [cols](int i) -> OilPoint::Params::CellPos{
            return OilPoint::Params::CellPos(i/cols, i%cols);
        };

        auto seaType = CellType::SEA;
        auto equals = [](const TypeInfo &t1, const TypeInfo &t2) -> bool {
            return t1.halfTime == t2.halfTime &&
                   (t1.color.r == t2.color.r && t1.color.g == t2.color.g && t1.color.b == t2.color.b);
        };

        auto addVec = [](Vector2& v1, const Vector2& v2) -> void{
            v1.x += v2.x;
            v1.y += v2.y;
        };

        auto cellSize = config.cellSize;
        auto minSlickThickness = config.minSlickThickness;
        cgh.parallel_for<class SSUpdate2>(sycl::range<1>(opParamsBuf.get_count()), [=](sycl::id<1> i){
            auto& op = opParamsIO[i];
            if(op.removed)
                return;

            auto cell_i = id(op.cellPos);
            auto& cell = cellParamsI[cell_i];
            if(cell.row == rows-1 || cell.row%2 != 1 || equals(cell.type, seaType))
                return;

            auto thickness = volumeI[cell_i] / (cellSize * cellSize);
            auto deltaMass = dataI[cell_i].deltaMass;
            if (thickness > minSlickThickness) {
                Vector2 v1(0, (deltaMass>0)*(-cellSize) + (deltaMass<0)*(cellSize));
                if (dataI[cell_i].ratio > randomRatioI[i]) {
                    addVec(op.position, v1);
                    op.cellPos = posFromId(dataI[cell_i].ti);
                }
            }
        });
    });
    queue.wait();

    /**
     * dx=0
     * dy=1
     * s=0
     */
    queue.submit([&](sycl::handler& cgh){
        auto cellParamsI = cellParamsBuf.get_access<sycl::access::mode::read>(cgh);
        auto oilDensityI = oilDensityBuf.get_access<sycl::access::mode::read>(cgh);
        auto massOfEmulsionI = massOfEmulsionBuf.get_access<sycl::access::mode::read>(cgh);
        auto dataO = dataBuf.get_access<sycl::access::mode::write>(cgh);

        auto equals = [](const TypeInfo &t1, const TypeInfo &t2) -> bool {
            return t1.halfTime == t2.halfTime &&
                   (t1.color.r == t2.color.r && t1.color.g == t2.color.g && t1.color.b == t2.color.b);
        };

        int cols = cells.getCol();
        int rows = cells.getRow();

        auto seaType = CellType::SEA;
        double salinity = config.salinity;
        double cellSize = config.cellSize;
        double spreadingCoefficient = config.spreadingCoefficient;

        cgh.parallel_for<class SSCalc3>(sycl::range<1>(cellParamsBuf.get_count()), [=](sycl::id<1> i){
            auto& cell = cellParamsI[i];

            if(cell.col == cols-1 || cell.col%2 != 0)
                return;

            auto n_i = i[0] + 1;
            auto& cell2 = cellParamsI[n_i];

            double mass1 = massOfEmulsionI[i];
            double mass2 = massOfEmulsionI[n_i];

            if (mass1 == 0 || mass2 == 0)
                return;

            if (equals(cell.type, seaType) && equals(cell2.type, seaType))
                return;

            double oilDensity = (mass1 * oilDensityI[i] + mass2 * oilDensityI[n_i]) / (mass1 + mass2);
            double temperature = (cell.temperature + cell2.temperature) / 2;
            double waterDensity = calculateWaterDensity(temperature, salinity);
            double kinematicWaterViscosity = calcWaterDynamicViscosity(temperature, salinity) / waterDensity;
            double delta = (waterDensity - oilDensity) / waterDensity;
            double base = (g * delta * volume * volume) / (sycl::sqrt(kinematicWaterViscosity));
            double diffusion = 0.49 / sycl::pow(spreadingCoefficient, 2.0) * sycl::pow(base, 1.0 / 3) * 1 / sycl::sqrt(time);

            double deltaMass = (0.5 * (mass2 - mass1) * (1 - sycl::exp(-2 * diffusion / (cellSize * cellSize) * timestep)));

            if (deltaMass == 0)
                return;

            double ratio = (deltaMass < 0)*(-deltaMass/mass1) + (deltaMass>0)*(deltaMass/mass2);
            int ti = (deltaMass < 0)*(n_i) + (deltaMass>0)*(i[0]);
            int fi = (deltaMass < 0)*(i[0]) + (deltaMass>0)*(n_i);

            dataO[i] = data(fi, ti, ratio, deltaMass);
        });
    });
    queue.wait();
    /**
     * Update OP
     */
    queue.submit([&](sycl::handler& cgh){
        auto cellParamsI = cellParamsBuf.get_access<sycl::access::mode::read>(cgh);
        auto opParamsIO = opParamsBuf.get_access<sycl::access::mode::read_write>(cgh);
        auto oilDensityI = oilDensityBuf.get_access<sycl::access::mode::read>(cgh);
        auto massOfEmulsionI = massOfEmulsionBuf.get_access<sycl::access::mode::read>(cgh);
        auto dataI = dataBuf.get_access<sycl::access::mode::read>(cgh);
        auto volumeI = volumeBuf.get_access<sycl::access::mode::read>(cgh);
        auto randomRatioI = randomRatioBuf.get_access<sycl::access::mode::read>(cgh);

        int cols = cells.getCol();
        int rows = cells.getRow();
        auto id = [cols](OilPoint::Params::CellPos pos) -> int {
            return pos.x * cols + pos.y;
        };
        auto posFromId = [cols](int i) -> OilPoint::Params::CellPos{
            return OilPoint::Params::CellPos(i/cols, i%cols);
        };

        auto seaType = CellType::SEA;
        auto equals = [](const TypeInfo &t1, const TypeInfo &t2) -> bool {
            return t1.halfTime == t2.halfTime &&
                   (t1.color.r == t2.color.r && t1.color.g == t2.color.g && t1.color.b == t2.color.b);
        };

        auto addVec = [](Vector2& v1, const Vector2& v2) -> void{
            v1.x += v2.x;
            v1.y += v2.y;
        };

        auto cellSize = config.cellSize;
        auto minSlickThickness = config.minSlickThickness;
        cgh.parallel_for<class SSUpdate3>(sycl::range<1>(opParamsBuf.get_count()), [=](sycl::id<1> i){
            auto& op = opParamsIO[i];
            if(op.removed)
                return;

            auto cell_i = id(op.cellPos);
            auto& cell = cellParamsI[cell_i];
            if(cell.col == cols-1 || cell.col%2 != 0 || equals(cell.type, seaType))
                return;

            auto thickness = volumeI[cell_i] / (cellSize * cellSize);
            auto deltaMass = dataI[cell_i].deltaMass;
            if (thickness > minSlickThickness) {
                Vector2 v1(0, (deltaMass>0)*(-cellSize) + (deltaMass<0)*(cellSize));
                if (dataI[cell_i].ratio > randomRatioI[i]) {
                    addVec(op.position, v1);
                    op.cellPos = posFromId(dataI[cell_i].ti);
                }
            }
        });
    });
    queue.wait();

    /**
     * dx=1
     * dy=0
     * s=0
     */
    queue.submit([&](sycl::handler& cgh){
        auto cellParamsI = cellParamsBuf.get_access<sycl::access::mode::read>(cgh);
        auto oilDensityI = oilDensityBuf.get_access<sycl::access::mode::read>(cgh);
        auto massOfEmulsionI = massOfEmulsionBuf.get_access<sycl::access::mode::read>(cgh);
        auto dataO = dataBuf.get_access<sycl::access::mode::write>(cgh);

        auto equals = [](const TypeInfo &t1, const TypeInfo &t2) -> bool {
            return t1.halfTime == t2.halfTime &&
                   (t1.color.r == t2.color.r && t1.color.g == t2.color.g && t1.color.b == t2.color.b);
        };

        int cols = cells.getCol();
        int rows = cells.getRow();

        auto seaType = CellType::SEA;
        double salinity = config.salinity;
        double cellSize = config.cellSize;
        double spreadingCoefficient = config.spreadingCoefficient;

        cgh.parallel_for<class SSCalc4>(sycl::range<1>(cellParamsBuf.get_count()), [=](sycl::id<1> i){
            auto& cell = cellParamsI[i];

            if(cell.col == cols-1 || cell.col%2 != 1)
                return;

            auto n_i = i[0] + cols;
            auto& cell2 = cellParamsI[n_i];

            double mass1 = massOfEmulsionI[i];
            double mass2 = massOfEmulsionI[n_i];

            if (mass1 == 0 || mass2 == 0)
                return;

            if (equals(cell.type, seaType) && equals(cell2.type, seaType))
                return;

            double oilDensity = (mass1 * oilDensityI[i] + mass2 * oilDensityI[n_i]) / (mass1 + mass2);
            double temperature = (cell.temperature + cell2.temperature) / 2;
            double waterDensity = calculateWaterDensity(temperature, salinity);
            double kinematicWaterViscosity = calcWaterDynamicViscosity(temperature, salinity) / waterDensity;
            double delta = (waterDensity - oilDensity) / waterDensity;
            double base = (g * delta * volume * volume) / (sycl::sqrt(kinematicWaterViscosity));
            double diffusion = 0.49 / sycl::pow(spreadingCoefficient, 2.0) * sycl::pow(base, 1.0 / 3) * 1 / sycl::sqrt(time);

            double deltaMass = (0.5 * (mass2 - mass1) * (1 - sycl::exp(-2 * diffusion / (cellSize * cellSize) * timestep)));

            if (deltaMass == 0)
                return;

            double ratio = (deltaMass < 0)*(-deltaMass/mass1) + (deltaMass>0)*(deltaMass/mass2);
            int ti = (deltaMass < 0)*(n_i) + (deltaMass>0)*(i[0]);
            int fi = (deltaMass < 0)*(i[0]) + (deltaMass>0)*(n_i);

            dataO[i] = data(fi, ti, ratio, deltaMass);
        });
    });
    queue.wait();
    /**
     * Update OP
     */
    queue.submit([&](sycl::handler& cgh){
        auto cellParamsI = cellParamsBuf.get_access<sycl::access::mode::read>(cgh);
        auto opParamsIO = opParamsBuf.get_access<sycl::access::mode::read_write>(cgh);
        auto oilDensityI = oilDensityBuf.get_access<sycl::access::mode::read>(cgh);
        auto massOfEmulsionI = massOfEmulsionBuf.get_access<sycl::access::mode::read>(cgh);
        auto dataI = dataBuf.get_access<sycl::access::mode::read>(cgh);
        auto volumeI = volumeBuf.get_access<sycl::access::mode::read>(cgh);
        auto randomRatioI = randomRatioBuf.get_access<sycl::access::mode::read>(cgh);

        int cols = cells.getCol();
        int rows = cells.getRow();
        auto id = [cols](OilPoint::Params::CellPos pos) -> int {
            return pos.x * cols + pos.y;
        };
        auto posFromId = [cols](int i) -> OilPoint::Params::CellPos{
            return OilPoint::Params::CellPos(i/cols, i%cols);
        };

        auto seaType = CellType::SEA;
        auto equals = [](const TypeInfo &t1, const TypeInfo &t2) -> bool {
            return t1.halfTime == t2.halfTime &&
                   (t1.color.r == t2.color.r && t1.color.g == t2.color.g && t1.color.b == t2.color.b);
        };

        auto addVec = [](Vector2& v1, const Vector2& v2) -> void{
            v1.x += v2.x;
            v1.y += v2.y;
        };

        auto cellSize = config.cellSize;
        auto minSlickThickness = config.minSlickThickness;
        cgh.parallel_for<class SSUpdate4>(sycl::range<1>(opParamsBuf.get_count()), [=](sycl::id<1> i){
            auto& op = opParamsIO[i];
            if(op.removed)
                return;

            auto cell_i = id(op.cellPos);
            auto& cell = cellParamsI[cell_i];
            if(cell.col == cols-1 || cell.col%2 != 1 || equals(cell.type, seaType))
                return;

            auto thickness = volumeI[cell_i] / (cellSize * cellSize);
            auto deltaMass = dataI[cell_i].deltaMass;
            if (thickness > minSlickThickness) {
                Vector2 v1(0, (deltaMass>0)*(-cellSize) + (deltaMass<0)*(cellSize));
                if (dataI[cell_i].ratio > randomRatioI[i]) {
                    addVec(op.position, v1);
                    op.cellPos = posFromId(dataI[cell_i].ti);
                }
            }
        });
    });
    queue.wait();
}
