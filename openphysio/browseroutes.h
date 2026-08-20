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

#ifndef BROWSEROUTES_H
#define BROWSEROUTES_H

#include <QtGui>
#include "ui_browseroutes.h"
#include "browse.h"

class BrowseRoutes : public Browse
{
    Q_OBJECT
public:
    BrowseRoutes(int mode = MODE_FULL, QWidget *parent = 0);
    ~BrowseRoutes();
    int getSelectedRoute();
    
private:
    Ui::BrowseRoutesClass ui;
    int selRoute;
    QAction *actRouteAdd, *actRouteEdit, *actRouteDel;
    
private slots:
    void slotRoute();
    void slotRouteAdd();
    void slotRouteEdit();
    void slotRouteDel();
};

#endif // BROWSEROUTES_H
