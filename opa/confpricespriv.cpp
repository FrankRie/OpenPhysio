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

#include "confpricespriv.h"
#include "ui_confpricespriv.h"
#include <msgbox.h>


ConfPricesPriv::ConfPricesPriv(QWidget *parent, int l, int a) :
    ConfBase(parent, l, a),
    m_ui(new Ui::ConfPricesPriv)
{
    m_ui->setupUi(this);
    setTitle("Preise PKV");
    setKeyword("opa_prices_pkv");

#ifdef Q_OS_MACX
    // for some reason the layout is distored with MAC OS X if
    // spacing is not big enough
    m_ui->widget->layout()->setSpacing(12);
#endif

    connect(m_ui->pbAdd, &QPushButton::clicked, this, &ConfPricesPriv::slotAdd);
    connect(m_ui->pbDel, &QPushButton::clicked, this, &ConfPricesPriv::slotDel);
    connect(m_ui->pbUp, &QPushButton::clicked, this, &ConfPricesPriv::slotUp);
    connect(m_ui->pbDown, &QPushButton::clicked, this, &ConfPricesPriv::slotDown);
}


ConfPricesPriv::~ConfPricesPriv()
{
    delete m_ui;
}


void ConfPricesPriv::cleanup(bool re_init)
{
    if (re_init) {
        disconnect(m_ui->cbTable, &QComboBox::currentIndexChanged, this, &ConfPricesPriv::slotTableChanged);
        disconnect(m_ui->doubleSpinBox, &QDoubleSpinBox::valueChanged, this, &ConfPricesPriv::slotModified);
        disconnect(m_ui->doubleSpinBox_2, &QDoubleSpinBox::valueChanged, this, &ConfPricesPriv::slotModified);
        cpt.clear();
        m_ui->cbTable->clear();
    }
}


void ConfPricesPriv::init()
{
    cleanup(true);
    flag_change = false;

    QStringList qsl;
    db.getReceipeTypes(myDB::RT_PRIVATE, qsl);
    m_ui->cbTable->insertItems(0, qsl);

    loadTable(0);
    connect(m_ui->cbTable, &QComboBox::currentIndexChanged, this, &ConfPricesPriv::slotTableChanged);
    connect(m_ui->doubleSpinBox, &QDoubleSpinBox::valueChanged, this, &ConfPricesPriv::slotModified);
    connect(m_ui->doubleSpinBox_2, &QDoubleSpinBox::valueChanged, this, &ConfPricesPriv::slotModified);
}


bool ConfPricesPriv::write()
{
    if (flag_change) {
	slotSave();
    }
    return true;
}


void ConfPricesPriv::slotSave()
{
    cpt.clear();
    QAbstractItemModel* model;
    QString strPos, strPrice;

    model = m_ui->tableWidget->model();
    for (int i = 0; i < m_ui->tableWidget->rowCount(); i++) {
	strPos = model->index(i, 0).data().toString();
	if (!strPos.isEmpty()) {
	    strPrice = model->index(i, 1).data().toString();
	    strPrice.replace(",",".",Qt::CaseInsensitive);
	    cpt.addItem(strPos, strPrice);
	}
    }
    // save positions for "Hausbesuch" and "Kilometerpauschale"
    QString s;
    s.asprintf("%.2f", m_ui->doubleSpinBox_2->value());
    cpt.addItem("Hausbesuch", s);
    s.asprintf("%.2f", m_ui->doubleSpinBox->value());
    cpt.addItem("Kilometerpauschale", s);
    db.savePriceTablePrivate(cpt);
    flag_change = false;
}


void ConfPricesPriv::slotAdd()
{
    m_ui->tableWidget->insertRow(m_ui->tableWidget->rowCount());
}


void ConfPricesPriv::slotDel()
{
    if (m_ui->tableWidget->rowCount() > 0) {
	int i = m_ui->tableWidget->currentRow();
        if (i >= 0) {
	    m_ui->tableWidget->removeRow(i);
            flag_change = true;
    }
    }
}


void ConfPricesPriv::slotUp()
{
    if (m_ui->tableWidget->rowCount() <= 1) return;

    int i = m_ui->tableWidget->currentRow();
    QAbstractItemModel* model = m_ui->tableWidget->model();
    if ( (model == NULL) || (i == 0) ) return;

    QString strPos = model->index(i - 1, 0).data().toString();
    QString strPrice = model->index(i - 1, 1).data().toString();
    model->setData(model->index(i - 1, 0), model->index(i, 0).data().toString());
    model->setData(model->index(i - 1, 1), model->index(i, 1).data().toString());
    model->setData(model->index(i, 0), strPos);
    model->setData(model->index(i, 1), strPrice);
    m_ui->tableWidget->setCurrentIndex(model->index(i - 1, 0));
}


void ConfPricesPriv::slotDown()
{
    if (m_ui->tableWidget->rowCount() <= 1) return;

    int i = m_ui->tableWidget->currentRow();
    QAbstractItemModel* model = m_ui->tableWidget->model();
    if ( (model == NULL) || (i == m_ui->tableWidget->rowCount() - 1) ) return;

    QString strPos = model->index(i + 1, 0).data().toString();
    QString strPrice = model->index(i + 1, 1).data().toString();
    model->setData(model->index(i + 1, 0), model->index(i, 0).data().toString());
    model->setData(model->index(i + 1, 1), model->index(i, 1).data().toString());
    model->setData(model->index(i, 0), strPos);
    model->setData(model->index(i, 1), strPrice);
    m_ui->tableWidget->setCurrentIndex(model->index(i + 1, 0));
}


void ConfPricesPriv::loadTable(int i)
{
    disconnect(m_ui->tableWidget, &QTableWidget::cellChanged, this, &ConfPricesPriv::slotModified);
    disconnect(m_ui->doubleSpinBox, &QDoubleSpinBox::valueChanged, this, &ConfPricesPriv::slotModified);
    disconnect(m_ui->doubleSpinBox_2, &QDoubleSpinBox::valueChanged, this, &ConfPricesPriv::slotModified);
    flag_change = false;
    cpt.clear();
    m_ui->tableWidget->clearContents();
    while (m_ui->tableWidget->rowCount() > 0)
	m_ui->tableWidget->removeRow(0);

    if (db.loadPriceTablePrivate(-1, i + PRICETABLE_TYPE_PRIVAT, cpt)) {
	QAbstractItemModel* model;
	model = m_ui->tableWidget->model();
	int co = cpt.count();
	if (co - 2 > 0) {
	    // decrease by 2 because we handle "Hausbesuch" and "Kilometerpauschale" separately
	    for (int i = 0; i < co - 2; i++) {
		m_ui->tableWidget->insertRow(0);
	    }
	    for (int i = 0; i < co; i++) {
		if (cpt.getPos(i) == "Hausbesuch") {
		    m_ui->doubleSpinBox_2->setValue(cpt.getPrice(i).toFloat());
		} else if (cpt.getPos(i) == "Kilometerpauschale") {
		    m_ui->doubleSpinBox->setValue(cpt.getPrice(i).toFloat());
		} else {
		    model->setData(model->index(i, 0), cpt.getPos(i));
		    model->setData(model->index(i, 1), cpt.getPrice(i));
		}
	    }
	}
	m_ui->tableWidget->resizeColumnsToContents();
    }

    connect(m_ui->tableWidget, &QTableWidget::cellChanged, this, &ConfPricesPriv::slotModified);
    connect(m_ui->doubleSpinBox, &QDoubleSpinBox::valueChanged, this, &ConfPricesPriv::slotModified);
    connect(m_ui->doubleSpinBox_2, &QDoubleSpinBox::valueChanged, this, &ConfPricesPriv::slotModified);
}


void ConfPricesPriv::slotTableChanged(int i)
{
    if (i < 0) return;
    if (flag_change) {
	MsgBox box;
        QString msg = QString::fromUtf8("Sie haben Änderungen an der Preistabelle ");
	msg += m_ui->cbTable->currentText();
        msg += QString::fromUtf8(" vorgenommen.\n\nSoll die Tabelle gespeichert werden?");
	if (box.execYesNoBox(msg, MsgBox::BTN_YES) == MsgBox::BTN_YES) {
	    slotSave();
	}
	// ask whether to save changes
	flag_change = false;
    }
    loadTable(i);
}


void ConfPricesPriv::slotModified()
{
    flag_change = true;
}
