#include "PlotDialog.h"
#include "PlotDialogColors.h"
#include "ui_PlotDialog.h"
#include "qcustomplot.h"

#include <QDebug>
#include <QTimer>

static QCPScatterStyle::ScatterShape getScatterShape(PlotDialog::Shape shape) {
    switch (shape) {
    case PlotDialog::shape_circle:
        return QCPScatterStyle::ssCircle;
    case PlotDialog::shape_cross:
        return QCPScatterStyle::ssCross;
    case PlotDialog::shape_star:
        return QCPScatterStyle::ssStar;
    }
}

PlotDialog::PlotDialog(const QVector<PlotDialog::PlotDataType> &allPlotDatas, QWidget *parent):
    QDialog(parent),
    allDatas(allPlotDatas),
    ui(new Ui::PlotDialog)
{
    ui->setupUi(this);

    onLockVerticallyBtnClicked();
    ui->trackerPlot->setInteraction(QCP::Interaction::iRangeDrag);
    ui->trackerPlot->setInteraction(QCP::Interaction::iRangeZoom);

    connect(ui->rescale_Button, &QPushButton::clicked, this, &PlotDialog::onRescaleBtnClicked);
    connect(ui->lockVert_Button, &QPushButton::clicked, this, &PlotDialog::onLockVerticallyBtnClicked);
    connect(ui->toggle_Button, &QPushButton::clicked, this, &PlotDialog::onToggleButtonClicked);

    preparePlot();
    onRescaleBtnClicked();
}

PlotDialog::~PlotDialog()
{
    delete ui;
}

void PlotDialog::onRescaleBtnClicked()
{
    ui->trackerPlot->rescaleAxes();
    ui->trackerPlot->replot();
}

void PlotDialog::onLockVerticallyBtnClicked()
{
    if (ui->lockVert_Button->text() == "lock vertically") {
        ui->trackerPlot->axisRect()->setRangeZoom(Qt::Horizontal);
        ui->lockVert_Button->setText("unlock vertically");
    }
    else {
        ui->trackerPlot->axisRect()->setRangeZoom(Qt::Vertical | Qt::Horizontal);
        ui->lockVert_Button->setText("lock vertically");
    }
}

void PlotDialog::onCheckBoxClicked(int state)
{
    if (checkboxCallbackEnable) {
        Q_UNUSED(state)
        for (auto it = checkBoxBuffer.constBegin(); it != checkBoxBuffer.constEnd(); it++) {
            const Tag& tag = it.key();
            const auto& checkBox = it.value();

            bool visible = checkBox->isChecked();
            graphBuffer[tag]->setVisible(visible);

            if (errorBars.contains(tag)) {
                errorBars[tag]->setVisible(visible);
            }
        }

        ui->trackerPlot->replot();
    }
}

void PlotDialog::onToggleButtonClicked()
{
    const auto& lastCheckBox = (checkBoxBuffer.end() - 1).value();
    bool val = !lastCheckBox->isChecked();

    checkboxCallbackEnable = false;
    for (auto& checkbox : checkBoxBuffer) {
        checkbox->setChecked(val);
    }

    QTimer::singleShot(100, [this](){
        checkboxCallbackEnable = true;
        onCheckBoxClicked(0);
    });
}

void PlotDialog::preparePlot()
{
//    for(auto plotData: allDatas){
//            if((plotData.tag % 3 == 0)){
//                if(!trackCount.contains(plotData.tag / 3)){
//                    trackCount[plotData.tag / 3] = 1;
//                }
//                else {
//                    trackCount[plotData.tag / 3]++;
//                }
//            }

//    }
    QVBoxLayout *vbox = new QVBoxLayout;
    QSpacerItem *vSpacer = new QSpacerItem(100, 100, QSizePolicy::Minimum, QSizePolicy::Expanding);
    int colorCounter = 0;

    for(auto plotData: allDatas){
//        if(trackCount[plotData.tag / 3] < 10)
//            continue;
        if(!checkBoxBuffer.contains(plotData.tag)){
            auto graph = ui->trackerPlot->addGraph();

            graph->setLineStyle(plotData.connected ? QCPGraph::lsLine : QCPGraph::lsNone);
            graph->setScatterStyle(QCPScatterStyle(getScatterShape(plotData.shape), circleSize));
            QPen pen;
            pen.setColor(PlotDialogNS::colors[colorCounter++ % PlotDialogNS::colors.size()]);
            graph->setPen(pen);
            graphBuffer[plotData.tag] = graph;

            if (plotData.error) {
                QCPErrorBars* errorBar = new QCPErrorBars(ui->trackerPlot->xAxis, ui->trackerPlot->yAxis);
                errorBar->removeFromLegend();
                errorBar->setAntialiased(false);
                errorBar->setDataPlottable(graph);
                errorBar->setPen(QPen(QColor(180,180,180)));

                errorBars[plotData.tag] = errorBar;
            }

            QCheckBox* checkBox = new QCheckBox();
            checkBox->setText(QString("Track %1").arg(plotData.tag));
            checkBox->setChecked(plotData.tag % 3 == 0 ? true : false);
            checkBoxBuffer[plotData.tag] = checkBox;
            connect(checkBoxBuffer[plotData.tag], &QCheckBox::stateChanged, this, &PlotDialog::onCheckBoxClicked);
        }
        graphBuffer[plotData.tag]->addData(plotData.x, plotData.y);
        if (plotData.error) {
            errorBars[plotData.tag]->addData(*plotData.error);
        }

        /// Text Label on each data:
        /*
        QCPItemText* textLabel = new QCPItemText(ui->trackerPlot);
        textLabel->setPositionAlignment(Qt::AlignTop|Qt::AlignHCenter);
        textLabel->position->setType(QCPItemPosition::PositionType::ptPlotCoords);
        textLabel->position->setCoords(plotData.x, plotData.y);
        textLabel->setText(QString::number(plotData.tag));
        textLabel->setFont(QFont(font().family(), 8));
        */
    }

    /// sort the checkboxes before adding it to the layout:
    QVector<QCheckBox*> checkboxVec;
    checkboxVec.reserve(checkBoxBuffer.size());
    for (auto* checkbox : checkBoxBuffer) {
        checkboxVec.append(checkbox);
    }
    std::sort(checkboxVec.begin(), checkboxVec.end(), [](QCheckBox* a, QCheckBox* b) {
        int a_val = a->text().mid(6).toInt();
        int b_val = b->text().mid(6).toInt();
        return a_val < b_val;
    });
    for (QCheckBox* checkbox : checkboxVec) {
        vbox->addWidget(checkbox);
    }
    vbox->addItem(vSpacer);
    ui->tracksGroupBox->setLayout(std::move(vbox));
    ui->trackerPlot->replot();

}

void PlotDialog::on_plot_Button_clicked()
{
    for (auto it = checkBoxBuffer.begin(); it != checkBoxBuffer.end(); it++) {
        auto& tag = it.key();
        auto& checkbox = it.value();

        bool visible = checkbox->isChecked();
        graphBuffer[tag]->setVisible(visible);

        if (errorBars.contains(tag)) {
            errorBars[tag]->setVisible(visible);
        }
    }

    ui->trackerPlot->replot();
}
