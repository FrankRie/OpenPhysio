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

#ifndef CONFPLANNERDISPLAY_H
#define CONFPLANNERDISPLAY_H

#include "confbase.h"

namespace Ui {
    class ConfPlannerDisplay;
}

class ConfPlannerDisplay : public ConfBase {
    Q_OBJECT
public:
    ConfPlannerDisplay(QWidget *parent = 0, int l = -1, int a = -1);
    ~ConfPlannerDisplay();
    void init();
    bool write();

private:
    Ui::ConfPlannerDisplay *ui;

private slots:
    void slotColor();
};

#endif // CONFPLANNERDISPLAY_H
