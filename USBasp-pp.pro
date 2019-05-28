TEMPLATE = app
TARGET = USBasp-pp

QT += core gui qml quickcontrols2
CONFIG += c++11


SOURCES += \
    src/Avrdude.cpp \
    src/ConfigBlock.cpp \
    src/ConfigParser.cpp \
    src/Part.cpp \
    src/SettingsProvider.cpp \
    src/StaticDocuments.cpp \
    src/main.cpp

RESOURCES += \
    resources/resources.qrc

HEADERS += \
    src/Avrdude.hpp \
    src/ConfigBlock.hpp \
    src/ConfigParser.hpp \
    src/Part.hpp \
    src/SettingsProvider.hpp \
    src/StaticDocuments.hpp
