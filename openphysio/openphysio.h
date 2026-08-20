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

#ifndef OPENPHYSIO_H
#define OPENPHYSIO_H

#include <QtWidgets>
#include "ui_openphysio.h"
#include "browsepatients.h"
#include "browseemployees.h"
#include "browsereceipes.h"
#include "browsephysicians.h"
#include "browsepraxis.h"
#include "browsecourses.h"
#include "browsedocs.h"
#include "browsecashpos.h"
#include "dlgempl.h"
#include "dlgabout.h"
#include "protect.h"
#include "calendarwidget.h"


class openphysio : public QMainWindow
{
    Q_OBJECT

public:
    openphysio(QWidget *parent = 0);
    ~openphysio();

private:
    enum views { VIEW_KARTEI, VIEW_DATES };
    Ui::openphysioClass ui;
    BrowsePatients *browsePatients;
    BrowseEmployees *browseEmployees;
    BrowseReceipes *browseReceipes;
    BrowsePhysicians *browsePhysicians;
    BrowsePraxis *browsePraxis;
    BrowseCourses *browseCourses;
    BrowseDocs *browseDocs;
    BrowseCashPos *browseCashPos;
    CalendarWidget *calWidget;
    QToolButton *btnNewItem; //, *btnView;
    QMenu* menuNewItem;
    int view_state;
    CalendarDockWidget *dock;

    void buildMenu();
    void buildToolBars();
    void updateStatusLabel();
    void storeState();

protected:
    void keyPressEvent(QKeyEvent *event);
    void closeEvent(QCloseEvent *event);

private slots:
    void slotView();
    void slotAbout();
    void slotRole();
    void slotBill();
    void slotStatistics();
    void slotBackup();
    void slotHelp();
    void slotManageEmployees();
    void slotToolBarLocationChanged(Qt::Orientation orientation);
    void slotQuit();
};

#endif // OPENPHYSIO_H
