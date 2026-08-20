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

#include "dlgreceipe.h"
#include "config.h"
#include "gendoc.h"
#include "protect.h"
#include "msgbox.h"
#include "browseinsurances.h"
#include "dlgbrowse.h"
#include "helperwrapper.h"


//****************************************************************************
// MatchDelegate
//****************************************************************************

MatchDelegate::MatchDelegate(cPriceTable *c, QObject* parent) : QItemDelegate(parent)
{
    dlg = (DlgReceipe*)parent;
    cpt = c;
}


QWidget* MatchDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex& index) const
{
    QWidget* w;

    switch(index.column()) {
	case 0: {
	    QSpinBox *editor = new QSpinBox(parent);
	    editor->setObjectName("spin" + QString::number(index.row()));
	    editor->setMinimum(0);
	    editor->setMaximum(40);
	    w = editor;
	    connect(editor, SIGNAL(valueChanged(int)), dlg, SLOT(slotCommitCount(int)));
            }
	    break;
	case 1: {
	    QComboBox* editor = new QComboBox(parent);
	    editor->setObjectName("comboPosNr" + QString::number(index.row()));
	    if (cpt != NULL) editor->addItems(cpt->getPositionNumbers());
	    w = editor;
	    connect(editor, SIGNAL(currentIndexChanged(int)), dlg, SLOT(slotCommitPos(int)));
	    }
	    break;
	case 2: {
	    QComboBox* editor = new QComboBox(parent);
	    editor->setObjectName("combo" + QString::number(index.row()));
	    if (cpt != NULL) editor->addItems(cpt->getPositionDescriptions());
	    w = editor;
	    connect(editor, SIGNAL(currentIndexChanged(int)), dlg, SLOT(slotCommitPos(int)));
	    }
	    break;
	default:
	    w = 0;
	    break;
    }
    return w;
}


void MatchDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    QVariant value = index.model()->data(index, Qt::DisplayRole);

    switch(index.column()) {
	case 0: {
	    static_cast<QSpinBox*>(editor)->setValue(value.toInt());
	    break;
	    }
	case 1:
	case 2: {
	    QComboBox* box = static_cast<QComboBox*>(editor);
	    box->setCurrentIndex(box->findText(value.toString()));
	    }
	    break;
	default: break;
    }
}


void MatchDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    QVariant value;

    switch(index.column()) {
	case 0:
	    value = static_cast<QSpinBox*>(editor)->value();
	    break;
	case 1:
	case 2:
	    value = static_cast<QComboBox*>(editor)->currentText();
	    break;
	default: break;
    }
    model->setData(index, value);
}


void MatchDelegate::updateEditorGeometry( QWidget* editor,
                               const QStyleOptionViewItem& option,
                               const QModelIndex& ) const
{
    editor->setGeometry(option.rect);
}


//****************************************************************************
// ShortcutButton
//****************************************************************************

ShortcutButton::ShortcutButton(QWidget * parent)
    : QPushButton(parent)
{
    amount = 0;
    posnr = "";
}


ShortcutButton::~ShortcutButton()
{
}


int ShortcutButton::getAmount()
{
    return amount;
}


void ShortcutButton::setAmount(int i)
{
    amount = i;
}


QString ShortcutButton::getPosNr()
{
    return posnr;
}


void ShortcutButton::setPosNr(QString s)
{
    posnr = s;
}


//****************************************************************************
// DlgReceipe
//****************************************************************************

DlgReceipe::DlgReceipe(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);
    delegate = NULL;

    flag_change = false;
    auto_calc = true;
    auto_calc_hb = true;
    idx_hb_flat = idx_hb_dist = -1;

    ui.pbInsurance->hide();
    connect(ui.pbCancel, &QPushButton::clicked, this, &DlgReceipe::slotCancelReceipe);
    connect(ui.pbSave, &QPushButton::clicked, this, &DlgReceipe::slotSaveReceipe);
    connect(ui.pbInsurant, &QPushButton::clicked, this, &DlgReceipe::slotGetInsurant);
    connect(ui.pbPhysician, &QPushButton::clicked, this, &DlgReceipe::slotGetPhysician);
    connect(ui.pbPrint, &QPushButton::clicked, this, &DlgReceipe::slotPrint);
    connect(ui.pbCheck, &QPushButton::clicked, this, &DlgReceipe::slotCheck);
    connect(ui.pbReport, &QPushButton::clicked, this, &DlgReceipe::slotReport);
    connect(ui.pbDistance, &QPushButton::clicked, this, &DlgReceipe::slotDistance);
    connect(ui.pbOptions, &QPushButton::clicked, this, &DlgReceipe::slotOptions);
    connect(ui.pbQuittance, &QPushButton::clicked, this, &DlgReceipe::slotQuittance);
    connect(ui.pbPatInfo, &QPushButton::clicked, this, &DlgReceipe::slotShowPatInfo);
    connect(ui.pbPrintTherapy, &QPushButton::clicked, this,&DlgReceipe::slotPrintTherapyDates);
    //connect(ui.pbInsurance, &QPushButton::clicked, this, &DlgReceipe::slotSelectInsurance);
    connect(ui.pbDelPhys, &QPushButton::clicked, this, &DlgReceipe::slotDelPhysician);
    connect(ui.cbIndicator, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &DlgReceipe::slotModified);
    connect(ui.cbTOS, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &DlgReceipe::slotModified);
    connect(ui.leInsurant, &QLineEdit::textChanged, this, &DlgReceipe::slotModifiedWithCheck);
    connect(ui.cbReport, &QCheckBox::toggled, this, &DlgReceipe::slotModified);
    connect(ui.cbGroup, &QCheckBox::toggled, this, &DlgReceipe::slotModified);
    connect(ui.cbAccident, &QCheckBox::toggled, this, &DlgReceipe::slotModified);
    connect(ui.teDiag, &QTextEdit::textChanged, this, &DlgReceipe::slotModified);
    connect(ui.teNotes, &QPlainTextEdit::textChanged, this, &DlgReceipe::slotModified);
    connect(ui.lePhysician, &QLineEdit::textChanged, this, &DlgReceipe::slotModified);

    checkInput();

    db.retrieveItemList(myDB::TABLE_INDICATORS, indicators);
    for (int i = 0; i < indicators.count(); i++)
	ui.cbIndicator->addItem(((cIndicator*)(indicators.at(i)))->getDescription());
	
    db.retrieveItemList(myDB::TABLE_RECEIPE_STATES, receipe_states);
    for (int i = 0; i < receipe_states.count(); i++)
	ui.cbState->addItem(((cReceipeState*)(receipe_states.at(i)))->getDescription());
    ui.cbState->setCurrentIndex(0);

    therapy.stage(ui.tabTherapy, cDateType::DT_THERAPY);
    connect(ui.pbDelDate, &QPushButton::clicked, &therapy, &Therapy::slotTherapyDel);
    connect(ui.pbDelAllDates, &QPushButton::clicked, &therapy, &Therapy::slotTherapyDelAll);

    curReceipe.reset();
    curProfession = cProfession::PROF_PHYSIOTHERAPY;

    ui.tableWidget->setColumnWidth(0, 50);
    ui.tableWidget->setColumnWidth(1, 60);
    ui.tableWidget->setColumnWidth(2, 320);
    ui.tableWidget->setColumnWidth(3, 60);
    ui.tableWidget->setColumnWidth(4, 75);

    actPosDel = new QAction(QString::fromUtf8("Position löschen"), this);
    actPosDel->setIcon(QIcon(":/delete_pos.png"));
    connect(actPosDel, &QAction::triggered, this, [this]() {slotPosDel();});
    ui.tableWidget->addAction(actPosDel);
    ui.tableWidget->setContextMenuPolicy(Qt::ActionsContextMenu);

    ui.cbVisit->setChecked(false);
    ui.widgetHB->hide();

    // connect change signals of every components that is relevant for calculation
    connect(ui.cbCharge, &QCheckBox::checkStateChanged, this, &DlgReceipe::slotCharge);
    connect(ui.cbGroup,  &QCheckBox::checkStateChanged, this, &DlgReceipe::slotCalculate);
    connect(ui.cbReport,  &QCheckBox::checkStateChanged, this, &DlgReceipe::slotCalculate);
    connect(ui.cbSocial,  &QCheckBox::checkStateChanged, this, &DlgReceipe::slotCalculate);
    connect(ui.spVisitDistance, &QSpinBox::valueChanged, this, &DlgReceipe::slotVisitDistance);
    connect(ui.cbVisit,  &QCheckBox::checkStateChanged, this, &DlgReceipe::slotVisit);
    connect(ui.spbCountHB, &QSpinBox::valueChanged, this, &DlgReceipe::slotHBCount);
    connect(ui.cbManualHB,  &QCheckBox::checkStateChanged, this, &DlgReceipe::slotManualHB);

    connect(ui.tableWidget, &QTableWidget::cellChanged, this, &DlgReceipe::slotPosChanged);
    connect(ui.cbAddPay,  &QCheckBox::checkStateChanged, this, &DlgReceipe::slotAddPay);
    connect(ui.cbState, &QComboBox::currentIndexChanged, this, &DlgReceipe::slotBill);

    browseDocs = new BrowseDocs(this, BrowseDocs::MASK_QUITTANCES | BrowseDocs::MASK_REPORTS, 0);
    QGridLayout *qgl = (QGridLayout *) ui.tabDocs->layout();
    if (qgl != NULL) qgl->addWidget(browseDocs, 0, 0, 1, 6);
    browseDocs->setMode(BrowseDocs::MODE_RECEIPE);

    buildTableIcons();
    connect(ui.tableWidget->verticalHeader(), SIGNAL(sectionClicked(int)), SLOT(slotPosDel(int)));

    deRDate  = new DateEdit();
    deStartDate = new DateEdit();
    deBill = new DateEdit();
    deAddPay = new DateEdit();
    deRDate->setDate(QDate::currentDate());
    // FIXME: read the value from config/database
    deStartDate->setDate(QDate::currentDate().addDays(10));
    connect(deRDate, &DateEdit::dateChanged, this, &DlgReceipe::slotReceipeDate);
    connect(deStartDate, &DateEdit::dateChanged, this, &DlgReceipe::slotModified);
    deBill->setEnabled(false);
    deAddPay->setEnabled(false);
    connect(deAddPay, &DateEdit::dateChanged, this, &DlgReceipe::slotDateChangeAddPay);
    QGridLayout *grid = qobject_cast<QGridLayout*>(ui.widget_5->layout());
    if (grid) {
        grid->addWidget(deRDate, 1, 0);
        grid->addWidget(deStartDate, 1, 1);
    }
    grid = qobject_cast<QGridLayout*>(ui.widget_2->layout());
    if (grid) {
        grid->addWidget(deBill, 1, 4);
        grid->addWidget(deAddPay, 1, 7);
    }
    //setTabOrder(first, second);

    installEventFilter(this);
}


DlgReceipe::~DlgReceipe()
{
    if (delegate != NULL) delete delegate;
    while (!indicators.isEmpty()) delete indicators.takeFirst();
    while (!receipe_states.isEmpty()) delete receipe_states.takeFirst();
    if (actPosDel != NULL) delete actPosDel;
    if (browseDocs != NULL) delete browseDocs;
    while (!shortcut_buttons.isEmpty()) delete shortcut_buttons.takeFirst();
}


void DlgReceipe::updateInsuranceText(bool clear)
{
    if (clear) {
	ui.leInsurance->setText("");
	curInsurance.reset();
	curReceipe.setIK("");
	curReceipe.setPriceTabID(-1);
    } else {
	ui.leInsurance->setText(curInsurance.getShortName() + " [" + curInsurance.getIK() + "]" );
	ui.leInsurance->setCursorPosition(0);
    }
}


void DlgReceipe::loadPriceTable()
{
    QString tn;
    int tnid = -1;

    if (curReceipe.getPriceTabID() < 0) {
	// select a price list that matches our surgery (area, profession, union)
        tn = db.lookupPriceTableNameAndID(curProfession, curInsurance.getAssoc(), &tnid);
        curReceipe.setPriceTabID(tnid);
    } else {
        // select a price list that matches the price list ID save with the receipe
	tn = db.getPriceTableName(curReceipe.getPriceTabID());
    }
    if (!db.loadPriceTable(tn, cpt, curProfession)) {
        QMessageBox::warning(this, tr("OpenPhysio"), QString::fromUtf8("Es konnte keine Preisliste geladen werden!"), QMessageBox::Ok);
    } else {
        if (curInsurance.getAssoc() == cAssoc::BG) {
            if (ui.tableWidget->rowCount() < 4) {
                do {
                    ui.tableWidget->insertRow(0);
                } while (ui.tableWidget->rowCount() < 4);
            }
        } else {
            if (ui.tableWidget->rowCount() > 2) {
                do {
                    ui.tableWidget->removeRow(ui.tableWidget->rowCount() - 1);
                } while (ui.tableWidget->rowCount() > 2);
            }
        }
        buildTableIcons();
    }
    ui.labPos->setText("Heilmittel [" + db.getPriceTableDescription(curReceipe.getPriceTabID()) + "]");

    disconnect(ui.cbPosHBflat, SIGNAL(currentIndexChanged(int)), this, SLOT(slotHBPosFlat(int)));
    disconnect(ui.cbPosHBdist, SIGNAL(currentIndexChanged(int)), this, SLOT(slotHBPosDist(int)));
    ui.cbPosHBflat->clear();
    ui.cbPosHBdist->clear();
    QStringList list_hb = cpt.getHBPositionNumbers();
    ui.cbPosHBflat->addItems(list_hb);
    ui.cbPosHBdist->addItems(list_hb);
    connect(ui.cbPosHBflat, SIGNAL(currentIndexChanged(int)), this, SLOT(slotHBPosFlat(int)));
    connect(ui.cbPosHBdist, SIGNAL(currentIndexChanged(int)), this, SLOT(slotHBPosDist(int)));
}


void DlgReceipe::loadReceipe(cPatient& patient)
{
    if (curReceipe.getID() > 0) browseDocs->setFilterID(curReceipe.getID());
    else browseDocs->setFilterID(0);
    browseDocs->Reload();

    curPatient = patient;
    if (patient.getID() < 0) {
        deRDate->setDate(QDate::currentDate());
        slotReceipeDate(QDate::currentDate());
        ui.leInsurant->setText("");
        ui.leBirthday->setText("");
        ui.leInsurance->setText("");
        ui.lePhysician->setText("");
	ui.labPos->setText("Heilmittel");
	ui.cbVisit->setChecked(false);
	ui.widgetHB->hide();
        checkInput();
    } else {
        curProfession = curReceipe.getProfession();

        // set UI properties that can be read from patient objects straight forward
	ui.leInsurant->setText(patient.getName() + ", " + patient.getForename());
	ui.leBirthday->setText(patient.getBirthday().toString("dd.MM.yyyy"));
	ui.leTelPriv->setText(patient.getTelephone());
	ui.leTelJob->setText(patient.getTelephoneJob());
	ui.leTelMobile->setText(patient.getTelephoneMobile());

        // clear price list
	cpt.reset();
        therapy.reset();
	curInsurance.reset();

	bool b = true;
	if (curReceipe.getIK().isEmpty()) b = db.getInsurance(patient.getInsurance(), curInsurance);
	else b = db.getInsurance(curReceipe.getIK(), curInsurance);

	if ( (b == false) && (curReceipe.getPriceTabID() < 0) ) {
	    // there is no price list referrenced and there is no valid insurance IK saved for the patient;
	    // the user still might manually select an apppropriate price list
	    updateInsuranceText(true);
            QMessageBox::warning(this, tr("OpenPhysio"), QString::fromUtf8("Für den Patienten ist keine (gültige) Krankenkasse hinterlegt!\nDas automatische Laden der Heilmittelpositionen war daher nicht möglich!"), QMessageBox::Ok);
	} else {
	    // load appropriate price list
	    if (!curInsurance.getIK().isEmpty()) curReceipe.setIK(curInsurance.getIK());
	    updateInsuranceText(false);
	    loadPriceTable();
	}

	if (curReceipe.isSetManualCalc()) auto_calc = false;
	if (curReceipe.isSetManualCalcHB()) {
	    auto_calc_hb = false;
	    ui.cbManualHB->setChecked(true);
	}
	ui.spbCountHB->setValue(curReceipe.getHBCount());
        QDate d = curReceipe.getDate();
        if (d.isValid()) deRDate->setDate(d);
        else deRDate->setDate(QDate::currentDate());
        deStartDate->setDate(curReceipe.getStartDate());
        ui.cbIndicator->setCurrentIndex(curReceipe.getIndID());

        ui.cbVisit->setChecked((bool)curReceipe.getFlagVisit());
	if (!(bool)curReceipe.getFlagVisit()) ui.widgetHB->hide();
	else ui.widgetHB->show();

        if (curReceipe.getID() > 0) {
            ui.cbCharge->setChecked((bool)curReceipe.getFlagCharge());
        } else {
            if ((bool)curPatient.getAddpayFreed()) {
                if (deRDate->date() >= curPatient.getFreedFrom() && deRDate->date() < curPatient.getFreedUntil() ) {
                    ui.cbCharge->setChecked(false);
                } else {
                    ui.cbCharge->setChecked(true);
                }
            } else ui.cbCharge->setChecked(true);
        }
        ui.cbGroup->setChecked((bool)curReceipe.getFlagGroup());
        ui.cbAccident->setChecked((bool)curReceipe.getFlagAccident());
        ui.cbReport->setChecked((bool)curReceipe.getFlagReport());
        ui.spVisitDistance->setValue(curReceipe.getDistance());
        ui.cbTOS->setCurrentIndex(curReceipe.getTOS());
        ui.leSumBrutto->setText(curReceipe.getBrutto());
        ui.leSumAddpay->setText(curReceipe.getAddpay());
        ui.cbState->setCurrentIndex(curReceipe.getState());
        ui.teDiag->setPlainText(curReceipe.getDiagnosis());
        ui.teNotes->setPlainText(curReceipe.getNotes());

        disconnect(ui.cbAddPay, SIGNAL(stateChanged(int)), this, SLOT(slotAddPay(int)));
        ui.cbAddPay->setChecked((bool)curReceipe.getAddPayFlag());

        if (curReceipe.getAddPayFlag() > 0) {
            // addpay stuff
            deAddPay->setEnabled(true);
            deAddPay->setDate(curReceipe.getAddPayDate());
        } else {
            deAddPay->setEnabled(true);
	    // if patient is younger than 18 years no additionally payment required
            /*int diff = ui.deRDate->date().year() - patient.getBirthday().year();
	    if (diff < 18) ui.cbCharge->setChecked(Qt::Unchecked);
	    else if (diff > 18) ui.cbCharge->setChecked(Qt::Checked);
	    else {
		QDate ref_date1 = QDate(ui.deRDate->date().year(), 1, 1);
		QDate ref_date2(ui.deRDate->date().year(), patient.getBirthday().month(), patient.getBirthday().day());
		int diff1 = ref_date1.daysTo(ui.deRDate->date());
		int diff2 = ref_date1.daysTo(ref_date2);
		if (diff2 < diff1) ui.cbCharge->setChecked(Qt::Unchecked);
		else ui.cbCharge->setChecked(Qt::Checked);
            }*/
        }
		
        if (curReceipe.getState() == cReceipeState::RECEIPE_STATE_SETTLED) {  // if receipe is "abgerechnet"
            deBill->setEnabled(true);
            deBill->setDate(curReceipe.getBillDate());
        }
		
	// load properties of the physician who issued the receipe
        if (curReceipe.getPhysID() >= 0) {
            cPhys phy;
	    if (db.retrieveItem(phy, curReceipe.getPhysID()) == false) ui.lePhysician->setText("");
            else ui.lePhysician->setText(phy.getTitle() + " " + phy.getForename() + " " + phy.getName());
        } else {
            // if there is no physician associated yet check whether a default physician was configured for this patient
            if (curPatient.getDefaultPhysicianID() >= 0 && curReceipe.getID() < 0) {
                MsgBox box;
                if (box.execYesNoBox(QString::fromUtf8("Soll der für den Patienten voreingestellte Hausarzt für dieses Rezept eingetragen werden?"), MsgBox::BTN_YES) == MsgBox::BTN_YES) {
                    cPhys phys;
                    db.retrieveItem(phys, curPatient.getDefaultPhysicianID());
                    curReceipe.setPhysID(curPatient.getDefaultPhysicianID());
                    ui.lePhysician->setText(phys.getTitle() + " " + phys.getForename() + " " + phys.getName());
                } else ui.lePhysician->clear();
            } else ui.lePhysician->clear();
        }


        QStringList list_hb = cpt.getHBPositionNumbers();
        idx_hb_flat = curReceipe.getHBPosFlat();
        idx_hb_dist = curReceipe.getHBPosDist();
        if (curReceipe.getID() < 0 && idx_hb_flat < 0) { // auto-select a position
            if (ui.cbSocial->isChecked() || ui.cbGroup->isChecked()) idx_hb_flat = cpt.posHBgrp();
            else idx_hb_flat = cpt.posHB();
        } else if (curReceipe.getID() > 0 && idx_hb_flat < 0 && idx_hb_dist < 0 && curReceipe.getFlagVisit() > 0) {
            // probably an old receipe (< 20110824-beta) => need to guess a little
            // (or user has selected HB but did not specify the actual positions or cleared them respectively)
            if (ui.cbSocial->isChecked() || ui.cbGroup->isChecked()) idx_hb_flat = cpt.posHBgrp();
            else idx_hb_flat = cpt.posHB();
            idx_hb_dist = cpt.posHBdist();
        }
        ui.cbPosHBflat->setCurrentIndex(list_hb.indexOf(cpt.getPosNr(idx_hb_flat)));

        if (curReceipe.getID() < 0 && idx_hb_dist < 0) idx_hb_dist = cpt.posHBdist(); // auto-select a position
        ui.cbPosHBdist->setCurrentIndex(list_hb.indexOf(cpt.getPosNr(idx_hb_dist)));

        QAbstractItemModel* model = ui.tableWidget->model();
        QStringList qsl, qsl2;
        int idx = 0;
        int y, co;
		
        if (ui.leInsurance->text() != "") { // ID might be zero if copied or if new receipe
	    if (!curReceipe.getPositions().isEmpty()) { // if empty then "split" would crash
		qsl = curReceipe.getPositions().split(";");
		co = qsl.count();
                // GKV receipe is limited to 2 positions, BG is limited to 4
                if (co > 2 && curInsurance.getAssoc() != cAssoc::BG) co = 2;
                if (co > 4 && curInsurance.getAssoc() == cAssoc::BG) co = 4;
                for (y = 0; y < co; y++) {
                    if (QString(qsl[y]).isEmpty()) continue;
                    qsl2 = qsl[y].split(":");
                    if (!qsl2[0].isEmpty()) model->setData(model->index(y, 0), qsl2[0]);
                    idx = qsl2[1].toInt();
		    cPriceTableItem* cpti = cpt.at(idx);
		    if (cpti == NULL) continue;
		    model->setData(model->index(y, 1), cpti->getPosNr());
		    model->setData(model->index(y, 2), cpti->getPosDescr());
		    calcPos(y, idx, qsl2[0].toInt());
                    qsl2.clear();
                }
                if (curReceipe.getID() >= 0) therapy.load(curReceipe.getID());
            }
        } else {
            ui.tabTherapy->clearContents();
            ui.tableWidget->clearContents();
        }
	if (delegate != NULL) delete delegate;
	delegate = new MatchDelegate(&cpt, this);
	ui.tableWidget->setItemDelegate(delegate);

        connect(ui.cbAddPay, SIGNAL(stateChanged(int)), this, SLOT(slotAddPay(int)));
    }

    therapy.setDefaultEmployeeID(patient.getDefaultEmployeeID());
    flag_change = false;
}


void DlgReceipe::slotPosChanged(int row, int column)
{
    if (column == 0) slotCalculate();
}


void DlgReceipe::initDlg(cReceipe *r, int prof, cPatient *p)
{
    if (r == NULL) {
        curProfession = prof;
        buildShortcutButtons();
        if (p != NULL) {
            curReceipe.setPatientID(p->getID());
            curReceipe.setProfession(curProfession);
            loadReceipe(*p);
        }
        flag_change = false;
        return;
    }
    curReceipe = *r;
    cPatient p_tmp;
    if (!db.retrieveItem(p_tmp, curReceipe.getPatientID())) return;
    loadReceipe(p_tmp);
    // create shortcut buttons after loading an existing receipe
    // because the loading procedure sets "curProfession" according to
    // the information saved in the receipe
    buildShortcutButtons();
}


//
// calulates one row of the position table
//
void DlgReceipe::calcPos(int row, int pos_idx, int co)
{
    QAbstractItemModel* model = ui.tableWidget->model();

    cPriceTableItem* cpti = cpt.at(pos_idx);
    if (cpti == NULL) return;

    QString s;
    float f = co * (cpti->getPrice().toFloat());
    s.asprintf("%.2f", f);
    model->setData(model->index(row, 3), s);
    f = co * (cpti->getAddpay().toFloat());
    s.asprintf("%.2f", f);
    model->setData(model->index(row, 4), s);

    slotCalculate();
}


void DlgReceipe::slotCommitPos(int i)
{
    if (i < 0) return;
    cPriceTableItem* cpti = cpt.at(i);
    if (cpti == NULL) return;

    flag_change = true;
    int row = 0;
    int column = 2;
    QString s;
    // TODO: fix ugly code
    if (sender()->objectName().contains("PosNr")) {
        if (sender()->objectName() == "comboPosNr0") row = 0;
        else if (sender()->objectName() == "comboPosNr1") row = 1;
        else if (sender()->objectName() == "comboPosNr2") row = 2;
        else if (sender()->objectName() == "comboPosNr3") row = 3;
	s = cpti->getPosDescr();
    } else {
        if (sender()->objectName() == "combo0") row = 0;
        else if (sender()->objectName() == "combo1") row = 1;
        else if (sender()->objectName() == "combo2") row = 2;
        else if (sender()->objectName() == "combo3") row = 3;
	s = cpti->getPosNr();
	column = 1;
    }

    // set position number or position description
    QAbstractItemModel* model = ui.tableWidget->model();
    if (model == NULL) return;
    model->setData(model->index(row, column), s);
    calcPos(row, i, model->index(row, 0).data().toInt());
}


void DlgReceipe::slotCommitCount(int i)
{
    if (i < 0) return;
    flag_change = true;
    QAbstractItemModel* model = ui.tableWidget->model();
    int row = 0;
    // TODO: fix ugly code
    if (sender()->objectName() == "spin0") row = 0;
    else if (sender()->objectName() == "spin1") row = 1;
    else if (sender()->objectName() == "spin2") row = 2;
    else if (sender()->objectName() == "spin3") row = 3;
    QString pos = model->index(row, 1).data().toString();
    if (!pos.isEmpty()) {
	int x = cpt.indexOf(pos);
	if (x >= 0) calcPos(row, x, i);
    }
}


void DlgReceipe::slotGetInsurant()
{
    BrowsePatients *browsePatients = new BrowsePatients(Browse::MODE_SELECT);
    DlgBrowse browse(browsePatients);
    if (myConf.usePreload()) browsePatients->preload();
    if (browse.exec() == QDialog::Accepted) {
        cPatient p = browsePatients->getSelPatient();
        if (curPatient.getID() == -1 || p.getID() != curPatient.getID()) {
            curReceipe.reset();
            curInsurance.reset();
            curReceipe.setPatientID(p.getID());
            curReceipe.setProfession(curProfession);
        }
        loadReceipe(p);
        flag_change = true;
    }
    delete browsePatients;
}


void DlgReceipe::slotGetPhysician()
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


void DlgReceipe::reject()
{
    if (flag_change || therapy.changed()) {
	MsgBox box;
        if (box.execYesNoBox(QString::fromUtf8("Rezeptdaten wurden geändert. Jetzt speichern?"), MsgBox::BTN_YES) == MsgBox::BTN_YES) {
	    if (!saveReceipe()) return;
	    flag_change = false;
	    QDialog::accept();
            return;
	} else QDialog::reject();
    }
    QDialog::reject();
}


void DlgReceipe::slotCancelReceipe()
{
    this->reject();
}


bool DlgReceipe::saveReceipe()
{
    GUARD_BOOL(Protect::FUNC_REC);

    bool bRet = true;
	
    // patient already chossen?
    if (curReceipe.getPatientID() < 0) {
        QMessageBox::warning(this, tr("OpenPhysio"), QString::fromUtf8("Bitte vorher einen Patienten auswählen!\n"), QMessageBox::Ok);
        return false;
    }

    if (!therapy.isConsistent()) {
        QMessageBox::warning(this, tr("OpenPhysio"), QString::fromUtf8("Für jeden Behandlungstermin muss ein Therapeut eingetragen werden!\n"), QMessageBox::Ok);
	return false;
    }

    if (curInsurance.getAssoc() < cAssoc::BG) curReceipe.setType(0); // GKV
    else if (curInsurance.getAssoc() == cAssoc::POSTA) curReceipe.setType(1);
    else curReceipe.setType(2);
    curReceipe.setState(ui.cbState->currentIndex());
    curReceipe.setDate(deRDate->date());
    curReceipe.setStartDate(deStartDate->date());
    curReceipe.setIndID(ui.cbIndicator->currentIndex());
    curReceipe.setTOS(ui.cbTOS->currentIndex());
    curReceipe.setDiagnosis(ui.teDiag->toPlainText());
    curReceipe.setNotes(ui.teNotes->toPlainText());
    curReceipe.setFlagCharge((int)ui.cbCharge->isChecked());
    curReceipe.setFlagVisit((int)ui.cbVisit->isChecked());
    if (ui.cbVisit->isChecked()) {
        curReceipe.setHBCount(ui.spbCountHB->value());
        curReceipe.setDistance(ui.spVisitDistance->value());
        curReceipe.setHBPosFlat(idx_hb_flat);
        curReceipe.setHBPosDist(idx_hb_dist);
    } else {
        curReceipe.setHBCount(0);
        curReceipe.setDistance(0);
        curReceipe.setHBPosFlat(-1);
        curReceipe.setHBPosDist(-1);
    }
    curReceipe.setFlagGroup((int)ui.cbGroup->isChecked());
    curReceipe.setFlagAccident((int)ui.cbAccident->isChecked());
    curReceipe.setFlagReport((int)ui.cbReport->isChecked());

    curReceipe.setBrutto(ui.leSumBrutto->text());
    curReceipe.setAddpay(ui.leSumAddpay->text());
	
    if (ui.cbAddPay->checkState() == Qt::Checked) {
        curReceipe.setAddPayFlag(1);
        curReceipe.setAddPayDate(deAddPay->date());
    } else {
        curReceipe.setAddPayFlag(0);
    }
	
    if (ui.cbState->currentIndex() == cReceipeState::RECEIPE_STATE_SETTLED) curReceipe.setBillDate(deBill->date());
	
    QAbstractItemModel* model = ui.tableWidget->model();
    if (model == NULL) return false;

    QString pos = "";
    int x;

    for (int i = 0; i < ui.tableWidget->rowCount(); i++) {
        if (model->index(i, 0).data().toString().isEmpty()) continue;
        pos = pos + model->index(i, 0).data().toString() + ":";
        x = cpt.indexOf(model->index(i, 1).data().toString());
        if (x < 0) continue;
        pos = pos + QString::number(x) + ";";
    }
    curReceipe.setPositions(pos);

    if (curReceipe.getID() < 0) { // store new receipe in database
        int r_id;
	if ( db.storeItem(curReceipe, &r_id) == false) {
            QMessageBox::warning(this, tr("OpenPhysio"), QString::fromUtf8("Fehler beim Anlegen des Rezeptes aufgetreten!\n"), QMessageBox::Ok);
            bRet = false;
        } else {
            curReceipe.setID(r_id);
            browseDocs->setFilterID(curReceipe.getID());
            browseDocs->Reload();
        }
    } else { // update properties of existing receipe
	if ( db.updateItem(curReceipe) == false) {
            QMessageBox::warning(this, tr("OpenPhysio"), QString::fromUtf8("Fehler beim Aktualisieren der Rezeptdaten aufgetreten!\n"), QMessageBox::Ok);
            bRet = false;
        }
    }

    // only save therapies if saving of receipe was successful
    if (curReceipe.getID() >= 0) therapy.save(curReceipe.getID());

    if (bRet) flag_change = false;
    return bRet;
}


//
// slotSaveReceipe
//
void DlgReceipe::slotSaveReceipe()
{
    GUARD(Protect::FUNC_REC);
    if (saveReceipe()) accept();
}


void DlgReceipe::slotCalculate()
{
    if (!auto_calc) return;
    QAbstractItemModel* model = ui.tableWidget->model();
    if (model == NULL) return;

    QString s;
    float sum_brutto = 0;
    float sum_addpay = 0;
    float addpay_hb = 0;
    float addpay = 0;

    for (int i = 0; i < ui.tableWidget->rowCount(); i++) {
        addpay += model->index(i, 4).data().toString().toFloat();
        sum_brutto += model->index(i, 3).data().toString().toFloat();
    }

    /*if (ui.cbVisit->isChecked()) {  // check if HB is enabled
	float hb;
	int k, i, hb_count;

	// check if HB must be calculated for groups or social facilities
	if ( ui.cbSocial->isChecked() || ui.cbGroup->isChecked() ) i = cpt.posHBgrp();
	else i = cpt.posHB();

	if (auto_calc_hb) {
	    int pos_count1 = model->index(0, 0).data().toInt();
	    int pos_count2 = model->index(1, 0).data().toInt();
	    if (pos_count1 >= pos_count2) hb_count = pos_count1;
	    else hb_count = pos_count2;
	    ui.spbCountHB->setValue(hb_count);
	} else hb_count = ui.spbCountHB->value();

	hb = (float)(hb_count * cpt.getPrice(i).toFloat());
	addpay_hb = (float)(hb_count * cpt.getAddpay(i).toFloat());

	// check if insurance pays for each kilometer, too
        if (!cpt.isFlatHB() && ((k = ui.spVisitDistance->value()) > 0) ) {
	    // user enters full distance => we don't need to double it
	    hb += (hb_count * k * cpt.getPrice(cpt.posHBdist()).toFloat());
	    addpay_hb += (hb_count * k * cpt.getAddpay(cpt.posHBdist()).toFloat());
	}
			
        sum_brutto += hb;
    }*/

    if (ui.cbVisit->isChecked() && (idx_hb_flat >= 0 || idx_hb_dist >= 0)) {  // check if HB is enabled
        float hb;
        int hb_count;
        if (auto_calc_hb) {
            int pos_count1 = model->index(0, 0).data().toInt();
            int pos_count2 = model->index(1, 0).data().toInt();
            if (pos_count1 >= pos_count2) hb_count = pos_count1;
            else hb_count = pos_count2;
            ui.spbCountHB->setValue(hb_count);
        } else hb_count = ui.spbCountHB->value();

        hb = (float)(hb_count * cpt.getPrice(idx_hb_flat).toFloat());
        addpay_hb = (float)(hb_count * cpt.getAddpay(idx_hb_flat).toFloat());

        // user enters full distance => we don't need to double it
        hb += (hb_count * ui.spVisitDistance->value() * cpt.getPrice(idx_hb_dist).toFloat());
        addpay_hb += (hb_count * ui.spVisitDistance->value() * cpt.getAddpay(idx_hb_dist).toFloat());
        sum_brutto += hb;
    }

    // FIXME: replace hard-coded value for receipe fee (10 EUR)
    if ( ui.cbCharge->isChecked() && curInsurance.getAssoc() != cAssoc::BG ) {
        sum_addpay = 10 + addpay + addpay_hb;
        s.asprintf("%.2f", sum_addpay);
        ui.leSumAddpay->setText(s);
    } else {
        ui.leSumAddpay->setText("0.00");
    }

    if (ui.cbReport->isChecked()) sum_brutto += cpt.getPrice(cpt.posReport()).toFloat();

    s.asprintf("%.2f", sum_brutto);
    ui.leSumBrutto->setText(s);

}


void DlgReceipe::slotPrint()
{
    if (flag_change) {
	MsgBox box;
	if (box.execYesNoBox("Rezept sollte vorher gespeichert werden.\nJetzt speichern?", MsgBox::BTN_YES) == MsgBox::BTN_YES) {
	    saveReceipe();
	}
    }

    GenDoc gendoc;
    gendoc.genMusterX(curProfession, curReceipe, cpt);
}


void DlgReceipe::slotCheck()
{
    if (curProfession != cProfession::PROF_PHYSIOTHERAPY) {
        MsgBox::info(QString::fromUtf8("Diese Funktion ist bisher nur für Physiotherapie-Rezepte realisiert."));
        return;
    }
    if (curInsurance.getAssoc() == cAssoc::BG) {
        MsgBox::info(QString::fromUtf8("Diese Funktion ist nicht für BG-Rezepte realisiert."));
        return;
    }
    QAbstractItemModel* model = ui.tableWidget->model();
    if (model == NULL) return;

    QDialog dlg;
    dlg.setWindowTitle(QString::fromUtf8("Heilmittel-Check - Ergebnisse"));
    dlg.setWindowIcon(QIcon(":/favicon.png"));

    QGridLayout layout(&dlg);
    QPushButton pbs(QString::fromUtf8("Schließen"));
    QTextEdit te;
    layout.addWidget(&pbs, 1, 0, 1, 1);
    layout.addWidget(&te, 0, 0, 1, 1);
    connect(&pbs, &QPushButton::clicked, &dlg, &DlgReceipe::accept);

    QList<cTableItem*> list_domains;
    QList<cTableItem*> list_rules;
    if (!db.retrieveItemList(myDB::TABLE_HM_CAT_DOMAINS, list_domains)) {
        MsgBox::error(QString::fromUtf8("Fehler beim Lesen der HM-Katalogdomänen aufgetreten!"));
        return;
    }
    QMap<QString, QString> map_domains;
    for (int i = 0; i < list_domains.count(); i++) {
        cHmCatDomain *d = (cHmCatDomain*)(list_domains.at(i));
        map_domains[d->getName()] = d->getPositions();
    }

    if (!db.retrieveItemList(myDB::TABLE_HM_CAT, list_rules)) {
        MsgBox::error(QString::fromUtf8("Fehler beim Lesen der HM-Katalogregeln aufgetreten!"));
        while (!list_domains.isEmpty()) delete list_domains.takeFirst();
        return;
    }

    bool valid = true;

    if (deStartDate->date().isValid()) {
        if (deRDate->date() > deStartDate->date()) {
            valid = false;
            te.append(QString::fromUtf8("Das Datum des Behandlungsbeginns liegt vor dem Rezeptdatum.<br>"));
        }
    }

    QString ind = ui.cbIndicator->currentText();
    int ind_pos = -1;
    if (!ind.isEmpty()) {
        // find the rule for the selected indicator
        for (int i = 0; i < list_rules.count(); i++)
        {
            cHmCat *c  = (cHmCat*)list_rules.at(i);
            if (ind == c->getIndicator()) {
                ind_pos = i;
                break;
            }
        }
    } else {
        valid = false;
        te.append(QString::fromUtf8("Der Indikationsschlüssel fehlt.<br>"));
    }

    // read hm positions of the current receipe
    QString hm_pos1 = model->index(0, 1).data().toString();
    QString hm_pos2 = model->index(1, 1).data().toString();
    int hm_pos1_count = model->index(0, 0).data().toInt();
    int hm_pos2_count = model->index(1, 0).data().toInt();

    if (hm_pos1.isEmpty() || hm_pos1_count < 1) {
        valid = false;
        te.append(QString::fromUtf8("Es wurde kein vorrangiges Heilmittel (Heilmittelposition 1) gewählt oder die Verordnungsmenge ist 0.<br>"));
    } else {
        if (ind_pos < 0){
            valid = false;
            te.append(QString::fromUtf8("Es konnte keine Regel für den gewählten Indikationsschlüssel gefunden werden.<br>"));
        } else {
            cHmCat *c  = (cHmCat*)list_rules.at(ind_pos);

            // check whether a child indicator is used for adults or the other way around
            bool child = false;
            if ((QDate::currentDate().year() - curPatient.getBirthday().year()) < 18) child = true;
            if (child) {
                if (!(bool)c->getChildOnly()) {
                    valid = false;
                    te.append(QString::fromUtf8("Es wurde ein Indikationsschlüssel gewählt, der nicht für Kinder oder Jugendliche zulässig ist.<br>"));
                }
            } else {
                if ((bool)c->getChildOnly()) {
                    valid = false;
                    te.append(QString::fromUtf8("Es wurde ein Indikationsschlüssel gewählt, der nicht für Erwachsene zulässig ist.<br>"));
                }
            }

            // check the amount of allowed hm positions
            if (ui.cbTOS->currentIndex() == cReceipe::ERSTVO) {
                if (hm_pos1_count > c->getErstVO()) {
                    valid = false;
                    te.append(QString::fromUtf8("Die Verordnungsmenge für Heilmittelposition 1 [") +
                              QString::number(hm_pos1_count) + "x " + hm_pos1 + QString::fromUtf8("] ist größer als die zulässige Verordnungsmenge [") +
                              QString::number(c->getErstVO()) + "x]<br>");
                }
                if (hm_pos2_count > c->getErstVO()) {
                    valid = false;
                    te.append(QString::fromUtf8("Die Verordnungsmenge für Heilmittelposition 2 [") +
                              QString::number(hm_pos2_count) + "x " + hm_pos2 + QString::fromUtf8("] ist größer als die zulässige Verordnungsmenge [") +
                              QString::number(c->getErstVO()) + "x]<br>");
                }
            } else if (ui.cbTOS->currentIndex() == cReceipe::FOLGEVO) {
                if (c->getFolgeVO() == 0) {
                    valid = false;
                    te.append(QString::fromUtf8("Eine Folgeverordnung ist für den angegebenen Indikationsschlüssel nicht erlaubt."));
                } else {
                    if (hm_pos1_count > c->getFolgeVO()) {
                        valid = false;
                        te.append(QString::fromUtf8("Die Verordnungsmenge für Heilmittelposition 1 [") +
                                  QString::number(hm_pos1_count) + "x " + hm_pos1 + QString::fromUtf8("] ist größer als die zulässige Verordnungsmenge [") +
                                  QString::number(c->getFolgeVO()) + "x]<br>");
                    }
                    if (hm_pos2_count > c->getFolgeVO()) {
                        valid = false;
                        te.append(QString::fromUtf8("Die Verordnungsmenge fÃ¼r Heilmittelposition 2 [") +
                                  QString::number(hm_pos2_count) + "x " + hm_pos2 + QString::fromUtf8("] ist größer als die zulässige Verordnungsmenge [") +
                                  QString::number(c->getFolgeVO()) + "x]<br>");
                    }
                }
            }

            // get the catalog domains
            QString primary = c->getHmPrimary();
            QString optional = c->getHmOptional();
            QString additional = c->getHmAdditional();
            QStringList primary_list, optional_list, additional_list;
            if (!primary.isEmpty()) primary_list = primary.split(":");
            if (!optional.isEmpty()) optional_list = optional.split(":");
            if (!additional.isEmpty()) additional_list = additional.split(":");

            // resolve catalog domains to the actual hm positions
            QStringList primary_resolved, optional_resolved, additional_resolved;
            for (int i = 0; i < primary_list.count(); i++) {
                QString s = map_domains[primary_list.at(i)];
                if (!s.isEmpty()) primary_resolved.append(s.split(":"));
            }
            for (int i = 0; i < optional_list.count(); i++) {
                QString s = map_domains[optional_list.at(i)];
                if (!s.isEmpty()) optional_resolved.append(s.split(":"));
            }
            for (int i = 0; i < additional_list.count(); i++) {
                QString s = map_domains[additional_list.at(i)];
                if (!s.isEmpty()) additional_resolved.append(s.split(":"));
            }

            // check whether the selected hm positions are allowed for the current indicator
            QString hm_pos1_tmp = hm_pos1;
            hm_pos1_tmp.remove(0, 1); // remove leading profession indicator (e.g. 2)
            if (!primary_resolved.contains(hm_pos1_tmp)) {
                if (optional_resolved.isEmpty() || (!optional_resolved.contains(hm_pos1_tmp))) {
                    valid = false;
                    te.append(QString::fromUtf8("Heilmittelposition 1 [") + hm_pos1 +
                              QString::fromUtf8("] ist kein vorrangiges Heilmittel für den gewählten Indikationsschlüssel.<br>"));
                }
            }
            if (!hm_pos2.isEmpty()) {
                QString hm_pos2_tmp = hm_pos2;
                hm_pos2_tmp.remove(0, 1);
                if (!additional_resolved.contains(hm_pos2_tmp)) {
                    valid = false;
                    te.append(QString::fromUtf8("Heilmittelposition 2 [") + hm_pos2 +
                              QString::fromUtf8("] ist kein ergänzendes Heilmittel für den gewählten Indikationsschlüssel.<br>"));
                }
            }
        }
    }

    if (!valid) {
        QString s = te.toPlainText();
        s = QString::fromUtf8("Es sind folgende Unstimmigkeiten identifiziert worden:\n\n") + s;
        te.setPlainText(s);
    } else {
        te.append(QString::fromUtf8("Der Heilmittel-Check wurde erfolgreich abgeschlossen!"));
    }

    dlg.exec();

    while (!list_domains.isEmpty()) delete list_domains.takeFirst();
    while (!list_rules.isEmpty()) delete list_rules.takeFirst();
}


void DlgReceipe::slotReport()
{
    GUARD(Protect::FUNC_REP);
	
    if (!ui.cbReport->isChecked()) {
	MsgBox box;
        if (box.execYesNoBox(QString::fromUtf8("Ein Therapiebericht ist lt. Rezeptdaten nicht erforderlich\n\nTrotzdem einen Therapiebericht erstellen?"), MsgBox::BTN_YES) == MsgBox::BTN_NO) {
	    return;
	}
    }

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


void DlgReceipe::slotDistance()
{
    MsgBox::dummy();
/*    BrowseRoutes *browseRoutes = new BrowseRoutes(Browse::MODE_SELECT);
    DlgBrowse browse(browseRoutes);
    if (myConf.usePreload()) browseRoutes->preload();
    if (browse.exec() == QDialog::Accepted) {
        int i = browseRoutes->getSelectedRoute();
        if (i > 0) {
            ui.spVisitDistance->setValue(i);
            flag_change = true;
        }
    }
    delete browseRoutes;*/
}


void DlgReceipe::slotPosDel()
{
    slotPosDel(ui.tableWidget->currentRow());
}


void DlgReceipe::slotPosDel(int i)
{
    ui.tableWidget->removeRow(i);
    ui.tableWidget->insertRow(ui.tableWidget->rowCount());
    buildTableIcons();
    flag_change = true;
    slotCalculate();
}


void DlgReceipe::slotOptions()
{
    DlgReceipeOptions opt;
    opt.init(curReceipe.isSetManualCalc());
    opt.exec();
    if (opt.result() != QDialog::Accepted) return;
    flag_change = true;

    if (opt.isManCalcEnabled()) {
	// TODO: set flags in curReceipe
	auto_calc = false;
	curReceipe.setManualCalc(true);
	ui.leSumBrutto->setReadOnly(false);
	ui.leSumAddpay->setReadOnly(false);
    } else {
	auto_calc = true;
	curReceipe.setManualCalc(false);
	ui.leSumBrutto->setReadOnly(true);
	ui.leSumAddpay->setReadOnly(true);
	slotCalculate();
    }

    int id;
    if (opt.isManPriceEnabled(&id)) {
	// load the new price list
	if (id != curReceipe.getPriceTabID()) {
	    curReceipe.setPriceTabID(id);
	    // clear price list
	    cpt.reset();
	    QString tn = db.getPriceTableName(curReceipe.getPriceTabID());
            db.loadPriceTable(tn, cpt, curProfession);
	    ui.labPos->setText("Heilmittel [" + db.getPriceTableDescription(curReceipe.getPriceTabID()) + "]");
	    if (delegate != NULL) delete delegate;
	    delegate = new MatchDelegate(&cpt, this);
	    ui.tableWidget->setItemDelegate(delegate);
	    // clear receipe positions
	    ui.tableWidget->clearContents();
	}
    } // end: manual price check
}


void DlgReceipe::slotAddPay(int i)
{
    flag_change = true;
    if (i == Qt::Checked) {
        deAddPay->setDate(QDate::currentDate());
        deAddPay->setEnabled(true);
    } else {
        deAddPay->setEnabled(false);
    }
}


void DlgReceipe::slotBill(int i)
{
    flag_change = true;
    if (i == cReceipeState::RECEIPE_STATE_SETTLED) {
        deBill->setEnabled(true);
        deBill->setDate(QDate::currentDate());
    } else {
        deBill->setEnabled(false);
    }
}


void DlgReceipe::slotCharge(int i)
{
    // depending on whether the patient has to be charged for addpay do
    // - enable or disabled addpay status
    // - recalculate
    flag_change = true;
    if (i == Qt::Checked) {
        ui.cbAddPay->setEnabled(true);
    } else {
        ui.cbAddPay->setCheckState(Qt::Unchecked);
        ui.cbAddPay->setEnabled(false);
    }
    slotCalculate();
}


void DlgReceipe::slotQuittance()
{
    if (curReceipe.getPatientID() < 0) {
        QMessageBox::warning(this, tr("OpenPhysio"), QString::fromUtf8("Bitte vorher einen Patienten auswählen!"), QMessageBox::Ok);
        return;
    }

    if (curReceipe.getID() < 0) {
        MsgBox box;
        if (box.execYesNoBox(QString::fromUtf8("Das Rezept muss erst gespeichert werden, bevor eine Quittung erstellt werden kann!\n\nJetzt speichern?"), MsgBox::BTN_YES) == MsgBox::BTN_NO) {
            return;
        } else {
            if (!saveReceipe()) return;
        }
    }

    if (!(bool)curReceipe.getFlagCharge()) {
        QMessageBox::warning(this, tr("OpenPhysio"), QString::fromUtf8("Eine Zuzahlung ist nicht erforderlich. Daher wird keine Quittung ausgestellt!"), QMessageBox::Ok);
        return;
    }

    if (ui.cbAddPay->checkState() !=  Qt::Checked) {
        QMessageBox::warning(this, tr("OpenPhysio"), QString::fromUtf8("Es wurde noch nicht vermerkt, dass eine Zuzahlung erfolgt ist!\n\nDaher wird keine Quittung ausgestellt!"), QMessageBox::Ok);
	return;
    }

    cQuittance q;
    q.setReceipeID(curReceipe.getID());
    q.setPatientID(curReceipe.getPatientID());
    q.setDate(deAddPay->date());
    gendoc.genQuittance(q);

}


void DlgReceipe::slotDateChangeAddPay(const QDate& date)
{
    flag_change = true;
    curReceipe.setAddPayDate(date);
}


void DlgReceipe::slotShowPatInfo()
{
    if (ui.widgetPatInfo->isVisible()) {
	ui.widgetPatInfo->setVisible(false);
	ui.pbPatInfo->setText(">>");
    } else {
	ui.widgetPatInfo->setVisible(true);
	ui.pbPatInfo->setText("<<");
    }
}


void DlgReceipe::slotPrintTherapyDates()
{
    GenDoc gendoc;
    gendoc.genPatientSchedule(curReceipe, curPatient, therapy.getListDates());
}


void DlgReceipe::slotVisit(int i)
{
    if (i == Qt::Checked) {
	ui.widgetHB->show();
        if (curReceipe.getID() < 0) {
            ui.cbSocial->setChecked((bool)curPatient.getCareFacility());
            ui.spVisitDistance->setValue(curPatient.getDistance());
        } else {
            if (ui.spVisitDistance->value() <= 0) ui.spVisitDistance->setValue(curPatient.getDistance());
        }
    } else {
	ui.widgetHB->hide();
    }
    slotCalculate();
    flag_change = true;
}


void DlgReceipe::slotHBCount()
{
    if (auto_calc_hb) return;
    flag_change = true;
    slotCalculate();
}


void DlgReceipe::slotManualHB(int i)
{
    if (i == Qt::Checked) {
	auto_calc_hb = false;
	ui.spbCountHB->setReadOnly(false);
    } else {
	auto_calc_hb = true;
	ui.spbCountHB->setReadOnly(true);
    }
    curReceipe.setManualCalcHB(!auto_calc_hb);
    flag_change = true;
    slotCalculate();
}


/*void DlgReceipe::slotSelectInsurance()
{
    if (curReceipe.getPatientID() < 0) {
        QMessageBox::warning(this, "OpenPhysio", QString::fromUtf8("Bitte erst einen Patienten auswählen!"), QMessageBox::Ok);
	return;
    }
    BrowseInsurances *bi = new BrowseInsurances(Browse::MODE_SELECT);
    DlgBrowse browse(bi);
    if (browse.exec() == QDialog::Accepted) {
	int old_assoc = curInsurance.getAssoc();
	updateInsuranceText(true);
        curInsurance = bi->getSelInsurance();
	curReceipe.setIK(curInsurance.getIK());
	updateInsuranceText(false);
	// if assoc changes => reset positions and reload price table
	if (curInsurance.getAssoc() != old_assoc) {
	    cpt.clear();
	    loadPriceTable();
	    ui.tableWidget->clearContents();
            flag_change = true;
	}
    }
    delete bi;
}*/


void DlgReceipe::slotModifiedWithCheck()
{
    flag_change = true;
    checkInput();
}


void DlgReceipe::slotModified()
{
    flag_change = true;
}


void DlgReceipe::checkInput()
{
    QPalette p_hint(QApplication::palette());
    QPalette p_normal(QApplication::palette());
    p_hint.setColor(QPalette::Base, Qt::yellow);
    if (ui.leInsurant->text().isEmpty()) {
        ui.leInsurant->setPalette(p_hint);
        ui.leInsurant->setToolTip(QString::fromUtf8("Kein Patient ausgewählt"));
    } else {
        ui.leInsurant->setPalette(p_normal);
        ui.leInsurant->setToolTip("");
    }
}


void DlgReceipe::buildShortcutButtons()
{
    int co = db.listReceipeShortcuts.count();
    if (co <= 0) {
        ui.widgetShortcuts->hide();
        return;
    }
    QGridLayout *qgl = (QGridLayout *) ui.widgetShortcuts->layout();
    if (qgl == NULL) return;

    int grid_pos = 0;
    for (int i = 0; i < co; i++) {
        cReceipeShortcut* cut = db.listReceipeShortcuts.at(i);
        if (cut->getProfession() == curProfession) {
            shortcut_buttons.append(new ShortcutButton());
            ShortcutButton *b = shortcut_buttons.last();
            b->setText(QString::number(cut->getAmount()) + "x " + cut->getDescription());
            b->setAmount(cut->getAmount());
            b->setPosNr(cut->getPosNr());
            b->setToolTip("HM-Pos: " + cut->getPosNr());
            grid_pos++;
            qgl->addWidget(b, 0, grid_pos, 1, 1);
            connect(b, &QPushButton::clicked, this, &DlgReceipe::slotShortcut);
        }
    }
    if (grid_pos == 0) ui.widgetShortcuts->hide();
}


void DlgReceipe::slotShortcut()
{
    ShortcutButton *b = (ShortcutButton*)this->sender();
    if (b == NULL) return;

    int idx = cpt.indexOf(b->getPosNr());
    if (idx < 0) {
        MsgBox::info(QString::fromUtf8("Keine passende Position gefunden!\nSie müssen erst einen Patienten und die richtige Preisliste laden!"));
        return;
    }
    int row = ui.tableWidget->currentRow();
    if (row < 0) row = 0;

    QAbstractItemModel* model = ui.tableWidget->model();
    model->setData(model->index(row, 0),  b->getAmount());
    model->setData(model->index(row, 1),  b->getPosNr());
    cPriceTableItem* cpti = cpt.at(idx);
    model->setData(model->index(row, 2), cpti->getPosDescr());
    flag_change = true;
    calcPos(row, idx, b->getAmount());
}


bool DlgReceipe::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *ke = static_cast<QKeyEvent *>(event);
        if (ke->key() == Qt::Key_F1 || ke->key() == Qt::Key_Help) {
            helpWrapper.showHelpForKeyword("openphysio_receipes", true);
            return true;
        }
    }
    return QDialog::eventFilter(obj, event);
}


void DlgReceipe::buildTableIcons()
{
    int co = ui.tableWidget->rowCount();
    QTableWidgetItem *twi[co];
    for (int i = 0; i < co; i++) {
        twi[i] = new QTableWidgetItem();
        twi[i]->setIcon(QIcon(":/delete_pos.png"));
        twi[i]->setToolTip(QString::fromUtf8("Position löschen"));
    }
    for (int i = 0; i < co; i++) {
        if (i >= co) break;
        ui.tableWidget->setVerticalHeaderItem(i, twi[i]);
    }
}


void DlgReceipe::slotDelPhysician()
{
    curReceipe.setPhysID(-1);
    ui.lePhysician->clear();
}


void DlgReceipe::slotReceipeDate(const QDate& date)
{
    int ofs = 0;
    // TODO: remove hard-coded values
    if (curProfession == cProfession::PROF_PHYSIOTHERAPY) ofs = 14;
    if (curProfession == cProfession::PROF_ERGOTHERAPY) ofs = 14;
    deStartDate->setDate(date.addDays(ofs));
}


void DlgReceipe::slotHBPosFlat(int i)
{
    flag_change = true;
    if (i == 0) idx_hb_flat = -1;
    else idx_hb_flat = cpt.indexOf(ui.cbPosHBflat->itemText(i));
    slotCalculate();
}


void DlgReceipe::slotHBPosDist(int i)
{
    flag_change = true;
    if (i == 0) idx_hb_dist = -1;
    else idx_hb_dist = cpt.indexOf(ui.cbPosHBdist->itemText(i));
    slotCalculate();
}


void DlgReceipe::slotVisitDistance(int i)
{
    flag_change = true;
    slotCalculate();
}
