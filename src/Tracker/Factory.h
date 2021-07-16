#pragma once

#include "DistanceMetric.h"

#include <memory>

template<int dim, class T>
class Factory {
public:
    Factory() {}
    virtual ~Factory() {}

    virtual std::unique_ptr<Gater<dim, T>> getGater() = 0;
    virtual std::unique_ptr<Assignment<dim, T>> getAssignment() = 0;
    virtual std::unique_ptr<Filter<dim, T>> getFilter() = 0;
    virtual std::unique_ptr<Maintenance<dim, T>> getMaintenance() = 0;
    virtual DistanceMetric<dim, T>* getDistanceMetric() = 0;
};



