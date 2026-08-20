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

#ifndef DLGRECEIPEPROF_H
#define DLGRECEIPEPROF_H

#include <QtWidgets>

namespace Ui {
    class DlgReceipeProf;
}

class DlgReceipeProf : public QDialog
{
    Q_OBJECT

public:
    explicit DlgReceipeProf(QWidget *parent = 0);
    ~DlgReceipeProf();

private:
    Ui::DlgReceipeProf *ui;

private slots:
    void slotPhysio();
    void slotErgo();
    void slotLogo();
    void slotCancel();
};

#endif // DLGRECEIPEPROF_H
