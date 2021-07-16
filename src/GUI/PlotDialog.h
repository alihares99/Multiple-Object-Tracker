#pragma once

#include <QDialog>
#include <QVector>
#include <memory>
#include <optional>

class QCheckBox;
class QRadioButton;
class QCPItemText;
class QCPGraph;
class QCPScatterStyle;
class QCPErrorBars;


namespace Ui {
class PlotDialog;
}

class PlotDialog : public QDialog
{
    Q_OBJECT

public:
    using Tag = int;
    using TrackId = int;

    enum Shape {
        shape_circle,
        shape_cross,
        shape_star,
    };

    struct PlotDataType{
        double x;
        double y;
        Tag tag;
        bool connected = true;
        Shape shape = shape_circle;
        std::optional<double> error;
    };

    PlotDialog(const QVector<PlotDataType>& allPlotDatas, QWidget* parent = nullptr);
    ~PlotDialog();
private slots:
    void onRescaleBtnClicked();
    void onLockVerticallyBtnClicked();
    void onCheckBoxClicked(int state);
    void onToggleButtonClicked();
    void on_plot_Button_clicked();

private:
    void preparePlot();

    bool checkboxCallbackEnable = true;
    int circleSize = 8;

    QHash<Tag, QCPGraph*> graphBuffer;
    QHash<Tag, QCPErrorBars*> errorBars;
    QHash<Tag, QCheckBox*> checkBoxBuffer;
    QHash<TrackId, int> trackCount;
    QVector<PlotDataType> allDatas;

    Ui::PlotDialog *ui;
};

