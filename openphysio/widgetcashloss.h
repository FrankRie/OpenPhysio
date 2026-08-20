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

#ifndef WIDGETCASHLOSS_H
#define WIDGETCASHLOSS_H

#include <QtWidgets>
#include "browsepatients.h"


namespace Ui {
    class WidgetCashLoss;
}

class WidgetCashLoss : public QWidget {
    Q_OBJECT
public:
    WidgetCashLoss(QWidget *parent = 0);
    ~WidgetCashLoss();

signals:
    void sigAmountChanged(QString amount, int id);

private:
    Ui::WidgetCashLoss *m_ui;
    BrowsePatients *browsePatients;
    int cur_pid;

private slots:
    void slotPatientSelected(int pid);
    void slotCountChanged(int i);
    void slotAmountChanged(double d);
};

#endif // WIDGETCASHLOSS_H
