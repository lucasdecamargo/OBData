#ifndef OBDATA_H
#define OBDATA_H

#include <QObject>

class OBData : public QObject
{
    Q_OBJECT
public:
    explicit OBData(QObject *parent = nullptr);

    QString getConnectionStatus() const;
    QString getProtocolName() const;
    QString getPortName() const;

    double getRPM() const;
    double getSpeed() const;
    double getFuelLevel() const;
    double getIntakeTemp() const;
    double getMAF() const;
    double getThrottlePos() const;
    double getCoolantTemp() const;

signals:
    void ConnectionStatus_changed();
    void ProtocolName_changed();
    void PortName_changed();

    void RPM_changed(double);
    void Speed_changed(double);
    void FuelLevel_changed(double);
    void IntakeTemp_changed(double);
    void MAF_changed(double);
    void ThrottlePos_changed(double);
    void CoolantTemp_changed(double);

public slots:
    void setConnectionStatus(QString s);
    void setProtocolName(QString s);
    void setPortName(QString s);

    void setRPM(double val);
    void setSpeed(double val);
    void setFuelLevel(double val);
    void setIntakeTemp(double val);
    void setMAF(double val);
    void setThrottlePos(double val);
    void setCoolantTemp(double val);

private:
    QString ConnectionStatus;
    QString ProtocolName;
    QString PortName;

    double RPM;
    double Speed;
    double FuelLevel;
    double IntakeTemp;
    double MAF;
    double ThrottlePos;
    double CoolantTemp;
};

#endif // OBDATA_H
