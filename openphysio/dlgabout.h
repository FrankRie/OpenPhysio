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

#ifndef DLGABOUT_H
#define DLGABOUT_H

#include <QtGui>
#include "ui_dlgabout.h"

class DlgAbout : public QDialog
{
    Q_OBJECT

public:
    DlgAbout(QWidget *parent = 0);
    ~DlgAbout();
    QString getAboutText();
    void setAboutText(QString s);

private:
    Ui::DlgAboutClass ui;
};

#endif // DLGABOUT_H
