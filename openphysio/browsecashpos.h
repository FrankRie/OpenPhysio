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

#ifndef BROWSECASHPOS_H
#define BROWSECASHPOS_H

#include <QtWidgets>
#include "mydb.h"
#include "browsecash.h"
#include "browsereceipes.h"
#include "widgetcashloss.h"
#include "widgetcasharticle.h"


namespace Ui {
    class BrowseCashPos;
}

class BrowseCashPos : public QWidget {
    Q_OBJECT
public:
    BrowseCashPos(QWidget *parent = 0);
    ~BrowseCashPos();
    void Preload();

private:
    Ui::BrowseCashPos *m_ui;
    int cur_view;
    int cur_id;
    QList<QTreeWidgetItem *> items;
    QWidget *last_inserted_widget;
    BrowseCash *browseCash;
    BrowseReceipes *browseReceipesAddPay;
    BrowseReceipes *browseReceipesSelfPay;
    BrowseReceipes *browseReceipesPrivReceipe;
    WidgetCashLoss *widgetCashLoss;
    WidgetCashArticle *widgetCashArticle;
    void removeCurrentView();
    void loadCashPos(int i);

private slots:
    void slotSelected(QTreeWidgetItem *w);
    void slotCashPos();
    void slotAmountChanged(QString amount, int id);
};

#endif // BROWSECASHPOS_H
