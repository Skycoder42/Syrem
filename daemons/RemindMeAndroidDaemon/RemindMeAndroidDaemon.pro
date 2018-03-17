TEMPLATE = app

QMAKE_TARGET_PRODUCT = "Remind-Me Daemon"
DEFINES += "DISPLAY_NAME=\"\\\"$$QMAKE_TARGET_PRODUCT\\\"\""

QT -= gui
QT += androidextras

SOURCES += main.cpp

# link against main lib
include(../../lib.pri)
