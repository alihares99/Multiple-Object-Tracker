#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "Types.h"
#include "Tracker.h"
#include "Factory_StrictNoAmbiguity.h"

#include "Tester.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    Tester tester;
    tester.testFilter();
    tester.testAssignment();
    tester.testGater();
    tester.testTracker();
    tester.testMaintenance();

}

MainWindow::~MainWindow()
{
    delete ui;
}
