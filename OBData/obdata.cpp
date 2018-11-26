#include "obdata.h"

OBData::OBData(QObject *parent) : QObject(parent)
{
    RPM = 0.0;
    Speed = 0.0;
    FuelLevel = 0.0;
    IntakeTemp = 0.0;
    MAF = 0.0;
    ThrottlePos = 0.0;
    CoolantTemp = 0.0;
}


/* ========== GETTERS ========== */
QString OBData::getConnectionStatus() const
{
    return ConnectionStatus;
}

QString OBData::getProtocolName() const
{
    return ProtocolName;
}

QString OBData::getPortName() const
{
    return PortName;
}

double OBData::getRPM() const
{
    return RPM;
}

double OBData::getSpeed() const
{
    return Speed;
}

double OBData::getFuelLevel() const
{
    return FuelLevel;
}

double OBData::getIntakeTemp() const
{
    return IntakeTemp;
}

double OBData::getMAF() const
{
    return MAF;
}

double OBData::getThrottlePos() const
{
    return ThrottlePos;
}

double OBData::getCoolantTemp() const
{
    return CoolantTemp;
}


/* ========== SETTERS ========== */
void OBData::setConnectionStatus(QString s)
{
    ConnectionStatus = s;
    emit ConnectionStatus_changed();
}

void OBData::setProtocolName(QString s)
{
    ProtocolName = s;
    emit ProtocolName_changed();
}

void OBData::setPortName(QString s)
{
    PortName = s;
    emit PortName_changed();
}

void OBData::setRPM(double val)
{
    RPM = val;
    emit RPM_changed(val);
}

void OBData::setSpeed(double val)
{
    Speed = val;
    emit Speed_changed(val);
}

void OBData::setFuelLevel(double val)
{
    FuelLevel = val;
    emit FuelLevel_changed(val);
}

void OBData::setIntakeTemp(double val)
{
    IntakeTemp = val;
    emit IntakeTemp_changed(val);
}

void OBData::setMAF(double val)
{
    MAF = val;
    emit MAF_changed(val);
}

void OBData::setThrottlePos(double val)
{
    ThrottlePos = val;
    emit ThrottlePos_changed(val);
}

void OBData::setCoolantTemp(double val)
{
    CoolantTemp = val;
    emit CoolantTemp_changed(val);
}
