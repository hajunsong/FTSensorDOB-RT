#include "ftsensor.h"

FTSensor::FTSensor(QObject *parent) : QObject(parent)
{
    printf("FTSensor Constructor\n");
    comm = new SerialComm();
    connectState = false;
    logging_complete = false;
    finish_flag = false;

    loggingTimer = new QTimer(this);
    loggingTimer->setInterval(50);
    connect(loggingTimer, SIGNAL(timeout()), this, SLOT(loggingTimeout()));
}

FTSensor::~FTSensor()
{
    printf("FTSensor Destructor\n");
    connectState = false;
    comm->serial->write(QByteArray::fromRawData("\x55\x0c\x00\x00\x00\x00\x00\x00\x00\x0c\xAA", 11));
    disconnect(comm->serial, SIGNAL(readyRead()), this, SLOT(serialReadSlot()));

    finish_flag = true;
    printf("Wait logging complete");
    while(1){
        if(logging_complete){
            break;
        }
    }
    printf("Logging Stop");
    loggingTimer->stop();
    delete loggingTimer;
    delete logger;

    delete comm;
}

void FTSensor::start()
{
    connectState = comm->connect("/dev/ftsensor", "115200");
    connect(comm->serial, SIGNAL(readyRead()), this, SLOT(serialReadSlot()));
    comm->serial->write(QByteArray::fromRawData("\x55\x0b\x00\x00\x00\x00\x00\x00\x00\x0b\xAA", 11));

    QDateTime date;
    QString Path = "./";
    QDir dir(Path);
    dir.mkpath("logging");
    QString fileName = "logging/" + date.currentDateTime().toString("yyyy-MM-dd-hh-mm-ss") + ".csv";
    logger = new Logger(this, fileName);

    indx = 1;
    QString data = "Indx,";
    data += "Fx,";
    data += "Fy,";
    data += "Fz,";
    data += "Tx,";
    data += "Ty,";
    data += "Tz";
    data += "\n";
    logger->write(data);

    loggingTimer->start();
}

void FTSensor::serialReadSlot()
{
    QByteArray rxData = comm->serial->readAll();
    vector<unsigned char> uart_rx_buffer;
    QThread::msleep(4);
    if(rxData.length() > 0){
        for(int i = 0; i < rxData.length(); i++){
            uart_rx_buffer.push_back(static_cast<unsigned char>(rxData.at(i)));
        }
        if(rxData.at(0) == 0x55){
            if(rxData.at(1) == 0x0b){
                unsigned char data_field[16]; // storage buffer for data field
                // check the SOP, EOP, Checksum of received UART data
                // SOP == 0x55, EOP == 0xAA, Checksum == summation of each data in data_field
                // Save the data field's data in data field buffer
                for(unsigned int idx = 0; idx < 16; idx++){
                    data_field[idx] = uart_rx_buffer[idx + 1]; //in case that rx_buffer[0] is SOP
                }
                // data field processing
                short raw_data[6] = { 0 };
                unsigned short temp;
                unsigned DF=50, DT=1000; // DF, DT depend on the model, refer to 3.6.11

                for (int idx = 0; idx < 6; idx++)
                {
                    temp = data_field [2 * idx + 1] * 256;
                    temp += data_field [2 * idx + 2];
                    raw_data[idx] = static_cast<signed short>(temp); // casting process
                }
                // Conversion from signed short data to float data and data scaling
                // Set Force/Torque Original
                double ft_array[6];
                for (int n = 0; n < 3; n++)
                {
                    ft_array[n] = ((static_cast<double>(raw_data[n]))/DF);
                    ft_array[n + 3] = ((static_cast<double>(raw_data[n + 3]))/DT);
                }

                LoggingData data;
                data.indx = indx;
                data.Fx = ft_array[0];
                data.Fy = ft_array[1];
                data.Fz = ft_array[2];
                data.Tx = ft_array[3];
                data.Ty = ft_array[4];
                data.Tz = ft_array[5];
                logging_data.push_back(data);

                printf("\t%f\t%f\t%f\t%f\t%f\t %f\n", ft_array[0], ft_array[1], ft_array[2], ft_array[3], ft_array[4], ft_array[5]);
                indx++;
            }
        }
    }
}

void FTSensor::loggingTimeout(){
    qDebug() << "logging data size :" << logging_data.length();
    printf("logging data size : %d\n", logging_data.length());
    if(logging_data.length() > 0){
        QString logStr;
        logStr.push_back(QString::number(logging_data.front().indx));
        logStr.push_back(",");
        logStr.push_back(QString::number(logging_data.front().Fx, 'f', 6));
        logStr.push_back(",");
        logStr.push_back(QString::number(logging_data.front().Fy, 'f', 6));
        logStr.push_back(",");
        logStr.push_back(QString::number(logging_data.front().Fz, 'f', 6));
        logStr.push_back(",");
        logStr.push_back(QString::number(logging_data.front().Tx, 'f', 6));
        logStr.push_back(",");
        logStr.push_back(QString::number(logging_data.front().Ty, 'f', 6));
        logStr.push_back(",");
        logStr.push_back(QString::number(logging_data.front().Tz, 'f', 6));
        logStr.push_back("\n");
//        logger->write(logStr);
        logging_data.pop_front();
    }

//    if(finish_flag){
//        if(logging_data.length() == 0){
//            logging_complete = true;
//        }
//    }
}
