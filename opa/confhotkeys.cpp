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

#include "confhotkeys.h"
#include "ui_confhotkeys.h"


ConfHotkeys::ConfHotkeys(QWidget *parent, int l, int a) :
    ConfBase(parent, l, a),
    m_ui(new Ui::ConfHotkeys)
{
    m_ui->setupUi(this);
    setTitle("Hotkeys");
    setKeyword("opa_hotkeys");
}


ConfHotkeys::~ConfHotkeys()
{
    delete m_ui;
}


void ConfHotkeys::init()
{
    QStringList listHotkeyFunctions;
    db.loadHotKeyFunctions(listHotkeyFunctions);

    m_ui->cbF1->addItems(listHotkeyFunctions);
    m_ui->cbF1->setCurrentIndex(myConf.get(ConfXML::AREA_HOTKEYS, "F01").toInt());
    m_ui->cbF2->addItems(listHotkeyFunctions);
    m_ui->cbF2->setCurrentIndex(myConf.get(ConfXML::AREA_HOTKEYS, "F02").toInt());
    m_ui->cbF3->addItems(listHotkeyFunctions);
    m_ui->cbF3->setCurrentIndex(myConf.get(ConfXML::AREA_HOTKEYS, "F03").toInt());
    m_ui->cbF4->addItems(listHotkeyFunctions);
    m_ui->cbF4->setCurrentIndex(myConf.get(ConfXML::AREA_HOTKEYS, "F04").toInt());
    m_ui->cbF5->addItems(listHotkeyFunctions);
    m_ui->cbF5->setCurrentIndex(myConf.get(ConfXML::AREA_HOTKEYS, "F05").toInt());
    m_ui->cbF6->addItems(listHotkeyFunctions);
    m_ui->cbF6->setCurrentIndex(myConf.get(ConfXML::AREA_HOTKEYS, "F06").toInt());
    m_ui->cbF7->addItems(listHotkeyFunctions);
    m_ui->cbF7->setCurrentIndex(myConf.get(ConfXML::AREA_HOTKEYS, "F07").toInt());
    m_ui->cbF8->addItems(listHotkeyFunctions);
    m_ui->cbF8->setCurrentIndex(myConf.get(ConfXML::AREA_HOTKEYS, "F08").toInt());
    m_ui->cbF9->addItems(listHotkeyFunctions);
    m_ui->cbF9->setCurrentIndex(myConf.get(ConfXML::AREA_HOTKEYS, "F09").toInt());
    m_ui->cbF10->addItems(listHotkeyFunctions);
    m_ui->cbF10->setCurrentIndex(myConf.get(ConfXML::AREA_HOTKEYS, "F10").toInt());
    m_ui->cbF11->addItems(listHotkeyFunctions);
    m_ui->cbF11->setCurrentIndex(myConf.get(ConfXML::AREA_HOTKEYS, "F11").toInt());
    m_ui->cbF12->addItems(listHotkeyFunctions);
    m_ui->cbF12->setCurrentIndex(myConf.get(ConfXML::AREA_HOTKEYS, "F12").toInt());
}


bool ConfHotkeys::write()
{
    myConf.set(ConfXML::AREA_HOTKEYS, "F01", QString::number(m_ui->cbF1->currentIndex()));
    myConf.set(ConfXML::AREA_HOTKEYS, "F02", QString::number(m_ui->cbF2->currentIndex()));
    myConf.set(ConfXML::AREA_HOTKEYS, "F03", QString::number(m_ui->cbF3->currentIndex()));
    myConf.set(ConfXML::AREA_HOTKEYS, "F04", QString::number(m_ui->cbF4->currentIndex()));
    myConf.set(ConfXML::AREA_HOTKEYS, "F05", QString::number(m_ui->cbF5->currentIndex()));
    myConf.set(ConfXML::AREA_HOTKEYS, "F06", QString::number(m_ui->cbF6->currentIndex()));
    myConf.set(ConfXML::AREA_HOTKEYS, "F07", QString::number(m_ui->cbF7->currentIndex()));
    myConf.set(ConfXML::AREA_HOTKEYS, "F08", QString::number(m_ui->cbF8->currentIndex()));
    myConf.set(ConfXML::AREA_HOTKEYS, "F09", QString::number(m_ui->cbF9->currentIndex()));
    myConf.set(ConfXML::AREA_HOTKEYS, "F10", QString::number(m_ui->cbF10->currentIndex()));
    myConf.set(ConfXML::AREA_HOTKEYS, "F11", QString::number(m_ui->cbF11->currentIndex()));
    myConf.set(ConfXML::AREA_HOTKEYS, "F12", QString::number(m_ui->cbF12->currentIndex()));
    return true;
}
