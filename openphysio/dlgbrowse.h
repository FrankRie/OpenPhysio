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

#ifndef DLGBROWSE_H
#define DLGBROWSE_H

#include <QDialog>
#include "browse.h"

namespace Ui {
    class DlgBrowse;
}

class DlgBrowse : public QDialog {
    Q_OBJECT
public:
    DlgBrowse(Browse *b, QWidget *parent = 0);
    ~DlgBrowse();

private:
    Ui::DlgBrowse *ui;
    Browse *browse;
    QGridLayout *gridLayout;
    void generateDlgTitle();

private slots:
    void slotSelected();
};

#endif // DLGBROWSE_H
