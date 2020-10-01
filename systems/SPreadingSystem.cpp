//
// Created by arczipt on 24.08.2020.
//

#include <iostream>
#include "SPreadingSystem.h"

struct data {
    int ti = 0;
    int fi = 0;
    double ratio = 0;
    double deltaMass = 0;

    bool valid = true;

    data(bool valid): valid(valid) {}
    data(int ti, int fi, double ratio, double deltaMass) : ti(ti), fi(fi), ratio(ratio), deltaMass(deltaMass) {}
};

//TODO: refactor
void SpreadingSystem::update(sycl::queue &queue,
                             sycl::buffer<Cell::Params, 1> &cellParamsBuf,
                             sycl::buffer<OilPoint::Params, 1> &opParamsBuf,
                             sycl::buffer<OilComponent, 2> &opCompBuf,
                             int timestep) {
    std::vector<double> massOfEmulsion(cellParamsBuf.get_count(), 0);
    std::vector<double> oilDensity(cellParamsBuf.get_count(), 0);
    std::vector<double> volume(cellParamsBuf.get_count(), 0);
    std::vector<double> randomRatio(opParamsBuf.get_count(), 0);
    double tvolume = 0;

    //temporary
    sycl::buffer<data, 1> dataBuf(sycl::range<1>(cellParamsBuf.get_count()));

    //functions used on gpu cores
    auto calcWaterDynamicViscosity = [](double T, double S) -> double {
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

    auto calculateWaterDensity = [](double temp, double conc) -> double {
        // salinity in mg/L
        temp = temp - 273.15;
        double rho = 1000 * (1.0 - (temp + 288.9414)
                                   / (508929.2 * (temp + 68.12963)) * (sycl::pow(temp - 3.9863, 2.0)));

        double rhos, A, B;
        A = 0.824493 - 0.0040899 * temp + 0.000076438 * sycl::pow(temp, 2.0)
            - 0.00000082467 * sycl::pow(temp, 3.0) + 0.0000000053675
                                                     * sycl::pow(temp, 4.0);
        B = -0.005724 + 0.00010227 * temp - 0.0000016546 * sycl::pow(temp, 2.0);
        rhos = rho + A * conc + B * sycl::pow(conc, 3.0 / 2.0) + 0.00048314
                                                                 * sycl::pow(conc, 2.0);
        return rhos;
    };

    double time = timeSystem.totalTime + (double) timestep / 2;
    /*
     * TODO: refactor
     * For every direction (right and down) submit two jobs:
     *  - 0->1 2->3 4->5 .... and op update
     *  - wait to finish
     *  - 1->2 3->4 5->6 .... and op update
     *  - wait to finish
     */

    //prepare data
    {
        auto opParamsI = opParamsBuf.get_access<sycl::access::mode::read>();
        auto cellParamsI = cellParamsBuf.get_access<sycl::access::mode::read>();

        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_real_distribution<double> dist(0, 1);

        for (int i = 0; i < opParamsBuf.get_count(); i++) {
            if(!opParamsI[i].removed){
                massOfEmulsion[cells.id(opParamsI[i].cellPos)] += opParamsI[i].massOfEmulsion;
                oilDensity[cells.id(opParamsI[i].cellPos)] += opParamsI[i].density * opParamsI[i].massOfEmulsion;
            }
            randomRatio[i] = dist(mt);
        }

        auto cols = config.cols;
        auto rows = config.rows;
        for (int i = 0; i < cellParamsBuf.get_count(); i++) {
            if(oilDensity[i] != 0 && massOfEmulsion[i] != 0 && cellParamsI[i].row != 0 && cellParamsI[i].row != rows-1 && cellParamsI[i].col != cols-1 && cellParamsI[i].col != 0) {
                oilDensity[i] = oilDensity[i] / massOfEmulsion[i];
                volume[i] = massOfEmulsion[i] / oilDensity[i];
                tvolume += volume[i];
            }else{
                oilDensity[i] = 0;
                volume[i] = 0;
            }
        }
    }
    std::unique_ptr<sycl::buffer<double, 1>> massOfEmulsionBuf(new sycl::buffer<double, 1>(massOfEmulsion.data(), sycl::range<1>(cellParamsBuf.get_count())));
    std::unique_ptr<sycl::buffer<double, 1>> oilDensityBuf(new sycl::buffer<double, 1>(oilDensity.data(), sycl::range<1>(cellParamsBuf.get_count())));
    std::unique_ptr<sycl::buffer<double, 1>> volumeBuf(new sycl::buffer<double, 1>(volume.data(), sycl::range<1>(cellParamsBuf.get_count())));
    std::unique_ptr<sycl::buffer<double, 1>> randomRatioBuf(new sycl::buffer<double, 1>(randomRatio.data(), sycl::range<1>(opParamsBuf.get_count())));

    /**
     * dx=0
     * dy=1
     * s=0
     */
    queue.submit([&](sycl::handler &cgh) {
        auto cellParamsI = cellParamsBuf.get_access<sycl::access::mode::read>(cgh);
        auto oilDensityI = oilDensityBuf->get_access<sycl::access::mode::read>(cgh);
        auto massOfEmulsionI = massOfEmulsionBuf->get_access<sycl::access::mode::read>(cgh);
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

        cgh.parallel_for<class SSCalc4>(sycl::range<1>(cellParamsBuf.get_count()), [=](sycl::id<1> i) {
            auto &cell = cellParamsI[i];
            //second cell
            auto n_i = i[0] + 1;
            auto &cell2 = cellParamsI[n_i];

            if (cell.col % 2 == 0 || cell.col >= cols - 2 || cell.col == 0 || cell.row == 0 || cell.row == rows - 1)
                return;

            double mass1 = massOfEmulsionI[i];
            double mass2 = massOfEmulsionI[n_i];

            if (mass1 == 0 && mass2 == 0) {
                dataO[n_i] = dataO[i] = data(false);
                return;
            }

            if(!equals(cell.type, seaType) && !equals(cell2.type, seaType)) {
                dataO[n_i] = dataO[i] = data(false);
                return;
            }

            /*double temperature = (cell1.getTemperature() + cell2.getTemperature()) / 2;
            double salinity = config.getSalinity();
            double waterDensity = TemperatureDependency.calculateWaterDensity(temperature, salinity);
            double kinematicWaterViscosity = TemperatureDependency.calculateWaterDynamicViscosity(temperature, salinity)/ waterDensity;
            double time = timesystem.getTotalTime() + timestep / 2.;
            double delta = (waterDensity - oilDensity) / waterDensity;
            double base = (g * delta * volume * volume) / (Math.sqrt(kinematicWaterViscosity));
            double diffusion = 0.49/ Math.pow(config.getSpreadigCoefficient(),2 ) * Math.pow(base, 1.0 / 3) * 1 / Math.sqrt(time);
            double deltaMass = (0.5 * (mass2 - mass1) * (1 - Math.exp(-2* diffusion / (size * size) * timestep)));*/

            double oilDensity = (mass1 * oilDensityI[i] + mass2 * oilDensityI[n_i]) / (mass1 + mass2);
            double temperature = (cell.temperature + cell2.temperature) / 2;
            double waterDensity = calculateWaterDensity(temperature, salinity);
            double kinematicWaterViscosity = calcWaterDynamicViscosity(temperature, salinity) / waterDensity;
            double delta = (waterDensity - oilDensity) / waterDensity;
            double base = (g * delta * tvolume * tvolume) / (sycl::sqrt(kinematicWaterViscosity));
            double diffusion =
                    0.49 / sycl::pow(spreadingCoefficient, 2.0) * sycl::pow(base, 1.0 / 3) * 1 / sycl::sqrt(time);

            double deltaMass = (0.5 * (mass2 - mass1) *
                                (1 - sycl::exp(-2 * diffusion / (cellSize * cellSize) * timestep)));

            if (deltaMass == 0) {
                dataO[n_i] = dataO[i] = data(false);
                return;
            }

            //double ratio = (deltaMass < 0) * (-deltaMass / mass1) + (deltaMass > 0) * (deltaMass / mass2);
            //int ti = (deltaMass < 0) * (n_i) + (deltaMass > 0) * (i[0]);
            //int fi = (deltaMass < 0) * (i[0]) + (deltaMass > 0) * (n_i);
            double ratio = 0;
            int ti = 0, fi = 0;
            ratio = (deltaMass < 0) ? (-deltaMass/mass1) : (deltaMass/mass2);
            ti = (deltaMass < 0) ? n_i : i[0];
            fi = (deltaMass < 0) ? i[0] : n_i;

            dataO[n_i] = dataO[i] = data(ti, fi, ratio, deltaMass);
        });
    });
    /**
     * Update OP
     */
    queue.submit([&](sycl::handler &cgh) {
        auto cellParamsI = cellParamsBuf.get_access<sycl::access::mode::read>(cgh);
        auto opParamsIO = opParamsBuf.get_access<sycl::access::mode::read_write>(cgh);
        auto oilDensityI = oilDensityBuf->get_access<sycl::access::mode::read>(cgh);
        auto massOfEmulsionI = massOfEmulsionBuf->get_access<sycl::access::mode::read>(cgh);
        auto dataI = dataBuf.get_access<sycl::access::mode::read>(cgh);
        auto volumeI = volumeBuf->get_access<sycl::access::mode::read>(cgh);
        auto randomRatioI = randomRatioBuf->get_access<sycl::access::mode::read>(cgh);

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
            auto &data = dataI[cell_i];

            if (!data.valid || data.fi != cell_i || !equals(cellParamsI[data.ti].type, seaType))
                return;

            auto thickness = volumeI[data.fi] / (cellSize * cellSize);
            auto deltaMass = data.deltaMass;
            if (thickness > minSlickThickness){

                if (data.ratio > randomRatioI[i]) {
                    Vector2 v(0, 0);
                    ;
                    if (deltaMass > 0)
                        v.y = -cellSize;
                    else
                        v.y = cellSize;

                    addVec(op.position, v);
                    op.cellPos = posFromId(data.ti);
                }
            }
        });
    });
    queue.wait(); //!important synchronize data;
    //destory data
    oilDensityBuf->set_final_data(nullptr);
    massOfEmulsionBuf->set_final_data(nullptr);
    volumeBuf->set_final_data(nullptr);
    randomRatioBuf->set_final_data(nullptr);
    massOfEmulsion = std::vector<double>(cellParamsBuf.get_count(), 0);
    oilDensity = std::vector<double>(cellParamsBuf.get_count(), 0);
    volume = std::vector<double>(cellParamsBuf.get_count(), 0);
    randomRatio = std::vector<double>(opParamsBuf.get_count(), 0);

    //prepare data
    {
        auto opParamsI = opParamsBuf.get_access<sycl::access::mode::read>();
        auto cellParamsI = cellParamsBuf.get_access<sycl::access::mode::read>();

        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_real_distribution<double> dist(0, 1);

        for (int i = 0; i < opParamsBuf.get_count(); i++) {
            if(!opParamsI[i].removed){
                massOfEmulsion[cells.id(opParamsI[i].cellPos)] += opParamsI[i].massOfEmulsion;
                oilDensity[cells.id(opParamsI[i].cellPos)] += opParamsI[i].density * opParamsI[i].massOfEmulsion;
            }
            randomRatio[i] = dist(mt);
        }

        auto cols = config.cols;
        auto rows = config.rows;
        for (int i = 0; i < cellParamsBuf.get_count(); i++) {
            if(oilDensity[i] != 0 && massOfEmulsion[i] != 0 && cellParamsI[i].row != 0 && cellParamsI[i].row != rows-1 && cellParamsI[i].col != cols-1 && cellParamsI[i].col != 0) {
                oilDensity[i] = oilDensity[i] / massOfEmulsion[i];
                volume[i] = massOfEmulsion[i] / oilDensity[i];
            }else{
                oilDensity[i] = 0;
                volume[i] = 0;
            }
        }
    }
    massOfEmulsionBuf = std::unique_ptr<sycl::buffer<double, 1>>(new sycl::buffer<double, 1>(massOfEmulsion.data(), sycl::range<1>(cellParamsBuf.get_count())));
    oilDensityBuf = std::unique_ptr<sycl::buffer<double, 1>>(new sycl::buffer<double, 1>(oilDensity.data(), sycl::range<1>(cellParamsBuf.get_count())));
    volumeBuf = std::unique_ptr<sycl::buffer<double, 1>>(new sycl::buffer<double, 1>(volume.data(), sycl::range<1>(cellParamsBuf.get_count())));
    randomRatioBuf = std::unique_ptr<sycl::buffer<double, 1>>(new sycl::buffer<double, 1>(randomRatio.data(), sycl::range<1>(opParamsBuf.get_count())));

    /**
    * dx=0
    * dy=1
    * s=0
    */
    queue.submit([&](sycl::handler &cgh) {
        auto cellParamsI = cellParamsBuf.get_access<sycl::access::mode::read>(cgh);
        auto oilDensityI = oilDensityBuf->get_access<sycl::access::mode::read>(cgh);
        auto massOfEmulsionI = massOfEmulsionBuf->get_access<sycl::access::mode::read>(cgh);
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

        cgh.parallel_for<class SSCalc2>(sycl::range<1>(cellParamsBuf.get_count()), [=](sycl::id<1> i) {
            auto &cell = cellParamsI[i];
            //second cell
            auto n_i = i[0] + 1;
            auto &cell2 = cellParamsI[n_i];

            if (cell.col % 2 == 1 || cell.col >= cols - 2 || cell.col <= 1 || cell.row == 0 || cell.row == rows - 1)
                return;

            double mass1 = massOfEmulsionI[i];
            double mass2 = massOfEmulsionI[n_i];

            if (mass1 == 0 && mass2 == 0) {
                dataO[n_i] = dataO[i] = data(false);
                return;
            }

            if(!equals(cell.type, seaType) && !equals(cell2.type, seaType)) {
                dataO[n_i] = dataO[i] = data(false);
                return;
            }

            double oilDensity = (mass1 * oilDensityI[i] + mass2 * oilDensityI[n_i]) / (mass1 + mass2);
            double temperature = (cell.temperature + cell2.temperature) / 2;
            double waterDensity = calculateWaterDensity(temperature, salinity);
            double kinematicWaterViscosity = calcWaterDynamicViscosity(temperature, salinity) / waterDensity;
            double delta = (waterDensity - oilDensity) / waterDensity;
            double base = (g * delta * tvolume * tvolume) / (sycl::sqrt(kinematicWaterViscosity));
            double diffusion =
                    0.49 / sycl::pow(spreadingCoefficient, 2.0) * sycl::pow(base, 1.0 / 3) * 1 / sycl::sqrt(time);

            double deltaMass = (0.5 * (mass2 - mass1) *
                                (1 - sycl::exp(-2 * diffusion / (cellSize * cellSize) * timestep)));

            if (deltaMass == 0) {
                dataO[n_i] = dataO[i] = data(false);
                return;
            }

            //double ratio = (deltaMass < 0) * (-deltaMass / mass1) + (deltaMass > 0) * (deltaMass / mass2);
            //int ti = (deltaMass < 0) * (n_i) + (deltaMass > 0) * (i[0]);
            //int fi = (deltaMass < 0) * (i[0]) + (deltaMass > 0) * (n_i);
            double ratio = 0;
            int ti = 0, fi = 0;
            ratio = (deltaMass < 0) ? (-deltaMass/mass1) : (deltaMass/mass2);
            ti = (deltaMass < 0) ? n_i : i[0];
            fi = (deltaMass < 0) ? i[0] : n_i;

            dataO[n_i] = dataO[i] = data(ti, fi, ratio, deltaMass);
        });
    });
    /**
     * Update OP
     */
    queue.submit([&](sycl::handler &cgh) {
        auto cellParamsI = cellParamsBuf.get_access<sycl::access::mode::read>(cgh);
        auto opParamsIO = opParamsBuf.get_access<sycl::access::mode::read_write>(cgh);
        auto oilDensityI = oilDensityBuf->get_access<sycl::access::mode::read>(cgh);
        auto massOfEmulsionI = massOfEmulsionBuf->get_access<sycl::access::mode::read>(cgh);
        auto dataI = dataBuf.get_access<sycl::access::mode::read>(cgh);
        auto volumeI = volumeBuf->get_access<sycl::access::mode::read>(cgh);
        auto randomRatioI = randomRatioBuf->get_access<sycl::access::mode::read>(cgh);

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
        cgh.parallel_for<class SSUpdate2>(sycl::range<1>(opParamsBuf.get_count()), [=](sycl::id<1> i) {
            auto &op = opParamsIO[i];
            if (op.removed)
                return;

            auto cell_i = id(op.cellPos);
            auto &data = dataI[cell_i];

            if (!data.valid || data.fi != cell_i || !equals(cellParamsI[data.ti].type, seaType))
                return;

            auto thickness = volumeI[data.fi] / (cellSize * cellSize);
            auto deltaMass = data.deltaMass;
            if (thickness > minSlickThickness){
                if (data.ratio > randomRatioI[i]) {
                    Vector2 v(0, 0);
                    ;
                    if (deltaMass > 0)
                        v.y = -cellSize;
                    else
                        v.y = cellSize;

                    addVec(op.position, v);
                    op.cellPos = posFromId(data.ti);
                }
            }
        });
    });
    queue.wait(); //!important synchronize data;
    //destory data
    oilDensityBuf->set_final_data(nullptr);
    massOfEmulsionBuf->set_final_data(nullptr);
    volumeBuf->set_final_data(nullptr);
    randomRatioBuf->set_final_data(nullptr);
    massOfEmulsion = std::vector<double>(cellParamsBuf.get_count(), 0);
    oilDensity = std::vector<double>(cellParamsBuf.get_count(), 0);
    volume = std::vector<double>(cellParamsBuf.get_count(), 0);
    randomRatio = std::vector<double>(opParamsBuf.get_count(), 0);

    //prepare data
    {
        auto opParamsI = opParamsBuf.get_access<sycl::access::mode::read>();
        auto cellParamsI = cellParamsBuf.get_access<sycl::access::mode::read>();

        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_real_distribution<double> dist(0, 1);

        for (int i = 0; i < opParamsBuf.get_count(); i++) {
            if(!opParamsI[i].removed){
                massOfEmulsion[cells.id(opParamsI[i].cellPos)] += opParamsI[i].massOfEmulsion;
                oilDensity[cells.id(opParamsI[i].cellPos)] += opParamsI[i].density * opParamsI[i].massOfEmulsion;
            }
            randomRatio[i] = dist(mt);
        }

        auto cols = config.cols;
        auto rows = config.rows;
        for (int i = 0; i < cellParamsBuf.get_count(); i++) {
            if(oilDensity[i] != 0 && massOfEmulsion[i] != 0 && cellParamsI[i].row != 0 && cellParamsI[i].row != rows-1 && cellParamsI[i].col != cols-1 && cellParamsI[i].col != 0) {
                oilDensity[i] = oilDensity[i] / massOfEmulsion[i];
                volume[i] = massOfEmulsion[i] / oilDensity[i];\
            }else{
                oilDensity[i] = 0;
                volume[i] = 0;
            }
        }
    }
    massOfEmulsionBuf = std::unique_ptr<sycl::buffer<double, 1>>(new sycl::buffer<double, 1>(massOfEmulsion.data(), sycl::range<1>(cellParamsBuf.get_count())));
    oilDensityBuf = std::unique_ptr<sycl::buffer<double, 1>>(new sycl::buffer<double, 1>(oilDensity.data(), sycl::range<1>(cellParamsBuf.get_count())));
    volumeBuf = std::unique_ptr<sycl::buffer<double, 1>>(new sycl::buffer<double, 1>(volume.data(), sycl::range<1>(cellParamsBuf.get_count())));
    randomRatioBuf = std::unique_ptr<sycl::buffer<double, 1>>(new sycl::buffer<double, 1>(randomRatio.data(), sycl::range<1>(opParamsBuf.get_count())));

    /**
    * dx=0
    * dy=1
    * s=0
    */
    queue.submit([&](sycl::handler &cgh) {
        auto cellParamsI = cellParamsBuf.get_access<sycl::access::mode::read>(cgh);
        auto oilDensityI = oilDensityBuf->get_access<sycl::access::mode::read>(cgh);
        auto massOfEmulsionI = massOfEmulsionBuf->get_access<sycl::access::mode::read>(cgh);
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
            //second cell
            auto n_i = i[0] + cols;
            auto &cell2 = cellParamsI[n_i];

            if (cell.row % 2 == 0 || cell.row >= rows - 2 || cell.row == 0 || cell.col == 0 || cell.col == cols - 1)
                return;

            double mass1 = massOfEmulsionI[i];
            double mass2 = massOfEmulsionI[n_i];

            if (mass1 == 0 && mass2 == 0) {
                dataO[n_i] = dataO[i] = data(false);
                return;
            }

            if(!equals(cell.type, seaType) && !equals(cell2.type, seaType)) {
                dataO[n_i] = dataO[i] = data(false);
                return;
            }

            double oilDensity = (mass1 * oilDensityI[i] + mass2 * oilDensityI[n_i]) / (mass1 + mass2);
            double temperature = (cell.temperature + cell2.temperature) / 2;
            double waterDensity = calculateWaterDensity(temperature, salinity);
            double kinematicWaterViscosity = calcWaterDynamicViscosity(temperature, salinity) / waterDensity;
            double delta = (waterDensity - oilDensity) / waterDensity;
            double base = (g * delta * tvolume * tvolume) / (sycl::sqrt(kinematicWaterViscosity));
            double diffusion =
                    0.49 / sycl::pow(spreadingCoefficient, 2.0) * sycl::pow(base, 1.0 / 3) * 1 / sycl::sqrt(time);

            double deltaMass = (0.5 * (mass2 - mass1) *
                                (1 - sycl::exp(-2 * diffusion / (cellSize * cellSize) * timestep)));

            if (deltaMass == 0) {
                dataO[n_i] = dataO[i] = data(false);
                return;
            }

            //double ratio = (deltaMass < 0) * (-deltaMass / mass1) + (deltaMass > 0) * (deltaMass / mass2);
            //int ti = (deltaMass < 0) * (n_i) + (deltaMass > 0) * (i[0]);
            //int fi = (deltaMass < 0) * (i[0]) + (deltaMass > 0) * (n_i);
            double ratio = 0;
            int ti = 0, fi = 0;
            ratio = (deltaMass < 0) ? (-deltaMass/mass1) : (deltaMass/mass2);
            ti = (deltaMass < 0) ? n_i : i[0];
            fi = (deltaMass < 0) ? i[0] : n_i;

            dataO[n_i] = dataO[i] = data(ti, fi, ratio, deltaMass);
        });
    });

    /**
     * Update OP
     */
    queue.submit([&](sycl::handler &cgh) {
        auto cellParamsI = cellParamsBuf.get_access<sycl::access::mode::read>(cgh);
        auto opParamsIO = opParamsBuf.get_access<sycl::access::mode::read_write>(cgh);
        auto oilDensityI = oilDensityBuf->get_access<sycl::access::mode::read>(cgh);
        auto massOfEmulsionI = massOfEmulsionBuf->get_access<sycl::access::mode::read>(cgh);
        auto dataI = dataBuf.get_access<sycl::access::mode::read>(cgh);
        auto volumeI = volumeBuf->get_access<sycl::access::mode::read>(cgh);
        auto randomRatioI = randomRatioBuf->get_access<sycl::access::mode::read>(cgh);

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
        cgh.parallel_for<class SSUpdate3>(sycl::range<1>(opParamsBuf.get_count()), [=](sycl::id<1> i) {
            auto &op = opParamsIO[i];
            if (op.removed)
                return;

            auto cell_i = id(op.cellPos);
            auto &data = dataI[cell_i];

            if (!data.valid || data.fi != cell_i || !equals(cellParamsI[data.ti].type, seaType))
                return;

            auto thickness = volumeI[data.fi] / (cellSize * cellSize);
            auto deltaMass = data.deltaMass;
            if (thickness > minSlickThickness){

                if (data.ratio > randomRatioI[i]) {
                    Vector2 v(0, 0);
                    ;
                    if (deltaMass > 0)
                        v.x = -cellSize;
                    else
                        v.x = cellSize;

                    addVec(op.position, v);
                    op.cellPos = posFromId(data.ti);
                }
            }
        });
    });
    queue.wait(); //!important synchronize data;
    //destroy data
    oilDensityBuf->set_final_data(nullptr);
    massOfEmulsionBuf->set_final_data(nullptr);
    volumeBuf->set_final_data(nullptr);
    randomRatioBuf->set_final_data(nullptr);
    massOfEmulsion = std::vector<double>(cellParamsBuf.get_count(), 0);
    oilDensity = std::vector<double>(cellParamsBuf.get_count(), 0);
    volume = std::vector<double>(cellParamsBuf.get_count(), 0);
    randomRatio = std::vector<double>(opParamsBuf.get_count(), 0);

    //prepare data
    {
        auto opParamsI = opParamsBuf.get_access<sycl::access::mode::read>();
        auto cellParamsI = cellParamsBuf.get_access<sycl::access::mode::read>();

        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_real_distribution<double> dist(0, 1);

        for (int i = 0; i < opParamsBuf.get_count(); i++) {
            if(!opParamsI[i].removed){
                massOfEmulsion[cells.id(opParamsI[i].cellPos)] += opParamsI[i].massOfEmulsion;
                oilDensity[cells.id(opParamsI[i].cellPos)] += opParamsI[i].density * opParamsI[i].massOfEmulsion;
            }
            randomRatio[i] = dist(mt);
        }

        auto cols = config.cols;
        auto rows = config.rows;
        for (int i = 0; i < cellParamsBuf.get_count(); i++) {
            if(oilDensity[i] != 0 && massOfEmulsion[i] != 0 && cellParamsI[i].row != 0 && cellParamsI[i].row != rows-1 && cellParamsI[i].col != cols-1 && cellParamsI[i].col != 0) {
                oilDensity[i] = oilDensity[i] / massOfEmulsion[i];
                volume[i] = massOfEmulsion[i] / oilDensity[i];\
            }else{
                oilDensity[i] = 0;
                volume[i] = 0;
            }
        }
    }
    massOfEmulsionBuf = std::unique_ptr<sycl::buffer<double, 1>>(new sycl::buffer<double, 1>(massOfEmulsion.data(), sycl::range<1>(cellParamsBuf.get_count())));
    oilDensityBuf = std::unique_ptr<sycl::buffer<double, 1>>(new sycl::buffer<double, 1>(oilDensity.data(), sycl::range<1>(cellParamsBuf.get_count())));
    volumeBuf = std::unique_ptr<sycl::buffer<double, 1>>(new sycl::buffer<double, 1>(volume.data(), sycl::range<1>(cellParamsBuf.get_count())));
    randomRatioBuf = std::unique_ptr<sycl::buffer<double, 1>>(new sycl::buffer<double, 1>(randomRatio.data(), sycl::range<1>(opParamsBuf.get_count())));

    /**
    * dx=0
    * dy=1
    * s=0
    */
    queue.submit([&](sycl::handler &cgh) {
        auto cellParamsI = cellParamsBuf.get_access<sycl::access::mode::read>(cgh);
        auto oilDensityI = oilDensityBuf->get_access<sycl::access::mode::read>(cgh);
        auto massOfEmulsionI = massOfEmulsionBuf->get_access<sycl::access::mode::read>(cgh);
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

        cgh.parallel_for<class SSCalc3>(sycl::range<1>(cellParamsBuf.get_count()), [=](sycl::id<1> i) {
            auto &cell = cellParamsI[i];
            //second cell
            auto n_i = i[0] + cols;
            auto &cell2 = cellParamsI[n_i];

            if (cell.row % 2 == 1 || cell.row >= rows - 2 || cell.row <= 1 || cell.col == 0 || cell.col == cols - 1)
                return;

            double mass1 = massOfEmulsionI[i];
            double mass2 = massOfEmulsionI[n_i];

            if (mass1 == 0 && mass2 == 0) {
                dataO[n_i] = dataO[i] = data(false);
                return;
            }

            if(!equals(cell.type, seaType) && !equals(cell2.type, seaType)) {
                dataO[n_i] = dataO[i] = data(false);
                return;
            }

            double oilDensity = (mass1 * oilDensityI[i] + mass2 * oilDensityI[n_i]) / (mass1 + mass2);
            double temperature = (cell.temperature + cell2.temperature) / 2;
            double waterDensity = calculateWaterDensity(temperature, salinity);
            double kinematicWaterViscosity = calcWaterDynamicViscosity(temperature, salinity) / waterDensity;
            double delta = (waterDensity - oilDensity) / waterDensity;
            double base = (g * delta * tvolume * tvolume) / (sycl::sqrt(kinematicWaterViscosity));
            double diffusion =
                    0.49 / sycl::pow(spreadingCoefficient, 2.0) * sycl::pow(base, 1.0 / 3) * 1 / sycl::sqrt(time);

            double deltaMass = (0.5 * (mass2 - mass1) *
                                (1 - sycl::exp(-2 * diffusion / (cellSize * cellSize) * timestep)));

            if (deltaMass == 0) {
                dataO[n_i] = dataO[i] = data(false);
                return;
            }

            //double ratio = (deltaMass < 0) * (-deltaMass / mass1) + (deltaMass > 0) * (deltaMass / mass2);
            //int ti = (deltaMass < 0) * (n_i) + (deltaMass > 0) * (i[0]);
            //int fi = (deltaMass < 0) * (i[0]) + (deltaMass > 0) * (n_i);
            double ratio = 0;
            int ti = 0, fi = 0;
            ratio = (deltaMass < 0) ? (-deltaMass/mass1) : (deltaMass/mass2);
            ti = (deltaMass < 0) ? n_i : i[0];
            fi = (deltaMass < 0) ? i[0] : n_i;

            dataO[n_i] = dataO[i] = data(ti, fi, ratio, deltaMass);
        });
    });

    /**
     * Update OP
     */
    queue.submit([&](sycl::handler &cgh) {
        auto cellParamsI = cellParamsBuf.get_access<sycl::access::mode::read>(cgh);
        auto opParamsIO = opParamsBuf.get_access<sycl::access::mode::read_write>(cgh);
        auto oilDensityI = oilDensityBuf->get_access<sycl::access::mode::read>(cgh);
        auto massOfEmulsionI = massOfEmulsionBuf->get_access<sycl::access::mode::read>(cgh);
        auto dataI = dataBuf.get_access<sycl::access::mode::read>(cgh);
        auto volumeI = volumeBuf->get_access<sycl::access::mode::read>(cgh);
        auto randomRatioI = randomRatioBuf->get_access<sycl::access::mode::read>(cgh);

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
        cgh.parallel_for<class SSUpdate4>(sycl::range<1>(opParamsBuf.get_count()), [=](sycl::id<1> i) {
            auto &op = opParamsIO[i];
            if (op.removed)
                return;

            auto cell_i = id(op.cellPos);
            auto &data = dataI[cell_i];

            if (!data.valid || data.fi != cell_i || !equals(cellParamsI[data.ti].type, seaType))
                return;

            auto thickness = volumeI[data.fi] / (cellSize * cellSize);
            auto deltaMass = data.deltaMass;
            if (thickness > minSlickThickness){

                if (data.ratio > randomRatioI[i]) {
                    Vector2 v(0, 0);
                    ;
                    if (deltaMass > 0)
                        v.x = -cellSize;
                    else
                        v.x = cellSize;

                    addVec(op.position, v);
                    op.cellPos = posFromId(data.ti);
                }
            }
        });
    });
    queue.wait(); //!important synchronize data;
    //destroy data
    oilDensityBuf->set_final_data(nullptr);
    massOfEmulsionBuf->set_final_data(nullptr);
    volumeBuf->set_final_data(nullptr);
    randomRatioBuf->set_final_data(nullptr);
}
