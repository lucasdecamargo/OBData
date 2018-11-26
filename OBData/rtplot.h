#ifndef RTPLOT_H
#define RTPLOT_H

#include <QWidget>
#include <QVector>
#include "qcustomplot.h"

class RTPlot : public QCustomPlot
{
    Q_OBJECT
public:
    explicit RTPlot(QWidget *parent = nullptr);
    ~RTPlot();
    void makePlot();
    QCPGraph *addGraph();
    QVector<double> getxData() const;
    QVector<double> getyData() const;
    double getIntegral(double i = 0, double f = -1);
    void setData(QVector<double> &x, QVector<double> &y, bool alreadySorted = true);


signals:
    void contextMenu(const QPoint&, RTPlot *rt);

public slots:
    void plot(double value, int ngraph = 0);
    void stop();
    void resume();
    void clean();
    void setScroll(int val);
    void rescale();
    void contextMenuRequested(const QPoint&);
    void setAverage(bool set, double period = 0);
    void calculateAverage();

    static void initiate();
    static void restart();
    static void setOnlyRead(bool);


private slots:
   void scroll_xaxis();

protected:
    QVector<double> xdata;
    QVector<double> ydata;

private:
    static QTime time;
    static bool running;
    static bool onlyRead;

    QTimer *dataTimer;
    QTime *time_aux;

    double time_aux_passed;
    double lastPointKey;
    double lastlastPointKey;
    double last_value;

    bool status;    
    int scroll;


    // Variaveis para o calculo da media
    bool average;
    int average_graph;
    double average_val;
    double average_period;
    int average_count;
    double average_last;
};

#endif // RTPLOT_H
