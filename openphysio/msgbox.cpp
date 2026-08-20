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

#include <QtWidgets>
#include "msgbox.h"


MsgBox::MsgBox()
{
}


int MsgBox::execYesNoBox(QString question, int defButton)
{
    QMessageBox box;
    box.setText(question);
    box.setWindowTitle(tr("OpenPhysio - Nachfrage"));
    box.setWindowIcon(QIcon(":/favicon.png"));
    box.setIcon(QMessageBox::Question);
    QPushButton *yesButton = box.addButton(tr("Ja"), QMessageBox::YesRole);
    QPushButton *noButton = box.addButton(tr("Nein"), QMessageBox::NoRole);
    switch(defButton) {
	case BTN_YES:
	    box.setDefaultButton(yesButton);
	    break;
	case BTN_NO:
	    box.setDefaultButton(noButton);
	    break;
	default: break;
    }

    box.exec();
    if (box.clickedButton() == yesButton) return BTN_YES;
    else return BTN_NO;
}


void MsgBox::dummy()
{
    QMessageBox::warning(NULL, tr("OpenPhysio - Hinweis"), "Diese Funktion ist noch nicht implementiert!", QMessageBox::Ok);
}


void MsgBox::error(QString s)
{
    QMessageBox::critical(NULL, tr("OpenPhysio - Fehler"), s, QMessageBox::Ok);
}

void MsgBox::info(QString s)
{
    QMessageBox::information(NULL, tr("OpenPhysio - Hinweis"), s, QMessageBox::Ok);
}
