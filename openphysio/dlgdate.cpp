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

#include "dlgdate.h"
#include "dlgbrowse.h"
#include "browsereceipes.h"
#include "browsepatients.h"
#include "browsecourses.h"
#include "msgbox.h"
#include "helperwrapper.h"
#include "employeetools.h"
#include "datetools.h"
#include "dlgtextblock.h"
#include "dlgreceipe.h"
#include "dlgreceipepriv.h"


ActivityCheckBox::ActivityCheckBox(int i, QWidget * parent)
    : QCheckBox(parent)
{
    a_id = i;
}

void ActivityCheckBox::setActivityID(int i)
{
    a_id = i;
}

int ActivityCheckBox::getActivityID()
{
    return a_id;
}


DlgDate::DlgDate(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);

    ui.labelTel->hide();

    for (int i = 0; i < db.dateTypes.count(); i++)
        ui.cbType->addItem(db.dateTypes.at(i)->getDescription());

    EmplTool.getActiveEmployees(list_empl);
    EmplTool.fillComboBox(ui.cbEmployee);

    for (int i = 0; i < db.listDateStates.count(); i++)
        ui.cbState->addItem(db.listDateStates.at(i)->getDescription());

    for (int i = 0; i < db.rooms.count(); i++)
        ui.cbRoom->addItem(db.rooms.at(i)->getName());

    for (int i = 0; i < db.listCars.count(); i++)
        ui.cbCar->addItem(db.listCars.at(i)->getName());

    slotSwitchDateType(0);

    connect(ui.pbCancel, &QPushButton::clicked, this, &DlgDate::slotCancel);
    connect(ui.pbSave, &QPushButton::clicked, this, &DlgDate::slotSave);
    connect(ui.pbFollowUp, &QPushButton::clicked, this, &DlgDate::slotFollowUp);
    connect(ui.pbReceipe, &QPushButton::clicked, this, &DlgDate::slotReceipe);
    connect(ui.pbPatient, &QPushButton::clicked, this, &DlgDate::slotPatient);
    connect(ui.pbCourse, &QPushButton::clicked, this, &DlgDate::slotCourse);
    connect(ui.cbType, &QComboBox::currentIndexChanged, this, &DlgDate::slotSwitchDateType);
    connect(ui.cbFullDay, &QCheckBox::checkStateChanged, this, &DlgDate::slotFullDay);
    connect(ui.pbEditPatient, &QPushButton::clicked, this, &DlgDate::slotEditPatient);
    connect(ui.pbEditReceipe, &QPushButton::clicked, this, &DlgDate::slotEditReceipe);
    connect(ui.pbDelPatient, &QPushButton::clicked, this, &DlgDate::slotDelPatient);
    connect(ui.pbDelReceipe, &QPushButton::clicked, this, &DlgDate::slotDelReceipe);
    connect(ui.pbTextBlock, &QPushButton::clicked, this, &DlgDate::slotTextModule);
    connect(ui.pbDelDate, &QPushButton::clicked, this, &DlgDate::slotDelDate);

    connect(ui.cbEmployee, &QComboBox::currentIndexChanged, this, &DlgDate::slotModified);
    connect(ui.cbState, &QComboBox::currentIndexChanged, this,&DlgDate::slotModified);
    connect(ui.cbRoom, &QComboBox::currentIndexChanged, this, &DlgDate::slotModified);
    connect(ui.cbCar, &QComboBox::currentIndexChanged, this, &DlgDate::slotModified);
    connect(ui.plainTextEdit, &QPlainTextEdit::textChanged, this, &DlgDate::slotModified);
    connect(ui.lePatient, &QLineEdit::textChanged, this, &DlgDate::slotModified);
    connect(ui.leReceipe, &QLineEdit::textChanged, this, &DlgDate::slotModified);

    deStart = new DateEdit();
    deEnd = new DateEdit();
    teStart = new TimeEdit();
    teEnd = new TimeEdit();
    QGridLayout *grid = qobject_cast<QGridLayout*>(ui.widget_5->layout());
    if (grid) {
        grid->addWidget(deStart, 0, 1);
        grid->addWidget(new QLabel("bis"), 0, 2);
        grid->addWidget(deEnd, 0, 3);
        grid->addWidget(teStart, 2, 1);
        grid->addWidget(new QLabel("bis"), 2, 2);
        grid->addWidget(teEnd, 2, 3);
    }

    connect(deStart, &DateEdit::dateChanged, this, &DlgDate::slotStartDateChanged);
    connect(deEnd, &DateEdit::dateChanged, this, &DlgDate::slotEndDateChanged);
    connect(teEnd, &TimeEdit::timeChanged, this, &DlgDate::slotModified);
    connect(teStart, &TimeEdit::timeChanged, this, &DlgDate::slotModified);

    grid_physio = new QGridLayout(ui.widgetPhysio);
    grid_physio->setContentsMargins(0, 0, 0, 0);
    grid_ergo = new QGridLayout(ui.widgetErgo);
    grid_ergo->setContentsMargins(0, 0, 0, 0);

    createActivities();

#ifdef Q_OS_MACX
    ui.widgetReceipe->layout()->setSpacing(-1);
#endif

    curMode = MODE_FULL;
    installEventFilter(this);
    flag_change = false;
    date_deleted = false;
    show_fullname = (bool)myConf.get(ConfXML::AREA_PLANNER, "render_fullname").toInt();
}


DlgDate::~DlgDate()
{
    while (!list_empl.isEmpty()) delete list_empl.takeFirst();
}


void DlgDate::init(cDateItem& cdi)
{
    enableActivities(false);

    curDateItem = cdi;
    curReceipe.reset();
    curPatient.reset();
    curCourse.reset();

    if (curDateItem.getRID() > 0) {
        if (db.retrieveItem(curReceipe, curDateItem.getRID()))
	    db.retrieveItem(curPatient, curReceipe.getPatientID());
    } else if (curDateItem.getPID() > 0) {
        db.retrieveItem(curPatient, curDateItem.getPID());
    }

    if (curDateItem.getCID() > 0) db.retrieveItem(curCourse, curDateItem.getCID());

    disconnect(deStart, SIGNAL(dateChanged(QDate)), this, SLOT(slotStartDateChanged(QDate)));
    disconnect(deEnd, SIGNAL(dateChanged(QDate)), this, SLOT(slotEndDateChanged(QDate)));

    deStart->setDate(curDateItem.getDate());
    deEnd->setDate(curDateItem.getDate());
    ui.widgetWeekdays->setEnabled(false);

    connect(deStart, SIGNAL(dateChanged(QDate)), this, SLOT(slotStartDateChanged(QDate)));
    connect(deEnd, SIGNAL(dateChanged(QDate)), this, SLOT(slotEndDateChanged(QDate)));

    ui.cbType->setCurrentIndex(curDateItem.getType());
    ui.cbState->setCurrentIndex(curDateItem.getStateID());
    teStart->setTime(curDateItem.getStart());
    teEnd->setTime(curDateItem.getEnd());
    if (curDateItem.getFullDay()) ui.cbFullDay->setChecked(true);
    else ui.cbFullDay->setChecked(false);
    ui.plainTextEdit->setPlainText(curDateItem.getDesc());

    int e_id = 2 ; // curDateItem.getEID();
    ui.cbEmployee->setCurrentIndex(EmplTool.getIndexOfEmployee(e_id, list_empl));

    int r_id = curDateItem.getRoomID();
    if (r_id > 0) {
        for (int x = 0; x < db.rooms.count(); x++) {
            if (r_id == db.rooms.at(x)->getID()) {
                ui.cbRoom->setCurrentIndex(x);
                break;
            }
        }
    } else ui.cbRoom->setCurrentIndex(-1);

    int c_id = curDateItem.getCarID();
    if (c_id > 0) {
        for (int x = 0; x < db.listCars.count(); x++) {
            if (c_id == db.listCars.at(x)->getID()) {
                ui.cbCar->setCurrentIndex(x);
                break;
            }
        }
    } else ui.cbCar->setCurrentIndex(-1);

    updateActivities();

    int opt = curDateItem.getOptions();
    if ((opt >> cDateItem::OPT_VISIT) & 1) ui.cbVisit->setChecked(true);
    else ui.cbVisit->setChecked(false);

    setupTitle();

    if (curDateItem.getID() > 0) setWindowTitle("Termin bearbeiten");
    else setWindowTitle("Termin erstellen");

    enableActivities(true);

    flag_change = false;
}


void DlgDate::setMode(int m)
{
    curMode = m;
    if (curMode == MODE_EDIT_COURSE) {
        ui.cbType->setEnabled(false);
        ui.widgetReceipe->hide();
        ui.widgetWeekdays->setEnabled(false);
        ui.leCourse->setEnabled(false);
        ui.pbCourse->setEnabled(false);
        ui.leTitle->setEnabled(false);
        deStart->setEnabled(false);
        deEnd->setEnabled(false);
        ui.cbCar->setEnabled(false);
    }
}


void DlgDate::reject()
{
    if (flag_change && !date_deleted) {
        MsgBox box;
        if (box.execYesNoBox(QString::fromUtf8("Wollen Sie die Änderungen jetzt speichern?"), MsgBox::BTN_YES) == MsgBox::BTN_YES) {
            if (!saveDate()) return;
            flag_change = false;
            QDialog::accept();
            return;
        }
    }
    QDialog::reject();
}


void DlgDate::slotCancel()
{
    this->reject();
}


bool DlgDate::saveDate()
{
    if (ui.leTitle->text().isEmpty() && ui.cbType->currentIndex() == cDateType::DT_BREAK) {
        ui.leTitle->setText("Pause");
    }

    if (!deStart->date().isValid()) {
        QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), QString::fromUtf8("Bitte einen gültiges Datum eingeben!"), QMessageBox::Ok);
        return false;
    }

    if (ui.leTitle->text().isEmpty()) {
        QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), QString::fromUtf8("Bitte einen Titel für den Termin angeben!"), QMessageBox::Ok);
        return false;
    }

    if (ui.cbEmployee->currentIndex() < 0 && ui.cbRoom->currentIndex() < 0 && ui.cbCar->currentIndex() < 0) {
        QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), "Bitte geben Sie einen Mitarbeiter, Raum oder Firmenwagen an!", QMessageBox::Ok);
        return false;
    }

    if (!ui.cbFullDay->isChecked()) {
        if (teEnd->time() <= teStart->time()) {
            QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), "Die Anfangszeit des Termins muss vor der Endzeit liegen!", QMessageBox::Ok);
            return false;
        }
    }

    if (deEnd->date().isNull()) deEnd->setDate(deStart->date());
    if (deEnd->date() < deStart->date()) {
        QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), "Das Anfangsdatum des Termins muss vor dem Enddatum liegen!", QMessageBox::Ok);
        return false;
    }

    curDateItem.setStateID(ui.cbState->currentIndex());
    curDateItem.setType(ui.cbType->currentIndex());
    curDateItem.setDate(deStart->date());
    if (ui.cbFullDay->isChecked()) {
        curDateItem.setFullDay(true);
        curDateItem.setStart(QTime(0, 0, 0, 0));
        curDateItem.setEnd(QTime(0, 0, 0, 0));
    } else {
        curDateItem.setFullDay(false);
        curDateItem.setStart(teStart->time());
        curDateItem.setEnd(teEnd->time());
    }
    curDateItem.setTitle(ui.leTitle->text());
    curDateItem.setDesc(ui.plainTextEdit->toPlainText());
    if (ui.cbEmployee->currentIndex() >= 0) {
        curDateItem.setEID(list_empl.at(ui.cbEmployee->currentIndex())->getID());
    } else curDateItem.setEID(-1);

    int x = ui.cbRoom->currentIndex();
    if (x >= 0 && x < db.rooms.count()) curDateItem.setRoomID(db.rooms.at(x)->getID());
    else curDateItem.setRoomID(-1);
    x = ui.cbCar->currentIndex();
    if (x >= 0 && x < db.listCars.count()) curDateItem.setCarID(db.listCars.at(x)->getID());
    else curDateItem.setCarID(-1);
    curDateItem.setRID(curReceipe.getID());
    curDateItem.setPID(curPatient.getID());
    curDateItem.setCID(curCourse.getID());

    int act_physio = 0;
    int act_ergo = 0;
    if (curDateItem.getType() == cDateType::DT_THERAPY) {
        if (db.getSurgery().isProfession(cProfession::PROF_PHYSIOTHERAPY)) {
            for (int i = 0; i < cbPhysioActs.count(); i++) {
                if (cbPhysioActs.at(i)->isChecked()) {
                    int x = cbPhysioActs.at(i)->getActivityID();
                    act_physio = act_physio | (1 << x);
                }
            }
        }
        if (db.getSurgery().isProfession(cProfession::PROF_ERGOTHERAPY)) {
            for (int i = 0; i < cbErgoActs.count(); i++) {
                if (cbErgoActs.at(i)->isChecked()) {
                    int x = cbErgoActs.at(i)->getActivityID();
                    act_ergo = act_ergo | (1 << x);
                }
            }
        }
    }
    curDateItem.setActPhysio(act_physio);
    curDateItem.setActErgo(act_ergo);

    int opt = 0;
    int mask_bit = 0;
    if (ui.cbVisit->isChecked()) mask_bit = 1;
    opt = mask_bit << cDateItem::OPT_VISIT;
    curDateItem.setOptions(opt);

    // FIXME: we also need to check the dates when a series of appointments should be created
    // check whether this date will overlap with other dates regarding employees, rooms, and cars
    QDate d_start = deStart->date();
    if (d_start == deEnd->date()) {
        DateTools dt;
        if (!dt.OverlapCheck(curDateItem)) return false;
   }

    bool bRet = true;

    // save meta date if necessary
    if (deStart->date() != deEnd->date()) {
        curMetaDate.setStart(deStart->date());
        curMetaDate.setEnd(deEnd->date());
        // FIXME: We could store the week days used for a series of dates
        curMetaDate.setRepeatFlag(0);
        if (curMetaDate.getID() > 0) {
            bRet = db.updateItem(curMetaDate);
        } else {
            int id;
            bRet = db.storeItem(curMetaDate, &id);
            curMetaDate.setID(id);
            curDateItem.setMetaID(id);
        }
    } else {
        curDateItem.setMetaID(-1);
    }

    if (!bRet) {  // if we failed to save the meta date => throw an error message
        if (!bRet) QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), QString::fromUtf8("Termin konnte nicht gespeichert werden!"), QMessageBox::Ok);
        return false;
    }

    if (curDateItem.getID() > 0) {
        curDateItem.setFlag(cTableItem::FLAG_MODIFIED);
        bRet = db.updateItem(curDateItem);
    } else {
        int co = deStart->date().daysTo(deEnd->date());
        if (co == 0) {
            curDateItem.setFlag(cTableItem::FLAG_NEW);
            int id = -1;
            bRet = db.storeItem(curDateItem, &id);
        } else {
            // we need to save multiple dates depending on the meta date settings
            int consider_days[7];
            consider_days[0] = ui.cbMonday->isChecked();
            consider_days[1] = ui.cbTuesday->isChecked();
            consider_days[2] = ui.cbWednesday->isChecked();
            consider_days[3] = ui.cbThursday->isChecked();
            consider_days[4] = ui.cbFriday->isChecked();
            consider_days[5] = ui.cbSaturday->isChecked();
            consider_days[6] = ui.cbSunday->isChecked();
            bool use_repetition = false;
            // if not at least one single checkbox was checked we assume that
            // the user does not want repetition but only wants to create an appointment
            // for a certain duration
            for (int i = 0; i < 7; i++)
                if (consider_days[i]) use_repetition = true;
            QList<cDateItem *> list_dates;
            cDateItem tmp = curDateItem;
            tmp.setFlag(cTableItem::FLAG_NEW);
            QDate d = tmp.getDate();
            for (int j = 0; j <= co; j++) {
                QDate date_tmp = d.addDays(j);
                if (consider_days[date_tmp.dayOfWeek() - 1] > 0 || !use_repetition) {
                    list_dates.append(new cDateItem());
                    tmp.setDate(date_tmp);
                    *list_dates.last() = tmp;
                }
            }
            if (list_dates.count() > 0) {
                if (!db.saveDateList(list_dates)) bRet = false;
                else this->accept();
            } else {
                QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), QString::fromUtf8("Es konnten keine Termine für den gewählten Zeitraum erzeugt werden!"), QMessageBox::Ok);
                return false;
            }
        }
    }

    if (bRet) flag_change = false;
    if (!bRet) QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), QString::fromUtf8("Termin konnte nicht gespeichert werden!"), QMessageBox::Ok);
    return bRet;
}


void DlgDate::slotSave()
{
    if (saveDate()) this->accept();
}


void DlgDate::setupTitle()
{
    if (curReceipe.getID() > 0)
        ui.leReceipe->setText("Rezept vom " + curReceipe.getDate().toString("dd.MM.yyyy"));

    QString title = "";
    if (curPatient.getID() > 0) {
	ui.lePatient->setText(curPatient.getForename() + " " + curPatient.getName());
        title = curPatient.getName()+ ", " + curPatient.getForename();
    } else if (curCourse.getID() > 0) {
        ui.leCourse->setText(curCourse.getName() + " (Beginn " +
                             curCourse.getStartDate().toString("dd.MM.yyyy") + ")" );
        title = curCourse.getName();
    } else {
	ui.lePatient->setText("");
        title = curDateItem.getTitle();
    }
    setupContact();

    QString tmp = "";
    switch(ui.cbType->currentIndex()) {
        case cDateType::DT_THERAPY: break;
        case cDateType::DT_HOLIDAY:
        case cDateType::DT_CORPORATE_HOLIDY:
        case cDateType::DT_TRAINING:
        case cDateType::DT_MEETING:
        case cDateType::DT_DRIVE:
        case cDateType::DT_BREAK:
            if (title.isEmpty()) title = db.dateTypes.at(ui.cbType->currentIndex())->getDescription();
            break;
        default: break;
    }

    ui.leTitle->setText(title + tmp);
    flag_change = true;
}


void DlgDate::setupContact()
{
    if (curPatient.getTelephone().isEmpty() && curPatient.getTelephoneJob().isEmpty() &&
        curPatient.getTelephoneMobile().isEmpty()) {
        ui.labelTel->hide();
    } else {
        QString s = " [Tel: ";
        if (!curPatient.getTelephone().isEmpty()) s = s + curPatient.getTelephone() + " (privat)";
        if (!curPatient.getTelephoneJob().isEmpty()) {
            if (s.length() < 7) s = s + curPatient.getTelephoneJob() + " (beruflich)";
            else s = s + " | " + curPatient.getTelephoneJob() + " (beruflich)";
        }
        if (!curPatient.getTelephoneMobile().isEmpty()) {
            if (s.length() < 7) s = s + curPatient.getTelephoneMobile() + " (Handy)";
            else s = s + " | " + curPatient.getTelephoneMobile() + " (Handy)";
        }
        s += " ]";
        ui.labelTel->setText(s);
        ui.labelTel->show();
    }
}


void DlgDate::slotReceipe()
{
    BrowseReceipes *browseReceipes = new BrowseReceipes(BrowseReceipes::MODE_SELECT);
    if (myConf.usePreload()) browseReceipes->preload();
    DlgBrowse browse(browseReceipes);
    int w = QApplication::primaryScreen()->geometry().width();
    if (w > 800) w = 800;
    browse.setGeometry(0, 0, w, 450);
    QRect r = QApplication::primaryScreen()->geometry();
    browse.move(r.center() - browse.rect().center());
    if (browse.exec() == QDialog::Accepted) {
        curReceipe = browseReceipes->getSelReceipe();
        curDateItem.setRID(curReceipe.getID());
	if (curReceipe.getID() >= 0) {
            curCourse.reset();
	    db.retrieveItem(curPatient, curReceipe.getPatientID());
            curDateItem.setPID(curReceipe.getPatientID());
            setDefaultEmployee(curPatient.getDefaultEmployeeID());
            setupTitle();
        }
    }
    delete browseReceipes;
}


void DlgDate::slotPatient()
{
    BrowsePatients *browsePatients = new BrowsePatients(Browse::MODE_SELECT);
    DlgBrowse browse(browsePatients);
    int w = QApplication::primaryScreen()->geometry().width();
    if (w > 800) w = 800;
    browse.setGeometry(0, 0, w, 450);
    QRect r = QApplication::primaryScreen()->geometry();
    browse.move(r.center() - browse.rect().center());
    if (myConf.usePreload()) browsePatients->preload();
    if (browse.exec() == QDialog::Accepted) {
        curPatient = browsePatients->getSelPatient();
        curDateItem.setPID(curPatient.getID());
        setDefaultEmployee(curPatient.getDefaultEmployeeID());
        setupContact();
        curReceipe.reset();
        curCourse.reset();
        setupTitle();
    }
    delete browsePatients;
}


void DlgDate::slotCourse()
{
    BrowseCourses *browseCourses = new BrowseCourses(Browse::MODE_SELECT);
    if (myConf.usePreload()) browseCourses->preload();
    DlgBrowse browse(browseCourses);
    int w = QApplication::primaryScreen()->geometry().width();
    if (w > 600) w = 600;
    browse.setGeometry(0, 0, w, 450);
    QRect r = QApplication::primaryScreen()->geometry();
    browse.move(r.center() - browse.rect().center());
    if (browse.exec() == QDialog::Accepted) {
        curCourse = browseCourses->getSelCourses();
        if (curCourse.getID() > 0) {
            teStart->setTime(curCourse.getStartTime());
            teEnd->setTime(teStart->time().addSecs(curCourse.getDuration() * 60));
            curPatient.reset();
            ui.labelTel->hide();
            curReceipe.reset();
            setDefaultEmployee(curCourse.getLeaderID());
            setupTitle();
        }
    }
    delete browseCourses;
}


void DlgDate::slotSwitchDateType(int i)
{
    // check if at least one date activity is enabled
    // if not we won't show the container widgets that would display activity checkboxes
    int x = 0;
    int y = 0;
    for (int j = 0; j < db.listDateActivities.count(); j++) {
        if (db.listDateActivities.at(j)->getProfession() == cProfession::PROF_PHYSIOTHERAPY)
            x += db.listDateActivities.at(j)->getActivityEnabled();
        if (db.listDateActivities.at(j)->getProfession() == cProfession::PROF_ERGOTHERAPY)
            y += db.listDateActivities.at(j)->getActivityEnabled();
    }

    switch(i) {
        case cDateType::DT_THERAPY:
            ui.widgetReceipe->setVisible(true);
            ui.labelActivities->hide();
            if (db.getSurgery().isProfession(cProfession::PROF_PHYSIOTHERAPY) && x > 0) {
                ui.labelActivities->show();
                ui.widgetPhysio->setVisible(true);
            } else ui.widgetPhysio->setVisible(false);
            if (db.getSurgery().isProfession(cProfession::PROF_ERGOTHERAPY) && y > 0) {
                ui.labelActivities->show();
                ui.widgetErgo->setVisible(true);
            } else ui.widgetErgo->setVisible(false);
            ui.widgetCourse->setVisible(false);
	    break;
        case cDateType::DT_COURSE:
            ui.widgetReceipe->setVisible(false);
            ui.widgetCourse->setVisible(true);
            break;
        default:
            ui.widgetReceipe->setVisible(false);
            ui.widgetCourse->setVisible(false);
            break;
    }
    setupTitle();
}


void DlgDate::slotFullDay(int i)
{
    if (i == Qt::Checked) {
        teStart->setEnabled(false);
        teEnd->setEnabled(false);
    } else {
        teStart->setEnabled(true);
        teEnd->setEnabled(true);
    }
    flag_change = true;
}


void DlgDate::slotModified()
{
    flag_change = true;
}

void DlgDate::slotStartDateChanged(const QDate &d)
//void DlgDate::slotStartDateChanged(QDate d)
{
    if (deEnd->date() < d) deEnd->setDate(d);
    if (d == deEnd->date()) {
        ui.widgetWeekdays->setEnabled(false);
    } else {
        ui.widgetWeekdays->setEnabled(true);
    }
    flag_change = true;
}

void DlgDate::slotEndDateChanged(const QDate &d)
//void DlgDate::slotEndDateChanged(QDate d)
{
    if (d < deStart->date()) {
        deStart->setDate(d);
        ui.widgetWeekdays->setEnabled(false);
    } else if (d != deStart->date()) {
        ui.widgetWeekdays->setEnabled(true);
    } else {
        ui.widgetWeekdays->setEnabled(false);
    }
    flag_change = true;
}


cDateItem DlgDate::getDateItem()
{
    return curDateItem;
}


void DlgDate::setDefaultEmployee(int eid)
{
    // use default employee if configured (and if user has not already selected an employee)
    if (ui.cbEmployee->currentIndex() < 0 && eid > 0) {
        ui.cbEmployee->setCurrentIndex(EmplTool.getIndexOfEmployee(eid, list_empl));
    }
}


bool DlgDate::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *ke = static_cast<QKeyEvent *>(event);
        if (ke->key() == Qt::Key_F1 || ke->key() == Qt::Key_Help) {
            helpWrapper.showHelpForKeyword("openphysio_dates", true);
            return true;
        }
    }
    return QDialog::eventFilter(obj, event);
}


void DlgDate::slotFollowUp()
{
    if (saveDate()) {
        curDateItem.setID(-1);
        curDateItem.setFlag(cTableItem::FLAG_NEW);
        // remove content field from course entries since this contains the
        // participance information (which should not be cloned ...)
        if (curDateItem.getType() == cDateType::DT_COURSE && curDateItem.getCID() > 0) {
            curDateItem.setContent("");
        }
        deEnd->setDate(deEnd->date().addDays(1));
        deStart->setDate(deStart->date().addDays(1));
    }
}


void DlgDate::slotEditPatient()
{
    if (curDateItem.getPID() <= 0 || curPatient.getID() <= 0) {
        QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), QString::fromUtf8("Es ist noch kein Patient mit diesem Termin verknüpft worden!"), QMessageBox::Ok);
        return;
    }
    DlgPatient dlgPatient;
    dlgPatient.initPatient(&curPatient);
    dlgPatient.exec();
}


void DlgDate::slotEditReceipe()
{
    if (curDateItem.getRID() <= 0 || curReceipe.getID() <= 0) {
        QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), QString::fromUtf8("Es ist noch kein Rezept mit diesem Termin verknüpft worden!"), QMessageBox::Ok);
        return;
    }
    if (curReceipe.getType() < 3) {
        DlgReceipe dlgReceipe;
        dlgReceipe.initDlg(&curReceipe, curReceipe.getProfession());
        dlgReceipe.exec();
    } else {
        DlgReceipePriv dlgReceipe;
        dlgReceipe.initDlg(&curReceipe);
        dlgReceipe.exec();
    }
}


void DlgDate::slotDelReceipe()
{
    curDateItem.setRID(-1);
    ui.leReceipe->clear();
}


void DlgDate::slotDelPatient()
{
    curDateItem.setPID(-1);
    ui.lePatient->clear();
}


void DlgDate::createActivities()
{
    bool use_physio = db.getSurgery().isProfession(cProfession::PROF_PHYSIOTHERAPY);
    bool use_ergo = db.getSurgery().isProfession(cProfession::PROF_ERGOTHERAPY);

    int physio_col = 0;
    int physio_row = 0;
    int ergo_col = 0;
    int ergo_row = 0;
    int row_break = 5; // TODO: it might be better to set up the value dynamically
    for (int i = 0; i < db.listDateActivities.count(); i++) {
        cDateActivity *cda = db.listDateActivities.at(i);
        if (!cda->getActivityEnabled()) continue;
        switch(cda->getProfession()) {
            case cProfession::PROF_PHYSIOTHERAPY:
                if (use_physio) {
                    cbPhysioActs.append(new ActivityCheckBox(cda->getActivityID()));
                    grid_physio->addWidget(cbPhysioActs.last(), physio_row, physio_col);
                    physio_col++;
                    if (physio_col > row_break) {
                        physio_row++;
                        physio_col = 0;
                    }
                    cbPhysioActs.last()->setText(cda->getActivityName());
                }
                break;
            case cProfession::PROF_ERGOTHERAPY:
                if (use_ergo) {
                    cbErgoActs.append(new ActivityCheckBox(cda->getActivityID()));
                    grid_ergo->addWidget(cbErgoActs.last(), ergo_row, ergo_col);
                    ergo_col++;
                    if (ergo_col > row_break) {
                        ergo_row++;
                        ergo_col = 0;
                    }
                    cbErgoActs.last()->setText(cda->getActivityName());
                }
                break;
            default: break;
        }
    }
    enableActivities(true);
}


void DlgDate::enableActivities(bool b)
{
    if (b) {
        for (int i = 0; i < cbPhysioActs.count(); i++) {
            connect(cbPhysioActs.at(i), SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
        }
        for (int i = 0; i < cbErgoActs.count(); i++) {
            connect(cbErgoActs.at(i), SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
        }
    } else {
        for (int i = 0; i < cbPhysioActs.count(); i++) {
            disconnect(cbPhysioActs.at(i), SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
        }
        for (int i = 0; i < cbErgoActs.count(); i++) {
            disconnect(cbErgoActs.at(i), SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
        }
    }
}


void DlgDate::updateActivities()
{
    int act_physio = curDateItem.getActPhysio();
    int act_ergo = curDateItem.getActErgo();

    for (int i = 0; i < cbPhysioActs.count(); i++) {
        cbPhysioActs.at(i)->setChecked((bool)((act_physio >> cbPhysioActs.at(i)->getActivityID()) & 1));
    }
    for (int i = 0; i < cbErgoActs.count(); i++) {
        cbErgoActs.at(i)->setChecked((bool)((act_ergo >> cbErgoActs.at(i)->getActivityID()) & 1));
    }
}


void DlgDate::slotTextModule()
{
    DlgTextBlock *dlg = new DlgTextBlock();
    dlg->init(cTextModule::TM_DATES);
    if (dlg->exec() == QDialog::Accepted) {
        ui.plainTextEdit->appendPlainText(dlg->getTextModule());
    }
    delete dlg;
}


void DlgDate::slotDelDate()
{
    if (curDateItem.getID() > 0) {
        MsgBox box;
        if (box.execYesNoBox(QString::fromUtf8("Termin wirklich löschen?"), MsgBox::BTN_NO) == MsgBox::BTN_YES) {
            if (!db.deleteItem(myDB::TABLE_DATES, curDateItem.getID())) {
                MsgBox::error(QString::fromUtf8("Fehler beim Löschen des Termins aufgetreten!"));
            } else {
                curDateItem.reset();
                date_deleted = true;
            }
        } else return;
    }
    this->close();
}
