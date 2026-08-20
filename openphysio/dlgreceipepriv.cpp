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

#include "dlgreceipepriv.h"
#include "mydb.h"
#include "gendoc.h"
#include "protect.h"
#include "msgbox.h"
#include "browsepatients.h"
#include "browsephysicians.h"
#include "dlgbrowse.h"
#include "helperwrapper.h"
#include "geneinv.h"


ReceipePrivDelegate::ReceipePrivDelegate(cPriceTablePriv* c, QObject* parent) : QItemDelegate(parent)
{
    cpt = c;
    dlg = (DlgReceipePriv*)parent;
}

QWidget* ReceipePrivDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex& index) const
{
    QWidget* w;

    switch( index.column() ) {
	case 0: {
	    QSpinBox *editor = new QSpinBox(parent);
	    editor->setObjectName("spin" + QString::number(index.row()));
	    editor->setMinimum(0);
	    editor->setMaximum(30);
	    w = editor;
        connect(editor, &QSpinBox::valueChanged, dlg, &DlgReceipePriv::slotCommitCount);
	    break;
	}
	case 1: {
	    QComboBox* editor = new QComboBox(parent);
	    editor->setObjectName("combo" + QString::number(index.row()));
	    if (cpt != NULL) editor->addItems(cpt->getFilteredPosNames());
	    //if (cpt != NULL) editor->addItems(cpt->qslPosName);
	    w = editor;
        connect(editor, &QComboBox::currentIndexChanged, dlg, &DlgReceipePriv::slotCommitPos);
	    break;
	}
	default:
	    w = 0;
	    break;
     }
     return w;
}

void ReceipePrivDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    QVariant value = index.model()->data( index, Qt::DisplayRole );
    	
    switch( index.column() ) {
	case 0: {
	    static_cast<QSpinBox*>(editor)->setValue(value.toInt());
	    break;
	}
	case 1: {
	    QComboBox* box = static_cast<QComboBox*>(editor);
	    box->setCurrentIndex(box->findText(value.toString()));
	    break;
	}
	default: break;
    }
}
	
void ReceipePrivDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    QVariant value;

    switch( index.column() ) {
	case 0: {
	    value = static_cast<QSpinBox*>(editor)->value();
	    break;
	}
	case 1: {
	    value = static_cast<QComboBox*>(editor)->currentText();
	    break;
	}
	default: break;
    }
    model->setData(index, value);
}

void ReceipePrivDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex&) const
{
    editor->setGeometry( option.rect );
}
    


DlgReceipePriv::DlgReceipePriv(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);

    delegate = NULL;
    curReceipe.reset();
    connect(ui.pbPatient, &QPushButton::clicked, this, &DlgReceipePriv::slotGetPatient);
    connect(ui.pbPhysician, &QPushButton::clicked, this, &DlgReceipePriv::slotGetPhysician);
    connect(ui.pbSave, &QPushButton::clicked, this, &DlgReceipePriv::slotSaveReceipe);
    connect(ui.pbCancel, &QPushButton::clicked, this, &DlgReceipePriv::slotCancelReceipe);
    connect(ui.pbGenInvoice, &QPushButton::clicked, this, &DlgReceipePriv::slotGenerateInvoice);
    connect(ui.pbReport, &QPushButton::clicked, this, &DlgReceipePriv::slotReport);
    connect(ui.pbPatInfo, &QPushButton::clicked, this, &DlgReceipePriv::slotShowPatInfo);
    connect(ui.cbVisit, &QCheckBox::checkStateChanged, this, &DlgReceipePriv::slotModified);
    connect(ui.spVisitCount, &QSpinBox::valueChanged, this, &DlgReceipePriv::slotModified);
    connect(ui.pbRoute, &QPushButton::clicked, this, &DlgReceipePriv::slotRoute);
    connect(ui.pbPrintTherapy, &QPushButton::clicked, this, &DlgReceipePriv::slotPrintTherapyDates);
    connect(ui.pbAddPos, &QPushButton::clicked, this, &DlgReceipePriv::slotPosAdd);
    connect(ui.pbDelPos, &QPushButton::clicked, this, [this]() {slotPosDel();});
    connect(ui.pbDelPhys, &QPushButton::clicked, this, &DlgReceipePriv::slotDelPhysician);

    connect(ui.cbReceipeType, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &DlgReceipePriv::slotModified);
    connect(ui.cbState, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &DlgReceipePriv::slotModified);
    connect(ui.leDiag, &QLineEdit::textChanged, this, &DlgReceipePriv::slotModified);
    connect(ui.teNotes, &QPlainTextEdit::textChanged, this, &DlgReceipePriv::slotModified);
    connect(ui.lePatient, &QLineEdit::textChanged, this, &DlgReceipePriv::slotModifiedWithCheck);
    connect(ui.lePhysician, &QLineEdit::textChanged, this, &DlgReceipePriv::slotModified);

    listReceipeTypes = new QStringList();
    if (db.getReceipeTypes(myDB::RT_PRIVATE, *listReceipeTypes)) {
        ui.cbReceipeType->addItems(*listReceipeTypes);
        ui.cbReceipeType->setCurrentIndex(0);
    }
	
    db.retrieveItemList(myDB::TABLE_RECEIPE_STATES, receipe_states);
    for (int i = 0; i < receipe_states.count(); i++)
	ui.cbState->addItem(((cReceipeState*)(receipe_states.at(i)))->getDescription());
    ui.cbState->setCurrentIndex(cReceipeState::RECEIPE_STATE_RUNNING);

    therapy.stage(ui.tabTherapy, cDateType::DT_THERAPY);
    connect(ui.pbDelDate, &QPushButton::clicked, &therapy, &Therapy::slotTherapyDel);
    connect(ui.pbDelAllDates, &QPushButton::clicked, &therapy, &Therapy::slotTherapyDelAll);

    ui.tableWidget->setColumnWidth(0, 50);
    ui.tableWidget->setColumnWidth(1, 290);
    ui.tableWidget->setColumnWidth(2, 68);
    ui.tableWidget->setColumnWidth(3, 80);
    if (delegate != NULL) delete delegate;
    delegate = new ReceipePrivDelegate(&cpt, this);
    ui.tableWidget->setItemDelegate(delegate);
    actPosAdd = new QAction(QString::fromUtf8("Position hinzufügen"), this);
    actPosAdd->setIcon(QIcon(":/add_pos.png"));
    actPosDel = new QAction(QString::fromUtf8("Position löschen"), this);
    actPosDel->setIcon(QIcon(":/delete_pos.png"));
    connect(actPosAdd, &QAction::triggered, this, &DlgReceipePriv::slotPosAdd);
    connect(actPosDel, &QAction::triggered, this, [this]() {slotPosDel();});
    ui.tableWidget->addAction(actPosAdd);
    ui.tableWidget->addAction(actPosDel);
    ui.tableWidget->setContextMenuPolicy(Qt::ActionsContextMenu);

    slotSwitchPriceTable(0);
    connect(ui.cbReceipeType, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &DlgReceipePriv::slotSwitchPriceTable);

    // show only following document types:
    // do not show documents until curReceipe has a valid ID
    browseDocs = new BrowseDocs(this, BrowseDocs::MASK_INVOICES | BrowseDocs::MASK_REMINDERS | BrowseDocs::MASK_REPORTS, 0);
    browseDocs->setMode(BrowseDocs::MODE_RECEIPE);

    QGridLayout *qgl = (QGridLayout *) ui.tabDocs->layout();
    if (qgl != NULL) qgl->addWidget(browseDocs, 0, 0, 1, 6);
    buildTableIcons();
    connect(ui.tableWidget->verticalHeader(), &QHeaderView::sectionClicked, this, [this](int section){ slotPosDel(); });

    deRDate  = new DateEdit();
    deRDate->setMaximumWidth(90);
    deRDate->setDate(QDate::currentDate());
    connect(deRDate, static_cast<void (DateEdit::*)(const QDate &)>(&DateEdit::dateChanged), this, [this](const QDate &) { slotModified(); });
    QGridLayout *grid = qobject_cast<QGridLayout*>(ui.tabReceipe->layout());
    if (grid) grid->addWidget(deRDate, 3, 5, 1, 2);

    installEventFilter(this);
    flag_change = false;
}


DlgReceipePriv::~DlgReceipePriv()
{
    if (delegate != NULL) delete delegate;
    if (listReceipeTypes != NULL) delete listReceipeTypes;
    while (!receipe_states.isEmpty()) delete receipe_states.takeFirst();
    if (actPosAdd != NULL) delete actPosAdd;
    if (actPosDel != NULL) delete actPosDel;
    if (browseDocs != NULL) delete browseDocs;
}


void DlgReceipePriv::loadReceipe(cPatient *p)
{
    if (curReceipe.getID() > 0) {
        browseDocs->setFilterID(curReceipe.getID());


//        db.setRidInDatesForPid(p->getID(), curReceipe.getID());
    }
    else browseDocs->setFilterID(0);
    browseDocs->Reload();

    if (p == NULL) {  // no patient loaded yet => set default values for receipe mask
        curPatient.reset();
        deRDate->setDate(QDate::currentDate());
        therapy.setDefaultEmployeeID(-1);
        flag_change = false;
        checkInput();
    } else {
        curPatient = *p;
        ui.lePatient->setText(p->getName() + ", " + p->getForename());
        ui.leBirthday->setText(p->getBirthday().toString("dd.MM.yyyy"));
        ui.leTelPriv->setText(p->getTelephone());
        ui.leTelJob->setText(p->getTelephoneJob());
        ui.leTelMobile->setText(p->getTelephoneMobile());
		
        cpt.clear();
        therapy.reset();
        if (curReceipe.getType() > 0) ui.cbReceipeType->setCurrentIndex(curReceipe.getType() - PRICETABLE_TYPE_PRIVAT);
        else if (p->getInsType() > 1) ui.cbReceipeType->setCurrentIndex(p->getInsType() - PRICETABLE_TYPE_PRIVAT);
            else ui.cbReceipeType->setCurrentIndex(0);

        if (curReceipe.getPriceTabID() < 0) {
	    // load default private price list and save ID
	    curReceipe.setPriceTabID(db.loadPriceTablePrivate(-1, PRICETABLE_TYPE_PRIVAT, cpt));
        } else {
            // load a price list that was used when the receipe was created
	    if (!db.loadPriceTablePrivate(curReceipe.getPriceTabID(), 0, cpt)) qDebug() << "failed";
        }
        if (cpt.count() < 1) {
            QMessageBox::warning(this, tr("OpenPhysio"), QString::fromUtf8("Es sind keine bisher keine privaten Leistungen und Preise hinterlegt worden.\nBitte konfigurieren Sie diese zuerst im OpenPhysio Administrator!"), QMessageBox::Ok);
        }

        ui.cbVisit->setChecked((bool)curReceipe.getFlagVisit());
        ui.spVisitCount->setValue(curReceipe.getHBCount());
        ui.spDistance->setValue(curReceipe.getDistance());

        QDate d = curReceipe.getDate();
        if (d.isValid()) deRDate->setDate(d);
        else deRDate->setDate(QDate::currentDate());

        ui.leDiag->setText(curReceipe.getDiagnosis());
        ui.teNotes->setPlainText(curReceipe.getNotes());
        ui.cbState->setCurrentIndex(curReceipe.getState());
        ui.leSumBrutto->setText(curReceipe.getBrutto());
		
        // load properties of the physician who issued the recipe
        if (curReceipe.getPhysID() >= 0) {
            cPhys phy;
            if (db.retrieveItem(phy, curReceipe.getPhysID()) == false) ui.lePhysician->setText("");
            else ui.lePhysician->setText(phy.getTitle() + " " + phy.getForename() + " " + phy.getName());
        } else {
            // if there is no physician associated yet check whether a default physician was configured for this patient
            if (curPatient.getDefaultPhysicianID() >= 0  && curReceipe.getID() < 0) {
                MsgBox box;
                if (box.execYesNoBox(QString::fromUtf8("Soll der für den Patienten voreingestellte Hausarzt für dieses Rezept eingetragen werden?"), MsgBox::BTN_YES) == MsgBox::BTN_YES) {
                    cPhys phys;
                    db.retrieveItem(phys, curPatient.getDefaultPhysicianID());
                    curReceipe.setPhysID(curPatient.getDefaultPhysicianID());
                    ui.lePhysician->setText(phys.getTitle() + " " + phys.getForename() + " " + phys.getName());
                } else ui.lePhysician->clear();
            } else ui.lePhysician->clear();
        }

        QAbstractItemModel* model = ui.tableWidget->model();
        if (model == NULL) return;

        QStringList qsl, qsl2;
        int idx = 0;
        int y;

        if (!curReceipe.getPositions().isEmpty()) {
            //if (curReceipe->getID() >= 0) {
            qsl = curReceipe.getPositions().split(";");
            for (y = 0; y < qsl.count(); y++) {
                qsl2 = qsl[y].split(":");
                if (!qsl2[0].isEmpty()) model->setData(model->index(y, 0), qsl2[0]);
                idx = qsl2[1].toInt();
                if (idx >= 0 && idx < cpt.qslPosName.count()) {
                    model->setData(model->index(y, 1), cpt.qslPosName[idx]);
                    calcPos(y, idx, qsl2[0].toInt());
                }
                calcSum();
                qsl2.clear();
            }

            if (curReceipe.getID() >= 0) therapy.load(curReceipe.getID());
        } else {
            ui.tabTherapy->clearContents();
            ui.tableWidget->clearContents();
        }
        therapy.setDefaultEmployeeID(p->getDefaultEmployeeID());
        flag_change = false;
    }
}


void DlgReceipePriv::initDlg(cReceipe *r, cPatient *p)
{
    if (r != NULL) {
        curReceipe = *r;
        cPatient p_tmp;
        if (db.retrieveItem(p_tmp, curReceipe.getPatientID()) == false) return;
            loadReceipe(&p_tmp);
    } else {
        if (p != NULL) {
            loadReceipe(p);
            curReceipe.setPatientID(p->getID());
        } else loadReceipe(NULL);
    }
}


void DlgReceipePriv::calcSum()
{
    QAbstractItemModel* model = ui.tableWidget->model();
    if (model == NULL) return;

    QString s;
    int y;
    float p = 0;
    float hb_sum = 0;
	
    // look at every row of position table
    for (y = 0; y < ui.tableWidget->rowCount(); y++) {
        s = model->index(y, 3).data().toString();
        p = p + s.toFloat();
    }

    if (ui.cbVisit->checkState() == Qt::Checked) {
	// we need to consider "Hausbesuch" stuff
	int hb_count = ui.spVisitCount->value();
	int hb_distance = hb_count * ui.spDistance->value();
	// get price for "Hausbesuch" and "Kilometerpauschale"
	// these prices are required to be at the end of the current price list
	if (cpt.count() > 1) {
	    float hb_dist_price = cpt.getPrice(cpt.count() - 1).toFloat();
	    float hb_price = cpt.getPrice(cpt.count() - 2).toFloat();
	    hb_sum = (float)(hb_count * hb_price);
	    hb_sum = hb_sum + (float)(hb_distance * hb_dist_price);
	}
    }

    p += hb_sum;
    s = QString::asprintf("%.2f", p);
    ui.leSumBrutto->setText(s);
}


void DlgReceipePriv::calcPos(int row, int pos_idx, int co)
{
    QAbstractItemModel* model = ui.tableWidget->model();
    if (model == NULL) return;

    QString s = "";
    float p = cpt.qslPrice[pos_idx].toFloat();
    s = QString::asprintf("%.2f", p);
    model->setData(model->index(row, 2), s);
    s = QString::asprintf("%.2f", (float)(p * co));
    model->setData(model->index(row, 3), s);
}


void DlgReceipePriv::slotSwitchPriceTable(int i)
{
    // FIXME: remove hard-coded values
    if ( (i >= 0) && (i != curReceipe.getType() - 3) ) {
        ui.tableWidget->clearContents();
        curReceipe.setPositions("");
	curReceipe.setType(i + 3);
        cpt.clear();
        therapy.reset();
	// load default price list: Private and latest price table version (0)
	db.loadPriceTablePrivate(-1, i + PRICETABLE_TYPE_PRIVAT, cpt);
	curReceipe.setPriceTabID(cpt.getID());
/*	if (curReceipe.getPriceTabID() < 0) {
	} else {
	    // load a price list that was used when the receipe was created
	    db.loadPriceTablePrivate(curReceipe.getPriceTabID(), i, cpt);
	}*/
    }
}


void DlgReceipePriv::slotCommitPos(int i)
{
    QAbstractItemModel* model = ui.tableWidget->model();
    if (model == NULL) return;

    if (i >= 0) {
        // strip "combo" from objectName to retrieve row
        QString qs = sender()->objectName();
        qs.remove(0, 5);
        int row = qs.right(5).toInt();
        if (row >= 0) {
            int cnt = model->index(row, 0).data().toInt();
            calcPos(row, i, cnt);
            calcSum();
        }
	flag_change = true;
    }
}


void DlgReceipePriv::slotCommitCount(int i)
{
    QAbstractItemModel* model = ui.tableWidget->model();
    if (model == NULL) return;
    if (i >= 0) {
        // strip "spin" from objectName to retrieve row
        QString qs = sender()->objectName();
        qs.remove(0, 4);
        int row = qs.right(5).toInt();
        QString pos = model->index(row, 1).data().toString();
        if (!pos.isEmpty()) {
            int x = cpt.qslPosName.indexOf(pos);
            if (x >= 0) {
                calcPos(row, x, i);
                calcSum();
            }
        }
	flag_change = true;
    }
}


void DlgReceipePriv::slotGetPatient()
{
    BrowsePatients *browsePatients = new BrowsePatients(Browse::MODE_SELECT);
    DlgBrowse browse(browsePatients);
    if (myConf.usePreload()) browsePatients->preload();
    if (browse.exec() == QDialog::Accepted) {
        cPatient p = browsePatients->getSelPatient();
        if (curPatient.getID() == -1 || p.getID() != curPatient.getID()) {
            curReceipe.reset();
            curReceipe.setPatientID(p.getID());
//            myDB::setRidInDatesForPid(p.getID(), curReceipe.getID());
        }
        loadReceipe(&p);
        flag_change = true;
    }
    delete browsePatients;
}


void DlgReceipePriv::slotGetPhysician()
{
    BrowsePhysicians *browsePhys = new BrowsePhysicians(Browse::MODE_SELECT);
    DlgBrowse browse(browsePhys);
    if (myConf.usePreload()) browsePhys->preload();
    if (browse.exec() == QDialog::Accepted) {
        cPhys p = browsePhys->getSelPhysician();
        ui.lePhysician->setText(p.getTitle() + " " + p.getForename() + " " + p.getName());
        curReceipe.setPhysID(p.getID());
        flag_change = true;
    }
    delete browsePhys;
}


void DlgReceipePriv::slotCancelReceipe()
{
    if (flag_change || therapy.changed()) {
	MsgBox box;
        if (box.execYesNoBox(QString::fromUtf8("Das Rezept wurde geändert. Soll das Rezept gespeichert werden?"), MsgBox::BTN_YES) == MsgBox::BTN_YES) {
	    if (saveReceipe()) {
		this->accept();
		return;
	    } else return;
	}
    }
    this->reject();
}


bool DlgReceipePriv::saveReceipe()
{
    // patient already chossen?
    if (curReceipe.getPatientID() < 0) {
        QMessageBox::warning(this, tr("OpenPhysio"), QString::fromUtf8("Bitte vorher einen Patienten auswählen!\n"), QMessageBox::Ok);
	return false;
    }

    if (!therapy.isConsistent()) {
        QMessageBox::warning(this, tr("OpenPhysio"), QString::fromUtf8("Für jeden Behandlungstermin muss ein Therapeut eingetragen werden!\n"), QMessageBox::Ok);
	return false;
    }

    // FIXME: remove hard-coded value
    curReceipe.setType(ui.cbReceipeType->currentIndex() + 3);
    curReceipe.setState(ui.cbState->currentIndex());
    curReceipe.setDate(deRDate->date());
    curReceipe.setDiagnosis(ui.leDiag->text());
    curReceipe.setNotes(ui.teNotes->toPlainText());
    curReceipe.setBrutto(ui.leSumBrutto->text());

    curReceipe.setFlagVisit((int)ui.cbVisit->isChecked());
    if (ui.cbVisit->isChecked()) {
        curReceipe.setHBCount(ui.spVisitCount->value());
        curReceipe.setDistance(ui.spDistance->value());
    } else {
        curReceipe.setHBCount(0);
        curReceipe.setDistance(0);
    }

    QAbstractItemModel* model = ui.tableWidget->model();
    if (model == NULL) return false;

    QString pos, qtmp;
    int x, y;

    pos = "";
    QStringList positions;
    // look at every row of position table
    for (y = 0; y < ui.tableWidget->rowCount(); y++) {
        qtmp = model->index(y, 0).data().toString();
        if (!qtmp.isEmpty()) {
	    pos = model->index(y, 0).data().toString() + ":";
            x = cpt.qslPosName.indexOf(model->index(y, 1).data().toString());
	    pos = pos + QString::number(x);
	    positions << pos;
        }
    }
    curReceipe.setPositions(positions.join(";"));

    //curReceipe.setPriceTabID(cpt.getType());
    curReceipe.setPricetableVersion(cpt.getVersion());

    if (curReceipe.getID() < 0) { // store new receipe in database
        int r_id;
	if ( db.storeItem(curReceipe, &r_id) == false) {
            QMessageBox::warning(this, tr("OpenPhysio"),
                QString::fromUtf8("Fehler beim Anlegen des Rezeptes aufgetreten!\n"),
                QMessageBox::Ok);
        } else {
            curReceipe.setID(r_id);
	    flag_change = false;
	}
    } else { // update properties of existing receipe
	if ( db.updateItem(curReceipe) == false) {
            QMessageBox::warning(this, tr("OpenPhysio"),
                QString::fromUtf8("Fehler beim Aktualisieren der Rezeptdaten aufgetreten!\n"),
                QMessageBox::Ok);
	} else {
	    flag_change = false;
	}
    }

    // only save therapies if saving of receipe was successful
    if (curReceipe.getID() >= 0) {
        therapy.save(curReceipe.getID());
    }

    // update the doc browser to filter the correct ID
    // (at least this is necessary for new receipes that have been saved for the first time)
    if (curReceipe.getID() > 0) browseDocs->setFilterID(curReceipe.getID());
    else browseDocs->setFilterID(0);
    browseDocs->Reload();

    return true;
}


void DlgReceipePriv::slotSaveReceipe()
{
    GUARD(Protect::FUNC_REC);
    if (saveReceipe()) accept();
}


void DlgReceipePriv::slotGenerateInvoice()
{
//    QStringList listPos;
//    QAbstractItemModel* model = ui.tableWidget->model();

//    if (model == NULL) return;

    if (curReceipe.getPatientID() < 0) {
        QMessageBox::warning(this, tr("OpenPhysio"),
            QString::fromUtf8("Bitte vorher einen Patienten auswählen und das Rezept vollständig ausfüllen!\n"),
            QMessageBox::Ok);
        return;
    }

    if (curReceipe.getID() < 0) {
        MsgBox box;
        if (box.execYesNoBox(QString::fromUtf8("Das Rezept muss erst gespeichert werden, bevor eine Rechnung erstellt werden kann!\n\nJetzt speichern?"), MsgBox::BTN_YES) == MsgBox::BTN_NO) {
            return;
        } else {
            if (!saveReceipe()) return;
        }
    }

    if (curReceipe.getState() == cReceipeState::RECEIPE_STATE_RUNNING) {
	MsgBox box;
        if (box.execYesNoBox(QString::fromUtf8("Der Rezeptstatus ist \"Laufend\". Bevor eine Rechnung erstellt werden kann,\nmuss der Rezeptstatus auf \"Abgeschlossen\" geändert werden\n\nSoll dies jetzt geschehen (gleichzeitig wird das Rezept gespeichert)?"), MsgBox::BTN_YES) == MsgBox::BTN_YES) {
            ui.cbState->setCurrentIndex(cReceipeState::RECEIPE_STATE_READY);
	    saveReceipe();
	} else return;
    } else {
	if (flag_change || therapy.changed()) {
	    MsgBox box;
            if (box.execYesNoBox(QString::fromUtf8("Das Rezept wurde geändert und muss vor der Rechnungsstellung gespeichert werden!\nSoll das Rezept jetzt gespeichert werden?"), MsgBox::BTN_YES) == MsgBox::BTN_YES) {
		saveReceipe();
	    } else return;
	}
	QStringList listInv;
        if ( db.getInvoiceNumbers(curReceipe.getID(), listInv) == true) {
            if (listInv.count() > 0) {
		MsgBox box;
                QString msg = QString::fromUtf8("Für das aktuelle Rezept wurden bereits folgenden Rechnungen erstellt:\n");
		for (int x = 0; x < listInv.count(); x++) {
		    msg += (listInv.at(x) + "\n");
		}
		msg +=("\nSoll trotzdem eine neue Rechnung mit neuer Rechnungsnummer erstellt werden?");
		if (box.execYesNoBox(msg, MsgBox::BTN_NO) == MsgBox::BTN_NO) return;
            }
        } else {
                // TODO: add error handling
        }
    }

/*    for (int y = 0; y < ui.tableWidget->rowCount(); y++) {
        if (!model->index(y, 0).data().toString().isEmpty()) {
            listPos << model->index(y, 0).data().toString() << model->index(y, 1).data().toString();
            listPos << model->index(y, 2).data().toString() << model->index(y, 3).data().toString();
        }
    }*/

    QList<cInvoice *> invoices;
    invoices.append(new cInvoice());
    invoices.first()->setReceipeID(curReceipe.getID());
    invoices.first()->setPatientID(curReceipe.getPatientID());
    invoices.first()->setType(cInvoice::INVOICE_RECEIPE);

    if (gendoc.genInvoice(invoices, 0)) {

        cPatient p;
        db.retrieveItem(p, curReceipe.getPatientID());

        geneinv.genEInvoice(invoices[0]->getInvData(), &p);
    }

}


void DlgReceipePriv::slotPosAdd()
{
    ui.tableWidget->insertRow(ui.tableWidget->rowCount());
    buildTableIcons();
    flag_change = true;
}


void DlgReceipePriv::slotPosDel()
{
    slotPosDel(ui.tableWidget->currentRow());
}


void DlgReceipePriv::slotPosDel(int i)
{
    int r = ui.tableWidget->currentRow();
    if (r >= 0) {
        ui.tableWidget->removeRow(r);
        buildTableIcons();
        calcSum();
        flag_change = true;
    }
}


void DlgReceipePriv::slotReport()
{
    GUARD(Protect::FUNC_REP);

    if (curReceipe.getID() < 0) {
        MsgBox box;
        if (box.execYesNoBox(QString::fromUtf8("Das Rezept muss erst gespeichert werden, bevor ein Bericht erstellt werden kann!\n\nJetzt speichern?"), MsgBox::BTN_YES) == MsgBox::BTN_NO) {
            return;
        } else {
            if (!saveReceipe()) return;
        }
    }

    cReport rep;
    rep.setPatientID(curReceipe.getPatientID());
    rep.setReceipeID(curReceipe.getID());
    rep.setPhysID(curReceipe.getPhysID());
    gendoc.genReport(rep);
}


void DlgReceipePriv::slotShowPatInfo()
{
    if (ui.widgetPatInfo->isVisible()) {
	ui.widgetPatInfo->setVisible(false);
	ui.pbPatInfo->setText(">>");
    } else {
	ui.widgetPatInfo->setVisible(true);
	ui.pbPatInfo->setText("<<");
    }
}


void DlgReceipePriv::slotVisit(int i)
{
    if (i == Qt::Checked) {
        if (curReceipe.getID() < 0) {
            ui.spDistance->setValue(curPatient.getDistance());
        } else {
            if (ui.spDistance->value() == 0) ui.spDistance->setValue(curPatient.getDistance());
        }
    }
    flag_change = true;
    calcSum();
}


void DlgReceipePriv::slotModified()
{
    flag_change = true;
    calcSum();
}


void DlgReceipePriv::slotModifiedWithCheck()
{
    flag_change = true;
    checkInput();
}


void DlgReceipePriv::checkInput()
{
    QPalette p_hint(QApplication::palette());
    QPalette p_normal(QApplication::palette());
    p_hint.setColor(QPalette::Base, Qt::yellow);
    if (ui.lePatient->text().isEmpty()) {
        ui.lePatient->setPalette(p_hint);
        ui.lePatient->setToolTip(QString::fromUtf8("Kein Patient ausgewählt"));
    } else {
        ui.lePatient->setPalette(p_normal);
        ui.lePatient->setToolTip("");
    }
}


void DlgReceipePriv::slotRoute()
{
    MsgBox::dummy();
    /*BrowseRoutes *browseRoutes = new BrowseRoutes(Browse::MODE_SELECT);
    DlgBrowse browse(browseRoutes);
    if (myConf.usePreload()) browseRoutes->preload();
    if (browse.exec() == QDialog::Accepted) {
        int i = browseRoutes->getSelectedRoute();
        if (i > 0) {
            ui.leVisit->setText(QString::number(i));
            flag_change = true;
        }
    }
    delete browseRoutes;*/
}


void DlgReceipePriv::slotPrintTherapyDates()
{
    GenDoc gendoc;
    gendoc.genPatientSchedule(curReceipe, curPatient, therapy.getListDates());
}


bool DlgReceipePriv::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *ke = static_cast<QKeyEvent *>(event);
        if (ke->key() == Qt::Key_F1 || ke->key() == Qt::Key_Help) {
            helpWrapper.showHelpForKeyword("openphysio_receipes_private", true);
            return true;
        }
    }
    return QDialog::eventFilter(obj, event);
}


void DlgReceipePriv::buildTableIcons()
{
    // set "clear" icons to the receipe position table
    int co = ui.tableWidget->rowCount();
    for (int i = 0; i < co; i++) {
        QTableWidgetItem *twi = new QTableWidgetItem();
        twi->setIcon(QIcon(":/delete_pos.png"));
        twi->setToolTip(QString::fromUtf8("Position löschen"));
        ui.tableWidget->setVerticalHeaderItem(i, twi);
   }
}


void DlgReceipePriv::slotDelPhysician()
{
    curReceipe.setPhysID(-1);
    ui.lePhysician->clear();
}
