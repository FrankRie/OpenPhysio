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

#ifndef PROTECT_H_
#define PROTECT_H_

#include "config.h"
#include "mydb.h"


class Protect
{
private:
    int curRole;
    QList<cTableItem *> protFuncs;
    QList<cTableItem *> roles;

public:
    enum funcs { FUNC_PAT = 0, FUNC_PAT_DEL, FUNC_READCHIP, FUNC_REC, FUNC_REC_DEL, FUNC_COU, FUNC_COU_DEL,
                FUNC_EMP, FUNC_EMP_DEL, FUNC_PHY, FUNC_PHY_DEL, FUNC_INV, FUNC_INV_DEL, FUNC_REM, FUNC_REM_DEL,
                FUNC_QUITT, FUNC_QUITT_DEL, FUNC_REP,
		FUNC_FIND, FUNC_PLAN, FUNC_BILL, FUNC_FUNC_CONFIG, FUNC_UPDATE, FUNC_PRICES, FUNC_INS,
                FUNC_BAK, FUNC_STAT, FUNC_CASH_ACCESS, FUNC_CASH_TAKE };
    Protect();
    virtual ~Protect();
    void init();
    void setRole(int role);
    int getRole();
    QString getRoleDescription();
    bool isAccessable(int func);
    bool selectRole();
};

extern Protect protect;
#define GUARD(func) if (!protect.isAccessable((func))) { QMessageBox::warning(this, tr("OpenPhysio"), QString::fromUtf8("Sie haben für diese Funktion keine Berechtigung!"), QMessageBox::Ok); return;}
#define GUARD_BOOL(func) if (!protect.isAccessable((func))) { QMessageBox::warning(this, tr("OpenPhysio"), QString::fromUtf8("Sie haben für diese Funktion keine Berechtigung!"), QMessageBox::Ok); return false;}
#define GUARD_SILENT(func) if (!protect.isAccessable((func))) { return;}
#define GUARD_SILENT_BOOL(func) if (!protect.isAccessable((func))) { return false;}


#endif /*PROTECT_H_*/
