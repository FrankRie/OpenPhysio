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

#ifndef DLGDOCEDITNEW_H
#define DLGDOCEDITNEW_H

#include <QtWidgets>

namespace Ui {
    class DlgDocEditNew;
}

class DlgDocEditNew : public QDialog
{
    Q_OBJECT

public:
    explicit DlgDocEditNew(QWidget *parent = 0);
    ~DlgDocEditNew();
    void init(QStringList list);
    QString getSelectedDocType();
    int getSelectedPageSize();
    QString getName();

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private:
    Ui::DlgDocEditNew *ui;

private slots:
    void slotAccept();
};

#endif // DLGDOCEDITNEW_H
