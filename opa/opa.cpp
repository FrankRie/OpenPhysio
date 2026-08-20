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
#include "ui_opa.h"
#include "confadmin.h"
#include "confdatabase.h"
#include "confsurgery.h"
#include "confprinter.h"
#include "confreader.h"
#include "confdisplay.h"
#include "confroles.h"
#include "confperm.h"
#include "confbackup.h"
#include "confupdates.h"
#include "confprices.h"
#include "confpricespriv.h"
#include "confhotkeys.h"
#include "conftakeover.h"
#include "confdocs.h"
#include "confplannerdisplay.h"
#include "confother.h"
#include "confshortcuts.h"
#include "conftextmodules.h"
#include "helperwrapper.h"
#include <dlgabout.h>
#include <msgbox.h>


opa::opa(QWidget *parent, int m) :
    QMainWindow(parent),
    m_ui(new Ui::opa)
{
    m_ui->setupUi(this);
    mode = m;

    if (mode == MODE_MIGRATE) { // we are already in a batch mode ...
        return;
    }

    helpWrapper.init();
    connect(m_ui->actionQuit, &QAction::triggered, qApp, &QCoreApplication::quit);
    connect(m_ui->actionHelp,  &QAction::triggered, this, &opa::slotHelp);
    connect(m_ui->actionAbout,  &QAction::triggered, this, &opa::slotAbout);

    if (!db.DB_connect(myConf.getDatabase())) {
        QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), QString::fromUtf8("Verbindung zur Datenbank fehlgeschlagen!\n"), QMessageBox::Ok);
        //return;
    } else {
        int v = db.currentDatabaseVersion();
        if ( v != REQUIRED_DB_VERSION) {
            QString msg = QString::fromUtf8("Benötigte Datenbankversion (") + QString::number(REQUIRED_DB_VERSION) + QString::fromUtf8(") stimmt nicht mir der tatsächlich vorhandenen Version (") + QString::number(v) + QString::fromUtf8(") überein!\n\nSie müssen erst eine Datenbankmigration vornehmen. Jetzt starten?");
            MsgBox msgBox;
            if (msgBox.execYesNoBox(msg, MsgBox::BTN_YES) == MsgBox::BTN_YES) {
                mode = MODE_MIGRATE;
            } else {
                m_ui->actionSave->setEnabled(false);
                m_ui->statusBar->hide();
                centralWidget()->hide();
                return;
            }
        }
    }

    QStringList mainLayers;
    if (mode == MODE_MIGRATE) {
        mainLayers << "Datenbank";
        conf_db_id = 0;
        confs.append(new ConfDatabase(NULL, (int)ConfBase::LAYER_DB, (int)ConfXML::AREA_DATABASE));
        ConfDatabase *cdb = (ConfDatabase*)(confs.at(conf_db_id));
        cdb->enableMigrationMode(true);
        connect(cdb, &ConfDatabase::sigMigrate, this, &opa::slotMigrate);
        m_ui->pbLocalSave->hide();
        m_ui->pbSaveGlobal->hide();
        m_ui->actionSave->setEnabled(false);
    } else {
        db.initSurgery();
        mainLayers << "Admin" << "Datenbank" << "Globale Einstellungen" << "Arbeitsplatzeinstellungen";
        conf_db_id = 1;
        confs.append(new ConfAdmin(NULL, (int)ConfBase::LAYER_ADMIN, (int)ConfXML::AREA_ADMIN));
        confs.append(new ConfDatabase(NULL, (int)ConfBase::LAYER_DB, (int)ConfXML::AREA_DATABASE));
        confs.append(new ConfSurgery(NULL, (int)ConfBase::LAYER_GLOBAL, (int)ConfXML::AREA_SURGERY));
        confs.append(new ConfRoles(NULL, (int)ConfBase::LAYER_GLOBAL, (int)ConfXML::AREA_ROLES));
        confs.append(new ConfPerm(NULL, (int)ConfBase::LAYER_GLOBAL, (int)ConfXML::AREA_PERMISSIONS));
        confs.append(new ConfDocs(NULL, (int)ConfBase::LAYER_GLOBAL, (int)ConfXML::AREA_DOCS));
        confs.append(new ConfShortcuts(NULL, (int)ConfBase::LAYER_GLOBAL, (int)ConfXML::AREA_SHORTCUTS));
        confs.append(new ConfTextModules(NULL, (int)ConfBase::LAYER_GLOBAL, (int)ConfXML::AREA_TEXT_MODULES));
        confs.append(new ConfPrices(NULL, (int)ConfBase::LAYER_GLOBAL, (int)ConfXML::AREA_PRICES));
        confs.append(new ConfPricesPriv(NULL, (int)ConfBase::LAYER_GLOBAL, (int)ConfXML::AREA_PRICES_PRIV));
        confs.append(new ConfUpdates(NULL, (int)ConfBase::LAYER_GLOBAL, (int)ConfXML::AREA_UPDATE));
        confs.append(new ConfTakeover(NULL, (int)ConfBase::LAYER_GLOBAL, (int)ConfXML::AREA_TAKEOVER));

        confs.append(new ConfPrinter(NULL, (int)ConfBase::LAYER_LOCAL, (int)ConfXML::AREA_PRINTER));
        confs.append(new ConfReader(NULL, (int)ConfBase::LAYER_LOCAL, (int)ConfXML::AREA_READER));
        confs.append(new ConfDisplay(NULL, (int)ConfBase::LAYER_LOCAL, (int)ConfXML::AREA_DISPLAY));
        confs.append(new ConfPlannerDisplay(NULL, (int)ConfBase::LAYER_LOCAL, (int)ConfXML::AREA_PLANNER));
        confs.append(new ConfHotkeys(NULL, (int)ConfBase::LAYER_LOCAL, (int)ConfXML::AREA_HOTKEYS));
        confs.append(new ConfBackup(NULL, (int)ConfBase::LAYER_LOCAL, (int)ConfXML::AREA_BACKUP));
        confs.append(new ConfOther(NULL, (int)ConfBase::LAYER_LOCAL, (int)ConfXML::AREA_OTHER));
    }

    m_ui->treeWidget->setColumnCount(1);
    QList<QTreeWidgetItem *> items;
    for (int i = 0; i < mainLayers.count(); i++) {
	items.append(new QTreeWidgetItem(m_ui->treeWidget));
        QTreeWidgetItem *item = items.last();
        item->setText(0, mainLayers.at(i));
        item->setIcon(0, QIcon(":/folder.png"));
        item->setExpanded(true);
	for (int j = 0; j < confs.count(); j++) {
	    if (confs.at(j)->getLayer() == i) {
                item->addChild(new QTreeWidgetItem());
                item->child(items.at(items.count() - 1)->childCount() - 1)->setText(0, confs.at(j)->getTitle());
	    }
	}
    }

    m_ui->treeWidget->setCurrentItem(items.at(0)->child(0), 0);

    for (int i = 0; i < confs.count(); i++) {
	confs.at(i)->init();
    }

    curArea = -1;
    loadArea(0);

    if (mode != MODE_MIGRATE) {
        connect(m_ui->actionSave, &QAction::triggered, this, &opa::slotSaveAll);
        connect(m_ui->pbSaveGlobal, &QPushButton::clicked, this, &opa::slotSaveAll);
        connect(m_ui->pbLocalSave, &QPushButton::clicked, this, &opa::slotSaveLocal);
        connect((ConfDatabase*)(confs.at(conf_db_id)), &ConfDatabase::sigConfDatabase, this, &opa::slotDBConfChange);
        connect((ConfSurgery*)(confs.at(conf_db_id + 1)), &ConfSurgery::sigConfDatabase, this, &opa::slotDBConfChange);
        connect(m_ui->treeWidget,&QTreeWidget::currentItemChanged, this, &opa::slotAreaSelected);
    }
}


opa::~opa()
{
    db.DB_disconnect();
    delete m_ui;
}


ConfBase* opa::selectArea(int a)
{
    if ( (a >= 0) && (a < confs.count())) return confs.at(a);
    return NULL;
}


void opa::deleteArea()
{
    QGridLayout *qgl = (QGridLayout *) m_ui->groupBox->layout();
    if (qgl == NULL) return;

    ConfBase *widget = selectArea(curArea);
    if (widget != NULL) {
	qgl->removeWidget(widget);
	widget->hide();
    }
}


void opa::loadArea(int a)
{
    if (curArea != a) {
	deleteArea();
	curArea = a;
	ConfBase *widget = selectArea(curArea);
	if (widget != NULL) {
	    m_ui->groupBox->show();
	    QGridLayout *qgl = (QGridLayout *) m_ui->groupBox->layout();
	    qgl->addWidget(widget, 0, 0);
	    widget->show();
            m_ui->groupBox->setTitle(QString::fromUtf8("Einstellungen für den Bereich '") + widget->getTitle() + "'");
	}
    }
}


void opa::slotAreaSelected(QTreeWidgetItem *w)
{
    if (w == NULL) return;
    bool found = false;

    for (int i = 0; i < confs.count(); i++) {
	if (w->text(0) == confs.at(i)->getTitle()) {
	    loadArea(i);
	    found = true;
	    break;
	}
    }

    if (!found) {
	deleteArea();
	curArea = -1;
	m_ui->groupBox->hide();
    }
}


void opa::slotSaveAll()
{
    for (int i = 0; i < confs.count(); i++) {
	confs.at(i)->write();
    }
    myConf.writeXML();
}


void opa::slotSaveLocal()
{
    if ( (curArea >=0) && (curArea < confs.count()) ) {
        confs.at(curArea)->write();
        myConf.writeXML();
    }
}


void opa::slotAbout()
{
    DlgAbout dlgAbout;
    QString s = QString::fromUtf8("OpenPhysio Admin Modul, ") + dlgAbout.getAboutText();
    dlgAbout.setAboutText(s);
    dlgAbout.exec();
}


void opa::slotHelp()
{
    helpWrapper.showHelpForKeyword("opa_main");
}


void opa::displayHelp()
{
    if ( (curArea >= 0) && (curArea < confs.count())) {
	helpWrapper.showHelpForKeyword(confs.at(curArea)->getKeyword());
    }
}


void opa::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
	case Qt::Key_F1:
	    displayHelp();
	    break;
	default: break;
    }
}


void opa::slotDBConfChange()
{
    // save DB properties to config.xml
    confs.at(conf_db_id)->write();
    myConf.writeXML();

    // reload all DB-related config modules
    for (int i = 0; i < confs.count(); i++) {
        if (confs.at(i)->getLayer() == ConfBase::LAYER_GLOBAL) {
            confs.at(i)->init();
        }
    }
}


bool opa::migrate(cDatabase d, QTextEdit *te)
{
    if (mode != MODE_MIGRATE) return false;

    if (!db.DB_connect(myConf.getDatabase())) {
        QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), "Verbindung zur Datenbank fehlgeschlagen!\n", QMessageBox::Ok);
    }

    db_version = db.currentDatabaseVersion();
    if (db_version == REQUIRED_DB_VERSION) {
        if (te != NULL) te->append("Keine Datenbankmigration erforderlich");
        return true;
    }

    if (db_version < 3) {
        if (te != NULL) te->append(QString::fromUtf8("Die Datenbankversion ist zu alt. Eine Datenbankmigration wird leider nicht unterstützt."));
        return true;
    }

    if (d.getDriver() == "QSQLITE") return migrate_sqlite(d, te);
    else return migrate_mysql(d, te);
}


bool opa::migrate_sqlite(cDatabase &d, QTextEdit *te)
{
    ConfDatabase *cdb = (ConfDatabase*)(confs.at(conf_db_id));
    QString bak_path = cdb->getBackupPath();
    QDir dir(bak_path);
    QString backup_name = dir.filePath("migration_backup" + QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss") + ".db");
    te->append("Erstelle Sicherungskopie: " + backup_name);
    QFile file(d.getFile());
    if (!file.copy(backup_name)) {
        te->append("<b>FEHLER</b>: Sicherung fehlgeschlagen!");
        return false;
    } te->append("Sicherung erfolgreich\n");

    te->append("Beginne Migration der Datenbank " + d.getFile());

    for (int i = db_version; i < REQUIRED_DB_VERSION; i++) {
        QString schema_update = "migrate/migrate_sqlite_v" + QString::number(i + 1) + ".sql";
#ifdef Q_WS_MAC
        schema_update = QApplication::applicationDirPath() + "/" + schema_update;
#endif
        te->append("Aktuelles Update: " + schema_update);
        QFile schema_file(schema_update);
        if (!schema_file.open(QIODevice::ReadOnly)) {
            te->append(QString::fromUtf8("<b>FEHLER</b>: Update konnte nicht gelesen werden!"));
            return false;
        }

        QTextStream t(&schema_file);
        QString line = "";
        QStringList sqlStatements;
        QString statement = "";
        while (!line.isNull()) {
            line = t.readLine();
            if (line.isEmpty()) continue;
            if (line.left(2) == "--") continue; // skip SQL comments
            if (line.left(2) == "/*") continue;
            // we need to build a one-line SQL statements from multiline statements
            // (a statement is detected by the fact that it ends with a semicolon)
            statement += line;
            if (line.right(1) == ";") {
                sqlStatements.append(statement);
                statement = "";
            }
        }
        schema_file.close();

        if (sqlStatements.count() > 0) {
            if (!db.executeBulk(sqlStatements, false, false)) {
                te->append(QString::fromUtf8("<b>FEHLER</b>: Update konnte nicht eingespielt werden!"));
                return false;
            }
        } else {
            te->append(QString::fromUtf8("<b>FEHLER</b>: Es wurden keine Update-Kommandos gefunden!"));
        }
        te->append("<br>");

    }

    te->append("Datenbankmigration abgeschlossen\n");
    te->append("<b>Bitte OpenPhysio Administrator beenden!!!</b>");
    QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), QString::fromUtf8("Bitte OpenPhysio Administrator schließen!"), QMessageBox::Ok);
    return true;
}


bool opa::migrate_mysql(cDatabase &d, QTextEdit *te)
{
    if (!db.DB_connect(d)) {
        QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), "Verbindung zur Datenbank fehlgeschlagen!\n", QMessageBox::Ok);
        return false;
    }

    MsgBox msgBox;
    if (msgBox.execYesNoBox(QString::fromUtf8("Sie sollten vorher dringend ein Backup Ihrer Datenbank anfertigen!!!\nOpenPhysio unterstützt ein MySQL-Backup bislang nicht!\n\nTrotzdem fortfahren?"), MsgBox::BTN_NO) == MsgBox::BTN_NO) {
        return false;
    }

    te->append("Beginne Migration der Datenbank " + d.getFile());

    for (int i = db_version; i < REQUIRED_DB_VERSION; i++) {
        QString schema_update = "migrate/migrate_mysql_v" + QString::number(i + 1) + ".sql";
#ifdef Q_WS_MAC
        schema_update = QApplication::applicationDirPath() + "/" + schema_update;
#endif
        te->append("Aktuelles Update: " + schema_update);
        QFile schema_file(schema_update);
        if (!schema_file.open(QIODevice::ReadOnly)) {
            te->append(QString::fromUtf8("<b>FEHLER</b>: Update konnte nicht gelesen werden!"));
            return false;
        }

        QTextStream t(&schema_file);
        QString line = "";
        QStringList sqlStatements;
        QString statement = "";
        while (!line.isNull()) {
            line = t.readLine();
            if (line.isEmpty()) continue;
            if (line.left(2) == "--") continue; // skip SQL comments
            if (line.left(2) == "/*") continue;
            // we need to build a one-line SQL statements from multiline statements
            // (a statement is detected by the fact that it ends with a semicolon)
            statement += line;
            if (line.right(1) == ";") {
                sqlStatements.append(statement);
                statement = "";
            }
        }
        schema_file.close();

        if (sqlStatements.count() > 0) {
            if (!db.executeBulk(sqlStatements, false, false)) {
                te->append(QString::fromUtf8("<b>FEHLER</b>: Update konnte nicht eingespielt werden!"));
                return false;
            }
        } else {
            te->append(QString::fromUtf8("<b>FEHLER</b>: Es wurden keine Update-Kommandos gefunden!"));
        }
        te->append("<br>");

    }

    te->append("Datenbankmigration abgeschlossen\n");
    te->append("Bitte OpenPhysio Administrator beenden!!!");
    QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), QString::fromUtf8("Bitte OpenPhysio Administrator schließen!"), QMessageBox::Ok);
    return true;
}


void opa::slotMigrate(cDatabase *cdb, QTextEdit *te)
{
    if (cdb != NULL) migrate(*cdb, te);
}
