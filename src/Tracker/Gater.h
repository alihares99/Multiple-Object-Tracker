#pragma once

#include "DistanceMetric.h"
#include "Filter.h"

#include <array>
#include <memory>
#include <QVector>
#include <forward_list>
#include <Eigen/Core>
#include <QHash>

template <int dim, class T>
class Gater
{
public:
    Gater(Factory<dim, T>* factory, double gateRadiusPerTime, double maxGateRadius, double tentavieRadius, double maxWeight) :
        factory(factory),
        tentativeRadius(tentavieRadius),
        gateRadiusPerTime(gateRadiusPerTime),
        maxGateRadius(maxGateRadius),
        maxWeight(maxWeight),
        maxWeight_inverse(1.0 / maxWeight),
        distanceMetric(factory->getDistanceMetric())
    {}
    ~Gater() {}

    Eigen::MatrixXd calculateWeights(QVector<Measurment<dim, T>*>& meases,
                              std::forward_list<Track<dim, T>>& confirmedTracks, int n_confirmed,
                              std::forward_list<Track<dim, T>>& tentativeTracks, int n_tentative) {

        int meases_size = meases.size();
        int tracks_size = n_confirmed + n_tentative;
        Eigen::MatrixXd weights(meases_size, tracks_size);

        int i = 0;

        for (auto* tracks : {&confirmedTracks, &tentativeTracks}) {
            for (Track<dim, T>& track : *tracks) {
                int j = 0;
                for (Measurment<dim, T>* meas : meases) {
                    Measurment<dim, T>* prediction = track.filter->predict(meas->getToa());
                    double distance = distanceMetric->distance(prediction, meas);
                    double gateRadius = getGateRadius(track, meas->getToa());
                    weights(j++, i) = getWeight(distance, gateRadius);
                }
                i++;
            }
        }

        return weights;
    }

    double getGateRadius(Track<dim, T>& track, TOA_t now) {
        if (track.filter->isInNormalState()) {
            double deltaT = double(now - track.filter->getPosition()->getToa());
            return std::min(gateRadiusPerTime * deltaT, maxGateRadius);
        }
        else {
            return tentativeRadius;
        }
    }

private:
    inline double getWeight(double distance, double gateRadius) {
        if (maxWeight_inverse < distance && distance < gateRadius)
            return 1.0 / distance;
        else if (distance >= gateRadius)
            return 0;
        else
            return maxWeight;
    }

    Factory<dim, T> const * factory;

    const double tentativeRadius; /// tentativeRadius should be larger than [gateRadiusPerTime * normal_deltaT]
    const double gateRadiusPerTime;
    const double maxGateRadius;
    const double maxWeight;
    const double maxWeight_inverse;

    DistanceMetric<dim, T>* distanceMetric;

    friend void dummy();
};

