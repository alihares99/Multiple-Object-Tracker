#pragma once

#include "Types.h"

#include <memory>

template <int dim, class T>
class Filter
{
public:
    Filter(){}
    virtual ~Filter() {}

    virtual Measurment<dim, T>* predict(TOA_t predictionTime) = 0;
    virtual Measurment<dim, T>* getPosition() = 0;
    virtual void update(Measurment<dim, T>* meas) = 0;
    virtual bool isUpdated() = 0;
    virtual void clearUpdated() = 0;
    virtual bool isInNormalState() = 0;
};
