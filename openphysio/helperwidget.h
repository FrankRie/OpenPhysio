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

#ifndef HELPERWIDGET_H
#define HELPERWIDGET_H

#include <QtGui>
#include <QtHelp>
#include <QHelpEngine>

class HelperWidget : public QTextBrowser
{
    Q_OBJECT
public:
    HelperWidget(QWidget *parent = NULL);
    ~HelperWidget();
    void showHelpForKeyword(const QString &id);

private:
    QHelpEngine *m_helpEngine;
    QVariant loadResource(int type, const QUrl &url);
};

#endif // HELPERWIDGET_H
