QT       += core gui sql widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    database.cpp \
    logindialog.cpp \
    mainwindow.cpp \
    studentmodel.cpp \
    studentdelegate.cpp \
    studentdialog.cpp \
    chartwidget.cpp \
    dataexport.cpp \
    usermanagementdialog.cpp

HEADERS += \
    database.h \
    logindialog.h \
    mainwindow.h \
    studentmodel.h \
    studentdelegate.h \
    studentdialog.h \
    chartwidget.h \
    dataexport.h \
    usermanagementdialog.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += resources.qrc
