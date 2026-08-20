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

#ifndef DLGRECEIPE_H
#define DLGRECEIPE_H

#include <QtWidgets>
#include "ui_dlgreceipe.h"
#include "browsepatients.h"
#include "browsephysicians.h"
#include "browseroutes.h"
#include "therapy.h"
#include "dlgreceipeoptions.h"
#include "browsedocs.h"
#include "mydb.h"
#include "datetools.h"


class DlgReceipe;

class MatchDelegate : public QItemDelegate
{
private:
    cPriceTable* cpt;
    const DlgReceipe *dlg;

public:
    MatchDelegate(cPriceTable *c = NULL, QObject* parent = NULL);
    QWidget *createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex& index) const;
    void setEditorData(QWidget* editor, const QModelIndex& index) const;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index)  const;
    void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex&) const;
};


class ShortcutButton : public QPushButton
{
private:
    int amount;
    QString posnr;

public:
    ShortcutButton(QWidget * parent = 0);
    ~ShortcutButton();
    int getAmount();
    void setAmount(int i);
    QString getPosNr();
    void setPosNr(QString s);
};

class DlgReceipe : public QDialog
{
    Q_OBJECT

public:
    DlgReceipe(QWidget *parent = 0);
    ~DlgReceipe();
    void initDlg(cReceipe *r, int prof, cPatient *p = NULL);

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private:
    Ui::DlgReceipeClass ui;
    MatchDelegate* delegate;
    QList<cTableItem*> indicators;
    QList<cTableItem*> receipe_states;
    QList<ShortcutButton*> shortcut_buttons;
    bool auto_calc;
    bool auto_calc_hb;
    cPriceTable cpt;
    cReceipe curReceipe;
    cPatient curPatient;
    cInsurance curInsurance;
    QAction *actPosDel;
    BrowseDocs *browseDocs;
    Therapy therapy;
    bool flag_change;
    int curProfession;
    DateEdit *deRDate, *deStartDate, *deBill, *deAddPay;
    int idx_hb_flat, idx_hb_dist;

    void loadReceipe(cPatient& patient);
    void calcPos(int row, int pos_idx, int co);
    bool saveReceipe();
    void updateInsuranceText(bool clear);
    void loadPriceTable();
    void checkInput();
    void buildShortcutButtons();
    void buildTableIcons();

public slots:
    void reject();
    void slotCommitPos(int i);
    void slotCommitCount(int i);

private slots:
    void slotGetInsurant();
    void slotGetPhysician();
    void slotCancelReceipe();
    void slotSaveReceipe();
    void slotCalculate();
    void slotPrint();
    void slotCheck();
    void slotReport();
    void slotDistance();
    void slotPosDel();
    void slotPosDel(int i);
    void slotPosChanged(int row, int column);
    void slotOptions();
    void slotAddPay(int);
    void slotBill(int);
    void slotCharge(int);
    void slotQuittance();
    void slotDateChangeAddPay(const QDate& date);
    void slotShowPatInfo();
    void slotPrintTherapyDates();
    void slotVisit(int i);
    void slotHBCount();
    void slotManualHB(int i);
    //void slotSelectInsurance();
    void slotModifiedWithCheck();
    void slotModified();
    void slotShortcut();
    void slotDelPhysician();
    void slotReceipeDate(const QDate& date);
    void slotHBPosFlat(int i);
    void slotHBPosDist(int i);
    void slotVisitDistance(int i);
};

#endif // DLGRECEIPE_H
