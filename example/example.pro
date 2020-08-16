QT += quick

CONFIG += c++11 silent

INCLUDEPATH += $$PWD

SOURCES += \
    $$PWD/main.cpp

RESOURCES += \
    $$PWD/qml.qrc

# QML reLoader
include($$PWD/../qmlreloader/qmlreloader.pri)
