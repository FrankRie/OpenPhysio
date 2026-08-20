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

#include "gendoc.h"
#include <time.h>
#include <QXmlStreamWriter>
#include "msgbox.h"
#include "employeetools.h"
#include "src_docedit/doctools.h"
#include <qrencode.h>

GenDoc gendoc;
int f_id;

GenDoc::GenDoc()
{
}


GenDoc::~GenDoc()
{
}


QString GenDoc::generateNumbering(int nr_type)
{
    QString inv_number;
	
    // retrieve current invoice/quittance number from database
    inv_number = db.getCurrentNumbering(nr_type);
    inv_number = QString::number(inv_number.toInt() + 1);
    return inv_number;
}

int GenDoc::getFId()
{
    return f_id;
}

QString GenDoc::generateNextNumbering(QString s)
{
    return QString::number(s.toInt() + 1);
}


int GenDoc::getPropertyDomainID(QString prop_dom)
{
    if (prop_dom.isEmpty()) return -1;
    if (prop_dom == "PRA") return myDB::TABLE_SURGERY;
    else if (prop_dom == "P") return myDB::TABLE_PATIENTS;
    else if (prop_dom == "R") return myDB::TABLE_RECEIPES;
    else if (prop_dom == "RNG") return myDB::TABLE_INVOICES;
    else if (prop_dom == "KRS") return myDB::TABLE_COURSES;
    else if (prop_dom == "A") return myDB::TABLE_PHYSICIANS;
    else if (prop_dom == "M") return myDB::TABLE_EMPLOYEES;
    else if (prop_dom == "QNG") return myDB::TABLE_QUITTANCES;
    else if (prop_dom == "MNG") return myDB::TABLE_REMINDERS;
    else if (prop_dom == "BER") return myDB::TABLE_REPORTS;
    else if (prop_dom == "PPRA") return myDB::TABLE_PRAXIS;
    else if (prop_dom == "GEN") return myDB::TABLE_DUMMY;
    return -1;
}


QString GenDoc::getProperty(QList<cTableItem*>&items, QString strProp)
{
    QString strRet = "";
    if (strProp.isEmpty() || items.count() < 1) return strRet;

    QStringList l = strProp.split("."); // split domain from property
    if (l.count() != 2) return strRet;

    QString dom = l.at(0);
    QString prop = l.at(1);
    if (dom.isEmpty() || prop.isEmpty()) return strRet;

    int prop_dom = getPropertyDomainID(dom);
    if (prop_dom < 0) return strRet;

    if (prop_dom == myDB::TABLE_DUMMY) {
        if (prop == "Dat") return QDate::currentDate().toString("dd.MM.yyyy");
    } else {
        // find the table item that corresponds to the specified domain
        for (int i = 0; i < items.count(); i++) {
            cTableItem *item = items.at(i);
            if (item != NULL) {
                if (item->getTableID() == prop_dom) {
                    strRet = item->getProperty(prop);
                    break;
                }
            }
        }
    }

    return strRet;
}


void GenDoc::fillTemplate(QList<cTableItem*>&items, cDoc *doc)
{
    if (doc == NULL) return;

    for (int x = 0; x < doc->listDataElements.count(); x++) {
        cEditorDataElement *cde = doc->listDataElements.at(x);
        if (cde != NULL) {
            QString token = cde->getToken();
            doc->replace(token, getProperty(items, token));
        }
    }

    doc->finish();
}


int GenDoc::execViewer(DlgDocViewer& viewer, QString title)
{
    viewer.setWindowTitle(title);
    viewer.prepare();
    return viewer.exec();
}


int GenDoc::chooseTemplate(int tmpl_type)
{
    // check whether there are different templates available for a certain document
    // if so => ask the user which template version to use
    QList<cTemplate*> listTemplates;
    for (int i = 0; i < db.listTemplates.count(); i++) {
        if ((db.listTemplates.at(i)->getTemplateType() == tmpl_type) && (db.listTemplates.at(i)->getTemplateFlags() == 0))  {
            listTemplates.append(new cTemplate());
            *listTemplates.last() = *db.listTemplates.at(i);
        }
    }
    if (listTemplates.isEmpty()) return -1;
    // if there is only one template available for the specified template type
    // don't bother the user
    if (listTemplates.count() == 1) return listTemplates.at(0)->getID();

    QStringList l;
    for (int i = 0; i < listTemplates.count(); i++) {
        l << listTemplates.at(i)->getName();
    }

    QDialog dlg;
    QGridLayout layout(&dlg);
    QComboBox cb;
    QLabel label;
    QPushButton pbs("Erzeugen");
    QPushButton pbc("Abbrechen");
    pbc.setIcon(QIcon(":/cancel.png"));
    pbs.setIcon(QIcon(":/new.png"));

    label.setText(QString::fromUtf8("Es stehen mehrere Dokumentenvorlagen zur Verfügung.\nWelche Vorlage möchten Sie verwenden?"));
    layout.addWidget(&pbs, 2, 1, 1, 1);
    layout.addWidget(&pbc, 2, 0, 1, 1);
    layout.addWidget(&label, 0, 0, 1, 2);
    layout.addWidget(&cb, 1, 0, 1, 2);
    connect(&pbc, &QPushButton::clicked, &dlg, &QDialog::reject);
    connect(&pbs, &QPushButton::clicked, &dlg, &QDialog::accept);
    cb.addItems(l);
    cb.setCurrentIndex(0);
    dlg.setWindowTitle(QString::fromUtf8("Vorlagenauswahl"));
    dlg.setWindowIcon(QIcon(":/favicon.png"));

    if (dlg.exec() == QDialog::Accepted) {
        int i = cb.currentIndex();
        if (i < 0 || i >= listTemplates.count()) return -1;
        return listTemplates.at(i)->getID();

    }
    while (!listTemplates.isEmpty()) delete listTemplates.takeFirst();
    return -1;
}


cTemplate GenDoc::getTemplate(cTableItem& item, int tmpl_type)
{
    return(getTemplate(item, tmpl_type, ""));
}

cTemplate GenDoc::getTemplate(cTableItem& item, int tmpl_type, QString name)
{
    cTemplate tmpl;
    int tmpl_id = -1;

    if (item.getID() < 0) {
        tmpl_id = chooseTemplate(tmpl_type);
        if (tmpl_id < 0) return tmpl;
    } else if (tmpl_type == cTemplateType::TT_PATIENTDATA) {
        tmpl_id = chooseTemplate(tmpl_type);
    } else {
        QString s = item.getProperty("TPL");
        if (!s.isEmpty()) {
            tmpl_id = s.toInt();
        } else {
            // for older docs (db version < 4) the template information might be missing
            // => we need to select a fallback template (simply select the first template that
            //    matches our template type)
            for (int i = 0; i < db.listTemplates.count(); i++) {
                if (db.listTemplates.at(i)->getTemplateType() == tmpl_type) {
                    tmpl_id = db.listTemplates.at(i)->getID();
                    break;
                }
            }
        }
    }

    if (tmpl_id >= 0) {
        for (int i = 0; i < db.listTemplates.count(); i++) {
            if (db.listTemplates.at(i)->getID() == tmpl_id) {
                tmpl = *db.listTemplates.at(i);
                break;
            }
        }
    }

    if (tmpl.getID() < 0) {
        MsgBox msgbox;
        msgbox.error(QString::fromUtf8("Die Dokumentenvorlage konnte nicht geladen werden!"));
    }

    return tmpl;
}


QByteArray GenDoc::prepareXml(QMap<QString, QString>& map, QString startElement)
{
    QBuffer buf;
    if (!buf.open(QIODevice::ReadWrite | QIODevice::Text)) return buf.buffer();
    QXmlStreamWriter writer;
    writer.setDevice(&buf);
    writer.writeStartDocument();
    writer.writeStartElement(startElement);
    QMapIterator<QString, QString> i(map);
    while (i.hasNext()) {
        i.next();
        writer.writeTextElement(i.key(), i.value());
    }
    writer.writeEndElement();
    buf.close();
    return buf.buffer();
}


int GenDoc::getAuthor(int def_id)
{
    QList<cEmployee *> l;
    EmplTool.getActiveEmployees(l);
    if (l.count() < 1) return -1;

    QDialog dlg;
    dlg.setWindowTitle(QString::fromUtf8("Verfasser"));
    dlg.setWindowIcon(QIcon(":/favicon.png"));

    QGridLayout layout(&dlg);
    QComboBox cb;
    QLabel label;
    QPushButton pbs("Weiter");
    QPushButton pbc("Abbrechen");
    pbc.setIcon(QIcon(":/cancel.png"));
    pbs.setIcon(QIcon(":/rightarrow.png"));

    label.setText(QString::fromUtf8("Verfasser des Dokumentes:"));
    layout.addWidget(&pbs, 2, 1, 1, 1);
    layout.addWidget(&pbc, 2, 0, 1, 1);
    layout.addWidget(&label, 0, 0, 1, 2);
    layout.addWidget(&cb, 1, 0, 1, 2);
    connect(&pbc, &QPushButton::clicked, &dlg, &QDialog::reject);
    connect(&pbs, &QPushButton::clicked, &dlg, &QDialog::accept);

    EmplTool.fillComboBox(&cb);
    cb.setCurrentIndex(EmplTool.getIndexOfEmployee(def_id, l));

    int iRet = -1;
    if (dlg.exec() == QDialog::Accepted)
    {
        int i = cb.currentIndex();
        if (i >= 0 && i < l.count()) iRet = l.at(i)->getID();
    }
    while (!l.isEmpty()) delete l.takeFirst();
    return iRet;
}

bool GenDoc::genInvoice(QList<cInvoice*>& invoices, int id)
{
    DlgDocViewer *viewer = new DlgDocViewer();
    if (viewer == NULL) return false;
    if (invoices.isEmpty()) return false;

    f_id = id;

    QList<cDoc *> docs;
    QList<cDateItem*> list;
    QList<cTableItem*> items;
    QString strNumber, strDates;
    QStringList pos;
    int pos_count = 0;
    int d_type = 0;
    int d_id = 0;

    cSurgery surg = db.getSurgery();

    cTemplate ct = getTemplate(*invoices.at(0), cTemplateType::TT_INVOICE);
    if (ct.getID() < 0) return false;

    cDocTemplate *tmpl = new cDocTemplate();
    tmpl->curTemplate = ct;

    for (int i = 0; i < invoices.count(); i++) {
        cInvoice *inv = invoices.at(i);
        items.clear();
        items.append(&surg);
        items.append(inv);

        docs.append(new cDoc(tmpl));
        cDoc *doc = docs.last();
        tmpl->setDoc(doc);
        tmpl->readTemplateFromByteArray(ct.getContents());

        cReceipe receipe;
        cCourse course;
        cPatient patient;

        if (inv->getID() < 0) {
            doc->setFlagNew(true);
            pos_count = 0;
            inv->setDate(QDate::currentDate());
            if (i == 0) strNumber = generateNumbering(myDB::NR_INVOICE);
            else strNumber = generateNextNumbering(strNumber);
            inv->setNumber(strNumber);

            db.retrieveItem(patient, inv->getPatientID());
            items.append(&patient);
            doc->replace("RNG.Adr", patient.getProperty("Adr"));

            if (inv->getType() == cInvoice::INVOICE_RECEIPE) {
                db.retrieveItem(receipe, inv->getReceipeID());
                d_id = receipe.getID();
                d_type = cDateType::DT_THERAPY;
                inv->setAmount(receipe.getBrutto());
                items.append(&receipe);
                cPriceTablePriv cpt;
                // load price table that fits to the receipe in order to decode positions
                db.loadPriceTablePrivate(receipe.getPriceTabID(), 0, cpt);
                if (!receipe.getPositions().isEmpty()) {
                    QStringList qsl = receipe.getPositions().split(";");
                    pos_count = qsl.count();
                    for (int y = 0; y < pos_count; y++) {
                        QStringList qsl2 = qsl[y].split(":");
                        int idx = qsl2[1].toInt();
                        int cnt = qsl2[0].toInt();
                        if (cnt == 0) break;
                        pos << qsl2[0] << cpt.qslPosName[idx] << cpt.qslPrice[idx] + " EUR" << cpt.getSum(idx, cnt) + " EUR";
                    }
                }
                if ((bool)receipe.getFlagVisit() && (cpt.count() > 1) ) {
                    pos_count += 2; // add "Hausbesuch" and "Kilometerpauschale"
                    int idx = cpt.count() - 2;
                    int cnt = receipe.getHBCount();
                    QString sco = QString::number(receipe.getHBCount());
                    pos << sco << cpt.qslPosName[idx] << cpt.qslPrice[idx] + " EUR" << cpt.getSum(idx, cnt) + " EUR";
                    idx++;
                    int dist = receipe.getDistance();
                    QString txt = "Kilometerpauschale (" + cpt.qslPrice[idx] + "/km, je " + QString::number(dist) + "km)";
                    cnt = cnt * dist;
                    pos << sco << txt << cpt.getSum(idx, dist) + " EUR" << cpt.getSum(idx, cnt) + " EUR";
                }
            } else if (inv->getType() == cInvoice::INVOICE_COURSE) {
                db.retrieveItem(course, inv->getCourseID());
                items.append(&course);
                d_id = course.getID();
                d_type = cDateType::DT_COURSE;
                inv->setAmount(course.getAmount());
                QString s;
                float f;
                pos << QString::number(course.getUnits()) << course.getName();
                f = course.getPrice().toFloat();
                s = QString::asprintf("%.2f", f);
                pos << s + " EUR";
                f = course.getPrice().toFloat() * course.getUnits();
                s = QString::asprintf("%.2f", f);
                inv->setAmount(s);
                s += " EUR";
                pos << s;
                pos_count = 1;
            }

            doc->replace("R.Diag", receipe.getDiagnosis());
            // retrieve dates
            QStringList dates;
            if (db.getDateList(list, d_type, d_id)) {
                for (int j = 0; j < list.count(); j++) {
                    dates << list.at(j)->getDate().toString("dd.MM.yyyy");
                }
            }
            while (!list.isEmpty()) delete list.takeFirst();
            strDates = dates.join(", ");
            doc->replace("RNG.Ter", strDates);

            // tax stuff
            QString s_tax;
            s_tax = QString::asprintf("%.2f", inv->getAmount().toFloat() * surg.getTaxPercent() / 100);
            doc->replace("RNG.USt", s_tax + " EUR");

        } else {
            // use the XML data stored within the invoice rather than the patient's current data
            // since salution might have changed meanwhile
            doc->replace("P.Anr", inv->getProperty("Anr"));
            doc->replace("R.Diag", inv->getProperty("Diag"));
            // decode positions stored in xml data
            QString s = inv->getProperty("POSITIONS");
            QStringList qsl = s.split(";");
            pos_count = qsl.count();
            for (int y = 0; y < pos_count; y++) {
                QStringList qsl2 = qsl[y].split(":");
                for (int x = 0; x < qsl2.count(); x++) pos << qsl2[x];
            }
        }

        for (int co = 0; co < pos_count; co++) {
            QString strco = QString::number(co + 1);
            QString tmp;
            doc->replace("RNG.AP" + strco, pos.at(4 * co + 0));
            doc->replace("RNG.P" + strco, pos.at(4 * co + 1));
            tmp = pos.at(4 * co + 2); tmp.replace(".",",",Qt::CaseInsensitive);
            doc->replace("RNG.EP" + strco, tmp);
            tmp = pos.at(4 * co + 3); tmp.replace(".",",",Qt::CaseInsensitive);
            doc->replace("RNG.GP" + strco, tmp);
        }

        QString amount = inv->getAmount();
        QString addr = inv->getProperty("Adr");
        int n = addr.size();
        int n1 = addr.indexOf('\n');
        addr = addr.mid(n1+1, n-n1);
        n1 = addr.indexOf('\n');
        n = addr.size();
        QString name = addr.mid(0, n1);

        // Generiere QR Code
        QString qrCode = "BCD\n002\n2\nSCT\nBYLADEM1001\nAnja Bilgett\nDE87120300001007293515\nEUR";
        qrCode = qrCode + amount + "\nDEPT\n\n" +  inv->getNumber() + " / " + name + "\n\n";

        QImage code = generateQrCode(qrCode, 100);

        QByteArray byteArray;
        QBuffer buffer(&byteArray);
        buffer.open(QIODevice::WriteOnly);
        code.save(&buffer, "PNG");

        //QString binaryString = byteArray.toBase64();

        doc->createPixmap(555, 800, byteArray);

        this->fillTemplate(items, doc);
        viewer->addDoc(doc);
        viewer->hide();

        this->fillTemplate(items, doc);
        viewer->addDoc(doc);
    }

    QString t = invoices.count() > 1 ? "Rechnungen" : "Rechnung";
    if (execViewer(*viewer, t) == QDialog::Accepted) {
        for (int i = 0; i < docs.count(); i++) {
            cDoc *doc = docs.at(i);
            cInvoice *inv = invoices.at(i);
            if (inv->getNumber().isEmpty()) inv->setNumber(doc->getResult("RNG.Nr"));

            // assemble positions to one single string
            QStringList positions;
            for (int y = 1; y <= pos_count; y++) {
                QStringList pos_tmp;
                QString tmp = QString::number(y);
                pos_tmp << doc->getResult("RNG.AP" + tmp);
                pos_tmp << doc->getResult("RNG.P" + tmp);
                pos_tmp << doc->getResult("RNG.EP" + tmp);
                pos_tmp << doc->getResult("RNG.GP" + tmp);
                positions.append(pos_tmp.join(":"));
            }

            QMap<QString, QString> map;
            map["tpl"] = QString::number(ct.getID());
            map["date"] = doc->getResult("RNG.Dat");
            map["nr"] = doc->getResult("RNG.Nr");
            map["amount"] = doc->getResult("RNG.Sum");
            map["addr"] = doc->getResult("RNG.Adr");
            map["salut"] = doc->getResult("P.Anr");
            map["diagnostics"] = doc->getResult("R.Diag"); // ////////new
            map["dates"] = doc->getResult("RNG.Ter");
            map["pos"] = positions.join(";");
            QByteArray b = prepareXml(map, "invoice");
            inv->setInvData(b);



	    // FIXME:
            // take date from doc->getResult("#INVOICE.DATE#"), check it and put it to inv->setDate()
            // because the user might have edited the date

            if (inv->getID() < 0) db.setCurrentNumbering(inv->getNumber(), myDB::NR_INVOICE);
            db.saveItem(*inv);

        }
    }

    // We have to delete the viewer object before the list of docs
    // because the viewer object stores pointers to the doc objects!!!
    delete viewer;
    while (!docs.isEmpty()) delete docs.takeFirst();
    delete tmpl;
    return true;
}

bool GenDoc::genPraxisInvoice(QList<cPraxisInvoice*>& invoices)
{
    DlgDocViewer *viewer = new DlgDocViewer();
    if (viewer == NULL) return false;

    QList<cDoc *> docs;
    QList<cDateItem*> list;
    QList<cTableItem*> items;
    QString strDates, strMpart, strAmount;
    QString strNumber;
    QStringList pos;
    int pos_count = 0;

    cSurgery surg = db.getSurgery();

    cTemplate ct = getTemplate(*invoices.at(0), cTemplateType::TT_INVOICE, /* QString::fromStdString */("Praxisrechnung"));
    if (ct.getID() < 0) return false;

    cDocTemplate *tmpl = new cDocTemplate();
    tmpl->curTemplate = ct;

    for (int i = 0; i < invoices.count(); i++) {
        cPraxisInvoice *inv = invoices.at(i);
        items.clear();
        items.append(&surg);
        items.append(inv);

        docs.append(new cDoc(tmpl));
        cDoc *doc = docs.last();
        tmpl->setDoc(doc);
        tmpl->readTemplateFromByteArray(ct.getContents());

        //parseDocTemplate(*doc, ConfXML::DOC_PINVOICE);

        int tFlag = 0;

        //cReceipe receipe;
        //cCourse course;
        cPraxis praxis;

        if (inv->getID() > 0)
            if (inv->getProperty("FORM").isEmpty()) tFlag = 1;

        db.retrieveItem(praxis, inv->getPraxisID());
        if ((inv->getID() < 0) || tFlag) {
            doc->setFlagNew(true);
            pos_count = 0;
            if (inv->getDate().isNull())
                inv->setDate(QDate::currentDate());

            if (!tFlag) {
                if (i == 0) strNumber = generateNumbering(myDB::NR_INVOICE);
                else strNumber = generateNextNumbering(strNumber);
                inv->setNumber(strNumber);
            }

            doc->replace("PPRA.Adr", praxis.getProperty("Adr"));

            if (praxis.getProperty("Fullname") == "") {
                doc->replace("P.Anr", "Sehr geehrte Damen und Herren,");
            }
            else {
                if (praxis.getGender() == 0)
                    doc->replace("P.Anr", "Sehr geehrter Herr " + praxis.getProperty("Fullname") + ",");
                else
                    doc->replace("P.Anr", "Sehr geehrte Frau " + praxis.getProperty("Fullname") + ",");
            }

            doc->replace("RNG.Nr", inv->getNumber());
            strDates = inv->getDate().toString("dd.MM.yyyy");
            doc->replace("RNG.Dat", strDates);
            strDates = inv->getSdate().toString("dd.MM.yyyy");
            doc->replace("RNG.Sdate", strDates);
            strDates = inv->getEdate().toString("dd.MM.yyyy");
            doc->replace("RNG.Edate", strDates);
            strMpart = QString::asprintf("%2d", (100 - praxis.getPpart()));
            doc->replace("PPRA.MPart", strMpart);
            float f;
            f = inv->getAmount().toFloat();
            strAmount = QString::asprintf("%.2f EUR", f);
            doc->replace("RNG.Sum1", strAmount.replace(".", ",", Qt::CaseInsensitive));
            strAmount = QString::asprintf("%.2f EUR", (inv->getAmount()).toFloat() * (100 - praxis.getPpart())/100);
            doc->replace("RNG.Sum", strAmount.replace(".", ",", Qt::CaseInsensitive));
            doc->replace("PRA.Name", surg.getName1());
        } else {
            // use the XML data stored within the invoice rather than the patient's current data
            // since salution might have changed meanwhile
            doc->replace("P.Anr", inv->getProperty("FORM"));
            doc->replace("PPRA.MPart", inv->getProperty("MPART"));
            doc->replace("RNG.Sum1", inv->getProperty("SAMOUNT"));
            doc->replace("RNG.Sum", inv->getProperty("AMOUNT"));
            doc->replace("PRA.Name", inv->getProperty("NAME"));
            doc->replace("PPRA.Adr", inv->getProperty("ADDR"));
            doc->replace("RNG.Sdate", inv->getProperty("SDATE"));
            doc->replace("RNG.Edate", inv->getProperty("EDATE"));
            doc->replace("RNG.Dat", inv->getProperty("DATE"));
            doc->replace("RNG.Nr", inv->getProperty("NR"));
            doc->replace("R.Diag", inv->getProperty("DIAG"));
        }
        this->fillTemplate(items, doc);
        viewer->addDoc(doc);
    }

    QString t = invoices.count() > 1 ? "Rechnungen" : "Rechnung";
    if (execViewer(*viewer, t) == QDialog::Accepted) {
        for (int i = 0; i < docs.count(); i++) {
            cDoc *doc = docs.at(i);
            cPraxisInvoice *inv = invoices.at(i);
            if (inv->getNumber().isEmpty()) inv->setNumber(doc->getResult("RNG.Nr"));

            QMap<QString, QString> map;
            map["date"] = doc->getResult("RNG.Dat");
            map["nr"] = doc->getResult("RNG.Nr");
            map["samount"] = doc->getResult("RNG.Sum1");
            map["amount"] = doc->getResult("RNG.Sum");
            map["name"] = doc->getResult("PRA.Name");
            map["sdate"] = doc->getResult("RNG.Sdate");
            map["edate"] = doc->getResult("RNG.Edate");
            map["mpart"] = doc->getResult("PPRA.MPart");
            map["form"] = doc->getResult("P.Anr");
            map["addr"] = doc->getResult("PPRA.Adr");
            QByteArray b = prepareXml(map, "invoice");
            inv->setInvData(b);

            // FIXME:
            // take date from doc->getResult("#INVOICE.DATE#"), check it and put it to inv->setDate()
            // because the user might have edited the date

            // the number is always set over dlgpinvoice!
            //if (inv->getID() < 0) db.setCurrentNumbering(inv->getNumber(), myDB::NR_INVOICE);

            db.saveItem(*inv);
        }
    }

    // We have to delete the viewer object before the list of docs
    // because the viewer object stores pointers to the doc objects!!!
    delete viewer;
    while (!docs.isEmpty()) delete docs.takeFirst();
    return true;
}

void GenDoc::genPraxisAppendixNew(QList<cDoc *> &docs, cPraxisInvoice *inv)
{
    cTemplate ct = getTemplate(*inv, cTemplateType::TT_VARIOUS);
    if (ct.getID() < 0) return;

    cDocTemplate *tmpl = new cDocTemplate();
    tmpl->curTemplate = ct;

    docs.append(new cDoc(tmpl));
    cDoc *doc = docs.last();
    tmpl->setDoc(doc);
    tmpl->readTemplateFromByteArray(ct.getContents());

    //parseDocTemplate(*docs.last(), ConfXML::DOC_PAPPENDIX);

    doc->replace("RNG.Nr", inv->getNumber());
    doc->replace("RNG.Sdate", inv->getSdate().toString("dd.MM.yyyy"));
    doc->replace("RNG.Edate", inv->getEdate().toString("dd.MM.yyyy"));
}

bool GenDoc::genPraxisAppendix(QList<cPraxisInvoice*>& invoices)
{
    int row_count =0;
    int p_count =1;
    QList<cAppendixItem *> list;
    DlgDocViewer *viewer = new DlgDocViewer();
    if (viewer == NULL) return false;

    QList<cDoc *> docs;
    QList<cTableItem*> items;
    QString strIns, strAnz, strAmount;
    QString strRlist;
    float sumAmount = 0.0;

    cSurgery surg = db.getSurgery();

    cTemplate ct = getTemplate(*invoices.at(0), cTemplateType::TT_INVOICE);
    if (ct.getID() < 0) return false;

    cDocTemplate *tmpl = new cDocTemplate();
    tmpl->curTemplate = ct;

    cPatient patient;
    cInsurance insurance;
    cPraxisInvoice *inv = NULL;

    for (int k = 0; k < invoices.count(); k++) {
        inv = invoices.at(k);
        items.clear();
        items.append(&surg);
//        items.append(inv);

        db.loadPraxisInvoiceAppendix(list, inv->getRlist(), inv->getSdate(), inv->getEdate());

        genPraxisAppendixNew(docs, inv);
        row_count = 1;
        docs.last()->replace("PA.Page", QString::number(p_count));
        p_count++;

        int j = 0;
        for (int i = 0; i < list.count(); i++) {
            db.retrieveItem(patient, list.at(i)->getPid());
            docs.last()->replace("PA.Na" + QString::number(row_count), patient.getName() + ", " + patient.getForename().left(1) + ".");
            docs.last()->replace("PA.Da" + QString::number(row_count), list.at(i)->getRdate().toString("dd.MM.yy"));

            switch (list.at(i)->getPricetab()) {
            case 1:
                strIns = "Privatrezept";
                break;
            case 2:
                strIns = "Selbstzahler";
                break;
            case 3:
                strIns = "Heilpraktikerrezept";
                break;
            case 4:
                strIns = "";
                break;
            default:
                db.getInsurance(list.at(i)->getInsurance(), insurance);
                strIns = insurance.getShortName();
                break;
            }

            docs.last()->replace("PA.Ins" + QString::number(row_count), strIns);

            strAnz = list.at(i)->getPositions();
            QStringList qsl0 = strAnz.split(";");
            QStringList qsl1 = qsl0[0].split(":");

            strAmount = list.at(i)->getBrutto();
            docs.last()->replace("PA.Brt" + QString::number(row_count), strAmount.replace(".", ",", Qt::CaseInsensitive) + " EUR");
            row_count++;
            if (++j % 28 == 0) {
                fillTemplate(items, docs.last());
                viewer->addDoc(docs.last());
                genPraxisAppendixNew(docs, inv);
                row_count = 1;
                docs.last()->replace("PA.Page", QString::number(p_count));
                p_count++;
            }
            for (int l = 0; l < qsl0.count(); l++) {
                qsl1 = qsl0[l].split(":");
                if (qsl1[0] == "") break;
                docs.last()->replace("PA.Num" + QString::number(row_count), qsl1[0]);
                switch (list.at(i)->getPricetab()) {
                case 1:
                case 2:
                case 3:
                    {   int idx = qsl1[1].toInt();
                        cPriceTablePriv cpt;
                        // load price table that fits to the receipe in order to decode positions
                        db.loadPriceTablePrivate(list.at(i)->getPricetab(), 0, cpt);
                        docs.last()->replace("PA.Thera" + QString::number(row_count), cpt.qslPosName[idx]);
                    }
                    break;
                default:
                    {   int idx = qsl1[1].toInt();
                        cPriceTable cpt;
                        QString tn = db.getPriceTableName(list.at(i)->getPricetab());
                        db.loadPriceTable(tn, cpt, 0);
                        QString posstr = cpt.getPosNr(idx);
                        for (int j = 0; j < db.table_hm_pos.count(); j++) {
                            cHMPos *p = (cHMPos*)(db.table_hm_pos.at(j));
                            if (p == NULL) continue;
                            //if (prof == cProfession::PROF_PHYSIOTHERAPY && !p->getFlagPhysio()) continue;
                            //if (prof == cProfession::PROF_ERGOTHERAPY && !p->getFlagErgo()) continue;
                            if (p->getPosNr() == posstr) {
                                docs.last()->replace("PA.Thera" + QString::number(row_count), /*cpt.getPosNr(idx));*/
                                                     p->getDescriptionShort());
                                break;
                            }
                        }
                    }
                    break;
                }
                if (++j % 28 == 0) {
                    fillTemplate(items, docs.last());
                    viewer->addDoc(docs.last());
                    genPraxisAppendixNew(docs, inv);
                    row_count = 0;
                    docs.last()->replace("PA.Page", QString::number(p_count));
                    p_count++;
                }
                row_count++;
            }
        }
        QString s;
        s = QString::asprintf("%d", list.at(0)->getId());
        strRlist = s;
        for (int i = 1; i < list.count(); i++) {
            s = QString::asprintf("%d", list.at(i)->getId());
            strRlist = strRlist + ":" + s;
        }

        sumAmount = 0;
        for (int i = 0; i < list.count(); i++) {
            sumAmount +=  (list.at(i)->getBrutto()).toFloat();
        }

        fillTemplate(items, docs.last());
        viewer->addDoc(docs.last());
    }

    QString t = "Anlage";
    if (execViewer(*viewer, t) == QDialog::Accepted) {
        inv->setRlist(strRlist);
        strAmount = QString::asprintf("%.2f", sumAmount);
        inv->setAmount(strAmount);

        // FIXME:
        // take date from doc->getResult("#INVOICE.DATE#"), check it and put it to inv->setDate()
        // because the user might have edited the date
        if (inv->getID() < 0) db.setCurrentNumbering(inv->getNumber(), myDB::NR_INVOICE);
        db.saveItem(*inv);
    }
    // We have to delete the viewer object before the list of docs
    // because the viewer object stores pointers to the doc objects!!!
    delete viewer;
    while (!docs.isEmpty()) delete docs.takeFirst();
    return true;
}
#define MAX_REPORT_CONTENT_FIELDS 7

bool GenDoc::genReport(cReport& rep)
{
    cPatient p;
    if (rep.getPatientID() >= 0) {
        db.retrieveItem(p, rep.getPatientID());
    }
    int author_id = rep.getAuthorID();
    if (author_id < 0) {
        author_id = getAuthor(p.getDefaultEmployeeID());
    }
    if (author_id < 0) return false;

    cTemplate ct = getTemplate(rep, cTemplateType::TT_REPORT);
    if (ct.getID() < 0) return false;

    cDocTemplate *tmpl = new cDocTemplate();
    tmpl->curTemplate = ct;
    cDoc *doc = new cDoc(tmpl);
    tmpl->setDoc(doc);
    tmpl->readTemplateFromByteArray(ct.getContents());

    QList<cTableItem*> items;
    cSurgery surg = db.getSurgery();
    items.append(&surg);

    if (rep.getID() < 0) {  // this is a new report
        doc->setFlagNew(true);
        for (int i = 0; i < MAX_REPORT_CONTENT_FIELDS; i++)
            doc->replace("BER.F" + QString::number(i + 1), "Freitext");
        rep.setDate(QDate::currentDate());
    }

    items.append(&rep);
    if (p.getID() > 0) items.append(&p);

    cPhys phy;
    if (rep.getPhysID() >= 0) {
        db.retrieveItem(phy, rep.getPhysID());
        items.append(&phy);
    }

    fillTemplate(items, doc);
    DlgDocViewer viewer;
    viewer.addDoc(doc);
    bool bRet = false;
    if (execViewer(viewer, "Therapiebericht") == QDialog::Accepted) {
        QMap<QString, QString> map;
        map["tpl"] = QString::number(ct.getID());
        for (int i = 0; i < MAX_REPORT_CONTENT_FIELDS; i++)
            map["content" + QString::number(i + 1)] = doc->getResult("BER.F" + QString::number(i + 1));
        QByteArray b = prepareXml(map, "report");
        rep.setAuthorID(author_id);
        rep.setRData(b);
        rep.setDate(QDate::fromString(doc->getResult("BER.Dat"), "dd.MM.yyyy"));
        bRet = db.saveItem(rep);
    }

    delete doc;
    delete tmpl;
    return bRet;
}


bool GenDoc::genQuittance(cQuittance& quittance, QString amount)
{
    // TODO: we should perform a template subtype check
    //  if it's an addpay quittance we should display the addpay template(s) first
    cTemplate ct = getTemplate(quittance, cTemplateType::TT_QUITTANCE);
    if (ct.getID() < 0) return false;

    cDocTemplate *tmpl = new cDocTemplate();
    tmpl->curTemplate = ct;

    cDoc *doc = new cDoc(tmpl);
    tmpl->setDoc(doc);
    tmpl->readTemplateFromByteArray(ct.getContents());

    QList<cTableItem*> items;
    cSurgery surg = db.getSurgery();
    items.append(&surg);

    cPatient p;
    cReceipe r;
    if (quittance.getReceipeID() >= 0) {
        db.retrieveItem(r, quittance.getReceipeID());
        items.append(&r);
    }
    if (quittance.getPatientID() >= 0) {
        db.retrieveItem(p, quittance.getPatientID());
        items.append(&p);
    }

    QString strQNr;
    if (quittance.getID() < 0) {  // this is a new quittance
        doc->setFlagNew(true);
        QString strAmount, strPos1, strPos2;
        if (quittance.getType() == cQuittance::Q_ADDPAY) {
            strAmount = r.getAddpay();
            float addpay = strAmount.toFloat();
            // FIXME: remove hardcoded value
            strPos1 = "10,00";
            addpay -= 10;
            strPos2 = QString::asprintf("%.2f", addpay);
            doc->replace("QNG.Z", QString::fromUtf8("Zuzahlung für Verordnung vom ") + r.getDate().toString("dd.MM.yyyy"));
        } else {
            strAmount = amount;
            doc->replace("QNG.Z", quittance.getTypeString());
        }

        doc->replace("QNG.Sum", strAmount.replace(".", ",", Qt::CaseInsensitive));
        doc->replace("QNG.Pos1", strPos1);
        doc->replace("QNG.Pos2", strPos2);
        doc->replace("QNG.Dat", quittance.getDate().toString("dd.MM.yyyy"));
        // FIXME: We need to implement a lock for the selected quittance number
        strQNr = generateNumbering(myDB::NR_QUITTANCE);
        doc->replace("QNG.Nr", strQNr);
    }

    if (quittance.getType() == cQuittance::Q_ADDPAY) doc->replace("QNG.RDat", r.getProperty("RDat"));

    items.append(&quittance);
    fillTemplate(items, doc);

    bool bRet = false;
    DlgDocViewer viewer;
    viewer.addDoc(doc);
    if (execViewer(viewer, "Zuzahlungsquittung") == QDialog::Accepted) {
        QMap<QString, QString> map;
        map["tpl"] = QString::number(ct.getID());
        map["pos1"] = doc->getResult("QNG.Pos1");
        map["pos2"] = doc->getResult("QNG.Pos2");
        map["amount"] = doc->getResult("QNG.Sum");
        map["qnr"] = doc->getResult("QNG.Nr");
        map["purpose"] = doc->getResult("QNG.Z");
        map["date"] = doc->getResult("QNG.Dat");
        QByteArray b = prepareXml(map, "quittance");
        quittance.setQData(b);
        if (quittance.getID() < 0) {
            // FIXME: We need to implement a lock for the selected quittance number
            quittance.setNumber(generateNumbering(myDB::NR_QUITTANCE));
            // store global quittance number
            db.setCurrentNumbering(quittance.getNumber(), myDB::NR_QUITTANCE);
        }
        bRet = db.saveItem(quittance);
    }

    delete doc;
    delete tmpl;
    return bRet;
}


bool GenDoc::genReminder(cReminder& reminder)
{
    cTemplate ct = getTemplate(reminder, cTemplateType::TT_REMINDER);
    if (ct.getID() < 0) return false;

    cDocTemplate *tmpl = new cDocTemplate();
    tmpl->curTemplate = ct;

    cDoc *doc = new cDoc(tmpl);
    tmpl->setDoc(doc);
    tmpl->readTemplateFromByteArray(ct.getContents());

    QList<cTableItem*> items;
    cSurgery surg = db.getSurgery();
    items.append(&surg);

    cInvoice inv;
    cPatient patient;
    if (db.retrieveItem(inv, reminder.getInvoiceID())) items.append(&inv);

    if (reminder.getID() < 0) {
        doc->setFlagNew(true);
        if (db.retrieveItem(patient, reminder.getPatientID())) items.append(&patient);
        doc->replace("MNG.Sum", inv.getAmount() + " EUR");
        doc->replace("MNG.Dat", QDate::currentDate().toString("dd.MM.yyyy"));
        doc->replace("RNG.Nr", inv.getNumber() + " vom " + inv.getDate().toString("dd.MM.yyyy"));
        int add_days = myConf.get(ConfXML::AREA_OTHER, "reminder_period").toInt();
        doc->replace("MNG.ZDat", QDate::currentDate().addDays(add_days).toString("dd.MM.yyyy"));
    } else {
        // use the patient data stored in xml rather than the patient object
        // advantage: even if the patient object was delete the reminder is still complete
        doc->replace("P.Adr", reminder.getProperty("Adr"));
        doc->replace("P.Anr", reminder.getProperty("Anr"));
    }

    items.append(&reminder);
    fillTemplate(items, doc);

    bool bRet = false;
    DlgDocViewer viewer;
    viewer.addDoc(doc);
    if (execViewer(viewer, "Mahnung") == QDialog::Accepted) {  // save reminder
        QMap<QString, QString> map;
        map["tpl"] = QString::number(ct.getID());
        map["addr"] = doc->getResult("P.Adr");
        map["salut"] = doc->getResult("P.Anr");
        map["diagnostics"] = doc->getResult("R.Diag"); // ////////new
        map["date"] = doc->getResult("MNG.Dat");
        map["amount"] = doc->getResult("MNG.Sum");
        map["paydate"] = doc->getResult("MNG.ZDat");
        QByteArray b = prepareXml(map, "reminder");
        reminder.setRData(b);
        bRet = db.saveItem(reminder);
    }

    delete doc;
    delete tmpl;
    return bRet;
}


bool GenDoc::genParticipanceConfirmation(cCourse& course, QList<cPatient *> &patients)
{
    cTableItem dummy(myDB::TABLE_COURSES);
    cTemplate ct = getTemplate(dummy, cTemplateType::TT_PARTICIPANCE);
    if (ct.getID() < 0) return false;

    cDocTemplate *tmpl = new cDocTemplate();
    tmpl->curTemplate = ct;

    QList<cDoc *> docs;

    QList<cTableItem*> items;
    cSurgery surg = db.getSurgery();

    DlgDocViewer* viewer = new DlgDocViewer();

    // currently we do not save participance confirmations in our database
    viewer->setChangeTrackingEnabled(false);
    viewer->setSavingEnabled(false);

    // if there are no patients (course members) specified we need to create at least one empty instance of
    // a doc (e.g. this might be the case if a user wants to create an empty participance template list)
    if (patients.count() == 0) {
        docs.append(new cDoc(tmpl));
        cDoc *d = docs.first();
        tmpl->setDoc(d);
        tmpl->readTemplateFromByteArray(ct.getContents());
        fillTemplate(items, d);
        viewer->addDoc(d);
    } else {
        cEmployee employee;
        db.retrieveItem(employee, course.getLeaderID());

        // retrieve dates
        QList<cDateItem*> list;
        QStringList dates;
        if (db.getDateList(list, cDateType::DT_COURSE, course.getID())) {
            for (int i = 0; i < list.count(); i++) {
                dates << list.at(i)->getDate().toString("dd.MM.yyyy");
            }
        }
        while (!list.isEmpty()) delete list.takeFirst();
        QString strDates = dates.join(", ");

        for (int i = 0; i < patients.count(); i++) {
            items.clear();
            items.append(&surg);
            items.append(&course);
            items.append(&employee);
            items.append(patients.at(i));
            docs.append(new cDoc(tmpl));
            cDoc *d = docs.last();
            tmpl->setDoc(d);
            tmpl->readTemplateFromByteArray(ct.getContents());
            d->replace("KRS.Ter", strDates);
            fillTemplate(items, d);
            viewer->addDoc(d);
        }
    }

    if (patients.count() > 1 )execViewer(*viewer, QString::fromUtf8("Teilnahmebestätigungen"));
    else execViewer(*viewer, QString::fromUtf8("Teilnahmebestätigung"));

    delete viewer;
    while (!docs.isEmpty()) delete docs.takeFirst();
    delete tmpl;
    return true;
}


#define MAX_NUMBER_OF_PARTICIPANTS_PER_PAGE 9

bool GenDoc::genPartList(cCourse& course, QList<cPatient*> &patients, QString strLead)
{
    cTableItem dummy(myDB::TABLE_COURSES);
    cTemplate ct = getTemplate(dummy, cTemplateType::TT_PARTLIST);
    if (ct.getID() < 0) return false;

    cDocTemplate *tmpl = new cDocTemplate();
    tmpl->curTemplate = ct;

    QList<cTableItem*> items;

    DlgDocViewer *viewer = new DlgDocViewer();
    QList<cDoc *> docs;
    int row_count = 0;

    cEmployee employee;
    if (course.getLeaderID() > 0) db.retrieveItem(employee, course.getLeaderID());

    // currently we do not save participant lists in our database
    viewer->setChangeTrackingEnabled(false);
    viewer->setSavingEnabled(false);

    // if there are no patients (course members) specified we need to create at least one empty instance of
    // a doc (e.g. this might be the case if a user wants to create an empty participant template list)
    if (patients.count() == 0) {
        docs.append(new cDoc(tmpl));
        cDoc *doc = docs.first();
        tmpl->setDoc(doc);
        tmpl->readTemplateFromByteArray(ct.getContents());
        items.append(&course);
        if (employee.getID() > 0) items.append(&employee);
        else doc->replace("M.Nam", strLead);
    } else {
        items.append(&course);
        items.append(&employee);
    }

    for (int i = 0; i < patients.count(); i++) {
        if (i % MAX_NUMBER_OF_PARTICIPANTS_PER_PAGE == 0) {
            // start new page
            docs.append(new cDoc(tmpl));
            tmpl->setDoc(docs.last());
            tmpl->readTemplateFromByteArray(ct.getContents());
            row_count = 1;
        }
        cPatient *p = patients.at(i);
        if (p != NULL) {
            docs.last()->replace("KRS.T" + QString::number(row_count), p->getFullName());
            docs.last()->replace("KRS.TA" + QString::number(row_count), p->getStreet() + "\n" + p->getZipcode() + " " + p->getCity());
            docs.last()->replace("KRS.TK" + QString::number(row_count), p->getTelephone() + "\n" + p->getEMail());
            row_count++;
        }
        if (i % MAX_NUMBER_OF_PARTICIPANTS_PER_PAGE == MAX_NUMBER_OF_PARTICIPANTS_PER_PAGE - 1) {
            fillTemplate(items, docs.last());
            viewer->addDoc(docs.last());
        }
    }

    fillTemplate(items, docs.last());
    viewer->addDoc(docs.last());
    execViewer(*viewer, "Teilnahmeliste");

    delete viewer;
    while (!docs.isEmpty()) delete docs.takeFirst();
    delete tmpl;
    return true;
}


bool GenDoc::genMusterX(int profession, cReceipe& receipe, cPriceTable &cpt)
{
    cTableItem dummy(myDB::TABLE_DUMMY);
    cTemplate ct = getTemplate(dummy, cTemplateType::TT_RECEIPETMPL);
    if (ct.getID() < 0) return false;

    cDocTemplate *tmpl = new cDocTemplate();
    tmpl->curTemplate = ct;
    cDoc *doc = new cDoc(tmpl);
    tmpl->setDoc(doc);
    tmpl->readTemplateFromByteArray(ct.getContents());

    QList<cTableItem*> items;

    cSurgery surg = db.getSurgery();
    items.append(&surg);
    items.append(&receipe);

    if (!receipe.getPositions().isEmpty()) {
        QStringList qsl = receipe.getPositions().split(";");
        int co = qsl.count();
        if (co > 2) co = 2; // GKV receipe is limited to 2 positions
        for (int y = 0; y < co; y++) {
            if (QString(qsl[y]).isEmpty()) continue;
            QStringList qsl2 = qsl[y].split(":");
            if (qsl2.isEmpty()) continue;
            int pos_count = qsl2[0].toInt();
            if (pos_count < 1) continue;
            int pos_nr_idx = qsl2[1].toInt();
            cPriceTableItem* cpti = cpt.at(pos_nr_idx);
            if (cpti == NULL) continue;
            doc->replace("R.Pos" + QString::number(y + 1), cpti->getPosNr());
            doc->replace("R.Fak" + QString::number(y + 1), QString::number(pos_count));
        }
    }

    if (receipe.getFlagVisit() && receipe.getHBCount() > 0) {
        int idx_flat = receipe.getHBPosFlat();
        if (idx_flat > 0) {
            doc->replace("R.HB1", cpt.getPosNr(idx_flat));
            doc->replace("R.HBF1", QString::number(receipe.getHBCount()));
        }
        int idx_dist = receipe.getHBPosDist();
        if (idx_dist > 0) {
            doc->replace("R.WG", cpt.getPosNr(idx_dist));
            doc->replace("R.WGF", QString::number(receipe.getHBCount()));
            doc->replace("R.KM", QString::number(receipe.getDistance()));
        }
    }

    DlgDocViewer viewer;
    viewer.setChangeTrackingEnabled(false);
    viewer.setSavingEnabled(false);
    viewer.setPrinterMode(ConfXML::PRN_RECEIPE);
    viewer.setPrintScale(true);
    fillTemplate(items, doc);
    viewer.addDoc(doc);
    QString title = QString::fromUtf8("Druckvorschau für Heilmittelverordnung (Muster ");
    if (profession == cProfession::PROF_PHYSIOTHERAPY) title += "13)";
    else title += "18)";
    execViewer(viewer, title);

    delete doc;
    delete tmpl;
    return true;
}


bool GenDoc::genCorrection()
{
    cTableItem dummy(myDB::TABLE_DUMMY);
    cTemplate ct = getTemplate(dummy, cTemplateType::TT_CORRECTION);
    if (ct.getID() < 0) return false;

    cDocTemplate *tmpl = new cDocTemplate();
    tmpl->curTemplate = ct;
    cDoc *doc = new cDoc(tmpl);
    tmpl->setDoc(doc);
    tmpl->readTemplateFromByteArray(ct.getContents());

    QList<cTableItem*> items;
    cSurgery surg = db.getSurgery();
    items.append(&surg);

    DlgDocViewer viewer;
    viewer.setChangeTrackingEnabled(false);
    viewer.setSavingEnabled(false);
    fillTemplate(items, doc);
    viewer.addDoc(doc);
    execViewer(viewer, QString::fromUtf8("Rezeptkorrektur"));
    delete doc;
    delete tmpl;
    return true;
}


bool GenDoc::genSchedule(cEmployee& empl, int mode, QDate start_date, QDate end_date, QTime start_time, QTime end_time)
{
    QList<cDateItem *> listDates;
    if (!db.loadDates(listDates, start_date, end_date, empl.getID())) {
        return false;
    }

    cTableItem dummy(myDB::TABLE_DUMMY);
    cTemplate ct = getTemplate(dummy, cTemplateType::TT_SCHEDULE);
    if (ct.getID() < 0) return false;

    cDocTemplate *tmpl = new cDocTemplate();
    tmpl->curTemplate = ct;
    cDoc *doc = new cDoc(tmpl);
    tmpl->setDoc(doc);
    tmpl->readTemplateFromByteArray(ct.getContents());

    QList<cTableItem*> items;
    items.append(&empl);
    QString s, sdates;
    if (mode == 0) { // day view
        s = start_date.toString("dd. MMMM yyyy") + "\n" + start_time.toString("hh:mm") + " bis " +
            end_time.toString("hh:mm") + " Uhr";
        for (int i = 0; i < listDates.count(); i++) {
            cDateItem *cdi = listDates.at(i);
            if ( (cdi->getStart() >= start_time) && (cdi->getStart() <= end_time) ) {
                sdates = sdates + cdi->getStart().toString("hh:mm") + " - " + cdi->getEnd().toString("hh:mm") + " Uhr";
                sdates = sdates + "\t" + cdi->getTitle(true, false, false) + "\n";
            }
        }
    } else { // week view
        s = start_date.toString("dd. MMMM yyyy") + " - " + end_date.toString("dd. MMMM yyyy") + "\njeweils " + start_time.toString("hh:mm") + " bis " +
            end_time.toString("hh:mm") + " Uhr";
        int co = start_date.daysTo(end_date);
        for (int j = 0; j <= co; j++) {
            sdates += start_date.addDays(j).toString("dd. MMMM yyyy") + "\n";
            bool d_found = false;
            for (int i = 0; i < listDates.count(); i++) {
                cDateItem *cdi = listDates.at(i);
                if (cdi->getDate() == start_date.addDays(j)) {
                    if ( (cdi->getStart() >= start_time) && (cdi->getStart() <= end_time) ) {
                        sdates = sdates + cdi->getStart().toString("hh:mm") + " - " + cdi->getEnd().toString("hh:mm") + " Uhr";
                        sdates = sdates + "\t" + cdi->getTitle(true, false, false) + "\n";
                        d_found = true;
                    }
                }
            }
            if (!d_found) sdates += "Keine Termine vorhanden.";
            sdates += "\n\n";
        }
    }
    if (sdates.isEmpty()) sdates = "Keine Termine vorhanden.";
    doc->replace("T.Z", s);
    doc->replace("T.Ter", sdates);

    DlgDocViewer viewer;
    viewer.setChangeTrackingEnabled(false);
    viewer.setSavingEnabled(false);
    fillTemplate(items, doc);
    viewer.addDoc(doc);
    execViewer(viewer, QString::fromUtf8("Terminplan"));

    while (!listDates.isEmpty()) delete listDates.takeFirst();
    delete doc;
    delete tmpl;
    return true;
}


bool GenDoc::genPatientData(cPatient& patient)
{
    cTemplate ct = getTemplate(patient, cTemplateType::TT_PATIENTDATA);
    if (ct.getID() < 0) return false;

    cDocTemplate *tmpl = new cDocTemplate();
    tmpl->curTemplate = ct;
    cDoc *doc = new cDoc(tmpl);
    tmpl->setDoc(doc);
    tmpl->readTemplateFromByteArray(ct.getContents());

    if (patient.getGender() == GENDER_FEMALE) {
        doc->replace("P.F", "X");
        doc->replace("P.M", " ");
    } else {
        doc->replace("P.F", " ");
        doc->replace("P.M", "X");
    }
    QString nm;
    if (patient.getTitle() != "")
        nm = patient.getName() + " " + patient.getTitle();
    else
        nm = patient.getName();
    doc->replace("P.Nam1", nm + ", " + patient.getForename());
    doc->replace("P.StrNr", patient.getStreet());
    doc->replace("P.POrt", patient.getZipcode() + " " + patient.getCity());

    doc->replace("P.Vers", patient.getInsurance());

    QList<cTableItem*> items;
    cSurgery surg = db.getSurgery();
    items.append(&surg);
    items.append(&patient);
    cPhys phys;
    if (patient.getDefaultPhysicianID() > 0) {
        if (db.retrieveItem(phys, patient.getDefaultPhysicianID())) {
            doc->replace("P.HA", phys.getProperty("Nam"));
        }
    }
    QString rdata;
    QList<cReceipe *> receipes;
    if (db.loadReceipesForPatient(patient.getID(), receipes)) {
        for (int i = 0; i < receipes.count(); i++) {
            cReceipe *r = receipes.at(i);
            rdata = rdata + "Rezept vom " + r->getDate().toString("dd.MM.yyyy") + ":\n";
            rdata += QString::fromUtf8("\tIndikationsschlüssel ");
            int ind_id = r->getIndID();
            if (ind_id >= 0 && ind_id < db.listIndicators.count()) rdata += db.listIndicators.at(ind_id)->getDescription() + "\n";
            else rdata += "--\n";
            if (!r->getDiagnosis().isEmpty()) rdata += "\tDiagnose: " + r->getDiagnosis() + "\n";
            QString s = r->getProperty("RDat");
            if (!s.isEmpty()) {
                QStringList l;
                l = s.split("\n");
                for (int j = 0; j < l.count(); j++)
                    rdata = rdata + "\t" + l.at(j) + "\n";
            }
            if (!r->getNotes().isEmpty()) rdata += "\t" + r->getNotes() + "\n";
            rdata += "\n";
        }
    }
    while (!receipes.isEmpty()) delete receipes.takeFirst();
    doc->replace("P.RDat", rdata);

    DlgDocViewer viewer;
    viewer.setChangeTrackingEnabled(false);
    viewer.setSavingEnabled(false);
    fillTemplate(items, doc);
    viewer.addDoc(doc);
    execViewer(viewer, QString::fromUtf8("Patientendaten"));
    delete doc;
    delete tmpl;
    return true;
}


bool GenDoc::genPatientSchedule(cReceipe& receipe, cPatient &patient, QList<cDateItem*> listDates)
{
    cTemplate ct = getTemplate(receipe, cTemplateType::TT_DATES);
    if (ct.getID() < 0) return false;

    int co = listDates.count();
    int ca = 0;
    int cn = 0;
    int i = 0;
    QDateTime *a = new QDateTime( QDateTime::currentDateTime() );
    for (int i = 0; i < co; i++) {
        cDateItem *cdi = listDates.at(i);
        QDate d = cdi->getDate();
        if (d.year() == a->date().year()) ca++;
        if (d.year() == a->date().year()+1) cn++;
    }
    EmployeeTools et;

    if (ca > 0) {
        DlgDocViewer viewer;
        cDocTemplate *tmpl = new cDocTemplate();
        tmpl->curTemplate = ct;
        cDoc *doc = new cDoc(tmpl);
        tmpl->setDoc(doc);
        tmpl->readTemplateFromByteArray(ct.getContents());

        QList<cTableItem*> items;
        cSurgery surg = db.getSurgery();
        items.append(&surg);
        items.append(&receipe);
        items.append(&patient);

        for (i = 0; i < ca; i++) {
            cDateItem *cdi = listDates.at(i);
            QDate d = cdi->getDate();
            QString s = QLocale().dayName(d.dayOfWeek(), QLocale::ShortFormat) + ", " + d.toString("dd.MM.yyyy");
            s += "  " + cdi->getStart().toString("hh:mm");
            doc->replace("TR.T" + QString::number(i + 1), s);
            doc->replace("TR.M" + QString::number(i + 1), et.getEmplName(cdi->getEID(), true));
        }
        viewer.setChangeTrackingEnabled(false);
        viewer.setSavingEnabled(false);
        fillTemplate(items, doc);
        viewer.addDoc(doc);
        execViewer(viewer, QString::fromUtf8("Terminzettel"));
        delete doc;
        delete tmpl;
    }
    if (cn > 0) {
        DlgDocViewer viewer;
        cDocTemplate *tmpl = new cDocTemplate();
        tmpl->curTemplate = ct;
        cDoc *doc = new cDoc(tmpl);
        tmpl->setDoc(doc);
        tmpl->readTemplateFromByteArray(ct.getContents());

        QList<cTableItem*> items;
        cSurgery surg = db.getSurgery();
        items.append(&surg);
        items.append(&receipe);
        items.append(&patient);

        for (i = ca; i < co; i++) {
            cDateItem *cdi = listDates.at(i);
            QDate d = cdi->getDate();
            QString s = QLocale().dayName(d.dayOfWeek(), QLocale::ShortFormat) + ", " + d.toString("dd.MM.yyyy");
            s += "  " + cdi->getStart().toString("hh:mm");
            doc->replace("TR.T" + QString::number(i - ca + 1), s);
            doc->replace("TR.M" + QString::number(i - ca + 1), et.getEmplName(cdi->getEID(), true));
        }
        viewer.setChangeTrackingEnabled(false);
        viewer.setSavingEnabled(false);
        fillTemplate(items, doc);
        viewer.addDoc(doc);
        execViewer(viewer, QString::fromUtf8("Terminzettel"));
        delete doc;
        delete tmpl;
    }
    return true;
}


bool GenDoc::genEmptyTemplate(int tmpl_type, QString title)
{
    cTableItem dummy(myDB::TABLE_DUMMY);

    cTemplate ct = getTemplate(dummy, tmpl_type);
    if (ct.getID() < 0) return false;

    cDocTemplate *tmpl = new cDocTemplate();
    tmpl->curTemplate = ct;
    cDoc *doc = new cDoc(tmpl);
    tmpl->setDoc(doc);
    tmpl->readTemplateFromByteArray(ct.getContents());

    doc->clearDataToken();
    doc->finish();

    DlgDocViewer* viewer = new DlgDocViewer();
    viewer->setChangeTrackingEnabled(false);
    viewer->setSavingEnabled(false);
    viewer->addDoc(doc);
    execViewer(*viewer, title);
    delete viewer;
    delete doc;
    delete tmpl;

    return true;
}

QImage GenDoc::generateQrCode(const QString &data, int size = 300) {
    // String in UTF-8 umwandeln
    QByteArray utf8 = data.toUtf8();
    QRcode *qr = QRcode_encodeString(utf8.constData(), 1, QR_ECLEVEL_Q, QR_MODE_8, 1);

    if (!qr) {
        return QImage(); // Fehler
    }

    int qrSize = qr->width > 0 ? qr->width : 1;
    QImage image(qrSize, qrSize, QImage::Format_RGB32);
    image.fill(Qt::white);

    // QR-Matrix in Bild übertragen
    for (int y = 0; y < qrSize; ++y) {
        for (int x = 0; x < qrSize; ++x) {
            unsigned char b = qr->data[y * qrSize + x];
            if (b & 0x01) {
                image.setPixel(x, y, qRgb(0, 0, 0));
            }
        }
    }

    QRcode_free(qr);

    // Auf Zielgröße skalieren
    return image.scaled(size, size, Qt::KeepAspectRatio, Qt::FastTransformation);
}

