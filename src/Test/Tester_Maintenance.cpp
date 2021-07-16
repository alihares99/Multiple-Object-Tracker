#include "Tester_Maintenance.h"

using namespace std;

Tester_Maintenance::Tester_Maintenance() :
    maintenance(&factory, 2, 2, 2, 2)
{}

bool Tester_Maintenance::testMaintenance()
{
    qDebug() << endl << "*** Maintenance test ***";

    QVector<MyMeas> meases;
    MyMeas data;
    data.toa = 1;

    data.val = 1;
    data.trackId = 0;
    meases.append(data);

    data.val = 100;
    data.trackId = 0;
    meases.append(data);

    data.val = 200;
    data.trackId = 0;
    meases.append(data);

    auto meases_p = getPointerVector(meases);
    maintenance.maintain(confirmed, tentative, meases_p, data.toa);
    debug();

    for (TOA_t toa = 2; toa <= 20; toa++) {
        meases.clear();
        data.toa = toa;

        data.assignmentStatus = AssignmentStatus::assigned;

        data.val = 1;
        data.trackId = 1;
        meases.append(data);

        data.val = 100;
        data.trackId = 2;
        meases.append(data);

        data.assignmentStatus = AssignmentStatus::unassigned;

        data.val = 200;
        data.trackId = 0;
        meases.append(data);

        meases_p = getPointerVector(meases);
        updateFilters(meases_p);
        maintenance.maintain(confirmed, tentative, meases_p, data.toa);
    }

    debug();


    return true;
}

QVector<Measurment<1, double> *> Tester_Maintenance::getPointerVector(QVector<MyMeas> &meases) {
    QVector<Measurment<1, double>*> out;
    out.reserve(meases.size());
    for (auto& meas : meases) {
        out.append(&meas);
    }
    return out;
}

void Tester_Maintenance::updateFilters(QVector<Measurment<1, double> *> &meases)
{
    for (auto& meas : meases) {
        auto trackId = meas->trackId;
        bool found = false;
        for (auto& track : tentative) {
            if (track.trackId == trackId) {
                track.filter->update(meas);
                found = true;
                break;
            }
        }
        if (found) continue;
        for (auto& track : confirmed) {
            if (track.trackId == trackId) {
                track.filter->update(meas);
                break;
            }
        }
    }
}

void Tester_Maintenance::debug()
{
    qDebug() << "Confirmed:";
    int count = 0;
    for (auto& track : confirmed) {
        qDebug() << "    " << count++ << ": ID =" << track.trackId << "position =" << track.filter->getPosition()->getData(0);
    }

    qDebug() << "Tentative:";
    count = 0;
    for (auto& track : tentative) {
        qDebug() << "    " << count++ << ": ID =" << track.trackId << "position =" << track.filter->getPosition()->getData(0);
    }
}
