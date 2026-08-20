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

#ifndef THERAPY_H_
#define THERAPY_H_

#include <QtCore>
#include <QtWidgets>
#include "config.h"
#include "mydb.h"

class DelegateTherapy : public QItemDelegate
{
    Q_OBJECT

public:
    DelegateTherapy(QObject* parent = 0);
    QWidget *createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex& index) const;
    void setEditorData(QWidget* editor, const QModelIndex& index) const;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const;

signals:
    void contentChanged();
    void dateChanged();
    void sigExtDateChanged(int row);
};


class Therapy: public QObject
{
    Q_OBJECT

private:
    QTableWidget *qtw;
    DelegateTherapy *delegate;
    int date_type;
    int default_eid;
    QAction *actTherapyDel, *actTherapyDelAll;
    QList<cDateItem*> listDates;
    QList<int> listDatesDel;
    bool flag_changed;
    void autofill(QString s, int column);
    void clearDates();

public:
    Therapy();
    virtual ~Therapy();
    void stage(QTableWidget* q, int dtype);
    void reset();
    void load(int id);
    void loadP(int id);
    void save(int id);
    bool changed();
    bool isConsistent();
    void addEmptyRows(int c);
    void removeEndRow();
    void removeRows();
    void switchDateType(int dt);
    void setDefaultEmployeeID(int i);
    QList<cDateItem*> getListDates();

public slots:
    void slotTherapyDel();
    void slotTherapyDelAll();

private slots:
    void slotContextMenu();
    void slotChanged();
    void slotDateChanged(int row);
};

#endif /*THERAPY_H_*/
