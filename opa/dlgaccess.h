/****************************************************************************
**
** Copyright (C) 2013 Sebastian Vandersee
** All rights reserved.
**
** This file is part of OpenPhysio.
**
** Please refer to the LICENSE file for terms of use.
**
****************************************************************************/

#ifndef DLGACCESS_H
#define DLGACCESS_H

#include <QDialog>

namespace Ui {
    class DlgAccess;
}

class DlgAccess : public QDialog {
    Q_OBJECT
public:
    DlgAccess(QWidget *parent = 0);
    ~DlgAccess();
    static bool isPasswordSet();

private:
    Ui::DlgAccess *ui;

private slots:
    void slotCancel();
    void slotCheck();
};

#endif // DLGACCESS_H
