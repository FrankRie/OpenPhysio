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

#ifndef DLGBACKUP_H
#define DLGBACKUP_H

#include <QtWidgets>

namespace Ui {
    class DlgBackup;
}

class DlgBackup : public QDialog {
    Q_OBJECT
public:
    DlgBackup(QWidget *parent = 0);
    ~DlgBackup();

private:
    Ui::DlgBackup *m_ui;

private slots:
    void slotBackup();
    void slotCancel();
};

#endif // DLGBACKUP_H
