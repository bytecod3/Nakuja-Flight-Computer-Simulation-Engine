QT       += core gui charts serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++17
QMAKE_CXXFLAGS += -Wa,-mbig-obj

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    csv-parser.cpp \
    defines.cpp \
    main.cpp \
    mainwindow.cpp \
    qcustomplot.cpp \
    serialparser.cpp \
    serialport.cpp

HEADERS += \
    csv-parser.h \
    defines.h \
    mainwindow.h \
    qcustomplot.h \
    serialparser.h \
    serialport.h \
    window_defs.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    engine.qrc \
    play-icon.qrc \
    rocket-render.qrc \
    serial-icon.qrc \
    upload-icon.qrc

DISTFILES += \
    render.jpg
