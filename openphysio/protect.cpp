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
#include <QMessageBox>
#include "protect.h"
#include "mydb.h"

Protect protect;


Protect::Protect()
{
    curRole = -1;
}


Protect::~Protect()
{
    while (!protFuncs.isEmpty()) delete protFuncs.takeFirst();
    while (!roles.isEmpty()) delete roles.takeFirst();
}


void Protect::init()
{
    while (!protFuncs.isEmpty()) delete protFuncs.takeFirst();
    while (!roles.isEmpty()) delete roles.takeFirst();
    db.retrieveItemList(myDB::TABLE_PROTECT, protFuncs);
    db.retrieveItemList(myDB::TABLE_ROLES, roles);
}


void Protect::setRole(int role)
{
    curRole = role;
}


int Protect::getRole()
{
    return curRole;
}


QString Protect::getRoleDescription()
{
    if ( (curRole < 0) || (curRole >= roles.count()) ) return "";
    return ((cRole*)(roles.at(curRole)))->getName();
}


bool Protect::isAccessable(int func)
{
    if (curRole == ROLE_ADMIN) return true;
	
    bool bRet = false;
	
    if (curRole < 0) return bRet;
    if ( (func < 0) || (func >= protFuncs.count()) ) return bRet;
	
    // if func is allowed for curRole => return true / else false

    QString s = ((cProtectedFunc *)(protFuncs.at(func)))->getRidList();
    QStringList l = s.split(":");
    //qDebug() << s;
    QString tmp;
    for (int i = 0; i < l.count(); i++) {
	tmp = l.at(i);
	if ( tmp.toInt() == curRole ) {
	    bRet = true;
	    break;
	}
    }
		
    return bRet;
}


bool Protect::selectRole()
{
    QDialog dlg;
    QGridLayout layout(&dlg);
	
    QComboBox cb;
    QLineEdit lePW;
    QLabel l1, l2;
	
    QPushButton pbs("OK");
    QPushButton pbc("Abbrechen");
    pbc.setIcon(QIcon(":/cancel.png"));
    pbs.setIcon(QIcon(":/roles.png"));
    l1.setText("Rolle:");
    l2.setText("Passwort:");
	
    layout.addWidget(&pbc, 2, 1, 1, 1);
    layout.addWidget(&pbs, 2, 2, 1, 1);
    layout.addWidget(&cb, 0, 1, 1, 2);
    layout.addWidget(&lePW, 1, 1, 1, 2);
    layout.addWidget(&l1, 0, 0, 1, 1);
    layout.addWidget(&l2, 1, 0, 1, 1);

    QObject::connect(&pbs, &QPushButton::clicked, &dlg, &QColorDialog::accept);
    QObject::connect(&pbc, &QPushButton::clicked, &dlg, &QColorDialog::reject);
	
    for (int i = 0; i < roles.count(); i++) {
	cRole *r = (cRole*)(roles.at(i));
	cb.addItem(r->getName());
	if (r->getID() == protect.getRole()) cb.setCurrentIndex(i);
    }

    dlg.setWindowTitle(QString::fromUtf8("Neue Rolle wählen"));
    dlg.setWindowIcon(QIcon(":/favicon.png"));
    dlg.exec();
    if (dlg.result() == QDialog::Accepted) {
	if (cb.currentIndex() < 0) return false;
	if (cb.currentIndex() >= roles.count()) return false;
	cRole *r = ((cRole*)(roles.at(cb.currentIndex())));

	if (protect.getRole() == r->getID() ) return false;

    QByteArray hash = QCryptographicHash::hash(lePW.text().toUtf8(),QCryptographicHash::Md5);

	if ( hash.toHex() == r->getPW() ) {
	    protect.setRole(r->getID());
	} else {
	    QMessageBox::warning(NULL, QObject::tr("OpenPhysio"), QObject::tr("Falsches Passwort!"), QMessageBox::Ok);
	}
	return true;
    } else {
	return false;
    }
}
