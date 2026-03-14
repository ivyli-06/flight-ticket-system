QT       += core gui sql widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The name of your application
TARGET = flight-ticket-system
TEMPLATE = app

# Define source files
SOURCES += \
    src/main.cpp \
    src/database/dbmanager.cpp \
    src/models/usersession.cpp \
    src/ui/AdminWindow.cpp \
    src/ui/bookTicketConfirm.cpp \
    src/ui/loginwindow.cpp \
    src/ui/flightsearchwindow.cpp \
    src/ui/mainwindow.cpp \
    src/ui/ordermanagerwindow.cpp \
    src/ui/registerwindow.cpp

# Define header files
HEADERS += \
    src/database/dbmanager.h \
    src/models/user.h \
    src/models/flight.h \
    src/models/order.h \
    src/models/usersession.h \
    src/ui/AdminWindow.h \
    src/ui/bookTicketConfirm.h \
    src/ui/loginwindow.h \
    src/ui/flightsearchwindow.h \
    src/ui/mainwindow.h \
    src/ui/ordermanagerwindow.h \
    src/ui/registerwindow.h

# Define UI files
FORMS += \
    src/ui/AdminWindow.ui \
    src/ui/bookTicketConfirm.ui \
    src/ui/loginwindow.ui \
    src/ui/flightsearchwindow.ui \
    src/ui/mainwindow.ui \
    src/ui/ordermanagerwindow.ui \
    src/ui/registerwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# Include path for additional libraries if needed
# INCLUDEPATH += path/to/your/include

# Library path for additional libraries if needed
# LIBS += -Lpath/to/your/lib -lyourlib

# Add any additional configurations here if necessary
