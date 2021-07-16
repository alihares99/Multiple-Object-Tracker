#pragma once

#include "Assignment.h"
#include "Maintenance.h"

#include <memory>
#include <forward_list>

#include <QDebug>
#include <QVector>

#include <Eigen/Core>

template <int dim, class T>
class Tracker
{
public:
    explicit Tracker(std::shared_ptr<Factory<dim, T>> factory) :
        factory(factory),
        gater(factory->getGater()),
        assignment(factory->getAssignment()),
        maintenance(factory->getMaintenance())
    {}
    ~Tracker()
    {}

    void track(QVector<Measurment<dim, T>*>& meases, TOA_t now) {
        clearUpdatedFlag();
        mapTracks();

        Eigen::MatrixXd weights = gater->calculateWeights(meases, confirmedTracks, n_confirmed, tentativeTracks, n_tentative);

        QVector<Edge> edges = assignment->assign(weights, meases);

        for (const Edge& edge : edges) {
            Measurment<dim, T>* meas = meases.at(edge.meas_index);
            Track<dim, T>* assignedTrack = getTrack(edge.track_index);

            #if (SAVE_PREDICTION_FILTERPOSITION)
            auto prediction = assignedTrack->filter->predict(meas->getToa());
            for (size_t i = 0; i < dim; i++) {
                meas->prediction[i] = prediction->getData(i);
            }
            #endif
            assignedTrack->filter->update(meas);
            #if (SAVE_PREDICTION_FILTERPOSITION)
            auto filterPosition = assignedTrack->filter->getPosition();
            for (size_t i = 0; i < dim; i++) {
                meas->filterPosition[i] = filterPosition->getData(i);
            }
            #endif
            meas->trackId = assignedTrack->trackId;
        }

        maintenance->maintain(confirmedTracks,tentativeTracks, meases, now);
    }

    std::forward_list<Track<dim, T>>& getConfirmedTracks() {
        return confirmedTracks;
    }
    std::forward_list<Track<dim, T>>& getTentativeTracks() {
        return tentativeTracks;
    }
    Gater<dim, T>* getGater() {
        return gater.get();
    }

    void setLockedTrack(TrackId trackid){
        maintenance->setLockedTrack(trackid);
    }

    bool isLockedTrackDeleted(){return maintenance->isLockedTrackDeleted();}

private:
    void mapTracks() {
        tracksIndex.clear();
        int i = 0;
        for (auto& track : confirmedTracks) {
            tracksIndex.insert(i++, &track);
        }
        n_confirmed = i;
        for (auto& track : tentativeTracks) {
            tracksIndex.insert(i++, &track);
        }
        n_tentative = i - n_confirmed;
    }

    void clearUpdatedFlag() {
        for (auto& track : confirmedTracks) {
            track.filter->clearUpdated();
        }
        for (auto& track : tentativeTracks) {
            track.filter->clearUpdated();
        }
    }

    Track<dim, T>* getTrack(int rank)
    {
        if (tracksIndex.contains(rank)) {
            return tracksIndex.value(rank);
        }

        qDebug() << "CRITICAL in Tracker<dim, T>::getTrack(int rank): This line should have never executed.";
        return nullptr;
    }

    std::forward_list<Track<dim, T>> confirmedTracks;
    std::forward_list<Track<dim, T>> tentativeTracks;
    QHash<int, Track<dim, T>*> tracksIndex;
    int n_confirmed;
    int n_tentative;

    std::shared_ptr<Factory<dim, T>> factory;
    std::unique_ptr<Gater<dim, T>> gater;
    std::unique_ptr<Assignment<dim, T>> assignment;
    std::unique_ptr<Maintenance<dim, T>> maintenance;
};

