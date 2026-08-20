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

#ifndef DLGTEXTBLOCK_H
#define DLGTEXTBLOCK_H

#include <QtWidgets>
#include "mydb.h"


namespace Ui {
    class DlgTextBlock;
}

class DlgTextBlock : public QDialog
{
    Q_OBJECT

public:
    explicit DlgTextBlock(QWidget *parent = 0);
    ~DlgTextBlock();
    void init(int domain);
    QString getTextModule();

private:
    Ui::DlgTextBlock *ui;
    QList<cTextModule*> listTextModules;

private slots:
    void slotSelectText();
};

#endif // DLGTEXTBLOCK_H
