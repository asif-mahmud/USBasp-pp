TARGET = USBasp-pp
TEMPLATE = app
CONFIG += qt debug_and_release

INCLUDEPATH += \
    include \
    ui

RESOURCES += \
    rc/darkstyle/style.qrc \
    rc/resources.qrc

FORMS += \
    ui/mainwindow.ui \
    ui/tabcontainer.ui

HEADERS += \
    ui/MainWindow.hpp \
    ui/tabcontainer.hpp \
    include/Mcu.hpp \
    include/Avrdude.hpp

SOURCES += \
    src/main.cpp \
    ui/mainwindow.cpp \
    ui/tabcontainer.cpp \
    src/mcu.cpp \
    src/avrdude.cpp

OTHER_FILES += \
    rc/win32.rc

win32{
    RC_FILE = rc/win32.rc
}

debug {
    QMAKE_CXXFLAGS += -gdwarf-2
}
