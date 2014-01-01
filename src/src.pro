################################################################################
# Build
################################################################################

MIDISNOOP_APP_SUFFIX = bin
MIDISNOOP_DATA_SUFFIX = share

!isEmpty(DEBUG) {
    CONFIG += debug
    DEFINES += MIDISNOOP_DEBUG
}

isEmpty(BUILDDIR) {
    BUILDDIR = ../build
}
isEmpty(MAKEDIR) {
    MAKEDIR = ../make
}

isEmpty(MAJOR_VERSION) {
    MAJOR_VERSION = 0
}
isEmpty(MINOR_VERSION) {
    MINOR_VERSION = 1
}
isEmpty(REVISION) {
    REVISION = 3
}
MIDISNOOP_VERSION = $${MAJOR_VERSION}.$${MINOR_VERSION}.$${REVISION}

macx {
    CONFIG += x86_64
    DEFINES += MIDISNOOP_PLATFORM_MACX
} else:unix {
    DEFINES += MIDISNOOP_PLATFORM_UNIX
} else:win32 {
    DEFINES += MIDISNOOP_PLATFORM_WIN32
} else {
    warning(Your platform has not been detected successfully.  Expect errors.)
}

isEmpty(PREFIX) {
    win32 {
        PREFIX = C:/Program Files/midisnoop
    } else {
        PREFIX = /usr/local
    }
}
isEmpty(DATAROOTDIR) {
    DATAROOTDIR = $${PREFIX}/$${MIDISNOOP_DATA_SUFFIX}
}
isEmpty(EXECPREFIX) {
    EXECPREFIX = $${PREFIX}
}
isEmpty(BINDIR) {
    MIDISNOOP_APP_INSTALL_PATH = $${EXECPREFIX}/$${MIDISNOOP_APP_SUFFIX}
} else {
    MIDISNOOP_APP_INSTALL_PATH = $${BINDIR}
}
isEmpty(DATADIR) {
    MIDISNOOP_DATA_INSTALL_PATH = $${DATAROOTDIR}
} else {
    MIDISNOOP_DATA_INSTALL_PATH = $${DATADIR}
}

CONFIG += console warn_on
DEFINES += MIDISNOOP_MAJOR_VERSION=$${MAJOR_VERSION} \
    MIDISNOOP_MINOR_VERSION=$${MINOR_VERSION} \
    MIDISNOOP_REVISION=$${REVISION}
DESTDIR = $${BUILDDIR}/$${MIDISNOOP_APP_SUFFIX}
HEADERS += aboutview.h \
    application.h \
    closeeventfilter.h \
    configureview.h \
    controller.h \
    designerview.h \
    dialogview.h \
    engine.h \
    error.h \
    errorview.h \
    mainview.h \
    messagetabledelegate.h \
    messageview.h \
    util.h \
    view.h
LIBS += -lrtmidi
MOC_DIR = $${MAKEDIR}
OBJECTS_DIR = $${MAKEDIR}
QT += core gui uitools widgets
RCC_DIR = $${MAKEDIR}
RESOURCES += resources.qrc
SOURCES += aboutview.cpp \
    application.cpp \
    closeeventfilter.cpp \
    configureview.cpp \
    controller.cpp \
    designerview.cpp \
    dialogview.cpp \
    engine.cpp \
    error.cpp \
    errorview.cpp \
    main.cpp \
    mainview.cpp \
    messagetabledelegate.cpp \
    messageview.cpp \
    util.cpp \
    view.cpp
TARGET = midisnoop
TEMPLATE = app
VERSION = $${MIDISNOOP_VERSION}

################################################################################
# Install
################################################################################

# icon.files = src/lib/images/32x32/midisnoop.png

unix:!macx {
    #icon.path = $${MIDISNOOP_DATA_INSTALL_PATH}/icons/

    desktop.CONFIG += no_check_exist
    desktop.extra = ../install/build-desktop-file \
        '$${MIDISNOOP_APP_INSTALL_PATH}' '$${MIDISNOOP_DATA_INSTALL_PATH}'
    desktop.files = ../resources/midisnoop.desktop
    desktop.path = $${MIDISNOOP_DATA_INSTALL_PATH}/applications
    INSTALLS += desktop

} else {
    # icon.path = $${MIDISNOOP_DATA_INSTALL_PATH}
}
# INSTALLS += icon

target.path = $${MIDISNOOP_APP_INSTALL_PATH}
INSTALLS += target
