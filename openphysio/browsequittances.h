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

#ifndef BROWSEQUITTANCES_H
#define BROWSEQUITTANCES_H

#include <QtGui>
#include "browse.h"

namespace Ui {
    class BrowseQuittances;
}

class BrowseQuittances : public Browse {
//    Q_OBJECT
    Q_DISABLE_COPY(BrowseQuittances)
public:
    BrowseQuittances(int mode = MODE_FULL, QWidget *parent = 0);
    ~BrowseQuittances();

private:
    Ui::BrowseQuittances *m_ui;
    QAction *actQuittanceView, *actQuittanceDel;

private slots:
    void slotQuittanceSelected();
    void slotQuittanceDel();
};

#endif // BROWSEQUITTANCES_H
