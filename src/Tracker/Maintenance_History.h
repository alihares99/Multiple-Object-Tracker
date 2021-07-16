#pragma once

#include "Maintenance.h"

#include <QVector>
#include <forward_list>
#include <QHash>

template <int dim, class T>
class Maintenance_History : public Maintenance<dim, T>
{
public:
    Maintenance_History(Factory<dim, T>* factory,
                        TOA_t monitoringDuration_confirmation,
                        int confirmationLimit,
                        TOA_t monitoringDuration_deletion,
                        int deletionLimit) :
        Maintenance<dim, T>(factory),
        monitoringDuration_confirmation(monitoringDuration_confirmation),
        confirmationLimit(confirmationLimit),
        monitoringDuration_deletion(monitoringDuration_deletion),
        deletionLimit(deletionLimit)
    {}
    ~Maintenance_History() override
    {}

    void maintain(std::forward_list<Track<dim, T>>& confirmedTracks,
                  std::forward_list<Track<dim, T>>& tentativeTracks,
                  QVector<Measurment<dim, T>*>& meases,
                  TOA_t now) override
    {
        /// delete stale tracks:
        auto prev = confirmedTracks.before_begin();
        for (auto it = confirmedTracks.begin(); it != confirmedTracks.end();) {

            bool erase = false;

            Track<dim, T>& track = *it;
            TrackMetaData& metaData = tracksMetaData[track.trackId];
            if (track.filter->isUpdated())
                metaData.n_hits++;

            if (isPeriodElapsed(metaData, now, forDeletion)) {
                bool trackShouldBeDeleted = false;
                if (metaData.n_hits < deletionLimit) {
                    trackShouldBeDeleted = true;
                }
                metaData.n_hits = 0;

                if (trackShouldBeDeleted) {
                    tracksMetaData.remove(track.trackId);
                    erase = true;
                }
            }

            if (erase) {
                if(track.trackId == this->lockedTrackID)
                    this->lockedTrackDeleted = true;
                it++;
                confirmedTracks.erase_after(prev);
            }
            else {
                prev = it;
                it++;
            }
        }

        /// confirm good tracks:
        prev = tentativeTracks.before_begin();
        for (auto it = tentativeTracks.begin(); it != tentativeTracks.end();) {

            bool erase = false;

            Track<dim, T>& track = *it;
            TrackMetaData& metaData = tracksMetaData[track.trackId];
            if (track.filter->isUpdated())
                metaData.n_hits++;

            if (isPeriodElapsed(metaData, now, forConfirmation)) {
                bool trackConfirmed = false;
                if (metaData.n_hits >= confirmationLimit) {
                    trackConfirmed = true;
                }

                if (trackConfirmed) {
                    confirmedTracks.push_front(std::move(track));
                }
                else {
                    tracksMetaData.remove(track.trackId);
                }
                erase = true;
            }

            if (erase) {
                if(track.trackId == this->lockedTrackID)
                    this->lockedTrackDeleted = true;
                it++;
                tentativeTracks.erase_after(prev);
            }
            else {
                prev = it;
                it++;
            }
        }

        /// add tentative tracks for non-ambiguous measurements:
        for (Measurment<dim, T>* meas : meases) {
            if (meas->assignmentStatus == AssignmentStatus::unassigned) {
                Track<dim, T> track(Maintenance<dim, T>::factory->getFilter());
                track.filter->update(meas);
                meas->assignmentStatus = AssignmentStatus::assigned;
                meas->trackId = track.trackId;
                tentativeTracks.push_front(std::move(track));
            }
        }

    }

private:
    struct TrackMetaData {
        bool isPeriodStarted = false;
        TOA_t startOfPeriod = 0;
        int n_hits = 0;
    };

    enum PeriodElapsedForWhat {
        forConfirmation,
        forDeletion,
    };

    bool isPeriodElapsed(TrackMetaData& metaData, TOA_t now, PeriodElapsedForWhat forWhat) {
        if (!metaData.isPeriodStarted) {
            metaData.isPeriodStarted = true;
            metaData.startOfPeriod = now;
            return false;
        }

        auto monitoringDuration = (forWhat == forConfirmation ?
                                       monitoringDuration_confirmation :
                                       monitoringDuration_deletion);
        if (now >= metaData.startOfPeriod + monitoringDuration) {
            metaData.startOfPeriod = now;
            return true;
        }

        return false;
    }

    const TOA_t monitoringDuration_confirmation;
    const int confirmationLimit;
    const TOA_t monitoringDuration_deletion;
    const int deletionLimit;

    QHash<TrackId, TrackMetaData> tracksMetaData;
};




