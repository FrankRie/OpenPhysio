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

#ifndef CONFDATABASE_H
#define CONFDATABASE_H

#include "confbase.h"

namespace Ui {
    class ConfDatabase;
}

class ConfDatabase : public ConfBase {
    Q_OBJECT
    Q_DISABLE_COPY(ConfDatabase)
public:
    explicit ConfDatabase(QWidget *parent = 0, int l = -1, int a = -1);
    virtual ~ConfDatabase();
    void init();
    bool write();
    void enableMigrationMode(bool b);
    QString getBackupPath();

signals:
    void sigConfDatabase();
    void sigMigrate(cDatabase *cdb, QTextEdit *te);

private:
    Ui::ConfDatabase *m_ui;
    bool migration_mode;
    cDatabase migration_cdb;

private slots:
    void slotTestConnection();
    void slotSetupChanged(int i);
    void slotMigrate();
    void slotBackupPath();
};

#endif // CONFDATABASE_H
