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

#ifndef DLGDOCEDITDATA_H
#define DLGDOCEDITDATA_H

#include <QtWidgets>

class cDocTemplate;

namespace Ui {
    class DlgDocEditData;
}


class DlgDocEditData : public QDialog
{
    Q_OBJECT

public:
    explicit DlgDocEditData(QWidget *parent = 0);
    ~DlgDocEditData();
    void init(cDocTemplate *c);
    QString getSelectedDataField();

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private:
    Ui::DlgDocEditData *ui;
    QString selDataField;

private slots:
    void slotItemSelected(QTreeWidgetItem *w, int column);
    void slotItemChanged(QTreeWidgetItem *w);
    void slotAccept();
};

#endif // DLGDOCEDITDATA_H
