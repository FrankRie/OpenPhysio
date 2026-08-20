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


#ifndef CONFSURGERY_H
#define CONFSURGERY_H

#include "confbase.h"

namespace Ui {
    class ConfSurgery;
}

class ConfSurgery : public ConfBase {
    Q_OBJECT
    Q_DISABLE_COPY(ConfSurgery)
public:
    ConfSurgery(QWidget *parent = 0, int l = -1, int a = -1);
    virtual ~ConfSurgery();
    void init();
    bool write();

signals:
    void sigConfDatabase();

private:
    Ui::ConfSurgery *m_ui;
    QList<cTableItem*> list_areas;
    QList<cTableItem*> list_unions;
    QList<cTableItem*> list_branches;
    void loadBranches();

private slots:
    void slotPhysio(int i);
    void slotErgo(int i);
    void slotLogo(int i);
    void slotAddBranch();
    void slotDelBranch();
    // void on_leFax_cursorPositionChanged(int arg1, int arg2);
};

#endif // CONFSURGERY_H
