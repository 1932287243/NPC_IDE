QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

qtHaveModule(printsupport): QT+=printsupport

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
INCLUDEPATH += $$PWD/include

SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/mycodeeditor.cpp \
    src/myfilesystemmodel.cpp \
    src/myhighlighter.cpp \
    src/mysplitterlayout.cpp \
    src/mytextedit.cpp \
    src/mytexteditbycode.cpp \
    src/mytreeview.cpp

HEADERS += \
    include/mainwindow.h \
    include/mycodeeditor.h \
    include/myfilesystemmodel.h \
    include/myhighlighter.h \
    include/mysplitterlayout.h \
    include/mytextedit.h \
    include/mytexteditbycode.h \
    include/mytreeview.h

FORMS += \
    ui/mainwindow.ui \
    ui/mytextedit.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res/code.qrc \
    res/images.qrc

RC_ICONS = res/images/logo.ico
