#-------------------------------------------------
#
# Project created by QtCreator 2023-03-23T16:57:05
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = 01_First_Project StudentManagementSystem
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    studentinfoinput.cpp \
    tcpclient.cpp \
    loginwidget.cpp \
    messagebox.cpp \
    studentinfosearch.cpp \
    studentinfomodify.cpp \
    studentinfodelete.cpp \
    studentinfoclear.cpp

HEADERS  += widget.h \
    studentinfoinput.h \
    tcpclient.h \
    loginwidget.h \
    messagebox.h \
    studentinfosearch.h \
    studentinfomodify.h \
    studentinfodelete.h \
    studentinfoclear.h

RESOURCES += \
    GuidePageIcon.qrc

CONFIG += c++11
