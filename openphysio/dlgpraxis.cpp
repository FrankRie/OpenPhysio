
/****************************************************************************
**
** Copyright (C) 2010 Frank Riechert
**
** This file is part of OpenPhysio.
**
** This file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
****************************************************************************/

#include "dlgpraxis.h"
#include "mydb.h"
#include "protect.h"
#include "msgbox.h"
#include "browseemployees.h"
#include "dlgbrowse.h"

DlgPraxis::DlgPraxis(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);
    flag_change = false;

    connect(ui.pb_save, SIGNAL(clicked()), this, SLOT(slotSavePraxis()));
    connect(ui.pb_cancel, SIGNAL(clicked()), this, SLOT(slotCancelPraxis()));
    connect(ui.pb_eid, SIGNAL(clicked()), this, SLOT(slotReadEID()));

    connect(ui.le_city, SIGNAL(textChanged(QString)), this, SLOT(slotModified()));
    connect(ui.le_title, SIGNAL(textChanged(QString)), this, SLOT(slotModified()));
    connect(ui.le_email, SIGNAL(textChanged(QString)), this, SLOT(slotModified()));
    connect(ui.le_fax, SIGNAL(textChanged(QString)), this, SLOT(slotModified()));
    connect(ui.le_mobile, SIGNAL(textChanged(QString)), this, SLOT(slotModified()));
    connect(ui.le_street, SIGNAL(textChanged(QString)), this, SLOT(slotModified()));
    connect(ui.le_tel, SIGNAL(textChanged(QString)), this, SLOT(slotModified()));
    connect(ui.le_zip, SIGNAL(textChanged(QString)), this, SLOT(slotModified()));
    connect(ui.le_web, SIGNAL(textChanged(QString)), this, SLOT(slotModified()));
    connect(ui.le_eid, SIGNAL(textChanged(QString)), this, SLOT(slotModified()));
    connect(ui.le_ppart, SIGNAL(textChanged(QString)), this, SLOT(slotModified()));

    checkInput();
    connect(ui.le_name1, SIGNAL(textChanged(QString)), this, SLOT(slotModifiedWithCheck()));
    connect(ui.le_name2, SIGNAL(textChanged(QString)), this, SLOT(slotModifiedWithCheck()));
    connect(ui.le_forename, SIGNAL(textChanged(QString)), this, SLOT(slotModifiedWithCheck()));
    connect(ui.le_surname, SIGNAL(textChanged(QString)), this, SLOT(slotModifiedWithCheck()));
    connect(ui.cb_gender, SIGNAL(currentIndexChanged(int)), this, SLOT(slotModifiedWithCheck()));

}


DlgPraxis::~DlgPraxis()
{
    //while (!praxis_types.isEmpty()) delete praxis_types.takeFirst();
}


//
// initPraxis
//
void DlgPraxis::initPraxis(cPraxis *p)
{
    if (p != NULL) {
        curPraxis = *p;

        ui.le_name1->setText(curPraxis.getName1());
        ui.le_name2->setText(curPraxis.getName2());
        ui.le_surname->setText(curPraxis.getSurname());
        ui.le_forename->setText(curPraxis.getForename());
        ui.le_title->setText(curPraxis.getTitle());
        ui.le_street->setText(curPraxis.getStreet());
        ui.le_city->setText(curPraxis.getCity());
        ui.le_zip->setText(curPraxis.getZip());
        ui.le_tel->setText(curPraxis.getTel());
        ui.le_mobile->setText(curPraxis.getMobile());
        ui.le_fax->setText(curPraxis.getFax());
        ui.le_email->setText(curPraxis.getEmail());
        ui.cb_gender->setCurrentIndex(curPraxis.getGender());
        ui.le_web->setText(curPraxis.getWeb());
        ui.le_ppart->setText(QString::number(curPraxis.getPpart()) + " %");

        if (curPraxis.getEid() >= 0) {
            cEmployee emp;
            if (db.retrieveItem(emp, curPraxis.getEid()) == false) ui.le_eid->setText("");
            else ui.le_eid->setText(emp.getShortName());
        } else {
            ui.le_eid->setText("");
        }

    } else {

        curPraxis.reset();
        checkInput();
    }
    flag_change = false;
}


void DlgPraxis::reject()
{
    if (flag_change) {
        MsgBox box;
        if (box.execYesNoBox(QString::fromUtf8("Wollen Sie die Änderungen jetzt speichern?"), MsgBox::BTN_YES) == MsgBox::BTN_YES) {
            if (!savePraxis()) return;
            flag_change = false;
            QDialog::accept();
            return;
        } else QDialog::reject();
    }
    QDialog::reject();
}


//
// slotCancelPraxis
//
void DlgPraxis::slotCancelPraxis()
{
    this->reject();
}


bool DlgPraxis::savePraxis()
{
    GUARD_BOOL(Protect::FUNC_PHY);

    if (ui.le_name1->text().isEmpty()) {
        QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), QString::fromUtf8("Das Feld <name1> muss ausgefuellt werden!\n"), QMessageBox::Ok);
        return false;
    }

    curPraxis.setName1(ui.le_name1->text());
    curPraxis.setName2(ui.le_name2->text());
    curPraxis.setStreet(ui.le_street->text());
    curPraxis.setSurname(ui.le_surname->text());
    curPraxis.setForename(ui.le_forename->text());
    curPraxis.setTitle(ui.le_title->text());
    curPraxis.setGender(ui.cb_gender->currentIndex());
    curPraxis.setCity(ui.le_city->text());
    curPraxis.setZip(ui.le_zip->text());
    curPraxis.setTel(ui.le_tel->text());
    curPraxis.setMobile(ui.le_mobile->text());
    curPraxis.setFax(ui.le_fax->text());
    curPraxis.setEmail(ui.le_email->text());

    bool bRet = true;
    if (curPraxis.getID() < 0) {  // store new praxis in database
        int id;
        if ( db.storeItem(curPraxis, &id) == false) {
            bRet = false;
            QMessageBox::warning(this, tr("OpenPhysio"),
                tr("Fehler beim Anlegen einer Partnerpraxis aufgetreten!\n"),
                QMessageBox::Ok);
        } else curPraxis.setID(id);
    } else { // update properties of existing praxis
        if ( db.updateItem(curPraxis) == false) {
            bRet = false;
            QMessageBox::warning(this, tr("OpenPraxisio"),
                tr("Fehler beim Aktualisieren der Partnerpraxis aufgetreten!\n"),
                QMessageBox::Ok);
        }
    }

    if (bRet) flag_change = false;
    return bRet;
}


//
// slotSavePraxis
//
void DlgPraxis::slotSavePraxis()
{
    savePraxis();
}


void DlgPraxis::slotModified()
{
    flag_change = true;
}


void DlgPraxis::slotModifiedWithCheck()
{
    flag_change = true;
    checkInput();
}

void DlgPraxis::slotReadEID()
{
    BrowseEmployees *browseEmpl = new BrowseEmployees(Browse::MODE_SELECT);
    DlgBrowse browse(browseEmpl);
    if (myConf.usePreload()) browseEmpl->preload();
    if (browse.exec() == QDialog::Accepted) {
        //cEmployee p = browseEmpl->setTabOrder();
        //ui.le_eid->setText(p.getShortName());
        //curPraxis.setEid(p.getID());
        flag_change = true;
    }
    delete browseEmpl;
}




void DlgPraxis::checkInput()
{
    QPalette p_hint(QApplication::palette());
    QPalette p_normal(QApplication::palette());
    p_hint.setColor(QPalette::Base, Qt::yellow);
    if (ui.le_name1->text().isEmpty()) {
        ui.le_name1->setPalette(p_hint);
        ui.le_name1->setToolTip("Praxisname fehlt");
    } else {
        ui.le_name1->setPalette(p_normal);
        ui.le_name1->setToolTip("");
    }
    if (ui.le_name2->text().isEmpty()) {
        ui.le_name2->setPalette(p_hint);
        ui.le_name2->setToolTip("Inhaber fehlt");
    } else {
        ui.le_name2->setPalette(p_normal);
        ui.le_name2->setToolTip("");
    }

}
