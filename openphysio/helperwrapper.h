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

#ifndef HELPERWRAPPER_H
#define HELPERWRAPPER_H

#include <QObject>
#include "dlghelp.h"


class HelperWrapper : public QObject
{
private:
    DlgHelp *dlgHelp;

public:
    HelperWrapper();
    void init();
    void showHelpForKeyword(QString s, bool modal = false);
};

extern HelperWrapper helpWrapper;

#endif // HELPERWRAPPER_H
