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

#ifndef DLGLOGIN_H
#define DLGLOGIN_H

#include <QtWidgets>
#include "mydb.h"


namespace Ui {
    class DlgLogin;
}

class DlgLogin : public QDialog {
    Q_OBJECT
    Q_DISABLE_COPY(DlgLogin)
public:
    explicit DlgLogin(QWidget *parent = 0);
    virtual ~DlgLogin();
    bool init();
    bool getSelectedRole(cRole& role);
    bool getSelectedSurgery(cSurgery& surgery);

private:
    Ui::DlgLogin *m_ui;
    QList<cSurgery *> surgeries;
    QList<cTableItem *> roles;

private slots:
    void slotCancel();
    void slotLogin();
};

#endif // DLGLOGIN_H
