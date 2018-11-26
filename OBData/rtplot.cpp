#include "rtplot.h"

QTime RTPlot::time;
bool RTPlot::running = false;
bool RTPlot::onlyRead = false;

RTPlot::RTPlot(QWidget *parent) : QCustomPlot(parent),
    dataTimer(new QTimer(this)), time_aux(nullptr),
    time_aux_passed(0.0), status(true)
{
    lastPointKey = 0;
    lastlastPointKey = 0;
    scroll = 8;
    last_value = 0;

    average = false;
    average_period = 0;
    average_val = 0;
    average_last = 0;
    average_count = 0;
    average_graph = -1;

    makePlot();
    addGraph();
}

RTPlot::~RTPlot()
{
    delete dataTimer;
    if(time_aux != nullptr) delete time_aux;
}

void RTPlot::makePlot()
{

    // The base class tick generator used by QCPAxis to create tick positions and tick labels.
    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    xAxis->setTicker(timeTicker);
    // Convenience function to create an axis on each side that doesn't have any axes yet and set their visibility to true.
    axisRect()->setupFullAxesBox();
    yAxis->setRange(-0.1,1.2);
    xAxis->setRange(0, scroll, Qt::AlignRight);

    // make left and bottom axes transfer their ranges to right and top axes:
    connect(xAxis, SIGNAL(rangeChanged(QCPRange)), xAxis2, SLOT(setRange(QCPRange)));
    connect(yAxis, SIGNAL(rangeChanged(QCPRange)), yAxis2, SLOT(setRange(QCPRange)));
    connect(dataTimer, SIGNAL(timeout()), this, SLOT (scroll_xaxis()));

    // user interactions:
    setInteraction(QCP::iRangeDrag, true);
    setInteraction(QCP::iRangeZoom, true);
}

QCPGraph * RTPlot::addGraph()
{
    QCPGraph * gr = QCustomPlot::addGraph();
    if(graphCount() == 1) graph(0)->setPen(QPen(Qt::GlobalColor::blue));
    return gr;
}

QVector<double> RTPlot::getxData() const
{
    return xdata;
}
QVector<double> RTPlot::getyData() const
{
    return ydata;
}

void RTPlot::initiate()
{
    time.start();
    running = true;
}

void RTPlot::restart()
{
    //time.restart();
    time = QTime();
    running = false;
    onlyRead = false;
}

void RTPlot::setOnlyRead(bool b)
{
    onlyRead = b;
}

void RTPlot::clean()
{
    dataTimer->stop();
    if(time_aux) delete time_aux;
    time_aux = nullptr;
    time_aux_passed = 0;
    status = true;
    lastPointKey = 0;
    lastlastPointKey = 0;
    scroll = 8;
    last_value = 0;
    ydata.clear();
    xdata.clear();

    QCustomPlot::clearGraphs();
    addGraph();
    yAxis->setRange(-0.1,1.2);
    xAxis->setRange(0, scroll, Qt::AlignRight);

    bool av = average;
    average = false;
    average_last = 0;
    average_count = 0;
    average_val = 0;
    average_graph = -1;
    setAverage(av,average_period);

    replot();
}

void RTPlot::setAverage(bool set, double period)
{
    if(set){
        if(!average){
            average = true;
            addGraph();
            average_graph = graphCount() -1;
            graph(average_graph)->setPen(QPen(Qt::GlobalColor::red));
            graph(average_graph)->setName("Média");
            graph(average_graph)->setLineStyle(QCPGraph::lsLine);
            graph(average_graph)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 5));
        }
        average_period = period;
    }
    else{
        average = false;
        if(average_graph != -1){
            removeGraph(graph(average_graph));
            average_graph = -1;
            replot();
        }
    }

    if(!set || period == 0.0){
        average_last = 0;
        average_count = 0;
        average_val = 0;
        average_period = 0;
    }
}

void RTPlot::plot(double value, int ngraph)
{
    if(!status || onlyRead) return;

    // calculate two new data points
    double key = time.elapsed()/1000.0 - time_aux_passed; // time elapsed since start of demo, in seconds
    lastPointKey = key;

    graph(ngraph)->addData(key,value);
    graph(ngraph)->rescaleValueAxis(true);
    if(ngraph == 0) last_value = value;

    if(key - lastlastPointKey > 0.01 && ngraph == 0){
        QTime t(0,0);
        t = t.addMSecs(int(key*1000));
        xdata.append(key);
        ydata.append(value);

        lastlastPointKey = key;

    }

    if(average && average_period != 0.0){
            average_val += value;
            average_count++;
    }

    // make key axis range scroll with the data (at a constant range size of 8):
    xAxis->setRange(key, scroll, Qt::AlignRight);
    replot();
}

void RTPlot::scroll_xaxis()
{
    if (!running || !status) return;

    double key = time.elapsed()/1000.0 - time_aux_passed;

    if(key - lastPointKey > 0.002) // at most add point every 2 ms
    {
        if(!onlyRead){
            graph(0)->addData(key,last_value);

            if((average && average_period != 0.0) && (key - average_last > average_period)){
                average_last = key;
                if(average_count == 0) average_val = 0;
                else{
                    average_val /= average_count;
                    graph(average_graph)->addData(key,average_val);
                }
                average_val = 0;
                average_count = 0;
            }
        }

        lastPointKey = key;
        xAxis->setRange(key, scroll, Qt::AlignRight);
        replot();
    }
}

void RTPlot::stop()
{
    dataTimer->stop();
    if(time_aux == nullptr){
        status = false;
        time_aux = new QTime(QTime::currentTime());
    }
}

void RTPlot::resume()
{
    dataTimer->start();
    if(time_aux != nullptr){
        time_aux_passed += time_aux->elapsed()/1000.0 ;
        delete time_aux;
        time_aux = nullptr;
        status = true;
    }
}

void RTPlot::setScroll(int val)
{
    scroll = val;
    xAxis->setRange(xAxis->tickVector().last(), scroll, Qt::AlignRight);
    replot();
}

void RTPlot::rescale()
{
    for(int i = 0; i < graphCount(); i++){
        if(i == 0)
            graph(i)->rescaleValueAxis();
        else
            graph(i)->rescaleValueAxis(true);
    }
    replot();
}

void RTPlot::contextMenuRequested(const QPoint& point)
{
    emit contextMenu(point,this);
}

void RTPlot::calculateAverage()
{
    if(!average && average_period == 0.0) return;

    QVector<double> _x;
    QVector<double> _y;

    double t = 0;
    double val = 0;
    int count = 0;

    for(int i = 0; i < xdata.size(); i++){
        if(xdata[i] - t < average_period){
            val += ydata[i];
            count++;
        }
        else{
            if(count != 0){
                val /= count;
                _x.append(xdata[--i]);
                _y.append(val);
            }
            val = 0;
            count = 0;
            t = xdata[i];
        }
    }

    graph(average_graph)->setData(_x,_y,true);

    replot();
}

void RTPlot::setData(QVector<double> &x, QVector<double> &y, bool alreadySorted)
{
    xdata = x;
    ydata = y;
    graph(0)->setData(x,y,alreadySorted);
    xAxis->setRange(0, scroll, Qt::AlignLeft);
    replot();
}

double RTPlot::getIntegral(double i, double f)
{
    int size = xdata.size();
    if(size == 0) return -1;
    if(f >= xdata[size -1]) return -1;
    if(int(f) != -1 && i >= f) return -1;

    double val = 0;

    int j = 0;
    if(i != 0.0)
        while(xdata[++j] < i){}
    else j =1;

    if(int(f) != -1){
        size = 0;
        while(xdata[++size] < f){}
    }

    for(; j < size; j++){
        val += (ydata[j] + ydata[j-1])*(xdata[j] - xdata[j-1])/2;
    }

    return val;
}
