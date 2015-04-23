#-------------------------------------------------
#
# Project created by QtCreator 2015-03-30T22:51:13
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = shadowblade-master-qt
TEMPLATE = app

include(libs/libSessionConsole/libSessionConsole.pro)

SOURCES += main.cpp\
        mainwindow.cpp \
    session/datapack.cpp \
    dialog/sendsmsdialog.cpp \
    model/hosttablemodel.cpp \
    session/handler/sendsms.cpp \
    session/handler/loadcontacts.cpp \
    session/handler/loadsms.cpp \
    util/data.cpp \
    session/networkmanager.cpp \
    session/sessionmanager.cpp \
    session/hostpool.cpp \
    session/handler/shell.cpp \
    session/rawdata.cpp \
    util/system.cpp

HEADERS  += mainwindow.h \
    session/datapack.h \
    dialog/sendsmsdialog.h \
    model/hosttablemodel.h \
    session/session.h \
    session/handler/sendsms.h \
    defines.h \
    session/handler/loadcontacts.h \
    session/handler/loadsms.h \
    util/data.h \
    session/hostinfo.h \
    session/networkmanager.h \
    session/sessionmanager.h \
    session/hostpool.h \
    session/handler/shell.h \
    session/rawdata.h \
    util/system.h

FORMS    += mainwindow.ui \
    dialog/sendsmsdialog.ui

RC_FILE += icon.rc
