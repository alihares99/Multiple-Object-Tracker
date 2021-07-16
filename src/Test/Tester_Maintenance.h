#pragma once

#include "Tester_Types.h"
#include <QVector>

#include "Factory_StrictNoAmbiguity.h"
#include "Maintenance_History.h"

class Tester_Maintenance
{
public:
    Tester_Maintenance();
    bool testMaintenance();

private:
    QVector<Measurment<1, double>*> getPointerVector(QVector<MyMeas>& meases);
    void updateFilters(QVector<Measurment<1, double>*>& meases);
    void debug();

    Factory_StrictNoAmbiguity factory;
    Maintenance_History<1, double> maintenance;

    std::forward_list<Track<1, double>> confirmed;
    std::forward_list<Track<1, double>> tentative;
};

