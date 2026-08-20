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

#ifndef CONFOTHER_H
#define CONFOTHER_H

#include "confbase.h"

namespace Ui {
    class ConfOther;
}

class ConfOther : public ConfBase {
    Q_OBJECT
public:
    ConfOther(QWidget *parent = 0, int l = -1, int a = -1);
    ~ConfOther();
    void init();
    bool write();

private:
    Ui::ConfOther *ui;
};

#endif // CONFOTHER_H
