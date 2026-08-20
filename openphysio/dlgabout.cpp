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

#include "dlgabout.h"

DlgAbout::DlgAbout(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);

#ifdef Q_OS_MACX
    QFile f(QCoreApplication::applicationDirPath() + "/LICENSE");
#else
    QFile f("LICENSE");
#endif
    if (f.open(QIODevice::ReadOnly)) {
        QTextStream stream(&f);
        stream.setEncoding(QStringConverter::Utf8);
        ui.plainTextEdit->setPlainText(stream.readAll());
        f.close();
    }
}


DlgAbout::~DlgAbout()
{
}


QString DlgAbout::getAboutText()
{
    return ui.label_version->text();
}


void DlgAbout::setAboutText(QString s)
{
    ui.label_version->setText(s);
}
