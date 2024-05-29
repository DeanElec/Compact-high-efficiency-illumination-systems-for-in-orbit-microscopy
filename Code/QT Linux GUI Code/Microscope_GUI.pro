QT       += core gui concurrent
QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

LIBS += -L/home/dean/mosquitto-master/lib/cpp/mosquittopp.h -lmosquittopp
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    File_Monitor.cpp \
    IPAddress.cpp \
    MultiSelectComboBox.cpp \
    QMMqttClient.cpp \
    Socket_Comms.cpp \
    main.cpp \
    widget.cpp

HEADERS += \
    Custom_Slider.h \
    File_Monitor.h \
    IPAddress.h \
    MultiSelectComboBox.h \
    QMMqttClient.h \
    Socket_Comms.h \
    widget.h

FORMS += \
    widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    Open_Napari_With_User_Image.py
