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

#ifndef DLGCOURSE_H
#define DLGCOURSE_H

#include <QtWidgets>
#include "ui_dlgcourse.h"
#include "mydb.h"
#include "datetools.h"


class MemberItem: public QListWidgetItem
{
private:
    int custom_data;
    cPatient patient;

public:
    MemberItem(QListWidget *parent = 0);
    MemberItem(QString & text, QListWidget *parent = 0, int type = 0);
    ~MemberItem();
    int getPatientID();
    void setPatient(cPatient p);
    cPatient getPatient();
};


class DlgCourse : public QDialog
{
    Q_OBJECT

public:
    DlgCourse(QWidget *parent = 0);
    ~DlgCourse();
    void initDlg(cCourse *c);

private:
    Ui::DlgCourseClass ui;
    QList<cDateItem* > list_dates;
    QList<cEmployee*> list_empl;
    bool flag_change;
    cCourse curCourse;
    bool bMemberTable;
    int curDateOffset;
    TimeEdit *teClock, *teFrom, *teTo;
    DateEdit *deStart, *dateEdit;

    void loadCourse(cCourse *c);
    bool saveCourse();
    void buildDateTree();
    void buildMemberTable();
    void delDate(QTreeWidgetItem *item);
    int findItemID(QTreeWidgetItem *item);
    void checkInput();

public slots:
    virtual void reject();

private slots:
    void slotSave();
    void slotInvoice();
    void slotAddPatient();
    void slotDelPatient();
    void slotParticipance();
    void slotPartList();
    void slotAddDate();
    void slotDelDate();
    void slotDateSelected(QTreeWidgetItem *item);
    void slotParticipanceChanged(int row, int column);
    void slotLeader(int i);
    void slotModified();
    void slotModifiedWithCheck();
    void slotEditDate(QTreeWidgetItem *item);
    void slotContextMenu(const QPoint& pos);

protected:
    virtual void showEvent(QShowEvent * event);
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // DLGCOURSE_H
