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

#ifndef CONFPERM_H
#define CONFPERM_H

#include "confbase.h"

namespace Ui {
    class ConfPerm;
}

class ConfPerm : public ConfBase {
    Q_OBJECT
    Q_DISABLE_COPY(ConfPerm)
public:
    explicit ConfPerm(QWidget *parent = 0, int l = -1, int a = -1);
    virtual ~ConfPerm();
    void init();
    bool write();

private:
    Ui::ConfPerm *m_ui;
    QList<cTableItem*> funcs;
    QList<cTableItem *> roles;
    void cleanup();

private slots:
    void slotPermChanged();
};

#endif // CONFPERM_H
