//
// Created by Mateusz Święszek on 09/09/2020.
//
#include <cmath>
#include "TemperatureDependency.h"

double TemperatureDependency::calculateOilDensity(double densityAt15, double tempAtK) {
    double deltaT = tempAtK - 288.15;
    double alpha = 613.97226 / (densityAt15 * densityAt15);
    double Kt = std::exp(-alpha * deltaT * (1 + 0.8 * alpha * deltaT));
    return densityAt15 * Kt;
}

double TemperatureDependency::calculateWaterDensity(double temp, double conc) {
    // salinity in mg/L
    temp = temp - 273.15;
    double rho = 1000 * (1.0 - (temp + 288.9414)
                               / (508929.2 * (temp + 68.12963)) * (std::pow(temp - 3.9863, 2)));

    double rhos, A, B;
    A = 0.824493 - 0.0040899 * temp + 0.000076438 * std::pow(temp, 2)
        - 0.00000082467 * std::pow(temp, 3) + 0.0000000053675
                                              * std::pow(temp, 4);
    B = -0.005724 + 0.00010227 * temp - 0.0000016546 * std::pow(temp, 2);
    rhos = rho + A * conc + B * std::pow(conc, (3 / 2)) + 0.00048314
                                                          * std::pow(conc, 2);
    return rhos;
}

double TemperatureDependency::calculateWaterDensity2(double T, double s) {
    s = s / 1000;
    T -= 273.15;

    double a[] = {
            0.0,
            9.9992293295E+02,
            2.0341179217E-02,
            -6.1624591598E-03,
            2.2614664708E-05,
            -4.6570659168E-08,
    };

    double b[] = {
            0.0,
            8.0200240891E+02,
            -2.0005183488E+00,
            1.6771024982E-02,
            -3.0600536746E-05,
            -1.6132224742E-05,
    };

    double rho_w = a[1] + a[2] * T + a[3] * T * T + a[4] * T * T * T + a[5] * T * T * T * T;
    double D_rho = b[1] * s + b[2] * s * T + b[3] * s * T * T + b[4] * s * T * T * T + b[5] * s * s * T * T;
    return rho_w + D_rho;
}

double TemperatureDependency::calculateWaterDynamicViscosity(double T, double S) {
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

    double mu_w = a[4] + 1.0 / (a[1] * std::pow((T + a[2]), 2) + a[3]);


    double A = a[5] + a[6] * T + a[7] * T * T;
    double B = a[8] + a[9] * T + a[10] * T * T;
    double mu = mu_w * (1 + A * S + B * S * S);

    return mu;
}
