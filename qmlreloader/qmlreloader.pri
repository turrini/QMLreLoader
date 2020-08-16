QT += quick quick-private

CONFIG += c++11 silent

DEFINES += PROJECT_SOURCE_DIR=\\\"$$_PRO_FILE_PWD_/\\\"

INCLUDEPATH += $$PWD

SOURCES += \
    $$PWD/filewatcher.cpp \
    $$PWD/qmlfileinterceptor.cpp \
    $$PWD/qmlreloader.cpp

HEADERS += \
    $$PWD/filewatcher.h \
    $$PWD/qmlfileinterceptor.h \
    $$PWD/qmlreloader.h
