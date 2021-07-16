#include "Tester_Filter.h"
#include "Tester_Types.h"

#include "Filter_CVAB.h"
#include "PlotDialog.h"

bool Tester_Filter::testFilter()
{

    qDebug() << endl << "*** Filter test ***";

    Filter_CVAB<1, double> filter({0.5}, {0.01});

    QVector<MyMeas> meases(1000);
    for (int i = 0; i < meases.size(); i++) {
        meases[i].val = 100 * sin(i * M_PI / 180.0) + random_0_1() * 5;
        meases[i].toa = (TOA_t) i;
    }

    enum : int {
        tag_measurment,
        tag_prediction,
        tag_filterPosition,
    };
    QVector<PlotDialog::PlotDataType> plot_meas; plot_meas.reserve(meases.size());
    QVector<PlotDialog::PlotDataType> plot_prediction; plot_prediction.reserve(meases.size());
    QVector<PlotDialog::PlotDataType> plot_filterPosition; plot_filterPosition.reserve(meases.size());

    for (auto meas : meases) {
        auto prediction = filter.predict(meas.getToa());
        if (prediction) {
            PlotDialog::PlotDataType data;
            data.x = prediction->getToa();
            data.y = prediction->getData(0);
            data.tag = tag_prediction;
            plot_prediction.append(data);
        }

        filter.update(&meas);

        {
            PlotDialog::PlotDataType data;
            data.x = meas.getToa();
            data.y = meas.getData(0);
            data.tag = tag_measurment;
            plot_meas.append(data);
        }

        auto pos = filter.getPosition();
        if (pos) {
            PlotDialog::PlotDataType data;
            data.x = pos->getToa();
            data.y = pos->getData(0);
            data.tag = tag_filterPosition;
            plot_filterPosition.append(data);
        }
    }

    QVector<PlotDialog::PlotDataType> plotData;
    plotData.reserve(meases.size() * 3);

    plotData.append(plot_meas);
    plotData.append(plot_prediction);
    plotData.append(plot_filterPosition);

    PlotDialog dialog(plotData);
    dialog.exec();

    return true;
}
