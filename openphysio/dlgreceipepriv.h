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

#ifndef DLGRECEIPEPRIV_H
#define DLGRECEIPEPRIV_H

#include <QtWidgets>
#include "ui_dlgreceipepriv.h"
#include "browsedocs.h"
#include "therapy.h"
#include "datetools.h"


class DlgReceipePriv;

class ReceipePrivDelegate : public QItemDelegate
{
private:
    cPriceTablePriv* cpt;
    const DlgReceipePriv *dlg;

public:
    ReceipePrivDelegate(cPriceTablePriv* c, QObject* parent);
    QWidget *createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex& index) const;
    void setEditorData(QWidget* editor, const QModelIndex& index) const;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const;
    void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex&) const;
};


class DlgReceipePriv : public QDialog
{
    Q_OBJECT

public:
    DlgReceipePriv(QWidget *parent = 0);
    ~DlgReceipePriv();
    void initDlg(cReceipe *r, cPatient *p = NULL);
public slots:
    void slotCommitCount(int i);
    void slotCommitPos(int i);

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private:
    Ui::DlgReceipePrivClass ui;
    ReceipePrivDelegate *delegate;
    QList<cTableItem*> receipe_states;
    cReceipe curReceipe;
    cPatient curPatient;
    QAction *actPosAdd, *actPosDel;
    cPriceTablePriv cpt;
    QStringList *listReceipeTypes;
    BrowseDocs *browseDocs;
    Therapy therapy;
    bool flag_change;
    DateEdit *deRDate;

    void loadReceipe(cPatient *p);
    void calcPos(int row, int pos_idx, int co);
    void calcSum();
    bool saveReceipe();
    void checkInput();
    void buildTableIcons();

private slots:
    void slotSwitchPriceTable(int i);
    void slotGetPatient();
    void slotGetPhysician();
    void slotCancelReceipe();
    void slotSaveReceipe();
    void slotGenerateInvoice();
    void slotPosAdd();
    void slotPosDel();
    void slotPosDel(int i);
    void slotReport();
    void slotShowPatInfo();
    void slotVisit(int i);
    void slotModified();
    void slotModifiedWithCheck();
    void slotRoute();
    void slotPrintTherapyDates();
    void slotDelPhysician();
};

#endif // DLGRECEIPEPRIV_H
