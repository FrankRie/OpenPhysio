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

#ifndef WIDGETCASHARTICLE_H
#define WIDGETCASHARTICLE_H

#include <QtWidgets>

namespace Ui {
    class WidgetCashArticle;
}

class WidgetCashArticle : public QWidget {
    Q_OBJECT
public:
    WidgetCashArticle(QWidget *parent = 0);
    ~WidgetCashArticle();

signals:
    void sigAmountChanged(QString amount, int id);

private:
    Ui::WidgetCashArticle *m_ui;
};

#endif // WIDGETCASHARTICLE_H
