# link against main lib
QT += datasync concurrent

SYREMLIB_DIR = $$shadowed($$PWD/lib)
win32:CONFIG(release, debug|release): LIBS += -L$$SYREMLIB_DIR/release/ -l$${PROJECT_TARGET}
else:win32:CONFIG(debug, debug|release): LIBS += -L$$SYREMLIB_DIR/debug/ -l$${PROJECT_TARGET}
else:unix: LIBS += -L$$SYREMLIB_DIR/ -l$${PROJECT_TARGET}

INCLUDEPATH += $$PWD/lib $$SYREMLIB_DIR
win32:CONFIG(release, debug|release): INCLUDEPATH += $$SYREMLIB_DIR/release
else:win32:CONFIG(debug, debug|release): INCLUDEPATH += $$SYREMLIB_DIR/debug
DEPENDPATH += $$PWD/lib

# enable systemd builds
systemd_service: DEFINES += USE_SYSTEMD_SERVICE
