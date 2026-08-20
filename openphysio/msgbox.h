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

#ifndef MSGBOX_H
#define MSGBOX_H

#include <QObject>
#include <QtWidgets>
#include <QMessageBox>

class MsgBox : public QObject
{
public:
    enum buttons {BTN_YES, BTN_NO};
    MsgBox();
    int execYesNoBox(QString question, int defButton);
    static void error(QString s);
    static void info(QString s);
    static void dummy();
};

#endif // MSGBOX_H
