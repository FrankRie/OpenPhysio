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

#ifndef BROWSECASH_H
#define BROWSECASH_H

#include "browse.h"

namespace Ui {
    class BrowseCash;
}

class BrowseCash : public Browse {
    Q_OBJECT
public:
    BrowseCash(int mode = MODE_FULL, QWidget *parent = 0);
    ~BrowseCash();

private:
    Ui::BrowseCash *m_ui;
};

#endif // BROWSECASH_H
