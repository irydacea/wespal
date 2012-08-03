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

SOURCES += main.cpp \
    mainwindow.cpp \
    wesnothrc.cpp \
    defs.cpp \
    version.cpp \
    rc_qt4.cpp \
    customranges.cpp \
    appconfig.cpp \
    custompalettes.cpp \
    paletteitem.cpp \
    util.cpp \
    codesnippetdialog.cpp \
    colorlistinputdialog.cpp
HEADERS += mainwindow.hpp \
    wesnothrc.hpp \
    defs.hpp \
    version.hpp \
    rc_qt4.hpp \
    customranges.hpp \
    appconfig.hpp \
    custompalettes.hpp \
    paletteitem.hpp \
    util.hpp \
    codesnippetdialog.hpp \
    colorlistinputdialog.hpp
FORMS += mainwindow.ui \
    customranges.ui \
    custompalettes.ui \
    codesnippetdialog.ui \
    colorlistinputdialog.ui
RESOURCES += morningstar.qrc
RC_FILE += win32/wesnoth-rcx.rc
