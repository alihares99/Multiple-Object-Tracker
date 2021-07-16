#pragma once

#include "Types.h"

#include <array>
#include <cmath>

template <int dim, class T>
class DistanceMetric
{
public:
    DistanceMetric() {}
    virtual ~DistanceMetric() {}

    virtual double distance(Measurment<dim, T>* a, Measurment<dim, T>* b) {
        double out = 0;
        for (int i = 0; i < dim; i++) {
            auto diff  = a->getData(i) - b->getData(i);
            out += diff * diff;
        }
        return std::sqrt(out);
    }
};

template <class T>
class DistanceMetric<1, T>
{
public:
    DistanceMetric() {}
    virtual ~DistanceMetric() {}

    virtual double distance(Measurment<1, T>* a, Measurment<1, T>* b) {
        if (a->getData(0) > b->getData(0)) {
            return a->getData(0) - b->getData(0);
        }
        else {
            return b->getData(0) - a->getData(0);
        }
    }
};

