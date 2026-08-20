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

#ifndef BROWSEPHYSICIANS_H
#define BROWSEPHYSICIANS_H

#include <QtGui>
#include "ui_browsephysicians.h"
#include "browse.h"
#include "dlgphys.h"


class BrowsePhysicians : public Browse
{
    Q_OBJECT
public:
    BrowsePhysicians(int mode = MODE_FULL, QWidget *parent = 0);
    ~BrowsePhysicians();
    void showDlg(cPhys *p);
    void setupDefaultButtons();
    cPhys& getSelPhysician();
    
private:
    Ui::BrowsePhysiciansClass ui;
    QAction *actPhysAdd, *actPhysEdit, *actPhysDel;
    cPhys selPhys;
        
public slots:
    void slotPhysAdd();

private slots:
    void slotPhysSelected();
    void slotPhysEdit();
    void slotPhysDel();

};

#endif // BROWSEPHYSICIANS_H
