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

#ifndef CONFDISPLAY_H
#define CONFDISPLAY_H

#include "confbase.h"
#include <QtWidgets/QColorDialog>

namespace Ui {
    class ConfDisplay;
}

class ConfDisplay : public ConfBase {
    Q_OBJECT
    Q_DISABLE_COPY(ConfDisplay)
public:
    explicit ConfDisplay(QWidget *parent = 0, int l = -1, int a = -1);
    virtual ~ConfDisplay();
    void init();
    bool write();

private:
    Ui::ConfDisplay *m_ui;

private slots:
    void slotColor();
};

#endif // CONFDISPLAY_H
