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

#include "conftextmodules.h"
#include "ui_conftextmodules.h"


ConfTextModules::ConfTextModules(QWidget *parent, int l, int a) :
    ConfBase(parent, l, a),
    ui(new Ui::ConfTextModules)
{
    ui->setupUi(this);
    setTitle(QString::fromUtf8("Textbausteine"));
    setKeyword("opa_textmodules");
}


ConfTextModules::~ConfTextModules()
{
    delete ui;
    while (!text_modules.isEmpty()) delete text_modules.takeLast();
}


void ConfTextModules::init()
{
    ui->tmPatientDlg->clear();
    ui->tmDateDlg->clear();
    while (!text_modules.isEmpty()) delete text_modules.takeLast();
    db.retrieveItemList(myDB::TABLE_TEXT_MODULES, text_modules);
    for (int i = 0; i < text_modules.count(); i++) {
        cTextModule *ctm = (cTextModule*)(text_modules.at(i));
        if (ctm->getDomain() == cTextModule::TM_PATIENTS) {
            ui->tmPatientDlg->appendPlainText(ctm->getText());
        } else if (ctm->getDomain() == cTextModule::TM_DATES) {
            ui->tmDateDlg->appendPlainText(ctm->getText());
        }
    }
    ui->cbAddDate->setChecked((bool)myConf.get(ConfXML::AREA_TEXT_MODULES, "autodate").toInt());
}


bool ConfTextModules::write()
{
    if (ui->cbAddDate->isChecked()) myConf.set(ConfXML::AREA_TEXT_MODULES, "autodate", "1");
    else myConf.set(ConfXML::AREA_TEXT_MODULES, "autodate", "0");

    // delete existing items by marking them accordingly
    for (int i = 0; i < text_modules.count(); i++) {
        text_modules.at(i)->setFlag(cTableItem::FLAG_DEL);
    }
    db.saveItemList(myDB::TABLE_TEXT_MODULES, text_modules, true);
    while (!text_modules.isEmpty()) delete text_modules.takeLast();

    QStringList lines;
    lines = ui->tmPatientDlg->toPlainText().split("\n", Qt::SkipEmptyParts);
    for (int i = 0; i < lines.count(); i++) {
        text_modules.append(new cTextModule());
        ((cTextModule*)(text_modules.last()))->setDomain(cTextModule::TM_PATIENTS);
        ((cTextModule*)(text_modules.last()))->setText(lines.at(i));
    }
    lines.clear();
    lines = ui->tmPatientDlg->toPlainText().split("\n", Qt::SkipEmptyParts);
    for (int i = 0; i < lines.count(); i++) {
        text_modules.append(new cTextModule());
        ((cTextModule*)(text_modules.last()))->setDomain(cTextModule::TM_DATES);
        ((cTextModule*)(text_modules.last()))->setText(lines.at(i));
    }
    if (!db.saveItemList(myDB::TABLE_TEXT_MODULES, text_modules, true)) {
        QMessageBox::warning(this, "Hinweis", QString::fromUtf8("Fehler beim Speichern aufgetreten!"), QMessageBox::Ok);
        return false;
    }
    return true;
}
