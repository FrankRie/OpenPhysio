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

#ifndef CONFBACKUP_H
#define CONFBACKUP_H

#include "confbase.h"

namespace Ui {
    class ConfBackup;
}

class ConfBackup : public ConfBase {
    Q_OBJECT
    Q_DISABLE_COPY(ConfBackup)
public:
    explicit ConfBackup(QWidget *parent = 0, int l = -1, int a = -1);
    virtual ~ConfBackup();
    void init();
    bool write();

private:
    Ui::ConfBackup *m_ui;

private slots:
    void slotSelectBackupDir();
    void slotBackup();
    void slotRecovery();
};

#endif // CONFBACKUP_H
