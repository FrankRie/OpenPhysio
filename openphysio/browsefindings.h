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

#ifndef BROWSEFINDINGS_H
#define BROWSEFINDINGS_H

#include <QtGui>
#include "browse.h"


namespace Ui {
    class BrowseFindings;
}

class BrowseFindings : public Browse
{
    Q_OBJECT
    Q_DISABLE_COPY(BrowseFindings)
public:
    explicit BrowseFindings(int mode = MODE_SELECT, QWidget *parent = 0);
    virtual ~BrowseFindings();
    void setPatientID(int pid);

private:
    Ui::BrowseFindings *m_ui;
    QAction *actFindingView, *actFindingDel, *actFindingAdd;
    int pid;

private slots:
    void slotFindingSelected();
    void slotFindingDel();
    void slotFindingAdd();
};

#endif // BROWSEFINDINGS_H
