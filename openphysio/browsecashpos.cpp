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

#include "browsecashpos.h"
#include "ui_browsecashpos.h"
#include "msgbox.h"
#include "gendoc.h"
#include "protect.h"


BrowseCashPos::BrowseCashPos(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::BrowseCashPos)
{
    m_ui->setupUi(this);
    m_ui->lcdNumber->display("0.00");

    for (int i = 0; i < db.listCashPositions.count(); i++) {
        items.append(new QTreeWidgetItem(m_ui->treeWidget));
        items.at(i)->setText(0, db.listCashPositions.at(i)->getDescription());
        items.at(i)->setIcon(0, QIcon(":/abrechnung.png"));
    }

    cur_view = -1;
    cur_id = -1;

    browseCash = new BrowseCash();
    browseReceipesAddPay = new BrowseReceipes(Browse::MODE_CASH_ADDPAY);
    browseReceipesSelfPay = new BrowseReceipes(Browse::MODE_CASH_SELFPAY);
    browseReceipesPrivReceipe = new BrowseReceipes(Browse::MODE_CASH_PRIVRECEIPE);
    widgetCashLoss = new WidgetCashLoss();
    widgetCashArticle = new WidgetCashArticle();

    last_inserted_widget = NULL;

    loadCashPos(cCashPosition::CP_OVERVIEW);

   connect(m_ui->pbCash, &QPushButton::clicked, this, &BrowseCashPos::slotCashPos);

    m_ui->treeWidget->setCurrentItem(items.at(0), 0);
    connect(m_ui->treeWidget, &QTreeWidget::currentItemChanged, this, &BrowseCashPos::slotSelected);

    connect(browseReceipesAddPay, &BrowseReceipes::sigAmountChanged, this, &BrowseCashPos::slotAmountChanged);
    connect(browseReceipesSelfPay, &BrowseReceipes::sigAmountChanged, this, &BrowseCashPos::slotAmountChanged);
    connect(browseReceipesPrivReceipe, &BrowseReceipes::sigAmountChanged, this,  &BrowseCashPos::slotAmountChanged);
    connect(widgetCashLoss, &WidgetCashLoss::sigAmountChanged, this,  &BrowseCashPos::slotAmountChanged);
    connect(widgetCashArticle, &WidgetCashArticle::sigAmountChanged, this,  &BrowseCashPos::slotAmountChanged);
}


BrowseCashPos::~BrowseCashPos()
{
    if (browseReceipesAddPay != NULL) delete browseReceipesAddPay;
    if (browseReceipesSelfPay != NULL) delete browseReceipesSelfPay;
    if (browseReceipesPrivReceipe != NULL) delete browseReceipesPrivReceipe;
    if (browseCash != NULL) delete browseCash;
    if (widgetCashLoss != NULL) delete widgetCashLoss;
    if (widgetCashArticle != NULL) delete widgetCashArticle;
    disconnect(m_ui->treeWidget, &QTreeWidget::currentItemChanged, this, &BrowseCashPos::slotSelected);
    while (!items.isEmpty()) delete items.takeFirst();
    delete m_ui;
}


void BrowseCashPos::removeCurrentView()
{
    if ( (cur_view < 0) ) return;

    if (last_inserted_widget != NULL) {
        QGridLayout *qgl = (QGridLayout *) m_ui->gridLayoutMain;
        qgl->removeWidget(last_inserted_widget);
        last_inserted_widget = NULL;
    }
    m_ui->lcdNumber->display("0.00");

    switch(cur_view) {
        case cCashPosition::CP_OVERVIEW:
            browseCash->hide();
            break;
        case cCashPosition::CP_COURSE:
            break;
        case cCashPosition::CP_LOSS:
            widgetCashLoss->hide();
            break;
        case cCashPosition::CP_ADDPAY:
            browseReceipesAddPay->hide();
            break;
        case cCashPosition::CP_SELFPAY:
            browseReceipesSelfPay->hide();
            break;
        case cCashPosition::CP_PRIVRECEIPE:
            browseReceipesPrivReceipe->hide();
            break;
        case cCashPosition::CP_VOUCHER:
            break;
        case cCashPosition::CP_ARTICLE:
            widgetCashArticle->hide();
            break;
        default: break;
    }
}


void BrowseCashPos::loadCashPos(int i)
{
    GUARD_SILENT(Protect::FUNC_CASH_ACCESS);

    if ( (i == cur_view) || (i < 0) || (i >= db.listCashPositions.count()) ) return;
    removeCurrentView();
    cur_view = i;

    QWidget *widget = NULL;
    m_ui->groupBox1->setTitle(db.listCashPositions.at(i)->getDescription());
    if (i > 0) {
        m_ui->pbCash->setText("&Kassieren:\n" + db.listCashPositions.at(i)->getDescription());
        m_ui->groupBox2->show();
    } else {
        m_ui->groupBox2->hide();
    }

    switch(cur_view) {
        case cCashPosition::CP_OVERVIEW:
            widget = browseCash;
            break;
        case cCashPosition::CP_ADDPAY:
            m_ui->groupBox1->setTitle(m_ui->groupBox1->title() + QString::fromUtf8(": Offene Zuzahlungen für GKV Rezepte"));
            widget = browseReceipesAddPay;
            break;
        case cCashPosition::CP_COURSE:
            break;
        case cCashPosition::CP_LOSS:
            widget = widgetCashLoss;
            break;
        case cCashPosition::CP_SELFPAY:
            widget = browseReceipesSelfPay;
            break;
        case cCashPosition::CP_PRIVRECEIPE:
            widget = browseReceipesPrivReceipe;
            break;
        case cCashPosition::CP_VOUCHER:
            break;
        case cCashPosition::CP_ARTICLE:
            widget = widgetCashArticle;
            break;
        default: break;
    }

    if (widget != NULL) {
        QGridLayout *qgl = (QGridLayout *) m_ui->gridLayoutMain;
        qgl->addWidget(widget, 0, 0);
        last_inserted_widget = widget;
        widget->show();
    }
}


void BrowseCashPos::slotSelected(QTreeWidgetItem *w)
{
    if (w == NULL) return;
    if (w->columnCount() < 1) return;

    for (int i = 0; i < db.listCashPositions.count(); i++) {
        if (w->text(0) == db.listCashPositions.at(i)->getDescription()) {
            loadCashPos(i);
            break;
        }
    }
}


void BrowseCashPos::slotCashPos()
{
    GUARD(Protect::FUNC_CASH_TAKE);

    double d = m_ui->lcdNumber->value();
    if ( (d <= 0) || (cur_id < 0) || (cur_view < 1) ) return;

    QString amount;
    QDate date = QDate::currentDate();
    bool print_quittance = false;
    bool print_invoice = false;

    amount.asprintf("%.2f", d);

    QDialog dlg;
    QGridLayout layout(&dlg);

    QCheckBox check1, check2;
    QLabel label1, label2;
    QDateEdit dateEdit;
    QPushButton pbs("Ja");
    QPushButton pbc("Nein");

    check1.setText("Quittung erstellen?");
    check2.setText("Rechnung erstellen");
    check1.setCheckState(Qt::Checked);
    check2.setCheckState(Qt::Unchecked);
    check2.setDisabled(true);
    label1.setText(QString::fromUtf8("Den Betrag in Höhe von EUR ") + amount + " verbuchen?");
    label2.setText("Buchungsdatum:");
    dateEdit.setCalendarPopup(true);
    dateEdit.calendarWidget()->setFirstDayOfWeek(Qt::Monday);
    dateEdit.calendarWidget()->setGridVisible(true);
    dateEdit.setDate(QDate::currentDate());

    layout.addWidget(&label1, 0, 0, 1, 2);
    layout.addWidget(&label2, 1, 0, 1, 1);
    layout.addWidget(&dateEdit, 1, 1, 1, 1);
    layout.addWidget(&check1, 2, 0, 1, 2);
    layout.addWidget(&check2, 3, 0, 1, 2);
    layout.addWidget(&pbc, 4, 0, 1, 1);
    layout.addWidget(&pbs, 4, 1, 1, 1);
    pbs.setDefault(true);

    QObject::connect(&pbc, &QPushButton::clicked, &dlg, &QDialog::reject);
    QObject::connect(&pbs, &QPushButton::clicked, &dlg, &QDialog::accept);


    dlg.setWindowTitle("Kasse");
    dlg.setWindowIcon(QIcon(":/favicon.png"));
    dlg.exec();

    if (dlg.result() == QDialog::Accepted) {
        if (check1.checkState() == Qt::Checked) print_quittance = true;
        if (check2.checkState() == Qt::Checked) print_invoice = true;
        date = dateEdit.date();

        cCash cash;
        QString cdesc;
        int pid, cid, rid, xid;
        cReceipe receipe;
        cPatient patient;

        cash.setAmount(amount);
        cash.setPayDate(QDate::currentDate());
        cash.setPositionID(cur_view);
        pid = cid = rid = xid = -1;

        switch(cur_view) {
            case cCashPosition::CP_ADDPAY:  // fall through
            case cCashPosition::CP_SELFPAY:
            case cCashPosition::CP_PRIVRECEIPE:
                rid = cur_id;
                db.retrieveItem(receipe, cur_id);
                pid = receipe.getPatientID();
                db.retrieveItem(patient, pid);
                cdesc = patient.getForename() + " " + patient.getName();
                cdesc = cdesc + " (" + "Rezept vom " + receipe.getDate().toString("dd.MM.yyyy")+ ")";
                break;
            case cCashPosition::CP_COURSE:
                break;
            case cCashPosition::CP_LOSS:
                pid = cur_id;
                db.retrieveItem(patient, pid);
                cdesc = patient.getForename() + " " + patient.getName();
                break;
            case cCashPosition::CP_VOUCHER:
                break;
            case cCashPosition::CP_ARTICLE:
                break;
            default: break;
        }

        // save cash object
        cash.setPID(pid);
        cash.setRID(rid);
        cash.setCID(cid);
        cash.setXID(xid);
        cash.setDescription(cdesc);
        int store_id = -1;
        db.storeItem(cash, &store_id);

        // modify state of related objects
        if (store_id > 0) {
            if (cur_view == cCashPosition::CP_ADDPAY) {
                receipe.setAddPayDate(date);
                receipe.setAddPayFlag(1);
                db.updateItem(receipe);
            }
            if (cur_view == cCashPosition::CP_SELFPAY || cur_view == cCashPosition::CP_PRIVRECEIPE) {
                receipe.setAddPayDate(date);
                // set state to "Abgerechnet"
                receipe.setState(cReceipeState::RECEIPE_STATE_SETTLED);
                db.updateItem(receipe);
            }
        }

        if (print_quittance) {
            // generate quittance
            cQuittance quittance;
            quittance.setPatientID(pid);
            quittance.setDate(dateEdit.date());
            // map cash positions to cQuittance type
            int qtype = 0;
            switch(cur_view) {
                case cCashPosition::CP_ADDPAY: qtype = cQuittance::Q_ADDPAY; break;
                case cCashPosition::CP_SELFPAY: qtype = cQuittance::Q_SELFPAY; break;
                case cCashPosition::CP_PRIVRECEIPE: qtype = cQuittance::Q_PRIVRECEIPE; break;
                case cCashPosition::CP_COURSE: qtype = cQuittance::Q_COURSE; break;
                case cCashPosition::CP_LOSS: qtype = cQuittance::Q_LOSS; break;
                case cCashPosition::CP_VOUCHER: qtype = cQuittance::Q_VOUCHER; break;
                case cCashPosition::CP_ARTICLE: qtype = cQuittance::Q_ARTICLE; break;
                default: break;
            }
            quittance.setType(qtype);

            GenDoc gendoc;
            if (cur_view == cCashPosition::CP_ADDPAY) {
                quittance.setReceipeID(rid);
                gendoc.genQuittance(quittance);
            } else {
                gendoc.genQuittance(quittance, amount);
            }
        }

        if (print_invoice) {
            // generate invoice
        }
    }
}


void BrowseCashPos::slotAmountChanged(QString amount, int id)
{
    m_ui->lcdNumber->display(amount);
    cur_id = id;
}


void BrowseCashPos::Preload()
{
    if (myConf.usePreload()) {
        if (browseReceipesAddPay != NULL) browseReceipesAddPay->preload();
        if (browseReceipesSelfPay != NULL) browseReceipesSelfPay->preload();
        if (browseReceipesPrivReceipe != NULL) browseReceipesPrivReceipe->preload();
        if (browseCash != NULL) browseCash->preload();
    }
}
