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

#ifndef DLGSCHEDULE_H
#define DLGSCHEDULE_H

#include <QtWidgets>
#include "mydb.h"

namespace Ui {
    class DlgSchedule;
}

class DlgSchedule : public QDialog {
    Q_OBJECT
public:
    DlgSchedule(QWidget *parent = 0);
    ~DlgSchedule();
    void init(QDate start_date, QDate end_date, QTime start_time, QTime end_time);

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private:
    Ui::DlgSchedule *ui;
    QList<cEmployee*> list_empl;

private slots:
    void slotCancel();
    void slotCreate();
    void slotMode(int m);
};

#endif // DLGSCHEDULE_H
