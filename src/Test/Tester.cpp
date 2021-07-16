#include "Tester.h"

#include "Tester_Filter.h"
#include "Tester_Assignment.h"
#include "Tester_Gater.h"
#include "Tester_Tracker.h"
#include "Tester_Maintenance.h"

Tester::Tester(){}

bool Tester::testFilter()
{
    return Tester_Filter::testFilter();
}

bool Tester::testAssignment()
{
    return Tester_Assignment::testAssignment();
}

bool Tester::testGater()
{
    return Tester_Gater::testGater();
}

bool Tester::testMaintenance()
{
    Tester_Maintenance tester;
    return tester.testMaintenance();
}

bool Tester::testTracker()
{
    return Tester_Tracker::testTracker();
}


