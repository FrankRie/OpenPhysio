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

#ifndef OPA_H
#define OPA_H

#include <QtGui>
#include <QtXml>
#include <QtSql>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QTreeWidgetItem>
#include <QtWidgets/QMessageBox>
#include "confbase.h"

namespace Ui {
    class opa;
}


class opa : public QMainWindow {
    Q_OBJECT
    Q_DISABLE_COPY(opa)
public:
    enum modes { MODE_DEFAULT = 0, MODE_MIGRATE };
    explicit opa(QWidget *parent = 0, int m = MODE_DEFAULT);
    virtual ~opa();
    void setMode(int m);
    bool migrate(cDatabase d, QTextEdit *te);

protected:
    void keyPressEvent(QKeyEvent *event);

private:
    Ui::opa *m_ui;
    int curArea;
    int mode;
    int db_version;
    int conf_db_id;
    QList<ConfBase*> confs;
    QSqlDatabase db_handle;

    ConfBase* selectArea(int a);
    void deleteArea();
    void loadArea(int a);
    void displayHelp();
    bool migrate_sqlite(cDatabase &d, QTextEdit *te);
    bool migrate_mysql(cDatabase &d, QTextEdit *te);

private slots:
    void slotAreaSelected(QTreeWidgetItem *w);
    void slotSaveAll();
    void slotSaveLocal();
    void slotAbout();
    void slotHelp();
    void slotDBConfChange();
    void slotMigrate(cDatabase *cdb, QTextEdit *te);
};

#endif // OPA_H
