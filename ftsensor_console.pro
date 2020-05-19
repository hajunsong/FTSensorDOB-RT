QT -= gui
QT += core serialport widgets

CONFIG += c++11 console
CONFIG -= app_bundle

target.path = /home/user/Project/FTSensor
INSTALLS += target

SOURCES += main.cpp \
    FTSensor/ftsensor.cpp \
    KeyInput/keyinput.cpp \
    Logger/logger.cpp \
    SerialComm/serialcomm.cpp \

HEADERS += \
    FTSensor/ftsensor.h \
    KeyInput/keyinput.h \
    Logger/logger.h \
    SerialComm/serialcomm.h \
