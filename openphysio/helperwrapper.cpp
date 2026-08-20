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

#include "helperwrapper.h"

HelperWrapper helpWrapper;


HelperWrapper::HelperWrapper()
{
    dlgHelp = NULL;
}


void HelperWrapper::init()
{
    if (dlgHelp == NULL) {
	dlgHelp = new DlgHelp();
    }
}


void HelperWrapper::showHelpForKeyword(QString s, bool modal)
{
    if (dlgHelp != NULL) {
        if (!modal) dlgHelp->hide();
	dlgHelp->setKeyword(s);
        if (!modal) dlgHelp->show();
        else dlgHelp->exec();
    }
}
