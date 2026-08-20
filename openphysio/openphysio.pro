# ###########################################################################
# Copyright (C) 2013 Sebastian Vandersee
# All rights reserved.
#
# This file is part of OpenPhysio.
#
# Please refer to the LICENSE file for terms of use.
# ###########################################################################
TEMPLATE = app
TARGET = openphysio

DEFINES += ENABLE_ENCODER_GENERIC

win32 {
    LIBS += -LD:\Frank\Dokumente\Projekte\physio\branch5\run
    LIBS += -lSmtpMime2 -lQZXing3 -llibcrypto-1_1-x64 -llibssl-1_1-x64
    INCLUDEPATH +=D:\Frank\Dokumente\Projekte\smtp\src\

    INCLUDEPATH +=D:\Frank\Dokumente\Projekte\qzxing\src\
}
linux-* {
    INCLUDEPATH += /usr/include/PCSC
    QMAKE_LIBS += -lpcsclite
    QT += widgets
    CONFIG += c++17
    LIBS += -L/home/Frank/ssl/openssl-1.1.1q -lcrypto -L/home/Frank/ssl/openssl-1.1.1q -lssl
    # Location of SMTP Library
    SMTP_LIBRARY_LOCATION = /home/Frank/Dokumente/Projekte/smtp/src/build/Desktop_Qt_5_15_2_GCC_64bit-Release
    QZXING_LIBRARY_LOCATION = /home/Frank/Dokumente/Projekte/qzxing/src/build/Desktop_Qt_5_15_2_GCC_64bit-Release
    LIBS += -L$$SMTP_LIBRARY_LOCATION -lSmtpMime
    LIBS += -L$$QZXING_LIBRARY_LOCATION -lQZXing

    INCLUDEPATH += /home/Frank/Dokumente/Projekte/smtp/src/
    INCLUDEPATH += /home/Frank/Dokumente/Projekte/qzxing/src/
    DEPENDPATH += $$SMTP_LIBRARY_LOCATION
    DEPENDPATH += $$QZXING_LIBRARY_LOCATION

    # PoDoFo für Fedora
    INCLUDEPATH += /usr/local/include
    LIBS += -L/usr/local/lib64 -lpodofo
}
mac {
#    CONFIG += x86 x86_64 ppc
    INCLUDEPATH += /usr/include/PCSC
    QMAKE_LIBS += -framework PCSC
    ICON = appicon/openphysio.icns
    QMAKE_INFO_PLIST = $$PWD/Info.plist
# QMAKE_LIBS += -framework CoreFoundation
# -L/usr/local/lib -lmcscm
}

INCLUDEPATH += $$SMTP_LIBRARY_LOCATION
DEPENDPATH += $$SMTP_LIBRARY_LOCATION

QT += core \
    help \
    gui \
    svg \
    xml \
    sql \
    network \
    printsupport \
    widgets

HEADERS += config.h \
    dlgdate.h \
    dlgbill2.h \
    dlgbill1.h \
    geneinv.h \
    protect.h \
    browsedoctemplates.h \
    browse.h \
    dlgreceipeoptions.h \
    dlgcourse.h \
    browseemployees.h \
    browsecourses.h \
    browseinvoices.h \
    browsepinvoices.h \
    gendoc.h \
    therapy.h \
    mydb.h \
    dlgreceipepriv.h \
    browseroutes.h \
    kvkread.h \
    browsephysicians.h \
    browseinsurances.h \
    browsereceipes.h \
    browsepatients.h \
    dlgabout.h \
    dlgreceipe.h \
    dlgempl.h \
    dlgphys.h \
    ctbcs.h \
    dlgpatient.h \
    openphysio.h \
    browsedocs.h \
    browsequittances.h \
    browsereminders.h \
    msgbox.h \
    browsereports.h \
    dlgdocfinding.h \
    browsefindings.h \
    dlglogin.h \
    dlgdocviewer.h \
    helperwidget.h \
    dlghelp.h \
    helperwrapper.h \
    displayfilter.h \
    browsecashpos.h \
    browsecash.h \
    widgetcashloss.h \
    widgetcasharticle.h \
    dlgbackup.h \
    calendarwidget.h \
    dlgmanagerooms.h \
    dlgmanagetherapygroups.h \
    dlgbrowse.h \
    dlgmanagecars.h \
    dlgpinvoices.h \
    dlgdatemove.h \
    browsepraxis.h \
    employeetools.h \
    dlgpraxis.h \
    dlgschedule.h \
    datetools.h \
    dlgreceipeprof.h \
    src_docedit/doctools.h \
    dlgtextblock.h
SOURCES += dlgdate.cpp \
    dlgbill2.cpp \
    dlgbill1.cpp \
    geneinv.cpp \
    protect.cpp \
    browsedoctemplates.cpp \
    browse.cpp \
    dlgreceipeoptions.cpp \
    dlgcourse.cpp \
    browseemployees.cpp \
    browsecourses.cpp \
    browseinvoices.cpp \
    browsepinvoices.cpp \
    gendoc.cpp \
    therapy.cpp \
    mydb.cpp \
    dlgreceipepriv.cpp \
    browseroutes.cpp \
    kvkread.cpp \
    browsephysicians.cpp \
    browseinsurances.cpp \
    browsereceipes.cpp \
    browsepatients.cpp \
    dlgabout.cpp \
    dlgreceipe.cpp \
    dlgempl.cpp \
    dlgphys.cpp \
    config.cpp \
    dlgpatient.cpp \
    main.cpp \
    openphysio.cpp \
    browsedocs.cpp \
    browsequittances.cpp \
    browsereminders.cpp \
    msgbox.cpp \
    browsereports.cpp \
    dlgdocfinding.cpp \
    browsefindings.cpp \
    dlglogin.cpp \
    dlgdocviewer.cpp \
    helperwidget.cpp \
    dlghelp.cpp \
    helperwrapper.cpp \
    displayfilter.cpp \
    browsecashpos.cpp \
    browsecash.cpp \
    widgetcashloss.cpp \
    widgetcasharticle.cpp \
    dlgbackup.cpp \
    calendarwidget.cpp \
    dlgmanagerooms.cpp \
    dlgmanagetherapygroups.cpp \
    dlgbrowse.cpp \
    dlgmanagecars.cpp \
    dlgpinvoice.cpp \
    dlgdatemove.cpp \
    browsepraxis.cpp \
    employeetools.cpp \
    dlgpraxis.cpp \
    dlgschedule.cpp \
    datetools.cpp \
    dlgreceipeprof.cpp \
    src_docedit/doctools.cpp \
    dlgtextblock.cpp
FORMS += dlgdate.ui \
    dlgbill2.ui \
    dlgbill1.ui \
    browsedoctemplates.ui \
    dlgreceipeoptions.ui \
    dlgcourse.ui \
    browseemployees.ui \
    browsecourses.ui \
    browseinvoices.ui \
    browsepinvoices.ui \
    dlgreceipepriv.ui \
    browseroutes.ui \
    browsephysicians.ui \
    browsepraxis.ui \
    browseinsurances.ui \
    browsereceipes.ui \
    browsepatients.ui \
    dlgabout.ui \
    dlgreceipe.ui \
    dlgempl.ui \
    dlgphys.ui \
    dlgpatient.ui \
    openphysio.ui \
    browsedocs.ui \
    browsequittances.ui \
    browsereminders.ui \
    browsereports.ui \
    dlgdocfinding.ui \
    browsefindings.ui \
    dlglogin.ui \
    dlgdocviewer.ui \
    dlghelp.ui \
    displayfilter.ui \
    browsecashpos.ui \
    browsecash.ui \
    widgetcashloss.ui \
    widgetcasharticle.ui \
    dlgbackup.ui \
    dlgmanagerooms.ui \
    dlgmanagetherapygroups.ui \
    dlgbrowse.ui \
    dlgmanagecars.ui \
    dlgdatemove.ui \
    dlgpinvoice.ui \
    dlgschedule.ui \
    dlgpraxis.ui \
    dlgreceipeprof.ui \
    dlgtextblock.ui
RESOURCES += images/images.qrc
