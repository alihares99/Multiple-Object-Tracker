#include "Tester_Assignment.h"
#include "Tester_Types.h"

#include "Factory_StrictNoAmbiguity.h"
#include "Assignment_NoAmbiguity.h"

static inline QDebug& operator<<(QDebug& debug, const Edge& edge)
{
    QDebugStateSaver saver(debug);
    debug.noquote().nospace() << "meas " << edge.meas_index + 1 << " to Track " << edge.track_index + 1;
    return debug;
}

static inline void debug(const QVector<Edge>& edges)
{
    qDebug() << "Edges are:";
    for (auto& edge : edges) {
        qDebug() << "    " << edge;
    }
}

bool Tester_Assignment::testAssignment()
{
    test1();
    test2();
    return true;
}

bool Tester_Assignment::test1()
{

    qDebug() << endl << "*** Assignment test ***";

    Factory_StrictNoAmbiguity factory;
    auto assignment = factory.getAssignment();

    QVector<MyMeas> meases(3);
    QVector<Measurment<1, double>*> meases_p;
    meases_p.reserve(meases.size());
    for (auto& var : meases) {
        meases_p.append(&var);
    }

    Eigen::MatrixXd weights(3, 5);
    weights << 0, 0, 0, 1, 0,
               1, 0, 0, 0, 0,
               0, 1, 0, 0, 0;

    auto edges = assignment->assign(weights, meases_p);
    debug(edges);

    weights << 0, 10, 0, 0, 0,
               1, 10, 10, 0, 0,
               0, 1, 0, 0, 10;

    edges = assignment->assign(weights, meases_p);
    debug(edges);


    return true;
}

bool Tester_Assignment::test2()
{
    Factory_StrictNoAmbiguity factory;
    auto assignment = factory.getAssignment();

    QVector<MyMeas> meases;
    MyMeas data;
    data.toa = 1;

    data.val = 1;
    meases.append(data);
    data.val = 1;
    meases.append(data);
    data.val = 200;
    meases.append(data);
    data.val = 200;
    meases.append(data);
    data.val = 300;
    meases.append(data);

    QVector<Measurment<1, double>*> meases_p;
    meases_p.reserve(meases.size());
    for (auto& var : meases) {
        meases_p.append(&var);
    }

    auto weights = Eigen::MatrixXd(0, 0);
    auto edges = assignment->assign(weights, meases_p);
    for (int i = 0; i < meases.size(); i++) {
        auto& meas = meases[i];
        if (meas.assignmentStatus == AssignmentStatus::ambiguous) {
            qDebug() << "ambiguous data at" << i << "TOA" <<  meas.getToa() << "value" << meas.getData(0);
        }
    }

    return true;
}
