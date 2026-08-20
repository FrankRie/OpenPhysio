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

#ifndef CONFADMIN_H
#define CONFADMIN_H

#include <QWidget>

#include "confbase.h"

namespace Ui {
    class ConfAdmin;
}

class ConfAdmin : public ConfBase {
    Q_OBJECT
public:
    ConfAdmin(QWidget *parent = 0, int l = -1, int a = -1);
    ~ConfAdmin();
    void init();
    bool write();

private:
    Ui::ConfAdmin *ui;

private slots:
    void slotCheckPass();
};

#endif // CONFADMIN_H
