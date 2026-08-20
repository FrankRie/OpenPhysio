
#include "dlgpinvoices.h"
#include "mydb.h"
#include "gendoc.h"
#include "protect.h"
#include "msgbox.h"
#include "browsepraxis.h"
#include "browsepinvoices.h"
#include "dlgbrowse.h"


DlgPraxisInvoices::DlgPraxisInvoices(QWidget *parent)
    : QDialog(parent)
{
        xid = -1;
        from.setDate(0, 0, 0);
        until.setDate(0, 0, 0);

        ui.setupUi(this);

        connect(ui.pbCancel, &QPushButton::clicked, this, &DlgPraxisInvoices::slotCancelPraxisInvoice);
        connect(ui.pbGenInvoice, &QPushButton::clicked, this, &DlgPraxisInvoices::slotGeneratePraxisInvoice);
        connect(ui.pbPraxis, &QPushButton::clicked, this, &DlgPraxisInvoices::slotGetPraxis);

        curPraxisInvoice.reset();
        flag_change = false;

        ui.deFrom->calendarWidget()->setFirstDayOfWeek(Qt::Monday);
        ui.deUntil->calendarWidget()->setFirstDayOfWeek(Qt::Monday);
        ui.deIdate->calendarWidget()->setFirstDayOfWeek(Qt::Monday);
        ui.deIdate->setDate(QDate::currentDate());

        rdate = ui.deIdate->date();

        connect(ui.lePraxis1, &QLineEdit::textChanged, this, &DlgPraxisInvoices::slotModified);
        connect(ui.lePraxis2, &QLineEdit::textChanged, this, &DlgPraxisInvoices::slotModified);
        connect(ui.deFrom, &QDateEdit::dateChanged, this, &DlgPraxisInvoices::slotStartDateChanged);
        connect(ui.deUntil, &QDateEdit::dateChanged, this, &DlgPraxisInvoices::slotEndDateChanged);
        connect(ui.deIdate, &QDateEdit::dateChanged, this, &DlgPraxisInvoices::slotInvoiceDateChanged);

}

DlgPraxisInvoices::~DlgPraxisInvoices()
{

}

//
// initPraxisInvoices
//

void DlgPraxisInvoices::initPraxisInvoices(cPraxisInvoice *p)
{
    if (p != NULL) {
        //cPraxis prx;
        //ui.lePraxis1->setText(prx(xid)->getName1());
        //ui.lePraxis2->setText(prx(xid)->getName2());
        ui.deFrom->setDate(p->getSdate());
        ui.deUntil->setDate(p->getEdate());

    } else {
        curPraxisInvoice.reset();
        ui.deIdate->setDate(QDate::currentDate());
        //checkInput();
    }

    flag_change = false;
}

void DlgPraxisInvoices::reject()
{
    if (flag_change) {
        MsgBox box;
        if (box.execYesNoBox(QString::fromUtf8("Wollen Sie die Änderungen jetzt speichern?"), MsgBox::BTN_YES) == MsgBox::BTN_YES) {
            if (!savePraxisInvoices()) return;
            flag_change = false;
            QDialog::accept();
            return;
        } else QDialog::reject();
    }
    QDialog::reject();
}

QString DlgPraxisInvoices::getPraxisInvoicesText()
{
    return ""; //ui.label_version->text();
}


void DlgPraxisInvoices::setPraxisInvoicesText(QString s)
{
    s = "" ; //ui.label_version->setText(s);
}

void DlgPraxisInvoices::slotGetPraxis()
{
    BrowsePraxis *browsePraxis = new BrowsePraxis(Browse::MODE_SELECT);
    DlgBrowse browse(browsePraxis);
    if (myConf.usePreload()) browsePraxis->preload();
    if (browse.exec() == QDialog::Accepted) {
        cPraxis p = browsePraxis->getSelPraxis();
        ui.lePraxis1->setText(p.getName1());
        ui.lePraxis2->setText(p.getName2());
        curPraxisInvoice.setPraxisID(p.getID());
        xid = p.getID();
    }
    delete browsePraxis;
}

void DlgPraxisInvoices::slotGeneratePraxisInvoice()
{
    QList<cPraxisInvoice*> invoices;

    QDate nulld;
    nulld.setDate(0, 0, 0);

    if (xid < 1) {
        QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), QString::fromUtf8("Eine Praxis muss ausgewählt sein!\n"), QMessageBox::Ok);
        return;
    }

    if (from == nulld) {
        QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), QString::fromUtf8("Ein Startdutum muss eingegeben werden!\n"), QMessageBox::Ok);
        return;
    }

    if (until == nulld) {
        QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), QString::fromUtf8("Ein Enddatum muss eungegeben werden\n"), QMessageBox::Ok);
        return;
    }

    curPraxisInvoice.setDate(rdate);
    curPraxisInvoice.setNumber(generateNumbering());
    curPraxisInvoice.setPatientID(-1);
    curPraxisInvoice.setReceipeID(-1);
    curPraxisInvoice.setCourseID(-1);
    curPraxisInvoice.setPraxisID(xid);
    curPraxisInvoice.setType(cInvoice::INVOICE_PRAXIS);
    curPraxisInvoice.setPayStat(0);
    //prx.setInvData(adate);
    curPraxisInvoice.setSdate(from);
    curPraxisInvoice.setEdate(until);
    curPraxisInvoice.setRlist("");

    qDebug() << __FUNCTION__ << curPraxisInvoice.getID()  << rdate.toString("dd.MM.yyyy");

    bool bRet = true;
    if (curPraxisInvoice.getID() < 0) {  // store new Invoice in database
        int id;
        if ( db.storeItem(curPraxisInvoice, &id) == false) {
            bRet = false;
            QMessageBox::warning(this, tr("OpenPhysio"),
                tr("Fehler beim Anlegen der Rechnung aufgetreten!\n"),
                QMessageBox::Ok);
        } else curPraxisInvoice.setID(id);
    } else { // update properties of existing physician
        if ( db.updateItem(curPraxisInvoice) == false) {
            bRet = false;
            QMessageBox::warning(this, tr("OpenPhysio"),
                tr("Fehler beim Aktualisieren der Rechnung aufgetreten!\n"),
                QMessageBox::Ok);
        }
    }

    invoices.append(new cPraxisInvoice());
    db.setCurrentNumbering(curPraxisInvoice.getNumber(), myDB::NR_INVOICE);
    db.retrieveItem(*invoices.first(), curPraxisInvoice.getID());
    gendoc.genPraxisAppendix(invoices);

    QDialog::reject();
}

void DlgPraxisInvoices::slotCancelPraxisInvoice()
{
    this->reject();
}

void DlgPraxisInvoices::slotModified()
{
    flag_change = true;
}

void DlgPraxisInvoices::slotStartDateChanged(QDate d)
{
    if (ui.deUntil->date() < d) ui.deUntil->setDate(d);
    flag_change = true;

    from = d;
}


void DlgPraxisInvoices::slotEndDateChanged(QDate d)
{
    if (d < ui.deFrom->date()) ui.deFrom->setDate(d);
    flag_change = true;

    until = d;
}

void DlgPraxisInvoices::slotInvoiceDateChanged(QDate d)
{
    flag_change = true;

    until = d;
}


bool DlgPraxisInvoices::savePraxisInvoices()
{
    flag_change = true;

    return true;
}

QString DlgPraxisInvoices::generateNumbering()
{
    QString inv_number;

    // retrieve current invoice/quittance number from database
    inv_number = db.getCurrentNumbering(myDB::NR_INVOICE);
    inv_number = QString::number(inv_number.toInt() + 1);
    return inv_number;
}



