QT            = core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets # Qt 5


HEADERS       = Model/*.h \
                View/*.h \
                Control/*.h

SOURCES       = main.cpp \
                Model/*.cpp \
                View/*.cpp \
                Control/*.cpp

RESOURCES     = View/mdi.qrc

FORMS         = View/*.ui

CONFIG += c++11
