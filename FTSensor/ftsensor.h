#ifndef FTSENSOR_H
#define FTSENSOR_H

#include <stdio.h>
#include <iostream>
using namespace std;

#include "Logger/logger.h"
#include "SerialComm/serialcomm.h"

#include <QTimer>

struct LoggingData{
    unsigned int indx;
    double Fx, Fy, Fz, Tx, Ty, Tz;
};

class FTSensor : public QObject{
    Q_OBJECT

public:
    explicit FTSensor(QObject *parent = nullptr);
    ~FTSensor();
    void start();

private:
    bool connectState;
    SerialComm *comm;
    Logger *logger;
    unsigned int indx;
    QTimer *loggingTimer;
    QVector<LoggingData> logging_data;
    bool logging_complete;
    bool finish_flag;

public slots:
    void serialReadSlot();
    void loggingTimeout();
};

#endif // FTSENSOR_H
