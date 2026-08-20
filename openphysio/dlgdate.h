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

#ifndef DLGDATE_H
#define DLGDATE_H

#include <QtWidgets>
#include "ui_dlgdate.h"
#include "mydb.h"
#include "datetools.h"


class ActivityCheckBox : public QCheckBox
{
private:
    int a_id;
public:
    ActivityCheckBox(int i, QWidget * parent = 0);
    void setActivityID(int i);
    int getActivityID();
};


class DlgDate : public QDialog
{
    Q_OBJECT

public:
    enum modes { MODE_FULL = 0, MODE_EDIT_COURSE };
    DlgDate(QWidget *parent = 0);
    ~DlgDate();
    void init(cDateItem& cdi);
    void setMode(int m);
    cDateItem getDateItem();

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private:
    Ui::DlgDateClass ui;
    int curMode;
    bool flag_change;
    cReceipe curReceipe;
    cPatient curPatient;
    cCourse curCourse;
    cDateItem curDateItem;
    cMetaDate curMetaDate;
    QList<cEmployee*> list_empl;
    QList<ActivityCheckBox*> cbPhysioActs;
    QList<ActivityCheckBox*> cbErgoActs;
    QGridLayout *grid_physio;
    QGridLayout *grid_ergo;
    DateEdit *deStart, *deEnd;
    TimeEdit *teStart, *teEnd;
    bool date_deleted;
    bool show_fullname;

    void setupTitle();
    bool saveDate();
    void setupContact();
    void setDefaultEmployee(int eid);
    void createActivities();
    void enableActivities(bool b);
    void updateActivities();

private slots:
    void slotCancel();
    void slotSave();
    void slotReceipe();
    void slotPatient();
    void slotCourse();
    void slotSwitchDateType(int i);
    void slotFullDay(int i);
    void slotModified();
    void slotStartDateChanged(const QDate &d);
    void slotEndDateChanged(const QDate &d);
    void slotFollowUp();
    void slotEditPatient();
    void slotEditReceipe();
    void slotDelReceipe();
    void slotDelPatient();
    void slotTextModule();
    void slotDelDate();

public slots:
    void reject();
};

#endif // DLGDATE_H
