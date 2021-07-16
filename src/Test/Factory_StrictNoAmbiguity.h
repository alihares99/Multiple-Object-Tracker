#pragma once

#include "Factory.h"

#include "Gater.h"
#include "Assignment_NoAmbiguity.h"
#include "Filter_CVAB.h"
#include "Maintenance_History.h"

class Factory_StrictNoAmbiguity : public Factory<1, double>
{
public:
    Factory_StrictNoAmbiguity() {}
    ~Factory_StrictNoAmbiguity() override {}

    std::unique_ptr<Gater<1, double>> getGater() override {
        return std::unique_ptr<Gater<1, double>>(new Gater<1, double>(this, 2.0, 8, 30.0, 10000));
    }
    std::unique_ptr<Assignment<1, double>> getAssignment() override {
        return std::unique_ptr<Assignment<1, double>>(new Assignment_NoAmbiguity<1, double>(this, 5.0, 2.0));
    }
    std::unique_ptr<Filter<1, double>> getFilter() override {
        return std::unique_ptr<Filter<1, double>>(new Filter_CVAB<1, double>({0.8}, {0.1}));
    }
    std::unique_ptr<Maintenance<1, double>> getMaintenance() override {
        return std::unique_ptr<Maintenance<1, double>>(new Maintenance_History<1, double>(this, 5, 3, 20, 5));
    }
    DistanceMetric<1, double>* getDistanceMetric() override {
        return &distanceMetric;
    }

private:
    DistanceMetric<1, double> distanceMetric;
};

