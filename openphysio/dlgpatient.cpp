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

#include "dlgpatient.h"
#include "kvkread.h"
#include "protect.h"
#include "browsereceipes.h"
#include "browseinvoices.h"
#include "browsephysicians.h"
#include "msgbox.h"
#include "dlgbrowse.h"
#include "dlgreceipe.h"
#include "employeetools.h"
#include "helperwrapper.h"
#include "dlgtextblock.h"
#include "gendoc.h"


DlgPatient::DlgPatient(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);

#ifdef Q_OS_MACX
    ui.tabOther->layout()->setSpacing(-1);
#endif

    QDate datum = QDate::currentDate();
    int16_t zielJahr = datum.year() + (datum.month() >= 8 ? 1 : 0);
    ui.label_14->setText("Vertrag / " + QString::number(zielJahr));

    connect(ui.pb_chip, &QPushButton::clicked, this, &DlgPatient::slotReadChip);
    connect(ui.pb_save, &QPushButton::clicked, this, &DlgPatient::slotSavePatient);
    connect(ui.pb_cancel, &QPushButton::clicked, this, &DlgPatient::reject);
    connect(ui.pb_insurance, &QPushButton::clicked, this, &DlgPatient::slotSelectInsurance);
    connect(ui.pbPhys, &QPushButton::clicked, this, &DlgPatient::slotSelectPhysician);
    connect(ui.pbDelPhys, &QPushButton::clicked, this, &DlgPatient::slotDelPhysician);
    connect(ui.pbDelInsurance, &QPushButton::clicked, this, &DlgPatient::slotDelInsurance);
    connect(ui.pbZipCpde, &QPushButton::clicked, this, &DlgPatient::slotZipCode);
    connect(ui.pbAddText, &QPushButton::clicked, this, &DlgPatient::slotTextModule);
    connect(ui.pbPrint, &QPushButton::clicked, this, &DlgPatient::slotPrint);
    connect(ui.pbPrintTherapy, &QPushButton::clicked, this, &DlgPatient::slotPrintTherapyDates);
    connect(ui.pbReport, &QPushButton::clicked, this, &DlgPatient::slotReport);

    therapy.stage(ui.tabTherapy, cDateType::DT_PATIENT);
    connect(ui.pbDelDate, &QPushButton::clicked, &therapy, &Therapy::slotTherapyDel);
    connect(ui.pbDelAllDates, &QPushButton::clicked, &therapy, &Therapy::slotTherapyDelAll);
    curPatient.reset();
    flag_change = false;

    browseDocs = new BrowseDocs(this, BrowseDocs::MASK_FULL & (~BrowseDocs::MASK_TEMPLATES), 0);
    browseDocs->setMode(BrowseDocs::MODE_PATIENT);
    QGridLayout *qgl = (QGridLayout *) ui.tabDocs->layout();
    if (qgl != NULL) qgl->addWidget(browseDocs, 0, 0, 1, 1);

    for (int i = 0; i < db.listInsuranceTypes.count(); i++)
        ui.cb_instype->addItem(db.listInsuranceTypes.at(i)->getDescription());

    EmplTool.getActiveEmployees(list_empl, cEmployeeGroup::GROUP_THERAPY);
    EmplTool.fillComboBox(ui.cbEmployee, cEmployeeGroup::GROUP_THERAPY);

    ui.labelAge->setText("");

    connect(ui.leStatus, &QLineEdit::textChanged, this, &DlgPatient::slotModified);
    connect(ui.le_addname, &QLineEdit::textChanged, this, &DlgPatient::slotModified);
    connect(ui.le_city, &QLineEdit::textChanged, this, &DlgPatient::slotModified);
    connect(ui.le_email, &QLineEdit::textChanged, this, &DlgPatient::slotModified);
    connect(ui.le_fax, &QLineEdit::textChanged, this, &DlgPatient::slotModified);
    connect(ui.le_insurance, &QLineEdit::textChanged, this, &DlgPatient::slotModified);
    connect(ui.le_knr, &QLineEdit::textChanged, this, &DlgPatient::slotModified);
    connect(ui.le_mobile, &QLineEdit::textChanged, this, &DlgPatient::slotModified);
    connect(ui.le_street, &QLineEdit::textChanged, this, &DlgPatient::slotModified);
    connect(ui.le_tel, &QLineEdit::textChanged, this, &DlgPatient::slotModified);
    connect(ui.le_tel_job, &QLineEdit::textChanged, this, &DlgPatient::slotModified);
    connect(ui.le_title, &QLineEdit::textChanged, this, &DlgPatient::slotModified);
    connect(ui.le_vnr, &QLineEdit::textChanged, this, &DlgPatient::slotModified);
    connect(ui.le_zip, &QLineEdit::textChanged, this, &DlgPatient::slotModified);
    connect(ui.le_notices, &QTextEdit::textChanged, this, &DlgPatient::slotModified);
    connect(ui.le_phys, &QLineEdit::textChanged, this, &DlgPatient::slotModified);
    connect(ui.cb_instype, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &DlgPatient::slotModifiedWithCheck);
    connect(ui.cbEmployee, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &DlgPatient::slotModified);
    connect(ui.spDistance, &QSpinBox::valueChanged, this, &DlgPatient::slotModified);
    connect(ui.cbCareFacility, &QCheckBox::toggled, this, &DlgPatient::slotModified);
    connect(ui.cbState, &QCheckBox::toggled, this, &DlgPatient::slotModified);
    connect(ui.le_insurance, &QLineEdit::textChanged, this, &DlgPatient::slotModifiedWithCheck);
    connect(ui.le_forename, &QLineEdit::textChanged, this, &DlgPatient::slotModifiedWithCheck);
    connect(ui.le_surname, &QLineEdit::textChanged, this, &DlgPatient::slotModifiedWithCheck);
    connect(ui.cb_gender, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &DlgPatient::slotModifiedWithCheck);

    de_birthday = new DateEdit();
    de_valid = new DateEdit();
    de_freed_from = new DateEdit();
    de_freed_until = new DateEdit();
    de_contract = new DateEdit();
    de_plan = new DateEdit();
    QGridLayout *grid = qobject_cast<QGridLayout*>(ui.tabPatient->layout());
    if (grid) {
        grid->addWidget(de_birthday, 4, 1);
        grid->addWidget(de_valid, 17, 2);
        grid->addWidget(de_contract, 18,1);
        grid->addWidget(de_plan, 18,2);
    }
    grid = qobject_cast<QGridLayout*>(ui.widgetAddpay->layout());
    if (grid) {
        grid->addWidget(de_freed_from, 0, 2);
        grid->addWidget(de_freed_until, 0, 4);
    }
    connect(de_birthday, &DateEdit::dateChanged, this, &DlgPatient::slotAge);
    connect(de_valid, &DateEdit::dateChanged, this, &DlgPatient::slotModifiedWithCheck);
    connect(de_freed_from, &DateEdit::dateChanged, this, &DlgPatient::slotModified);
    connect(de_freed_until, &DateEdit::dateChanged, this, &DlgPatient::slotModified);
    connect(de_contract, &DateEdit::dateChanged, this, &DlgPatient::slotModifiedWithCheck);
    connect(de_plan, &DateEdit::dateChanged, this, &DlgPatient::slotModifiedWithCheck);
    connect(ui.cbAddpayFreed, &QCheckBox::toggled, this, &DlgPatient::slotModifiedWithCheck);

    ui.tabPatient->setTabOrder(ui.cb_gender, de_birthday);
    ui.tabPatient->setTabOrder(ui.le_knr, de_valid);

    ui.labInsuranceName->setText("");
    ui.le_insurance->setValidator(new QIntValidator(0, 999999999, this));

    installEventFilter(this);
}


DlgPatient::~DlgPatient()
{
    if (browseDocs != NULL) delete browseDocs;
    while (!list_empl.isEmpty()) delete list_empl.takeFirst();
}


//
// initPatient
//
void DlgPatient::initPatient(cPatient *p)
{
    curPatient = *p;
    if (p->getID() > 0) {
        QString title = "Patient bearbeiten: ";
        title += curPatient.getForename();
        title += " ";
        title += curPatient.getName();
        setWindowTitle(title);
	browseDocs->setFilterID(p->getID());
    } else {
        setWindowTitle(tr("Patient anlegen"));
	browseDocs->setFilterID(0);
    }
    therapy.loadP(p->getID());
    browseDocs->Reload();

    ui.le_surname->setText(curPatient.getName());
    ui.le_forename->setText(curPatient.getForename());
    ui.le_addname->setText(curPatient.getAddName());
    ui.le_street->setText(curPatient.getStreet());
    ui.le_city->setText(curPatient.getCity());
    ui.le_zip->setText(curPatient.getZipcode());
    ui.le_tel->setText(curPatient.getTelephone());
    ui.le_tel_job->setText(curPatient.getTelephoneJob());
    ui.le_mobile->setText(curPatient.getTelephoneMobile());
    ui.le_fax->setText(curPatient.getFax());
    ui.le_email->setText(curPatient.getEMail());
    ui.le_vnr->setText(curPatient.getVNR());
    ui.le_knr->setText(curPatient.getKVK());
    ui.le_title->setText(curPatient.getTitle());
    ui.le_notices->setText(curPatient.getNotices());
    ui.leStatus->setText(curPatient.getStatus());
    ui.cbEmployee->setCurrentIndex(EmplTool.getIndexOfEmployee(curPatient.getDefaultEmployeeID(), list_empl));
    ui.spDistance->setValue(curPatient.getDistance());
    ui.cbCareFacility->setChecked((bool)curPatient.getCareFacility());
    if (curPatient.getState() == cPatient::STATE_ACTIVE) ui.cbState->setChecked(true);
    else ui.cbState->setChecked(false);

    ui.cb_instype->setCurrentIndex(curPatient.getInsType());
    if (!curPatient.getInsurance().isEmpty()) {
        cInsurance ins;
        if (db.getInsurance(curPatient.getInsurance(), ins)) {
            //ui.le_insurance->setText(ins.getShortName() + " [" + ins.getIK() + "]" );
            ui.le_insurance->setText(ins.getIK());
            ui.labInsuranceName->setText(ins.getShortName());
        } else {
            ui.le_insurance->setText(curPatient.getInsurance());
        }
        ui.le_insurance->setCursorPosition(0);
    }

    de_birthday->setDate(curPatient.getBirthday());
    slotAge(curPatient.getBirthday());
    de_valid->setDate(curPatient.getValidKVK());
    de_contract->setDate(curPatient.getContract());
    de_plan->setDate(curPatient.getPlan());

    ui.cb_gender->setCurrentIndex(curPatient.getGender());
    ui.cb_instype->setCurrentIndex(curPatient.getInsType());

    if (curPatient.getDefaultPhysicianID() >= 0) {
        cPhys phy;
        if (db.retrieveItem(phy, curPatient.getDefaultPhysicianID()) == false) ui.le_phys->setText("");
        else ui.le_phys->setText(phy.getTitle() + " " + phy.getForename() + " " + phy.getName());
    } else {
        ui.le_phys->setText("");
    }

    ui.cbAddpayFreed->setChecked((bool)curPatient.getAddpayFreed());
    if (ui.cbAddpayFreed->isChecked()) {
        de_freed_from->setDate(curPatient.getFreedFrom());
        de_freed_until->setDate(curPatient.getFreedUntil());
    } else {
        de_freed_from->clear();
        de_freed_until->clear();
    }
    flag_change = false;
}


void DlgPatient::slotSelectInsurance()
{
    BrowseInsurances *bi = new BrowseInsurances(Browse::MODE_SELECT);
    DlgBrowse browse(bi);
    if (browse.exec() == QDialog::Accepted) {
        cInsurance ins = bi->getSelInsurance();
        curPatient.setInsurance(ins.getIK());
        //ui.le_insurance->setText(ins.getShortName() + " [" + ins.getIK() + "]" );
        ui.le_insurance->setText(ins.getIK());
        ui.le_insurance->setCursorPosition(0);
        ui.labInsuranceName->setText(ins.getShortName());
    }
    delete bi;
}


void DlgPatient::reject()
{
    if (flag_change) {
        MsgBox box;
        if (box.execYesNoBox(QString::fromUtf8("Patientendaten wurden geändert. Jetzt speichern?"), MsgBox::BTN_YES) == MsgBox::BTN_YES) {
            if (!savePatient()) return;
            flag_change = false;
            QDialog::accept();
            return;
        } else QDialog::reject();
    }
    QDialog::reject();
}


bool DlgPatient::savePatient()
{
    GUARD_BOOL(Protect::FUNC_PAT);

    if (ui.le_forename->text().isEmpty()) {
        QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), QString::fromUtf8("Das Feld <Vorname> muss ausgefüllt werden!\n"), QMessageBox::Ok);
        return false;
    }
    if (ui.le_surname->text().isEmpty()) {
        QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), QString::fromUtf8("Das Feld <Nachname> muss ausgefüllt werden!\n"), QMessageBox::Ok);
        return false;
    }
    if (ui.cb_gender->currentIndex() < 0) {
        QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), QString::fromUtf8("Bitte das Geschlecht angeben!\n"), QMessageBox::Ok);
        return false;
    }
    if (!ui.le_insurance->text().isEmpty() && !lookupInsurance()) {
        QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), QString::fromUtf8("Bitte eine gültige Kasse angeben!\n"), QMessageBox::Ok);
        return false;
    }
    curPatient.setInsurance(ui.le_insurance->text());
    curPatient.setName(ui.le_surname->text());
    curPatient.setForename(ui.le_forename->text());
    curPatient.setAddName(ui.le_addname->text());
    curPatient.setTitle(ui.le_title->text());
    curPatient.setBirthday(de_birthday->date());
    curPatient.setGender(ui.cb_gender->currentIndex());
    curPatient.setStreet(ui.le_street->text());
    curPatient.setCity(ui.le_city->text());
    curPatient.setZipcode(ui.le_zip->text());
    curPatient.setTelephone(ui.le_tel->text());
    curPatient.setTelephoneJob(ui.le_tel_job->text());
    curPatient.setTelephoneMobile(ui.le_mobile->text());
    curPatient.setFax(ui.le_fax->text());
    curPatient.setEMail(ui.le_email->text());
    curPatient.setVNR(ui.le_vnr->text());
    curPatient.setKVK(ui.le_knr->text());
    curPatient.setValidKVK(de_valid->date());
    curPatient.setNotices(ui.le_notices->toPlainText());
    curPatient.setInsType(ui.cb_instype->currentIndex());
    curPatient.setStatus(ui.leStatus->text());
    curPatient.setDistance(ui.spDistance->value());
    curPatient.setCareFacility(ui.cbCareFacility->isChecked());
    if (ui.cbState->isChecked()) curPatient.setState(cPatient::STATE_ACTIVE);
    else curPatient.setState(cPatient::STATE_INACTIVE);
    if (ui.cbEmployee->currentIndex() >= 0) {
        curPatient.setDefaultEmployeeID(list_empl.at(ui.cbEmployee->currentIndex())->getID());
    } else curPatient.setDefaultEmployeeID(-1);
    curPatient.setAddpayFreed(ui.cbAddpayFreed->isChecked());
    curPatient.setFreedFrom(de_freed_from->date());
    curPatient.setFreedUntil(de_freed_until->date());
    curPatient.setContract(de_contract->date());
    curPatient.setPlan(de_plan->date());

    bool bRet = true;
    if (curPatient.getID() < 0) {  // store new patient in database
        int id;
        if (db.existPatient(curPatient, &id)) {
            QMessageBox::warning(this, tr("OpenPhysio"),
                QString::fromUtf8("Ein Patient mit gleichem Namen und gleichem Geburtsdatum existiert bereits!\n"),
                QMessageBox::Ok);
            bRet = false;
        } else {
            int id;
            if (db.storeItem(curPatient, &id) == false) {
                QMessageBox::warning(this, tr("OpenPhysio"),
                    QString::fromUtf8("Fehler beim Anlegen des Patienten aufgetreten!\n"),
                    QMessageBox::Ok);
                bRet = false;
            } else {
                curPatient.setID(id);
                browseDocs->setFilterID(id);
                browseDocs->Reload();
            }
        }
    }  else { // update properties of existing patient
        if (db.updateItem(curPatient) == false) {
            QMessageBox::warning(this, tr("OpenPhysio"),
                QString::fromUtf8("Fehler beim Aktualisieren der Patientendaten aufgetreten!\n"),
                QMessageBox::Ok);
            bRet = false;
        }
    }

    if (bRet) flag_change = false;
    return bRet;
}


//
// slotSavePatient
//
void DlgPatient::slotSavePatient()
{
    GUARD(Protect::FUNC_PAT);
    if (savePatient()) accept();
}


bool DlgPatient::readChip()
{
    cPatient p;
    KvkRead kread;

    if ( kread.read_kvk(&p) == 0 ) {
	int id;
	if (db.existPatient(p, &id)) {
	    MsgBox box;
            if (box.execYesNoBox(QString::fromUtf8("Ein Patient gleichen Namens und Geburtsdatum existiert bereits!\nNur die Daten der KVK aktualisieren und andere Daten (z.B. Telefon, E-Mail) erhalten?"), MsgBox::BTN_YES) == MsgBox::BTN_YES) {
		// merge settings (empty fields of current patient will be set to values from old patient entry)
		cPatient old;
		db.retrieveItem(old, id);
		if (p.getGender() < 0) p.setGender(old.getGender());
		if (p.getEMail().isEmpty()) p.setEMail(old.getEMail());
		if (p.getTelephone().isEmpty()) p.setTelephone(old.getTelephone());
		if (p.getTelephoneJob().isEmpty()) p.setTelephoneJob(old.getTelephoneJob());
		if (p.getTelephoneMobile().isEmpty()) p.setTelephoneMobile(old.getTelephoneMobile());
		if (p.getFax().isEmpty()) p.setFax(old.getFax());
		if (p.getEMail().isEmpty()) p.setEMail(old.getEMail());
		p.setNotices(old.getNotices());
		p.setInsType(old.getInsType());
	    }
	    p.setID(id);
	}
	initPatient(&p);
        flag_change = true;
	return true;
    }
    return false;
}


//
// slotReadChip
//
void DlgPatient::slotReadChip()
{
    GUARD(Protect::FUNC_READCHIP);
    if (!readChip()) QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), QString::fromUtf8("Chipkarte konnte nicht gelesen werden!\n"), QMessageBox::Ok);
}


void DlgPatient::slotModified()
{
    flag_change = true;
}


void DlgPatient::slotModifiedWithCheck()
{
    flag_change = true;
    checkInput();
}


void DlgPatient::checkInput()
{
    QPalette p_hint(QApplication::palette());
    QPalette p_normal(QApplication::palette());
    p_hint.setColor(QPalette::Base, Qt::yellow);
    // FIXME: the colouring code for de_valid currently does not have an effect
    if (de_valid->date() < QDate::currentDate()) {
        de_valid->setPalette(p_hint);
        de_valid->setToolTip(QString::fromUtf8("Versichertenkarte ist abgelaufen"));
    } else {
        de_valid->setPalette(p_normal);
        de_valid->setToolTip("");
    }
    if (ui.cb_gender->currentIndex() < 0) {
        ui.cb_gender->setPalette(p_hint);
        ui.cb_gender->setToolTip(QString::fromUtf8("Geschlechtsangabe fehlt"));
    } else {
        ui.cb_gender->setPalette(p_normal);
        ui.cb_gender->setToolTip("");
    }
    if (ui.le_forename->text().isEmpty()) {
        ui.le_forename->setPalette(p_hint);
        ui.le_forename->setToolTip(QString::fromUtf8("Vorname fehlt"));
    } else {
        ui.le_forename->setPalette(p_normal);
        ui.le_forename->setToolTip("");
    }
    if (ui.le_surname->text().isEmpty()) {
        ui.le_surname->setPalette(p_hint);
        ui.le_surname->setToolTip(QString::fromUtf8("Nachname fehlt"));
    } else {
        ui.le_surname->setPalette(p_normal);
        ui.le_surname->setToolTip("");
    }

    if (ui.le_insurance->text().length() > 0) {
        if (lookupInsurance()) {
            ui.le_insurance->setPalette(p_normal);
            ui.le_insurance->setToolTip("");
        } else {
            ui.le_insurance->setPalette(p_hint);
            ui.le_insurance->setToolTip(QString::fromUtf8("Unbekannte Kasse oder ungültige IK"));
        }
    } else {
        ui.le_insurance->setPalette(p_normal);
        ui.le_insurance->setToolTip("");
    }

    if (ui.cbAddpayFreed->isChecked()) {
        if (de_freed_from->date().isValid()) {
            de_freed_from->setPalette(p_normal);
            de_freed_from->setToolTip("");
        } else {
            de_freed_from->setPalette(p_hint);
            de_freed_from->setToolTip(QString::fromUtf8("Ungültiges Datum"));
        }
        if (de_freed_until->date().isValid()) {
            de_freed_until->setPalette(p_normal);
            de_freed_until->setToolTip("");
        } else {
            de_freed_until->setPalette(p_hint);
            de_freed_until->setToolTip(QString::fromUtf8("Ungültiges Datum"));
        }
    } else {
        de_freed_from->setPalette(p_normal);
        de_freed_from->setToolTip("");
        de_freed_until->setPalette(p_normal);
        de_freed_until->setToolTip("");
    }
}


bool DlgPatient::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *ke = static_cast<QKeyEvent *>(event);
        if (ke->key() == Qt::Key_F1 || ke->key() == Qt::Key_Help) {
            helpWrapper.showHelpForKeyword("openphysio_patients", true);
            return true;
        }
    }
    return QDialog::eventFilter(obj, event);
}


void DlgPatient::slotAge(QDate d)
{
    if (!d.isValid()) {
        ui.labelAge->setText("");
        return;
    }
    QDate xd = QDate::currentDate();
    if (d > xd) ui.labelAge->setText("");
    else ui.labelAge->setText(dateTools.DateDiff(d, xd));
}


void DlgPatient::slotSelectPhysician()
{
    BrowsePhysicians *browsePhys = new BrowsePhysicians(Browse::MODE_SELECT);
    DlgBrowse browse(browsePhys);
    if (myConf.usePreload()) browsePhys->preload();
    if (browse.exec() == QDialog::Accepted) {
        cPhys p = browsePhys->getSelPhysician();
        ui.le_phys->setText(p.getTitle() + " " + p.getForename() + " " + p.getName());
        curPatient.setDefaultPhysicianID(p.getID());
        flag_change = true;
    }
    delete browsePhys;
}


void DlgPatient::slotDelPhysician()
{
    curPatient.setDefaultPhysicianID(-1);
    ui.le_phys->clear();
}


void DlgPatient::slotDelInsurance()
{
    curPatient.setInsurance("");
    ui.le_insurance->clear();
    ui.labInsuranceName->clear();
}


void DlgPatient::slotZipCode()
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
        QObject::connect(&pbs, &QPushButton::clicked, &dlg, &DlgPatient::accept);
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

void DlgPatient::slotReport()
{
    GUARD(Protect::FUNC_REP);

 /*   if (curPatient.getID() < 0) {
        MsgBox box;
        if (box.execYesNoBox(QString::fromUtf8("Das Rezept muss erst gespeichert werden, bevor ein Bericht erstellt werden kann!\n\nJetzt speichern?"), MsgBox::BTN_YES) == MsgBox::BTN_NO) {
            return;
        } else {
            if (!saveReceipe()) return;
        }
    }
*/
    cReport rep;
    rep.setPatientID(curPatient.getID());
//    rep.setReceipeID(curPatient.getID());
//    rep.setPhysID(curPatient.getPhysID());
    gendoc.genReport(rep);
}


void DlgPatient::slotTextModule()
{
    DlgTextBlock *dlg = new DlgTextBlock();
    dlg->init(cTextModule::TM_PATIENTS);
    if (dlg->exec() == QDialog::Accepted) {
        ui.le_notices->append(dlg->getTextModule());
    }
    delete dlg;
}


void DlgPatient::slotPrint()
{
    GenDoc gendoc;
    gendoc.genPatientData(curPatient);
}

void DlgPatient::slotPrintTherapyDates()
{
    GenDoc gendoc;
    cReceipe curReceipe;

    gendoc.genPatientSchedule(curReceipe, curPatient, therapy.getListDates());
}

void DlgPatient::on_tabDates_destroyed()
{
}

bool DlgPatient::lookupInsurance()
{
    // the first time we enter this function we should load the
    // insurance list from the database (but only once for the rest of
    // the program runtime)
    if (db.listInsurances.isEmpty()) {
        db.retrieveItemList(myDB::TABLE_INSURANCES, reinterpret_cast<QList<cTableItem*>&>(db.listInsurances));
        if (db.listInsurances.isEmpty()) {
            ui.labInsuranceName->setText(QString::fromUtf8("Unbekannte Kasse"));
            return false;
        }
    }

    QString ik = ui.le_insurance->text();

    if (ik.isEmpty()) {
        ui.labInsuranceName->setText("");
        return false;
    }

    if (ik.length() != 9) {
        ui.labInsuranceName->setText(QString::fromUtf8("Ungültige IK"));
        return false;
    }

    // TODO: Add lookup check for other insurance types
    // (right now we only have a lookup database for GKV)
    if (ui.cb_instype->currentIndex() != cInsuranceType::IT_GKV) {
        curPatient.setInsurance(ik);
        ui.labInsuranceName->setText("");
        return true;
    }

    QString ins_name = "";
    for (int i = 0; i < db.listInsurances.count(); i++) {
        if (db.listInsurances.at(i)->getIK() == ik) {
            ins_name = db.listInsurances.at(i)->getShortName();
            curPatient.setInsurance(ik);
            ui.labInsuranceName->setText(ins_name);
            break;
        }
    }

    if (ins_name.isEmpty()) {
        ui.labInsuranceName->setText(QString::fromUtf8("Unbekannte Kasse"));
        curPatient.setInsurance("");
        return false;
    }
    return true;
}
