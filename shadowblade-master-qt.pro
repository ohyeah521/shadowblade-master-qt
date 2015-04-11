#-------------------------------------------------
#
# Project created by QtCreator 2015-03-30T22:51:13
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = shadowblade-master-qt
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    session/networksessionmanager.cpp \
    session/networksession.cpp \
    dialog/sendsmsdialog.cpp \
    model/hosttablemodel.cpp

HEADERS  += mainwindow.h \
    session/networksessionmanager.h \
    session/networksession.h \
    dialog/sendsmsdialog.h \
    model/hosttablemodel.h

FORMS    += mainwindow.ui \
    dialog/sendsmsdialog.ui
