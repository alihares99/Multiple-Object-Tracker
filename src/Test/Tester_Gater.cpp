#include "Tester_Gater.h"
#include "Tester_Types.h"

#include "Factory_StrictNoAmbiguity.h"
#include "Filter_CVAB.h"
#include "Gater.h"
#include "Track.h"

bool Tester_Gater::testGater()
{
    Factory_StrictNoAmbiguity factory;
    Gater<1, double> gater(&factory, 15, 50, 50, 100);

    /// new measurments:
    QVector<MyMeas> meases;
    QVector<Measurment<1, double>*> meases_p;
    {
        MyMeas tmp;
        tmp.toa = 5;
        tmp.val = 10;
        meases.append(tmp);
        tmp.val = 20;
        meases.append(tmp);
        tmp.val = 25;
        meases.append(tmp);

        meases_p.reserve(meases.size());
        for (auto& var : meases) {
            meases_p.append(&var);
        }
    }

    /// create tracks:
    std::forward_list<Track<1, double>> confirmedTracks;
    std::forward_list<Track<1, double>> tentativeTracks;
    std::array<double, 1> alpha = {0.5};
    std::array<double, 1> beta = {0.05};
    confirmedTracks.push_front(Track<1, double>(std::unique_ptr<Filter_CVAB<1, double>> (new Filter_CVAB<1, double>(alpha, beta))));
    confirmedTracks.push_front(Track<1, double>(std::unique_ptr<Filter_CVAB<1, double>> (new Filter_CVAB<1, double>(alpha, beta))));
    confirmedTracks.push_front(Track<1, double>(std::unique_ptr<Filter_CVAB<1, double>> (new Filter_CVAB<1, double>(alpha, beta))));
    confirmedTracks.push_front(Track<1, double>(std::unique_ptr<Filter_CVAB<1, double>> (new Filter_CVAB<1, double>(alpha, beta))));

    /// update tracks:
    {
        MyMeas tmp;
        for (int i = 1; i <= 4; i++) {
            tmp.toa = (TOA_t) i;
            int j = 0;
            for (auto& track : confirmedTracks) {
                j++;
                tmp.val = j * 2 + i;
                track.filter->update(&tmp);
            }
        }
    }

    /// call gater:
    auto weights = gater.calculateWeights(meases_p, confirmedTracks, 4, tentativeTracks, 0);

    /// print the results:
    qDebug() << endl << "*** Gater test ***";
    qDebug() << "weights are:";
    std::stringstream s;
    s << weights;
    qDebug() << s.str().c_str();
    qDebug() << "new measurments are:";
    TOA_t newToa = 0;
    for (auto& meas : meases) {
        qDebug() << meas.getToa() << meas.getData(0);
        newToa = meas.getToa();
    }
    qDebug() << "predictions are";
    for (auto& track : confirmedTracks) {
        auto prediction = track.filter->predict(newToa);
        if (prediction) {
            qDebug() << prediction->getToa() << prediction->getData(0);
        }
    }
    qDebug() << "filter positions are:";
    for (auto& track : confirmedTracks) {
        auto position = track.filter->getPosition();
        if (position) {
            qDebug() << position->getToa() << position->getData(0);
        }
    }

    return true;
}
