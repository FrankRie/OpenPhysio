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

#ifndef BROWSEREPORTS_H
#define BROWSEREPORTS_H

#include <QtGui>
#include "browse.h"

namespace Ui {
    class BrowseReports;
}

class BrowseReports : public Browse
{
    Q_OBJECT
    Q_DISABLE_COPY(BrowseReports)
public:
    explicit BrowseReports(int mode = MODE_FULL, QWidget *parent = 0);
    virtual ~BrowseReports();

private:
    Ui::BrowseReports *m_ui;
    QAction *actReportView, *actReportDel;

private slots:
    void slotReportSelected();
    void slotReportDel();
};

#endif // BROWSEREPORTS_H
