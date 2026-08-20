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

#include "therapy.h"
#include "employeetools.h"
#include "datetools.h"

DelegateTherapy::DelegateTherapy(QObject* parent)
    : QItemDelegate(parent)
{
}

QWidget* DelegateTherapy::createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex& index) const
{
    QWidget* w;
    QVariant value = index.model()->data(index, Qt::DisplayRole);
    switch(index.column()) {
        case 0: {
            DateEdit *editor = new DateEdit(parent);
            if (value.isNull()) editor->setDate(QDate::currentDate());
            else editor->setDate(QDate::fromString(value.toString(), "dd.MM.yyyy"));
            w = editor;
            break;
        }
        case 1:
	case 2: {
            TimeEdit *editor = new TimeEdit(parent);
            if (value.isNull()) editor->setTime(QTime::currentTime());
            else editor->setTime(QTime::fromString(value.toString(), "hh:mm"));
            w = editor;
            break;
        }
        case 3: {
            QComboBox* editor = new QComboBox(parent);
            EmplTool.fillComboBox(editor, cEmployeeGroup::GROUP_THERAPY, EmployeeTools::FLAG_PID);
            w = editor;
            break;
        }
        case 4: {
            QComboBox* editor = new QComboBox(parent);
            for (int i = 0; i < db.rooms.count(); i++) {
                editor->addItem(db.rooms.at(i)->getName());
            }
            w = editor;
            break;
        }
        default:
            w = 0;
            break;
    }
    return w;
}


void DelegateTherapy::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    QVariant value = index.model()->data(index, Qt::DisplayRole);

    switch( index.column() ) {
        case 0: {
            QString s = value.toString();
            if (!s.isEmpty()) {
                static_cast<DateEdit*>(editor)->setDate(QDate::fromString(s, "dd.MM.yyyy"));
                // signal the change to the parent table widget
                emit ((DelegateTherapy*)this)->sigExtDateChanged(index.row());
            }
            break;
        }
        case 1:
        case 2: {
            static_cast<TimeEdit*>(editor)->setTime(value.toTime());
            break;
        }
        case 3:
        case 4: {
            QComboBox* box = static_cast<QComboBox*>( editor );
            box->setCurrentIndex( box->findText( value.toString() ) );
            break;
        }
        default: break;
    }
}

void DelegateTherapy::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    QVariant value;

    switch(index.column()) {
        case 0: {
            QDate qd = static_cast<DateEdit*>(editor)->date();
            value = qd.toString("dd.MM.yyyy");
            break;
        }
        case 1:
        case 2: {
            QTime qt = static_cast<TimeEdit*>(editor)->time();
            value = qt.toString("hh:mm");
            break;
        }
        case 3:
        case 4: {
            value = static_cast<QComboBox*>( editor )->currentText();
            break;
        }
        default: break;
    }
    model->setData(index, value);

    emit const_cast<DelegateTherapy*>(this)->contentChanged();
}



Therapy::Therapy()
{
    qtw = NULL;
    delegate = NULL;
    actTherapyDel = NULL;
    actTherapyDelAll = NULL;
    default_eid = -1;
    flag_changed = false;
}


Therapy::~Therapy()
{
    if (delegate != NULL) delete delegate;
    if (actTherapyDel != NULL) delete actTherapyDel;
    if (actTherapyDelAll != NULL) delete actTherapyDelAll;
    clearDates();
}


void Therapy::clearDates()
{
    while (!listDates.isEmpty()) delete listDates.takeFirst();
}


void Therapy::stage(QTableWidget* q, int dtype)
{
    qtw = q;
    date_type = dtype;

    qtw->setColumnWidth(0, 85);
    qtw->setColumnWidth(1, 50);
    qtw->setColumnWidth(2, 50);
    if (dtype == cDateType::DT_PATIENT) {
        qtw->resizeColumnToContents(3);
    } else {
        qtw->setColumnWidth(3, 80);
        qtw->resizeColumnToContents(4);
    }
    //qtw->setEditTriggers(QAbstractItemView::AllEditTriggers);

    if (delegate != NULL) delete delegate;
    delegate = new DelegateTherapy(this);
    qtw->setItemDelegate(delegate);
    connect(delegate, SIGNAL(sigExtDateChanged(int)), this, SLOT(slotDateChanged(int)));

    actTherapyDel = new QAction(QString::fromUtf8("Zeile löschen"), this);
    actTherapyDelAll = new QAction(QString::fromUtf8("Alle Termine löschen"), this);
    connect(actTherapyDel, SIGNAL(triggered()), this, SLOT(slotTherapyDel()));
    connect(actTherapyDelAll, SIGNAL(triggered()), this, SLOT(slotTherapyDelAll()));
    qtw->addAction(actTherapyDel);
    qtw->addAction(actTherapyDelAll);
    qtw->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(qtw, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotContextMenu()));
    connect(delegate, SIGNAL(contentChanged()), this, SLOT(slotChanged()));
    flag_changed = false;
}


void Therapy::slotContextMenu()
{
    QMenu m;
    QMenu sub("AutoFill (Mitarbeiter)");
    QMenu sub_room("AutoFill (Raum)");
    QList<QAction *> actions;
    QList<QAction *> actions_room;
    QStringList l = EmplTool.getActiveEmployeesList(EmployeeTools::FLAG_PID);
    for (int i = 0; i < l.count(); i++) {
        actions.append(new QAction(l.at(i), this));
        actions.last()->setData(1);
    }
    sub.addActions(actions);
    for (int i = 0; i < db.rooms.count(); i++) {
        actions_room.append(new QAction(db.rooms.at(i)->getName(), this));
        actions_room.last()->setData(2);
    }
    sub_room.addActions(actions_room);

    m.addAction(actTherapyDel);
    m.addAction(actTherapyDelAll);
    m.addMenu(&sub);
    m.addMenu(&sub_room);
    QAction *a = m.exec(QCursor::pos());

    if (a != NULL) {
        if (a->data() == 1) autofill(a->text(), 3);
        else if (a->data() == 2) autofill(a->text(), 4);
    }

    while (!actions.isEmpty()) delete actions.takeFirst();
    while (!actions_room.isEmpty()) delete actions_room.takeFirst();
}


void Therapy::autofill(QString s, int column)
{
    if (qtw == NULL) return;

    QAbstractItemModel* model;
    model = qtw->model();
    if (model == NULL) return;
    for (int i = 0; i < qtw->rowCount(); i++) {
        model->setData(model->index(i, column), s);
    }
    flag_changed = true;
}


void Therapy::reset()
{
    clearDates();
    listDatesDel.clear();
    if (qtw != NULL) qtw->clearContents();
}


void Therapy::load(int id)
{
    if (qtw == NULL) return;

    QAbstractItemModel* model = qtw->model();

    clearDates();
    if ( db.getDateList(listDates, date_type, id) == true ) {
	for (int y = 0; y < listDates.count(); y++) {
	    if (qtw->rowCount() < y + 1) qtw->insertRow(qtw->rowCount());
	    model->setData(model->index(y, 0), listDates.at(y)->getDate().toString("dd.MM.yyyy"));
	    model->setData(model->index(y, 1), listDates.at(y)->getStart().toString("hh:mm"));
	    model->setData(model->index(y, 2), listDates.at(y)->getEnd().toString("hh:mm"));
            model->setData(model->index(y, 3), EmplTool.getEmplPID(listDates.at(y)->getEID()));
            int room_id = listDates.at(y)->getRoomID();
            QString rn = "";
            for (int x = 0; x < db.rooms.count(); x++) {
                if (room_id == db.rooms.at(x)->getID()) {
                    rn = db.rooms.at(x)->getName();
                    break;
                }
            }
            model->setData(model->index(y, 4), rn);
	}
	// in order to avoid that the calendar widget immediately pops up
	// we set a non-existing cell to be active
	qtw->setCurrentCell(-1, -1);
    }
}

void Therapy::loadP(int id)
{
    if (qtw == NULL) return;

    QAbstractItemModel* model = qtw->model();

    clearDates();
    if ( db.loadDates(listDates, QDate::currentDate(), id) == true ) {
    for (int y = 0; y < listDates.count(); y++) {
        if (qtw->rowCount() < y + 1) qtw->insertRow(qtw->rowCount());
        model->setData(model->index(y, 0), listDates.at(y)->getDate().toString("dd.MM.yyyy"));
        model->setData(model->index(y, 1), listDates.at(y)->getStart().toString("hh:mm"));
        model->setData(model->index(y, 2), listDates.at(y)->getEnd().toString("hh:mm"));
        model->setData(model->index(y, 3), EmplTool.getEmplPID(listDates.at(y)->getEID()));
    }
    // in order to avoid that the calendar widget immediately pops up
    // we set a non-existing cell to be active
    qtw->setCurrentCell(-1, -1);
    }
}

void Therapy::save(int id)
{
    QAbstractItemModel* model;
    QString title = "";
    int i, co, org_co;
    cPatient patient;

    model = qtw->model();

    co = 0;
    org_co = listDates.count();

    // build title for dates
    switch (date_type) {
        case cDateType::DT_THERAPY: {
	    cReceipe receipe;
	    if (db.retrieveItem(receipe, id)) {
		db.retrieveItem(patient, receipe.getPatientID());
                title = patient.getName();
	    }
	    break; }
        case cDateType::DT_COURSE: {
	    cCourse course;
	    if (db.retrieveItem(course, id)) {
                title = course.getName();
	    }
            break; }
	default: break;
    }

    for (i = 0; i < qtw->rowCount(); i++) {
        // check if row is filled
        if ( !model->index(i, 0).data().isNull()) { // && !model->index(i, 3).data().isNull()) {
            QString room_name;
            int room_id = -1;
            if (!model->index(i, 4).data().isNull()) {
                room_name = model->index(i, 4).data().toString();
                for (int x = 0; x < db.rooms.count(); x++) {
                    if (room_name == db.rooms.at(x)->getName()) {
                        room_id = db.rooms.at(x)->getID();
                        break;
                    }
                }
            }
	    if (co < org_co) {
                cDateItem* cdi = listDates.at(co);
                cdi->setFlag(cTableItem::FLAG_MODIFIED);
                cdi->setDate(QDate::fromString(model->index(i, 0).data().toString(), "dd.MM.yyyy"));
                cdi->setStart(QTime::fromString(model->index(i, 1).data().toString(), "hh:mm"));
                cdi->setEnd(QTime::fromString(model->index(i, 2).data().toString(), "hh:mm"));
                cdi->setEID(EmplTool.getEmplID(model->index(i, 3).data().toString()));
                cdi->setPID(patient.getID());
                cdi->setType(date_type);
                cdi->setTitle(title);
                cdi->setRoomID(room_id);
		co++;
	    } else {
		cDateItem* cdi = new cDateItem;
		cdi->setDate(QDate::fromString(model->index(i, 0).data().toString(), "dd.MM.yyyy"));
		cdi->setStart(QTime::fromString(model->index(i, 1).data().toString(), "hh:mm"));
		cdi->setEnd(QTime::fromString(model->index(i, 2).data().toString(), "hh:mm"));
                cdi->setEID(EmplTool.getEmplID(model->index(i, 3).data().toString()));
                cdi->setPID(patient.getID());
		cdi->setType(date_type);
                cdi->setTitle(title);
                cdi->setRoomID(room_id);
		switch (date_type) {
                    case cDateType::DT_THERAPY: cdi->setRID(id); break;
                    case cDateType::DT_COURSE: cdi->setCID(id); break;
		    default: break;
		}
                listDates.append(cdi);
	    }
        }
    }

    // delete existing therapies the user wants to remove
    for (i = 0; i < listDatesDel.count(); i++) {
	db.deleteItem(myDB::TABLE_DATES, listDatesDel.value(i));
    }

    // update database
    db.saveDateList(listDates);

    flag_changed = false;
}


void Therapy::slotTherapyDel()
{
    int r = qtw->currentRow();
    if (r >= 0) {
        if (r < listDates.count()) {
	    listDatesDel << listDates.at(r)->getID();
            delete listDates.takeAt(r);
        }
        qtw->removeRow(r);
        // add an empty row to the end of the table
        qtw->insertRow(qtw->rowCount());
	flag_changed = true;
    }
}


void Therapy::slotTherapyDelAll()
{
    for (int i = 0; i < listDates.count(); i++)
	listDatesDel << listDates.at(i)->getID();
    clearDates();
    if (qtw != NULL) qtw->clearContents();
    flag_changed = true;
}


bool Therapy::changed()
{
    return flag_changed;
}


void Therapy::slotChanged()
{
    flag_changed = true;
}


bool Therapy::isConsistent()
{
    if (qtw == NULL) return false;

    QAbstractItemModel* model = qtw->model();
    if (model == NULL) return false;

    for (int i = 0; i < qtw->rowCount(); i++) {
	if (!model->index(i, 0).data().isNull() && (model->index(i, 3).data().isNull())) {
	    return false;
	}
    }
    return true;
}


void Therapy::addEmptyRows(int c)
{
    if ((qtw == NULL) || (c <= 0)) return;
    for (int i = 0; i < c; i++) {
	qtw->insertRow(qtw->rowCount());
    }
}


void Therapy::removeEndRow()
{
    if (qtw == NULL) return;
    if (qtw->rowCount() <= 0) return;
    qtw->removeRow(qtw->rowCount() - 1);
}


void Therapy::removeRows()
{
    if (qtw == NULL) return;
    while (qtw->rowCount()) {
	qtw->removeRow(qtw->rowCount() - 1);
    }
}


void Therapy::switchDateType(int dt)
{
    date_type = dt;
    for (int i = 0; i < listDates.count(); i++)
	listDates.at(i)->setType(date_type);
}


void Therapy::setDefaultEmployeeID(int i)
{
    default_eid = i;
}


void Therapy::slotDateChanged(int row)
{
    if (qtw == NULL || default_eid < 0 || row < 0) return;

    if (!EmplTool.isEmplActive(default_eid)) return;
    // we will fill the employee data field in table widget if a default employee was configured
    QAbstractItemModel* model = qtw->model();
    if (model->index(row, 3).data().isNull())
        model->setData(model->index(row, 3), EmplTool.getEmplPID(default_eid));
}


QList<cDateItem*> Therapy::getListDates()
{
    return listDates;
}
