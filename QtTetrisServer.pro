QT += core network
QT -= gui

CONFIG += c++11

TARGET = QtTetrisServer
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    server.cpp

HEADERS += \
    server.h
