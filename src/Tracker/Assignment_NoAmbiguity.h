#pragma once

#include "Assignment.h"
#include "DistanceMetric.h"
#include "EnumClass.h"

#include <QVector>
#include <variant>
#include <optional>
#include <Eigen/Core>

template <int dim, class T>
class Assignment_NoAmbiguity : public Assignment<dim, T>
{
public:
    /**
     * @param closeRangeRatio: The ratio range in which measurments are considered ambiguous. Must be greater than 1.
     */
    Assignment_NoAmbiguity(Factory<dim, T>* factory, double closeRangeDistance, double closeRangeRatio) :
        factory(factory),
        distanceMetric(factory->getDistanceMetric()),
        closeRangeRatio(closeRangeRatio),
        closeRangeRatio_inverse(1.0 / closeRangeRatio),
        closeRangeDistance (closeRangeDistance)
    {}
    ~Assignment_NoAmbiguity() override {}

    virtual QVector<Edge> assign(Eigen::MatrixXd& weights,
                                 QVector<Measurment<dim, T>*>& meases)
    {
        const int n_meases = (int) weights.rows();
        const int n_tracks = (int) weights.cols();
        QVector<Edge> out;

        if (n_meases > 0 && n_tracks > 0) {
            QVector<IndexValid_Pair> candidateMeases(n_tracks);

            for (int i = 0; i < n_meases; i++) {
                auto res = getTrackIndex(weights, i);

                /// in case of int return value:
                if (int* trackIndex = std::get_if<int>(&res)) {
                    auto& candidateMeas = candidateMeases[*trackIndex];

                    switch (candidateMeas.valid) {
                    case TrackValid::invalid:
                        candidateMeas.index = i;
                        candidateMeas.valid = TrackValid::valid;
                        meases[i]->assignmentStatus = AssignmentStatus::assigned;
                        break;
                    case TrackValid::valid: {
                        double weight_new = weights(i, *trackIndex);
                        double weight_old = weights(candidateMeas.index, *trackIndex);
                        if (areClose(weight_new, weight_old)) {

                            /// mark the two meases as ambiguous:
                            meases[i                  ]->assignmentStatus = AssignmentStatus::ambiguous;
                            meases[candidateMeas.index]->assignmentStatus = AssignmentStatus::ambiguous;

                            /// mark the track ambiguous:
                            candidateMeas.valid = TrackValid::ambiguous;
                        }
                        else if (weight_new > weight_old) {
                            meases[candidateMeas.index]->assignmentStatus = AssignmentStatus::ambiguous;
                            candidateMeas.index = i;
                            meases[i]->assignmentStatus = AssignmentStatus::assigned;
                        }
                        else if (weight_new <= weight_old) {
                            meases[i]->assignmentStatus = AssignmentStatus::ambiguous;
                        }
                        break;
                    }
                    case TrackValid::ambiguous:
                        break;
                    }
                }

                /// in case of Error return value:
                else if (Error* err = std::get_if<Error>(&res)) {
                    if (*err == Error::ambiguous) {
                        meases[i]->assignmentStatus = AssignmentStatus::ambiguous;
                    }
                }
            }

            /// prepare out:
            int n_out = 0;
            for (const auto& candidateMeas : candidateMeases) {
                if (candidateMeas.valid == TrackValid::valid)
                    n_out++;
            }
            if (n_out > 0) {
                out.reserve(n_out);
                for (int i = 0; i < candidateMeases.size(); i++) {
                    const auto& candidateMeas = candidateMeases[i];
                    if (candidateMeas.valid == TrackValid::valid) {
                        Edge edge;
                        edge.meas_index = candidateMeas.index;
                        edge.track_index = i;
                        out.append(edge);
                    }
                }
            }
        }

        /// mark unassigned ambiguous measurments:
        for (int i = 0; i < meases.size(); i++) {
            if (meases.at(i)->assignmentStatus != AssignmentStatus::unassigned)
                continue;
            for (int j = i + 1; j < meases.size(); j++) {
                if (meases.at(j)->assignmentStatus != AssignmentStatus::unassigned)
                    continue;
                double distance = distanceMetric->distance(meases.at(i), meases.at(j));
                if (areClose(distance)) {
                    meases.at(i)->assignmentStatus = AssignmentStatus::ambiguous;
                    meases.at(j)->assignmentStatus = AssignmentStatus::ambiguous;
                }
            }
        }

        return out;
    }

private:
    EnumClass Error {
        enum Value : int {
            noTrackFound,
            ambiguous,
        };
        ENUM_CLASS(Error)
    };

    EnumClass TrackValid {
        enum Value {
            invalid,
            valid,
            ambiguous
        };
        ENUM_CLASS(TrackValid)
    };

    struct IndexValid_Pair {
        int index;
        TrackValid valid = TrackValid::invalid;
    };

    std::variant<int, Error> getTrackIndex(Eigen::MatrixXd& weights, int meas_index)
    {
        const int n_tracks = (int) weights.cols();

        bool found = false;
        int foundIndex = 0;
        double foundWeight = 0;

        for (int i = 0; i < n_tracks; i++) {
            double weight = weights(meas_index, i);
            if (weight > 0) {
                if (!found) {
                    found = true;
                    foundIndex = i;
                    foundWeight = weight;
                }
                else {
                    if (areClose(weight, foundWeight)) {
                        return Error(Error::ambiguous);
                    }
                    else if (weight > foundWeight) {
                        foundIndex = i;
                        foundWeight = weight;
                    }
                }
            }
        }

        if (found) {
            return foundIndex;
        }
        else {
            return Error(Error::noTrackFound);
        }
    }
    bool areClose(double weight1, double weight2)
    {
        return (closeRangeRatio_inverse * weight1 < weight2 && weight2 < closeRangeRatio * weight1);
    }
    bool areClose(double distance) {
        return distance < closeRangeDistance;
    }

    Factory<dim, T> const * factory;
    DistanceMetric<dim, T> * const distanceMetric;
    const double closeRangeRatio;
    const double closeRangeRatio_inverse;
    const double closeRangeDistance;
};

