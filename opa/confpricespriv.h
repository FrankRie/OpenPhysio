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

#ifndef CONFPRICESPRIV_H
#define CONFPRICESPRIV_H

#include "confbase.h"

namespace Ui {
    class ConfPricesPriv;
}

class ConfPricesPriv : public ConfBase {
    Q_OBJECT
    Q_DISABLE_COPY(ConfPricesPriv)
public:
    explicit ConfPricesPriv(QWidget *parent = 0, int l = -1, int a = -1);
    virtual ~ConfPricesPriv();
    void init();
    bool write();

private:
    Ui::ConfPricesPriv *m_ui;
    cPriceTablePriv cpt;
    void loadTable(int i);
    void cleanup(bool re_init);

private slots:
    void slotSave();
    void slotAdd();
    void slotDel();
    void slotUp();
    void slotDown();
    void slotTableChanged(int);
    void slotModified();
};

#endif // CONFPRICESPRIV_H
