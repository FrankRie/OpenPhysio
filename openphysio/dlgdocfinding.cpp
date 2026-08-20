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

#include "dlgdocfinding.h"
#include "ui_dlgdocfinding.h"
#include "mydb.h"
#include "helperwrapper.h"
#include "config.h"

DlgDocFinding::DlgDocFinding(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::DlgDocFinding)
{
    m_ui->setupUi(this);

    connect(m_ui->pbSave, &QPushButton::clicked, this, &DlgDocFinding::slotSave);
    connect(m_ui->pbCancel, &QPushButton::clicked, this, &DlgDocFinding::slotCancel);
    connect(m_ui->spFontSize, &QSpinBox::valueChanged, this, &DlgDocFinding::slotFontSize);

    for (int i = 0; i < db.listEmpl.count(); i++) {
	m_ui->cbEmpl->addItem(db.listEmpl.at(i)->getForename() + " " + db.listEmpl.at(i)->getName());
    }

    m_ui->spFontSize->setValue(myConf.get(ConfXML::AREA_DISPLAY, "fontsize_findings").toInt());

    m_ui->deFinding->calendarWidget()->setFirstDayOfWeek(Qt::Monday);
    m_ui->deFinding->calendarWidget()->setGridVisible(true);
    installEventFilter(this);
}


DlgDocFinding::~DlgDocFinding()
{
    delete m_ui;
}


void DlgDocFinding::init(cFinding& f)
{
    //qDebug() << __FUNCTION__ << "id: " << f.getID();
    if (f.getID() > 0) {
	curFinding = f;
	m_ui->deFinding->setDate(curFinding.getDate());
	for (int i = 0; i < db.listEmpl.count(); i++) {
	    if (curFinding.getEmplID() == db.listEmpl.at(i)->getID()) {
		m_ui->cbEmpl->setCurrentIndex(i);
	    }
	}
	m_ui->teFinding->setPlainText(f.getFData());
    } else {
	curFinding.reset();
	curFinding.setPatientID(f.getPatientID());
	m_ui->deFinding->setDate(QDate::currentDate());
	m_ui->cbEmpl->setCurrentIndex(0);
    }
    cPatient p;
    db.retrieveItem(p, curFinding.getPatientID());
    m_ui->lePatient->setText(p.getForename() + " " + p.getName());
    // if set, use the default employee (but only if this is a new finding report)
    if (f.getID() < 0) {
        int eid = p.getDefaultEmployeeID();
        if (eid > 0) {
            for (int i = 0; i < db.listEmpl.count(); i++) {
                if (eid == db.listEmpl.at(i)->getID()) {
                    m_ui->cbEmpl->setCurrentIndex(i);
                }
            }
        }
    }
}


void DlgDocFinding::slotCancel()
{
    this->reject();
}


void DlgDocFinding::slotSave()
{
    curFinding.setDate(m_ui->deFinding->date());
    if ( (m_ui->cbEmpl->currentIndex() >= 0) && (m_ui->cbEmpl->currentIndex() < db.listEmpl.count()) ) {
	curFinding.setEmplID(db.listEmpl.at(m_ui->cbEmpl->currentIndex())->getID());
    }
    curFinding.setFData(m_ui->teFinding->toPlainText());
    if (curFinding.getID() > 0) {  // update
	db.updateItem(curFinding);
    } else {                       // create new
	int id;
	db.storeItem(curFinding, &id);
	curFinding.setID(id);
    }
    this->accept();
}


bool DlgDocFinding::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *ke = static_cast<QKeyEvent *>(event);
        if (ke->key() == Qt::Key_F1 || ke->key() == Qt::Key_Help) {
            helpWrapper.showHelpForKeyword("openphysio_findings", true);
            return true;
        }
    }
    return QDialog::eventFilter(obj, event);
}


void DlgDocFinding::slotFontSize(int i)
{
    QFont f = m_ui->teFinding->font();
    f.setPixelSize(i);
    m_ui->teFinding->setFont(f);
}
