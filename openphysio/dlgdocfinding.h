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

#ifndef DLGDOCFINDING_H
#define DLGDOCFINDING_H

#include <QtWidgets>
#include "mydb.h"


namespace Ui {
    class DlgDocFinding;
}

class DlgDocFinding : public QDialog
{
    Q_OBJECT
    Q_DISABLE_COPY(DlgDocFinding)
public:
    explicit DlgDocFinding(QWidget *parent = 0);
    virtual ~DlgDocFinding();
    void init(cFinding& f);

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private:
    Ui::DlgDocFinding *m_ui;
    cFinding curFinding;

private slots:
    void slotCancel();
    void slotSave();
    void slotFontSize(int i);
};

#endif // DLGDOCFINDING_H
