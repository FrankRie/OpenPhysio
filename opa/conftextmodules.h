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

#ifndef CONFTEXTMODULES_H
#define CONFTEXTMODULES_H

#include "confbase.h"

namespace Ui {
    class ConfTextModules;
}

class ConfTextModules : public ConfBase
{
    Q_OBJECT

public:
    explicit ConfTextModules(QWidget *parent = 0, int l = -1, int a = -1);
    ~ConfTextModules();
    void init();
    bool write();

private:
    Ui::ConfTextModules *ui;
    QList<cTableItem *> text_modules;
};

#endif // CONFTEXTMODULES_H
