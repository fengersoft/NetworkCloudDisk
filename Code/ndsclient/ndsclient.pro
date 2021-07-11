QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../api/controls/logowidget.cpp \
    ../api/controls/setvaluedialog.cpp \
    ../api/dialogs/selectfiledialog.cpp \
    ../api/file/fileseacher.cpp \
    ../api/global/globalconst.cpp \
    ../api/global/globaldata.cpp \
    ../api/http/synchttp.cpp \
    ../api/http/synchttpobject.cpp \
    ../api/http/webinfo.cpp \
    ../api/tcp/netdiskclient.cpp \
    src/adminmanagerwindow.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/system/logindialog.cpp \
    src/system/urlsetdialog.cpp

HEADERS += \
    ../api/controls/logowidget.h \
    ../api/controls/setvaluedialog.h \
    ../api/dialogs/selectfiledialog.h \
    ../api/file/fileseacher.h \
    ../api/global/globalconst.h \
    ../api/global/globaldata.h \
    ../api/http/synchttp.h \
    ../api/http/synchttpobject.h \
    ../api/http/webinfo.h \
    ../api/tcp/netdiskclient.h \
    src/adminmanagerwindow.h \
    src/datastructs.h \
    src/mainwindow.h \
    src/system/logindialog.h \
    src/system/urlsetdialog.h

FORMS += \
    ../api/controls/logowidget.ui \
    ../api/controls/setvaluedialog.ui \
    ../api/dialogs/selectfiledialog.ui \
    src/adminmanagerwindow.ui \
    src/mainwindow.ui \
    src/system/logindialog.ui \
    src/system/urlsetdialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    app.qrc
RC_ICONS=app.ico
DESTDIR=$$PWD/bin
