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

#include "dlgcourse.h"
#include "browsepatients.h"
#include "protect.h"
#include "gendoc.h"
#include "msgbox.h"
#include "dlgbrowse.h"
#include "dlgdate.h"
#include "employeetools.h"
#include "helperwrapper.h"


#define ROW_PARTICIPANCE 2

MemberItem::MemberItem(QListWidget *parent)
    : QListWidgetItem(parent)
{
    custom_data = -1;
}

MemberItem::MemberItem(QString & text, QListWidget *parent, int type)
    : QListWidgetItem(text, parent, type)
{
    custom_data = -1;
}

MemberItem::~MemberItem()
{
}

int MemberItem::getPatientID()
{
    return patient.getID();
}

void MemberItem::setPatient(cPatient p)
{
    patient = p;
}

cPatient MemberItem::getPatient()
{
    return patient;
}


DlgCourse::DlgCourse(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);

#ifdef Q_OS_MACX
    ui.tab_1->layout()->setSpacing(-1);
    ui.widget_2->layout()->setSpacing(-1);
#endif

    bMemberTable = false; // we did not build the member table yet
    curDateOffset = -1;

    ui.treeWidget->setIconSize(QSize(16,16));

    curCourse.reset();
	
    for (int i = 0; i < db.listReceipeStates.count(); i++)
        ui.cbState->addItem(db.listReceipeStates.at(i)->getDescription());

    ui.cbState->setCurrentIndex(0);

    EmplTool.getActiveEmployees(list_empl, cEmployeeGroup::GROUP_THERAPY);
    EmplTool.fillComboBox(ui.cbLeader, list_empl, EmployeeTools::FLAG_FULLNAME);
    EmplTool.fillComboBox(ui.cbCurLeader, list_empl, EmployeeTools::FLAG_PID);

    for (int i = 0; i < db.rooms.count(); i++) {
        ui.cbRoom->addItem(db.rooms.at(i)->getName());
    }

    connect(ui.pbAddDate, &QPushButton::clicked, this, &DlgCourse::slotAddDate);
    connect(ui.pbDelDate, &QPushButton::clicked, this, &DlgCourse::slotDelDate);
    connect(ui.pbCancel, &QPushButton::clicked, this, &QDialog::reject);
    connect(ui.pbSave, &QPushButton::clicked, this, &DlgCourse::slotSave);
    connect(ui.pbInvoice, &QPushButton::clicked, this, &DlgCourse::slotInvoice);
    connect(ui.pbAddPatient, &QPushButton::clicked, this, &DlgCourse::slotAddPatient);
    connect(ui.pbDelPatient, &QPushButton::clicked, this, &DlgCourse::slotDelPatient);
    connect(ui.pbParticipance, &QPushButton::clicked, this, &DlgCourse::slotParticipance);
    connect(ui.pbPartList, &QPushButton::clicked, this, &DlgCourse::slotPartList);
    connect(ui.cbLeader, &QComboBox::currentIndexChanged, this, &DlgCourse::slotLeader);

    connect(ui.cbState, &QComboBox::currentIndexChanged, this, &DlgCourse::slotModified);
    connect(ui.spDuration, &QSpinBox::valueChanged, this, &DlgCourse::slotModified);
    connect(ui.spPrice, &QDoubleSpinBox::valueChanged, this, &DlgCourse::slotModified);
    connect(ui.spUnits, &QSpinBox::valueChanged, this, &DlgCourse::slotModified);
    connect(ui.leName, &QLineEdit::textChanged, this, &DlgCourse::slotModifiedWithCheck);
    connect(ui.treeWidget, &QTreeWidget::currentItemChanged, this, &DlgCourse::slotDateSelected);
    connect(ui.treeWidget,  &QTreeWidget::itemDoubleClicked, this, &DlgCourse::slotEditDate);

    ui.treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui.treeWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotContextMenu(QPoint)));

    ui.listPatients->setSortingEnabled(true);

    teClock = new TimeEdit();
    teFrom = new TimeEdit();
    teTo = new TimeEdit();
    deStart = new DateEdit();
    deStart->setDate(QDate::currentDate());
    dateEdit = new DateEdit();
    dateEdit->setDate(QDate::currentDate());
    connect(deStart, SIGNAL(dateChanged(QDate)), this, SLOT(slotModified()));
    connect(teClock, SIGNAL(timeChanged(QTime)), this, SLOT(slotModified()));
    QGridLayout *grid = qobject_cast<QGridLayout*>(this->layout());
    if (grid) {
        grid->addWidget(deStart, 2, 1);
        grid->addWidget(teClock, 3, 1);
    }
    setTabOrder(ui.cbLeader, deStart);
    setTabOrder(ui.cbState, teClock);
    grid = qobject_cast<QGridLayout*>(ui.widget_2->layout());
    if (grid) grid->addWidget(dateEdit, 0, 2);
    grid = qobject_cast<QGridLayout*>(ui.widget_3->layout());
    if (grid) {
        teFrom->setMinimumWidth(40);
        teTo->setMinimumWidth(40);
        grid->addWidget(teFrom, 0, 1);
        grid->addWidget(new QLabel("bis"), 0, 2);
        grid->addWidget(teTo, 0, 3);
    }

    installEventFilter(this);
    flag_change = false;
}


DlgCourse::~DlgCourse()
{
    while (!list_empl.isEmpty()) delete list_empl.takeFirst();
    while (!list_dates.isEmpty()) delete list_dates.takeFirst();
}


void DlgCourse::initDlg(cCourse *c)
{
    loadCourse(c);
}


void DlgCourse::loadCourse(cCourse *c)
{
    if (c != NULL) {
        curCourse = *c;
        ui.leName->setText(curCourse.getName());
        ui.cbState->setCurrentIndex(curCourse.getState());
        deStart->setDate(curCourse.getStartDate());
	ui.spUnits->setValue(curCourse.getUnits());
	ui.spDuration->setValue(curCourse.getDuration());
	ui.spPrice->setValue(curCourse.getPrice().toFloat());
        teClock->setTime(curCourse.getStartTime());
        teFrom->setTime(curCourse.getStartTime());
        teTo->setTime(curCourse.getStartTime().addSecs(curCourse.getDuration() * 60));
        ui.cbLeader->setCurrentIndex(EmplTool.getIndexOfEmployee(curCourse.getLeaderID(), list_empl));

	cPatient p;
        QString s;
        QList<int> del_pids; // we need this if a patient does not exist anymore
        for (int i = 0; i < curCourse.getPidList().count(); i++) {
            p.reset();
            if (!db.retrieveItem(p, curCourse.getPidList().at(i))) {
                // FIXME: it might be a difference if the patient is removed permanently or
                //        if we can't query the database temporarly
                del_pids.append(i);
            } else {
                QString s = p.getName() + ", " + p.getForename();
                MemberItem *item = new MemberItem(s);
                item->setPatient(p);
                ui.listPatients->addItem(item);
            }
        }
        for (int i = 0; i < del_pids.count(); i++)
            curCourse.delPid(del_pids.at(i));

        while (!list_dates.isEmpty()) delete list_dates.takeFirst();
        db.getDateList(list_dates, cDateType::DT_COURSE, curCourse.getID());
        if (list_dates.count() > 0) {
            curDateOffset = 0;
            buildMemberTable();
            buildDateTree();
        }

    } else {
        curCourse.reset();
	ui.spDuration->setValue(myConf.get(ConfXML::AREA_COURSES, "default_duration").toInt());
	ui.spUnits->setValue(myConf.get(ConfXML::AREA_COURSES, "default_units").toInt());
        dateEdit->setDate(QDate::currentDate());
        ui.cbRoom->setCurrentIndex(-1);
        ui.cbLeader->setCurrentIndex(-1);
    }

    flag_change = false;
}


void DlgCourse::reject()
{
    if (flag_change) {
        MsgBox box;
        if (box.execYesNoBox(QString::fromUtf8("Die Kursdaten wurde geändert. Soll der Kurs gespeichert werden?"), MsgBox::BTN_YES) == MsgBox::BTN_YES) {
	    GUARD(Protect::FUNC_COU);
	    if (saveCourse()) QDialog::accept();
	    else return;
	}
    }
    QDialog::reject();
}


void DlgCourse::slotSave()
{
    GUARD(Protect::FUNC_COU);
    if (saveCourse()) accept();
}


bool DlgCourse::saveCourse()
{
    bool bRet = true;
	
    if (ui.leName->text().isEmpty()) {
        QMessageBox::warning(this, tr("OpenPhysio"), QString::fromUtf8("Bitte geben Sie erst einen Kursnamen ein!\n"), QMessageBox::Ok);
	return false;
    }
    if (ui.cbLeader->currentIndex() < 0) {
        QMessageBox::warning(this, tr("OpenPhysio"), QString::fromUtf8("Bitte geben Sie erst einen Kursleiter an!\n"), QMessageBox::Ok);
        return false;
    }

    curCourse.setName(ui.leName->text());
    curCourse.setState(ui.cbState->currentIndex());
    curCourse.setStartDate(deStart->date());
    curCourse.setPrice(QString::number(ui.spPrice->value()));
    curCourse.setDuration(ui.spDuration->value());
    curCourse.setUnits(ui.spUnits->value());
    curCourse.setStartTime(teClock->time());

    if (ui.cbLeader->currentIndex() >= 0) {
        curCourse.setLeaderID(list_empl.at(ui.cbLeader->currentIndex())->getID());
    }

    if (curCourse.getID() <= 0) {
        int id;
	if ( db.storeItem(curCourse, &id) == false) {
            QMessageBox::warning(this, tr("OpenPhysio"), QString::fromUtf8("Fehler beim Speichern des Kurses aufgetreten!\n"), QMessageBox::Ok);
            bRet = false;
        } else {
            curCourse.setID(id);
            teFrom->setTime(curCourse.getStartTime());
            teTo->setTime(curCourse.getStartTime().addSecs(curCourse.getDuration() * 60));
            ui.cbCurLeader->setCurrentIndex(ui.cbLeader->currentIndex());
        }
    } else {
	if ( db.updateItem(curCourse) == false) {
            QMessageBox::warning(this, tr("OpenPhysio"), QString::fromUtf8("Fehler beim Aktualisieren der Kursdaten aufgetreten!\n"), QMessageBox::Ok);
            bRet = false;
        }
    }

    if (bRet) {
        db.saveDateList(list_dates);
        flag_change = false;
    }

    return bRet;
}


void DlgCourse::slotInvoice()
{
    if (curCourse.getID() < 0) {
        MsgBox box;
        if (box.execYesNoBox(QString::fromUtf8("Der Kurs muss erst gespeichert werden, bevor eine Rechnung erstellt werden kann!\n\nJetzt speichern?"), MsgBox::BTN_YES) == MsgBox::BTN_NO) {
            return;
        } else {
            if (!saveCourse()) return;
        }
    }

    if (curCourse.getID() < 0) {
        QMessageBox::warning(this, tr("OpenPhysio"), QString::fromUtf8("Der Kurs muss erst gespeichert werden!"), QMessageBox::Ok);
	return;
    }

    if (curCourse.getPidList().count() == 0) {
        QMessageBox::warning(this, tr("OpenPhysio"), QString::fromUtf8("Der Kurs enthält noch keine Teilnehmer!\n"), QMessageBox::Ok);
	return;
    }

   if (flag_change) {
	MsgBox box;
        if (box.execYesNoBox(QString::fromUtf8("Sie haben inzwischen Kursdaten geändert!\nDie Daten vorher speichern (empfohlen)?"), MsgBox::BTN_YES) == MsgBox::BTN_YES) {
	    saveCourse();
	}
    }

    QItemSelectionModel *model = ui.listPatients->selectionModel();
    QModelIndexList indexes = model->selectedRows();
    if (indexes.count() <= 0) {
	MsgBox box;
        if (box.execYesNoBox(QString::fromUtf8("Sie haben keine Teilnehmer ausgewählt!\nSoll für alle Teilnehmer eine Rechnung erstellt werden?"), MsgBox::BTN_YES) == MsgBox::BTN_NO) {
	    return;
	}
	ui.listPatients->selectAll();
	indexes = model->selectedRows();
	if (indexes.count() <= 0) return;
    }

    QList<cInvoice *> invoices;
    for (int i = 0; i < indexes.count(); i++) {
	int row = indexes.at(i).row();
        if (row >= 0) {
            invoices.append(new cInvoice());
            MemberItem *item = (MemberItem*)ui.listPatients->item(row);
            cPatient p = item->getPatient();
            invoices.last()->setCourseID(curCourse.getID());
            invoices.last()->setPatientID(p.getID());
            invoices.last()->setType(cInvoice::INVOICE_COURSE);
        }
    }

    gendoc.genInvoice(invoices, 0);
    while (!invoices.isEmpty()) delete invoices.takeFirst();
}


void DlgCourse::slotAddPatient()
{
    BrowsePatients *browsePatients = new BrowsePatients(Browse::MODE_MULTISELECT);
    DlgBrowse browse(browsePatients);
    if (myConf.usePreload()) browsePatients->preload();
    if (browse.exec() == QDialog::Accepted) {
        flag_change = true;
        QList<int> listPat = browsePatients->getSelectedPatients();
        cPatient p;
        // retreive data from database for each selected patient
        for (int i = 0; i < listPat.count(); i++) {
            // check if patients is already in course list
            if (!curCourse.getPidList().contains(listPat.at(i))) {
                p.reset();
                if (db.retrieveItem(p, listPat.at(i))) {
                    curCourse.addPid(listPat.at(i));
                    QString s = p.getName() + ", " + p.getForename();
                    MemberItem *item = new MemberItem(s);
                    item->setPatient(p);
                    ui.listPatients->addItem(item);
                }
            }
        }

        if (list_dates.count() > 0) buildMemberTable();
    }
    delete browsePatients;
}


void DlgCourse::slotDelPatient()
{
    QItemSelectionModel *model = ui.listPatients->selectionModel();
    QModelIndexList indexes = model->selectedRows();
    if (indexes.count() <= 0) return;
    flag_change = true;
    QList<int> del_pids;
    for (int i = 0; i < indexes.count(); i++) {
        int row = indexes.at(i).row();
        MemberItem *item = (MemberItem*)ui.listPatients->item(row);
        int pid = item->getPatientID();
        del_pids.append(pid);
        delete ui.listPatients->takeItem(row);
        curCourse.delPid(pid);
    }

    // we need to remove the patients from every entry of the date list to be safe
    for (int i = 0; i < list_dates.count(); i++) {
        QString s = list_dates.at(i)->getContent();
        if (s.isEmpty()) continue;
        QStringList qsl = s.split(":");
        for (int x = 0; x < del_pids.count(); x++) {
            for (int y = 0; y < qsl.count(); y++) {
                if (del_pids.at(x) == qsl[y].toInt()) {
                    qsl.removeAt(y);
                    break;
                }
            }
        }
        if (qsl.count() > 0) s = qsl.join(":");
        else s = "";
        list_dates.at(i)->setContent(s);
        if (list_dates.at(i)->getFlag() == cTableItem::FLAG_ORG) {
            list_dates.at(curDateOffset)->setFlag(cTableItem::FLAG_MODIFIED);
        }
    }

    if (list_dates.count() > 0) buildMemberTable();
}


void DlgCourse::slotParticipance()
{
    if (curCourse.getPidList().count() == 0) {
        QMessageBox::warning(this, tr("OpenPhysio"), QString::fromUtf8("Der Kurs enthält noch keine Teilnehmer!\n"), QMessageBox::Ok);
	return;
    }
    if (flag_change) {
	MsgBox box;
        if (box.execYesNoBox(QString::fromUtf8("Sie haben inzwischen Kursdaten geändert!\nDie Daten vorher speichern?"), MsgBox::BTN_YES) == MsgBox::BTN_YES) {
	    saveCourse();
	}
    }

    QItemSelectionModel *model = ui.listPatients->selectionModel();
    QModelIndexList indexes = model->selectedRows();
    if (indexes.count() <= 0) {
	MsgBox box;
        if (box.execYesNoBox(QString::fromUtf8("Sie haben keine Teilnehmer ausgewählt!\nSoll für alle Teilnehmer eine Teilnahmebestätigung erstellt werden?"), MsgBox::BTN_YES) == MsgBox::BTN_NO) {
	    return;
	}
	ui.listPatients->selectAll();
	indexes = model->selectedRows();
	if (indexes.count() <= 0) return;
    }

    QList<cPatient *> patients;
    for (int i = 0; i < indexes.count(); i++) {
	int row = indexes.at(i).row();
        if (row >= 0) {
            MemberItem *item = (MemberItem*)ui.listPatients->item(row);
            patients.append(new cPatient(item->getPatient()));
        }
    }

    if (patients.count() > 0) {
	gendoc.genParticipanceConfirmation(curCourse, patients);
        // delete patient objects
        while (!patients.isEmpty()) delete patients.takeFirst();
    }
}


void DlgCourse::slotPartList()
{
    if (flag_change) {
	MsgBox box;
        if (box.execYesNoBox(QString::fromUtf8("Sie haben inzwischen Kursdaten geändert!\nDie Daten vorher speichern?"), MsgBox::BTN_YES) == MsgBox::BTN_YES) {
	    saveCourse();
	}
    }

    // we will prepare a list of patient objects in order to avoid that
    // the gendoc layer needs to query the database again
    QList<cPatient*> list;
    for (int i = 0; i < ui.listPatients->count(); i++) {
        list.append(new cPatient(((MemberItem*)(ui.listPatients->item(i)))->getPatient()));
    }
    gendoc.genPartList(curCourse, list);
}


void DlgCourse::slotAddDate()
{
    if (curCourse.getID() < 0) {
        MsgBox box;
        if (box.execYesNoBox(QString::fromUtf8("Der Kurs muss erst gespeichert werden!\n\nJetzt speichern?"), MsgBox::BTN_YES) == MsgBox::BTN_NO) {
            return;
        } else {
            if (!saveCourse()) return;
        }
    }

    if (list_empl.count() < 1) {
        QMessageBox::information(this, "OpenPhysio", QString::fromUtf8("Es muss mindestens ein Mitarbeiter (Gruppe: Therapeuten) bereits angelegt sein!"), QMessageBox::Ok);
        return;
    }
    if (teFrom->time() >= teTo->time()) {
        QMessageBox::information(this, "OpenPhysio", QString::fromUtf8("Der Beginn des Termins muss vor der Enduhrzeit liegen!"), QMessageBox::Ok);
        return;
    }
    if (ui.cbCurLeader->currentIndex() < 0 && ui.cbLeader->currentIndex() < 0) {
        QMessageBox::information(this, "OpenPhysio", QString::fromUtf8("Bitte wählen Sie einen Mitarbeiter aus!"), QMessageBox::Ok);
        return;
    }

    QDate date = dateEdit->date();
    // check if that date is already used
    // (we assume that a course is only held once a day; if a course is held multiple
    //  times a time it is recommended to create a new course since each course will
    //  certainly have its own participants)
    for (int i = 0; i < list_dates.count(); i++) {
        if ((list_dates.at(i)->getDate() == date) && (list_dates.at(i)->getFlag() != cTableItem::FLAG_DEL ) ) {
            QMessageBox::information(this, "OpenPhysio", QString::fromUtf8("Dieses Kursdatum ist bereits hinterlegt!"), QMessageBox::Ok);
            return;
        }
    }

    // now find the right place to insert the new date
    // (the dates should be ordered in treewidget)
    int insert_pos = -1;
    if (list_dates.count() > 0) {
        for (int i = 0; i < list_dates.count(); i++) {
            if ((list_dates.at(i)->getDate() > date) && (list_dates.at(i)->getFlag() != cTableItem::FLAG_DEL ) ) {
                insert_pos = i;
                break;
            }
        }
        if (insert_pos == -1) insert_pos = list_dates.size();
    } else insert_pos = 0;

    list_dates.insert(insert_pos, new cDateItem());
    cDateItem *cdi = list_dates.at(insert_pos);
    cdi->setDate(date);
    cdi->setCID(curCourse.getID());
    cdi->setTitle(curCourse.getName());
    cdi->setStart(teFrom->time());
    cdi->setEnd(teTo->time());
    cdi->setType(cDateType::DT_COURSE);
    cdi->setFlag(cTableItem::FLAG_NEW);
    if (ui.cbCurLeader->currentIndex() >= 0) {
        cdi->setEID(list_empl.at(ui.cbCurLeader->currentIndex())->getID());
    } else cdi->setEID(list_empl.at(ui.cbLeader->currentIndex())->getID());

    if ( (ui.cbRoom->currentIndex() >= 0) && (ui.cbRoom->currentIndex() < db.rooms.count()) ) {
        cdi->setRoomID(db.rooms.at(ui.cbRoom->currentIndex())->getID());
    } else cdi->setRoomID(-1);

    flag_change = true;

    if (!bMemberTable) {  // we need to build the member table at least once
        buildMemberTable();
    }
    buildDateTree();
}


void DlgCourse::delDate(QTreeWidgetItem *item)
{
    // look for the date item in the list that matches the selected item and that is
    // not already marked for deletion
    int date_ofs = findItemID(item);
    if (date_ofs < 0) return;

    if (list_dates.at(date_ofs)->getFlag() == cTableItem::FLAG_NEW) {
        // the date item was just created by the user but not yet saved to the database
        // => simply remove item from list
        delete list_dates.takeAt(date_ofs);
    } else if (list_dates.at(date_ofs)->getFlag() == cTableItem::FLAG_ORG) {
        // the date was already written to the database
        // => we mark it for later deletion
        list_dates.at(date_ofs)->setFlag(cTableItem::FLAG_DEL);
    }

    buildDateTree();
    flag_change = true;
}


void DlgCourse::slotDelDate()
{
    delDate(ui.treeWidget->currentItem());
}


void DlgCourse::slotDateSelected(QTreeWidgetItem *item)
{
    int date_ofs = findItemID(item);
    if (date_ofs < 0) return;

    disconnect(ui.tableWidget, SIGNAL(cellChanged(int,int)), this, SLOT(slotParticipanceChanged(int,int)));

    curDateOffset = date_ofs;
    // Build the member table for the selected date
    // (actually, only fill the participance column)
    QString s = list_dates.at(date_ofs)->getContent();
    // clear the whole participance colum
    for (int i = 0; i < ui.tableWidget->rowCount(); i++) {
        QTableWidgetItem *item = ui.tableWidget->item(i, ROW_PARTICIPANCE);
        if (item != NULL) item->setCheckState(Qt::Unchecked);
    }

    if (!s.isEmpty()) {
        // if pid is in date.content then setCheckState(Qt::Checked);
        QStringList qsl = s.split(":");

        // j correlates to the tablewidget row
        for (int j = 0; j < curCourse.getPidList().count(); j++) {
            int pid = curCourse.getPidList().at(j);
            for (int i = 0; i < qsl.count(); i++) {
                if ( pid == qsl[i].toInt() ) {
                    QTableWidgetItem *item = ui.tableWidget->item(j, ROW_PARTICIPANCE);
                    if (item != NULL) item->setCheckState(Qt::Checked);
                    break;
                }
            }
        }
    }

    connect(ui.tableWidget, SIGNAL(cellChanged(int,int)), this, SLOT(slotParticipanceChanged(int,int)));
}


void DlgCourse::buildDateTree()
{
    // clear treewidget and add items from list_dates that are not marked for deletion
    for (int i = 0; i < ui.treeWidget->topLevelItemCount(); ++i) {
        qDeleteAll(ui.treeWidget->topLevelItem(i)->takeChildren());
    }
    ui.treeWidget->clear();
    int co = 0;
    for (int i = 0; i < list_dates.count(); i++) {
        if (list_dates.at(i)->getFlag() == cTableItem::FLAG_DEL) continue;
        QTreeWidgetItem *item = new QTreeWidgetItem(ui.treeWidget);
        if (item != NULL) {
            cDateItem *cdi = list_dates.at(i);
            co++;
            QString s = cdi->getDate().toString("dd.MM.yyyy") + "  [" +
                        cdi->getStart().toString("hh:mm") + " - " + cdi->getEnd().toString("hh:mm") +
                        " " + EmplTool.getEmplPID(cdi->getEID()) + "]";
            item->setText(0, s);
            item->setIcon(0, QIcon(":/termine.png"));
        }
    }

    if (list_dates.count() > 0) curDateOffset = 0;
    else curDateOffset = -1;

    // TODO: no dates were displayed => clear member table
    if (co == 0) {
        //ui.tableWidget->clear();
    }

    // we trigger slotDateSelected(QTreeWidgetItem *w)
    // in order to show the participance list for the first date entry
    QTreeWidgetItem *item = ui.treeWidget->topLevelItem(0);
    if (item != NULL) ui.treeWidget->setCurrentItem(item);    
}


// called when the list of course participants changes
void DlgCourse::buildMemberTable()
{
    if (curCourse.getID() < 0) return;
    if (curCourse.getPidList().count() < 1) return;
    bMemberTable = true;

    disconnect(ui.tableWidget, SIGNAL(cellChanged(int,int)), this, SLOT(slotParticipanceChanged(int,int)));

    ui.tableWidget->clear();
    QStringList qsl;
    qsl << "Name" << "Vorname" << "Teilnahme";
    ui.tableWidget->setHorizontalHeaderLabels(qsl);

    // insert as many rows as registered course participants
    int co = ui.listPatients->count();
    ui.tableWidget->setRowCount(co);
    for (int i = 0; i < co; i++) {
        cPatient p = ((MemberItem*)(ui.listPatients->item(i)))->getPatient();

        QTableWidgetItem* item_surname = new QTableWidgetItem(p.getName());
        item_surname->setFlags(Qt::NoItemFlags);
        ui.tableWidget->setItem(i, 0, item_surname);

        QTableWidgetItem* item_forename = new QTableWidgetItem(p.getForename());
        item_forename->setFlags(Qt::NoItemFlags);
        ui.tableWidget->setItem(i, 1, item_forename);

        QTableWidgetItem* item_participance = new QTableWidgetItem();
        item_participance->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        item_participance->setCheckState(Qt::Unchecked);
        ui.tableWidget->setItem(i, ROW_PARTICIPANCE, item_participance);
    }
    ui.tableWidget->resizeColumnsToContents();

    connect(ui.tableWidget, SIGNAL(cellChanged(int,int)), this, SLOT(slotParticipanceChanged(int,int)));
}


// called when the user changes the participance of course members
// by clicking on the member the table (ui.tableWidget)
void DlgCourse::slotParticipanceChanged(int row, int column)
{
    // we only care about participants selection column
    if (column != ROW_PARTICIPANCE) return;

    // perform sanity checking
    if (curCourse.getPidList().count() != ui.tableWidget->rowCount()) return;
    if ( (curDateOffset < 0) || (curDateOffset >= list_dates.count()) ) return;

    QStringList qsl;
    for (int i = 0; i < ui.tableWidget->rowCount(); i++) {
        QTableWidgetItem *item = ui.tableWidget->item(i, ROW_PARTICIPANCE);
        if (item == NULL) continue;
        if (item->checkState() == Qt::Checked) {
            qsl.append(QString::number(curCourse.getPidList().at(i)));
        }
    }
    QString content = "";
    if (qsl.count() > 0) content = qsl.join(":");

    // save selected participants to the corresponding date entry
    list_dates.at(curDateOffset)->setContent(content);
    if (list_dates.at(curDateOffset)->getFlag() == cTableItem::FLAG_ORG) {
        list_dates.at(curDateOffset)->setFlag(cTableItem::FLAG_MODIFIED);
    }
    flag_change = true;
}


void DlgCourse::slotLeader(int i)
{
    ui.cbCurLeader->setCurrentIndex(i);
    slotModifiedWithCheck();
}


void DlgCourse::slotModified()
{
    flag_change = true;
}


void DlgCourse::slotModifiedWithCheck()
{
    flag_change = true;
    checkInput();
}


void DlgCourse::checkInput()
{
    QPalette p_hint(QApplication::palette());
    QPalette p_normal(QApplication::palette());
    p_hint.setColor(QPalette::Base, Qt::yellow);
    if (ui.leName->text().isEmpty()) {
        ui.leName->setPalette(p_hint);
        ui.leName->setToolTip("Kursname fehlt");
    } else {
        ui.leName->setPalette(p_normal);
        ui.leName->setToolTip("");
    }
    if (ui.cbLeader->currentIndex() < 0) {
        ui.cbLeader->setPalette(p_hint);
        ui.cbLeader->setToolTip("Angabe des Kursleiters fehlt");
    } else {
        ui.cbLeader->setPalette(p_normal);
        ui.cbLeader->setToolTip("");
    }
}


void DlgCourse::slotEditDate(QTreeWidgetItem *item)
{
    int date_ofs = findItemID(item);
    if (date_ofs < 0) return;

    DlgDate dlgDate;
    dlgDate.init(*list_dates.at(date_ofs));
    dlgDate.setMode(DlgDate::MODE_EDIT_COURSE);
    if (dlgDate.exec() == QDialog::Accepted) {
        // the user might have changed some properties of the date such as
        // employee or start/end time => need to update the list of dates as well as the treewidget
        cDateItem cdi_tmp = dlgDate.getDateItem();
        cDateItem *cdi = list_dates.at(date_ofs);
        int flag = cdi->getFlag();  // save flag
        *cdi = cdi_tmp;
        cdi->setFlag(flag);         // restore flag
        buildDateTree();
    }
}


int DlgCourse::findItemID(QTreeWidgetItem *item)
{
    int date_ofs = -1;
    if (item == NULL) return date_ofs;

    // We just match the date (which consists of 10 characters dd.MM.yyyy)
    // the remaining text of the item must be skipped
    QString match = item->text(0).left(10);
    // find the selected date in our date list
    for (int i = 0; i < list_dates.count(); i++) {
        if ( (match == list_dates.at(i)->getDate().toString("dd.MM.yyyy")) &&
             (list_dates.at(i)->getFlag() != cTableItem::FLAG_DEL) ) {
            date_ofs = i;
            break;
        }
    }
    return date_ofs;
}


void DlgCourse::slotContextMenu(const QPoint& pos)
{
    QTreeWidgetItem * item = NULL;
    item = ui.treeWidget->itemAt(pos);
    if (item == NULL) return;

    QMenu menu;
    QAction actDateEdit(QString::fromUtf8("Bearbeiten"), this);
    QAction actDateDel(QString::fromUtf8("Löschen"), this);
    actDateEdit.setData(0);
    actDateDel.setData(1);
    menu.addAction(&actDateEdit);
    menu.addAction(&actDateDel);
    QAction *selAct = menu.exec(ui.treeWidget->mapToGlobal(pos));
    if (selAct == NULL) return;
    switch (selAct->data().toInt()) {
        case 0: slotEditDate(item); break;
        case 1: delDate(item); break;
        default: break;
    }

}


void DlgCourse::showEvent(QShowEvent * event)
{
    if (db.listEmpl.count() <= 0) {
        QMessageBox::information(this, "OpenPhysio", QString::fromUtf8("Bitte legen Sie erst einen Mitarbeiter an.\nAnderenfalls können Sie keinen Kurs anlegen."), QMessageBox::Ok);
    }
    QDialog::showEvent(event);
}


bool DlgCourse::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *ke = static_cast<QKeyEvent *>(event);
        if (ke->key() == Qt::Key_F1 || ke->key() == Qt::Key_Help) {
            helpWrapper.showHelpForKeyword("openphysio_courses", true);
            return true;
        }
    }
    return QDialog::eventFilter(obj, event);
}
