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

#ifndef CONFDOCS_H
#define CONFDOCS_H

#include "confbase.h"

namespace Ui {
    class ConfDocs;
}

class ConfDocs : public ConfBase {
    Q_OBJECT
public:
    explicit ConfDocs(QWidget *parent = 0, int l = -1, int a = -1);
    virtual ~ConfDocs();
    void init();
    bool write();

private:
    Ui::ConfDocs *m_ui;

private slots:
    void slotEditor();
};

#endif // CONFDOCS_H
