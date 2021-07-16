#pragma once

#include "Factory.h"
#include "Track.h"

#include <forward_list>

template <int dim, class T>
class Maintenance
{
public:
    explicit Maintenance(Factory<dim, T>* factory) : factory(factory) {}
    virtual ~Maintenance() {}

    virtual void maintain(std::forward_list<Track<dim, T>>& confirmedTracks,
                          std::forward_list<Track<dim, T>>& tentativeTracks,
                          QVector<Measurment<dim, T>*>& meases,
                          TOA_t now) = 0;
    void setLockedTrack(TrackId trackid){
        lockedTrackDeleted = false;
        lockedTrackID = trackid;
    }

    bool isLockedTrackDeleted(){return lockedTrackDeleted;}

protected:
    Factory<dim, T>* const factory;
    TrackId lockedTrackID;
    bool lockedTrackDeleted = false;
};

