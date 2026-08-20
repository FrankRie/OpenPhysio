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

#include "dlgupdateinsurances.h"
#include "ui_dlgupdateinsurances.h"
#include <config.h>


DlgUpdateInsurances::DlgUpdateInsurances(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::DlgUpdateInsurances)
{
    m_ui->setupUi(this);

    token_separator = "+";
    token_segment_end = "'";
    assoc = -1;

    connect(m_ui->pbStart, &QPushButton::clicked, this, &DlgUpdateInsurances::slotStart);
    connect(m_ui->pbCancel, &QPushButton::clicked, this, &DlgUpdateInsurances::slotCancel);
}


DlgUpdateInsurances::~DlgUpdateInsurances()
{
    for (int i = 0; i < ins_existing.count(); i++) delete ins_existing.at(i);
    ins_existing.clear();
    delete m_ui;
}


bool DlgUpdateInsurances::init(QStringList list)
{
    fnames = list;
    if (fnames.count() < 1) return false;

    setWindowTitle(QString::fromUtf8("Kostenträger einlesen 1/") + QString::number(fnames.count()));

    // analyze file name
    QFileInfo info(fnames.at(0));
    return analyzeFilename(info.fileName());
}


bool DlgUpdateInsurances::analyzeFilename(QString s)
{
    s = s.toUpper();
    m_ui->labFile->setText(s);

    QStringList l = s.split(".");

    // ensure that filename format is xxxxxxxx.xxx
    if ( (s.length() != 12) || (l.count() != 2) ||
	 (l.at(0).length() != 8) || (l.at(1).length() != 3)) {
        QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), QString::fromUtf8("Keine Kostenträgerdatei (Ungültiger Dateiname)!"), QMessageBox::Ok);
	return false;
    }

    QString x;

    x = s.left(2);
    // assoc must must to TABLE associations
    if (x == "AO") { m_ui->labKassenart->setText("AOK"); assoc = cAssoc::AOK; }
    else if (x == "EK") { m_ui->labKassenart->setText("Ersatzkassen"); assoc = cAssoc::VDEK; }
    else if (x == "BK") { m_ui->labKassenart->setText("Betriebskrankenkassen"); assoc = cAssoc::BKK; }
    else if (x == "IK") { m_ui->labKassenart->setText("Innungskrankenkassen"); assoc = cAssoc::IKK; }
    else if (x == "BN") { m_ui->labKassenart->setText("Bundesknappschaft"); assoc = cAssoc::KBS; }
    else if (x == "LK") { m_ui->labKassenart->setText("Landwirtschaftliche Krankenkassen"); assoc = cAssoc::LKK; }
    else if (x == "SE") { m_ui->labKassenart->setText("Seekrankenkassen"); assoc = cAssoc::KBS; }
    // FIXME: what is the right assoc ???
    else if (x == "GK") { m_ui->labKassenart->setText("Gesetzliche Krankenversicherung"); assoc = cAssoc::AOK; }
    else if (x == "BG") { m_ui->labKassenart->setText("BG"); assoc = cAssoc::BG; }
    else if (x == "PB") { m_ui->labKassenart->setText("Post-A"); assoc = cAssoc::POSTA; }
    else if (x == "HP") { m_ui->labKassenart->setText(QString::fromUtf8("Heilfürsorge Bundespolizei")); assoc = cAssoc::BPOL; } // Heilfürsorge Bundespolizei
    else {
        QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), QString::fromUtf8("Ungültige Kassenart!"), QMessageBox::Ok);
	return false;
    }

    x = s.mid(2, 2);
    if (x == "05") m_ui->labVerfahren->setText(QString::fromUtf8("Sonstige Leistungserbringer"));
    else {
        QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), QString::fromUtf8("Ungültiges Verfahren!"), QMessageBox::Ok);
	return false;
    }

    x = s.mid(4, 2);
    m_ui->labGueltigkeit->setText(x + "/20" + s.mid(6, 2));

    if ( (l.at(1) == "CSV") && (assoc == cAssoc::BG) ) {
	m_ui->labFormat->setText("CSV");
        m_ui->labEinsatzgebiet->setText(QString::fromUtf8("Kostenträger BG"));
	m_ui->labVersion->setText("0");
    } else if ( (l.at(1) == "CSV") && (assoc == cAssoc::POSTA) ) {
	m_ui->labFormat->setText("CSV");
        m_ui->labEinsatzgebiet->setText(QString::fromUtf8("Kostenträger POST-A"));
	m_ui->labVersion->setText("0");
    } else if ( (l.at(1) == "CSV") && (assoc == cAssoc::BPOL) ) {
        m_ui->labFormat->setText("CSV");
        m_ui->labEinsatzgebiet->setText(QString::fromUtf8("Kostenträger Bundespolizei"));
        m_ui->labVersion->setText("0");
    } else {
        x = s.mid(9, 1);
	if (x != "K") {
            QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), QString::fromUtf8("Ungültiges Einsatzgebiet!"), QMessageBox::Ok);
	    return false;
        } else m_ui->labEinsatzgebiet->setText(QString::fromUtf8("Kostenträgerdatei Datenaustausch"));

	x = s.mid(10, 1);
	if (x != "E") {
            QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), QString::fromUtf8("Ungültiges Dateiformat (kein Edifact)!"), QMessageBox::Ok);
	    return false;
        } else m_ui->labFormat->setText(QString::fromUtf8("Edifact"));

	m_ui->labVersion->setText(s.right(1));
    }

    return true;
}


int DlgUpdateInsurances::getOpcode(QString s)
{
    if (s == "UNA") return UNA;
    if (s == "UNB") return UNB;
    if (s == "UNH") return UNH;
    if (s == "UNT") return UNT;
    if (s == "UNZ") return UNZ;
    if (s == "IDK") return IDK;
    if (s == "VDT") return VDT;
    if (s == "FKT") return FKT;
    if (s == "KTO") return KTO;
    if (s == "VKG") return VKG;
    if (s == "NAM") return NAM;
    if (s == "ANS") return ANS;
    if (s == "ASP") return ASP;
    if (s == "UEM") return UEM;
    if (s == "DFU") return DFU;
    return -1;
}


void DlgUpdateInsurances::parseLine(int opcode, QString &line, cInsurance* ins)
{
    if (ins == NULL) return;

    // remove segment end token from line
    QString tmp = line.replace(token_segment_end, "", Qt::CaseSensitive);

    QStringList list;
    list = tmp.split(token_separator);
    if (list.count() < 2) return;
    list.removeFirst(); // remove opcode

    switch(opcode) {
	case IDK: {
	    if (list.count() < 3) break;
	    QString ik = list[0];
	    if (ik.length() != 9) break;
	    QString name_short;
	    name_short = list[2];
	    m_ui->textEdit->append("Lese " + name_short + " (IK " + ik + ")");
	    ins->setIK(ik);
	    ins->setShortName(name_short);
	    }
	    break;

	case VDT: {
	    if (list.count() < 1) break;
	    ins->setVDT(list.join("+"));
	    }
	    break;

	case FKT: {
	    if (list.count() < 1) break;
	    ins->setFKT(list[0]);
	    }
	    break;

	case KTO: {
	    if (list.count() < 3) break;
	    ins->setKTO(list.join("+"));
	    }
	    break;

	case VKG: {
	    if (list.count() < 3) break;
	    ins->setKTO(list.join("+"));
	    }
	    break;

	case NAM: {
	    if (list.count() < 2) break;
	    list.removeFirst(); // remove number
	    ins->setNAM(list.join(" "));
	    }
	    break;

	case ANS: {
	    if (list.count() < 3) break;
	    list.removeFirst(); // number
	    QString s = list.join("+");
	    if (ins->getANS1().isEmpty()) ins->setANS1(s);
	    else if (ins->getANS2().isEmpty()) ins->setANS2(s);
	    else if (ins->getANS3().isEmpty()) ins->setANS3(s);
	    }
	    break;

	case ASP: {
	    if (list.count() < 2) break;
	    list.removeFirst(); // number
	    QString s = list.join("+");
	    if (ins->getASP1().isEmpty()) ins->setASP1(s);
	    else if (ins->getASP2().isEmpty()) ins->setASP2(s);
	    else if (ins->getASP3().isEmpty()) ins->setASP3(s);
	    }
	    break;

	case UEM: {
	    if (list.count() < 3) break;
	    QString s = list.join("+");
	    if (ins->getUEM1().isEmpty()) ins->setUEM1(s);
	    else if (ins->getUEM2().isEmpty()) ins->setUEM2(s);
	    }
	    break;

	case DFU: {
	    if (list.count() < 3) break;
	    QString s = list.join("+");
	    if (ins->getDFU1().isEmpty()) ins->setDFU1(s);
	    else if (ins->getDFU2().isEmpty()) ins->setDFU2(s);
	    }
	    break;

	default: break;
    }
}


void DlgUpdateInsurances::merge(cInsurance *ins)
{
    // enumerate through ins_existing
    // - if an IK matches check whether
    //    - we just perform an update
    //    - or if we have to save old insurance into ins_history
    // - if IK does not existing perform an insert

    for (int i = 0; i < ins_existing.count(); i++) {
	cInsurance *tmp = (cInsurance*)(ins_existing.at(i));
	if (tmp->getIK() == ins->getIK()) {

	    if ( (tmp->getAssoc() != ins->getAssoc()) || (tmp->getShortName() != ins->getShortName()) ) {
		// save old insurance data
		ins_history.append(new cInsurance());
		*ins_history.at(ins_history.count() - 1) = *tmp;
	    }

	    // signal the DB layer that this will an update (and not an insert) operation
	    ins->setID(tmp->getID());
	    break;
	}
    }

    ins_new.append(new cInsurance());
    *ins_new.at(ins_new.count() - 1) = *ins;
}


bool DlgUpdateInsurances::perform(QString fname)
{
    if (!db.isConnected()) {
        m_ui->textEdit->append(QString::fromUtf8("Verbindung mit Datenbank wird aufgebaut"));
	if (!db.DB_connect(myConf.getDatabase())) {
            m_ui->textEdit->append(QString::fromUtf8("<b>FEHLER</b>: Keine Verbindung zur Datenbank!"));
	    return false;
	}
    }
    m_ui->textEdit->append(QString::fromUtf8("Verbindung mit Datenbank ist aufgebaut"));
    m_ui->textEdit->repaint();

    ins_new.clear();
    ins_history.clear();
    if (!db.retrieveItemList(myDB::TABLE_INSURANCES, ins_existing)) {
        m_ui->textEdit->append(QString::fromUtf8("<b>FEHLER</b>: Bestehende Kostenträger konnten nicht aus der Datenbank gelesen werden!"));
	return false;
    }

    // open file
    QFile file(fname);
    if (!file.open(QIODevice::ReadOnly)) {
        m_ui->textEdit->append(QString::fromUtf8("<b>FEHLER</b>: Datei ") + fname + QString::fromUtf8(" konnte nicht geöffnet werden!"));
	return false;
    }
    m_ui->textEdit->append(QString::fromUtf8("Datei ") + fname + QString::fromUtf8(" wird geöffnet"));
    m_ui->textEdit->repaint();

    QTextStream t(&file);
    QString line = "";
    int state = STATE_INIT;
    cInsurance *ins = NULL;
    bool flag_exit = false;

    // parse
    if (assoc == cAssoc::BG) { // BG data is supplied via CSV file => use different parsing procedure
	line = t.readLine(); // skip first line containing header data
	while (!line.isNull()) {
	    line = t.readLine();
	    if (line.isEmpty()) continue;
	    QStringList l = line.split(";");
	    if (l.count() < 16) continue;
	    ins = new cInsurance();
	    ins->setAssoc(assoc);
	    ins->setIK(l[0]);
	    ins->setShortName(l[4]);
	    ins->setNAM(l[4]);
	    if (l[5] != "") ins->setNAM(ins->getNAM() + " " + l[5]);
	    // ZIP+CITY+STREET
	    ins->setANS1(l[8] + "+" + l[9] + "+" + l[6]);
	    merge(ins);
	    delete ins;

	}
    } else if (assoc == cAssoc::POSTA || assoc == cAssoc::BPOL) {
	line = t.readLine(); // skip first line containing header data
	while (!line.isNull()) {
	    line = t.readLine();
	    if (line.isEmpty()) continue;
	    QStringList l = line.split(";");
	    if (l.count() < 8) continue;
	    ins = new cInsurance();
	    ins->setAssoc(assoc);
	    ins->setIK(l[0]);
	    ins->setShortName(l[1]);
	    ins->setNAM(l[1]);
	    ins->setANS1(l[2]);
	    ins->setANS2(l[3]);
	    merge(ins);
	    delete ins;

	}
    } else {
	while (!line.isNull()) {
	    if (flag_exit) break;

	    line = t.readLine();
	    int opcode = getOpcode(line.left(3));
	    if (opcode < 0) {
                m_ui->textEdit->append(QString::fromUtf8("WARNUNG: Unbekannter Opcode ") + line.left(3));
		continue;
	    }

	    switch(state) {

	    case STATE_INIT:
		// TODO: handle UNA, UNB (e.g. retrieve token separator)
		if (opcode == UNZ) flag_exit = true;
		if (opcode == UNH) {
		    if (ins != NULL) delete ins;
		    ins = new cInsurance();
		    state = STATE_BEGIN;
		}
		break;

	    case STATE_BEGIN:
		if (opcode == UNZ) flag_exit = true;
		else if (opcode == UNT) {
		    // TODO: write insurance object to database
		    if (ins != NULL) {
			ins->setAssoc(assoc);
			merge(ins);
			delete ins;
			ins = NULL;
		    }
		    state = STATE_END;
		} else parseLine(opcode, line, ins);
		break;

	    case STATE_END:
		if (opcode == UNZ) flag_exit = true;
		if (opcode == UNH) {
		    if (ins != NULL) delete ins;
		    ins = new cInsurance();
		    state = STATE_BEGIN;
		}
		break;

	    default: break;
	    }
	}
    }

    file.close();
    m_ui->textEdit->append(QString::fromUtf8("Datei wird geschlossen"));
    m_ui->textEdit->repaint();

    /*m_ui->textEdit->append(QString::fromUtf8("<br><u>History</u>"));
    for (int i = 0; i < ins_history.count(); i++) {
	m_ui->textEdit->append(ins_history.at(i)->getIK() + " " + ins_history.at(i)->getShortName());
    }
    m_ui->textEdit->append(QString::fromUtf8("<br>"));*/
    /*m_ui->textEdit->append(QString::fromUtf8("<br><u>New List</u>"));
    for (int i = 0; i < ins_new.count(); i++) {
	m_ui->textEdit->append(ins_new.at(i)->getIK() + " " + ins_new.at(i)->getShortName());
    }*/

    // TODO: save history of insurances

    m_ui->textEdit->append(QString::fromUtf8("<br>Schreibe Daten in die Datenbank. Bitte warten .."));
    m_ui->textEdit->repaint();
    if (!db.saveInsurances(&ins_new)) {
        m_ui->textEdit->append(QString::fromUtf8("<bFEHLER:</b>Kostenträger konnten nicht in der Datenbank gespeichert werden!"));
    }

    return true;
}


void DlgUpdateInsurances::slotStart()
{
    m_ui->pbStart->setDisabled(true);
    m_ui->pbCancel->setDisabled(true);
    m_ui->textEdit->append(QString::fromUtf8("Starte Kostenträger-Update ..."));
    m_ui->textEdit->repaint();
    bool bRet = true;
    for (int i = 0; i < fnames.count(); i++) {
        this->setWindowTitle(QString::fromUtf8("Kostenträger einlesen ") +
                             QString::number(i + 1) + "/" + QString::number(fnames.count()));
        // analyze file name
        QFileInfo info(fnames.at(i));
        if (!analyzeFilename(info.fileName())) {
            bRet = false;
            continue;
        }
        if (!perform(fnames.at(i))) bRet = false;
    }
    if (bRet) {
        m_ui->textEdit->append(QString::fromUtf8("<br><br><b>Kostenträger-Update erfolgreich durchgeführt!</b>"));
    } else {
        m_ui->textEdit->append(QString::fromUtf8("<br><br><b>Es sind Fehler aufgetreten!</b>"));
    }

    m_ui->pbCancel->setText("Beenden");
    m_ui->pbCancel->setDisabled(false);
}


void DlgUpdateInsurances::slotCancel()
{
    this->reject();
}
