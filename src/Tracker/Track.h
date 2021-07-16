#pragma once

#include "Filter.h"

template <int dim, class T>
struct Track {
    const TrackId trackId;
    std::unique_ptr<Filter<dim, T>> filter;

    explicit Track(std::unique_ptr<Filter<dim, T>> filter) :
        trackId(getUniqueTrackId()),
        filter(std::move(filter))
    {}

private:
    static TrackId trackIdCounter;
    static TrackId getUniqueTrackId() {
        if (trackIdCounter < 0) {
            trackIdCounter = 0;
        }
        return ++trackIdCounter;
    }
};

template <int dim, class T>
int Track<dim, T>::trackIdCounter = 0;
