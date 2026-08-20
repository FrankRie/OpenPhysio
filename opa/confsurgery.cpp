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

#include "confsurgery.h"
#include "ui_confsurgery.h"
#include <msgbox.h>


ConfSurgery::ConfSurgery(QWidget *parent, int l, int a) :
    ConfBase(parent, l, a),
    m_ui(new Ui::ConfSurgery)
{
    m_ui->setupUi(this);

    // hide branch stuff for now because it's not fully implemented yet
    m_ui->tab_2->setEnabled(false);

    setTitle("Praxisverwaltung");
    setKeyword("opa_surgery");

    connect(m_ui->cbProfPhysio, &QCheckBox::checkStateChanged, this, &ConfSurgery::slotPhysio);
    connect(m_ui->cbProfErgo, &QCheckBox::checkStateChanged, this, &ConfSurgery::slotErgo);
    connect(m_ui->cbProfLogo, &QCheckBox::checkStateChanged, this, &ConfSurgery::slotLogo);
    connect(m_ui->pbAddBranch, &QCheckBox::clicked, this, &ConfSurgery::slotAddBranch);
    connect(m_ui->pbDelBranch, &QCheckBox::clicked, this, &ConfSurgery::slotDelBranch);

}


ConfSurgery::~ConfSurgery()
{
    delete m_ui;
}



void ConfSurgery::init()
{
    if (!db.isConnected()) {
	QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), "Keine Verbindung zur Datenbank!", QMessageBox::Ok);
	return;
    }
    m_ui->cbTariff->clear();
    for (int i = 0; i < list_areas.count(); i++) delete list_areas.at(i);
    for (int i = 0; i < list_unions.count(); i++) delete list_unions.at(i);
    for (int i = 0; i < list_branches.count(); i++) delete list_branches.at(i);
    list_areas.clear();
    list_unions.clear();
    list_branches.clear();
    db.retrieveItemList(myDB::TABLE_TARIFF_AREAS, list_areas);
    db.retrieveItemList(myDB::TABLE_UNIONS, list_unions);
    db.retrieveItemList(myDB::TABLE_SURGERY_BRANCHES, list_branches);
    for (int i = 0; i < list_areas.count(); i++)
        m_ui->cbTariff->addItem(((cTariffArea*)(list_areas.at(i)))->getName());
    for (int x = 0; x < list_unions.count(); x++) {
        cUnion *u = (cUnion*)(list_unions.at(x));
        if (u->getProfession() == -1) {
            m_ui->cbUnionPhysio->addItem(u->getNameShort());
            m_ui->cbUnionErgo->addItem(u->getNameShort());
            m_ui->cbUnionLogo->addItem(u->getNameShort());
        } else if (u->getProfession() == cProfession::PROF_PHYSIOTHERAPY) {
            m_ui->cbUnionPhysio->addItem(u->getNameShort());
        } else if (u->getProfession() == cProfession::PROF_ERGOTHERAPY) {
            m_ui->cbUnionErgo->addItem(u->getNameShort());
        } else if (u->getProfession() == cProfession::PROF_LOGOPEDICS) {
            m_ui->cbUnionErgo->addItem(u->getNameShort());
        }
    }

    cSurgery surg = db.getSurgery();
    m_ui->leName1->setText(surg.getName1());
    m_ui->leName2->setText(surg.getName2());
    m_ui->leStreet->setText(surg.getStreet());
    m_ui->leZip->setText(surg.getZip());
    m_ui->leCity->setText(surg.getCity());
    m_ui->leTel->setText(surg.getTel());
    m_ui->leMobile->setText(surg.getMobile());
    m_ui->leFax->setText(surg.getFax());
    m_ui->leEMail->setText(surg.getEMail());
    m_ui->leWebsite->setText(surg.getWebsite());
    m_ui->leServer->setText(surg.getServer());
    m_ui->leName->setText(surg.getName());
    m_ui->lePassword->setText(surg.getPassword());
    m_ui->lePhysioIK->setText(surg.getPhysioIK());
    m_ui->leErgoIK->setText(surg.getErgoIK());
    m_ui->leLogoIK->setText(surg.getLogoIK());
    m_ui->leBank->setText(surg.getBank());
    m_ui->leBankcode->setText(surg.getBankCode());
    m_ui->leAccount->setText(surg.getAccount());
    m_ui->cbTariff->setCurrentIndex(surg.getArea() - 1);

    int prof = surg.getProfession();
    if (prof & 1) m_ui->cbProfPhysio->setChecked(true);
    else  m_ui->cbProfPhysio->setChecked(false);
    if ((prof >> 1) & 1) m_ui->cbProfErgo->setChecked(true);
    else  m_ui->cbProfErgo->setChecked(false);
    if ((prof >> 2) & 1) m_ui->cbProfLogo->setChecked(true);
    else  m_ui->cbProfLogo->setChecked(false);

    QString s_union = surg.getUnion();
    QStringList listUnion = s_union.split(":");
    for (int x = 0; x < listUnion.count(); x++) {
        QString s = listUnion.at(x);
        int u = s.toInt();
        switch(x) {
            case cProfession::PROF_PHYSIOTHERAPY:
                m_ui->cbUnionPhysio->setCurrentIndex(u);
                break;
            case cProfession::PROF_ERGOTHERAPY:
                m_ui->cbUnionErgo->setCurrentIndex(u);
                break;
            case cProfession::PROF_LOGOPEDICS:
                m_ui->cbUnionErgo->setCurrentIndex(u);
                break;
            default: break;
        }
    }

    m_ui->leTaxID->setText(surg.getTaxID());
    m_ui->leSalesTaxID->setText(surg.getSalesTaxID());
    m_ui->leTradeRegister->setText(surg.getTradeRegister());
    m_ui->cbSalesTax->setChecked(surg.getTaxLiability());
    m_ui->spSalesTaxPercent->setValue(surg.getTaxPercent());

    loadBranches();
}


bool ConfSurgery::write()
{
    if (m_ui->cbTariff->currentIndex() < 0) {
        QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), QString::fromUtf8("Bitte erst das Tarifgebiet auswählen!"), QMessageBox::Ok);
        return false;
    }

    int prof = (int)m_ui->cbProfPhysio->isChecked() | ((int)m_ui->cbProfErgo->isChecked() << 1) | ((int)m_ui->cbProfLogo->isChecked() << 2);
    if (prof == 0) {
        QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), QString::fromUtf8("Bitte mindestens einen Berufszweig angeben!"), QMessageBox::Ok);
        return false;
    }

    if (m_ui->lePhysioIK->text().isEmpty() && m_ui->leErgoIK->text().isEmpty() && m_ui->leLogoIK->text().isEmpty()) {
        QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), "Bitte die IK(s) der Praxis angeben!", QMessageBox::Ok);
        return false;
    }

    cSurgery surg;
    surg.setID(1);
    surg.setName1(m_ui->leName1->text());
    surg.setName2(m_ui->leName2->text());
    surg.setStreet(m_ui->leStreet->text());
    surg.setCity(m_ui->leCity->text());
    surg.setZip(m_ui->leZip->text());
    surg.setTel(m_ui->leTel->text());
    surg.setMobile(m_ui->leMobile->text());
    surg.setFax(m_ui->leFax->text());
    surg.setEMail(m_ui->leEMail->text());
    surg.setWebsite(m_ui->leWebsite->text());
    surg.setServer(m_ui->leServer->text());
    surg.setName(m_ui->leName->text());
    surg.setPassword(m_ui->lePassword->text());
    surg.setPhysioIK(m_ui->lePhysioIK->text());
    surg.setErgoIK(m_ui->leErgoIK->text());
    surg.setLogoIK(m_ui->leLogoIK->text());
    surg.setBank(m_ui->leBank->text());
    surg.setBankCode(m_ui->leBankcode->text());
    surg.setAccount(m_ui->leAccount->text());
    surg.setArea(m_ui->cbTariff->currentIndex() + 1);
    surg.setProfession(prof);
    QStringList listUnion;
    listUnion << QString::number(m_ui->cbUnionPhysio->currentIndex());
    listUnion << QString::number(m_ui->cbUnionErgo->currentIndex());
    listUnion << QString::number(m_ui->cbUnionLogo->currentIndex());
    surg.setUnion(listUnion.join(":"));
    surg.setTaxID(m_ui->leTaxID->text());
    surg.setSalesTaxID(m_ui->leSalesTaxID->text());
    surg.setTradeRegister(m_ui->leTradeRegister->text());
    surg.setTaxLiability(m_ui->cbSalesTax->isChecked());
    surg.setTaxPercent(m_ui->spSalesTaxPercent->value());

    if (!db.updateItem(surg)) {
            QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), "Die Praxisdaten konnten nicht aktualisiert werden!", QMessageBox::Ok);
            return false;
    }

    return true;
}


void ConfSurgery::slotPhysio(int i)
{
    if (i == Qt::Checked) {
        m_ui->cbUnionPhysio->setEnabled(true);
        m_ui->lePhysioIK->setEnabled(true);
    } else {
        m_ui->cbUnionPhysio->setEnabled(false);
        m_ui->cbUnionPhysio->setCurrentIndex(0);
        m_ui->lePhysioIK->setEnabled(false);
        m_ui->lePhysioIK->clear();
    }
}


void ConfSurgery::slotErgo(int i)
{
    if (i == Qt::Checked) {
        m_ui->cbUnionErgo->setEnabled(true);
        m_ui->leErgoIK->setEnabled(true);
    } else {
        m_ui->cbUnionErgo->setEnabled(false);
        m_ui->cbUnionErgo->setCurrentIndex(0);
        m_ui->leErgoIK->setEnabled(false);
        m_ui->leErgoIK->clear();
    }
}


void ConfSurgery::slotLogo(int i)
{
    if (i == Qt::Checked) {
        m_ui->cbUnionLogo->setEnabled(true);
        m_ui->leLogoIK->setEnabled(true);
    } else {
        m_ui->cbUnionLogo->setEnabled(false);
        m_ui->cbUnionLogo->setCurrentIndex(0);
        m_ui->leLogoIK->setEnabled(false);
        m_ui->leLogoIK->clear();
    }
}


void ConfSurgery::slotAddBranch()
{
}


void ConfSurgery::slotDelBranch()
{
}


void ConfSurgery::loadBranches()
{
    m_ui->listWidgetBranches->clear();
    for (int i = 0; i < list_branches.count(); i++) {
        cSurgeryBranch *csb = (cSurgeryBranch*)list_branches.at(i);
        m_ui->listWidgetBranches->addItem(csb->getName1() + " [" + csb->getIK() + "]");
    }
}

/*void ConfSurgery::on_leTel_cursorPositionChanged(int arg1, int arg2)
{

}


void ConfSurgery::on_leFax_cursorPositionChanged(int arg1, int arg2)
{

} */

