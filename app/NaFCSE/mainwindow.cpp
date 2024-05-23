#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtCharts>
#include <QSplineSeries>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ////////////////// DRAW SPLINE CHART ///////////////////////
    QSplineSeries* series = new QSplineSeries();
    series->setName("spline");

    // add spline data points
    series->append(0,6);
    series->append(2,4);
    series->append(3,8);
    series->append(7,4);
    series->append(10,5);

    *series<<QPoint(11,1)<<QPoint(13,3)<<QPoint(17,6)<<QPoint(18,3)<<QPoint(20,2);

    // create the actual chart
    QChart* chart = new QChart();
    chart->legend()->hide();
    chart->addSeries(series);
    chart->setTitle("Acceleration");
    chart->createDefaultAxes();
    chart->axes(Qt::Vertical).first()->setRange(0, 10);

    // create qchartview
    QChartView* chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing); // make lines look nicer

    ui->accelerationChart->addWidget(chartView);




    // create dummy graph 2
    QSplineSeries* series2 = new QSplineSeries();
    series2->setName("spline");

    // add spline data points
    series2->append(0,6);
    series2->append(2,4);
    series2->append(3,8);
    series2->append(7,4);
    series2->append(10,5);

    *series2<<QPoint(11,1)<<QPoint(13,3)<<QPoint(17,6)<<QPoint(18,3)<<QPoint(20,2);

    // create the actual chart
    QChart* chart2 = new QChart();
    chart2->legend()->hide();
    chart2->addSeries(series2);
    chart2->setTitle("Acceleration");
    chart2->createDefaultAxes();
    chart2->axes(Qt::Vertical).first()->setRange(0, 10);

    // create qchartview
    QChartView* chartView2 = new QChartView(chart2);
    chartView2->setRenderHint(QPainter::Antialiasing); // make lines look nicer


    ui->simulatedDataChart->addWidget(chartView2);

}

MainWindow::~MainWindow()
{
    delete ui;
}

