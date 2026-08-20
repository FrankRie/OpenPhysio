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

#include "confperm.h"
#include "ui_confperm.h"


ConfPerm::ConfPerm(QWidget *parent, int l, int a) :
    ConfBase(parent, l, a),
    m_ui(new Ui::ConfPerm)
{
    m_ui->setupUi(this);
    setTitle("Berechtigungen");
    setKeyword("opa_perms");
}


ConfPerm::~ConfPerm()
{
    cleanup();
    delete m_ui;
}


void ConfPerm::cleanup()
{
    for (int i = 0; i < roles.count(); i++) delete roles.at(i);
    for (int i = 0; i < funcs.count(); i++) delete funcs.at(i);
    roles.clear();
    funcs.clear();
}


void ConfPerm::init()
{
    flag_change = false;
    cleanup();

    db.retrieveItemList(myDB::TABLE_ROLES, roles);

    // remove existing rows and columns
    int a, z;
    a = m_ui->tabPerm->rowCount();
    for (z = 0; z < a; z++) m_ui->tabPerm->removeRow(0);
    a = m_ui->tabPerm->columnCount();
    for (z = 0; z < a; z++) m_ui->tabPerm->removeColumn(0);

    if (db.retrieveItemList(myDB::TABLE_PROTECT, funcs) ) {
	// fill tabPerm
	for (int i = 0; i < roles.count(); i++) {
	    m_ui->tabPerm->insertColumn(i);
	    m_ui->tabPerm->setHorizontalHeaderItem(i, new QTableWidgetItem(((cRole*)(roles.at(i)))->getName()));
	}

	QString s;
	QStringList rid;
	bool flag;
	for (int j = 0; j < funcs.count(); j++) {
	    cProtectedFunc *pf = (cProtectedFunc*)(funcs.at(j));
	    m_ui->tabPerm->insertRow(j);
	    m_ui->tabPerm->setVerticalHeaderItem(j, new QTableWidgetItem(pf->getName()));
	    s = pf->getRidList();
	    rid.clear();
	    rid = s.split(":");
	    flag = false;
	    // TODO: this way of doing it only allows up to 9 different roles !!! (contains ...) => FIX !!!
	    for (int x = 0; x < roles.count(); x++) {
		cRole *r = (cRole*)(roles.at(x));
		QTableWidgetItem* item = new QTableWidgetItem();
		if (rid.contains(QString::number(r->getID()), Qt::CaseInsensitive)) {
		    item->setCheckState(Qt::Checked);
		} else {
		    item->setCheckState(Qt::Unchecked);
		}
        if (r->getID() == ROLE_ADMIN) item->setFlags(Qt::NoItemFlags);
		m_ui->tabPerm->setItem(j, x, item);
	    }
	}
	m_ui->tabPerm->resizeColumnToContents(0);

        connect(m_ui->tabPerm, &QTableWidget::cellChanged, this, &ConfPerm::slotPermChanged);
    }
}


bool ConfPerm::write()
{
    if (flag_change) {
	flag_change = false;

	QString s;
	QStringList l_tmp;
	QStringList l;

	for (int x = 0; x < m_ui->tabPerm->rowCount(); x++) {
	    l_tmp.clear();
	    l_tmp.append(QString::number(ROLE_ADMIN)); // admin role is always included
	    for (int y = 1; y < m_ui->tabPerm->columnCount(); y++) {
		QTableWidgetItem *item = m_ui->tabPerm->item(x, y);
		if (item == NULL) break;
		if (item->checkState() == Qt::Checked) l_tmp.append(QString::number(((cRole*)(roles.at(y)))->getID()));
	    }
	    s = l_tmp.join(":");
	    l.append(s);
	    //qDebug() << s;
	}
	if (!db.updateProtectedFuncs(l)) {
	    QMessageBox::warning(this, tr("OpenPhysio"), tr("Fehler beim Speichern aufgetreten!\n"), QMessageBox::Ok);
	}
    }
    return true;
}


void ConfPerm::slotPermChanged()
{
    flag_change = true;
}
