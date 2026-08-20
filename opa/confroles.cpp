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

#include "confroles.h"
#include "ui_confroles.h"


ConfRoles::ConfRoles(QWidget *parent, int l, int a) :
    ConfBase(parent, l, a),
    m_ui(new Ui::ConfRoles)
{
    m_ui->setupUi(this);
    setTitle(QString::fromUtf8("Rollen"));
    setKeyword("opa_roles");

    connect(m_ui->pbAdd, &QPushButton::clicked, this, &ConfRoles::slotAdd);
    connect(m_ui->pbDel, &QPushButton::clicked, this, &ConfRoles::slotDel);
    connect(m_ui->pbEdit, &QPushButton::clicked, this, &ConfRoles::slotEdit);
}


ConfRoles::~ConfRoles()
{
    cleanup(false);
    delete m_ui;
}


void ConfRoles::cleanup(bool re_init)
{
    for (int i = 0; i < roles.count(); i++) delete roles.at(i);
    for (int i = 0; i < funcs.count(); i++) delete funcs.at(i);
    roles.clear();
    funcs.clear();

    if (re_init) {
        disconnect(m_ui->cbDefaultRole, &QComboBox::currentIndexChanged, this, &ConfRoles::slotDefRoleChanged);
        m_ui->listRoles->clear();
        m_ui->cbDefaultRole->clear();
        delRoles.clear();
    }
}


void ConfRoles::init()
{
    flag_change = false;
    cleanup(true);

    db.retrieveItemList(myDB::TABLE_ROLES, roles);
    for (int i = 0; i < roles.count(); i++) {
	QString s = ((cRole*)(roles.at(i)))->getName();
	m_ui->listRoles->addItem(s);
	m_ui->cbDefaultRole->addItem(s);
    }

    // map role ID to combox index
    int ci = myConf.getDefaultRole();
    for (int i = 0; i < roles.count(); i++) {
	if (((cRole*)(roles.at(i)))->getID() == ci) {
	    m_ui->cbDefaultRole->setCurrentIndex(i);
	}
    }

    connect(m_ui->cbDefaultRole, &QComboBox::currentIndexChanged, this, &ConfRoles::slotDefRoleChanged);
}


bool ConfRoles::write()
{
    if (flag_change) {
	flag_change = false;
	int i;

	// delete all roles with ID < 100 contained in delRoles
	for (i = 0; i < delRoles.count(); i++) {
	    if (delRoles.at(i) < 100) {
		db.deleteItem(myDB::TABLE_ROLES, delRoles.at(i));
		// TODO: remove this PID from DB table "protected functions"
		//  and renumber other roles in DB table "roles"
	    }
	}

	// update all roles with ID < 100 contained in roles
	// insert all roles with ID >= 100 contained in roles
	for (i = 0; i < roles.count(); i++) {
	    cRole *r = (cRole*)roles.at(i);
	    if (r->getID() < 100) {
		db.updateItem(*r);
	    } else {
		int id;
		db.storeItem(*r, &id);
	    }
	}

	int ci = m_ui->cbDefaultRole->currentIndex();
	// map ci to role ID
	if ( (ci >= 0) && (ci < roles.count()) ) {
        myConf.set(ConfXML::AREA_ROLES, "default_role", QString::number(((cRole*)roles.at(ci))->getID()));
	}
    }
    return true;
}


void ConfRoles::slotAdd()
{
    QDialog dlg;
    QGridLayout layout(&dlg);

    QLineEdit leName, lePW;
    QLabel l1, l2;
    QPushButton pbs("OK");
    QPushButton pbc("Abbrechen");

    l1.setText("Rollenname:");
    l2.setText("Passwort:");
    layout.addWidget(&pbc, 2, 1, 1, 1);
    layout.addWidget(&pbs, 2, 2, 1, 1);
    layout.addWidget(&l1, 0, 0, 1, 1);
    layout.addWidget(&l2, 1, 0, 1, 1);
    layout.addWidget(&leName, 0, 1, 1, 2);
    layout.addWidget(&lePW, 1, 1, 1, 2);

    QObject::connect(&pbs, &QPushButton::clicked, &dlg, &QColorDialog::accept);
    QObject::connect(&pbc, &QPushButton::clicked, &dlg, &QColorDialog::reject);

    dlg.setWindowTitle(QString::fromUtf8("Neue Rollen hinzufügen"));
    dlg.setWindowIcon(QIcon(":/favicon.png"));
    dlg.exec();
    if (dlg.result() == QDialog::Accepted) {
	flag_change = true;
	// new roles will be temporarly given an ID >= 100
    QByteArray hash = QCryptographicHash::hash(lePW.text().toUtf8() /*  Ascii()*/,QCryptographicHash::Md5);
	roles.append(new cRole(100 + roles.count(), leName.text(), hash.toHex()));
	m_ui->listRoles->clear();
	int ci = m_ui->cbDefaultRole->currentIndex();
	m_ui->cbDefaultRole->clear();
	for (int i = 0; i < roles.count(); i++) {
	    cRole *r = (cRole*)(roles.at(i));
	    //if (!delRoles.contains(roles.at(i)->getID())) // do not display roles marked for deletion
	    m_ui->listRoles->addItem(r->getName());
	    m_ui->cbDefaultRole->addItem(r->getName());
	}
	m_ui->cbDefaultRole->setCurrentIndex(ci);
    }
}


void ConfRoles::slotDel()
{
    int i = m_ui->listRoles->currentRow();
    if (i == 0) {
        QMessageBox::warning(this, tr("OpenPhysio"), QString::fromUtf8("Diese Rolle kann nicht gelöscht werden!\n"), QMessageBox::Ok);
	return;
    }
    if (i > 0) {
	flag_change = true;
	delete m_ui->listRoles->takeItem(i);
	delRoles.append(((cRole*)(roles.at(i)))->getID());
	roles.removeAt(i);
	int ci = m_ui->cbDefaultRole->currentIndex();
	m_ui->cbDefaultRole->clear();
	for (int i = 0; i < roles.count(); i++) {
	    cRole *r = (cRole*)(roles.at(i));
	    //if (!delRoles.contains(roles.at(i)->getID())) // do not display roles marked for deletion
	    m_ui->cbDefaultRole->addItem(r->getName());
	}
	m_ui->cbDefaultRole->setCurrentIndex(ci);
    }
}


void ConfRoles::slotEdit()
{
    int i = m_ui->listRoles->currentRow();
    if (i >= 0) {
	QDialog dlg;
	QGridLayout layout(&dlg);

	QLineEdit leName, lePW;
	QLabel l1, l2;
	QPushButton pbs("OK");
	QPushButton pbc("Abbrechen");

	l1.setText("Neuer Rollenname:");
	l2.setText("Neues Passwort:");
	leName.setText(((cRole*)(roles.at(i)))->getName());
	lePW.setText("");

	layout.addWidget(&pbc, 2, 1, 1, 1);
	layout.addWidget(&pbs, 2, 2, 1, 1);
	layout.addWidget(&l1, 0, 0, 1, 1);
	layout.addWidget(&l2, 1, 0, 1, 1);
	layout.addWidget(&leName, 0, 1, 1, 2);
	layout.addWidget(&lePW, 1, 1, 1, 2);

    QObject::connect(&pbs, &QPushButton::clicked, &dlg, &QColorDialog::accept);
    QObject::connect(&pbc, &QPushButton::clicked, &dlg, &QColorDialog::reject);

        dlg.setWindowTitle(QString::fromUtf8("Rolle bearbeiten"));
	dlg.setWindowIcon(QIcon(":/favicon.png"));
	dlg.exec();
	if (dlg.result() == QDialog::Accepted) {
		flag_change = true;
		((cRole*)(roles.at(i)))->setName(leName.text());
        QByteArray hash = QCryptographicHash::hash(lePW.text().toUtf8() /* Ascii()*/,QCryptographicHash::Md5);
		((cRole*)(roles.at(i)))->setPW(hash.toHex());
	}
    }
}


void ConfRoles::slotDefRoleChanged()
{
    flag_change = true;
}
