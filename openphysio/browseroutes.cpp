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

#include "browseroutes.h"
#include "mydb.h"


BrowseRoutes::BrowseRoutes(int mode, QWidget *parent)
    : Browse(mode, parent)
{
    ui.setupUi(this);
    init((TableViewAnimated*)ui.tvRoutes, myDB::TABLE_ROUTES);

    connect(ui.tvRoutes, SIGNAL(activated(const QModelIndex&)), this, SLOT(slotRoute()));
    selRoute = -1;
	
    ui.tvRoutes->setColumnWidth(1, 400);
	
    actRouteAdd = new QAction(QString::fromUtf8("Neue Route"), this);
    actRouteEdit = new QAction(QString::fromUtf8("Route bearbeiten"), this);
    actRouteDel = new QAction(QString::fromUtf8("Route löschen"), this);
    connect(actRouteAdd, SIGNAL(triggered()), this, SLOT(slotRouteAdd()));
    connect(actRouteEdit, SIGNAL(triggered()), this, SLOT(slotRouteEdit()));
    connect(actRouteDel, SIGNAL(triggered()), this, SLOT(slotRouteDel()));
    ui.tvRoutes->addAction(actRouteAdd);
    ui.tvRoutes->addAction(actRouteEdit);
    ui.tvRoutes->addAction(actRouteDel);
    ui.tvRoutes->setContextMenuPolicy(Qt::ActionsContextMenu);

    //ReloadTable();
}


BrowseRoutes::~BrowseRoutes()
{
    if (actRouteAdd != NULL) delete actRouteAdd;
    if (actRouteEdit != NULL) delete actRouteEdit;
    if (actRouteDel != NULL) delete actRouteDel;
}


void BrowseRoutes::slotRoute()
{
    int id = getSelectedItem();
    if (id < 0) return;
	
    selRoute = id;
    emit sigItemSelected();
}


int BrowseRoutes::getSelectedRoute()
{
    return selRoute;
}


void BrowseRoutes::slotRouteAdd()
{
}


void BrowseRoutes::slotRouteEdit()
{
}


void BrowseRoutes::slotRouteDel()
{
}
