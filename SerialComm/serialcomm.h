#ifndef SERIALCOMM_H
#define SERIALCOMM_H

#include <QSerialPort>
#include <QtDebug>
#include <QThread>

#include <iostream>

class SerialComm
{
public:
    SerialComm();
    ~SerialComm();
    bool connect(QString Port, QString Baud);
    void disconnect();

    void writeread();
    QSerialPort *serial;

private:
    QString name = "/dev/ftsensor";
    int32_t baud = QSerialPort::Baud115200;
    QSerialPort::DataBits bit = QSerialPort::DataBits::Data8;
    QSerialPort::Parity parity = QSerialPort::NoParity;
    QSerialPort::StopBits stop = QSerialPort::OneStop;
    QSerialPort::FlowControl flow = QSerialPort::NoFlowControl;

};

#endif // SERIALCOMM_H
