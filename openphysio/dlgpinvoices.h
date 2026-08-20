#ifndef DLGPINVOICES_H
#define DLGPINVOICES_H

#include <QtWidgets>
#include <QMessageBox>
#include "ui_dlgpinvoice.h"
#include "mydb.h"
#include "browsedocs.h"

class DlgPraxisInvoices : public QDialog
{
    Q_OBJECT

public:
    DlgPraxisInvoices(QWidget *parent = 0);
    ~DlgPraxisInvoices();
    QString getPraxisInvoicesText();
    void setPraxisInvoicesText(QString s);
    void initPraxisInvoices(cPraxisInvoice *);

private:
    Ui::DlgPraxisInvoices ui;
    cPraxisInvoice curPraxisInvoice;
    QList<cTableItem *> phys_types;
    BrowseDocs *browseDocs;
    bool flag_change;
    bool savePraxisInvoices();
    QDate from;
    QDate until;
    QDate rdate;
    int xid;
    QString generateNumbering();

private slots:
    void slotGetPraxis();
    void slotGeneratePraxisInvoice();
    void slotCancelPraxisInvoice();
    void slotModified();
    void reject();
    void slotStartDateChanged(QDate);
    void slotEndDateChanged(QDate);
    void slotInvoiceDateChanged(QDate);
};

#endif // DLGPINVOICES_H
