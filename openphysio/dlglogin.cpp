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

#include "dlglogin.h"
#include "ui_dlglogin.h"
#include "config.h"


DlgLogin::DlgLogin(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::DlgLogin)
{
    m_ui->setupUi(this);

    connect(m_ui->pbCancel, &QPushButton::clicked, this, &DlgLogin::slotCancel);
    connect(m_ui->pbLogin, &QPushButton::clicked, this, &DlgLogin::slotLogin);
}


DlgLogin::~DlgLogin()
{
    while (!roles.isEmpty()) delete roles.takeFirst();
    delete m_ui;
}


bool DlgLogin::init()
{
    // read surgeries and users from DB and fill dialog elements
    if (!db.retrieveItemList(myDB::TABLE_ROLES, roles)) return false;

    int dr = myConf.getDefaultRole();
    if (dr < 0) dr = 0;
    int idx = 0;
    for (int i = 0; i < roles.count(); i++) {
	m_ui->cbRole->addItem(((cRole*)(roles.at(i)))->getName());
        if (dr == ((cRole*)(roles.at(i)))->getID()) idx = i;
    }
    m_ui->cbRole->setCurrentIndex(idx);
    cSurgery surg = db.getSurgery();
    QString s = surg.getName1();
    if (!surg.getCity().isEmpty()) s += ", " + surg.getCity();
    m_ui->cbSurgery->addItem(s);

    return true;
}


void DlgLogin::slotCancel()
{
    this->reject();
}


void DlgLogin::slotLogin()
{
    int i = m_ui->cbRole->currentIndex();
    if ( (i < 0) || (i >= roles.count()) ) return;

    QByteArray hash = QCryptographicHash::hash(m_ui->lePW->text().toUtf8(),QCryptographicHash::Md5);
    if (!(hash.toHex() == ((cRole*)(roles.at(i)))->getPW())) {
    QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), "Falsches Passwort", QMessageBox::Ok);
    return;
    }

    this->accept();
}


bool DlgLogin::getSelectedRole(cRole& role)
{
    int i = m_ui->cbRole->currentIndex();
    if ( (i < 0) || (i >= roles.count()) ) {
	return false;
    } else {
	role = *(cRole*)(roles.at(i));
	return true;
    }
}


/*bool DlgLogin::getSelectedSurgery(cSurgery& surgery)
{
    int i = m_ui->cbSurgery->currentIndex();
    if ( (i < 0) || (i >= surgeries.count()) ) {
	return false;
    } else {
	surgery = *surgeries.at(i);
	return true;
    }
}*/

