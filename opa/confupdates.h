/****************************************************************************
**
** Copyright (C) 2013 Sebastian Vandersee
** All rights reserved.
**
** This file is part of OpenPhysio.
**
** Please refer to the LICENSE file for terms of use.
**
****************************************************************************/

#ifndef CONFUPDATES_H
#define CONFUPDATES_H

#include "confbase.h"

namespace Ui {
    class ConfUpdates;
}



class ConfUpdates : public ConfBase {
    Q_OBJECT
    Q_DISABLE_COPY(ConfUpdates)
public:
    explicit ConfUpdates(QWidget *parent = 0, int l = - 1, int a = -1);
    virtual ~ConfUpdates();
    void init();
    bool write();

private:
    Ui::ConfUpdates *m_ui;
    QStringList listInsFiles;


private slots:
    void slotInsurances();
    void slotSelectInsuranceFile();
};

#endif // CONFUPDATES_H
