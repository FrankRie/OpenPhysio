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

#include "dlgempl.h"
#include "mydb.h"
#include "protect.h"
#include "dlgmanagetherapygroups.h"
#include "msgbox.h"
#include "helperwrapper.h"


DlgEmpl::DlgEmpl(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);
    flag_change = false;

    connect(ui.pb_save, &QPushButton::clicked, this, &DlgEmpl::slotSaveEmpl);
    connect(ui.pb_cancel, &QPushButton::clicked, this, &DlgEmpl::slotCancelEmpl);
	
    db.retrieveItemList(myDB::TABLE_EMPLOYEE_TYPES, employee_types);
    for (int i = 0; i < employee_types.count(); i++)
	ui.cbType->addItem( ((cEmployeeType*)(employee_types.at(i)))->getDescription());

    for (int i = 0; i < db.listEmployeeGroups.count(); i++)
        ui.cbGroup->addItem(db.listEmployeeGroups.at(i)->getDescription());

    for (int i = 0; i < db.listTherapyGroups.count(); i++)
        ui.cbTGroup->addItem(db.listTherapyGroups.at(i)->getName());
    ui.cbTGroup->setCurrentIndex(-1);

    ui.de_birthday->calendarWidget()->setFirstDayOfWeek(Qt::Monday);
    ui.de_birthday->calendarWidget()->setGridVisible(true);

    ui.cbGender->setCurrentIndex(-1);

    ui.tableWidget->hideColumn(0); // hide ID column
    int co = 0;
    for (int i = 0; i < db.listQualifications.count(); i++) {
        int p = db.listQualifications.at(i)->getProfession();
        if (p >= 0 && p < db.listProfessions.count()) {
            ui.tableWidget->insertRow(co);
            QTableWidgetItem* item_id = new QTableWidgetItem();
            item_id->setText(QString::number(db.listQualifications.at(i)->getID()));
            QTableWidgetItem* item_group = new QTableWidgetItem();
            item_group->setText(db.listProfessions.at(p)->getName());
            QTableWidgetItem* item_quali = new QTableWidgetItem();
            item_quali->setText(db.listQualifications.at(i)->getName());
            item_quali->setCheckState(Qt::Unchecked);
            ui.tableWidget->setItem(co, 0, item_id);
            ui.tableWidget->setItem(co, 1, item_group);
            ui.tableWidget->setItem(co, 2, item_quali);
            co++;
        }
    }
    ui.tableWidget->resizeColumnsToContents();
    connect(ui.tableWidget,  &QTableWidget::cellChanged, this, &DlgEmpl::slotModified);

    slotGroup(0);
    connect(ui.cbGroup, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &DlgEmpl::slotGroup);
    connect(ui.pbManage, &QPushButton::clicked, this, &DlgEmpl::slotManageTherapyGroup);

    connect(ui.le_city, &QLineEdit::textChanged, this, &DlgEmpl::slotModified);
    connect(ui.le_email, &QLineEdit::textChanged, this, &DlgEmpl::slotModified);
    connect(ui.le_fax, &QLineEdit::textChanged, this, &DlgEmpl::slotModified);
    connect(ui.le_mobile, &QLineEdit::textChanged, this, &DlgEmpl::slotModified);
    connect(ui.le_street, &QLineEdit::textChanged, this, &DlgEmpl::slotModified);
    connect(ui.le_tel, &QLineEdit::textChanged, this, &DlgEmpl::slotModified);
    connect(ui.le_zip, &QLineEdit::textChanged, this, &DlgEmpl::slotModified);
    connect(ui.le_persnr, &QLineEdit::textChanged, this, &DlgEmpl::slotModified);
    connect(ui.cbType, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &DlgEmpl::slotModified);
    connect(ui.cbTGroup, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &DlgEmpl::slotModified);
    connect(ui.cbGender, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &DlgEmpl::slotModified);
    connect(ui.cbState, &QCheckBox::toggled, this, &DlgEmpl::slotModified);

    connect(ui.le_forename, &QLineEdit::textChanged, this, &DlgEmpl::slotModifiedWithCheck);
    connect(ui.le_surname, &QLineEdit::textChanged, this, &DlgEmpl::slotModifiedWithCheck);
    connect(ui.le_shortname, &QLineEdit::textChanged, this, &DlgEmpl::slotModifiedWithCheck);

    connect(ui.pbZipCode, &QPushButton::clicked, this, &DlgEmpl::slotZipCode);

    installEventFilter(this);
}


DlgEmpl::~DlgEmpl()
{
    for (int i = 0; i < employee_types.count(); i++) delete employee_types.at(i);
    employee_types.clear();
}


void DlgEmpl::initEmployee(cEmployee *e)
{
    if (e != NULL) {
        curEmpl = *e;
        ui.le_surname->setText(curEmpl.getName());
        ui.le_forename->setText(curEmpl.getForename());
        ui.le_street->setText(curEmpl.getStreet());
        ui.le_city->setText(curEmpl.getCity());
        ui.le_zip->setText(curEmpl.getZipcode());
        ui.le_tel->setText(curEmpl.getTelephone());
        ui.le_mobile->setText(curEmpl.getTelephoneMobile());
        ui.le_fax->setText(curEmpl.getFax());
        ui.le_email->setText(curEmpl.getEMail());
        ui.de_birthday->setDate(curEmpl.getBirthday());
        ui.le_shortname->setText(curEmpl.getShortName());
        ui.le_persnr->setText(curEmpl.getPersonellNumber());
        ui.cbType->setCurrentIndex(curEmpl.getType());
        ui.cbGender->setCurrentIndex(curEmpl.getGender());
        if (curEmpl.getState() == cEmployee::STATE_ACTIVE) ui.cbState->setChecked(true);
        else ui.cbState->setChecked(false);
        for (int i = 0; i < db.listEmployeeGroups.count(); i++) {
            if (db.listEmployeeGroups.at(i)->getID() == curEmpl.getGroupID()) {
		ui.cbGroup->setCurrentIndex(i);
		break;
	    }
	}
        for (int i = 0; i < db.listTherapyGroups.count(); i++) {
            if (db.listTherapyGroups.at(i)->getID() == curEmpl.getTherapyGroupID() ) {
                ui.cbTGroup->setCurrentIndex(i);
                break;
            }
        }
        slotGroup(ui.cbGroup->currentIndex());

        disconnect(ui.tableWidget,  &QTableWidget::cellChanged, this, &DlgEmpl::slotModified);
        for (int i = 0; i < curEmpl.getQualifications().count(); i++) {
            for (int x = 0; x < ui.tableWidget->rowCount(); x++) {
                QTableWidgetItem *item = ui.tableWidget->item(x, 0);
                if (curEmpl.getQualifications().at(i) == item->text().toInt()) {
                    QTableWidgetItem *check = ui.tableWidget->item(x, 2);
                    check->setCheckState(Qt::Checked);
                    break;
                }
            }
        }
        connect(ui.tableWidget,  &QTableWidget::cellChanged, this, &DlgEmpl::slotModified);
    } else {
        curEmpl.reset();
        ui.cbTGroup->setCurrentIndex(-1);
        checkInput();
    }

    flag_change = false;
}


void DlgEmpl::reject()
{
    if (flag_change) {
        MsgBox box;
        if (box.execYesNoBox(QString::fromUtf8("Wollen Sie die Änderungen jetzt speichern?"), MsgBox::BTN_YES) == MsgBox::BTN_YES) {
            if (!saveEmpl()) return;
            flag_change = false;
            QDialog::accept();
        } else QDialog::reject();
    }
    QDialog::reject();
}


void DlgEmpl::slotCancelEmpl()
{
    this->reject();
}


bool DlgEmpl::saveEmpl()
{
    GUARD_BOOL(Protect::FUNC_EMP);

    bool bRet = true;

    if (ui.le_forename->text().length() <= 0) {
        QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), QString::fromUtf8("Das Feld <Vorname> muss ausgefüllt werden!"), QMessageBox::Ok);
        return false;
    }
    if (ui.le_surname->text().length() <= 0) {
        QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), QString::fromUtf8("Das Feld <Nachname> muss ausgefüllt werden!"), QMessageBox::Ok);
        return false;
    }
    if (ui.le_shortname->text().length() <= 0) {
        QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), QString::fromUtf8("Das Feld <Kürzel> muss ausgefüllt werden!"), QMessageBox::Ok);
        return false;
    }

    curEmpl.setName(ui.le_surname->text());
    curEmpl.setForename(ui.le_forename->text());
    curEmpl.setStreet(ui.le_street->text());
    curEmpl.setCity(ui.le_city->text());
    curEmpl.setZipcode(ui.le_zip->text());
    curEmpl.setTelephone(ui.le_tel->text());
    curEmpl.setTelephoneMobile(ui.le_mobile->text());
    curEmpl.setFax(ui.le_fax->text());
    curEmpl.setEMail(ui.le_email->text());
    curEmpl.setBirthday(ui.de_birthday->date());
    curEmpl.setGender(ui.cbGender->currentIndex());
    curEmpl.setShortName(ui.le_shortname->text());
    curEmpl.setType(ui.cbType->currentIndex());
    curEmpl.setPersonellNumber(ui.le_persnr->text());
    if (ui.cbState->isChecked()) curEmpl.setState(cEmployee::STATE_ACTIVE);
    else curEmpl.setState(cEmployee::STATE_INACTIVE);
    if ( (ui.cbGroup->currentIndex() >= 0) && (ui.cbGroup->currentIndex() < db.listEmployeeGroups.count()) )
        curEmpl.setGroupID(db.listEmployeeGroups.at(ui.cbGroup->currentIndex())->getID());
    else curEmpl.setGroupID(-1);
    if ( (ui.cbTGroup->currentIndex() >= 0) && (ui.cbTGroup->currentIndex() < db.listTherapyGroups.count()) )
        curEmpl.setTherapyGroupID(db.listTherapyGroups.at(ui.cbTGroup->currentIndex())->getID());
    else curEmpl.setTherapyGroupID(-1);

    QList<int> l;
    for (int i = 0; i < ui.tableWidget->rowCount(); i++) {
        QTableWidgetItem *check = ui.tableWidget->item(i, 2);
        if (check->checkState() == Qt::Checked) {
            QTableWidgetItem *item = ui.tableWidget->item(i, 0);
            l.append(item->text().toInt());
        }
    }
    curEmpl.setQualifications(l);

    if (curEmpl.getID() < 0) {  // store new employee in database
        int id;
        if ( db.storeItem(curEmpl, &id) == false) {
            bRet = false;
            QMessageBox::warning(this, tr("OpenPhysio"), QString::fromUtf8("Fehler beim Anlegen des Mitarbeiters aufgetreten!\n"), QMessageBox::Ok);
        } else curEmpl.setID(id);
    } else  { // update properties of existing employee
        if ( db.updateItem(curEmpl) == false) {
            bRet = false;
            QMessageBox::warning(this, tr("OpenPhysio"), QString::fromUtf8("Fehler beim Aktualisieren der Mitarbeiterdaten aufgetreten!\n"), QMessageBox::Ok);
        }
    }

    if (bRet) flag_change = false;
    return bRet;
}


void DlgEmpl::slotSaveEmpl()
{
    if (saveEmpl()) accept();
}


void DlgEmpl::slotGroup(int i)
{
    if ( (i < 0) || ( i >= db.listEmployeeGroups.count()) ) return;
    if (db.listEmployeeGroups.at(i)->getID() == cEmployeeGroup::GROUP_THERAPY) {
        //ui.cbTGroup->setCurrentIndex(0);
        ui.cbTGroup->show();
        ui.labelTGroup->show();
        ui.pbManage->show();
    } else {
        ui.cbTGroup->hide();
        ui.labelTGroup->hide();
        ui.pbManage->hide();
    }
    flag_change = true;
}


void DlgEmpl::slotManageTherapyGroup()
{
    DlgManageTherapyGroups dlg;
    if ( dlg.exec() == QDialog::Accepted) {
        ui.cbTGroup->clear();
        for (int i = 0; i < db.listTherapyGroups.count(); i++)
            ui.cbTGroup->addItem(db.listTherapyGroups.at(i)->getName());
        for (int i = 0; i < db.listTherapyGroups.count(); i++) {
            if (db.listTherapyGroups.at(i)->getID() == curEmpl.getTherapyGroupID() ) {
                ui.cbTGroup->setCurrentIndex(i);
                break;
            }
        }
    }
}


void DlgEmpl::slotModified()
{
    flag_change = true;
}


void DlgEmpl::slotModifiedWithCheck()
{
    flag_change = true;
    checkInput();
}


void DlgEmpl::checkInput()
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
    if (ui.le_shortname->text().isEmpty()) {
        ui.le_shortname->setPalette(p_hint);
        ui.le_shortname->setToolTip(QString::fromUtf8("Kürzel fehlt"));
    } else {
        ui.le_shortname->setPalette(p_normal);
        ui.le_shortname->setToolTip("");
    }
}


bool DlgEmpl::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *ke = static_cast<QKeyEvent *>(event);
        if (ke->key() == Qt::Key_F1 || ke->key() == Qt::Key_Help) {
            helpWrapper.showHelpForKeyword("openphysio_employees", true);
            return true;
        }
    }
    return QDialog::eventFilter(obj, event);
}


void DlgEmpl::slotZipCode()
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
        QObject::connect(&pbs, &QPushButton::clicked, &dlg, &DlgEmpl::accept);
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
