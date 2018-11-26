#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "obdata.h"
#include "rtplot.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // OBDATA
    void RTPlotSlot();
    void ConnectionStatus();
    void ProtocolName();
    void PortName();

    // QProcess
    void processOutput();

    /*
     * INTERFACE GRÁFICA
     */
    void RTPlotContextMenu(const QPoint&, RTPlot*);
    void on_scroll_Slider_valueChanged(int value);
    void on_conectar_Button_clicked();
    void on_desconectar_Button_clicked();
    void on_actionSalvar_log_triggered();
    void on_timeEdit_userTimeChanged(const QTime &time);
    void on_timeEdit_2_userTimeChanged(const QTime &time);
    void on_okMediaButton_clicked();
    void on_calculoTotalButton_clicked();
    void on_calculoPerButton_clicked();

    // Actions
    void on_actionAbrir_log_triggered();
    void on_actionIniciar_triggered();
    void on_actionContinuar_triggered();
    void on_actionPausar_triggered();
    void on_actionReiniciar_triggered();
    void on_actionReescalar_triggered();    
    void on_actionAbout_Qt_triggered();
    void on_actionAjuda_triggered();
    /* */


private:
    Ui::MainWindow *ui;
    OBData obdata;
    QTimer *dataTimer;
    QProcess obdatapy;
    QVector<RTPlot*> rt;
    bool pausado;
    int color_count;

    static QVector<Qt::GlobalColor> color;

    // func auxiliares
    bool aviso_pausar();
    void conectar_graficos(bool);
};

#endif // MAINWINDOW_H
