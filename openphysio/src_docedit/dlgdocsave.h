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

#ifndef DLGDOCSAVE_H
#define DLGDOCSAVE_H

#include <QtWidgets>

namespace Ui {
    class DlgDocSave;
}

class DlgDocSave : public QDialog
{
    Q_OBJECT

public:
    explicit DlgDocSave(QWidget *parent = 0);
    ~DlgDocSave();
    void init(QString tmpl_name);
    QString getTemplateName();

private:
    Ui::DlgDocSave *ui;

private slots:
    void slotSave();
};

#endif // DLGDOCSAVE_H
