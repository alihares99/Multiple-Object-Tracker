#pragma once

#include <QtGlobal>
#include <memory>
#include <array>

#include "EnumClass.h"

using TOA_t = quint64;
using TrackId = int;

template <int dim, class T> class Gater;
template <int dim, class T> class Assignment;
template <int dim, class T> class Maintenance;
template <int dim, class T> class Filter;
template <int dim, class T> struct Track;
template <int dim, class T> class Factory;

EnumClass AssignmentStatus {
    enum Value {
        unassigned,
        assigned,
        ambiguous
    };
    ENUM_CLASS(AssignmentStatus)
};

template <int dim, class T>
struct Measurment {
    /// pointer is not good the assigned track. The memory pointed by the pointer
    /// may contain another track later.
    TrackId trackId = 0;
    AssignmentStatus assignmentStatus = AssignmentStatus::unassigned;

    #if (SAVE_PREDICTION_FILTERPOSITION)
    std::array<T, dim> prediction;
    std::array<T, dim> filterPosition;
    #endif

    int getDimension() { return dim; }
    virtual TOA_t& getToa() = 0;
    virtual T& getData(int dimension) = 0;
};

struct Edge {
    int meas_index;
    int track_index;
};
