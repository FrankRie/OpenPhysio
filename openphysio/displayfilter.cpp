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

#include "displayfilter.h"
#include "ui_displayfilter.h"
#include <QDebug>


DisplayFilter::DisplayFilter(QWidget *parent, bool auto_keyboard_focus) :
    QWidget(parent),
    m_ui(new Ui::DisplayFilter),
    auto_kbd_focus(auto_keyboard_focus)
{
    m_ui->setupUi(this);

}


DisplayFilter::~DisplayFilter()
{
    delete m_ui;
}


void DisplayFilter::init(QStringList& headers, int default_header)
{
    int i = default_header;
    if (i < 0) i = 0;

    m_ui->leFilter->setText("");
    m_ui->cbColumn->addItems(headers);
    m_ui->cbColumn->setCurrentIndex(i);
    m_ui->label->setBuddy(m_ui->leFilter);
    m_ui->label_2->setBuddy(m_ui->cbColumn);
    connect(m_ui->leFilter, SIGNAL(returnPressed()), this, SLOT(slotStart()));
}


void DisplayFilter::setFilterString(QString s)
{
    m_ui->leFilter->setText(s);
    slotStart();
}


void DisplayFilter::slotStart()
{
    int i = m_ui->cbColumn->currentIndex();
    if (i >= 0)
	emit filterChanged(m_ui->leFilter->text(), i);
}


void DisplayFilter::clearFilterField()
{
    m_ui->leFilter->setText("");
}


void DisplayFilter::setKeyboardFocus()
{
    m_ui->leFilter->setFocus();
}


bool DisplayFilter::useAutoKeyboardFocus()
{
    return auto_kbd_focus;
}


void DisplayFilter::useAutoKeyboardFocus(bool auto_keyboard_focus)
{
    auto_kbd_focus = auto_keyboard_focus;
}
