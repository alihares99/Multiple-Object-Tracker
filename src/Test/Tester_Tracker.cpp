#include "Factory_StrictNoAmbiguity.h"
#include "Tester_Tracker.h"
#include "Tester_Types.h"

#include "PlotDialog.h"
#include "Tracker.h"


class Factory_TrackerTest : public Factory<1, double>
{
public:
    Factory_TrackerTest() {}
    ~Factory_TrackerTest() override {}

    std::unique_ptr<Gater<1, double>> getGater() override {
        return std::unique_ptr<Gater<1, double>>(new Gater<1, double>(this, 5.0, 20, 30.0, 10000)); /// max gate radius
    }
    std::unique_ptr<Assignment<1, double>> getAssignment() override {
        return std::unique_ptr<Assignment<1, double>>(new Assignment_NoAmbiguity<1, double>(this, 5.0, 2.0));
    }
    std::unique_ptr<Filter<1, double>> getFilter() override {
        return std::unique_ptr<Filter<1, double>>(new Filter_CVAB<1, double>({0.4}, {0.05}));
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


bool Tester_Tracker::testTracker()
{

    qDebug() << endl << "*** Tracker (All parts) test ***";

    /// construct measurments:
    const int n = 1000;
    QVector<MyMeas> track1(n);
    QVector<MyMeas> track2(n);
    for (int i = 0; i < track1.size(); i++) {
        track1[i].val = 100 * sin(i * M_PI / 180.0) + random_0_1() * 1.0;
        track1[i].toa = (TOA_t) i;
    }
    for (int i = 0; i < track2.size(); i++) {
        track2[i].val = 100 * cos(i * M_PI / 180.0) + random_0_1() * 1.0;
        track2[i].toa = (TOA_t) i;
    }

    QVector<PlotDialog::PlotDataType> plotData;
    plotData.reserve(n * 20);

    /// call the tracker:
    auto factory = std::make_shared<Factory_TrackerTest>();
    Tracker<1, double> tracker(factory);
    QVector<Measurment<1, double>*> newMeas(2);
    for (int i = 0; i < n; i++) {
        TOA_t now = (TOA_t) i;

        /// GUI: prepare filter predictions:
        {
            auto& confirmed = tracker.getConfirmedTracks();
            auto& tentative = tracker.getTentativeTracks();
            for (auto* tracks : {&confirmed, &tentative}) {
                for (auto& track : *tracks) {
                    PlotDialog::PlotDataType data;
                    data.x = now;
                    data.y = track.filter->predict(now)->getData(0);
                    data.tag = track.trackId * 3 + 1;
                    data.connected = true;
                    data.shape = PlotDialog::shape_cross;

                    auto* gater = tracker.getGater();
                    data.error = gater->getGateRadius(track, now);

                    plotData.append(data);
                }
            }
        }

        /// call the tracker to do its job:
        newMeas[0] = &track1[i];
        newMeas[1] = &track2[i];
        tracker.track(newMeas, now);

        /// after the tracker is called, assignmentStatus is changed:
        Q_ASSERT(newMeas[0]->assignmentStatus != AssignmentStatus::unassigned);
        Q_ASSERT(newMeas[1]->assignmentStatus != AssignmentStatus::unassigned);

        /// GUI: prepare filter positions:
        {
            auto& confirmed = tracker.getConfirmedTracks();
            auto& tentative = tracker.getTentativeTracks();
            for (auto* tracks : {&confirmed, &tentative}) {
                for (auto& track : *tracks) {
                    if (track.filter->isUpdated()) {
                        PlotDialog::PlotDataType data;
                        data.x = now;
                        data.y = track.filter->getPosition()->getData(0);
                        data.tag = track.trackId * 3 + 2;
                        data.connected = true;
                        data.shape = PlotDialog::shape_star;

                        plotData.append(data);
                    }
                }
            }
        }

    }

    /// show data:
    QVector<MyMeas> allTracks;
    allTracks.reserve(n * 2);
    allTracks.append(track1);
    allTracks.append(track2);
    for (auto& meas : allTracks) {
        PlotDialog::PlotDataType data;
        data.x = meas.getToa();
        data.y = meas.getData(0);

        if (meas.assignmentStatus == AssignmentStatus::ambiguous) {
            data.tag = 0;
            data.shape = PlotDialog::shape_circle;
            data.connected = false;
        }
        else {
            data.tag = meas.trackId * 3;
            data.shape = PlotDialog::shape_circle;
            data.connected = true;
        }
        plotData.append(data);
    }

    PlotDialog dialog(plotData);
    dialog.exec();

    return true;
}
