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

#include "confdatabase.h"
#include "ui_confdatabase.h"
#include <msgbox.h>


ConfDatabase::ConfDatabase(QWidget *parent, int l, int a) :
    ConfBase(parent, l, a),
    m_ui(new Ui::ConfDatabase)
{
    m_ui->setupUi(this);
    setTitle("Datenbank-Konfiguration");
    setKeyword("opa_dbconfig");

    migration_mode = false;
    m_ui->textEdit->hide();
    m_ui->pbMigrate->hide();
    m_ui->widgetBackup->hide();
}


ConfDatabase::~ConfDatabase()
{
    delete m_ui;
}


void ConfDatabase::enableMigrationMode(bool b)
{
    migration_mode = b;
    if (migration_mode) {
        m_ui->textEdit->show();
        m_ui->pbMigrate->show();
        m_ui->widgetBackup->show();
        connect(m_ui->pbMigrate, &QPushButton::clicked, this, &ConfDatabase::slotMigrate);
        connect(m_ui->pbBackupPath, &QPushButton::clicked, this, &ConfDatabase::slotBackupPath);
    }
}


void ConfDatabase::init()
{
    //m_ui->leFile->setText(myConf.get(ConfXML::AREA_DATABASE, "file"));
//    qDebug() << myConf.getDatabase().getFile();
//    qDebug() << db.DB_getProperties().getFile();
    m_ui->leFile->setText(db.DB_getProperties().getFile());
    m_ui->leHost->setText(myConf.get(ConfXML::AREA_DATABASE, "host"));
    m_ui->leUser->setText(myConf.get(ConfXML::AREA_DATABASE, "user"));
    QString pw_decoded = QByteArray::fromBase64(myConf.get(ConfXML::AREA_DATABASE, "pw").toUtf8());
                                              //  .toAscii());
    m_ui->lePW->setText(pw_decoded);
    if (myConf.get(ConfXML::AREA_DATABASE, "driver") == "QSQLITE") {
	m_ui->cbSetupType->setCurrentIndex(0);
	slotSetupChanged(0);
    } else {
	m_ui->cbSetupType->setCurrentIndex(1);
	slotSetupChanged(1);
    }
    m_ui->leBackupPath->setText(myConf.getBackupDir());
    connect(m_ui->pbTestConn, &QPushButton::clicked, this, &ConfDatabase::slotTestConnection);
    connect(m_ui->cbSetupType, &QComboBox::currentIndexChanged, this, &ConfDatabase::slotSetupChanged);
}


bool ConfDatabase::write()
{
    if (migration_mode) return true;

    if (m_ui->cbSetupType->currentIndex() == 0) {
	myConf.set(ConfXML::AREA_DATABASE, "driver", "QSQLITE");
    } else if (m_ui->cbSetupType->currentIndex() == 1) {
	myConf.set(ConfXML::AREA_DATABASE, "driver", "QMYSQL");
    }
    myConf.set(ConfXML::AREA_DATABASE, "file", m_ui->leFile->text());
    myConf.set(ConfXML::AREA_DATABASE, "host", m_ui->leHost->text());
    myConf.set(ConfXML::AREA_DATABASE, "user", m_ui->leUser->text());
    myConf.set(ConfXML::AREA_DATABASE, "pw", m_ui->lePW->text().toUtf8().toBase64());
               //.toAscii().toBase64());
    return true;
}


void ConfDatabase::slotTestConnection()
{
    cDatabase cdb_save = db.DB_getProperties();
    cDatabase cdb;
    db.DB_disconnect();

    if (m_ui->cbSetupType->currentIndex() == 0) {
	cdb.setDriver("QSQLITE");
	cdb.setName(myConf.get(ConfXML::AREA_DATABASE, "name"));
	cdb.setFile(m_ui->leFile->text());
    } else if (m_ui->cbSetupType->currentIndex() == 1) {
	cdb.setDriver("QMYSQL");
	cdb.setName(myConf.get(ConfXML::AREA_DATABASE, "name"));
	cdb.setHost(m_ui->leHost->text());
	cdb.setUser(m_ui->leUser->text());
	cdb.setPW(m_ui->lePW->text());
    }
    if (db.DB_connect(cdb)) {
        if (migration_mode) {
            QMessageBox::information(this, "OpenPhysio", "Verbindungstest erfolgreich", QMessageBox::Ok);
            return;
        }
        MsgBox msgbox;
        if ( msgbox.execYesNoBox(QString::fromUtf8("Verbindungstest erfolgreich!\nSollen diese Einstellungen jetzt übernommen werden?"), MsgBox::BTN_YES) == MsgBox::BTN_YES) {
            db.initSurgery();
            emit sigConfDatabase();
        } else {
            db.DB_disconnect();
            db.DB_connect(cdb_save);
        }

    } else {
	QMessageBox::warning(this, "OpenPhysio", "Verbindungstest fehlgeschlagen", QMessageBox::Ok);
        db.DB_disconnect();
        db.DB_connect(cdb_save);
    }
}


void ConfDatabase::slotSetupChanged(int i)
{
    if (i == 0) {
	m_ui->widgetFile->show();
	m_ui->widgetServer->hide();
    } else {
	m_ui->widgetFile->hide();
	m_ui->widgetServer->show();
    }
}


void ConfDatabase::slotMigrate()
{
    if (m_ui->cbSetupType->currentIndex() == 0) {
        QString path = m_ui->leBackupPath->text();
        if (path.isEmpty()) {
            QMessageBox::warning(this, "OpenPhysio", QString::fromUtf8("Es wurde noch keine Verzeichnis für die Sicherungskopie konfiguriert!"), QMessageBox::Ok);
            return;
        }
        QDir dir(path);
        if (!dir.exists()) {
            QMessageBox::warning(this, "OpenPhysio", QString::fromUtf8("Das Verzeichnis für die Sicherungskopie existiert nicht!"), QMessageBox::Ok);
            return;
        }
        migration_cdb.setDriver("QSQLITE");
        migration_cdb.setName(myConf.get(ConfXML::AREA_DATABASE, "name"));
        migration_cdb.setFile(m_ui->leFile->text());
    } else if (m_ui->cbSetupType->currentIndex() == 1) {
        migration_cdb.setDriver("QMYSQL");
        migration_cdb.setName(myConf.get(ConfXML::AREA_DATABASE, "name"));
        migration_cdb.setHost(m_ui->leHost->text());
        migration_cdb.setUser(m_ui->leUser->text());
        migration_cdb.setPW(m_ui->lePW->text());
    }
    emit sigMigrate(&migration_cdb, m_ui->textEdit);
}


void ConfDatabase::slotBackupPath()
{
    QString dir = QFileDialog::getExistingDirectory(this, QString::fromUtf8("Pfad für Sicherungskopie wählen"), ".",
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    m_ui->leBackupPath->setText(dir);
}


QString ConfDatabase::getBackupPath()
{
    if (m_ui->leBackupPath->text().isEmpty()) return "";
    QDir dir(m_ui->leBackupPath->text());
    if (!dir.exists()) return "";
    return m_ui->leBackupPath->text();
}
