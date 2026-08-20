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

#ifndef GENDOC_H_
#define GENDOC_H_

#include <QObject>
#include "mydb.h"
#include "dlgdocviewer.h"
#include "config.h"


class GenDoc: public QObject
{
    Q_OBJECT

public:
    GenDoc();
    ~GenDoc();

    QString generateNumbering(int nr_type);
    QString generateNextNumbering(QString s);

    bool genInvoice(QList<cInvoice*>& invoices, int id);
    bool genPraxisInvoice(QList<cPraxisInvoice*>& invoices);
    bool genPraxisAppendix(QList<cPraxisInvoice*>& invoices);
    bool genReport(cReport& rep);
    bool genQuittance(cQuittance& quittance, QString amount = "");
    bool genReminder(cReminder& reminder);
    bool genParticipanceConfirmation(cCourse& course, QList<cPatient *> &patients);
    bool genPartList(cCourse& course, QList<cPatient*> &patients, QString strLead = "");
    bool genMusterX(int profession, cReceipe& receipe, cPriceTable &cpt);
    bool genCorrection();
    bool genSchedule(cEmployee& empl, int mode, QDate start_date, QDate end_date, QTime start_time, QTime end_time);
    bool genPatientData(cPatient& patient);
    bool genPatientSchedule(cReceipe& receipe, cPatient &patient, QList<cDateItem*> listDates);
    bool genEmptyTemplate(int tmpl_type, QString title);

    int getFId();


private:
    int getPropertyDomainID(QString prop_dom);
    QString getProperty(QList<cTableItem*>&items, QString strProp);
    void fillTemplate(QList<cTableItem*>&items, cDoc *doc);
    int execViewer(DlgDocViewer& viewer, QString title);
    int chooseTemplate(int tmpl_type);
    cTemplate getTemplate(cTableItem& item, int tmpl_type, QString name);
    cTemplate getTemplate(cTableItem& item, int tmpl_type);
    QByteArray prepareXml(QMap<QString, QString>& map, QString startElement);
    int getAuthor(int def_id);
    void genPraxisAppendixNew(QList<cDoc *> &docs, cPraxisInvoice *inv);
    QImage generateQrCode(const QString& data, int size);
};

extern GenDoc gendoc;

#endif /*GENDOC_H_*/
