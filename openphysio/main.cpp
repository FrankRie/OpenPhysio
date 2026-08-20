/****************************************************************************
**
** Copyright (C) 2013 Sebastian Vandersee
**               2026 Dr. Frankl Riechert 
** All rights reserved.
**
** This file is part of OpenPhysio.
**
** Please refer to the LICENSE file for terms of use.
**
****************************************************************************/

#include "openphysio.h"

//#include <QtGui>
#include <QApplication>
#include "mydb.h"
#include "config.h"
//#include "gendoc.h"
#include "dlglogin.h"
#include "msgbox.h"
#include <QSslSocket>
#include <QDebug>

#ifdef Q_WS_MAC
//#include <CoreFoundation/CoreFoundation.h>
#endif


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

   // QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
   // QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    //QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));


    if (QSslSocket::supportsSsl()) {
        qDebug() << "Qt unterstützt SSL: Version" << QSslSocket::sslLibraryVersionString();
    } else {
        qDebug() << "Qt findet keine SSL-Unterstützung!";
    }

#ifdef Q_WS_MAC
    /*
    CFURLRef appUrlRef = CFBundleCopyBundleURL(CFBundleGetMainBundle());
    CFStringRef macPath = CFURLCopyFileSystemPath(appUrlRef,
                                           kCFURLPOSIXPathStyle);
    const char *pathPtr = CFStringGetCStringPtr(macPath,
                                           CFStringGetSystemEncoding());
    qDebug("Path = %s", pathPtr);
    CFRelease(appUrlRef);
    CFRelease(macPath);
    */
#endif
    myConf.init();
    myConf.searchAndSetConfigFilePath();
    if (!myConf.readXML()) MsgBox::error("Achtung: Fehler beim Lesen der Konfigurationsdatei (config.xml) im Verzeichnis " + myConf.getConfigFilePath() + " aufgetreten!");

#ifdef Q_OS_LINUX
    a.setStyleSheet("* { font-size: 12px }");
#endif

    QSplashScreen splash(QPixmap(":/splash.png"));
#ifdef Q_OS_LINUX
    // at least under Kubuntu 09.04 the default font size is annoying
    splash.setStyleSheet("* { font-size: 12px }");
#endif
    splash.showMessage("Verbindung zur Datenbank wird hergestellt..", Qt::AlignLeft | Qt::AlignBottom);
    splash.show();

    if (!db.DB_connect(myConf.getDatabase())) {
        MsgBox msgBox;
        msgBox.error("Keine Verbindung zur Datenbank! Bitte den OpenPhysio Administrator zur Problembehebung starten!");
        return -1;
    }

    int v = db.currentDatabaseVersion();
    if ( v != REQUIRED_DB_VERSION) {
        MsgBox msgBox;
        msgBox.error("Falsche Datenbankversion! Bitte den OpenPhysio Administrator zur Problembehebung starten!");
        db.DB_disconnect();
        return -1;
    }

    if (!db.initSurgery()) {
        MsgBox msgBox;
        msgBox.error("Praxisdaten konnten nicht geladen werden!");
        db.DB_disconnect();
        return -1;
    }

    // FIXME:
    // Add auto-login support (read from config whether credentials are stored)

    DlgLogin login;
    login.move(splash.pos().x() + splash.width() / 2 - login.width() / 2,
	       splash.pos().y() + splash.height() - 25);
    if (!login.init() ) {
	db.DB_disconnect();
	return -1;
    }
    if ( login.exec() == QDialog::Rejected ) {
	db.DB_disconnect();
	return -1;
    }

    splash.showMessage(QString::fromUtf8("Initialisierung läuft.."), Qt::AlignLeft | Qt::AlignBottom);
    cRole role;
    if (!login.getSelectedRole(role)) return 0;

    // load global base types (lists), such as employees, rooms, date types, states etc.
    db.loadBaseTypes();

    // init protection framework
    protect.setRole(role.getID());
    protect.init();

    openphysio w;
    cSurgery surg = db.getSurgery();
    QString s = w.windowTitle() + " [" + surg.getName1();
    if (!surg.getName2().isEmpty()) s = s + " " + surg.getName2();
    s += "]";
    w.setWindowTitle(s);
    //w.show();
    w.showMaximized();
    a.connect(&a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()));
    
    splash.finish(&w);
    
    a.exec();
    db.DB_disconnect();
    
    return 0;
}
