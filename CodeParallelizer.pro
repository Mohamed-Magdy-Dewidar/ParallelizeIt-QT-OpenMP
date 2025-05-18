QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


CONFIG += c++17



# OpenMP Support
OPENMP_FLAGS = -fopenmp
QMAKE_CXXFLAGS += $$OPENMP_FLAGS
QMAKE_LFLAGS   += $$OPENMP_FLAGS


SOURCES += \
    Parallelizer.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    Parallelizer.h \
    mainwindow.h

FORMS += \
    mainwindow.ui



# Default rules for deployment
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    SampleSequentialCode.txt
