# ###########################################################################
#
#
# Copyright (C) 2013 Sebastian Vandersee
# All rights reserved.
#
# This file is part of OpenPhysio.
#
# Please refer to the LICENSE file for terms of use.
#
# ###########################################################################

TEMPLATE = app
TARGET = opa

DEFINES += OPA
DEFINES += Q_WS_WIN
#win32 {
#    QMAKE_LIBS += E:\Frank\Dokumente\Projekte\physio\run\appicon/appicon.res

#}
#linux-* {
#    INCLUDEPATH += /usr/include/PCSC
#    QMAKE_LIBS += -lpcsclite
#}
#mac {
#    CONFIG += x86 x86_64 ppc
#     INCLUDEPATH += /usr/include/PCSC
#     QMAKE_LIBS += -framework PCSC
#     ICON = appicon/opa.icns
#     QMAKE_INFO_PLIST = $$PWD/Info.plist
#}

INCLUDEPATH += ../openphysio

QT += printsupport \
    core \
    gui \
    sql \
    xml \
    svg \
    help

HEADERS += opa.h \
    confsurgery.h \
    confdatabase.h \
    confbase.h \
    confprinter.h \
    ../openphysio/config.h \
    ../openphysio/mydb.h \
    ../openphysio/msgbox.h \
    ../openphysio/dlgabout.h \
    ../openphysio/kvkread.h \
    ../openphysio/helperwidget.h \
    ../openphysio/dlghelp.h \
    ../openphysio/helperwrapper.h \
    confreader.h \
    confdisplay.h \
    confroles.h \
    confbackup.h \
    confupdates.h \
    confpricespriv.h \
    confperm.h \
    dlgupdateinsurances.h \
    confhotkeys.h \
    conftakeover.h \
    confdocs.h \
    confplannerdisplay.h \
    confother.h \
    confadmin.h \
    dlgaccess.h \
    confshortcuts.h \
    confprices.h \
    ../openphysio/src_docedit/dlgdoceditor.h \
    ../openphysio/src_docedit/dlgdoceditdata.h \
    ../openphysio/src_docedit/dlgdoceditnew.h \
    ../openphysio/src_docedit/dlgdocopen.h \
    ../openphysio/src_docedit/dlgdocsave.h \
    ../openphysio/src_docedit/doctools.h \
    conftextmodules.h
SOURCES += main.cpp \
    opa.cpp \
    confsurgery.cpp \
    confdatabase.cpp \
    confbase.cpp \
    confprinter.cpp \
    ../openphysio/config.cpp \
    ../openphysio/mydb.cpp \
    ../openphysio/msgbox.cpp \
    ../openphysio/dlgabout.cpp \
    ../openphysio/kvkread.cpp \
    ../openphysio/helperwidget.cpp \
    ../openphysio/dlghelp.cpp \
    ../openphysio/helperwrapper.cpp \
    confreader.cpp \
    confdisplay.cpp \
    confroles.cpp \
    confbackup.cpp \
    confupdates.cpp \
    confpricespriv.cpp \
    confperm.cpp \
    dlgupdateinsurances.cpp \
    confhotkeys.cpp \
    conftakeover.cpp \
    confdocs.cpp \
    confplannerdisplay.cpp \
    confother.cpp \
    confadmin.cpp \
    dlgaccess.cpp \
    confshortcuts.cpp \
    confprices.cpp \
    ../openphysio/src_docedit/dlgdoceditor.cpp \
    ../openphysio/src_docedit/dlgdoceditdata.cpp \
    ../openphysio/src_docedit/dlgdoceditnew.cpp \
    ../openphysio/src_docedit/dlgdocopen.cpp \
    ../openphysio/src_docedit/dlgdocsave.cpp \
    ../openphysio/src_docedit/doctools.cpp \
    conftextmodules.cpp
FORMS += opa.ui \
    ../openphysio/dlgabout.ui \
    ../openphysio/dlghelp.ui \
    confsurgery.ui \
    confdatabase.ui \
    confprinter.ui \
    confreader.ui \
    confdisplay.ui \
    confroles.ui \
    confbackup.ui \
    confupdates.ui \
    confpricespriv.ui \
    confperm.ui \
    dlgupdateinsurances.ui \
    confhotkeys.ui \
    conftakeover.ui \
    confdocs.ui \
    confplannerdisplay.ui \
    confother.ui \
    confadmin.ui \
    dlgaccess.ui \
    confshortcuts.ui \
    confprices.ui \
    ../openphysio/src_docedit/dlgdoceditor.ui \
    ../openphysio/src_docedit/dlgdoceditdata.ui \
    ../openphysio/src_docedit/dlgdoceditnew.ui \
    ../openphysio/src_docedit/dlgdocopen.ui \
    ../openphysio/src_docedit/dlgdocsave.ui \
    conftextmodules.ui
RESOURCES += images/images.qrc \
    ../openphysio/src_docedit/docedit.qrc
