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

#include "dlgbackup.h"
#include "ui_dlgbackup.h"
#include "config.h"


DlgBackup::DlgBackup(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::DlgBackup)
{
    m_ui->setupUi(this);

    m_ui->labelBackup->setText(myConf.getBackupDir());
    connect(m_ui->pbBackup, &QPushButton::clicked, this, &DlgBackup::slotBackup);
    connect(m_ui->pbCancel, &QPushButton::clicked, this, &DlgBackup::slotCancel);
}


DlgBackup::~DlgBackup()
{
    delete m_ui;
}


void DlgBackup::slotBackup()
{
    // TODO: Check if backup database file matches the version requirements
    // (might be an outdated database with a different database schema!)
    m_ui->plainTextEdit->clear();

    QString path = myConf.getBackupDir();
    if (path.isEmpty()) {
        m_ui->plainTextEdit->appendPlainText(QString::fromUtf8("FEHLER: Sicherungsverzeichnis wurde noch nicht konfiguriert!"));
        m_ui->plainTextEdit->appendPlainText(QString::fromUtf8("Bitte korrigieren Sie diese Einstellung mittels OpenPhysio Administrator!"));
        return;
    }
    if (path.right(1) != "/") path += "/";
    QString fname, fullname;

    QFile bak_dir;
    if (!bak_dir.exists(path)) {
        m_ui->plainTextEdit->appendPlainText(QString::fromUtf8("Das konfigurierte Sicherungsverzeichnis existiert nicht: ") + path);
        m_ui->plainTextEdit->appendPlainText(QString::fromUtf8("Bitte korrigieren Sie diese Einstellung mittels OpenPhysio Administrator!"));
        return;
    }
    if (myConf.get(ConfXML::AREA_DATABASE, "driver") == "QSQLITE") {
        m_ui->plainTextEdit->appendPlainText(QString::fromUtf8("Sichere lokale Datenbankdatei"));
        //fname = myConf.get(ConfXML::AREA_DATABASE, "file");
        //fname = myConf.getConfigFilePath() + fname;
        fname = db.DB_getProperties().getFile();
        m_ui->plainTextEdit->appendPlainText(QString::fromUtf8("Quelle:") + " " + fname);
        QFile f;
        if (!f.exists(fname)) {
            m_ui->plainTextEdit->appendPlainText(QString::fromUtf8("FEHLER: Quelldatei konnte nicht gefunden werden!"));
        } else {
            QFile file;
            file.setFileName(myConf.get(ConfXML::AREA_DATABASE, "file"));
            fullname = path + "backup_" + QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss") + ".db";
            m_ui->plainTextEdit->appendPlainText(QString::fromUtf8("Ziel:") + " " + fullname);
            if (!file.copy(fullname)) {
                m_ui->plainTextEdit->appendPlainText(QString::fromUtf8("FEHLER: Sicherung fehlgeschlagen!"));
            } else {
                m_ui->plainTextEdit->appendPlainText(QString::fromUtf8("Erfolgreich"));
            }
        }
    } else {
        m_ui->plainTextEdit->appendPlainText(QString::fromUtf8("ACHTUNG: Sicherung für MySQL noch nicht implementiert! Bitte auf MySQL-Server mittels mysqldump eine Sicherung manuell vornehmen!"));
    }
}


void DlgBackup::slotCancel()
{
    this->reject();
}
