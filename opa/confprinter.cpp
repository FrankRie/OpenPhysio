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

#include "confprinter.h"
#include "ui_confprinter.h"
#include <msgbox.h>


ConfPrinter::ConfPrinter(QWidget *parent, int l, int a) :
    ConfBase(parent, l, a),
    m_ui(new Ui::ConfPrinter)
{
    m_ui->setupUi(this);
    setTitle("Drucker");
    setKeyword("opa_print");
}


ConfPrinter::~ConfPrinter()
{
    delete m_ui;
}


void ConfPrinter::init()
{
    connect(m_ui->pbTestPrint, &QPushButton::clicked, this, &ConfPrinter::slotTestPrint);
    connect(m_ui->pbTestPrintDoc, &QPushButton::clicked, this, &ConfPrinter::slotTestPrintDoc);
    connect(m_ui->pbTestPrintDates, &QPushButton::clicked, this, &ConfPrinter::slotTestPrintDates);

   int prn_co = QPrinterInfo::availablePrinters().count();
    int prn_default = myConf.get(ConfXML::AREA_PRINTER, "prndev_receipe").toInt();
    int prn_def_doc = myConf.get(ConfXML::AREA_PRINTER, "prndev_doc").toInt();
    int prn_def_dates = myConf.get(ConfXML::AREA_PRINTER, "prndev_dates").toInt();
    if (prn_default >= prn_co) prn_default = 0;
    if (prn_def_doc >= prn_co) prn_def_doc = 0;
    if (prn_def_dates >= prn_co) prn_def_dates = 0;
    for (int i = 0; i < prn_co; i++) {
        m_ui->cbPrinter->addItem(QPrinterInfo::availablePrinters().at(i).printerName());
        m_ui->cbPrinterDoc->addItem(QPrinterInfo::availablePrinters().at(i).printerName());
        m_ui->cbPrinterDates->addItem(QPrinterInfo::availablePrinters().at(i).printerName());
    }
    m_ui->cbPrinter->setCurrentIndex(prn_default);
    m_ui->cbPrinterDoc->setCurrentIndex(prn_def_doc);
    m_ui->cbPrinterDates->setCurrentIndex(prn_def_dates);
    m_ui->spOfsX->setValue(myConf.get(ConfXML::AREA_PRINTER, "ofx").toInt());
    m_ui->spOfsY->setValue(myConf.get(ConfXML::AREA_PRINTER, "ofy").toInt());
    m_ui->spDocOfsX->setValue(myConf.get(ConfXML::AREA_PRINTER, "doc_ofx").toInt());
    m_ui->spDocOfsY->setValue(myConf.get(ConfXML::AREA_PRINTER, "doc_ofy").toInt());

    if (myConf.get(ConfXML::AREA_PRINTER, "prndlg_receipe") == "true") m_ui->cbShowPrnDlg->setCheckState(Qt::Checked);
    else m_ui->cbShowPrnDlg->setCheckState(Qt::Unchecked);
    if (myConf.get(ConfXML::AREA_PRINTER, "prndlg_doc") == "true") m_ui->cbShowPrnDlgDoc->setChecked(true);
    else m_ui->cbShowPrnDlgDoc->setChecked(false);
    if (myConf.get(ConfXML::AREA_PRINTER, "prndlg_dates") == "true") m_ui->cbShowPrnDlgDates->setChecked(true);
    else m_ui->cbShowPrnDlgDates->setChecked(false);
}


bool ConfPrinter::write()
{
    myConf.set(ConfXML::AREA_PRINTER, "prndev_receipe", QString::number(m_ui->cbPrinter->currentIndex()));
    myConf.set(ConfXML::AREA_PRINTER, "prndev_doc", QString::number(m_ui->cbPrinterDoc->currentIndex()));
    myConf.set(ConfXML::AREA_PRINTER, "prndev_dates", QString::number(m_ui->cbPrinterDates->currentIndex()));
    myConf.set(ConfXML::AREA_PRINTER, "ofx", QString::number(m_ui->spOfsX->value()));
    myConf.set(ConfXML::AREA_PRINTER, "ofy", QString::number(m_ui->spOfsY->value()));
    myConf.set(ConfXML::AREA_PRINTER, "doc_ofx", QString::number(m_ui->spDocOfsX->value()));
    myConf.set(ConfXML::AREA_PRINTER, "doc_ofy", QString::number(m_ui->spDocOfsY->value()));
    if (m_ui->cbShowPrnDlg->isChecked()) myConf.set(ConfXML::AREA_PRINTER, "prndlg_receipe", "true");
    else myConf.set(ConfXML::AREA_PRINTER, "prndlg_receipe", "false");
    if (m_ui->cbShowPrnDlgDoc->isChecked()) myConf.set(ConfXML::AREA_PRINTER, "prndlg_doc", "true");
    else myConf.set(ConfXML::AREA_PRINTER, "prndlg_doc", "false");
    if (m_ui->cbShowPrnDlgDates->isChecked()) myConf.set(ConfXML::AREA_PRINTER, "prndlg_dates", "true");
    else myConf.set(ConfXML::AREA_PRINTER, "prndlg_dates", "false");

    return true;
}


void ConfPrinter::slotTestPrint()
{
    MsgBox::dummy();
}


void ConfPrinter::slotTestPrintDoc()
{
    MsgBox::dummy();
}


void ConfPrinter::slotTestPrintDates()
{
    MsgBox::dummy();
}
