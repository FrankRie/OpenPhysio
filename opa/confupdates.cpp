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

#include "confupdates.h"
#include "ui_confupdates.h"
#include "dlgupdateinsurances.h"
#include <msgbox.h>


ConfUpdates::ConfUpdates(QWidget *parent, int l, int a) :
    ConfBase(parent, l, a),
    m_ui(new Ui::ConfUpdates)
{
    m_ui->setupUi(this);
    setTitle(QString::fromUtf8("Kostenträger-Updates"));
    setKeyword("opa_updates");

    connect(m_ui->pbInsurances, &QPushButton::clicked, this, &ConfUpdates::slotInsurances);
    connect(m_ui->pbFileInsurances, &QPushButton::clicked, this, &ConfUpdates::slotSelectInsuranceFile);
}


ConfUpdates::~ConfUpdates()
{
    delete m_ui;
}


void ConfUpdates::init()
{
    m_ui->leFileInsurances->clear();
    listInsFiles.clear();
}


bool ConfUpdates::write()
{
    return true;
}


void ConfUpdates::slotInsurances()
{
    if (listInsFiles.isEmpty()) {
        QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), QString::fromUtf8("Bitte erst eine Datei wählen!"), QMessageBox::Ok);
	return;
    }

    DlgUpdateInsurances dlg;
    if (!dlg.init(listInsFiles)) return;
    dlg.exec();
}


void ConfUpdates::slotSelectInsuranceFile()
{
    listInsFiles = QFileDialog::getOpenFileNames(this,
        QString::fromUtf8("Kostenträgerdatei laden"), ".", "Alle Dateien (*.*)");
    m_ui->leFileInsurances->setText(listInsFiles.join(" ; "));
}
