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

#include "conftakeover.h"
#include "ui_conftakeover.h"
#include <mydb.h>
#include <msgbox.h>


ConfTakeover::ConfTakeover(QWidget *parent, int l, int a) :
    ConfBase(parent, l, a),
    m_ui(new Ui::ConfTakeover)
{
    m_ui->setupUi(this);
    setTitle(QString::fromUtf8("Datenübernahme"));
    setKeyword("opa_takeover");

    connect(m_ui->pbStart, &QPushButton::clicked, this, &ConfTakeover::slotStart);
    connect(m_ui->pbDir, &QPushButton::clicked, this, &ConfTakeover::slotSelectDirectory);
}


ConfTakeover::~ConfTakeover()
{
    delete m_ui;
}


void ConfTakeover::init()
{
}


bool ConfTakeover::write()
{
    return true;
}


bool ConfTakeover::handle_sp(QString dir)
{
    QString fname_patients = dir + "/patient.txt";
    QString fname_addr = dir + "/adresse.txt";
    QString fname_phys = dir + "/arzt.txt";
    bool convert_pat, convert_phys;

    convert_pat = convert_phys = true;

    QFile file_addr(fname_addr);
    if (!file_addr.open(QIODevice::ReadOnly)) {
        m_ui->textEdit->append("<b>FEHLER</b>: Datei " + fname_addr + QString::fromUtf8(" konnte nicht geöffnet werden!"));
	return false;
    } else m_ui->textEdit->append("Datei " + fname_addr + " wird gelesen");


    QFile file_patients(fname_patients);
    if (!file_patients.open(QIODevice::ReadOnly)) {
        m_ui->textEdit->append("<b>WARNUNG</b>: Datei " + fname_patients + QString::fromUtf8(" konnte nicht geöffnet werden! => Patientendaten werden nicht übernommen!"));
	convert_pat = false;
    } else m_ui->textEdit->append("Datei " + fname_patients + " wird gelesen");

    QFile file_phys(fname_phys);
    if (!file_phys.open(QIODevice::ReadOnly)) {
        m_ui->textEdit->append("<b>WARNUNG</b>: Datei " + fname_phys + QString::fromUtf8(" konnte nicht geöffnet werden! => Arztdaten werden nicht übernommen!"));
	convert_phys = false;
    } else m_ui->textEdit->append("Datei " + fname_phys + " wird gelesen");

    QTextStream t;
    QString line = "";
    QList<cPatient *> patients;
    QList<cPhys *> physicians;
    cPatient *p;
    cPhys *phys;
    QStringList columns;
    QList<int> col_pos;

    // import patients
    if (convert_pat) {
	t.setDevice(&file_patients);

	// read first line to learn about the columns of patient.txt
	line = t.readLine();
	if (!line.isEmpty()) {
	    columns = line.split(";");
	    for (int i = 0; i < PAT_ATTR_NUMBER; i++) col_pos.append(-1);
	    for (int i = 0; i < columns.count(); i++) {
		QString token = columns.at(i).toUpper();
		if (token == "ADR::NR") col_pos[PAT_ADDR_REF] = i;
		if (token == "NACHNAME") col_pos[PAT_SURNAME] = i;
		if (token == "VORNAME") col_pos[PAT_FORENAME] = i;
		if (token == "GESCHLECHT") col_pos[PAT_GENDER] = i;
		if (token == "TITEL") col_pos[PAT_TITLE] = i;
		if (token == "NAMENSZUSATZ") col_pos[PAT_ADDNAME] = i;
		if (token == "GEBURTSDATUM") col_pos[PAT_BIRTHDAY] = i;
		if (token == "TELEFON") col_pos[PAT_TEL] = i;
		if (token == "VERSICHERTENNUMMER") col_pos[PAT_VNR] = i;
		if (token == "VKGUELTIGKEIT") col_pos[PAT_KVKVALID] = i;
		if (token == "STATUS") col_pos[PAT_STATUS] = i;
		if (token == "NOTIZ") col_pos[PAT_NOTES] = i;
	    }

	    // TODO: retrieve insurance type
	    while (!line.isNull()) {
		line = t.readLine();
		if (line.isEmpty()) continue;
		// split and remove trailing whitespaces
		QStringList listPatProps = line.split(";");
		for (int i = 0; i < listPatProps.count(); i++) listPatProps[i] = listPatProps.at(i).trimmed();
		patients.append(new cPatient());
		p = patients.last();
		// misuse patient ID fore storing temporarly storing the address reference
		// in order to retrieve the address from adresse.txt later on
		if (col_pos.at(PAT_ADDR_REF) != -1) p->setID(listPatProps.at(col_pos.at(PAT_ADDR_REF)).toInt());
		if (col_pos.at(PAT_SURNAME) != -1) p->setName(listPatProps.at(col_pos.at(PAT_SURNAME)));
		if (col_pos.at(PAT_FORENAME) != -1) p->setForename(listPatProps.at(col_pos.at(PAT_FORENAME)));
		if (col_pos.at(PAT_ADDNAME) != -1) p->setAddName(listPatProps.at(col_pos.at(PAT_ADDNAME)));
		if (col_pos.at(PAT_GENDER) != -1) {
		    if (listPatProps.at(col_pos.at(PAT_GENDER)) == "M") p->setGender(0);
		    else p->setGender(1);
		}
		if (col_pos.at(PAT_TITLE) != -1) p->setTitle(listPatProps.at(col_pos.at(PAT_TITLE)));
		// FIXME: birthday and kvkvalid => which format uses sp ???

		if (col_pos.at(PAT_TEL) != -1) p->setTelephone(listPatProps.at(col_pos.at(PAT_TEL)));
		if (col_pos.at(PAT_VNR) != -1) p->setVNR(listPatProps.at(col_pos.at(PAT_VNR)));
		// FIXME: add status
		//if (col_pos.at(PAT_STATUS) != -1) p->setStatus(listPatProps.at(col_pos.at(PAT_STATUS)));

		if (col_pos.at(PAT_NOTES) != -1) p->setNotices(listPatProps.at(col_pos.at(PAT_NOTES)));
	    }
	}
    }
    if (file_patients.isOpen()) file_patients.close();


    // import physicians
    if (convert_phys) {
	columns.clear();
	col_pos.clear();
	t.setDevice(&file_phys);

	// read first line to learn about the columns of arzt.txt
	line = t.readLine();
	if (!line.isEmpty()) {
	    columns = line.split(";");
	    for (int i = 0; i < PHYS_ATTR_NUMBER; i++) col_pos.append(-1);
	    for (int i = 0; i < columns.count(); i++) {
		QString token = columns.at(i).toUpper();
		if (token == "ADR::NR") col_pos[PHYS_ADDR_REF] = i;
		if (token == "NACHNAME") col_pos[PHYS_SURNAME] = i;
		if (token == "VORNAME") col_pos[PHYS_FORENAME] = i;
		if (token == "GESCHLECHT") col_pos[PHYS_GENDER] = i;
		if (token == "TITEL") col_pos[PHYS_TITLE] = i;
		if (token == "TELEFON") col_pos[PHYS_TEL] = i;
	    }

	    while (!line.isNull()) {
		line = t.readLine();
		if (line.isEmpty()) continue;
		// split and remove trailing whitespaces
		QStringList listPhysProps = line.split(";");
		for (int i = 0; i < listPhysProps.count(); i++) listPhysProps[i] = listPhysProps.at(i).trimmed();
		physicians.append(new cPhys());
		phys = physicians.last();
		if (col_pos.at(PHYS_ADDR_REF) != -1) phys->setID(listPhysProps.at(col_pos.at(PHYS_ADDR_REF)).toInt());
		if (col_pos.at(PHYS_SURNAME) != -1) phys->setName(listPhysProps.at(col_pos.at(PHYS_SURNAME)));
		if (col_pos.at(PHYS_FORENAME) != -1) phys->setForename(listPhysProps.at(col_pos.at(PHYS_FORENAME)));
		if (col_pos.at(PHYS_GENDER) != -1) {
		    if (listPhysProps.at(col_pos.at(PHYS_GENDER)) == "M") phys->setGender(0);
		    else phys->setGender(1);
		}
		if (col_pos.at(PHYS_TITLE) != -1) phys->setTitle(listPhysProps.at(col_pos.at(PHYS_TITLE)));
		if (col_pos.at(PHYS_TEL) != -1) phys->setTelephone(listPhysProps.at(col_pos.at(PHYS_TEL)));
	    }
	}
    }
    if (file_phys.isOpen()) file_phys.close();

    if ( (!convert_pat) && (!convert_phys)) goto skip;

    // now map addresses to patients and physicians
    t.setDevice(&file_addr);
    columns.clear();
    col_pos.clear();
    line = t.readLine(); // read first line to learn about the columns of adresse.txt
    if (line.isEmpty()) {
        m_ui->textEdit->append(QString::fromUtf8("<b>FEHLER:</b> Probleme beim Lesen der Adressdatei aufgetreten!"));
	goto skip;
    }
    columns = line.split(";");
    for (int i = 0; i < ADDR_ATTR_NUMBER; i++) col_pos.append(-1);
    for (int i = 0; i < columns.count(); i++) {
	QString token = columns.at(i).toUpper();
	if (token == "REF::NR") col_pos[ADDR_REF] = i;
	if (token == "STRASSE") col_pos[ADDR_STREET] = i;
	if (token == "PLZ") col_pos[ADDR_ZIP] = i;
	if (token == "ORT") col_pos[ADDR_CITY] = i;
    }
    if (col_pos.at(ADDR_REF) == -1) {
        m_ui->textEdit->append(QString::fromUtf8("<b>FEHLER:</b> Probleme beim Lesen der Adressdatei aufgetreten!"));
	goto skip;
    }

    while (!line.isNull()) {
	line = t.readLine();
	if (line.isEmpty()) continue;
	// split and remove trailing whitespaces
	QStringList listAddrProps = line.split(";");
	for (int i = 0; i < listAddrProps.count(); i++) listAddrProps[i] = listAddrProps.at(i).trimmed();
	int ref_id = listAddrProps.at(col_pos.at(ADDR_REF)).toInt();
	for (int i = 0; i < patients.count(); i++) {
	    p = patients.at(i);
	    if (p->getID() == ref_id) {
		if (col_pos.at(ADDR_STREET) != -1) p->setStreet(listAddrProps.at(col_pos.at(ADDR_STREET)));
		if (col_pos.at(ADDR_ZIP) != -1) p->setZipcode(listAddrProps.at(col_pos.at(ADDR_ZIP)));
		if (col_pos.at(ADDR_CITY) != -1) p->setCity(listAddrProps.at(col_pos.at(ADDR_CITY)));
		break;
	    }
	}
	for (int i = 0; i < physicians.count(); i++) {
	    phys = physicians.at(i);
	    if (phys->getID() == ref_id) {
		if (col_pos.at(ADDR_STREET) != -1) phys->setStreet(listAddrProps.at(col_pos.at(ADDR_STREET)));
		if (col_pos.at(ADDR_ZIP) != -1) phys->setZipcode(listAddrProps.at(col_pos.at(ADDR_ZIP)));
		if (col_pos.at(ADDR_CITY) != -1) phys->setCity(listAddrProps.at(col_pos.at(ADDR_CITY)));
		break;
	    }
	}
    }


    int id;
    if (convert_pat) {
        m_ui->textEdit->append(QString::fromUtf8("<br><u>Übernommene Patienten</u>"));
	for (int i = 0; i < patients.count(); i++) {
	    p = patients.at(i);
	    m_ui->textEdit->append(p->getForename() + " " + p->getName());
	    p->setID(-1); // mark patient as new object
            if (db.existPerson(p, &id)) {
		p->setID(id);
                if (!db.updateItem(*p)) m_ui->textEdit->append(QString::fromUtf8("<bFEHLER:</b> Übernahme fehlgeschlagen"));
	    } else {
                if (!db.storeItem(*p, &id)) m_ui->textEdit->append(QString::fromUtf8("<bFEHLER:</b> Übernahme fehlgeschlagen"));
	    }
	}
    }

    if (convert_phys) {
        m_ui->textEdit->append(QString::fromUtf8("<br><u>Übernommene Ärzte</u>"));
	for (int i = 0; i < physicians.count(); i++) {
	    phys = physicians.at(i);
	    m_ui->textEdit->append(phys->getTitle() + " " + phys->getForename() + " " + phys->getName());
	    phys->setID(-1); // mark physician as new object
            if (db.existPerson(phys, &id)) {
		phys->setID(id);
                if (!db.updateItem(*phys)) m_ui->textEdit->append(QString::fromUtf8("<bFEHLER:</b> Übernahme fehlgeschlagen"));
	    } else {
                if (!db.storeItem(*phys, &id)) m_ui->textEdit->append(QString::fromUtf8("<bFEHLER:</b> Übernahme fehlgeschlagen"));
	    }
	}
    }


skip:
    file_addr.close();
    m_ui->textEdit->append(QString::fromUtf8("<br>Dateien wurden geschlossen"));
    while (patients.count() > 0) {
	delete patients.first();
	patients.removeFirst();
    }
    while (physicians.count() > 0) {
	delete physicians.first();
	physicians.removeFirst();
    }
    return true;
}


void ConfTakeover::slotStart()
{
    int sel_prog = m_ui->cbProg->currentIndex();
    if (m_ui->leDir->text().isEmpty()) {
        QMessageBox::warning(this, "Hinweis", QString::fromUtf8("Bitte erst das Datenverzeichnis wählen!"), QMessageBox::Ok);
	return;
    }
    switch (sel_prog) {

        case PRG_SP: // starke praxis
	    handle_sp(m_ui->leDir->text());
	    break;

	default: break;
    }
}


void ConfTakeover::slotSelectDirectory()
{
    QString dir = QFileDialog::getExistingDirectory(this, QString::fromUtf8("Datenverzeichnis wählen"), ".",
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    m_ui->leDir->setText(dir);
}
