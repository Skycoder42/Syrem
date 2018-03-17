TEMPLATE = app

TARGET = remind-med

QMAKE_TARGET_PRODUCT = "Remind-Me Daemon"
DEFINES += "DISPLAY_NAME=\"\\\"$$QMAKE_TARGET_PRODUCT\\\"\""

QT += widgets

SOURCES += main.cpp

# link against main lib
include(../../lib.pri)
