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

#ifndef BROWSERECEIPES_H
#define BROWSERECEIPES_H

#include <QtGui>
#include "ui_browsereceipes.h"
#include "browse.h"
//#include "dlgreceipe.h"
//#include "dlgreceipepriv.h"

class BrowseReceipes : public Browse
{
    Q_OBJECT
public:
    BrowseReceipes(int mode = MODE_FULL, QWidget *parent = 0);
    ~BrowseReceipes();
    void showDlg(int t);
    cReceipe& getSelReceipe();
    void setCurPatient(cPatient& p);

signals:
    void sigAmountChanged(QString amount, int id);

private:
    Ui::BrowseReceipesClass ui;
    TableViewAnimated *tvReceipes;
    cReceipe selReceipe;
    QAction *actReceipeAdd, *actReceipePrivateAdd, *actReceipeEdit, *actReceipeDel, *actReceipeCopy;
    cPatient curPatient;

public slots:
    void slotReceipeAdd();
    void slotReceipePrivateAdd();

private slots:
    void slotReceipeEdit();
    void slotReceipeDel();
    void slotReceipeCopy();
    void slotReceipeSelected();
    void slotAmountChanged(const QModelIndex& index);
};

#endif // BROWSERECEIPES_H
