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

#ifndef DLGDOCOPEN_H
#define DLGDOCOPEN_H

#include <QtWidgets>
#include "mydb.h"

namespace Ui {
    class DlgDocOpen;
}

class DlgDocOpen : public QDialog
{
    Q_OBJECT

public:
    explicit DlgDocOpen(QWidget *parent = 0);
    ~DlgDocOpen();
    cTemplate getSelectedTemplate();

private:
    Ui::DlgDocOpen *ui;
    QString selTemplate;
    QList<cTemplateType*> listTemplateTypes;
    QList<cTemplate*> listTemplates;
    void init(bool re_init);

private slots:
    void slotItemSelected(QTreeWidgetItem *w, int column);
    void slotItemChanged(QTreeWidgetItem *w);
    void slotOpen();
    void slotDel();
};

#endif // DLGDOCOPEN_H
