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

#include "confbase.h"


ConfBase::ConfBase(QWidget *parent, int l, int a) :
    QWidget(parent)
{
    area = a;
    layer = l;
    title = keyword = "";
    flag_change = false;
}


ConfBase::~ConfBase()
{
}

void ConfBase::setTitle(QString s) { title = s; }
QString ConfBase::getTitle() { return title; }
void ConfBase::setKeyword(QString s) { keyword = s; }
QString ConfBase::getKeyword() { return keyword; }
int ConfBase::getArea() { return area; }
int ConfBase::getLayer() { return layer; }

void ConfBase::init() {}
bool ConfBase::write() { return true; }
