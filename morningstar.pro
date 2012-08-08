# -------------------------------------------------
# Project created by QtCreator 2010-09-30T09:42:03
# -------------------------------------------------
TARGET = wesnoth-rcx
TEMPLATE = app

unix {
    icon64.path = /usr/local/share/icons/hicolor/64x64/apps
    icon64.files = icons/64x64/wesnoth-rcx.png
    icon128.path = /usr/local/share/icons/hicolor/128x128/apps
    icon128.files = icons/128x128/wesnoth-rcx.png

    menu.path = /usr/local/share/applications
    menu.files = x11/wesnoth-rcx.desktop

    target.path = /usr/local/bin

    INSTALLS += target icon64 icon128 menu
}

OBJECTS_DIR = obj
MOC_DIR = obj
UI_DIR = obj
RCC_DIR = obj
DESTDIR = .

VPATH = src

SOURCES += \
    appconfig.cpp \
    codesnippetdialog.cpp \
    colorlistinputdialog.cpp \
    custompalettes.cpp \
    customranges.cpp \
    defs.cpp \
    imagelabel.cpp \
    main.cpp \
    mainwindow.cpp \
    paletteitem.cpp \
    rc_qt4.cpp \
    util.cpp \
    wesnothrc.cpp \
    version.cpp

HEADERS += \
    appconfig.hpp \
    codesnippetdialog.hpp \
    colorlistinputdialog.hpp \
    custompalettes.hpp \
    customranges.hpp \
    defs.hpp \
    imagelabel.hpp \
    mainwindow.hpp \
    paletteitem.hpp \
    rc_qt4.hpp \
    util.hpp \
    wesnothrc.hpp \
    version.hpp

FORMS += \
    codesnippetdialog.ui \
    colorlistinputdialog.ui \
    custompalettes.ui \
    customranges.ui \
    mainwindow.ui

RESOURCES += morningstar.qrc

RC_FILE += win32/wesnoth-rcx.rc
