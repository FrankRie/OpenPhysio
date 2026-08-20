/****************************************************************************
**
** Copyright (C) 2013 Sebastian Vandersee
** All rights reserved.
**
** This file is part of OpenPhysio.
**
** Please refer to the LICENSE file for terms of use.
**
****************************************************************************/

#include "confprices.h"
#include "ui_confprices.h"
#include <msgbox.h>


ConfPrices::ConfPrices(QWidget *parent, int l, int a) :
    ConfBase(parent, l, a),
    ui(new Ui::ConfPrices)
{
    ui->setupUi(this);
    setTitle("Preise GKV");
    setKeyword("opa_prices");

#ifdef Q_OS_MACX
    ui->widget->layout()->setSpacing(12);
#endif

    connect(ui->pbFilePrices, SIGNAL(clicked()), this, SLOT(slotSelectPriceFile()));
    connect(ui->pbPrices, SIGNAL(clicked()), this, SLOT(slotPrices()));
    connect(ui->cbProf, SIGNAL(currentIndexChanged(int)), this, SLOT(slotProfession(int)));
    connect(ui->pbDelList, SIGNAL(clicked()), this, SLOT(slotDelList()));
    connect(ui->pbViewList, SIGNAL(clicked()), this, SLOT(slotViewList()));
}


ConfPrices::~ConfPrices()
{
    delete ui;
    while (!listTariffAreas.isEmpty()) delete listTariffAreas.takeFirst();
    while (!listPriceLists.isEmpty()) delete listPriceLists.takeFirst();
    while (!listPriceListsOrg.isEmpty()) delete listPriceListsOrg.takeFirst();
}


void ConfPrices::reloadPriceLists()
{
    while (!listPriceLists.isEmpty()) delete listPriceLists.takeFirst();
    while (!listPriceListsOrg.isEmpty()) delete listPriceListsOrg.takeFirst();
    db.retrieveItemList(myDB::TABLE_PRICETABLES, reinterpret_cast<QList<cTableItem*>&>(listPriceListsOrg));

    for (int i = 0; i < listPriceListsOrg.count(); i++) {
        cPriceTableNames *c_org = ((cPriceTableNames*)listPriceListsOrg.at(i));
        // filter redundant price lists
        bool redundant = false;
        for (int j = 0; j < listPriceLists.count(); j++) {
            cPriceTableNames *c = ((cPriceTableNames*)listPriceLists.at(j));
            if (c->getName() == c_org->getName()) {
                redundant = true;
                break;
            }
        }
        if (!redundant) {
            listPriceLists.append(new cPriceTableNames());
            cPriceTableNames *c = ((cPriceTableNames*)listPriceLists.last());
            *c = *c_org;
        }
    }

    loadTable(ui->cbProf->currentIndex());
}


void ConfPrices::init()
{
    listPriceFiles.clear();
    ui->leFilePrices->clear();

    while (!listTariffAreas.isEmpty()) delete listTariffAreas.takeFirst();
    lookupArea.clear();
    db.retrieveItemList(myDB::TABLE_TARIFF_AREAS, reinterpret_cast<QList<cTableItem*>&>(listTariffAreas));
    for (int i = 0; i < listTariffAreas.count(); i++) {
        cTariffArea *t = ((cTariffArea*)listTariffAreas.at(i));
        lookupArea[t->getID()] = t->getName();
    }

    reloadPriceLists();
}


bool ConfPrices::write()
{
    return true;
}


void ConfPrices::loadTable(int i)
{
    ui->tablePriceLists->setSortingEnabled(false);

    ui->tablePriceLists->clearContents();
    while (ui->tablePriceLists->rowCount() > 0)
        ui->tablePriceLists->removeRow(0);

    QAbstractItemModel* model;
    model = ui->tablePriceLists->model();
    int co = 0;
    for (int x = 0; x < listPriceLists.count(); x++) {
        cPriceTableNames *c = ((cPriceTableNames*)listPriceLists.at(x));
        if (c->getProfession() != i) continue;
        ui->tablePriceLists->insertRow(ui->tablePriceLists->rowCount());
        model->setData(model->index(co, 0), c->getName());
        if (c->getTariffArea() > listTariffAreas.count()) {
            switch(c->getTariffArea()) {
                case cTariffScope::TARIFF_SCOPE_ID_GERMANY:
                    model->setData(model->index(co, 1), "Bundesweit");
                    break;
                case cTariffScope::TARIFF_SCOPE_ID_EAST:
                    model->setData(model->index(co, 1), "Deutschland Ost");
                    break;
                case cTariffScope::TARIFF_SCOPE_ID_WEST:
                    model->setData(model->index(co, 1), "Deutschland West");
                    break;
                default: break;
            }
        } else {
            model->setData(model->index(co, 1), lookupArea.value(c->getTariffArea()));
        }
        model->setData(model->index(co, 2), c->getValidFrom().toString("yyyy-MM-dd"));
        model->setData(model->index(co, 3), c->getDescription());
        co++;
    }
    ui->tablePriceLists->hideColumn(0);
    ui->tablePriceLists->resizeColumnsToContents();
    ui->tablePriceLists->setSortingEnabled(true);
    ui->tablePriceLists->sortItems(1, Qt::AscendingOrder);
}


int ConfPrices::getOpcode(QString s)
{
    if (s.isEmpty()) return OPC_INVALID;
    if (s == "DESCR") return OPC_DESCR;
    if (s == "PROF") return OPC_PROF;
    if (s == "AREA") return OPC_AREA;
    if (s == "ASSOC") return OPC_ASSOC;
    if (s == "UNION") return OPC_UNION;
    if (s == "VALID") return OPC_VALID;
    if (s.length() == 5) return OPC_ENTRY;
    return OPC_INVALID;
}


QString ConfPrices::getError(int opcode)
{
    QString s = "";
    switch(opcode) {
        case OPC_DESCR: s = QString::fromUtf8("Ungültige Beschreibung");
        case OPC_PROF: s = QString::fromUtf8("Ungültiger Berufszweig");
        case OPC_AREA: s = QString::fromUtf8("Ungültiges Tarifgebiet");
        case OPC_ASSOC: s = QString::fromUtf8("Ungültige Kasse");
        case OPC_UNION: s = QString::fromUtf8("Ungültiger Verband");
        case OPC_VALID: s = QString::fromUtf8("Fehlerhaftes Gültigkeitsdatum");
        default: break;
    }
    return s;
}

// Returns a list of SQL statements in order to import the price table
// If string list is empty or return value != 0 something went wrong
int ConfPrices::parse_pricelist_file(QString fname, QStringList &statements)
{
    int errcode = 0;

    // open price list file
    QFile file(fname);
    if (!file.open(QIODevice::ReadOnly)) {
        ui->textEdit->append(QString::fromUtf8("Fehler beim Öffnen der Preisliste aufgetreten!"));
        return -1;
    }
    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);
    QString line = "";

    QList<int> listAssoc;
    QList<int> listUnion;
    QDate validFrom;
    QString descr = "";
    QString unions;
    int prof = -1;
    int area = -1;

    QMap<QString, QList<QString>> mapPrices;
    bool bChecks[OPC_SIZE]; // indicates whether value conversion from price list file was successful or not
    for (int i = 0; i < OPC_SIZE - 1; i++) bChecks[i] = true;

    // parse price list file
    while (!line.isNull()) {
        line = stream.readLine();
        if (line.isEmpty()) continue;  // skip empty lines
        if (line[0] == '#') continue;  // skip comment lines

        QStringList list = line.split("=");
        if (list.count() != 2) continue;  // only one argument (parameter) is allowed
        switch(getOpcode(list.at(0))) {
            case OPC_DESCR:
                descr = list.at(1);
                break;
            case OPC_PROF:
                prof = (list.at(1)).toInt(&bChecks[OPC_PROF]);
                break;
            case OPC_AREA:
                area = (list.at(1)).toInt(&bChecks[OPC_AREA]);
                break;
            case OPC_ASSOC: {
                QStringList l = (list.at(1)).split(":");
                for (int i = 0; i < l.count(); i++) {
                    bool ok = true;
                    listAssoc.append((l.at(i)).toInt(&ok));
                    if (!ok) bChecks[OPC_ASSOC] = false;
                }
                }
                break;
            case OPC_UNION: {
                unions = list.at(1);
                QStringList l = (list.at(1)).split(":");
                for (int i = 0; i < l.count(); i++) {
                    bool ok = true;
                    listUnion.append((l.at(i)).toInt(&ok));
                    if (!ok) bChecks[OPC_UNION] = false;
                }
                }
                break;
            case OPC_VALID:
                validFrom = QDate::fromString(list.at(1), "yyyy-MM-dd");
                if (!validFrom.isValid()) bChecks[OPC_VALID] = false;
                break;
            case OPC_ENTRY: {
                QStringList l = (list.at(1)).split(":");
                if (l.count() == 2) {
                    mapPrices[list.at(0)].append(l.at(1));
                    mapPrices[list.at(0)].append(l.at(0));
                }
                }
                break;
            default: break;
        }
    }

    file.close();

    // sanity checks
    for (int i = 0; i < OPC_SIZE - 1; i++) if (!bChecks[i]) return i;
    if (prof < 0 || prof >= cProfession::PROF_COUNT) return OPC_PROF;
    if (area < 1) return OPC_AREA;
    if (area > listTariffAreas.count()) {
        if (area < cTariffScope::TARIFF_SCOPE_ID_GERMANY ||
            area > cTariffScope::TARIFF_SCOPE_ID_WEST) errcode = OPC_AREA;
    }
    if (listAssoc.count() < 1) return OPC_ASSOC;
    for (int i = 0; i < listAssoc.count(); i++) {
        int a = listAssoc.at(i);
        if (a <= 0 || a > cAssoc::ASSOC_COUNT - 1) return OPC_ASSOC;
    }
    if (listUnion.count() < 1) return OPC_UNION;
    if (listUnion.at(0) == 0 && listUnion.count() > 1) return OPC_UNION;
    for (int i = 0; i < listUnion.count(); i++) {
        int u = listUnion.at(i);
        if (u < cUnion::NONE) return OPC_UNION;
    }
    if (mapPrices.size() < 2) return OPC_ENTRY;
    if (mapPrices.size() % 2 != 0) return OPC_ENTRY;

    QString sql_statement;

    // create table name
    // FIXME: table_name must consider the assocation and/or union somehow
    QString table_name_part = "_";
    if (listAssoc.count() == 1) table_name_part += QString::number(listAssoc.at(0)) + "_";
    QString table_name = "P_" + QString::number(prof) + "_" + QString::number(area) + table_name_part + validFrom.toString("yyyyMMdd");

    // drop existing table statement
    sql_statement = "DROP TABLE IF EXISTS " + table_name;
    statements.append(sql_statement);

    // create table statement
    sql_statement = "CREATE TABLE " + table_name + " (pos_nr VARCHAR(5) NOT NULL PRIMARY KEY, price VARCHAR(6), addpay VARCHAR(6))";
    statements.append(sql_statement);

    QString prefix;
    if (db.DB_getProperties().getType() == cDatabase::DB_SQLITE) prefix = "INSERT OR IGNORE ";
    else prefix = "INSERT IGNORE ";

    // insert into price_tables
    for (int j = 0; j < listAssoc.count(); j++) {
        sql_statement = prefix + "INTO price_tables VALUES(NULL," + QString::number(prof) + "," +
                        QString::number(area) + "," + QString::number(listAssoc.at(j)) + ",'" +
                        unions + "','" + validFrom.toString("yyyy-MM-dd") + "','" + table_name + "','" + descr + "')";
        statements.append(sql_statement);
    }

    // insert price entries statements
    int assoc = listAssoc.at(0);
    for (int i = 0; i < mapPrices.size() / 2; i++) {
        QStringList l = mapPrices.value(mapPrices.keys().at(2 * i));
        if (assoc == cAssoc::BG && prof == cProfession::PROF_PHYSIOTHERAPY) { // if BG => cut the leading char from the position name
            QString pos = mapPrices.keys().at(2 * i);
            if (!pos.isEmpty()) pos = pos.remove(0, 1);
            sql_statement = ("INSERT INTO " + table_name + " VALUES ('" + pos + "', '" +
                              l.at(0) + "', '" + l.at(1) + "')");
        } else if (assoc == cAssoc::BG && prof == cProfession::PROF_ERGOTHERAPY) { // if BG => cut the leading zeros from the position name
            QString pos = mapPrices.keys().at(2 * i);
            if (!pos.isEmpty()) pos = pos.remove(0, 2);
            sql_statement = ("INSERT INTO " + table_name + " VALUES ('" + pos + "', '" +
                              l.at(0) + "', '" + l.at(1) + "')");
        } else {
            sql_statement = ("INSERT INTO " + table_name + " VALUES ('" + mapPrices.keys().at(2 * i) + "', '" +
                              l.at(0) + "', '" + l.at(1) + "')");
        }
        statements.append(sql_statement);
    }

    return errcode;
}


void ConfPrices::slotPrices()
{
    ui->textEdit->clear();

    if (listPriceFiles.isEmpty()) {
        ui->textEdit->append(QString::fromUtf8("Bitte erst eine Preisliste wählen"));
        return;
    }

    if (!db.isConnected()) {
        QMessageBox::warning(this, tr("OpenPhysio - Hinweis"), "Keine Verbindung zur Datenbank!", QMessageBox::Ok);
        return;
    }

    ui->textEdit->append("Starte Preislisten-Import - bitte warten ...");
    ui->textEdit->repaint();

    for (int i = 0; i < listPriceFiles.count(); i++) {
        ui->textEdit->append("<br>Importiere " + listPriceFiles.at(i) + "<br>");
        QStringList sqlStatements;
        int errcode = parse_pricelist_file(listPriceFiles.at(i), sqlStatements);
        if (errcode != 0) {
            ui->textEdit->append("Fehler beim Lesen aufgetreten! (Datei: " +
                                   listPriceFiles.at(i) + "; Fehlernummer: " + QString::number(errcode) +
                                   " " + getError(errcode) + ")");
            continue;
        }

        ui->textEdit->repaint();
        if (!db.executeBulk(sqlStatements)) {
            ui->textEdit->append("Fehler beim Datenbankimport der Preislisten aufgetreten! (Datei: " +
                                   listPriceFiles.at(i) + ")");
            continue;
        }
        ui->textEdit->repaint();
    }
    ui->textEdit->append("Import-Vorgang abgeschlossen.");
    reloadPriceLists();
}


void ConfPrices::slotSelectPriceFile()
{
    listPriceFiles = QFileDialog::getOpenFileNames(this,
     "Preislisten laden", ".", "Preislisten (*.opl)");
    ui->leFilePrices->setText(listPriceFiles.join(" ; "));
}


void ConfPrices::slotProfession(int i)
{
    loadTable(i);
}


void ConfPrices::slotViewList()
{
    MsgBox::dummy();
}


void ConfPrices::slotDelList()
{
    int r = ui->tablePriceLists->currentRow();
    if (r < 0) {
        QMessageBox::warning(this, tr("OpenPhysio"), QString::fromUtf8("Es ist keine Preisliste zum Löschen ausgewählt worden!\n"), QMessageBox::Ok);
        return;
    }
    if (r >= listPriceLists.count()) return;

    QAbstractItemModel* model;
    model = ui->tablePriceLists->model();
    QString s = model->index(r, 0).data().toString();
    QString msg;
    if (db.priceTableUsed(s)) msg = QString::fromUtf8("ACHTUNG: Preisliste wird von vorhandenen Rezepten verwenden!\n\nWirklich löschen?");
    else msg = QString::fromUtf8("Preisliste wirklich löschen?");
    MsgBox box;
    if (box.execYesNoBox(msg, MsgBox::BTN_NO) == MsgBox::BTN_YES) {
        if (!db.deletePriceTable(s)) {
            QMessageBox::warning(this, tr("OpenPhysio"), QString::fromUtf8("Fehler beim Löschen der Preisliste aufgetreten!\n"), QMessageBox::Ok);
        }
        reloadPriceLists();
    }
}
