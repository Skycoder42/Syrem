# link against main lib
QT += datasync concurrent

RMLIB_DIR = $$shadowed($$PWD/lib)
win32:CONFIG(release, debug|release): LIBS += -L$$RMLIB_DIR/release/ -l$${PROJECT_TARGET}
else:win32:CONFIG(debug, debug|release): LIBS += -L$$RMLIB_DIR/debug/ -l$${PROJECT_TARGET}
else:unix: LIBS += -L$$RMLIB_DIR/ -l$${PROJECT_TARGET}

INCLUDEPATH += $$PWD/lib $$RMLIB_DIR
DEPENDPATH += $$PWD/lib
