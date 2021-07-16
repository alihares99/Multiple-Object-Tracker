#pragma once

#include "Types.h"

#include <QDebug>

#define _USE_MATH_DEFINES
#include <math.h>

struct MyMeas : Measurment<1, double> {
    TOA_t toa = 0;
    double val = 0;

    TOA_t& getToa() override {return toa;}
    double& getData(int dimension) override {return val; Q_UNUSED(dimension)}
    virtual ~MyMeas() = default;
};

static inline double random_0_1() {
    double v = rand();
    return v / (RAND_MAX / 2) - 1.0;
}
