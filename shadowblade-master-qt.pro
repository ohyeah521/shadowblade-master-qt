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
    session/datapack.cpp \
    dialog/sendsmsdialog.cpp \
    model/hosttablemodel.cpp \
    session/sessionmanager.cpp \
    session/handler/sendsms.cpp \
    session/handler/loadcontacts.cpp \
    session/handler/loadsms.cpp \
    util/data.cpp

HEADERS  += mainwindow.h \
    session/datapack.h \
    dialog/sendsmsdialog.h \
    model/hosttablemodel.h \
    session/session.h \
    session/sessionmanager.h \
    session/handler/sendsms.h \
    defines.h \
    session/handler/loadcontacts.h \
    session/handler/loadsms.h \
    util/data.h \
    session/hostinfo.h

FORMS    += mainwindow.ui \
    dialog/sendsmsdialog.ui

RC_FILE += icon.rc
