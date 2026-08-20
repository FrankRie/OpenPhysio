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

#ifndef DLGDATEMOVE_H
#define DLGDATEMOVE_H

#include <QDialog>
#include "mydb.h"
#include "datetools.h"


namespace Ui {
    class DlgDateMove;
}


class DlgDateMove : public QDialog {
    Q_OBJECT
public:
    DlgDateMove(QWidget *parent = 0);
    ~DlgDateMove();
    void init(cDateItem& cdi);

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private:
    Ui::DlgDateMove *ui;
    DateEdit *dateEdit;
    TimeEdit *timeEdit;
    cDateItem new_cdi;

private slots:
    void slotCancel();
    void slotSave();
};

#endif // DLGDATEMOVE_H
