TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt
LIBS += -lmysqlcppconn
LIBS += -pthread
SOURCES += \
        main.cpp \
    database.cpp \
    chatroom.cpp

HEADERS += \
    database.h \
    chatroom.h
