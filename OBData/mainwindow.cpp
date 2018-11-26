#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "obdata_adaptor.h"

QVector<Qt::GlobalColor> MainWindow::color = {Qt::GlobalColor::blue,
                                             Qt::GlobalColor::green,
                                             Qt::GlobalColor::yellow,
                                             Qt::GlobalColor::magenta,
                                             Qt::GlobalColor::cyan};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    pausado(true),
    color_count(0)
{
    ui->setupUi(this);    dataTimer = new QTimer(this);
    ui->menuArquivo->addAction(ui->actionAbrir_log);
    ui->menuArquivo->addAction(ui->actionSalvar_log);

    ui->actionIniciar->setEnabled(true);
    ui->actionReiniciar->setEnabled(false);
    ui->actionPausar->setEnabled(false);
    ui->actionContinuar->setEnabled(false);

    // Cria um interface adaptor
    new OBDataAdaptor(&obdata);

    // Conecta ao session bus
    QDBusConnection connection = QDBusConnection::sessionBus();
    if (!connection.isConnected()) {
        qWarning("Cannot connect to the D-Bus session bus.\n"
                 "Please check your system settings and try again.\n");
    }
    connection.registerObject("/OBData", &obdata);
    connection.registerService("local.OBData");

    ui->RPM_RTP->yAxis->setLabel("Giro [RPM]");
    ui->Speed_RTP->yAxis->setLabel("Vel. [km/h]");
    ui->IntakeTemp_RTP->yAxis->setLabel("Temp. Admissão [ºC]");
    ui->FuelLevel_RTP->yAxis->setLabel("Combustível [%]");
    ui->MAF_RTP->yAxis->setLabel("Admissão de Ar [g/s]");
    ui->ThrottlePos_RTP->yAxis->setLabel("Pos. Acelerador [%]");
    ui->CoolantTemp_RTP->yAxis->setLabel("Temp. Refrigerante [ºC]");

    rt.append(ui->RPM_RTP);
    rt.append(ui->Speed_RTP);
    rt.append(ui->IntakeTemp_RTP);
    rt.append(ui->FuelLevel_RTP);
    rt.append(ui->MAF_RTP);
    rt.append(ui->ThrottlePos_RTP);
    rt.append(ui->CoolantTemp_RTP);

    for(auto &i : rt){
        i->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(i,SIGNAL(customContextMenuRequested(const QPoint&)),i, SLOT(contextMenuRequested(const QPoint&)));
        connect(i,SIGNAL(contextMenu(const QPoint&, RTPlot*)), this, SLOT(RTPlotContextMenu(const QPoint&, RTPlot*)));
    }

    connect(&obdatapy,SIGNAL(readyReadStandardError()), this, SLOT(processOutput()));
    connect(&obdata, SIGNAL(ConnectionStatus_changed()), SLOT(ConnectionStatus()));
    connect(&obdata, SIGNAL(ProtocolName_changed()), SLOT(ProtocolName()));
    connect(&obdata, SIGNAL(PortName_changed()), SLOT(PortName()));
}

MainWindow::~MainWindow()
{
    obdatapy.kill();
    // Conecta ao session bus
    QDBusConnection connection = QDBusConnection::sessionBus();
    connection.unregisterObject("/OBData");
    connection.unregisterService("local.OBData");

    delete ui;
    delete dataTimer;
}

void MainWindow::RTPlotSlot()
{
    ui->RPM_RTP->plot(obdata.getRPM());
    ui->Speed_RTP->plot(obdata.getSpeed());
    ui->IntakeTemp_RTP->plot(obdata.getIntakeTemp());
    ui->FuelLevel_RTP->plot(obdata.getFuelLevel());
    ui->MAF_RTP->plot(obdata.getMAF());
    ui->ThrottlePos_RTP->plot(obdata.getThrottlePos());
    ui->CoolantTemp_RTP->plot(obdata.getCoolantTemp());
}

void MainWindow::ConnectionStatus()
{
    ui->conexao_Label->setText(obdata.getConnectionStatus());
}

void MainWindow::ProtocolName()
{
    ui->protocolo_Label->setText(obdata.getProtocolName());
}

void MainWindow::PortName()
{
    ui->porta_Label->setText(obdata.getPortName());
}


void MainWindow::on_scroll_Slider_valueChanged(int value)
{
    ui->scrollLabel->setText(QString::number(value));

    for(auto &i : rt)
        i->setScroll(value);
}

void MainWindow::on_conectar_Button_clicked()
{
    obdatapy.start("python ../OBData/obdpy.py");
    obdatapy.waitForStarted();
    ui->conectar_Button->setDisabled(true);
    ui->desconectar_Button->setEnabled(true);
    ui->saida_Browser->clear();
    qWarning() << obdatapy.state();
}

void MainWindow::on_desconectar_Button_clicked()
{
    obdatapy.kill();
    ui->conectar_Button->setDisabled(false);
    ui->desconectar_Button->setEnabled(false);
    ui->saida_Browser->append("/* CONEXÃO INTERROMPIDA */");
}

void MainWindow::processOutput()
{
    QString saida = QString::fromUtf8(obdatapy.readAllStandardError());
    ui->saida_Browser->append(saida);
}

bool MainWindow::aviso_pausar()
{
    if(!pausado){
         QMessageBox::StandardButton reply;
         reply = QMessageBox::question(this, "Aviso", "O programa será pausado. Deseja continuar?",
                                         QMessageBox::Yes|QMessageBox::No);
         if (reply == QMessageBox::Yes) {
           on_actionPausar_triggered();
           return true;
         }
         else return false;
    }

    return true;
}

void MainWindow::on_actionSalvar_log_triggered()
{
    if(!aviso_pausar()) return;

    QFileDialog dialog(this);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setNameFilter("OBData(*.log)");
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setDefaultSuffix("log");
    if (dialog.exec() != QDialog::Accepted)
        return;
    QString fileName = dialog.selectedFiles().first();

    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName),
                                  file.errorString()));
        return;
    }

    QTextStream out(&file);

#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif

    for(auto &i : rt){
        out << i->yAxis->label() << "{\n";
        for(auto &j : i->getxData()){
            out << QString::number(j) << ",";
        }
        out << "\n";

        for(auto &j : i->getyData()){
            out << QString::number(j) << ",";
        }
        out << "\n";
        out << "}\n\n";
    }

#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    statusBar()->showMessage(tr("Arquivo salvo"), 2000);
}

void MainWindow::RTPlotContextMenu(const QPoint& pos, RTPlot* plt)
{
    // for most widgets
    QPoint globalPos = plt->mapToGlobal(pos);
    // for QAbstractScrollArea and derived classes you would use:
    // QPoint globalPos = myWidget->viewport()->mapToGlobal(pos);

    QMenu myMenu;
    myMenu.addAction("Salvar PDF");

    QAction* selectedItem = myMenu.exec(globalPos);
    if (selectedItem)
    {
        if(selectedItem->text() == QString("Salvar PDF")){
            if(!aviso_pausar()) return;

            QFileDialog dialog(this);
            dialog.setWindowModality(Qt::WindowModal);
            dialog.setAcceptMode(QFileDialog::AcceptSave);
            dialog.setNameFilter("OBData(*.pdf)");
            dialog.setFileMode(QFileDialog::AnyFile);
            dialog.setDefaultSuffix("pdf");
            if (dialog.exec() != QDialog::Accepted)
                return;
            QString fileName = dialog.selectedFiles().first();

            plt->savePdf(fileName,1920,1080);
            statusBar()->showMessage(tr("PDF salvo"), 2000);
        }
    }

    else{

    }
}

void MainWindow::on_actionAbrir_log_triggered()
{
//    if(!aviso_pausar()) return;
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Aviso", "Dados não salvos serão perdidos. Deseja continuar?",
                                    QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::No) {
      return;
    }

    conectar_graficos(false);

    for(auto &i : rt)
        i->clean();

    RTPlot::restart();

    ui->actionIniciar->setEnabled(true);
    ui->actionReiniciar->setEnabled(false);
    ui->actionContinuar->setEnabled(false);
    ui->actionPausar->setEnabled(false);
    ui->timeEdit->setEnabled(true);
    color_count = 0;
    pausado = true;

    QFileDialog dialog(this);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setNameFilter("OBData(*.log)");
    dialog.setFileMode(QFileDialog::AnyFile);
    if (dialog.exec() != QDialog::Accepted)
        return;

    RTPlot::setOnlyRead(true);

    QString fileName = dialog.selectedFiles().first();

    QFile file(fileName);

    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName),
                                  file.errorString()));
        return;
    }

    QVector<QList<QByteArray>> bal;

    while(!file.atEnd()){
        bal.append(QList<QByteArray>());
        bal.last().append(file.readLine().split('{').first());
        bal.last().append(file.readLine());
        bal.last().append(file.readLine());
        file.readLine();
        file.readLine();
    }

    for(auto &i : rt){
        for(auto &j : bal){
            if(i->yAxis->label() == QString::fromUtf8(j[0].data())){
                QVector<double> keys;
                QVector<double> values;

                for(auto &k : j[1].split(',')){
                    keys.append(k.toDouble());
                }

                for(auto &k : j[2].split(',')){
                    values.append(k.toDouble());
                }

//                i->addGraph();
//                i->graph(i->graphCount() -1)->setPen(QPen(color[color_count]));
//                i->graph(i->graphCount() -1)->setData(keys,values,true);
//                i->xAxis->setRange(0, 8, Qt::AlignLeft);
//                i->replot();
                keys.removeLast();
                values.removeLast();
                i->setData(keys,values);

                break;
            }
        }
    }

    if(++color_count >= color.size()) color_count = 0;
    on_actionReescalar_triggered();
    ui->actionReiniciar->setEnabled(true);
    ui->timeEdit->setEnabled(true);

    statusBar()->showMessage(tr("Arquivo aberto"), 2000);
}

void MainWindow::on_actionIniciar_triggered()
{
    RTPlot::initiate();
    for(auto &i : rt)
        i->resume();
    on_actionReescalar_triggered();
    conectar_graficos(true);

    pausado = false;
    ui->actionIniciar->setEnabled(false);
    ui->actionReiniciar->setEnabled(true);
    ui->actionPausar->setEnabled(true);
    ui->actionContinuar->setEnabled(false);
    ui->timeEdit->setEnabled(false);
}

void MainWindow::on_actionContinuar_triggered()
{
    for(auto &i : rt)
        i->resume();
    ui->actionContinuar->setEnabled(false);
    ui->actionPausar->setEnabled(true);
    ui->timeEdit->setEnabled(false);
    pausado = false;
    conectar_graficos(true);
}

void MainWindow::on_actionPausar_triggered()
{
    for(auto &i : rt)
        i->stop();
    ui->actionContinuar->setEnabled(true);
    ui->actionPausar->setEnabled(false);
    ui->timeEdit->setEnabled(true);
    pausado = true;
    conectar_graficos(false);
}

void MainWindow::on_actionReiniciar_triggered()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Aviso", "Dados não salvos serão perdidos. Deseja continuar?",
                                    QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::No) {
      return;
    }

    conectar_graficos(false);

    for(auto &i : rt)
        i->clean();

    RTPlot::restart();

    ui->actionIniciar->setEnabled(true);
    ui->actionReiniciar->setEnabled(false);
    ui->actionContinuar->setEnabled(false);
    ui->actionPausar->setEnabled(false);
    ui->timeEdit->setEnabled(true);
    color_count = 0;
    pausado = true;
}

void MainWindow::on_actionReescalar_triggered()
{
    for(auto &i : rt)
        i->rescale();
}


void MainWindow::on_timeEdit_userTimeChanged(const QTime &time)
{
    QTime t(0,0);
    double key = t.msecsTo(time)/1000;
    for(auto &i : rt){
        i->xAxis->setRange(key, ui->scroll_Slider->value(), Qt::AlignLeft);
        i->replot();
    }
}

void MainWindow::on_actionAbout_Qt_triggered()
{
    QApplication::aboutQt();
}

void MainWindow::on_actionAjuda_triggered()
{
    QMessageBox::about(this, tr("Ajuda"),
                       tr("<p><strong>Conectando um Adaptador OBD-II<br /><br /></strong>Requisitos: Adaptador ELM327 OBD-II com Bluetooth ou conex&atilde;o USB.</p><p>Conecte a OBDII ao seu computador. Para uma conex&atilde;o via Bluetooth, talvez voc&ecirc; precise instalar e configurar sua conex&atilde;o. Em sistemas baseados em Debian, isso pode ser feito instalando os seguintes pacotes:</p><p><code class=\"shell\">$ sudo apt-get install bluetooth bluez-utils blueman</code></p><p>Em seguida, a sequ&ecirc;ncia de comandos pode ser &uacute;til para gerar uma conex&atilde;o atrav&eacute;s da porta rfcomm:</p><p><code class=\"shell\">$ hcitool scan<br />$ sudo rfcomm bind 0 [endere&ccedil;o do adaptador OBDII]</code></p><p>Ap&oacute;s estabelecer uma conex&atilde;o com o adaptador, entre na aba <em>Conex&atilde;o&nbsp;</em>e conecte o software.</p>"));
}


void MainWindow::conectar_graficos(bool b)
{
    if(b){
        connect(&obdata, SIGNAL(RPM_changed(double)), ui->RPM_RTP, SLOT(plot(double)));
        connect(&obdata, SIGNAL(Speed_changed(double)), ui->Speed_RTP, SLOT(plot(double)));
        connect(&obdata, SIGNAL(IntakeTemp_changed(double)), ui->IntakeTemp_RTP, SLOT(plot(double)));
        connect(&obdata, SIGNAL(FuelLevel_changed(double)), ui->FuelLevel_RTP, SLOT(plot(double)));
        connect(&obdata, SIGNAL(MAF_changed(double)), ui->MAF_RTP, SLOT(plot(double)));
        connect(&obdata, SIGNAL(ThrottlePos_changed(double)), ui->ThrottlePos_RTP, SLOT(plot(double)));
        connect(&obdata, SIGNAL(CoolantTemp_changed(double)), ui->CoolantTemp_RTP, SLOT(plot(double)));
    }

    else{
        disconnect(&obdata, SIGNAL(RPM_changed(double)), ui->RPM_RTP, SLOT(plot(double)));
        disconnect(&obdata, SIGNAL(Speed_changed(double)), ui->Speed_RTP, SLOT(plot(double)));
        disconnect(&obdata, SIGNAL(IntakeTemp_changed(double)), ui->IntakeTemp_RTP, SLOT(plot(double)));
        disconnect(&obdata, SIGNAL(FuelLevel_changed(double)), ui->FuelLevel_RTP, SLOT(plot(double)));
        disconnect(&obdata, SIGNAL(MAF_changed(double)), ui->MAF_RTP, SLOT(plot(double)));
        disconnect(&obdata, SIGNAL(ThrottlePos_changed(double)), ui->ThrottlePos_RTP, SLOT(plot(double)));
        disconnect(&obdata, SIGNAL(CoolantTemp_changed(double)), ui->CoolantTemp_RTP, SLOT(plot(double)));
    }

    /* SEGUNDO METODO */
//    if(b){
//        connect(dataTimer, SIGNAL(timeout()), this, SLOT (RTPlotSlot()));
//        dataTimer->start();
//    }

//    else{
//        disconnect(dataTimer, SIGNAL(timeout()), this, SLOT (RTPlotSlot()));
//        dataTimer->stop();
//    }
}

void MainWindow::on_timeEdit_2_userTimeChanged(const QTime &time)
{
    QTime t(0,0);
    double key = t.msecsTo(time)/1000;
    for(auto &i : rt){
        i->setAverage(true,key);
        i->calculateAverage();
    }
}

void MainWindow::on_okMediaButton_clicked()
{
    ui->timeEdit_2->setTime(QTime(0,0));
    for(auto &i : rt)
        i->setAverage(false);
}


void MainWindow::on_calculoTotalButton_clicked()
{
    ui->calculoTotalButton->setEnabled(false);
    double dist = ui->Speed_RTP->getIntegral();
    double comb = ui->MAF_RTP->getIntegral();
    double rpm = ui->RPM_RTP->getIntegral();
    if(dist < 0) dist = 0;
    if(comb < 0) comb = 0;
    if(rpm < 0) rpm = 0;
    ui->distTotalNumber->display(QString::number(dist/3600,'f',3));
    ui->combTotalNumber->display(QString::number(comb,'f',3));
    ui->rpmTotalNumber->display(QString::number(rpm/60,'f',3));
    ui->calculoTotalButton->setEnabled(true);
}

void MainWindow::on_calculoPerButton_clicked()
{
    ui->calculoPerButton->setEnabled(false);

    QTime t(0,0);
    double in = t.msecsTo(ui->calculoInTimeEdit->time())/1000;
    double fim = t.msecsTo(ui->calculoFimTimeEdit->time())/1000;

    double dist = ui->Speed_RTP->getIntegral(in,fim);
    double comb = ui->MAF_RTP->getIntegral(in,fim);
    double rpm = ui->RPM_RTP->getIntegral(in,fim);
    if(dist < 0) dist = 0;
    if(comb < 0) comb = 0;
    if(rpm < 0) rpm = 0;

    ui->distPerNumber->display(QString::number(dist/3600,'f',3));
    ui->combPerNumber->display(QString::number(comb,'f',3));
    ui->rpmPerNumber->display(QString::number(rpm/60,'f',3));

    ui->calculoPerButton->setEnabled(true);
}
