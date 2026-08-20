/****************************************************************************
**
** Copyright (C) 2013 Sebastian Vandersee
** All rights reserved.
**
** This file is part of OpenPhysio.
**
** Please refer to the LICENSE file for terms of use.
**
****************************************************************************/

#include "opa.h"
#include <QtGui>
#include <QApplication>
#include <mydb.h>
#include "msgbox.h"
#include "dlgaccess.h"


#if 1
void usage()
{
    printf("OpenPhysio Administrator\n");
    printf("-------------------\n");
    printf("opa <mode> <parameter>\n");
    printf("Modi:\n");
    printf(" migrate\n\n");
    printf("migrate - Parameter:\n");
    printf(" -t <n>    Datenbanktyp 0=SQLite / 1=MySQL\n");
    printf(" -f <name> Dateiname der Datenbank (nur bei -t 0)\n");
    printf(" -h <host> Hostname oder IP-Adresse des MySQL-Servers (nur bei -t 1)\n");
    printf(" -u <user> Benutzername (nur bei -t 1)\n");
    printf(" -p <pass> Passwort (nur bei -t 1)\n");
    exit(0);
}
#endif


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    myConf.init();
    myConf.searchAndSetConfigFilePath();

    if (!myConf.readXML()) MsgBox::error("Achtung: Fehler beim Lesen der Konfigurationsdatei (config.xml) aufgetreten!");

    if (DlgAccess::isPasswordSet()) {
        DlgAccess dlg;
        if (dlg.exec() == QDialog::Rejected) exit(0);
    }


#if 1
    QStringList list = a.arguments();
    bool migrate_mode = false;
    int db_type = 0;  // default is SQLite
    QString file, host, user, pw;

    if (list.count() > 1) {
        if (list.at(1) == "-?") usage();
        if (list.at(1) != "migrate") usage();
        migrate_mode = true;

        int i = 2;
        int limit = list.count() - 1;
        while (i < limit) {
            if ((list.at(i) == "-t") && (i + 1 <= limit)) db_type = list.at(1 + i++).toInt();
            if ((list.at(i) == "-f") && (i + 1 <= limit)) file = list.at(1 + i++);
            if ((list.at(i) == "-h") && (i + 1 <= limit)) host = list.at(1 + i++);
            if ((list.at(i) == "-u") && (i + 1 <= limit)) user = list.at(1 + i++);
            if ((list.at(i) == "-p") && (i + 1 <= limit)) pw = list.at(1 + i++);
            i++;
        }
    }

    // TODO: sanity checks

    if (migrate_mode) {
        opa w(NULL, opa::MODE_MIGRATE);
        cDatabase d;
        if (db_type == 0) {
            d.setDriver("QSQLITE");
            d.setFile(file);
        } else {
            d.setDriver("QMYSQL");
            d.setHost(host);
            d.setUser(user);
            d.setPW(pw);
        }
        w.migrate(d, NULL);
    } else {
#endif
        opa w;
        w.show();
        a.connect(&a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()));
        a.exec();
#if 1
    }
#endif
    return 0;
}
