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

#include "dlgphys.h"
#include "mydb.h"
#include "protect.h"
#include "msgbox.h"
#include "helperwrapper.h"


DlgPhys::DlgPhys(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);
    flag_change = false;

    connect(ui.pb_save, &QPushButton::clicked, this, &DlgPhys::slotSavePhys);
    connect(ui.pb_cancel, &QPushButton::clicked, this, &DlgPhys::slotCancelPhys);

    db.retrieveItemList(myDB::TABLE_PHYSICIAN_TYPES, phys_types);
    for (int i = 0; i < phys_types.count(); i++) {
	ui.cbPType->addItem(((cPhysType*)(phys_types.at(i)))->getDescription());
    }

    connect(ui.le_city, &QLineEdit::textChanged, this, &DlgPhys::slotModified);
    connect(ui.le_email, &QLineEdit::textChanged, this, &DlgPhys::slotModified);
    connect(ui.le_fax, &QLineEdit::textChanged, this, &DlgPhys::slotModified);
    connect(ui.le_mobile, &QLineEdit::textChanged, this, &DlgPhys::slotModified);
    connect(ui.le_street, &QLineEdit::textChanged, this, &DlgPhys::slotModified);
    connect(ui.le_surgery, &QLineEdit::textChanged, this, &DlgPhys::slotModified);
    connect(ui.le_tel, &QLineEdit::textChanged, this, &DlgPhys::slotModified);
    connect(ui.le_title, &QLineEdit::textChanged, this, &DlgPhys::slotModified);
    connect(ui.le_zip, &QLineEdit::textChanged, this, &DlgPhys::slotModified);
    connect(ui.cbPType, &QComboBox::currentIndexChanged, this, &DlgPhys::slotModified);
    connect(ui.leBSNR, &QLineEdit::textChanged, this, &DlgPhys::slotModified);
    connect(ui.leLANR, &QLineEdit::textChanged, this, &DlgPhys::slotModified);
    connect(ui.pbClinic, &QPushButton::clicked, this, &DlgPhys::slotClinic);
    connect(ui.pbZipCode, &QPushButton::clicked, this, &DlgPhys::slotZipCode);

    checkInput();
    connect(ui.le_forename, &QLineEdit::textChanged, this, &DlgPhys::slotModifiedWithCheck);
    connect(ui.le_surname, &QLineEdit::textChanged, this, &DlgPhys::slotModifiedWithCheck);
    connect(ui.cb_gender, &QComboBox::currentIndexChanged, this, &DlgPhys::slotModifiedWithCheck);

    installEventFilter(this);
}


DlgPhys::~DlgPhys()
{
    while (!phys_types.isEmpty()) delete phys_types.takeFirst();
}


//
// initPhys
//
void DlgPhys::initPhys(cPhys *p)
{
    if (p != NULL) {
        curPhys = *p;
        ui.le_surname->setText(curPhys.getName());
        ui.le_forename->setText(curPhys.getForename());
        ui.le_surgery->setText(curPhys.getSurgery());
        ui.le_title->setText(curPhys.getTitle());
        ui.le_street->setText(curPhys.getStreet());
        ui.le_city->setText(curPhys.getCity());
        ui.le_zip->setText(curPhys.getZipcode());
        ui.le_tel->setText(curPhys.getTelephone());
        ui.le_mobile->setText(curPhys.getTelephoneMobile());
        ui.le_fax->setText(curPhys.getFax());
        ui.le_email->setText(curPhys.getEMail());
        ui.cb_gender->setCurrentIndex(curPhys.getGender());
	ui.cbPType->setCurrentIndex(curPhys.getPhysType());
        ui.leBSNR->setText(curPhys.getBSNR());
        ui.leLANR->setText(curPhys.getLANR());

        // TODO: add clinic handling
    } else {
        curPhys.reset();
        checkInput();
    }
    flag_change = false;
}


void DlgPhys::reject()
{
    if (flag_change) {
        MsgBox box;
        if (box.execYesNoBox(QString::fromUtf8("Wollen Sie die Änderungen jetzt speichern?"), MsgBox::BTN_YES) == MsgBox::BTN_YES) {
            if (!savePhys()) return;
            flag_change = false;
            QDialog::accept();
            return;
        } else QDialog::reject();
    }
    QDialog::reject();
}


//
// slotCancelPhys
//
void DlgPhys::slotCancelPhys()
{
    this->reject();
}


bool DlgPhys::savePhys()
{
    GUARD_BOOL(Protect::FUNC_PHY);

    if (ui.le_forename->text().isEmpty()) {
        QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), QString::fromUtf8("Das Feld <Vorname> muss ausgefuellt werden!\n"), QMessageBox::Ok);
        return false;
    }
    if (ui.le_surname->text().isEmpty()) {
        QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), QString::fromUtf8("Das Feld <Nachname> muss ausgefuellt werden!\n"), QMessageBox::Ok);
        return false;
    }
    if (ui.cb_gender->currentIndex() < 0) {
        QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), QString::fromUtf8("Bitte das Geschlecht angeben!\n"), QMessageBox::Ok);
        return false;
    }

    curPhys.setName(ui.le_surname->text());
    curPhys.setForename(ui.le_forename->text());
    curPhys.setSurgery(ui.le_surgery->text());
    curPhys.setTitle(ui.le_title->text());
    curPhys.setGender(ui.cb_gender->currentIndex());
    curPhys.setStreet(ui.le_street->text());
    curPhys.setCity(ui.le_city->text());
    curPhys.setZipcode(ui.le_zip->text());
    curPhys.setTelephone(ui.le_tel->text());
    curPhys.setTelephoneMobile(ui.le_mobile->text());
    curPhys.setFax(ui.le_fax->text());
    curPhys.setEMail(ui.le_email->text());
    curPhys.setPhysType(ui.cbPType->currentIndex());
    curPhys.setBSNR(ui.leBSNR->text());
    curPhys.setLANR(ui.leLANR->text());
    // FIXME
    curPhys.setClinicID(-1);

    bool bRet = true;
    if (curPhys.getID() < 0) {  // store new physician in database
        int id;
        if ( db.storeItem(curPhys, &id) == false) {
            bRet = false;
            QMessageBox::warning(this, tr("OpenPhysio"),
                tr("Fehler beim Anlegen des Arztes aufgetreten!\n"),
                QMessageBox::Ok);
        } else curPhys.setID(id);
    } else { // update properties of existing physician
        if ( db.updateItem(curPhys) == false) {
            bRet = false;
            QMessageBox::warning(this, tr("OpenPhysio"),
                tr("Fehler beim Aktualisieren der Arztdaten aufgetreten!\n"),
                QMessageBox::Ok);
        }
    }

    if (bRet) flag_change = false;
    return bRet;
}


//
// slotSavePhys
//
void DlgPhys::slotSavePhys()
{
    if (savePhys()) accept();
}


void DlgPhys::slotModified()
{
    flag_change = true;
}


void DlgPhys::slotModifiedWithCheck()
{
    flag_change = true;
    checkInput();
}


void DlgPhys::checkInput()
{
    QPalette p_hint(QApplication::palette());
    QPalette p_normal(QApplication::palette());
    p_hint.setColor(QPalette::Base, Qt::yellow);
    if (ui.le_forename->text().isEmpty()) {
        ui.le_forename->setPalette(p_hint);
        ui.le_forename->setToolTip("Vorname fehlt");
    } else {
        ui.le_forename->setPalette(p_normal);
        ui.le_forename->setToolTip("");
    }
    if (ui.le_surname->text().isEmpty()) {
        ui.le_surname->setPalette(p_hint);
        ui.le_surname->setToolTip("Nachname fehlt");
    } else {
        ui.le_surname->setPalette(p_normal);
        ui.le_surname->setToolTip("");
    }
    if (ui.cb_gender->currentIndex() < 0) {
        ui.cb_gender->setPalette(p_hint);
        ui.cb_gender->setToolTip("Geschlechtsangabe fehlt");
    } else {
        ui.cb_gender->setPalette(p_normal);
        ui.cb_gender->setToolTip("");
    }
}


void DlgPhys::slotClinic()
{
    MsgBox::dummy();
}


bool DlgPhys::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *ke = static_cast<QKeyEvent *>(event);
        if (ke->key() == Qt::Key_F1 || ke->key() == Qt::Key_Help) {
            helpWrapper.showHelpForKeyword("openphysio_physicians", true);
            return true;
        }
    }
    return QDialog::eventFilter(obj, event);
}


void DlgPhys::slotZipCode()
{
    QString zip = ui.le_zip->text();
    if (zip.isEmpty() || zip.length() != 5) {
        QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), QString::fromUtf8("Kein gültige Postleitzahl angegeben!\n"), QMessageBox::Ok);
        return;
    }

    // the first time we enter this function we should load the
    // ZIP codes from the database (but only once for the rest of
    // the program runtime)
    if (db.listZipCodes.isEmpty()) {
        db.retrieveItemList(myDB::TABLE_ZIPCODES, reinterpret_cast<QList<cTableItem*>&>(db.listZipCodes));
    }

    QStringList l;
    for (int i = 0; i < db.listZipCodes.count(); i++) {
        if (db.listZipCodes.at(i)->getZip() == zip) {
            l << db.listZipCodes.at(i)->getCity();
        }
    }

    if (l.count() == 0) {
        QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), QString::fromUtf8("Kein Ort für die angegebene Postleitzahl gefunden!\n"), QMessageBox::Ok);
    } else if (l.count() == 1) {
        ui.le_city->setText(l.at(0));
    } else {
        QDialog dlg;
        QGridLayout layout(&dlg);

        QComboBox cb;
        QLabel label1;

        QPushButton pbs("OK");
        QPushButton pbc("Abbrechen");
        pbc.setIcon(QIcon(":/cancel.png"));
        pbs.setIcon(QIcon(":/save.png"));
        label1.setText(QString::fromUtf8("Mehrere Ort gefunden - bitte auswählen"));

        layout.addWidget(&label1, 0, 0, 1, 2);
        layout.addWidget(&cb, 1, 0, 1, 2);
        layout.addWidget(&pbc, 2, 0, 1, 1);
        layout.addWidget(&pbs, 2, 1, 1, 1);

        QObject::connect(&pbc, &QPushButton::clicked, &dlg, &QDialog::reject);
        QObject::connect(&pbs, &QPushButton::clicked, &dlg, &QDialog::accept);
        pbs.setDefault(true);

        cb.addItems(l);
        dlg.setWindowTitle(QString::fromUtf8("Ort auswählen"));
        dlg.setWindowIcon(QIcon(":/favicon.png"));
        dlg.exec();
        if (dlg.result() == QDialog::Accepted) {
            ui.le_city->setText(cb.currentText());
        }
    }
}
