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

#include "openphysio.h"
#include "config.h"
#include "mydb.h"
//#include "dlgbill1.h"
//#include "dlgbill2.h"
#include "msgbox.h"
#include "helperwrapper.h"
#include "dlgbackup.h"


openphysio::openphysio(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    view_state = VIEW_KARTEI;

    browsePatients = new BrowsePatients();
    browseEmployees = new BrowseEmployees();
    browseReceipes = new BrowseReceipes();
    browsePhysicians = new BrowsePhysicians();
    browsePraxis = new BrowsePraxis();
    browseCourses = new BrowseCourses();
	
    QGridLayout *qgl;
    qgl = (QGridLayout *) ui.tabPatients->layout();
    if (qgl != NULL) qgl->addWidget(browsePatients, 0, 0, 1, 6);

    qgl = (QGridLayout *) ui.tabEmployees->layout();
    if (qgl != NULL) qgl->addWidget(browseEmployees, 0, 0, 1, 6);

    qgl = (QGridLayout *) ui.tabReceipes->layout();
    if (qgl != NULL) qgl->addWidget(browseReceipes, 0, 0, 1, 6);

    qgl = (QGridLayout *) ui.tabCourses->layout();
    if (qgl != NULL) qgl->addWidget(browseCourses, 0, 0, 1, 6);

    qgl = (QGridLayout *) ui.tabPhysicians->layout();
    if (qgl != NULL) qgl->addWidget(browsePhysicians, 0, 0, 1, 6);

    qgl = (QGridLayout *) ui.tabPraxis->layout();
    if (qgl != NULL) qgl->addWidget(browsePraxis, 0, 0, 1, 6);

    browseDocs = new BrowseDocs();
    //browseDocs->Reload();
    qgl = (QGridLayout *) ui.tabDocs->layout();
    if (qgl != NULL) qgl->addWidget(browseDocs, 0, 0, 1, 6);

    browseCashPos = new BrowseCashPos();
    qgl = (QGridLayout *) ui.tabCash->layout();
    if (qgl != NULL) qgl->addWidget(browseCashPos, 0, 0, 1, 6);

    dock = new CalendarDockWidget(this);
    dock->setObjectName("CalendarDockWidget");
    dock->setAllowedAreas(Qt::AllDockWidgetAreas);
    dock->setFloating(false);
    dock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetClosable);
    addDockWidget(Qt::RightDockWidgetArea, dock);

    calWidget = new CalendarWidget(dock, ui.mainPanel);
    calWidget->restoreRowHeight();
    calWidget->hide();

    qgl = (QGridLayout *) ui.mainPanel->layout();
    if (qgl != NULL) qgl->addWidget(calWidget, 0, 0);
    connect(calWidget, SIGNAL(sigManageEmployees()), this, SLOT(slotManageEmployees()));

    buildMenu();
    buildToolBars();
    calWidget->enableToolBar(false);
    dock->hide();

    if ((bool)myConf.get(ConfXML::AREA_PLANNER, "planstart").toInt()) slotView();

    updateStatusLabel();

    helpWrapper.init();

    if (myConf.usePreload()) {
        browsePatients->preload();
        browseReceipes->preload();
        browseCourses->preload();
        browseEmployees->preload();
        browsePhysicians->preload();
        browsePraxis->preload();
        browseDocs->Preload();
        browseCashPos->Preload();
    }
}


openphysio::~openphysio()
{
    if (calWidget != NULL) delete calWidget;
    if (browseDocs != NULL) delete browseDocs;
    if (browsePatients != NULL) delete browsePatients;
    if (browseEmployees != NULL) delete browseEmployees;
    if (browseReceipes != NULL)  delete browseReceipes;
    if (browsePhysicians != NULL) delete browsePhysicians;
    if (browsePraxis != NULL) delete browsePraxis;
    if (browseCourses != NULL) delete browseCourses;
    if (browseCashPos != NULL) delete browseCashPos;
}


void openphysio::buildMenu()
{
    // connect menu items
    connect(ui.actionQuit, SIGNAL (triggered()), this, SLOT(slotQuit()));
    connect(ui.actionAbout, SIGNAL (triggered()), this, SLOT(slotAbout()));
    connect(ui.actionHelp, SIGNAL (triggered()), this, SLOT(slotHelp()));
    connect(ui.actionSwitchView, SIGNAL(triggered()), this, SLOT(slotView()));
    connect(ui.actionBackup, SIGNAL(triggered()), this, SLOT(slotBackup()));
    connect(ui.actionStatistics, SIGNAL(triggered()), this, SLOT(slotStatistics()));
    connect(ui.actionReceipes, SIGNAL(triggered()), this, SLOT(slotBill()));

    connect(ui.actionChangeRole, SIGNAL(triggered()), this, SLOT(slotRole()));
    connect(ui.actionNewPatient, SIGNAL(triggered()), browsePatients, SLOT(slotPatientAdd()));
    connect(ui.actionNewReceipe, SIGNAL(triggered()), browseReceipes, SLOT(slotReceipeAdd()));
    connect(ui.actionNewReceipePrivate, SIGNAL(triggered()), browseReceipes, SLOT(slotReceipePrivateAdd()));
    connect(ui.actionNewEmployee, SIGNAL(triggered()), browseEmployees, SLOT(slotEmplAdd()));
    connect(ui.actionNewPhysician, SIGNAL(triggered()), browsePhysicians, SLOT(slotPhysAdd()));
    //connect(ui.actionNewPraxis, SIGNAL(triggered()), browsePraxis, SLOT(slotPraxisAdd()));
    connect(ui.actionNewCourse, SIGNAL(triggered()), browseCourses, SLOT(slotCourseAdd()));
    connect(ui.actionNewDate, SIGNAL(triggered()), calWidget, SLOT(slotDateNew()));
}


void openphysio::buildToolBars()
{
    ui.mainToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    ui.mainToolBar->setIconSize(QSize(20, 20));
    ui.menuView->addAction(ui.mainToolBar->toggleViewAction());

    QMenu* menuNewItem = new QMenu();
    menuNewItem->addAction(QIcon(":/new.png"), "Kassenrezept", browseReceipes, SLOT(slotReceipeAdd()));
    menuNewItem->addAction(QIcon(":/new_private.png"), "Privatrezept", browseReceipes, SLOT(slotReceipePrivateAdd()));
    menuNewItem->addAction(QIcon(":/termine.png"), "Termin", calWidget, SLOT(slotDateNew()));
    menuNewItem->addAction(QIcon(":/new_user.png"), "Patient", browsePatients, SLOT(slotPatientAdd()));
    menuNewItem->addAction(QIcon(":/new.png"), "Kurs", browseCourses, SLOT(slotCourseAdd()));
    menuNewItem->addAction(QIcon(":/physician_add.png"), "Arzt", browsePhysicians, SLOT(slotPhysAdd()));
    menuNewItem->addAction(QIcon(":/group.png"), "Mitarbeiter", browseEmployees, SLOT(slotEmplAdd()));

    btnNewItem = new QToolButton();
    btnNewItem->setText("Neu .. ");
    btnNewItem->setMenu(menuNewItem);
    btnNewItem->setPopupMode(QToolButton::InstantPopup);
    ui.mainToolBar->addWidget(btnNewItem);
    ui.mainToolBar->addSeparator();
    ui.mainToolBar->addAction(ui.actionSwitchView);
    ui.mainToolBar->addAction(QIcon(":/roles.png"), QString::fromUtf8("Rollenwechsel"), this, SLOT(slotRole()));
    connect(ui.mainToolBar, SIGNAL(orientationChanged(Qt::Orientation)), this, SLOT(slotToolBarLocationChanged(Qt::Orientation)));

    // add the calendar toolbar but hide it right now (should only be visible when calendar widget is displayed)
    addToolBar(calWidget->getToolBar());
    calWidget->getToolBar()->setObjectName("Kalendar-Toolbar");
    ui.menuView->addAction(calWidget->getToolBar()->toggleViewAction());
    ui.menuView->addAction(dock->toggleViewAction());

    QSettings settings("OpenPhysio", "OpenPhysio");
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());
}


//
// keyPressEvent
//
void openphysio::keyPressEvent(QKeyEvent *event)
{
    // map QT Fx keys to the function ID configured by the user
    int hkf = myConf.lookupHotkeyFunction(event->key());
    if (hkf <= 0) return;

    switch(hkf) {
    case ConfXML::HKF_CHIP:  // read chipcard
    	browsePatients->slotReadChipcard();
	break;
    case ConfXML::HKF_RECEIPE:  // create new receipe
    	browseReceipes->showDlg(RECEIPE_TYPE_GKV);
	break;
    case ConfXML::HKF_RECEIPE_PRIV:  // create new private receipe
    	browseReceipes->showDlg(RECEIPE_TYPE_PKV);
    	break;
    case ConfXML::HKF_PLAN:
    	slotView();
    	break;
    case ConfXML::HKF_BACKUP:
    	slotBackup();
    	break;
    case ConfXML::HKF_HELP:
    	slotHelp();
    	break;
    case ConfXML::HKF_DATE:
        calWidget->slotDateNew();
        break;
    case ConfXML::HKF_COURSE:
        browseCourses->slotCourseAdd();
        break;
    case ConfXML::HKF_PHYS:
        browsePhysicians->slotPhysAdd();
        break;
    case ConfXML::HKF_PATIENT:
        browsePatients->slotPatientAdd();
        break;
    default:
	break;
    }
}


void openphysio::slotAbout()
{
    DlgAbout dlgAbout;
    dlgAbout.exec();
}


void openphysio::slotView()
{
    switch(view_state) {
	case VIEW_KARTEI:
	    GUARD(Protect::FUNC_PLAN);
	    view_state = VIEW_DATES;
            ui.actionSwitchView->setText("Kartei");
            ui.actionSwitchView->setIcon(QIcon(":/kartei.png"));
            ui.actionSwitchView->setToolTip("Kartei");
	    ui.tabWidget->hide();
            calWidget->enableToolBar(true);
            dock->show();
            calWidget->show();
	    break;
	case VIEW_DATES:
	    view_state = VIEW_KARTEI;
            ui.actionSwitchView->setText("Terminplaner");
            ui.actionSwitchView->setIcon(QIcon(":/termine.png"));
            ui.actionSwitchView->setToolTip("Terminplaner");
            calWidget->hide();
            dock->hide();
            calWidget->enableToolBar(false);
	    ui.tabWidget->show();
	    break;
	default: break;
    }
}


void openphysio::slotRole()
{
    if (protect.selectRole()) updateStatusLabel();
}


void openphysio::slotBill()
{
    MsgBox::dummy();
/*    DlgBill1 dlgBill1;

again:	
    dlgBill1.exec();
    if (dlgBill1.result() == QDialog::Accepted) {
        DlgBill2 dlgBill2;
        dlgBill2.exec();
        if (dlgBill2.getResult() == DlgBill2::RES_BACK) goto again;
    }
	
    // refresh receipe view in case of any changes
    browseReceipes->Refresh();
*/
}


void openphysio::slotStatistics()
{
    MsgBox::dummy();
}


void openphysio::slotBackup()
{
    GUARD(Protect::FUNC_BAK);
    DlgBackup dlgBackup;
    dlgBackup.exec();
}


void openphysio::slotHelp()
{
    if (view_state == VIEW_DATES)
        helpWrapper.showHelpForKeyword("openphysio_planner");
    else
        helpWrapper.showHelpForKeyword("main");
}


void openphysio::slotManageEmployees()
{
    slotView();
    ui.tabWidget->setCurrentWidget(ui.tabEmployees);
}


void openphysio::updateStatusLabel()
{
    // fill status label with current hotkey config and current role name
    ui.statLab->setText(" Aktuelle Rolle: " + protect.getRoleDescription() + "  ||  " + myConf.getHotkeyLabelString());
}


void openphysio::closeEvent(QCloseEvent *event)
 {
    storeState();
    QMainWindow::closeEvent(event);
 }


void openphysio::slotToolBarLocationChanged(Qt::Orientation orientation)
{
    if (orientation == Qt::Horizontal) {
        ui.mainToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    } else {
        ui.mainToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    }
}


void openphysio::slotQuit()
{
    storeState();
    qApp->quit();
}


void openphysio::storeState()
{
    QSettings settings("OpenPhysio", "OpenPhysio");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    settings.setValue("plannerRowHeight", calWidget->getRowHeight());
}
