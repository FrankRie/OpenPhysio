
#include "browsepraxis.h"
#include "mydb.h"
#include "protect.h"
#include "msgbox.h"


BrowsePraxis::BrowsePraxis(int mode, QWidget *parent)
    : Browse(mode, parent)
{
    ui.setupUi(this);
    init((TableViewAnimated*)ui.tvPraxis, myDB::TABLE_PRAXIS);

    // add display filter widget and toolbar
    QGridLayout *layout = (QGridLayout*) this->layout();
    layout->addWidget(&browseToolBar, 0, 0);
    layout->addWidget(&displayFilter, 0, 1);

    if (mode == MODE_SELECT) {
        connect(ui.tvPraxis, SIGNAL(activated(const QModelIndex&)), this, SLOT(slotPraxisSelected()));
    } else {
        connect(ui.tvPraxis, SIGNAL(activated(const QModelIndex&)), this, SLOT(slotPraxisEdit()));
    }

    actPraxisAdd = new QAction(QIcon(":/new.png"), QString::fromUtf8("Neuer Praxis"), this);
    actPraxisEdit = new QAction(QIcon(":/edit.png"), QString::fromUtf8("Praxis bearbeiten"), this);
    actPraxisDel = new QAction(QIcon(":/reminder.png"), QString::fromUtf8("Praxis löschen"), this);
    connect(actPraxisAdd, SIGNAL(triggered()), this, SLOT(slotPraxisAdd()));
    connect(actPraxisEdit, SIGNAL(triggered()), this, SLOT(slotPraxisEdit()));
    connect(actPraxisDel, SIGNAL(triggered()), this, SLOT(slotPraxisDel()));
    browseToolBar.addAction(actPraxisAdd);
    browseToolBar.addSeparator();
    browseToolBar.addAction(actPraxisEdit);
    browseToolBar.addSeparator();
    browseToolBar.addAction(actPraxisDel);
    ui.tvPraxis->addAction(actPraxisAdd);
    ui.tvPraxis->addAction(actPraxisEdit);
    ui.tvPraxis->addAction(actPraxisDel);
    ui.tvPraxis->setContextMenuPolicy(Qt::ActionsContextMenu);
}


BrowsePraxis::~BrowsePraxis()
{
    if (actPraxisAdd != NULL) delete actPraxisAdd;
    if (actPraxisEdit != NULL) delete actPraxisEdit;
    if (actPraxisDel != NULL) delete actPraxisDel;
}


void BrowsePraxis::showDlg(cPraxis *p)
{
    DlgPraxis dlgPraxis;
    dlgPraxis.initPraxis(p);
    dlgPraxis.exec();
}


cPraxis& BrowsePraxis::getSelPraxis()
{
    return selPraxis;
}


void BrowsePraxis::slotPraxisSelected()
{
    int id = getSelectedItem();
    if (id < 0) return;

    db.retrieveItem(selPraxis, id);
    emit sigItemSelected();
}


void BrowsePraxis::slotPraxisAdd()
{
    GUARD(Protect::FUNC_PHY);

    // indicate that the user wants to create a new Praxisician
    DlgPraxis dlgPraxis;
    cPraxis p;
    dlgPraxis.initPraxis(&p);
    dlgPraxis.exec();
}


void BrowsePraxis::slotPraxisEdit()
{
    GUARD(Protect::FUNC_PHY);

    int id = getSelectedItem();
    if (id < 0) {
        QMessageBox::warning(this, tr("OpenPraxisio - Hinweis"), QString::fromUtf8("Bitte erst eine Praxis wählen!\n"), QMessageBox::Ok);
        return;
    }

    DlgPraxis dlgPraxis;
    cPraxis p;
    db.retrieveItem(p, id);
    dlgPraxis.initPraxis(&p);
    dlgPraxis.exec();
}


void BrowsePraxis::slotPraxisDel()
{
    GUARD(Protect::FUNC_PHY_DEL);

    int id = getSelectedItem();
    if (id < 0) {
        QMessageBox::warning(this, tr("OpenPraxisio - Hinweis"), QString::fromUtf8("Bitte erst eine Praxis wählen!\n"), QMessageBox::Ok);
        return;
    }

    MsgBox box;
    if (box.execYesNoBox(QString::fromUtf8("Praxis wirklich löschen?"), MsgBox::BTN_NO) == MsgBox::BTN_YES) {
        db.deleteItem(myDB::TABLE_PRAXIS, id);
    }
}
