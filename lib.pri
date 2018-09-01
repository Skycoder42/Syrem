# link against main lib
QT += datasync concurrent

RMLIB_DIR = $$shadowed($$PWD/RemindMeLib)
win32:CONFIG(release, debug|release): LIBS += -L$$RMLIB_DIR/release/ -lremind-me
else:win32:CONFIG(debug, debug|release): LIBS += -L$$RMLIB_DIR/debug/ -lremind-me
else:unix: LIBS += -L$$RMLIB_DIR/ -lremind-me

INCLUDEPATH += $$PWD/RemindMeLib $$RMLIB_DIR
DEPENDPATH += $$PWD/RemindMeLib
