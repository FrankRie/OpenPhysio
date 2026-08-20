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

#include "confdocs.h"
#include "ui_confdocs.h"
#include "src_docedit/dlgdoceditor.h"


ConfDocs::ConfDocs(QWidget *parent, int l, int a) :
    ConfBase(parent, l, a),
    m_ui(new Ui::ConfDocs)
{
    m_ui->setupUi(this);
    setTitle(tr("Dokumente und Vorlagen"));
    setKeyword("opa_docs");

    connect(m_ui->pbEditor, SIGNAL(clicked()), this, SLOT(slotEditor()));
}


ConfDocs::~ConfDocs()
{
    delete m_ui;
}


void ConfDocs::init()
{
    if (db.isConnected()) {
        m_ui->spInvoice->setEnabled(true);
        m_ui->spQuittance->setEnabled(true);
        QString inv_number = db.getCurrentNumbering(myDB::NR_INVOICE);
        m_ui->spInvoice->setValue(inv_number.toInt());
        QString quitt_number = db.getCurrentNumbering(myDB::NR_QUITTANCE);
        m_ui->spQuittance->setValue(quitt_number.toInt());
    } else {
        m_ui->spInvoice->setEnabled(false);
        m_ui->spQuittance->setEnabled(false);
    }
}


bool ConfDocs::write()
{
    if (db.isConnected()) {
        db.setCurrentNumbering(QString::number(m_ui->spInvoice->value()), myDB::NR_INVOICE);
        db.setCurrentNumbering(QString::number(m_ui->spQuittance->value()), myDB::NR_QUITTANCE);
    }
    return true;
}


void ConfDocs::slotEditor()
{
    DlgDocEditor *dlg = new DlgDocEditor(this);
    if (dlg) {
        dlg->exec();
        delete dlg;
    }
}
