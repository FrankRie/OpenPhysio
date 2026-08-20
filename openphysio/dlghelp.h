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

#ifndef DLGHELP_H
#define DLGHELP_H

#include <QtGui>
#include "helperwidget.h"


namespace Ui {
    class DlgHelp;
}

class DlgHelp : public QDialog {
    Q_OBJECT
    Q_DISABLE_COPY(DlgHelp)
public:
    explicit DlgHelp(QWidget *parent = 0);
    virtual ~DlgHelp();
    void setKeyword(const QString &keyword);

private:
    HelperWidget *helperWidget;
    Ui::DlgHelp *m_ui;
};


#endif // DLGHELP_H
