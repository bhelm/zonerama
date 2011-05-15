TEMPLATE = app
TARGET = Zonerama
QT += core \
    sql \
    xml \
    script
HEADERS += LinkedMonitor.h \
    zMergedFrame.h \
    zFrame.h \
    Login.h \
    ZoneramaManager.h \
    MergedMonitor.h
SOURCES += LinkedMonitor.cpp \
    ZoneramaManager.cpp \
    MergedMonitor.cpp \
    main.cpp
FORMS += 
RESOURCES += 
CONFIG += debug