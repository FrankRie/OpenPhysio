#include "dlgroles.h"
#include "mydb.h"
#include "protect.h"

DlgRoles::DlgRoles(QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);

	connect(ui.pbCancel, SIGNAL(clicked()), this, SLOT(slotCancel()));
	connect(ui.pbSave, SIGNAL(clicked()), this, SLOT(slotSave()));
	connect(ui.pbAdd, SIGNAL(clicked()), this, SLOT(slotAdd()));
	connect(ui.pbDel, SIGNAL(clicked()), this, SLOT(slotDel()));
	connect(ui.pbEdit, SIGNAL(clicked()), this, SLOT(slotEdit()));
	
	db.getRoles(&roles);
	for (int i = 0; i < roles.count(); i++) {
		ui.listRoles->addItem(roles.at(i)->getName());
	}
}

DlgRoles::~DlgRoles()
{

}

void DlgRoles::slotCancel()
{
	this->reject();
}

void DlgRoles::slotSave()
{
	int i;

	// delete all roles with ID < 100 contained in delRoles
	for (i = 0; i < delRoles.count(); i++) {
		if (delRoles.at(i) < 100) {
			db.deleteRole(delRoles.at(i));
			// TODO: remove this PID from DB table "protected functions"
			//  and renumber other roles in DB table "roles"
		}
	}
	
	// update all roles with ID < 100 contained in roles
	// insert all roles with ID >= 100 contained in roles
	for (i = 0; i < roles.count(); i++) {
		if (roles.at(i)->getID() < 100) {
                        db.updateRole(*roles.at(i));
		} else {
                        db.insertRole(*roles.at(i));
		}
	}
	
	this->accept();
}

void DlgRoles::slotAdd()
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

	QObject::connect(&pbs, SIGNAL(clicked()), &dlg, SLOT(accept()));
	QObject::connect(&pbc, SIGNAL(clicked()), &dlg, SLOT(reject()));

	dlg.setWindowTitle("Neue Rollen hinzufügen");
	dlg.setWindowIcon(QIcon("images/favicon.png"));
	dlg.exec();
	if (dlg.result() == QDialog::Accepted) {
		// new roles will be temporarly given an ID >= 100
		roles.append(new cRole(100 + roles.count(), leName.text(), lePW.text()));
		ui.listRoles->clear();
		for (int i = 0; i < roles.count(); i++) {
			//if (!delRoles.contains(roles.at(i)->getID())) // do not display roles marked for deletion
				ui.listRoles->addItem(roles.at(i)->getName());
		}
	}
}

void DlgRoles::slotDel()
{
	int i = ui.listRoles->currentRow();
	if (i == 0) {
		QMessageBox::warning(this, tr("OpenPhysio"), tr("Diese Rolle kann nicht gelöscht werden!\n"), QMessageBox::Ok);
		return;
	}
	if (roles.at(i)->getID() == protect.getRole()) {
		QMessageBox::warning(this, tr("OpenPhysio"), tr("Diese Rolle kann nicht gelöscht werden, da sie derzeit benutzt wird!\n"), QMessageBox::Ok);
		return;
	}
	if (roles.at(i)->getID() == myConf.getDefaultRole()) {
		QMessageBox::warning(this, tr("OpenPhysio"), tr("Diese Rolle kann nicht gelöscht werden, da sie derzeit als Standardardrolle beim Start von OpenPhysio eingerichtet ist!\n"), QMessageBox::Ok);
		return;		
	}
	if (i > 0) {
		delete ui.listRoles->takeItem(i);
		delRoles.append(roles.at(i)->getID());
		roles.removeAt(i);
	}
}

void DlgRoles::slotEdit()
{
	int i = ui.listRoles->currentRow();
	if (i >= 0) {
		QDialog dlg;
		QGridLayout layout(&dlg);
		
		QLineEdit leName, lePW;
		QLabel l1, l2;
		QPushButton pbs("OK");
		QPushButton pbc("Abbrechen");
		
		l1.setText("Neuer Rollenname:");
		l2.setText("Neues Passwort:");
		leName.setText(roles.at(i)->getName());
		lePW.setText(roles.at(i)->getPW());
		
		layout.addWidget(&pbc, 2, 1, 1, 1);
		layout.addWidget(&pbs, 2, 2, 1, 1);
		layout.addWidget(&l1, 0, 0, 1, 1);
		layout.addWidget(&l2, 1, 0, 1, 1);
		layout.addWidget(&leName, 0, 1, 1, 2);
		layout.addWidget(&lePW, 1, 1, 1, 2);

		QObject::connect(&pbs, SIGNAL(clicked()), &dlg, SLOT(accept()));
		QObject::connect(&pbc, SIGNAL(clicked()), &dlg, SLOT(reject()));

		dlg.setWindowTitle("Rolle bearbeiten");
		dlg.setWindowIcon(QIcon("images/favicon.png"));
		dlg.exec();
		if (dlg.result() == QDialog::Accepted) {
			roles.at(i)->setName(leName.text());
			roles.at(i)->setPW(lePW.text());
		}
	}	
}
