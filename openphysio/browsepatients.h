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

#ifndef BROWSEPATIENTS_H
#define BROWSEPATIENTS_H

#include <QtGui>
#include "ui_browsepatients.h"
#include "browse.h"
#include "dlgpatient.h"
#include "kvkread.h"
#include "browsereceipes.h"


class BrowsePatients : public Browse
{
    Q_OBJECT
public:
    BrowsePatients(int mode = MODE_FULL, QWidget *parent = 0);
    virtual ~BrowsePatients();
    void showDlg(cPatient *p);
    void setupDefaultButtons();
    cPatient& getSelPatient();
    QList<int> getSelectedPatients();
    
private:
    Ui::BrowsePatientsClass ui;
    TableViewAnimated *tvPatient;
    QList<int> listSelPatients;
    QAction *actPatientAdd, *actPatientEdit, *actPatientDel, *actViewReceipes;
    QAction *actViewFindings, *actViewReports;
    QAction *actAcceptSelection;
    QCheckBox *cbFilterActive;
    cPatient selPatient;
    KvkRead *kread;
    enum data { DATA_RECEIPES, DATA_FINDINGS, DATA_REPORTS };
    void showPatientData(int i);

signals:
    void sigPatientSelected(int id);

public slots:
    void slotReadChipcard();
    void slotPatientAdd();

private slots:
    void slotPatientSelected();
    void slotPatientEdit();
    void slotPatientDel();
    void slotViewReceipes();
    void slotViewFindings();
    void slotViewReports();
    void slotSelectMulti();
    void slotFilterPatients(bool b);
};

#endif // BROWSEPATIENTS_H
