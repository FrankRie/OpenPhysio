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

#include "confdisplay.h"
#include "ui_confdisplay.h"

ConfDisplay::ConfDisplay(QWidget *parent, int l, int a) :
    ConfBase(parent, l, a),
    m_ui(new Ui::ConfDisplay)
{
    m_ui->setupUi(this);
    setTitle("Darstellung");
    setKeyword("opa_display");
}

ConfDisplay::~ConfDisplay()
{
    delete m_ui;
}


void ConfDisplay::init()
{
    m_ui->spFontSize->setValue(myConf.get(ConfXML::AREA_DISPLAY, "fontsize_findings").toInt());

    if (myConf.isTableColorMode()) m_ui->cbTableColor->setCheckState(Qt::Checked);
    else m_ui->cbTableColor->setCheckState(Qt::Unchecked);

    QPalette pal;
    pal = m_ui->widgetRunning->palette();
    pal.setColor(QPalette::Window, myConf.getColorFromString(myConf.get(ConfXML::AREA_DISPLAY, "colRunning")));
    m_ui->widgetRunning->setPalette(pal);

    pal = m_ui->widgetDone->palette();
    pal.setColor(QPalette::Window, myConf.getColorFromString(myConf.get(ConfXML::AREA_DISPLAY, "colDone")));
    m_ui->widgetDone->setPalette(pal);

    pal = m_ui->widgetReminder->palette();
    pal.setColor(QPalette::Window, myConf.getColorFromString(myConf.get(ConfXML::AREA_DISPLAY, "colReminder")));
    m_ui->widgetReminder->setPalette(pal);

    pal = m_ui->widgetPayed->palette();
    pal.setColor(QPalette::Window, myConf.getColorFromString(myConf.get(ConfXML::AREA_DISPLAY, "colPayed")));
    m_ui->widgetPayed->setPalette(pal);

    pal = m_ui->widgetCleared->palette();
    pal.setColor(QPalette::Window, myConf.getColorFromString(myConf.get(ConfXML::AREA_DISPLAY, "colCleared")));
    m_ui->widgetCleared->setPalette(pal);

    pal = m_ui->widgetOpen->palette();
    pal.setColor(QPalette::Window, myConf.getColorFromString(myConf.get(ConfXML::AREA_DISPLAY, "colOpen")));
    m_ui->widgetOpen->setPalette(pal);

    pal = m_ui->widgetCanceled->palette();
    pal.setColor(QPalette::Window, myConf.getColorFromString(myConf.get(ConfXML::AREA_DISPLAY, "colCanceled")));
    m_ui->widgetCanceled->setPalette(pal);

    pal = m_ui->widgetInactive->palette();
    pal.setColor(QPalette::Window, myConf.getColorFromString(myConf.get(ConfXML::AREA_DISPLAY, "colInactive")));
    m_ui->widgetInactive->setPalette(pal);

    connect(m_ui->pbRunning, &QPushButton::clicked, this, &ConfDisplay::slotColor);
    connect(m_ui->pbDone, &QPushButton::clicked, this, &ConfDisplay::slotColor);
    connect(m_ui->pbReminder, &QPushButton::clicked, this, &ConfDisplay::slotColor);
    connect(m_ui->pbPayed, &QPushButton::clicked, this, &ConfDisplay::slotColor);
    connect(m_ui->pbCleared, &QPushButton::clicked, this, &ConfDisplay::slotColor);
    connect(m_ui->pbOpen, &QPushButton::clicked, this, &ConfDisplay::slotColor);
    connect(m_ui->pbCanceled, &QPushButton::clicked, this, &ConfDisplay::slotColor);
    connect(m_ui->pbInactive, &QPushButton::clicked, this, &ConfDisplay::slotColor);
}


bool ConfDisplay::write()
{
    myConf.set(ConfXML::AREA_DISPLAY, "fontsize_findings", QString::number(m_ui->spFontSize->value()));
    if (m_ui->cbTableColor->checkState() == Qt::Checked) myConf.set(ConfXML::AREA_DISPLAY, "table_colormode", "true");
    else myConf.set(ConfXML::AREA_DISPLAY, "table_colormode", "false");

    myConf.set(ConfXML::AREA_DISPLAY, "colRunning", myConf.getStringFromColor(m_ui->widgetRunning->palette().color(QPalette::Window)));
    myConf.set(ConfXML::AREA_DISPLAY, "colDone", myConf.getStringFromColor(m_ui->widgetDone->palette().color(QPalette::Window)));
    myConf.set(ConfXML::AREA_DISPLAY, "colReminder", myConf.getStringFromColor(m_ui->widgetReminder->palette().color(QPalette::Window)));
    myConf.set(ConfXML::AREA_DISPLAY, "colPayed", myConf.getStringFromColor(m_ui->widgetPayed->palette().color(QPalette::Window)));
    myConf.set(ConfXML::AREA_DISPLAY, "colCleared", myConf.getStringFromColor(m_ui->widgetCleared->palette().color(QPalette::Window)));
    myConf.set(ConfXML::AREA_DISPLAY, "colOpen", myConf.getStringFromColor(m_ui->widgetOpen->palette().color(QPalette::Window)));
    myConf.set(ConfXML::AREA_DISPLAY, "colCanceled", myConf.getStringFromColor(m_ui->widgetCanceled->palette().color(QPalette::Window)));
    myConf.set(ConfXML::AREA_DISPLAY, "colInactive", myConf.getStringFromColor(m_ui->widgetInactive->palette().color(QPalette::Window)));
    return true;
}


void ConfDisplay::slotColor()
{
    QString s = this->sender()->objectName();
    // cut of "pb" from button name and add "widget" as prefix
    // (e.g. pbRunning => widgetRunning)
    s = "widget" + s.right(s.length() - 2);
    QWidget *w = this->findChild<QWidget *>(s);
    if (w == NULL) return;

    QColorDialog dlg;
    dlg.setCurrentColor(w->palette().color(QPalette::Window));
    if (dlg.exec() == QDialog::Accepted) {
        QPalette pal = w->palette();
        pal.setColor(QPalette::Window, dlg.selectedColor());
        w->setPalette(pal);
    }
}
