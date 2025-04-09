QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Cliente
TEMPLATE = app

SOURCES += \
    cliente.cpp \
    VentanaMensajes.cpp \
    enviarmensaje.cpp \
    recibirmensaje.cpp \
    hiloreceptor.cpp

HEADERS += \
    VentanaMensajes.h \
    enviarmensaje.h \
    recibirmensaje.h \
    hiloreceptor.h

unix:LIBS += -pthread