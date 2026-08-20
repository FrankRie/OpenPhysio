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

#include "confbackup.h"
#include "ui_confbackup.h"
#include <msgbox.h>


ConfBackup::ConfBackup(QWidget *parent, int l, int a) :
    ConfBase(parent, l, a),
    m_ui(new Ui::ConfBackup)
{
    m_ui->setupUi(this);
    setTitle(QString::fromUtf8("Datensicherung/Wiederherstellung"));
    setKeyword("opa_backup");

    connect(m_ui->pbDir, &QPushButton::clicked, this, &ConfBackup::slotSelectBackupDir);
    connect(m_ui->pbBackup, &QPushButton::clicked, this, &ConfBackup::slotBackup);
    connect(m_ui->pbRecovery, &QPushButton::clicked, this, &ConfBackup::slotRecovery);
}


ConfBackup::~ConfBackup()
{
    delete m_ui;
}


void ConfBackup::init()
{
    m_ui->lineEdit->setText(myConf.getBackupDir());
}


bool ConfBackup::write()
{
    myConf.set(ConfXML::AREA_BACKUP, "dir_backup", m_ui->lineEdit->text());
    return true;
}


void ConfBackup::slotSelectBackupDir()
{
    QString dir = QFileDialog::getExistingDirectory(this, QString::fromUtf8("Datenverzeichnis wählen"), ".",
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    m_ui->lineEdit->setText(dir);
}


void ConfBackup::slotBackup()
{
    m_ui->textEdit->clear();

    QString path = m_ui->lineEdit->text();
    if (path.isEmpty()) {
        m_ui->textEdit->append(QString::fromUtf8("FEHLER: Sicherungsverzeichnis wurde noch nicht gewählt!"));
        return;
    }
    if (path.right(1) != "/") path += "/";
    QString fname = QString::fromUtf8("testbak.tmp");
    QString fullname = "";
    fullname = path + fname;

    m_ui->textEdit->append(QString::fromUtf8("Prüfe Berechtigungen für Verzeichnis ") + path);
    QFile file(fullname);
    if (!file.open(QIODevice::ReadWrite))
    {
        m_ui->textEdit->append(QString::fromUtf8("FEHLER: Temporäre Datei konnte nicht angelegt werden!"));
        return;
    } else m_ui->textEdit->append(QString::fromUtf8("Erfolgreich\n"));

    m_ui->textEdit->append(QString::fromUtf8("Entferne temporäre Datei"));
    if (!file.remove()) {
        m_ui->textEdit->append(QString::fromUtf8("FEHLER: Temporäre Datei konnte nicht entfernt werden!"));
    } else  m_ui->textEdit->append(QString::fromUtf8("Erfolgreich\n"));

    file.close();

    // FIXME: Maybe we should disconnect from our database before we copy the database file?

    if (myConf.get(ConfXML::AREA_DATABASE, "driver") == "QSQLITE") {
        m_ui->textEdit->append(QString::fromUtf8("Sichere lokale Datenbankdatei"));
        //fname = myConf.get(ConfXML::AREA_DATABASE, "file");
        //fname = myConf.getConfigFilePath() + fname;
        fname = db.DB_getProperties().getFile();
        m_ui->textEdit->append(QString::fromUtf8("Quelle: ") + fname);
        QFile f;
        if (!f.exists(fname)) {
            m_ui->textEdit->append(QString::fromUtf8("FEHLER: Quelldatei konnte nicht gefunden werden!"));
        } else {
            file.setFileName(fname);
            fullname = path + "backup_" + QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss") + ".db";
            m_ui->textEdit->append(QString::fromUtf8("Ziel: ") + fullname);
            if (!file.copy(fullname)) {
                m_ui->textEdit->append(QString::fromUtf8("FEHLER: Sicherung fehlgeschlagen!"));
            } else {
                m_ui->textEdit->append(QString::fromUtf8("Erfolgreich"));
            }
        }
    } else {
        // mysqldump -u <user> -p<pw> -h <host> <database> -c > backup.sql
        m_ui->textEdit->append(QString::fromUtf8("ACHTUNG: Sicherung für MySQL noch nicht implementiert! Bitte auf MySQL mittels mysqldump eine Sicherung vornehmen!"));
    }
}


void ConfBackup::slotRecovery()
{
    // TODO: Check if backup database file matches the version requirements
    // (might be an outdated database with a different database schema!)

    m_ui->textEdit->clear();

    if (myConf.get(ConfXML::AREA_DATABASE, "driver") == "QSQLITE") {

        // select backup used for recovery purposes
        QString fileName = QFileDialog::getOpenFileName(this,
        QString::fromUtf8("Sicherungsdatei zur Datenübernahme auswählen"), m_ui->lineEdit->text(), QString::fromUtf8("Sicherungsdateien (*.db)"));
        if (fileName.isEmpty()) {
            m_ui->textEdit->append(QString::fromUtf8("WARNUNG: Datenübername wurde abgebrochen!"));
            return;
        }

        MsgBox msgBox;
        if (msgBox.execYesNoBox(QString::fromUtf8("ACHTUNG: Ihre aktuellen Daten werden unwiederbringlich mit den gesicherten Daten überschrieben!!! Sie sollten daher immer erst eine weitere Sicherung anfertigen.\n\nWirklich fortfahren?"),
                                MsgBox::BTN_NO) == MsgBox::BTN_NO) {
            m_ui->textEdit->append(QString::fromUtf8("WARNUNG: Datenübernahme wurde abgebrochen!"));
            return;
        }

        m_ui->textEdit->append(QString::fromUtf8("Folgende Sicherung wird wiederhergestellt: ") + fileName);

        // disconnect from database
        m_ui->textEdit->append(QString::fromUtf8("Trenne Datenbankverbindung"));
        db.DB_disconnect();

        // TODO: Instead of deleting the database file we should
        //  a) check if a "last resort" file already exists - and if so remove it
        //  b) rename current database file

        QFile fremove;
        QString old = db.DB_getProperties().getFile();
        m_ui->textEdit->append(QString::fromUtf8("Entferne aktuelle Datenbank ") + old);
        if (!fremove.remove(old)) {
            m_ui->textEdit->append(QString::fromUtf8("FEHLER: Aktuelle Datenbank kann nicht entfernen werden!"));
            db.DB_connect(myConf.getDatabase());
            return;
        } else m_ui->textEdit->append(QString::fromUtf8("Aktuelle Datenbank wurde entfernt!"));

        // recover database
        QFile file(fileName);
        if (!file.copy(old)) {
            m_ui->textEdit->append(QString::fromUtf8("FEHLER: Datenbank konnte nicht wiederhergestellt werden!"));
            db.DB_connect(myConf.getDatabase());
            return;
        }

        // re-connect to database
        m_ui->textEdit->append(QString::fromUtf8("Verbindungsaufbau mit wiederhergestellter Datenbank"));
        if (!db.DB_connect(myConf.getDatabase())) {
            QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), QString::fromUtf8("FEHLER: Verbindung zur Datenbank fehlgeschlagen!"), QMessageBox::Ok);
            return;
        } else {
            QMessageBox::warning(this, QString::fromUtf8("OpenPhysio - Hinweis"), QString::fromUtf8("Wiederherstellung abgeschlossen. Bitte OpenPhysio Administrator schließen!"), QMessageBox::Ok);
        }
    } else {
        // mysql -u <user> -p<pw> <database> < backup.sql
        m_ui->textEdit->append(QString::fromUtf8("ACHTUNG: Wiederherstellung für MySQL noch nicht implementiert! Bitte auf MySQL mittels mysql Kommando eine Wiederherstellung vornehmen!"));
    }
}
