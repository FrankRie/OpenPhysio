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

#ifndef CONFHOTKEYS_H
#define CONFHOTKEYS_H

#include "confbase.h"

namespace Ui {
    class ConfHotkeys;
}

class ConfHotkeys : public ConfBase {
    Q_OBJECT
    Q_DISABLE_COPY(ConfHotkeys)
public:
    explicit ConfHotkeys(QWidget *parent = 0, int l = -1, int a = -1);
    virtual ~ConfHotkeys();
    void init();
    bool write();

private:
    Ui::ConfHotkeys *m_ui;
};

#endif // CONFHOTKEYS_H
