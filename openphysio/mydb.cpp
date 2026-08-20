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

#include "mydb.h"
#include <iostream>
#include "config.h"
#ifndef OPA
#include "browse.h"
#endif
#include <QtSql>
#include <QSslConfiguration>
#include <QSslCertificate>
#include <QSslKey>
#include <QSslSocket>
#include <QDate>

myDB db;


//****************************************************************************
// cTable
//****************************************************************************
cTable::cTable()
{
}

cTable::~cTable()
{
}

void cTable::init(QString s)
{
    tabname = s;
}

int cTable::tabNumEntries()
{
    return listEntries.count();
}

QString cTable::tabName()
{
    return tabname;
}

QStringList cTable::tabEntries()
{
    return listEntries;
}


QStringList& cTable::operator<< (const QString & str)
{
    listEntries.append(str);
    return listEntries;
}

void cTable::setTableProperties(QStringList *qslHeaders, QStringList *qslTables,
            QStringList *qslItems, QStringList *qslConditions)
{
    setHeaders(qslHeaders);
    setQueryTables(qslTables);
    setQueryItems(qslItems);
    setQueryConditions(qslConditions);
}

void cTable::setHeaders(QStringList *qsl)
{
    if (qsl != NULL) listHeaders = *qsl;
}

void cTable::setQueryTables(QStringList *qsl)
{
    if (qsl != NULL) listQueryTables = *qsl;
}

void cTable::setQueryItems(QStringList *qsl)
{
    if (qsl != NULL) listQueryItems = *qsl;
}

void cTable::setQueryConditions(QStringList *qsl)
{
    if (qsl != NULL) listQueryConditions = *qsl;
}

QStringList& cTable::getHeaders()
{
    return listHeaders;
}

QStringList& cTable::getQueryTables()
{
    return listQueryTables;
}

QStringList& cTable::getQueryItems()
{
    return listQueryItems;
}

QStringList& cTable::getQueryConditions()
{
    return listQueryConditions;
}


//****************************************************************************
// cTableItem
//****************************************************************************
cTableItem::cTableItem(int t)
{
    table = t;
    id = -1;
    flag = FLAG_NEW;
}

cTableItem::~cTableItem()
{
}

void cTableItem::reset()
{
    id = -1;
    flag = FLAG_NEW;
}

int cTableItem::getID()
{
    return id;
}

void cTableItem::setID(int i)
{
    id = i;
}

int cTableItem::getTableID()
{
    return table;
}

void cTableItem::setFlag(int i)
{
    flag = i;
}

int cTableItem::getFlag()
{
    return flag;
}

void cTableItem::retrieve(QSqlQuery& query)
{
    flag = FLAG_ORG;
}

void cTableItem::bind4store(QSqlQuery& query)
{
}

QString cTableItem::getProperty(QString strProp)
{
    return "";
}

//****************************************************************************
// cSurgery
//****************************************************************************
cSurgery::cSurgery() : cTableItem(myDB::TABLE_SURGERY) { this->reset(); }
cSurgery::~cSurgery() {}

void cSurgery::reset ()
{
    name1 = name2 = street = city = zip = "";
    tel = mobile = fax = email = website = "";
    password = name = server = "";
    ik_physio = ik_ergo = ik_logo = "";
    taxid = sales_taxid = "";
    group = area = -1;
    s_union = "0";
    bank = bankcode = account = "";
    tax_liability = false;
    tax_percent = 19;
    cTableItem::reset();
}

cSurgery& cSurgery::operator=(cSurgery& other)
{
    if (this != &other) {
	setID(other.getID());
	name1 = other.getName1();
	name2 = other.getName2();
	street = other.getStreet();
	city = other.getCity();
	zip = other.getZip();
	tel = other.getTel();
	mobile = other.getMobile();
    fax = other.getFax();
    email = other.getEMail();
    website = other.getWebsite();
    server = other.getServer();
    name = other.getName();
    password = other.getPassword();
     ik_physio = other.getPhysioIK();
        ik_ergo = other.getErgoIK();
        ik_logo = other.getLogoIK();
        group = other.getProfession();
	area = other.getArea();
	bank = other.getBank();
	bankcode = other.getBankCode();
	account = other.getAccount();
        taxid = other.getTaxID();
        sales_taxid = other.getSalesTaxID();
        tax_liability = other.getTaxLiability();
        tax_percent = other.getTaxPercent();
    }
    return *this;
}

void cSurgery::setName1(QString s) { name1 = s; }
QString cSurgery::getName1() { return name1; }
void cSurgery::setName2(QString s) { name2 = s; }
QString cSurgery::getName2() { return name2; }
void cSurgery::setStreet(QString s) { street = s; }
QString cSurgery::getStreet() { return street; }
void cSurgery::setCity(QString s) { city = s; }
QString cSurgery::getCity() { return city; }
void cSurgery::setZip(QString s) { zip = s; }
QString cSurgery::getZip() { return zip; }
void cSurgery::setTel(QString s) { tel = s; }
QString cSurgery::getTel() { return tel; }
void cSurgery::setMobile(QString s) { mobile = s; }
QString cSurgery::getMobile() { return mobile; }
void cSurgery::setFax(QString s) { fax = s; }
QString cSurgery::getFax() { return fax; }
void cSurgery::setEMail(QString s) { email = s; }
QString cSurgery::getEMail() { return email; }
void cSurgery::setWebsite(QString s) { website = s; }
QString cSurgery::getWebsite() { return website; }
void cSurgery::setServer(QString s) { server = s; }
QString cSurgery::getServer() { return server; }
void cSurgery::setName(QString s) { name = s; }
QString cSurgery::getName() { return name; }
void cSurgery::setPassword(QString s) { password = s; }
QString cSurgery::getPassword() { return password; }
void cSurgery::setPhysioIK(QString s) { ik_physio = s; }
QString cSurgery::getPhysioIK() { return ik_physio; }
void cSurgery::setErgoIK(QString s) { ik_ergo = s; }
QString cSurgery::getErgoIK() { return ik_ergo; }
void cSurgery::setLogoIK(QString s) { ik_logo = s; }
QString cSurgery::getLogoIK() { return ik_logo; }
void cSurgery::setProfession(int i) { group = i; }
int cSurgery::getProfession() { return group; }
void cSurgery::setArea(int i) { area = i; }
int cSurgery::getArea() { return area; }
void cSurgery::setBank(QString s) { bank = s; }
QString cSurgery::getBank() { return bank; }
void cSurgery::setBankCode(QString s) { bankcode = s; }
QString cSurgery::getBankCode() { return bankcode; }
void cSurgery::setAccount(QString s) { account = s; }
QString cSurgery::getAccount() { return account; }
void cSurgery::setUnion(QString s) { s_union = s; }
QString cSurgery::getUnion() { return s_union; }
void cSurgery::setTaxID(QString s) { taxid = s; }
QString cSurgery::getTaxID() { return taxid; }
void cSurgery::setSalesTaxID(QString s) { sales_taxid = s; }
QString cSurgery::getSalesTaxID() { return sales_taxid; }
void cSurgery::setTradeRegister(QString s) { hrb = s; }
QString cSurgery::getTradeRegister() { return hrb; }
void cSurgery::setTaxLiability(bool b) { tax_liability = b; }
bool cSurgery::getTaxLiability() { return tax_liability; }
void cSurgery::setTaxPercent(int i) { tax_percent = i; }
int cSurgery::getTaxPercent() { return tax_percent; }

void cSurgery::retrieve(QSqlQuery& query)
{
    setID(query.value(0).toInt());
    setName1(query.value(1).toString());
    setName2(query.value(2).toString());
    setStreet(query.value(3).toString());
    setCity(query.value(4).toString());
    setZip(query.value(5).toString());
    setTel(query.value(6).toString());
    setMobile(query.value(7).toString());
    setFax(query.value(8).toString());
    setEMail(query.value(9).toString());
    setWebsite(query.value(10).toString());
    setServer(query.value(11).toString());
    setName(query.value(12).toString());
    setPassword(query.value(13).toString());

    setProfession(query.value(14).toInt());
    setArea(query.value(15).toInt());
    setPhysioIK(query.value(16).toString());
    setErgoIK(query.value(17).toString());
    setLogoIK(query.value(18).toString());
    setBank(query.value(19).toString());
    setBankCode(query.value(20).toString());
    setAccount(query.value(21).toString());
    setUnion(query.value(22).toString());
    setTaxID(query.value(23).toString());
    setSalesTaxID(query.value(24).toString());
    setTradeRegister(query.value(25).toString());
    setTaxLiability(query.value(26).toBool());
    setTaxPercent(query.value(27).toInt());
}

void cSurgery::bind4store(QSqlQuery& query)
{
    query.addBindValue(getName1());
    query.addBindValue(getName2());
    query.addBindValue(getStreet());
    query.addBindValue(getCity());
    query.addBindValue(getZip());
    query.addBindValue(getTel());
    query.addBindValue(getMobile());
    query.addBindValue(getFax());
    query.addBindValue(getEMail());
    query.addBindValue(getWebsite());
    query.addBindValue(getServer());
    query.addBindValue(getName());
    query.addBindValue(getPassword());
    query.addBindValue(getProfession());
    query.addBindValue(getArea());
    query.addBindValue(getPhysioIK());
    query.addBindValue(getErgoIK());
    query.addBindValue(getLogoIK());
    query.addBindValue(getBank());
    query.addBindValue(getBankCode());
    query.addBindValue(getAccount());
    query.addBindValue(getUnionID(getProfession()));
    query.addBindValue(getTaxID());
    query.addBindValue(getSalesTaxID());
    query.addBindValue(getTradeRegister());
    query.addBindValue(getTaxLiability());
    query.addBindValue(getTaxPercent());
}

QString cSurgery::getProperty(QString strProp)
{
    if (strProp.isEmpty()) return "";
    QString s = "";
    if (strProp == "Adr") {
        s = getName1() + "\n";
        if (!getName2().isEmpty()) s = s + getName2() + "\n";
        if (!getStreet().isEmpty()) s = s + getStreet() + "\n";
        s = s + getZip() + " " + getCity() + "\n";
        if (!getTel().isEmpty()) s = s + "Tel: " + getTel() + "\n";
        if (!getFax().isEmpty()) s = s + "Fax: " + getFax();

        return s;
    } else if (strProp == "Bank") {
        s = getName1();
        if (getName2().isEmpty()) s += "\n";
        else s = s + " - " + getName2() + "\n";
        s = s + getStreet() + ", " + getZip() + " " + getCity() + "\n";
        s = s + "Bankverbindung: " + getBank() + " - IBAN: " + getAccount() + " - BIC: " + getBankCode();

        QString tax = "";
        if (!getTaxID().isEmpty()) tax = "\nSteuernr. " + getTaxID();

        if (!getSalesTaxID().isEmpty()) {
            if (tax.isEmpty()) tax = "\n";
            else tax += "  ";
            tax = tax + "USt-IdNr. " + getSalesTaxID();
        }
        s += tax;
    //} else if (strProp == "CITYDATE") {
    //    s = getCity() + ", den " + QDate::currentDate().toString("dd.MM.yyyy");
    } else if (strProp == "IK") {
        switch(group) {
        case 1: s = getPhysioIK(); break;
        case 2: s = getErgoIK(); break;
        case 4: s = getLogoIK(); break;
        case 6: s = getErgoIK(); break;
        case 3:
        case 5:
        case 7: s = getPhysioIK(); break;
        default: break;
        }
    } else if (strProp == "IKP") {
        s = getPhysioIK();
    } else if (strProp == "IKE") {
        s = getErgoIK();
    } else if (strProp == "IKL") {
        s = getLogoIK();
    } else if (strProp == "Fax") {
        s = getFax();
    } else if (strProp == "Tel") {
        s = getTel();
    } else if (strProp == "Nam") {
        s = getName1();
        if (!getName2().isEmpty()) s += "\n" + getName2();
    } else if (strProp == "Str") {
        s = getStreet();
    }
    return s;
}

int cSurgery::getUnionID(int profession)
{
    if (s_union.isEmpty()) return -1;
    QStringList listUnion = s_union.split(":");
    if (profession >= 0 && profession < listUnion.count()) {
        QString s = listUnion.at(profession);
        return s.toInt();
    }
    return -1;
}

bool cSurgery::isProfession(int profession)
{
    if (profession < 0 || profession >= cProfession::PROF_COUNT) return false;
    return (group >> profession) & 1;
}


//****************************************************************************
// cSurgeryBranch
//****************************************************************************
cSurgeryBranch::cSurgeryBranch() : cTableItem(myDB::TABLE_SURGERY_BRANCHES) { this->reset(); }
cSurgeryBranch::~cSurgeryBranch() {}

void cSurgeryBranch::reset ()
{
    name1 = name2 = street = city = zip = "";
    tel = mobile = fax = ik = "";
    cTableItem::reset();
}

cSurgeryBranch& cSurgeryBranch::operator=(cSurgeryBranch& other)
{
    if (this != &other) {
        setID(other.getID());
        name1 = other.getName1();
        name2 = other.getName2();
        street = other.getStreet();
        city = other.getCity();
        zip = other.getZip();
        tel = other.getTel();
        mobile = other.getMobile();
        fax = other.getFax();
        ik = other.getIK();
    }
    return *this;
}

void cSurgeryBranch::setName1(QString s) { name1 = s; }
QString cSurgeryBranch::getName1() { return name1; }
void cSurgeryBranch::setName2(QString s) { name2 = s; }
QString cSurgeryBranch::getName2() { return name2; }
void cSurgeryBranch::setStreet(QString s) { street = s; }
QString cSurgeryBranch::getStreet() { return street; }
void cSurgeryBranch::setCity(QString s) { city = s; }
QString cSurgeryBranch::getCity() { return city; }
void cSurgeryBranch::setZip(QString s) { zip = s; }
QString cSurgeryBranch::getZip() { return zip; }
void cSurgeryBranch::setTel(QString s) { tel = s; }
QString cSurgeryBranch::getTel() { return tel; }
void cSurgeryBranch::setMobile(QString s) { mobile = s; }
QString cSurgeryBranch::getMobile() { return mobile; }
void cSurgeryBranch::setFax(QString s) { fax = s; }
QString cSurgeryBranch::getFax() { return fax; }
void cSurgeryBranch::setIK(QString s) { ik = s; }
QString cSurgeryBranch::getIK() { return ik; }

void cSurgeryBranch::retrieve(QSqlQuery& query)
{
    setID(query.value(0).toInt());
    setName1(query.value(1).toString());
    setName2(query.value(2).toString());
    setStreet(query.value(3).toString());
    setCity(query.value(4).toString());
    setZip(query.value(5).toString());
    setTel(query.value(6).toString());
    setMobile(query.value(7).toString());
    setFax(query.value(8).toString());
    setIK(query.value(9).toString());
}

void cSurgeryBranch::bind4store(QSqlQuery& query)
{
    query.addBindValue(getName1());
    query.addBindValue(getName2());
    query.addBindValue(getStreet());
    query.addBindValue(getCity());
    query.addBindValue(getZip());
    query.addBindValue(getTel());
    query.addBindValue(getMobile());
    query.addBindValue(getFax());
    query.addBindValue(getIK());
}


//****************************************************************************
// cPraxisInvoice
//****************************************************************************
#ifndef OPA

cPraxisInvoice::cPraxisInvoice() : cTableItem(myDB::TABLE_PINVOICES) { this->reset(); }
cPraxisInvoice::~cPraxisInvoice() {}

void cPraxisInvoice::reset()
{
    rid = pid = cid = xid = -1;
    paystat = 0;
    i_nr = fname = amount = rlist = "";
    cTableItem::reset();
}

cPraxisInvoice& cPraxisInvoice::operator=(cPraxisInvoice& other)
{
    if (this != &other) {
	setID(other.getID());
	rid = other.getReceipeID();
	pid = other.getPatientID();
	cid = other.getCourseID();
        xid = other.getPraxisID();
	type = other.getType();
	amount = other.getAmount();
	paystat = other.getPayStat();
	i_date = other.getDate();
	i_nr = other.getNumber();
	invdata = other.getInvData();
        s_date = other.getSdate();
        e_date = other.getEdate();
        rlist = other.getRlist();
    }
    return *this;
}

int cPraxisInvoice::getReceipeID() { return rid; }
void cPraxisInvoice::setReceipeID(int i) { rid = i; }
int cPraxisInvoice::getPatientID() { return pid; }
void cPraxisInvoice::setPatientID(int i) { pid = i; }
int cPraxisInvoice::getCourseID() { return cid; }
void cPraxisInvoice::setCourseID(int i) { cid = i; }
int cPraxisInvoice::getPraxisID() { return xid; }
void cPraxisInvoice::setPraxisID(int i) { xid = i; }
int cPraxisInvoice::getType() { return type; }
void cPraxisInvoice::setType(int i) { type = i; }
int cPraxisInvoice::getPayStat() { return paystat; }
void cPraxisInvoice::setPayStat(int i) { paystat = i; }
QDate cPraxisInvoice::getDate() { return i_date; }
void cPraxisInvoice::setDate(QDate t) { i_date = t; }
QString cPraxisInvoice::getNumber() { return i_nr; }
void cPraxisInvoice::setNumber(QString s) { i_nr = s; }
QString cPraxisInvoice::getAmount() { return amount; }
void cPraxisInvoice::setAmount(QString s) { amount = s; }
QByteArray& cPraxisInvoice::getInvData() { return invdata; }
void cPraxisInvoice::setInvData(QByteArray& buf) { invdata = buf; }
QDate cPraxisInvoice::getSdate() { return s_date; }
void cPraxisInvoice::setSdate(QDate t) { s_date = t; }
QDate cPraxisInvoice::getEdate() { return e_date; }
void cPraxisInvoice::setEdate(QDate t) { e_date = t; }
QString cPraxisInvoice::getRlist() { return rlist; }
void cPraxisInvoice::setRlist(QString s) { rlist = s; }

QString cPraxisInvoice::getFileName()
{
    QString s;
    s = i_date.toString("yyyy-MM-dd") + "-" + i_nr + ".pdf";
    return s;
}

 void cPraxisInvoice::retrieve(QSqlQuery& query)
 {
    setID(query.value(0).toInt());
    setDate(QDate::fromString(query.value(1).toString(), "yyyy-MM-dd"));
    setNumber(query.value(2).toString());
    setAmount(query.value(3).toString());
    setPatientID(query.value(4).toInt());
    setReceipeID(query.value(5).toInt());
    setCourseID(query.value(6).toInt());
    setPraxisID(query.value(7).toInt());
    setType(query.value(8).toInt());
    setPayStat(query.value(9).toInt());
    QByteArray qba = query.value(10).toByteArray();
    setInvData(qba);
    setSdate(QDate::fromString(query.value(11).toString(), "yyyy-MM-dd"));
    setEdate(QDate::fromString(query.value(12).toString(), "yyyy-MM-dd"));
    setRlist(query.value(13).toString());
}

void cPraxisInvoice::bind4store(QSqlQuery& query)
{
    query.addBindValue(getDate().toString("yyyy-MM-dd"));
    query.addBindValue(getNumber());
    query.addBindValue(getAmount());
    query.addBindValue(getPatientID());
    query.addBindValue(getReceipeID());
    query.addBindValue(getCourseID());
    query.addBindValue(getPraxisID());
    query.addBindValue(getType());
    query.addBindValue(getPayStat());
    query.addBindValue(getInvData());
    query.addBindValue(getSdate().toString("yyyy-MM-dd"));
    query.addBindValue(getEdate().toString("yyyy-MM-dd"));
    query.addBindValue(getRlist());
}

QString cPraxisInvoice::getProperty(QString strProp)
{
    QString s = "";

    QXmlStreamReader xml(getInvData());
    QString look_for = "";
    if (strProp == "DATE") look_for = "date";
    else if (strProp == "ADDR") look_for = "addr";
    else if (strProp == "FORM")look_for ="form";
    else if (strProp == "SDATE")look_for ="sdate";
    else if (strProp == "EDATE")look_for ="edate";
    else if (strProp == "MPART")look_for ="mpart";
    else if (strProp == "SAMOUNT")look_for ="samount";
    else if (strProp == "AMOUNT")look_for ="amount";
    else if (strProp == "NAME")look_for ="name";
    else if (strProp == "NR")look_for ="nr";

    if (!look_for.isEmpty()) {
        while (!xml.atEnd()) {
            xml.readNext();
            if (xml.isStartElement()) {
                if (xml.name() == look_for) {
                    s = xml.readElementText();
                    break;
                }
            }
        }
    } else {
        if (strProp == "Nr") {
            s = getNumber();
        } else if (strProp == "Sum") {
            s = getAmount() + " EUR";
        } else if (strProp == "Dat") s = getDate().toString("dd.MM.yyyy");
    }

    return s;
}

#endif // OPA


//****************************************************************************
// cInvoice
//****************************************************************************
#ifndef OPA

cInvoice::cInvoice() : cTableItem(myDB::TABLE_INVOICES) { this->reset(); }
cInvoice::~cInvoice() {}

void cInvoice::reset()
{
    rid = pid = cid = xid = -1;
    paystat = 0;
    i_nr = fname = amount = rlist = "";
    cTableItem::reset();
}

cInvoice& cInvoice::operator=(cInvoice& other)
{
    if (this != &other) {
        setID(other.getID());
        rid = other.getReceipeID();
        pid = other.getPatientID();
        cid = other.getCourseID();
        xid = other.getPraxisID();
        type = other.getType();
        amount = other.getAmount();
        paystat = other.getPayStat();
        i_date = other.getDate();
        i_nr = other.getNumber();
        invdata = other.getInvData();
        s_date = other.getSdate();
        e_date = other.getEdate();
        maildate = other.getMailDate();
        rlist = other.getRlist();
    }
    return *this;
}

int cInvoice::getReceipeID() { return rid; }
void cInvoice::setReceipeID(int i) { rid = i; }
int cInvoice::getPatientID() { return pid; }
void cInvoice::setPatientID(int i) { pid = i; }
int cInvoice::getCourseID() { return cid; }
void cInvoice::setCourseID(int i) { cid = i; }
int cInvoice::getPraxisID() { return xid; }
void cInvoice::setPraxisID(int i) { xid = i; }
int cInvoice::getType() { return type; }
void cInvoice::setType(int i) { type = i; }
int cInvoice::getPayStat() { return paystat; }
void cInvoice::setPayStat(int i) { paystat = i; }
QDate cInvoice::getDate() { return i_date; }
void cInvoice::setDate(QDate t) { i_date = t; }
QString cInvoice::getNumber() { return i_nr; }
void cInvoice::setNumber(QString s) { i_nr = s; }
QString cInvoice::getAmount() { return amount; }
void cInvoice::setAmount(QString s) { amount = s; }
QByteArray& cInvoice::getInvData() { return invdata; }
void cInvoice::setInvData(QByteArray& buf) { invdata = buf; }
QDate cInvoice::getSdate() { return s_date; }
void cInvoice::setSdate(QDate t) { s_date = t; }
QDate cInvoice::getEdate() { return e_date; }
void cInvoice::setEdate(QDate t) { e_date = t; }
QDate cInvoice::getMailDate() { return maildate; }
void cInvoice::setMailDate(QDate t) { maildate = t; }
QString cInvoice::getRlist() { return rlist; }
void cInvoice::setRlist(QString s) { rlist = s; }

QString cInvoice::getFileName()
{
    QString s;
    s = i_date.toString("yyyy-MM-dd") + "-" + i_nr + ".pdf";
    return s;
}

 void cInvoice::retrieve(QSqlQuery& query)
 {
    setID(query.value(0).toInt());
    setDate(QDate::fromString(query.value(1).toString(), "yyyy-MM-dd"));
    setNumber(query.value(2).toString());
    setAmount(query.value(3).toString());
    setPatientID(query.value(4).toInt());
    setReceipeID(query.value(5).toInt());
    setCourseID(query.value(6).toInt());
    setPraxisID(query.value(7).toInt());
    setType(query.value(8).toInt());
    setPayStat(query.value(9).toInt());
    QByteArray qba = query.value(10).toByteArray();
    setInvData(qba);
    setSdate(QDate::fromString(query.value(11).toString(), "yyyy-MM-dd"));
    setEdate(QDate::fromString(query.value(12).toString(), "yyyy-MM-dd"));
    setMailDate(QDate::fromString(query.value(13).toString(), "yyyy-MM-dd"));
    setRlist(query.value(14).toString());
}

void cInvoice::bind4store(QSqlQuery& query)
{
    query.addBindValue(getDate().toString("yyyy-MM-dd"));
    query.addBindValue(getNumber());
    query.addBindValue(getAmount());
    query.addBindValue(getPatientID());
    query.addBindValue(getReceipeID());
    query.addBindValue(getCourseID());
    query.addBindValue(getPraxisID());
    query.addBindValue(getType());
    query.addBindValue(getPayStat());
    query.addBindValue(getInvData());
    query.addBindValue(getSdate().toString("yyyy-MM-dd"));
    query.addBindValue(getEdate().toString("yyyy-MM-dd"));
    query.addBindValue(getMailDate().toString("yyyy-MM-dd"));
    query.addBindValue(getRlist());
}

QString cInvoice::getProperty(QString strProp)
{
    QString s = "";

    QXmlStreamReader xml(getInvData());
    QString look_for = "";
    if (strProp == "Ter") look_for = "dates";
    else if (strProp == "POSITIONS") look_for = "pos";
    else if (strProp == "Adr") look_for = "addr";
    else if (strProp == "Anr") look_for = "salut";
    else if (strProp == "TPL") look_for = "tpl";
    else if (strProp == "Diag") look_for = "diagnostics";
    if (!look_for.isEmpty()) {
        while (!xml.atEnd()) {
            xml.readNext();
            if (xml.isStartElement()) {
                if (xml.name() == look_for) {
                    s = xml.readElementText();
                    break;
                }
            }
        }
    } else {
        if (strProp == "Nr") {
            s = getNumber();
        } else if (strProp == "Sum") {
            s = getAmount() + " EUR";
        } else if (strProp == "Dat") s = getDate().toString("dd.MM.yyyy");
    }

    return s;
}

#endif // OPA


//****************************************************************************
// cPatient
//****************************************************************************

cPatient::cPatient() : cTableItem(myDB::TABLE_PATIENTS)
{
    this->reset();
}

cPatient::~cPatient()
{
}

void cPatient::reset()
{
    gender = group = default_employee = def_phys = -1;
    ins_type = 0;
    care_facility = distance = 0;
    state = STATE_ACTIVE;
    surname = forename = addname = title = street = city = zip = "";
    tel = tel_job = tel_mobile = fax = email = insurance = "";
    vnr = kvknr = vknr = status = "";
    notices = "";
    addpay_freed = 0;
    cTableItem::reset();
}

cPatient& cPatient::operator=(cPatient& other)
{
    if (this != &other) {
        setID(other.getID());
        surname = other.getName();
        forename = other.getForename();
        addname = other.getAddName();
        title = other.getTitle();
        birthday = other.getBirthday();
        gender = other.getGender();
        street = other.getStreet();
        city = other.getCity();
        zip = other.getZipcode();
        tel = other.getTelephone();
        tel_job = other.getTelephoneJob();
        tel_mobile = other.getTelephoneMobile();
        fax = other.getFax();
        email = other.getEMail();
        insurance = other.getInsurance();
        vnr = other.getVNR();
        kvknr = other.getKVK();
        kvk_valid = other.getValidKVK();
        vknr = other.getVKNR();
        group = other.getGroup();
        ins_type = other.getInsType();
        notices = other.getNotices();
        status = other.getStatus();
        default_employee = other.getDefaultEmployeeID();
        def_phys = other.getDefaultPhysicianID();
        distance = other.getDistance();
        care_facility = other.getCareFacility();
        state = other.getState();
        addpay_freed = other.getAddpayFreed();
        freed_from = other.getFreedFrom();
        freed_until = other.getFreedUntil();
        date_contract = other.getContract();
        date_plan = other.getPlan();
    }
    return *this;
}

void cPatient::setName(QString qs)
{
    surname = qs;
}

void cPatient::setForename(QString qs)
{
    forename = qs;
}

void cPatient::setAddName(QString qs)
{
    addname = qs;
}

void cPatient::setTitle(QString qs)
{
    title = qs;
}

void cPatient::setBirthday(QDate qd)
{
    birthday = qd;
}

void cPatient::setGender(int i)
{
    gender = i;
}

void cPatient::setStreet(QString qs)
{
    street = qs;
}

void cPatient::setCity(QString qs)
{
    city = qs;
}

void cPatient::setZipcode(QString qs)
{
    zip = qs;
}

void cPatient::setTelephone(QString qs)
{
    tel = qs;
}

void cPatient::setTelephoneJob(QString qs)
{
    tel_job = qs;
}

void cPatient::setTelephoneMobile(QString qs)
{
    tel_mobile = qs;
}

void cPatient::setFax(QString qs)
{
    fax = qs;
}

void cPatient::setEMail(QString qs)
{
    email = qs;
}

void cPatient::setInsurance(QString qs)
{
    insurance = qs;
}

void cPatient::setVNR(QString qs)
{
    vnr = qs;
}

void cPatient::setKVK(QString qs)
{
    kvknr = qs;
}

void cPatient::setValidKVK(QDate qd)
{
    kvk_valid = qd;
}

void cPatient::setVKNR(QString qs)
{
    vknr = qs;
}

void cPatient::setGroup(int i)
{
    group = i;
}

void cPatient::setInsType(int i)
{
    ins_type = i;
}

void cPatient::setNotices(QString qs)
{
    notices = qs;
}

void cPatient::setStatus(QString qs)
{
    status = qs;
}

void cPatient::setCareFacility(int i)
{
    care_facility = i;
}

void cPatient::setDefaultEmployeeID(int i)
{
    default_employee = i;
}

void cPatient::setDefaultPhysicianID(int i)
{
    def_phys = i;
}

void cPatient::setDistance(int i)
{
    distance = i;
}

void cPatient::setState(int i)
{
    state = i;
}

void cPatient::setAddpayFreed(int i)
{
    addpay_freed = i;
}

void cPatient::setFreedFrom(QDate d)
{
    freed_from = d;
}

void cPatient::setFreedUntil(QDate d)
{
    freed_until = d;
}

void cPatient::setContract(QDate d)
{
    date_contract = d;
}

void cPatient::setPlan(QDate d)
{
    date_plan = d;
}

QString cPatient::getName()
{
    return surname;
}

QString cPatient::getForename()
{
    return forename;
}

QString cPatient::getAddName()
{
    return addname;
}

QString cPatient::getTitle()
{
    return title;
}

QDate cPatient::getBirthday()
{
    return birthday;
}

int cPatient::getGender()
{
    return gender;
}

QString cPatient::getStreet()
{
    return street;
}

QString cPatient::getCity()
{
    return city;
}

QString cPatient::getZipcode()
{
    return zip;
}

QString cPatient::getTelephone()
{
    return tel;
}

QString cPatient::getTelephoneJob()
{
    return tel_job;
}

QString cPatient::getTelephoneMobile()
{
    return tel_mobile;
}

QString cPatient::getFax()
{
    return fax;
}

QString cPatient::getEMail()
{
    return email;
}

QString cPatient::getInsurance()
{
    return insurance;
}

QString cPatient::getVNR()
{
    return vnr;
}

QString cPatient::getKVK()
{
    return kvknr;
}

QDate cPatient::getValidKVK()
{
    return kvk_valid;
}

QString cPatient::getVKNR()
{
    return vknr;
}

int cPatient::getGroup()
{
    return group;
}

int cPatient::getInsType()
{
    return ins_type;
}

QString cPatient::getNotices()
{
    return notices;
}

QString cPatient::getStatus()
{
    return status;
}

int cPatient::getCareFacility()
{
    return care_facility;
}

int cPatient::getDefaultEmployeeID()
{
    return default_employee;
}

int cPatient::getDefaultPhysicianID()
{
    return def_phys;
}

int cPatient::getDistance()
{
    return distance;
}

int cPatient::getState()
{
    return state;
}

int cPatient::getAddpayFreed()
{
    return addpay_freed;
}

QDate cPatient::getFreedFrom()
{
    return freed_from;
}

QDate cPatient::getFreedUntil()
{
    return freed_until;
}

QDate cPatient::getContract()
{
    return date_contract;
}

QDate cPatient::getPlan()
{
    return date_plan;
}

QString cPatient::getFullName()
{
    return getForename() + " " + getName();
}

void cPatient::retrieve(QSqlQuery& query)
{
    setID(query.value(0).toInt());
    setName(query.value(1).toString());
    setForename(query.value(2).toString());
    setAddName(query.value(3).toString());
    setTitle(query.value(4).toString());
    setBirthday(QDate::fromString(query.value(5).toString(), "yyyy-MM-dd"));
    setGender(query.value(6).toInt());
    setStreet(query.value(7).toString());
    setCity(query.value(8).toString());
    setZipcode(query.value(9).toString());
    setTelephone(query.value(10).toString());
    setTelephoneJob(query.value(11).toString());
    setTelephoneMobile(query.value(12).toString());
    setFax(query.value(13).toString());
    setEMail(query.value(14).toString());
    setInsurance(query.value(15).toString());
    setVNR(query.value(16).toString());
    setStatus(query.value(17).toString());
    setKVK(query.value(18).toString());
    setValidKVK(QDate::fromString(query.value(19).toString(), "yyyy-MM-dd"));
    setNotices(query.value(20).toString());
    setInsType(query.value(21).toInt());
    setGroup(query.value(22).toInt());
    setCareFacility(query.value(23).toInt());
    setDistance(query.value(24).toInt());
    setDefaultEmployeeID(query.value(25).toInt());
    setDefaultPhysicianID(query.value(26).toInt());
    setState(query.value(27).toInt());
    setAddpayFreed(query.value(28).toInt());
    setFreedFrom(QDate::fromString(query.value(29).toString(), "yyyy-MM-dd"));
    setFreedUntil(QDate::fromString(query.value(30).toString(), "yyyy-MM-dd"));
    setContract(QDate::fromString(query.value(31).toString(), "yyyy-MM-dd"));
    setPlan(QDate::fromString(query.value(32).toString(), "yyyy-MM-dd"));
}

void cPatient::bind4store(QSqlQuery& query)
{
    query.addBindValue(getName());
    query.addBindValue(getForename());
    query.addBindValue(getAddName());
    query.addBindValue(getTitle());
    query.addBindValue(getBirthday().toString("yyyy-MM-dd"));
    query.addBindValue(getGender());
    query.addBindValue(getStreet());
    query.addBindValue(getCity());
    query.addBindValue(getZipcode());
    query.addBindValue(getTelephone());
    query.addBindValue(getTelephoneJob());
    query.addBindValue(getTelephoneMobile());
    query.addBindValue(getFax());
    query.addBindValue(getEMail());
    query.addBindValue(getInsurance());
    query.addBindValue(getVNR());
    query.addBindValue(getStatus());
    query.addBindValue(getKVK());
    query.addBindValue(getValidKVK().toString("yyyy-MM-dd"));
    query.addBindValue(getNotices());
    query.addBindValue(getInsType());
    query.addBindValue(getGroup());
    query.addBindValue(getCareFacility());
    query.addBindValue(getDistance());
    query.addBindValue(getDefaultEmployeeID());
    query.addBindValue(getDefaultPhysicianID());
    query.addBindValue(getState());
    query.addBindValue(getAddpayFreed());
    query.addBindValue(getFreedFrom());
    query.addBindValue(getFreedUntil());
    query.addBindValue(getContract().toString("yyyy-MM-dd"));
    query.addBindValue(getPlan().toString("yyyy-MM-dd"));
}

QString cPatient::getProperty(QString strProp)
{
    QString s = "";

    if (strProp == "Adr") {
        // if patient is younger than 18 years, address the familiy
        if ( getBirthday().daysTo(QDate::currentDate()) > (18 * 365) ) {
            if (getGender() == GENDER_MALE) s = "Herr\n";
            else if (getGender() == GENDER_FEMALE) s = "Frau\n";
            else s = "Frau/Herr\n";
            s = s + getForename() + " " + getName() + "\n";
        } else {
            s = "Familie " + getName() + "\n";
            s = getForename() + " " + getName() + "\n";
        }
        s = s + getStreet() + "\n";
        s = s + getZipcode() + " " + getCity() + "\n";
    } else if (strProp == "AdrK") {
        s = getStreet() + "\n";
        s = s + getZipcode() + " " + getCity() + "\n";
    } else if (strProp == "Anr") {
        if ( getBirthday().daysTo(QDate::currentDate()) > (18 * 365) ) {
            if (getGender() == 0) s = "Sehr geehrter Herr ";
            else if (getGender() == 1) s = "Sehr geehrte Frau ";
            else s = "Sehr geehrte/r Frau/Herr ";
        } else {
            s = "Sehr geehrte Familie ";
        }
        s = s + getName() + ",";
    } else if (strProp == "Nam") {
        s = getForename() + " " + getName();
    } else if (strProp == "Notiz") {
        s = getNotices();
    } else if (strProp == "GDat") {
        s = getBirthday().toString("dd.MM.yyyy");
    } else if (strProp == "TelP") {
        s = getTelephone();
    } else if (strProp == "TelB") {
        s = getTelephoneJob();
    } else if (strProp == "TelM") {
        s = getTelephoneMobile();
    } else if (strProp == "Mail") {
        s = getEMail();
    }

    return s;
}


//****************************************************************************
// cTherapyGroup
//****************************************************************************
cTherapyGroup::cTherapyGroup() : cTableItem(myDB::TABLE_THERAPY_GROUPS) { t_name = t_desc = ""; cTableItem::reset(); }
void cTherapyGroup::cTherapyGroup::setName(QString s) { t_name = s; }
QString cTherapyGroup::getName() { return t_name; }
void cTherapyGroup::cTherapyGroup::setDescription(QString s) { t_desc = s; }
QString cTherapyGroup::getDescription() { return t_desc; }
void cTherapyGroup::retrieve(QSqlQuery& query)
{
    setID(query.value(0).toInt());
    setName(query.value(1).toString());
    setDescription(query.value(2).toString());
}
void cTherapyGroup::bind4store(QSqlQuery& query)
{
    query.addBindValue(getName());
    query.addBindValue(getDescription());
}



//****************************************************************************
// cCashPosition
//****************************************************************************
#ifndef OPA

cCashPosition::cCashPosition() : cTableItem(myDB::TABLE_CASHPOS) { cp_desc = ""; cTableItem::reset(); }
void cCashPosition::setDescription(QString s) { cp_desc = s; }
QString cCashPosition::getDescription() { return cp_desc; }
void cCashPosition::retrieve(QSqlQuery& query)
{
    setID(query.value(0).toInt());
    setDescription(query.value(1).toString());
}
void cCashPosition::bind4store(QSqlQuery& query)
{
    query.addBindValue(getDescription());
}

#endif // OPA


//****************************************************************************
// cCash
//****************************************************************************
#ifndef OPA

cCash::cCash() : cTableItem(myDB::TABLE_CASH) { this->reset(); }
void cCash::reset() {
    cdesc = amount = "";
    pid = rid = cid = xid = -1;
    cTableItem::reset();
}
void cCash::setPositionID(int i) { cpid = i; }
void cCash::setDescription(QString s) { cdesc = s; }
void cCash::setPayDate(QDate d) { paydate = d; }
void cCash::setAmount(QString s) { amount = s; }
void cCash::setPID(int i) { pid = i; }
void cCash::setRID(int i) { rid = i; }
void cCash::setCID(int i) { cid = i; }
void cCash::setXID(int i) { xid = i; }
int cCash::getPositionID() { return cpid; }
QString cCash::getDescription() { return cdesc; }
QDate cCash::getPayDate() { return paydate; }
QString cCash::getAmount() { return amount; }
int cCash::getPID() { return pid; }
int cCash::getRID() { return rid; }
int cCash::getCID() { return cid; }
int cCash::getXID() { return xid; }
void cCash::retrieve(QSqlQuery& query)
{
    setID(query.value(0).toInt());
    setPositionID(query.value(1).toInt());
    setDescription(query.value(2).toString());
    setPayDate(QDate::fromString(query.value(3).toString(), "yyyy-MM-dd"));
    setAmount(query.value(4).toString());
    setPID(query.value(5).toInt());
    setRID(query.value(6).toInt());
    setCID(query.value(7).toInt());
    setXID(query.value(8).toInt());
}
void cCash::bind4store(QSqlQuery& query)
{
    query.addBindValue(getPositionID());
    query.addBindValue(getDescription());
    query.addBindValue(getPayDate().toString("yyyy-MM-dd"));
    query.addBindValue(getAmount());
    query.addBindValue(getPID());
    query.addBindValue(getRID());
    query.addBindValue(getCID());
    query.addBindValue(getXID());
}


#endif // OPA

//****************************************************************************
// cEmployeeGroup
//****************************************************************************
#ifndef OPA

cEmployeeGroup::cEmployeeGroup() : cTableItem(myDB::TABLE_EMPLOYEE_GROUPS) { tdesc = ""; cTableItem::reset(); }
void cEmployeeGroup::cEmployeeGroup::setDescription(QString s) { tdesc = s; }
QString cEmployeeGroup::getDescription() { return tdesc; }
void cEmployeeGroup::retrieve(QSqlQuery& query)
{
    setID(query.value(0).toInt());
    setDescription(query.value(1).toString());
}
void cEmployeeGroup::bind4store(QSqlQuery& query)
{
    query.addBindValue(getDescription());
}

#endif // OPA


//****************************************************************************
// cEmployeeType
//****************************************************************************
#ifndef OPA

cEmployeeType::cEmployeeType() : cTableItem(myDB::TABLE_EMPLOYEE_TYPES) { tdesc = ""; cTableItem::reset(); }
void cEmployeeType::cEmployeeType::setDescription(QString s) { tdesc = s; }
QString cEmployeeType::getDescription() { return tdesc; }
void cEmployeeType::retrieve(QSqlQuery& query)
{
    setID(query.value(0).toInt());
    setDescription(query.value(1).toString());
}
void cEmployeeType::bind4store(QSqlQuery& query)
{
    query.addBindValue(getDescription());
}

#endif // OPA


//****************************************************************************
// cEmployee
//****************************************************************************
#ifndef OPA

cEmployee::cEmployee() : cTableItem(myDB::TABLE_EMPLOYEES)  { this->reset(); }
cEmployee::~cEmployee() {}

void cEmployee::reset()
{
    etype = id_grp = flag = state = 0;
    id_tgrp = gender = -1;
    surname = forename = street = city = zip = tel = tel_mobile = fax = email = shortname = "";
    persnr = "";
    listQuali.clear();
    cTableItem::reset();
}

cEmployee& cEmployee::operator=(cEmployee& other)
{
    if (this != &other) {
	setID(other.getID());
	surname = other.getName();
	forename = other.getForename();
	birthday = other.getBirthday();
	street = other.getStreet();
	city = other.getCity();
	zip = other.getZipcode();
	tel = other.getTelephone();
	tel_mobile = other.getTelephoneMobile();
	fax = other.getFax();
	email = other.getEMail();
        shortname = other.getShortName();
        listQuali = other.getQualifications();
        etype = other.getType();
	id_grp = other.getGroupID();
	id_tgrp = other.getTherapyGroupID();
	flag = other.getFlag();
        gender = other.getGender();
        persnr = other.getPersonellNumber();
        state = other.getState();
    }
    return *this;
}

void cEmployee::setName(QString qs) { surname = qs; }
void cEmployee::setForename(QString qs) { forename = qs; }
void cEmployee::setBirthday(QDate qd) { birthday = qd; }
void cEmployee::setStreet(QString qs) { street = qs; }
void cEmployee::setCity(QString qs) { city = qs; }
void cEmployee::setZipcode(QString qs) { zip = qs; }
void cEmployee::setTelephone(QString qs) { tel = qs; }
void cEmployee::setTelephoneMobile(QString qs) { tel_mobile = qs; }
void cEmployee::setFax(QString qs) { fax = qs; }
void cEmployee::setEMail(QString qs) { email = qs; }
void cEmployee::setShortName(QString qs) { shortname = qs; }
void cEmployee::setType(int s) { etype = s; }
void cEmployee::setGroupID(int i) { id_grp = i; }
void cEmployee::setTherapyGroupID(int i) { id_tgrp = i; }
void cEmployee::setFlag(int i) { flag = i; }
void cEmployee::setPersonellNumber(QString s) { persnr = s; }
QString cEmployee::cEmployee::getName() { return surname; }
QString cEmployee::getForename() { return forename; }
QDate cEmployee::getBirthday() { return birthday; }
QString cEmployee::getStreet() { return street; }
QString cEmployee::getCity() { return city; }
QString cEmployee::getZipcode() { return zip; }
QString cEmployee::getTelephone() { return tel; }
QString cEmployee::getTelephoneMobile() { return tel_mobile; }
QString cEmployee::getFax() { return fax; }
QString cEmployee::getEMail() { return email; }
QString cEmployee::getShortName() { return shortname; }
int cEmployee::getType() { return etype; }
int cEmployee::getGroupID() { return id_grp; }
int cEmployee::getTherapyGroupID() { return id_tgrp; }
int cEmployee::getFlag() { return flag; }
QString cEmployee::getPersonellNumber() { return persnr; }
QString cEmployee::getFullName() { return forename + " " + surname; }
QList<int>& cEmployee::getQualifications()
{
    return listQuali;
}
void cEmployee::setQualifications(QList<int> l)
{
    listQuali = l;
}
int cEmployee::getGender() { return gender; }
void cEmployee::setGender(int i) { gender = i; }
int cEmployee::getState() { return state; }
void cEmployee::setState(int i) { state = i; }

void cEmployee::retrieve(QSqlQuery& query)
{
    setID(query.value(0).toInt());
    setName(query.value(1).toString());
    setForename(query.value(2).toString());
    setStreet(query.value(3).toString());
    setCity(query.value(4).toString());
    setZipcode(query.value(5).toString());
    setTelephone(query.value(6).toString());
    setTelephoneMobile(query.value(7).toString());
    setFax(query.value(8).toString());
    setEMail(query.value(9).toString());
    setBirthday(QDate::fromString(query.value(10).toString(), "yyyy-MM-dd"));
    setShortName(query.value(11).toString());
    QString s = query.value(12).toString();
    if (!s.isEmpty()) {
        QStringList l = s.split(":");
        for (int i = 0; i < l.count(); i++) {
            listQuali.append(l.at(i).toInt());
        }
    }
    setType(query.value(13).toInt());
    setGroupID(query.value(14).toInt());
    setTherapyGroupID(query.value(15).toInt());
    setFlag(query.value(16).toInt());
    setGender(query.value(17).toInt());
    setPersonellNumber(query.value(18).toString());
    setState(query.value(19).toInt());
}

void cEmployee::bind4store(QSqlQuery& query)
{
    query.addBindValue(getName());
    query.addBindValue(getForename());
    query.addBindValue(getStreet());
    query.addBindValue(getCity());
    query.addBindValue(getZipcode());
    query.addBindValue(getTelephone());
    query.addBindValue(getTelephoneMobile());
    query.addBindValue(getFax());
    query.addBindValue(getEMail());
    query.addBindValue(getBirthday().toString("yyyy-MM-dd"));
    query.addBindValue(getShortName());
    QStringList l;
    for (int i = 0; i < listQuali.count(); i++) {
        l << QString::number(listQuali.at(i));
    }
    query.addBindValue(l.join(":"));
    query.addBindValue(getType());
    query.addBindValue(getGroupID());
    query.addBindValue(getTherapyGroupID());
    query.addBindValue(getFlag()); // del flag
    query.addBindValue(getGender());
    query.addBindValue(getPersonellNumber());
    query.addBindValue(getState());
}

QString cEmployee::getProperty(QString strProp)
{
    QString s = "";

    if (strProp == "Nam") {
        s = getFullName();
    }

    return s;
}

#endif // OPA


//****************************************************************************
// cPhysType
//****************************************************************************
#ifndef OPA

cPhysType::cPhysType() : cTableItem(myDB::TABLE_PHYSICIAN_TYPES) { descr = ""; cTableItem::reset(); }
cPhysType& cPhysType::operator=(cPhysType& other)
{
    if (this != &other) {
	setID(other.getID());
	descr = other.getDescription();
    }
    return *this;
}
void cPhysType::setDescription(QString s) { descr = s; }
QString cPhysType::getDescription() { return descr; }

void cPhysType::retrieve(QSqlQuery& query)
{
    setID(query.value(0).toInt());
    setDescription(query.value(1).toString());
}

void cPhysType::bind4store(QSqlQuery& query)
{
    query.addBindValue(getDescription());
}

#endif // OPA

//****************************************************************************
// cPraxis
//****************************************************************************

cPraxis::cPraxis() : cTableItem(myDB::TABLE_PRAXIS) { reset(); }

void cPraxis::reset()
{
    prof = tariff_area = gender = eid = ppart = -1;
    name1 = name2 = surname = forename = title = street = "";
    city = zip = tel = mobile = fax = email = "";
    web = ik = "";

    cTableItem::reset();
}

cPraxis& cPraxis::operator=(cPraxis& other)
{
    if (this != &other) {
        setID(other.getID());
        name1 = other.getName1();
        name2 = other.getName2();
        surname = other.getSurname();
        forename = other.getForename();
        title =   other.getTitle();
        gender = other.getGender();
        street = other.getStreet();
        city = other.getCity();
        zip = other.getZip();
        tel = other.getTel();
        mobile = other.getMobile();
        fax = other.getFax();
        email = other.getEmail();
        web = other.getWeb();
        prof = other.getProf();
        tariff_area = other.getTariffArea();
        ik = other.getIk();
        eid = other.getEid();
        ppart = other.getPpart();
    }
    return *this;
}

void cPraxis::setName1(QString qs) { name1 = qs; }
void cPraxis::setName2(QString qs) { name2 = qs; }
void cPraxis::setSurname(QString qs) { surname = qs; }
void cPraxis::setForename(QString qs) { forename = qs; }
void cPraxis::setTitle(QString qs) { title = qs; }
void cPraxis::setGender(int i) { gender = i; }
void cPraxis::setStreet(QString qs) { street = qs;}
void cPraxis::setCity(QString qs) { city = qs; }
void cPraxis::setZip(QString qs) { zip = qs; }
void cPraxis::setTel(QString qs) {tel =qs; }
void cPraxis::setMobile (QString qs) { mobile = qs; }
void cPraxis::setFax (QString qs) {fax = qs; }
void cPraxis::setEmail(QString qs) { email = qs; }
void cPraxis::setWeb(QString qs) { web = qs; }
void cPraxis::setProf(int i) { prof = i; }
void cPraxis::setTariffArea(int i) { tariff_area = i; }
void cPraxis::setIk(QString qs) { ik = qs; }
void cPraxis::setEid(int i) { eid = i; }
void cPraxis::setPpart(int i) { ppart = i; }

QString cPraxis::getName1() { return name1; }
QString cPraxis::getName2() { return name2; }
QString cPraxis::getSurname() { return surname; }
QString cPraxis::getForename() { return forename; }
QString cPraxis::getTitle() { return title; }
int cPraxis::getGender() { return gender; }
QString cPraxis::getStreet() { return street; }
QString cPraxis::getCity() { return city; }
QString cPraxis::getZip() { return zip; }
QString cPraxis::getTel() { return tel; }
QString cPraxis::getMobile() { return mobile; }
QString cPraxis::getFax() { return fax; }
QString cPraxis::getEmail() { return email; }
QString cPraxis::getWeb() { return web; }
int cPraxis::getProf() { return prof; }
int cPraxis::getTariffArea() { return tariff_area; }
QString cPraxis::getIk() { return ik; }
int cPraxis::getEid() { return eid; }
int cPraxis::getPpart() { return ppart; }


void cPraxis::retrieve(QSqlQuery& query)
{
    setID(query.value(0).toInt());
    setName1(query.value(1).toString());
    setName2(query.value(2).toString());
    setSurname(query.value(3).toString());
    setForename(query.value(4).toString());
    setTitle(query.value(5).toString());
    setGender(query.value(6).toInt());
    setStreet(query.value(7).toString());
    setCity(query.value(8).toString());
    setZip(query.value(9).toString());
    setTel(query.value(10).toString());
    setMobile (query.value(11).toString());
    setFax (query.value(12).toString());
    setEmail(query.value(13).toString());
    setWeb(query.value(14).toString());
    setProf(query.value(15).toInt());
    setTariffArea(query.value(16).toInt());
    setIk(query.value(17).toString());
    setEid(query.value(18).toInt());
    setPpart(query.value(19).toInt());
 }

void cPraxis::bind4store(QSqlQuery& query)
{
    query.addBindValue(getName1());
    query.addBindValue(getName2());
    query.addBindValue(getSurname());
    query.addBindValue(getForename());
    query.addBindValue(getTitle());
    query.addBindValue(getGender());
    query.addBindValue(getStreet());
    query.addBindValue(getCity());
    query.addBindValue(getZip());
    query.addBindValue(getTel());
    query.addBindValue(getMobile());
    query.addBindValue(getFax() );
    query.addBindValue(getEmail());
    query.addBindValue(getWeb());
    query.addBindValue(getProf());
    query.addBindValue(getTariffArea());
    query.addBindValue(getIk());
    query.addBindValue(getEid());
    query.addBindValue(getPpart());
}

QString cPraxis::getProperty(QString strProp)
{
    QString s = "";

   if (strProp == "Adr") {
        if (!getName1().isEmpty()) s = getName1() + "\n";
        if (!getName2().isEmpty()) s = s + getName2() + "\n";
        s = s + getStreet() + "\n" + getZip() + " " + getCity();
    } else if (strProp == "Adr1") {
        s = getStreet() + "\n" + getZip() + " " + getCity();
    } else if (strProp == "Fullname") {
        if (!getTitle().isEmpty()) s = getTitle() + " ";
        s = s + getSurname();
    }

    return s;
}


//****************************************************************************
// cPhys
//****************************************************************************

cPhys::cPhys() : cTableItem(myDB::TABLE_PHYSICIANS) { reset(); }

void cPhys::reset()
{
    gender = ptype = clinic = -1;
    surname = forename = surgery = title = street = "";
    city = zip = tel = tel_mobile = fax = email = "";
    bsnr = lanr = "";
    cTableItem::reset();
}

cPhys& cPhys::operator=(cPhys& other)
{
    if (this != &other) {
	setID(other.getID());
	surname = other.getName();
	forename = other.getForename();
	surgery = other.getSurgery();
	title = other.getTitle();
	gender = other.getGender();
	street = other.getStreet();
	city = other.getCity();
	zip = other.getZipcode();
	tel = other.getTelephone();
	tel_mobile = other.getTelephoneMobile();
	fax = other.getFax();
	email = other.getEMail();
	ptype = other.getPhysType();
    bsnr = other.getBSNR();
    lanr = other.getLANR();
    clinic = other.getClinicID();
    }
    return *this;
}

void cPhys::setName(QString qs) { surname = qs; }
void cPhys::setForename(QString qs) { forename = qs; }
void cPhys::setSurgery(QString qs) { surgery = qs; }
void cPhys::setTitle(QString qs) { title = qs; }
void cPhys::setGender(int i) { gender = i; }
void cPhys::setStreet(QString qs) { street = qs; }
void cPhys::setCity(QString qs) { city = qs; }
void cPhys::setZipcode(QString qs) { zip = qs; }
void cPhys::setTelephone(QString qs) { tel = qs; }
void cPhys::setTelephoneMobile(QString qs) { tel_mobile = qs; }
void cPhys::setFax(QString qs) { fax = qs; }
void cPhys::setEMail(QString qs) { email = qs; }
void cPhys::setPhysType(int i) { ptype = i; }
void cPhys::setBSNR(QString s) { bsnr = s; }
void cPhys::setLANR(QString s) { lanr = s; }
void cPhys::setClinicID(int i) { clinic = i; }

QString cPhys::getName() { return surname; }
QString cPhys::getForename() { return forename; }
QString cPhys::getSurgery() { return surgery; }
QString cPhys::getTitle() { return title; }
int cPhys::getGender() { return gender; }
QString cPhys::getStreet() { return street; }
QString cPhys::getCity() { return city; }
QString cPhys::getZipcode() { return zip; }
QString cPhys::getTelephone() { return tel; }
QString cPhys::getTelephoneMobile() { return tel_mobile; }
QString cPhys::getFax() { return fax; }
QString cPhys::getEMail() { return email; }
int cPhys::getPhysType() { return ptype; }
QString cPhys::getBSNR() { return bsnr; }
QString cPhys::getLANR() { return lanr; }
int cPhys::getClinicID() { return clinic; }

void cPhys::retrieve(QSqlQuery& query)
{
    setID(query.value(0).toInt());
    setName(query.value(1).toString());
    setForename(query.value(2).toString());
    setTitle(query.value(3).toString());
    setGender(query.value(4).toInt());
    setSurgery(query.value(5).toString());
    setStreet(query.value(6).toString());
    setCity(query.value(7).toString());
    setZipcode(query.value(8).toString());
    setTelephone(query.value(9).toString());
    setTelephoneMobile(query.value(10).toString());
    setFax(query.value(11).toString());
    setEMail(query.value(12).toString());
    setPhysType(query.value(13).toInt());
    setBSNR(query.value(14).toString());
    setLANR(query.value(15).toString());
    setClinicID(query.value(16).toInt());
}

void cPhys::bind4store(QSqlQuery& query)
{
    query.addBindValue(getName());
    query.addBindValue(getForename());
    query.addBindValue(getTitle());
    query.addBindValue(getGender());
    query.addBindValue(getSurgery());
    query.addBindValue(getStreet());
    query.addBindValue(getCity());
    query.addBindValue(getZipcode());
    query.addBindValue(getTelephone());
    query.addBindValue(getTelephoneMobile());
    query.addBindValue(getFax());
    query.addBindValue(getEMail());
    query.addBindValue(getPhysType());
    query.addBindValue(getBSNR());
    query.addBindValue(getLANR());
    query.addBindValue(getClinicID());
}

QString cPhys::getProperty(QString strProp)
{
    QString s = "";

    if (strProp == "Adr") {
        if (!getTitle().isEmpty()) s = getTitle() + "\n";
        s = s + getForename() + " " + getName() + "\n";
        s = s + getStreet() + "\n" + getZipcode() + " " + getCity();

    } else if (strProp == "Nam") {
        if (!getTitle().isEmpty()) s = getTitle() + " ";
        s = s + getForename() + " " + getName();
    }

    return s;
}


//****************************************************************************
// cClinic
//****************************************************************************
cClinic::cClinic(): cTableItem(myDB::TABLE_CLINICS) { this->reset(); }
void cClinic::reset()
{
    name1 = name2 = street = city = zip = "";
    tel = fax = email = "";
    cTableItem::reset();
}
cClinic& cClinic::operator=(cClinic& other)
{
    if (this != &other) {
        setID(other.getID());
        name1 = other.getName1();
        name2 = other.getName2();
        street = other.getStreet();
        city = other.getCity();
        zip = other.getZipcode();
        tel = other.getTelephone();
        fax = other.getFax();
        email = other.getEMail();
    }
    return *this;
}
void cClinic::setName1(QString qs) { name1 = qs; }
void cClinic::setName2(QString qs) { name2 = qs; }
void cClinic::setStreet(QString qs) { street = qs; }
void cClinic::setCity(QString qs) { city = qs; }
void cClinic::setZipcode(QString qs) { zip = qs; }
void cClinic::setTelephone(QString qs) { tel = qs; }
void cClinic::setFax(QString qs) { fax = qs; }
void cClinic::setEMail(QString qs) { email = qs; }

QString cClinic::getName1() { return name1; }
QString cClinic::getName2() { return name2; }
QString cClinic::getStreet() { return street; }
QString cClinic::getCity() { return city; }
QString cClinic::getZipcode() { return zip; }
QString cClinic::getTelephone() { return tel; }
QString cClinic::getFax() { return fax; }
QString cClinic::getEMail() { return email; }

void cClinic::retrieve(QSqlQuery& query)
{
    setID(query.value(0).toInt());
    setName1(query.value(1).toString());
    setName2(query.value(2).toString());
    setCity(query.value(3).toString());
    setStreet(query.value(4).toString());
    setZipcode(query.value(5).toString());
    setTelephone(query.value(6).toString());
    setFax(query.value(7).toString());
    setEMail(query.value(8).toString());
}

void cClinic::bind4store(QSqlQuery& query)
{
    query.addBindValue(getName1());
    query.addBindValue(getName2());
    query.addBindValue(getCity());
    query.addBindValue(getStreet());
    query.addBindValue(getZipcode());
    query.addBindValue(getTelephone());
    query.addBindValue(getFax());
    query.addBindValue(getEMail());
}


//****************************************************************************
// cIndicator
//****************************************************************************
#ifndef OPA

cIndicator::cIndicator() : cTableItem(myDB::TABLE_INDICATORS) { ind = ""; cTableItem::reset(); }
void cIndicator::setDescription(QString s) { ind = s; }
QString cIndicator::getDescription() { return ind; }

void cIndicator::retrieve(QSqlQuery& query)
{
    setID(query.value(0).toInt());
    setDescription(query.value(1).toString());
}

void cIndicator::bind4store(QSqlQuery& query)
{
    query.addBindValue(getDescription());
}

#endif // OPA


//****************************************************************************
// cInsuranceType
//****************************************************************************
cInsuranceType::cInsuranceType() : cTableItem(myDB::TABLE_INSURANCE_TYPES) { name = ""; cTableItem::reset(); }
void cInsuranceType::setDescription(QString s) { name = s; }
QString cInsuranceType::getDescription() { return name; }
void cInsuranceType::retrieve(QSqlQuery& query)
{
    setID(query.value(0).toInt());
    setDescription(query.value(1).toString());
}
void cInsuranceType::bind4store(QSqlQuery& query)
{
    query.addBindValue(getDescription());
}


//****************************************************************************
// cInsurance
//****************************************************************************
cInsurance::cInsurance() : cTableItem(myDB::TABLE_INSURANCES) { this->reset(); }
cInsurance::~cInsurance() {}
void cInsurance::reset() {
    assoc = 0;
    ik = "";
    cTableItem::reset();
}

cInsurance& cInsurance::operator=(cInsurance& other)
{
    if (this != &other) {
	setID(other.getID());
	assoc = other.getAssoc();
	name_short = other.getShortName();
	ik = other.getIK();
	vdt = other.getVDT();
	fkt = other.getFKT();
	kto = other.getKTO();
	vkg = other.getVKG();
	nam = other.getNAM();
	ans1 = other.getANS1();
	ans2 = other.getANS2();
	ans3 = other.getANS3();
	asp1 = other.getASP1();
	asp2 = other.getASP2();
	asp3 = other.getASP3();
	uem1 = other.getUEM1();
	uem2 = other.getUEM2();
	dfu1 = other.getDFU1();
	dfu2 = other.getDFU2();
    }
    return *this;
}

void cInsurance::setAssoc(int i) { assoc = i; }
int cInsurance::getAssoc() { return assoc; }
void cInsurance::setIK(QString qs) { ik = qs; }
QString cInsurance::getIK() { return ik; }
void cInsurance::setShortName(QString qs) { name_short = qs; }
QString cInsurance::getShortName() { return name_short; }

void cInsurance::setVDT(QString s) { vdt = s; }
QString cInsurance::getVDT() { return vdt; }
void cInsurance::setFKT(QString s) { fkt = s; }
QString cInsurance::getFKT() { return fkt; }
void cInsurance::setKTO(QString s) { kto = s; }
QString cInsurance::getKTO() { return kto; }
void cInsurance::setVKG(QString s) { vkg = s; }
QString cInsurance::getVKG() { return vkg; }
void cInsurance::setNAM(QString s) { nam = s; }
QString cInsurance::getNAM() { return nam; }
void cInsurance::setANS1(QString s) { ans1 = s; }
QString cInsurance::getANS1() { return ans1; }
void cInsurance::setANS2(QString s) { ans2 = s; }
QString cInsurance::getANS2() { return ans2; }
void cInsurance::setANS3(QString s) { ans3 = s; }
QString cInsurance::getANS3() { return ans3; }
void cInsurance::setASP1(QString s) { asp1 = s; }
QString cInsurance::getASP1() { return asp1; }
void cInsurance::setASP2(QString s) { asp2 = s; }
QString cInsurance::getASP2() { return asp2; }
void cInsurance::setASP3(QString s) { asp3 = s; }
QString cInsurance::getASP3() { return asp3; }
void cInsurance::setUEM1(QString s) { uem1 = s; }
QString cInsurance::getUEM1() { return uem1; }
void cInsurance::setUEM2(QString s) { uem2 = s; }
QString cInsurance::getUEM2() { return uem2; }
void cInsurance::setDFU1(QString s) { dfu1 = s; }
QString cInsurance::getDFU1() { return dfu1; }
void cInsurance::setDFU2(QString s) { dfu2 = s; }
QString cInsurance::getDFU2() { return dfu2; }

void cInsurance::retrieve(QSqlQuery& query)
{
    setID(query.value(0).toInt());
    setIK(query.value(1).toString());
    setShortName(query.value(2).toString());
    setAssoc(query.value(3).toInt());
    setVDT(query.value(4).toString());
    setFKT(query.value(5).toString());
    setKTO(query.value(6).toString());
    setVKG(query.value(7).toString());
    setNAM(query.value(8).toString());
    setANS1(query.value(9).toString());
    setANS2(query.value(10).toString());
    setANS3(query.value(11).toString());
    setASP1(query.value(12).toString());
    setASP2(query.value(13).toString());
    setASP3(query.value(14).toString());
    setUEM1(query.value(15).toString());
    setUEM2(query.value(16).toString());
    setDFU1(query.value(17).toString());
    setDFU2(query.value(18).toString());
}

void cInsurance::bind4store(QSqlQuery& query)
{
    query.addBindValue(getIK());
    query.addBindValue(getShortName());
    query.addBindValue(getAssoc());
    query.addBindValue(getVDT());
    query.addBindValue(getFKT());
    query.addBindValue(getKTO());
    query.addBindValue(getVKG());
    query.addBindValue(getNAM());
    query.addBindValue(getANS1());
    query.addBindValue(getANS2());
    query.addBindValue(getANS3());
    query.addBindValue(getASP1());
    query.addBindValue(getASP2());
    query.addBindValue(getASP3());
    query.addBindValue(getUEM1());
    query.addBindValue(getUEM2());
    query.addBindValue(getDFU1());
    query.addBindValue(getDFU1());
}


//****************************************************************************
// cReceipeState
//****************************************************************************
#ifndef OPA
cReceipeState::cReceipeState() : cTableItem(myDB::TABLE_RECEIPE_STATES) { descr = ""; cTableItem::reset(); }
void cReceipeState::setDescription(QString s) { descr = s; }
QString cReceipeState::getDescription() { return descr; }
void cReceipeState::retrieve(QSqlQuery& query)
{
    setID(query.value(0).toInt());
    setDescription(query.value(1).toString());
}
void cReceipeState::bind4store(QSqlQuery& query)
{
    query.addBindValue(getDescription());
}
#endif // OPA


//****************************************************************************
// cReceipeShortcut
//****************************************************************************
cReceipeShortcut::cReceipeShortcut() : cTableItem(myDB::TABLE_RECEIPE_SHORTCUTS) { this->reset(); }
void cReceipeShortcut::reset()
{
    amount = 0;
    prof = 0;
    descr = "";
    posnr = "";
    cTableItem::reset();
}

void cReceipeShortcut::setProfession(int i) { prof = i; }
int cReceipeShortcut::getProfession() { return prof; }
void cReceipeShortcut::setAmount(int i) { amount = i; }
int cReceipeShortcut::getAmount() { return amount; }
void cReceipeShortcut::setDescription(QString s) { descr = s; }
QString cReceipeShortcut::getDescription() { return descr; }
void cReceipeShortcut::setPosNr(QString s) { posnr = s; }
QString cReceipeShortcut::getPosNr() { return posnr; }
void cReceipeShortcut::retrieve(QSqlQuery& query)
{
    setID(query.value(0).toInt());
    setProfession(query.value(1).toInt());
    setAmount(query.value(2).toInt());
    setDescription(query.value(3).toString());
    setPosNr(query.value(4).toString());
}
void cReceipeShortcut::bind4store(QSqlQuery& query)
{
    query.addBindValue(getProfession());
    query.addBindValue(getAmount());
    query.addBindValue(getDescription());
    query.addBindValue(getPosNr());
}


//****************************************************************************
// cReceipe
//****************************************************************************

cReceipe::cReceipe() : cTableItem(myDB::TABLE_RECEIPES) {this->reset();}
cReceipe::~cReceipe() {}

cReceipe& cReceipe::operator=(cReceipe& other)
{
    if (this != &other) {
	setID(other.getID());
	rtype = other.getType();
	rstate = other.getState();
	pid = other.getPatientID();
	physid = other.getPhysID();
	ik = other.getIK();
	rdate = other.getDate();
        sdate = other.getStartDate();
	ind = other.getIndID();
	diag = other.getDiagnosis();
	f_charge = other.getFlagCharge();
	f_visit = other.getFlagVisit();
	f_group = other.getFlagGroup();
	f_accident = other.getFlagAccident();
	f_report = other.getFlagReport();
	tos = other.getTOS();
	dist = other.getDistance();
	pos = other.getPositions();
	brutto = other.getBrutto();
	addpay = other.getAddpay();
	pricetab = other.getPriceTabID();
	oflags = other.getOptionFlags();
	addpay_flag = other.getAddPayFlag();
	addpay_date = other.getAddPayDate();
	bill_date = other.getBillDate();
	pt_version = other.getPricetableVersion();
	hb_count = other.getHBCount();
        prof = other.getProfession();
        notes = other.getNotes();
        hb_pos_flat = other.getHBPosFlat();
        hb_pos_dist = other.getHBPosDist();
    }
    return *this;
}

void cReceipe::reset()
{
    insurant = ik = brutto = addpay = pos = "";
    pid = physid = pricetab = ind = -1;
    f_charge = 1;
    hb_count = 0;
    prof = -1;
    f_visit = f_group = f_accident = f_report = 0;
    rtype = rstate = dist = tos = oflags = addpay_flag = 0;
    pt_version = "";
    notes = "";
    hb_pos_flat = hb_pos_dist = -1;
    cTableItem::reset();
}

QString cReceipe::getInsurant() { return insurant; }
int cReceipe::getType() { return rtype; }
void cReceipe::setType(int t) { rtype = t; }
int cReceipe::getState() { return rstate; }
void cReceipe::setState(int s) { rstate = s; }
int cReceipe::getPatientID() { return pid; }
void cReceipe::setPatientID(int p) { pid = p; }
int cReceipe::getPhysID() { return physid; }
void cReceipe::setPhysID(int p) { physid = p; }
QString cReceipe::getIK() { return ik; }
void cReceipe::setIK(QString s) { ik = s; }
QDate cReceipe::getDate() { return rdate; }
void cReceipe::setDate(QDate d) { rdate = d; }
QDate cReceipe::getStartDate() { return sdate; }
void cReceipe::setStartDate(QDate d) { sdate = d; }
int cReceipe::getIndID() { return ind; }
void cReceipe::setIndID(int i) { ind = i; }
QString cReceipe::getDiagnosis() { return diag; }
void cReceipe::setDiagnosis(QString s) { diag = s; }
int cReceipe::getFlagCharge() { return f_charge; }
void cReceipe::setFlagCharge(int f) { f_charge = f; }
int cReceipe::getFlagVisit() { return f_visit; }
void cReceipe::setFlagVisit(int f) { f_visit = f; }
int cReceipe::getFlagGroup() { return f_group; }
void cReceipe::setFlagGroup(int f) { f_group = f; }
int cReceipe::getFlagAccident() { return f_accident; }
void cReceipe::setFlagAccident(int f) { f_accident = f; }
int cReceipe::getFlagReport() { return f_report; }
void cReceipe::setFlagReport(int f) { f_report = f; }
int cReceipe::getTOS() { return tos; }
void cReceipe::setTOS(int t) { tos = t; }
int cReceipe::getDistance() { return dist; }
void cReceipe::setDistance(int d) { dist = d; }
QString cReceipe::getPositions() { return pos; }
void cReceipe::setPositions(QString p) { pos = p; }
QString cReceipe::getBrutto() { return brutto; }
void cReceipe::setBrutto(QString b) { brutto = b; }
QString cReceipe::getAddpay() { return addpay; }
void cReceipe::setAddpay(QString b) { addpay = b; }
void cReceipe::setPriceTabID(int t) { pricetab = t; }
int cReceipe::getPriceTabID() { return pricetab; }
void cReceipe::setOptionFlags(int i) { oflags = i; }
int cReceipe::getOptionFlags() { return oflags; }
void cReceipe::setAddPayFlag(int i) { addpay_flag = i; }
int cReceipe::getAddPayFlag() { return addpay_flag; }
void cReceipe::setAddPayDate(QDate d) { addpay_date = d; }
QDate cReceipe::getAddPayDate() { return addpay_date; }
void cReceipe::setBillDate(QDate d) { bill_date = d; }
QDate cReceipe::getBillDate() { return bill_date; }
QString cReceipe::getPricetableVersion() { return pt_version; }
void cReceipe::setPricetableVersion(QString v) { pt_version = v; }
int cReceipe::getHBCount() { return hb_count; }
void cReceipe::setHBCount(int i) { hb_count = i; }
int cReceipe::getProfession() { return prof; }
void cReceipe::setProfession(int i) { prof = i; }
QString cReceipe::getNotes() { return notes; }
void cReceipe::setNotes(QString s) { notes = s; }
int cReceipe::getHBPosFlat() { return hb_pos_flat; }
void cReceipe::setHBPosFlat(int i) { hb_pos_flat = i; }
int cReceipe::getHBPosDist() { return hb_pos_dist; }
void cReceipe::setHBPosDist(int i) { hb_pos_dist = i; }

bool cReceipe::isSetManualCalc() { return (bool)((oflags >> POS_FLAG_MANUAL_CALC) & 1); }
void cReceipe::setManualCalc(bool b)
{
    if (b) oflags |= (1 << POS_FLAG_MANUAL_CALC);
    else oflags &= (not (1 << POS_FLAG_MANUAL_CALC));
}
bool cReceipe::isSetManualCalcHB() { return (bool)((oflags >> POS_FLAG_MANUAL_CALC_HB) & 1); }
void cReceipe::setManualCalcHB(bool b)
{
    if (b) oflags |= (1 << POS_FLAG_MANUAL_CALC_HB);
    else oflags &= (not (1 << POS_FLAG_MANUAL_CALC_HB));
}

void cReceipe::retrieve(QSqlQuery& query)
{
    setID(query.value(0).toInt());
    setType(query.value(1).toInt());
    setState(query.value(2).toInt());
    setPatientID(query.value(3).toInt());
    setPhysID(query.value(4).toInt());
    setIK(query.value(5).toString());
    setPriceTabID(query.value(6).toInt());
    setPricetableVersion(query.value(7).toString());
    setDate(QDate::fromString(query.value(8).toString(), "yyyy-MM-dd"));
    setStartDate(QDate::fromString(query.value(9).toString(), "yyyy-MM-dd"));
    setIndID(query.value(10).toInt());
    setDiagnosis(query.value(11).toString());
    setFlagCharge(query.value(12).toInt());
    setFlagVisit(query.value(13).toInt());
    setFlagGroup(query.value(14).toInt());
    setFlagAccident(query.value(15).toInt());
    setFlagReport(query.value(16).toInt());
    setDistance(query.value(17).toInt());
    setTOS(query.value(18).toInt());
    setPositions(query.value(19).toString());
    setBrutto(query.value(20).toString());
    setAddpay(query.value(21).toString());
    setOptionFlags(query.value(22).toInt());
    setAddPayFlag(query.value(23).toInt());
    setAddPayDate(QDate::fromString(query.value(24).toString(), "yyyy-MM-dd"));
    setBillDate(QDate::fromString(query.value(25).toString(), "yyyy-MM-dd"));
    setHBCount(query.value(26).toInt());
    setHBPosFlat(query.value(27).toInt());
    setHBPosDist(query.value(28).toInt());
    setProfession(query.value(29).toInt());
    setNotes(query.value(30).toString());
}

void cReceipe::bind4store(QSqlQuery& query)
{
    query.addBindValue(getType());
    query.addBindValue(getState());
    query.addBindValue(getPatientID());
    query.addBindValue(getPhysID());
    query.addBindValue(getIK());
    query.addBindValue(getPriceTabID());
    query.addBindValue(getPricetableVersion());
    query.addBindValue(getDate().toString("yyyy-MM-dd"));
    query.addBindValue(getStartDate().toString("yyyy-MM-dd"));
    query.addBindValue(getIndID());
    query.addBindValue(getDiagnosis());
    query.addBindValue(getFlagCharge());
    query.addBindValue(getFlagVisit());
    query.addBindValue(getFlagGroup());
    query.addBindValue(getFlagAccident());
    query.addBindValue(getFlagReport());
    query.addBindValue(getDistance());
    query.addBindValue(getTOS());
    query.addBindValue(getPositions());
    query.addBindValue(getBrutto());
    query.addBindValue(getAddpay());
    query.addBindValue(getOptionFlags());
    query.addBindValue(getAddPayFlag());
    query.addBindValue(getAddPayDate().toString("yyyy-MM-dd"));
    query.addBindValue(getBillDate().toString("yyyy-MM-dd"));
    query.addBindValue(getHBCount());
    query.addBindValue(getHBPosFlat());
    query.addBindValue(getHBPosDist());
    query.addBindValue(getProfession());
    query.addBindValue(getNotes());
}

QString cReceipe::getProperty(QString strProp)
{
    QString s = "";

    if (strProp == "Dat") s = getDate().toString("dd.MM.yyyy");
    else if (strProp == "Br") s = getBrutto().replace(".","",Qt::CaseInsensitive);
    else if (strProp == "ZZ") s = getAddpay().replace(".","",Qt::CaseInsensitive);
    else if (strProp == "RDat") {
        if (getID() > 0 && getPriceTabID() >= 0 && !getPositions().isEmpty()) { // build receipe data
            cPriceTable cpt;
            db.loadPriceTable(db.getPriceTableName(getPriceTabID()), cpt, getProfession());
            QStringList qsl = getPositions().split(";");
            for (int y = 0; y < qsl.count(); y++) {
                if (QString(qsl[y]).isEmpty()) continue;
                QStringList qsl2 = qsl[y].split(":");
                s = s + qsl2[0] + "x ";
                int idx = qsl2[1].toInt();
                cPriceTableItem* cpti = cpt.at(idx);
                if (cpti == NULL) continue;
                s = s + cpti->getPosCategory() + "\n";
                qsl2.clear();
            }
            if ((bool)getFlagVisit()) {
                s = s + QString::number(getHBCount()) + "x Hausbesuch";
            }
        }
    }
    return s;
}


//****************************************************************************
// cRole
//****************************************************************************
cRole::cRole() : cTableItem(myDB::TABLE_ROLES) { reset(); }
cRole::cRole(int i, QString n, QString p) : cTableItem(myDB::TABLE_ROLES)
{
    setID(i);
    name = n;
    pw = p;
}
cRole::~cRole() {}

void cRole::reset()
{
    name = pw = "";
    cTableItem::reset();
}

cRole& cRole::operator=(cRole& other)
{
    if (this != &other) {
	setID(other.getID());
	name = other.getName();
	pw = other.getPW();
    }
    return *this;
}

void cRole::setName(QString s) { name = s; }
QString cRole::getName() { return name; }
void cRole::setPW(QString s) { pw = s; }
QString cRole::getPW() { return pw; }

void cRole::retrieve(QSqlQuery& query)
{
    setID(query.value(0).toInt());
    setName(query.value(1).toString());
    setPW(query.value(2).toString());
}

void cRole::bind4store(QSqlQuery& query)
{
    query.addBindValue(getName());
    query.addBindValue(getPW());
}


//****************************************************************************
// cDateType
//****************************************************************************
#ifndef OPA
cDateType::cDateType() : cTableItem(myDB::TABLE_DATE_TYPES) { dt_desc = ""; cTableItem::reset(); }
cDateType::~cDateType() {}
void cDateType::setDescription(QString s) { dt_desc = s; }
QString cDateType::getDescription() { return dt_desc; }
void cDateType::retrieve(QSqlQuery& query)
{
    setID(query.value(0).toInt());
    setDescription(query.value(1).toString());
}
void cDateType::bind4store(QSqlQuery& query)
{
    query.addBindValue(getDescription());
}
#endif // OPA


//****************************************************************************
// cDateState
//****************************************************************************
#ifndef OPA
cDateState::cDateState() : cTableItem(myDB::TABLE_DATE_STATES) { descr = ""; cTableItem::reset(); }
void cDateState::setDescription(QString s) { descr = s; }
QString cDateState::getDescription() { return descr; }
void cDateState::retrieve(QSqlQuery& query)
{
    setID(query.value(0).toInt());
    setDescription(query.value(1).toString());
}
void cDateState::bind4store(QSqlQuery& query)
{
    query.addBindValue(getDescription());
}
#endif // OPA


//****************************************************************************
// cDateActivity
//****************************************************************************
#ifndef OPA
cDateActivity::cDateActivity() : cTableItem(myDB::TABLE_DATE_ACTIVITIES)
{
    a_id = prof = -1;
    a_enabled = true;
    a_name = "";
    cTableItem::reset();
}
void cDateActivity::setProfession(int i) { prof = i; }
int cDateActivity::getProfession() { return prof; }
void cDateActivity::setActivityID(int i) { a_id = i; }
int cDateActivity::getActivityID() { return a_id; }
void cDateActivity::setActivityName(QString s) { a_name = s; }
QString cDateActivity::getActivityName() { return a_name; }
void cDateActivity::setActivityEnabled(bool b) { a_enabled = b; }
bool cDateActivity::getActivityEnabled() { return a_enabled; }

void cDateActivity::retrieve(QSqlQuery& query)
{
    setID(query.value(0).toInt());
    setProfession(query.value(1).toInt());
    setActivityID(query.value(2).toInt());
    setActivityName(query.value(3).toString());
    setActivityEnabled(query.value(4).toBool());
}
void cDateActivity::bind4store(QSqlQuery& query)
{
    query.addBindValue(getProfession());
    query.addBindValue(getActivityID());
    query.addBindValue(getActivityName());
    query.addBindValue(getActivityEnabled());
}
#endif


//****************************************************************************
// cMetaDate
//****************************************************************************
#ifndef OPA
cMetaDate::cMetaDate() : cTableItem(myDB::TABLE_META_DATES) { reset(); }
cMetaDate::~cMetaDate() {}
cMetaDate& cMetaDate::operator=(cMetaDate& other)
{
    if (this != &other) {
        setID(other.getID());
        setStart(other.getStart());
        setEnd(other.getEnd());
        setRepeatFlag(other.getRepeatFlag());
    }
    return *this;
}
void cMetaDate::reset() { flag_repeat = -1; cTableItem::reset(); }
void cMetaDate::setStart(QDate d) { md_start = d; }
void cMetaDate::setEnd(QDate d) { md_end = d; }
void cMetaDate::setRepeatFlag(int i) { flag_repeat = i; }
QDate cMetaDate::getStart() { return md_start; }
QDate cMetaDate::getEnd() { return md_end; }
int cMetaDate::getRepeatFlag() { return flag_repeat; }

void cMetaDate::retrieve(QSqlQuery& query)
{
    setID(query.value(0).toInt());
    setStart(QDate::fromString(query.value(1).toString(), "yyyy-MM-dd"));
    setEnd(QDate::fromString(query.value(2).toString(), "yyyy-MM-dd"));
    setRepeatFlag(query.value(3).toInt());
}

void cMetaDate::bind4store(QSqlQuery& query)
{
    query.addBindValue(getStart().toString("yyyy-MM-dd"));
    query.addBindValue(getEnd().toString("yyyy-MM-dd"));
    query.addBindValue(getRepeatFlag());
}

//****************************************************************************
// cAppendixItem
//****************************************************************************
cAppendixItem::cAppendixItem() : cTableItem(myDB::TABLE_PAPPENDIX) { reset(); }
cAppendixItem::~cAppendixItem() {}

void cAppendixItem::reset()
{
    cTableItem::reset();
}

void cAppendixItem::setId(int i) { id = i; }
int cAppendixItem::getId() { return id; }
void cAppendixItem::setPid(int i) { pid = i; }
int cAppendixItem::getPid() { return pid; }
void cAppendixItem::setRdate(QDate d) { rdate = d; }
QDate cAppendixItem::getRdate() { return rdate; }
void cAppendixItem::setInsurance(QString s) { insurance = s; }
QString cAppendixItem::getInsurance() { return insurance; }
void cAppendixItem::setPricetab(int i) { pricetab = i; }
int cAppendixItem::getPricetab() { return pricetab; }
void cAppendixItem::setPositions(QString s) { positions = s; }
QString cAppendixItem::getPositions() { return positions; }
void cAppendixItem::setBrutto(QString s) { brutto = s; }
QString cAppendixItem::getBrutto() { return brutto; }
void cAppendixItem::setBillDate(QDate d) { bill_date = d; }
QDate cAppendixItem::getBillDdate() {return bill_date; }

cAppendixItem& cAppendixItem::operator=(cAppendixItem& other)
{
    if (this != &other) {
        setId(other.getId());
        pid = other.getPid();
        rdate = other.getRdate();
        insurance = other.getInsurance();
        pricetab = other.getPricetab();
        positions = other.getPositions();
        brutto = other.getBrutto();
        bill_date = other.getBillDdate();
    }
    return *this;
}

void cAppendixItem::retrieve(QSqlQuery& query)
{
    setFlag(cTableItem::FLAG_ORG);
    setId(query.value(0).toInt());
    setPid(query.value(1).toInt());
    setRdate(query.value(2).toDate());
    setInsurance(query.value(3).toString());
    setPricetab(query.value(4).toInt());
    setPositions(query.value(5).toString());
    setBrutto(query.value(6).toString());
    setBillDate(query.value(7).toDate());
}

//****************************************************************************
// cDateItem
//****************************************************************************
cDateItem::cDateItem() : cTableItem(myDB::TABLE_DATES) { reset(); }
cDateItem::~cDateItem() {}

void cDateItem::reset()
{
    eid = pid = rid = cid = room = car = mid = -1;
    d_type = state = 0;
    act_physio = act_ergo = 0;
    options = 0;
    full_day = false;
    desc = content = title = activities = "";
    cTableItem::reset();
}
void cDateItem::setType(int i) { d_type = i; }
int cDateItem::getType() { return d_type; }
void cDateItem::setEID(int i) { eid = i; }
int cDateItem::getEID() { return eid; }
void cDateItem::setPID(int i) { pid = i; }
int cDateItem::getPID() { return pid; }
void cDateItem::setRID(int i) { rid = i; }
int cDateItem::getRID() { return rid; }
void cDateItem::setCID(int i) { cid = i; }
int cDateItem::getCID() { return cid; }
void cDateItem::setDate(QDate d) { date = d; }
QDate cDateItem::getDate() { return date; }
void cDateItem::setStart(QTime t) { t_start = t; }
QTime cDateItem::getStart() { return t_start; }
void cDateItem::setEnd(QTime t) { t_end = t; }
QTime cDateItem::getEnd() { return t_end; }
void cDateItem::setTitle(QString s) { title = s; }
QString cDateItem::getTitle() { return title; }
void cDateItem::setDesc(QString s) { desc = s; }
QString cDateItem::getDesc() { return desc; }
void cDateItem::setContent(QString s) { content = s; }
QString cDateItem::getContent() { return content; }
void cDateItem::setRoomID(int i) { room = i; }
int cDateItem::getRoomID() { return room; }
void cDateItem::setFullDay(bool b) { full_day = b; }
bool cDateItem::getFullDay() { return full_day; }
void cDateItem::setActPhysio(int i) { act_physio = i; }
int cDateItem::getActPhysio() { return act_physio; }
void cDateItem::setActErgo(int i) { act_ergo = i; }
int cDateItem::getActErgo() { return act_ergo; }
void cDateItem::setCarID(int i) { car = i; }
int cDateItem::getCarID() { return car; }
void cDateItem::setMetaID(int i) { mid = i; }
int cDateItem::getMetaID() { return mid; }
void cDateItem::setStateID(int i) { state = i; }
int cDateItem::getStateID() { return state; }
void cDateItem::setOptions(int i) { options = i; }
int cDateItem::getOptions() { return options; }

QString cDateItem::getTitleAndTime()
{
    return title + "\n" + t_start.toString("hh:mm") + "-" + t_end.toString("hh:mm");
}

QString cDateItem::getTitle(bool incl_activities, bool incl_time, bool line_break)
{
    QString s = title;
    QString lb = line_break ? "\n" : " ";
    if (incl_activities && !activities.isEmpty()) s += lb + activities;
    if (incl_time) s += lb + t_start.toString("hh:mm") + "-" + t_end.toString("hh:mm");
    return s;
}

QString cDateItem::getActivityString() { return activities; }

void cDateItem::decodeActivities()
{
    activities = "";
    if (act_physio == 0 && act_ergo == 0) return;

    QStringList l;
    int co_physio = 0;
    int co_ergo = 0;
    for (int i = 0; i < db.listDateActivities.count(); i++) {
        cDateActivity *cda = db.listDateActivities.at(i);
        if (cda->getProfession() == cProfession::PROF_PHYSIOTHERAPY) {
            if ((bool)((act_physio >> co_physio) & 1)) {
                l << cda->getActivityName();
            }
            co_physio++;
        }
        if (cda->getProfession() == cProfession::PROF_ERGOTHERAPY) {
            if ((bool)((act_ergo >> co_ergo) & 1)) {
                l << cda->getActivityName();
            }
            co_ergo++;
        }
    }
    if (!l.isEmpty()) {
        activities = "(" + l.join(",") + ")";
    }
}

cDateItem& cDateItem::operator=(cDateItem& other)
{
    if (this != &other) {
	setID(other.getID());
	d_type = other.getType();
	eid = other.getEID();
	pid = other.getPID();
	rid = other.getRID();
	cid = other.getCID();
	date = other.getDate();
	t_start = other.getStart();
	t_end = other.getEnd();
        title = other.getTitle();
        desc = other.getDesc();
        content = other.getContent();
        room = other.getRoomID();
        car = other.getCarID();
        state = other.getStateID();
        mid = other.getMetaID();
        full_day = other.getFullDay();
        act_physio = other.getActPhysio();
        act_ergo = other.getActErgo();
        setFlag(other.getFlag());
        options = other.getOptions();
        activities = other.getActivityString();
    }
    return *this;
}

bool cDateItem::overlap(QTime t1, QTime t2)
{
    int s_t1 = abs(t1.secsTo(QTime(0,0)));
    int s_t2 = abs(t2.secsTo(QTime(0,0)));
    int s_start = abs(t_start.secsTo(QTime(0,0)));
    int s_end = abs(t_end.secsTo(QTime(0,0)));
    if ( (s_t1 > s_start) && (s_t1 < s_end) ) return true;
    if ( (s_t2 > s_start) && (s_t2 < s_end) ) return true;
    return false;
}

void cDateItem::retrieve(QSqlQuery& query)
{
    setFlag(cTableItem::FLAG_ORG);
    setID(query.value(0).toInt());
    setDate(QDate::fromString(query.value(1).toString(), "yyyy-MM-dd"));
    setStart(QTime::fromString(query.value(2).toString(), "hh:mm"));
    setEnd(QTime::fromString(query.value(3).toString(), "hh:mm"));
    setType(query.value(4).toInt());
    setTitle(query.value(5).toString());
    setDesc(query.value(6).toString());
    setContent(query.value(7).toString());
    setFullDay(query.value(8).toBool());
    setActPhysio(query.value(9).toInt());
    setActErgo(query.value(10).toInt());
    setOptions(query.value(11).toInt());
    setRoomID(query.value(12).toInt());
    setCarID(query.value(13).toInt());
    setStateID(query.value(14).toInt());
    setMetaID(query.value(15).toInt());
    setEID(query.value(16).toInt());
    setPID(query.value(17).toInt());
    setRID(query.value(18).toInt());
    setCID(query.value(19).toInt());
    decodeActivities();
}

void cDateItem::bind4store(QSqlQuery& query)
{
    query.addBindValue(getDate().toString("yyyy-MM-dd"));
    query.addBindValue(getStart().toString("hh:mm"));
    query.addBindValue(getEnd().toString("hh:mm"));
    query.addBindValue(getType());
    query.addBindValue(getTitle());
    query.addBindValue(getDesc());
    query.addBindValue(getContent());
    query.addBindValue((int)getFullDay());
    query.addBindValue(getActPhysio());
    query.addBindValue(getActErgo());
    query.addBindValue(getOptions());
    query.addBindValue(getRoomID());
    query.addBindValue(getCarID());
    query.addBindValue(getStateID());
    query.addBindValue(getMetaID());
    query.addBindValue(getEID());
    query.addBindValue(getPID());
    query.addBindValue(getRID());
    query.addBindValue(getCID());
}

#endif // OPA


//****************************************************************************
// cReport
//****************************************************************************
#ifndef OPA

cReport::cReport() : cTableItem(myDB::TABLE_REPORTS)
{
    this->reset();
}

cReport::~cReport()
{
}

void cReport::reset()
{
    pid = rid = phys_id = eid = -1;
    type = 0;
    cTableItem::reset();
}

cReport& cReport::operator=(cReport& other)
{
    if (this != &other) {
	setID(other.getID());
	pid = other.getPatientID();
	rid = other.getReceipeID();
	phys_id = other.getPhysID();
	r_date = other.getDate();
	type = other.getType();
        eid = other.getAuthorID();
	rdata = other.getRData();
    }
    return *this;
}

int cReport::getReceipeID()
{
    return rid;
}

void cReport::setReceipeID(int i)
{
    rid = i;
}

int cReport::getPatientID()
{
    return pid;
}

void cReport::setPatientID(int i)
{
    pid = i;
}

int cReport::getPhysID()
{
    return phys_id;
}

void cReport::setPhysID(int i)
{
    phys_id = i;
}

QDate cReport::getDate()
{
    return r_date;
}

void cReport::setDate(QDate t)
{
    r_date = t;
}

int cReport::getType()
{
    return type;
}

void cReport::setType(int i)
{
    type = i;
}

QByteArray& cReport::getRData()
{
    return rdata;
}

void cReport::setRData(QByteArray& buf)
{
    rdata = buf;
}

int cReport::getAuthorID()
{
    return eid;
}

void cReport::setAuthorID(int i)
{
    eid = i;
}

void cReport::retrieve(QSqlQuery& query)
{
    setID(query.value(0).toInt());
    setDate(QDate::fromString(query.value(1).toString(), "yyyy-MM-dd"));
    setType(query.value(2).toInt());
    setPatientID(query.value(3).toInt());
    setReceipeID(query.value(4).toInt());
    setPhysID(query.value(5).toInt());
    setAuthorID(query.value(6).toInt());
    QByteArray qba = query.value(7).toByteArray();
    setRData(qba);
}

void cReport::bind4store(QSqlQuery& query)
{
    query.addBindValue(getDate().toString("yyyy-MM-dd"));
    query.addBindValue(getType());
    query.addBindValue(getPatientID());
    query.addBindValue(getReceipeID());
    query.addBindValue(getPhysID());
    query.addBindValue(getAuthorID());
    query.addBindValue(getRData());
}

QString cReport::getProperty(QString strProp)
{
    QString s = "";
    QXmlStreamReader xml(getRData());
    QString look_for = "";
    if (strProp == "F1") look_for = "content1";
    else if (strProp == "F2") look_for = "content2";
    else if (strProp == "F3") look_for = "content3";
    else if (strProp == "F4") look_for = "content4";
    else if (strProp == "F5") look_for = "content5";
    else if (strProp == "F6") look_for = "content6";
    else if (strProp == "F7") look_for = "content7";
    else if (strProp == "TPL") look_for = "tpl";
    if (!look_for.isEmpty()) {
        while (!xml.atEnd()) {
            xml.readNext();
            if (xml.isStartElement()) {
                if (xml.name() == look_for) {
                    s = xml.readElementText();
                    break;
                }
            }
        }
    } else {
        if (strProp == "Dat") return getDate().toString("dd.MM.yyyy");
    }

    return s;
}

#endif // OPA


//****************************************************************************
// cFinding
//****************************************************************************
#ifndef OPA

cFinding::cFinding() : cTableItem(myDB::TABLE_FINDINGS)
{
    this->reset();
}

cFinding::~cFinding()
{
}

void cFinding::reset()
{
    pid = eid = -1;
    fdata = "";
    cTableItem::reset();
}

cFinding& cFinding::operator=(cFinding& other)
{
    if (this != &other) {
	setID(other.getID());
	pid = other.getPatientID();
	eid = other.getEmplID();
	f_date = other.getDate();
	fdata = other.getFData();
    }
    return *this;
}

int cFinding::getPatientID()
{
    return pid;
}

void cFinding::setPatientID(int i)
{
    pid = i;
}

int cFinding::getEmplID()
{
    return eid;
}

void cFinding::setEmplID(int i)
{
    eid = i;
}

QDate cFinding::getDate()
{
    return f_date;
}

void cFinding::setDate(QDate t)
{
    f_date = t;
}

QString cFinding::getFData()
{
    return fdata;
}

void cFinding::setFData(QString buf)
{
    fdata = buf;
}

void cFinding::retrieve(QSqlQuery& query)
{
    setID(query.value(0).toInt());
    setDate(QDate::fromString(query.value(1).toString(), "yyyy-MM-dd"));
    setPatientID(query.value(2).toInt());
    setEmplID(query.value(3).toInt());
    setFData(query.value(4).toString());
 }

void cFinding::bind4store(QSqlQuery& query)
{
    query.addBindValue(getDate().toString("yyyy-MM-dd"));
    query.addBindValue(getPatientID());
    query.addBindValue(getEmplID());
    query.addBindValue(getFData());
}

#endif // OPA


//****************************************************************************
// cReminder
//****************************************************************************
#ifndef OPA

cReminder::cReminder() : cTableItem(myDB::TABLE_REMINDERS)
{
    this->reset();
}

cReminder::~cReminder()
{
}

void cReminder::reset()
{
    iid = pid = rid = cid = -1;
    cTableItem::reset();
}

cReminder& cReminder::operator=(cReminder& other)
{
    if (this != &other) {
	setID(other.getID());
	iid = other.getInvoiceID();
	pid = other.getPatientID();
	rid = other.getReceipeID();
	cid = other.getCourseID();
	r_date = other.getDate();
	rdata = other.getRData();
    }
    return *this;
}

int cReminder::getInvoiceID()
{
    return iid;
}

void cReminder::setInvoiceID(int i)
{
    iid = i;
}

int cReminder::getPatientID()
{
    return pid;
}

void cReminder::setPatientID(int i)
{
    pid = i;
}

int cReminder::getReceipeID()
{
    return rid;
}

void cReminder::setReceipeID(int i)
{
    rid = i;
}

int cReminder::getCourseID()
{
    return cid;
}

void cReminder::setCourseID(int i)
{
    cid = i;
}

QDate cReminder::getDate()
{
    return r_date;
}

void cReminder::setDate(QDate t)
{
    r_date = t;
}

QByteArray& cReminder::getRData()
{
    return rdata;
}

void cReminder::setRData(QByteArray& buf)
{
    rdata = buf;
}

void cReminder::retrieve(QSqlQuery& query)
{
    setID(query.value(0).toInt());
    setDate(QDate::fromString(query.value(1).toString(), "yyyy-MM-dd"));
    setInvoiceID(query.value(2).toInt());
    setPatientID(query.value(3).toInt());
    setReceipeID(query.value(4).toInt());
    setCourseID(query.value(5).toInt());
    QByteArray qba = query.value(6).toByteArray();
    setRData(qba);
}

void cReminder::bind4store(QSqlQuery& query)
{
    query.addBindValue(getDate().toString("yyyy-MM-dd"));
    query.addBindValue(getInvoiceID());
    query.addBindValue(getPatientID());
    query.addBindValue(getReceipeID());
    query.addBindValue(getCourseID());
    query.addBindValue(getRData());
}

QString cReminder::getProperty(QString strProp)
{
    QString s = "";
    QXmlStreamReader xml(getRData());
    QString look_for = "";
    if (strProp == "Dat") look_for = "date";
    else if (strProp == "Sum") look_for = "amount";
    else if (strProp == "ZDat") look_for = "paydate";
    else if (strProp == "Adr") look_for = "addr";
    else if (strProp == "Anr") look_for = "salut";
    else if (strProp == "TPL") look_for = "tpl";
    else if (strProp == "Diag") look_for = "diagnostics";
    if (!look_for.isEmpty()) {
        while (!xml.atEnd()) {
            xml.readNext();
            if (xml.isStartElement()) {
                if (xml.name() == look_for) {
                    s = xml.readElementText();
                    break;
                }
            }
        }
    }

    return s;
}

#endif // OPA


//****************************************************************************
// cQuittance
//****************************************************************************
#ifndef OPA

cQuittance::cQuittance() : cTableItem(myDB::TABLE_QUITTANCES)
{
    this->reset();
}

cQuittance::~cQuittance()
{
}

void cQuittance::reset()
{
    pid = rid = -1;
    type = 0;
    q_nr = "";
    cTableItem::reset();
}

cQuittance& cQuittance::operator=(cQuittance& other)
{
    if (this != &other) {
	setID(other.getID());
	rid = other.getReceipeID();
	pid = other.getPatientID();
	q_date = other.getDate();
	q_nr = other.getNumber();
	type = other.getType();
	qdata = other.getQData();
    }
    return *this;
}

int cQuittance::getReceipeID()
{
    return rid;
}

void cQuittance::setReceipeID(int i)
{
    rid = i;
}

int cQuittance::getPatientID()
{
    return pid;
}

void cQuittance::setPatientID(int i)
{
    pid = i;
}

QDate cQuittance::getDate()
{
    return q_date;
}

void cQuittance::setDate(QDate t)
{
    q_date = t;
}

QString cQuittance::getNumber()
{
    return q_nr;
}

void cQuittance::setNumber(QString s)
{
    q_nr = s;
}

int cQuittance::getType()
{
    return type;
}

QString cQuittance::getTypeString()
{
    QString s = "";
    // TODO: maybe we should read purpose out of the database
    switch(getType()) {
        case Q_ARTICLE: s = "Artikel"; break;
        case Q_COURSE: s = QString::fromUtf8("Kursgebühr"); break;
        case Q_LOSS: s = QString::fromUtf8("Ausfallgebühr"); break;
        case Q_PRIVRECEIPE: s = "Privatrezept"; break;
        case Q_SELFPAY: s = "Selbstzahler"; break;
        case Q_VOUCHER: s = "Gutschein"; break;
        default: break;
    }
    return s;
}

void cQuittance::setType(int i)
{
    type = i;
}

QByteArray& cQuittance::getQData()
{
    return qdata;
}

void cQuittance::setQData(QByteArray& buf)
{
    qdata = buf;
}

void cQuittance::retrieve(QSqlQuery& query)
{
    setID(query.value(0).toInt());
    setDate(QDate::fromString(query.value(1).toString(), "yyyy-MM-dd"));
    setNumber(query.value(2).toString());
    setPatientID(query.value(3).toInt());
    setReceipeID(query.value(4).toInt());
    setType(query.value(5).toInt());
    QByteArray qba = query.value(6).toByteArray();
    setQData(qba);
}

void cQuittance::bind4store(QSqlQuery& query)
{
    query.addBindValue(getDate().toString("yyyy-MM-dd"));
    query.addBindValue(getNumber());
    query.addBindValue(getPatientID());
    query.addBindValue(getReceipeID());
    query.addBindValue(getType());
    query.addBindValue(getQData());
}

QString cQuittance::getProperty(QString strProp)
{
    QString s = "";
    QXmlStreamReader xml(getQData());
    QString look_for = "";
    if (strProp == "Pos1") look_for = "pos1";
    else if (strProp == "Pos2") look_for = "pos2";
    else if (strProp == "Sum") look_for = "amount";
    else if (strProp == "Dat") look_for = "date";
    //else if (strProp == "PATIENT") look_for = "patient";
    else if (strProp == "Z") look_for = "purpose";
    else if (strProp == "Nr") look_for = "qnr";
    else if (strProp == "TPL") look_for = "tpl";
    if (!look_for.isEmpty()) {
        while (!xml.atEnd()) {
            xml.readNext();
            if (xml.isStartElement()) {
                if (xml.name() == look_for) {
                    s = xml.readElementText();
                    break;
                }
            }
        }
    }/* else {
        if (strProp == "Dat") s = getDate().toString("dd.MM.yyyy");
    }*/

    return s;
}

#endif // OPA


//****************************************************************************
// cCourse
//****************************************************************************
#ifndef OPA

cCourse::cCourse() : cTableItem(myDB::TABLE_COURSES)
{
    this->reset();
}

cCourse::~cCourse()
{
}

void cCourse::reset()
{
    leader = -1;
    duration = units = 0;
    name = price = "";
    pidlist.clear();
    cTableItem::reset();
}

cCourse& cCourse::operator=(cCourse& other)
{
    if (this != &other) {
	setID(other.getID());
	name = other.getName();
	c_start = other.getStartDate();
	c_end = other.getEndDate();
        c_clock = other.getStartTime();
	state = other.getState();
	pidlist = other.getPidList();
	leader = other.getLeaderID();
	duration = other.getDuration();
	price = other.getPrice();
	units = other.getUnits();
    }
    return *this;
}

QString cCourse::getName()
{
    return name;
}

void cCourse::setName(QString s)
{
    name = s;
}

QDate cCourse::getStartDate()
{
    return c_start;
}

void cCourse::setStartDate(QDate t)
{
    c_start = t;
}

QTime cCourse::getStartTime()
{
    return c_clock;
}

void cCourse::setStartTime(QTime t)
{
    c_clock = t;
}

QDate cCourse::getEndDate()
{
    return c_end;
}

void cCourse::setEndDate(QDate t)
{
    c_end = t;
}

int cCourse::getState()
{
    return state;
}

void cCourse::setState(int s)
{
    state = s;
}

int cCourse::getLeaderID()
{
    return leader;
}

void cCourse::setLeaderID(int l)
{
    leader = l;
}

int cCourse::getDuration()
{
    return duration;
}

void cCourse::setDuration(int d)
{
    duration = d;
}

QString cCourse::getPrice()
{
    return price;
}

void cCourse::setPrice(QString p)
{
    price = p;
}

QList<int> cCourse::getPidList()
{
    return pidlist;
}

void cCourse::setPidList(QList<int> l)
{
    pidlist = l;
}

int cCourse::getUnits()
{
    return units;
}

void cCourse::setUnits(int u)
{
    units = u;
}

void cCourse::addPid(int pid)
{
    pidlist.append(pid);
}

void cCourse::delPidAt(int pos)
{
    pidlist.removeAt(pos);
}

void cCourse::delPid(int pid)
{
    if (pidlist.count() > 0) {
        int i = pidlist.indexOf(pid);
        if (i >= 0) pidlist.removeAt(i);
    }
}

QString cCourse::getAmount()
{
    QString s;
    float f = getPrice().toFloat() * getUnits();
    s = QString::asprintf("%.2f EUR", f);
    return s;
}

void cCourse::retrieve(QSqlQuery& query)
{
    setID(query.value(0).toInt());
    setName(query.value(1).toString());
    setStartDate(QDate::fromString(query.value(2).toString(), "yyyy-MM-dd"));
    setEndDate(QDate::fromString(query.value(3).toString(), "yyyy-MM-dd"));
    setStartTime(QTime::fromString(query.value(4).toString(), "hh:mm"));
    setUnits(query.value(5).toInt());
    setDuration(query.value(6).toInt());
    setPrice(query.value(7).toString().replace(".",",",Qt::CaseInsensitive));
    setLeaderID(query.value(8).toInt());
    QString s = query.value(9).toString();
    if (!s.isEmpty()) {
	QStringList l = s.split(":");
	for (int i = 0; i < l.count(); i++) {
	    addPid(l.at(i).toInt());
	}
    }
    setState(query.value(10).toInt());
}

void cCourse::bind4store(QSqlQuery& query)
{
    query.addBindValue(getName());
    query.addBindValue(getStartDate().toString("yyyy-MM-dd"));
    query.addBindValue(getEndDate().toString("yyyy-MM-dd"));
    query.addBindValue(getStartTime().toString("hh:mm"));
    query.addBindValue(getUnits());
    query.addBindValue(getDuration());
    query.addBindValue(getPrice().replace(",",".",Qt::CaseInsensitive));
    query.addBindValue(getLeaderID());
    // convert pids to a string (pid separated by ":")
    QStringList l;
    for (int i = 0; i < getPidList().count(); i++) {
	l << QString::number(getPidList().at(i));
    }
    query.addBindValue(l.join(":"));
    query.addBindValue(getState());
}

QString cCourse::getProperty(QString strProp)
{
    QString s = "";

    if (strProp == "Eht") {
        s = QString::number(getUnits());
    } else if (strProp == "D") {
        s = QString::number(getDuration());
    } else if (strProp == "Nam") {
        s = getName();
    } else if (strProp == "Sum") {
        float price_value = getPrice().toFloat() * (float)getUnits();
        s = QString::asprintf("%.2f", (float)(price_value));
        s = s.replace(".", ",", Qt::CaseInsensitive);
    } else if (strProp == "Beg") {
        s = getStartDate().toString("dd.MM.yyyy");
    }

    return s;
}

#endif // OPA



//****************************************************************************
// cAssoc
//****************************************************************************

cAssoc::cAssoc() : cTableItem(myDB::TABLE_ASSOC) {this->reset(); }
cAssoc::~cAssoc() {}
void cAssoc::reset() { name_short = name_long = ""; cTableItem::reset(); }
void cAssoc::setNameShort(QString s) { name_short = s; }
void cAssoc::setNameLong(QString s) { name_long = s; }
QString cAssoc::getNameShort() { return name_short; }
QString cAssoc::getNameLong() { return name_long; }
void cAssoc::retrieve(QSqlQuery& query)
{
    setID(query.value(0).toInt());
    setNameShort(query.value(1).toString());
    setNameLong(query.value(2).toString());
}
void cAssoc::bind4store(QSqlQuery& query)
{
    query.addBindValue(getNameShort());
    query.addBindValue(getNameLong());
}


//****************************************************************************
// cHMPos
//****************************************************************************
cHMPos::cHMPos() : cTableItem(myDB::TABLE_HMPOS) { this->reset(); }
cHMPos::~cHMPos() {}
void cHMPos::reset()
{
    posnr = category = descr_long = descr_short = "";
    flag_physio = flag_masseur = flag_logo = flag_ergo = 0;
    cTableItem::reset();
}
void cHMPos::setPosNr(QString s) { posnr = s; }
QString cHMPos::getPosNr() { return posnr; }
void cHMPos::setCategory(QString s) { category = s; }
QString cHMPos::getCategory() { return category; }
void cHMPos::setDescriptionLong(QString s) { descr_long = s; }
QString cHMPos::getDescriptionLong() { return descr_long; }
void cHMPos::setDescriptionShort(QString s) { descr_short = s; }
QString cHMPos::getDescriptionShort() { return descr_short; }
void cHMPos::setFlagPhysio(int i) { flag_physio = i; }
int cHMPos::getFlagPhysio() { return flag_physio; }
void cHMPos::setFlagMasseur(int i) { flag_masseur = i; }
int cHMPos::getFlagMasseur() { return flag_masseur; }
void cHMPos::setFlagLogo(int i) { flag_logo = i; }
int cHMPos::getFlagLogo() { return flag_logo; }
void cHMPos::setFlagErgo(int i) { flag_ergo = i; }
int cHMPos::getFlagErgo() { return flag_ergo; }

void cHMPos::retrieve(QSqlQuery& query)
{
    setID(query.value(0).toInt());
    setPosNr(query.value(1).toString());
    setFlagPhysio(query.value(2).toInt());
    setFlagMasseur(query.value(3).toInt());
    setFlagLogo(query.value(4).toInt());
    setFlagErgo(query.value(5).toInt());
    setCategory(query.value(6).toString());
    setDescriptionShort(query.value(7).toString());
    setDescriptionLong(query.value(8).toString());
}
void cHMPos::bind4store(QSqlQuery& query)
{
    query.addBindValue(getPosNr());
    query.addBindValue(getFlagPhysio());
    query.addBindValue(getFlagMasseur());
    query.addBindValue(getFlagLogo());
    query.addBindValue(getFlagErgo());
    query.addBindValue(getCategory());
    query.addBindValue(getDescriptionShort());
    query.addBindValue(getDescriptionLong());
}


//****************************************************************************
// cPriceTableNames
//****************************************************************************
cPriceTableNames::cPriceTableNames() : cTableItem(myDB::TABLE_PRICETABLES) { this->reset(); }
cPriceTableNames::~cPriceTableNames() {}
void cPriceTableNames::reset() { assoc = area = prof = -1; unions = name = descr = ""; cTableItem::reset(); }
void cPriceTableNames::setProfession(int i) { prof = i; }
int cPriceTableNames::getProfession() { return prof; }
void cPriceTableNames::setTariffArea(int i) { area = i; }
int cPriceTableNames::getTariffArea() { return area; }
void cPriceTableNames::setAssoc(int i) { assoc = i; }
int cPriceTableNames::getAssoc() { return assoc; }
void cPriceTableNames::setUnions(QString s) { unions = s; }
QString cPriceTableNames::getUnions() { return unions; }
void cPriceTableNames::setName(QString s) { name = s; }
QString cPriceTableNames::getName() { return name; }
void cPriceTableNames::setDescription(QString s) { descr = s; }
QString cPriceTableNames::getDescription() { return descr; }
void cPriceTableNames::setValidFrom(QDate d) { valid_from = d; }
QDate cPriceTableNames::getValidFrom() { return valid_from; }
void cPriceTableNames::retrieve(QSqlQuery& query)
{
    setID(query.value(0).toInt());
    setProfession(query.value(1).toInt());
    setTariffArea(query.value(2).toInt());
    setAssoc(query.value(3).toInt());
    setUnions(query.value(4).toString());
    setValidFrom(QDate::fromString(query.value(5).toString(), "yyyy-MM-dd"));
    setName(query.value(6).toString());
    setDescription(query.value(7).toString());
}

void cPriceTableNames::bind4store(QSqlQuery& query)
{
    query.addBindValue(getProfession());
    query.addBindValue(getTariffArea());
    query.addBindValue(getAssoc());
    query.addBindValue(getUnions());
    query.addBindValue(getValidFrom().toString("yyyy-MMd-dd"));
    query.addBindValue(getName());
    query.addBindValue(getDescription());
}


//****************************************************************************
// cPriceTableItem
//****************************************************************************
cPriceTableItem::cPriceTableItem() { pos_nr = price = addpay = ""; ref_id = -1; }
cPriceTableItem::cPriceTableItem(QString pos, QString p, QString a) { pos_nr = pos; price = p; addpay = a; ref_id = -1; }
cPriceTableItem::~cPriceTableItem() {}
void cPriceTableItem::setPosNr(QString s) { pos_nr = s; }
QString cPriceTableItem::getPosNr() { return pos_nr; }
void cPriceTableItem::setPrice(QString s) { price = s; }
QString cPriceTableItem::getPrice() { return price; }
void cPriceTableItem::setAddpay(QString s) { addpay = s; }
QString cPriceTableItem::getAddpay() { return addpay; }
void cPriceTableItem::setRefID(int i) { ref_id = i; }
int cPriceTableItem::getRefID() { return ref_id; }
QString cPriceTableItem::getPosDescr() {
    if ( (ref_id == -1) || (ref_id >= db.table_hm_pos.count()) ) return "";
    cHMPos *pos = (cHMPos*)(db.table_hm_pos.at(ref_id));
    if (pos == NULL) return "";
    return pos->getCategory() + ":" + pos->getDescriptionShort();
}
QString cPriceTableItem::getPosCategory() {
    if ( (ref_id == -1) || (ref_id >= db.table_hm_pos.count()) ) return "";
    cHMPos *pos = (cHMPos*)(db.table_hm_pos.at(ref_id));
    if (pos == NULL) return "";
    return pos->getCategory();
}

//****************************************************************************
// cPriceTable
//****************************************************************************
cPriceTable::cPriceTable() {}
cPriceTable::~cPriceTable() { clear(); }
void cPriceTable:: reset()
{
    hb_flat = false;
    id_hb = id_hb_group = id_distance = id_report = -1;
    listPosHB.clear();
    listPosHB.append(-1);
    clear();
}

bool cPriceTable::isFlatHB() { return hb_flat; }
int cPriceTable::posHB() { return id_hb; }
int cPriceTable::posHBgrp() { return id_hb_group; }
int cPriceTable::posHBdist() { return id_distance; }
int cPriceTable::posReport() { return id_report; }
QString cPriceTable::getPosNr(int i)
{
    if ( (i < 0) || (i >= items.count()) ) return "";
    return items.at(i)->getPosNr();

}
QString cPriceTable::strPosHB() { return getPosNr(id_hb); }
QString cPriceTable::strPosHBgrp() { return getPosNr(id_hb_group); }
QString cPriceTable::strPosHBdist() { return getPosNr(id_distance); }
QString cPriceTable::strPosReport() { return getPosNr(id_report); }


void cPriceTable::append(QString pos, QString pos_price, QString pos_addpay)
{
    items.append(new cPriceTableItem(pos, pos_price, pos_addpay));
}

cPriceTableItem* cPriceTable::at(int i)
{
    if ( (i < 0) || (i >= items.count()) ) return NULL;
    return items.at(i);
}

int cPriceTable::indexOf(QString pos_nr)
{
    int ret = -1;
    for (int i = 0; i < items.count(); i++) {
	if (items.at(i)->getPosNr() == pos_nr) {
	    ret = i;
	    break;
	}
    }
    return ret;
}

QString cPriceTable::getPrice(int i)
{
    if ( (i < 0) || (i >= items.count()) ) return "0.00";
    else return items.at(i)->getPrice();
}

QString cPriceTable::getAddpay(int i)
{
    if ( (i < 0) || (i >= items.count()) ) return "0.00";
    else return items.at(i)->getAddpay();
}

QStringList cPriceTable::getPositionDescriptions()
{
    QStringList list;
    for (int i = 0; i < items.count(); i++) {
	int ref_id = items.at(i)->getRefID();
	if (ref_id == -1) {
	    list << "Unbekannt:Unbekannt";
	} else {
	    cHMPos *p = (cHMPos*)(db.table_hm_pos.at(ref_id));
	    if (p != NULL) list << p->getCategory() + ":" + p->getDescriptionShort();
	}
    }
    return list;
}


QStringList cPriceTable::getPositionNumbers()
{
    QStringList list;

    for (int i = 0; i < items.count(); i++) {
	list << items.at(i)->getPosNr();
    }
    return list;
}


QStringList cPriceTable::getHBPositionNumbers()
{
    QStringList list;

    for (int i = 0; i < listPosHB.count(); i++) {
        int x = listPosHB.at(i);
        if (x < 0 || x >= items.count()) list << "";
        else list << items.at(listPosHB.at(i))->getPosNr();
    }
    return list;
}


// setup a price table: match pos_nr to global position table (table_hm_pos)
// and store matched IDs in price table for further lookups
void cPriceTable::prepare(int prof)
{
    for (int i = 0; i < items.count(); i++) {
	QString pos_nr = items.at(i)->getPosNr();
        for (int j = 0; j < db.table_hm_pos.count(); j++) {
	    cHMPos *p = (cHMPos*)(db.table_hm_pos.at(j));
	    if (p == NULL) continue;
            if (prof == cProfession::PROF_PHYSIOTHERAPY && !p->getFlagPhysio()) continue;
            if (prof == cProfession::PROF_ERGOTHERAPY && !p->getFlagErgo()) continue;
            if (p->getPosNr().right(4) == pos_nr.right(4)) {
		items.at(i)->setRefID(j);
		break;
	    }
	}

        QString pos_tmp = pos_nr.right(4);
        if (pos_tmp == "9933") { id_hb = i; hb_flat = true; }
        else if (pos_tmp == "9934") id_hb_group = i;
        else if (pos_tmp == "9701") id_report = i;
        else if (pos_tmp == "9901") id_hb = i;
        else if (pos_tmp == "9902") id_hb_group = i;
        else if (pos_tmp == "9907") id_distance = i;
        else if (pos_tmp == "9910") id_distance = i;
        else if (pos_tmp == "8602") { id_hb = i; listPosHB.append(i); }
        else if (pos_tmp == "8603") { id_distance = i; listPosHB.append(i); }

        if (pos_tmp.left(2) == "99") listPosHB.append(i);
    }
}

void cPriceTable::clear()
{
    while (!items.isEmpty()) delete items.takeFirst();
    listPosHB.clear();
    listPosHB.append(-1);
}


//****************************************************************************
// cPriceTablePriv
//****************************************************************************
cPriceTablePriv::cPriceTablePriv() { id = -1; }
cPriceTablePriv::~cPriceTablePriv() {}
void cPriceTablePriv::setID(int i) { id = i; }
int cPriceTablePriv::getID() { return id; }

void cPriceTablePriv::addItem(QString pos_name, QString pos_price)
{
    qslPosName.append(pos_name);
    QString price = pos_price;
    // check pos_price for correct syntax and adjust, if necessary
    QRegularExpression regexp;
    regexp.setPattern("^[0-9]{1,4}\\.[0-9]{2}$");
    QRegularExpressionMatch match = regexp.match(price);
    if (match.hasMatch()) {
        qslPrice.append(price);
        return;
    }
    regexp.setPattern("^[0-9]{1,4}$");
    match = regexp.match(price);
    if (match.hasMatch()) {
        qslPrice.append(price + ".00");
        return;
    }
    regexp.setPattern("^[0-9]{1,4}\\.[0-9]{1}$");
    match = regexp.match(price);
    if (match.hasMatch()) {
        qslPrice.append(price + "0");
        return;
    }
}

QString cPriceTablePriv::getPos(int i)
{
    if (i < qslPosName.count()) return qslPosName[i];
    else return "";
}
QString cPriceTablePriv::getPrice(int i)
{
    if (i < qslPrice.count()) return qslPrice[i];
    else return "";
}
void cPriceTablePriv::reset()
{
    clear();
    pt_type = 4;
    pt_version = "";
}
void cPriceTablePriv::clear()
{
    qslPosName.clear();
    qslPrice.clear();
}
int cPriceTablePriv::getType() { return pt_type; }
void cPriceTablePriv::setType(int t) { pt_type = t; }
QString cPriceTablePriv::getVersion() { return pt_version; }
void cPriceTablePriv::setVersion(QString v) { pt_version = v; }
int cPriceTablePriv::count() { return qslPosName.count(); }
QString cPriceTablePriv::getSum(int pos, int cnt)
{
    if ( (pos < 0) || (pos >= qslPrice.count()) ) return "";
    QString s = "";
    float p = qslPrice[pos].toFloat();
    p = p * cnt;
    s = QString::asprintf("%.2f", p);
    return s;
}
QStringList cPriceTablePriv::getFilteredPosNames()
{
    QStringList list = qslPosName;
    if (list.count() > 1) {
	list.removeLast();
	list.removeLast();
	// remove entries for "Hausbesuch" and "Kilometerpauschale"
	// which are always required to be at the end of the position list
    }
    /*for (int i = 0; qslPosName.count(); i++) {
	if (qslPosName.at(i) == "Hausbesuch") continue;
	if (qslPosName.at(i) == "Kilometerpauschale") continue;
	list << qslPosName;
    }*/
    return list;
}


//****************************************************************************
// cTariffScope
//****************************************************************************
cTariffScope::cTariffScope() : cTableItem(myDB::TABLE_TARIFF_SCOPES) { name = ""; scope_id = -1; cTableItem::reset(); }
void cTariffScope::setName(QString s) { name = s; }
QString cTariffScope::getName() { return name; }
void cTariffScope::setScopeID(int i) { scope_id = i; }
int cTariffScope::getScopeID() { return scope_id; }
void cTariffScope::retrieve(QSqlQuery& query)
{
    setID(query.value(0).toInt());
    setScopeID(query.value(1).toInt());
    setName(query.value(2).toString());
}
void cTariffScope::bind4store(QSqlQuery& query)
{
    query.addBindValue(getScopeID());
    query.addBindValue(getName());
}


//****************************************************************************
// cTariffArea
//****************************************************************************
cTariffArea::cTariffArea() : cTableItem(myDB::TABLE_TARIFF_AREAS) { name = ""; scope = -1; cTableItem::reset(); }
void cTariffArea::setName(QString s) { name = s; }
QString cTariffArea::getName() { return name; }
void cTariffArea::setScope(int i) { scope = i; }
int cTariffArea::getScope() { return scope; }
void cTariffArea::retrieve(QSqlQuery& query)
{
    setID(query.value(0).toInt());
    setScope(query.value(1).toInt());
    setName(query.value(2).toString());
}
void cTariffArea::bind4store(QSqlQuery& query)
{
    query.addBindValue(getScope());
    query.addBindValue(getName());
}


//****************************************************************************
// cUnion
//****************************************************************************
cUnion::cUnion() : cTableItem(myDB::TABLE_UNIONS) { prof = -1; name_short = ""; name_long = ""; cTableItem::reset(); }
void cUnion::setProfession(int i) { prof = i; }
int cUnion::getProfession() { return prof; }
void cUnion::setNameShort(QString s) { name_short = s; }
QString cUnion::getNameShort() { return name_short; }
void cUnion::setNameLong(QString s) { name_long = s; }
QString cUnion::getNameLong() { return name_long; }
void cUnion::retrieve(QSqlQuery& query)
{
    setID(query.value(0).toInt());
    setProfession(query.value(1).toInt());
    setNameShort(query.value(2).toString());
    setNameLong(query.value(3).toString());
}
void cUnion::bind4store(QSqlQuery& query)
{
    query.addBindValue(getProfession());
    query.addBindValue(getNameShort());
    query.addBindValue(getNameShort());
}


//****************************************************************************
// cProfession
//****************************************************************************
cProfession::cProfession() : cTableItem(myDB::TABLE_PROFESSIONS) { name = ""; cTableItem::reset(); }
void cProfession::setName(QString s) { name = s; }
QString cProfession::getName() { return name; }
void cProfession::retrieve(QSqlQuery& query)
{
    setID(query.value(0).toInt());
    setName(query.value(1).toString());
}
void cProfession::bind4store(QSqlQuery& query)
{
    query.addBindValue(getName());
}


//****************************************************************************
// cQualification
//****************************************************************************
cQualification::cQualification() : cTableItem(myDB::TABLE_QUALIFICATIONS) { name = ""; profession = -1; cTableItem::reset(); }
QString cQualification::getName() { return name; }
int cQualification::getProfession() { return profession; }
void cQualification::setName(QString s) { name = s; }
void cQualification::setProfession(int i) { profession = i; }
void cQualification::retrieve(QSqlQuery& query)
{
    setID(query.value(0).toInt());
    setProfession(query.value(1).toInt());
    setName(query.value(2).toString());
}
void cQualification::bind4store(QSqlQuery& query)
{
    query.addBindValue(getProfession());
    query.addBindValue(getName());
}


//****************************************************************************
// cRoom
//****************************************************************************
cRoom::cRoom() : cTableItem(myDB::TABLE_ROOMS) { name = building = notice = ""; cTableItem::reset(); }
void cRoom::setName(QString s) { name = s; }
void cRoom::setBuilding(QString s) { building = s; }
void cRoom::setNotice(QString s) { notice = s; }
QString cRoom::getName() { return name; }
QString cRoom::getBuilding() { return building; }
QString cRoom::getNotice() { return notice; }
void cRoom::retrieve(QSqlQuery& query)
{
    setID(query.value(0).toInt());
    setName(query.value(1).toString());
    setBuilding(query.value(2).toString());
    setNotice(query.value(3).toString());
}
void cRoom::bind4store(QSqlQuery& query)
{
    query.addBindValue(getName());
    query.addBindValue(getBuilding());
    query.addBindValue(getNotice());
}


//****************************************************************************
// cCar
//****************************************************************************
cCar::cCar() : cTableItem(myDB::TABLE_CARS) { name = notice = ""; cTableItem::reset(); }
void cCar::setName(QString s) { name = s; }
void cCar::setNotice(QString s) { notice = s; }
QString cCar::getName() { return name; }
QString cCar::getNotice() { return notice; }
void cCar::retrieve(QSqlQuery& query)
{
    setID(query.value(0).toInt());
    setName(query.value(1).toString());
    setNotice(query.value(2).toString());
}
void cCar::bind4store(QSqlQuery& query)
{
    query.addBindValue(getName());
    query.addBindValue(getNotice());
}


//****************************************************************************
// cInvoiceStat
//****************************************************************************
#ifndef OPA
cInvoiceStat::cInvoiceStat() : cTableItem(myDB::TABLE_INVOICE_STATS) { name = ""; cTableItem::reset(); }
void cInvoiceStat::setName(QString s) { name = s; }
QString cInvoiceStat::getName() { return name; }
void cInvoiceStat::retrieve(QSqlQuery& query)
{
    setID(query.value(0).toInt());
    setName(query.value(1).toString());
}
void cInvoiceStat::bind4store(QSqlQuery& query)
{
    query.addBindValue(getName());
}
#endif // OPA


//****************************************************************************
// cPraxisInvoiceStat
//****************************************************************************
#ifndef OPA
cPraxisInvoiceStat::cPraxisInvoiceStat() : cTableItem(myDB::TABLE_PINVOICE_STATS) { name = ""; cTableItem::reset(); }
void cPraxisInvoiceStat::setName(QString s) { name = s; }
QString cPraxisInvoiceStat::getName() { return name; }
void cPraxisInvoiceStat::retrieve(QSqlQuery& query)
{
    setID(query.value(0).toInt());
    setName(query.value(1).toString());
}
void cPraxisInvoiceStat::bind4store(QSqlQuery& query)
{
    query.addBindValue(getName());
}
#endif

//****************************************************************************
// cZipCode
//****************************************************************************
#ifndef OPA
cZipCode::cZipCode() : cTableItem(myDB::TABLE_ZIPCODES) { zip = city = ""; cTableItem::reset(); }
void cZipCode::setZip(QString s) { zip = s; }
QString cZipCode::getZip() { return zip; }
void cZipCode::setCity(QString s) { city = s; }
QString cZipCode::getCity() { return city; }
void cZipCode::retrieve(QSqlQuery& query)
{
    setID(query.value(0).toInt());
    setZip(query.value(1).toString());
    setCity(query.value(2).toString());
}
void cZipCode::bind4store(QSqlQuery& query)
{
    query.addBindValue(getZip());
    query.addBindValue(getCity());
}
#endif


//****************************************************************************
// cTextModule
//****************************************************************************
cTextModule::cTextModule() : cTableItem(myDB::TABLE_TEXT_MODULES) { tm_domain = -1; tm_text = ""; }
void cTextModule::setDomain(int i) { tm_domain = i; }
int cTextModule::getDomain() { return tm_domain; }
void cTextModule::setText(QString s) { tm_text = s; }
QString cTextModule::getText() { return tm_text; }
void cTextModule::retrieve(QSqlQuery& query)
{
    setID(query.value(0).toInt());
    setDomain(query.value(1).toInt());
    setText(query.value(2).toString());
}
void cTextModule::bind4store(QSqlQuery& query)
{
    query.addBindValue(getDomain());
    query.addBindValue(getText());
}


//****************************************************************************
// cHmCatDomain
//****************************************************************************
#ifndef OPA
cHmCatDomain::cHmCatDomain() : cTableItem(myDB::TABLE_HM_CAT_DOMAINS) { name = positions = ""; cTableItem::reset(); }
void cHmCatDomain::setName(QString s) { name = s; }
void cHmCatDomain::setPositions(QString s) { positions = s; }
QString cHmCatDomain::getName() { return name; }
QString cHmCatDomain::getPositions() { return positions; }
void cHmCatDomain::retrieve(QSqlQuery& query)
{
    setID(query.value(0).toInt());
    setName(query.value(1).toString());
    setPositions(query.value(2).toString());
}
#endif // OPA


//****************************************************************************
// cHmCat
//****************************************************************************
#ifndef OPA
cHmCat::cHmCat() : cTableItem(myDB::TABLE_HM_CAT) { this->reset(); }
void cHmCat::reset()
{
    maxcount = erstvo = folgevo = child_only = 0;
    ind = hm_primary = hm_optional = hm_additional = "";
    cTableItem::reset();
}
void cHmCat::setMaxCount(int i) { maxcount = i; }
void cHmCat::setErstVO(int i) { erstvo = i; }
void cHmCat::setFolgeVO(int i) { folgevo = i; }
void cHmCat::setIndicator(QString s) { ind = s; }
void cHmCat::setHmPrimary(QString s) { hm_primary = s; }
void cHmCat::setHmOptional(QString s) { hm_optional = s; }
void cHmCat::setHmAdditional(QString s) { hm_additional = s; }
void cHmCat::setChildOnly(int i) { child_only = i; }
int cHmCat::getMaxCount() { return maxcount; }
int cHmCat::getErstVO() { return erstvo; }
int cHmCat::getFolgeVO() { return folgevo; }
QString cHmCat::getIndicator() { return ind; }
QString cHmCat::getHmPrimary() { return hm_primary; }
QString cHmCat::getHmOptional() { return hm_optional; }
QString cHmCat::getHmAdditional() { return hm_additional; }
int cHmCat::getChildOnly() { return child_only; }
void cHmCat::retrieve(QSqlQuery& query)
{
    setID(query.value(0).toInt());
    setIndicator(query.value(1).toString());
    setHmPrimary(query.value(2).toString());
    setHmOptional(query.value(3).toString());
    setHmAdditional(query.value(4).toString());
    setMaxCount(query.value(5).toInt());
    setErstVO(query.value(6).toInt());
    setFolgeVO(query.value(7).toInt());
    setChildOnly(query.value(8).toInt());
}

#endif // OPA


//****************************************************************************
// cTemplateType
//****************************************************************************
cTemplateType::cTemplateType() : cTableItem(myDB::TABLE_TEMPLATE_TYPES) { t_name = ""; }
cTemplateType& cTemplateType::operator=(cTemplateType& other)
{
    if (this != &other) {
        setID(other.getID());
        t_name = other.getName();
    }
    return *this;
}
void cTemplateType::setName(QString s) { t_name = s; }
QString cTemplateType::getName() { return t_name; }
void cTemplateType::retrieve(QSqlQuery& query)
{
    setID(query.value(0).toInt());
    setName(query.value(1).toString());
}


//****************************************************************************
// cTemplateDataCategory
//****************************************************************************
cTemplateDataCategory::cTemplateDataCategory() : cTableItem(myDB::TABLE_TEMPLATE_DATA_CATEGORIES) { c_name = c_shortname = c_fields = ""; }
cTemplateDataCategory& cTemplateDataCategory::operator=(cTemplateDataCategory& other)
{
    if (this != &other) {
        setID(other.getID());
        c_name = other.getName();
        c_shortname = other.getShortName();
        c_fields = other.getFields();
    }
    return *this;
}
void cTemplateDataCategory::setName(QString s) { c_name = s; }
QString cTemplateDataCategory::getName() { return c_name; }
void cTemplateDataCategory::setShortName(QString s) { c_shortname = s; }
QString cTemplateDataCategory::getShortName() { return c_shortname; }
void cTemplateDataCategory::setFields(QString s) { c_fields = s; }
QString cTemplateDataCategory::getFields() { return c_fields; }
void cTemplateDataCategory::retrieve(QSqlQuery& query)
{
    setID(query.value(0).toInt());
    setName(query.value(1).toString());
    setShortName(query.value(2).toString());
    setFields(query.value(3).toString());
}


//****************************************************************************
// cTemplateDataMapping
//****************************************************************************
cTemplateDataMapping::cTemplateDataMapping() : cTableItem(myDB::TABLE_TEMPLATE_DATA_MAPPINGS) { c_name = c_shortname = ""; }
cTemplateDataMapping& cTemplateDataMapping::operator=(cTemplateDataMapping& other)
{
    if (this != &other) {
        setID(other.getID());
        c_name = other.getName();
        c_shortname = other.getShortName();
    }
    return *this;
}
void cTemplateDataMapping::setName(QString s) { c_name = s; }
QString cTemplateDataMapping::getName() { return c_name; }
void cTemplateDataMapping::setShortName(QString s) { c_shortname = s; }
QString cTemplateDataMapping::getShortName() { return c_shortname; }
void cTemplateDataMapping::retrieve(QSqlQuery& query)
{
    setID(query.value(0).toInt());
    setShortName(query.value(1).toString());
    setName(query.value(2).toString());
}


//****************************************************************************
// cTemplateAvailDataCategory
//****************************************************************************
cTemplateAvailDataCategory::cTemplateAvailDataCategory() : cTableItem(myDB::TABLE_TEMPLATE_AVAIL_DATA_CATEGORIES)
{
    tmpl_type = -1;
    avail = "";
}
cTemplateAvailDataCategory& cTemplateAvailDataCategory::operator=(cTemplateAvailDataCategory& other)
{
    if (this != &other) {
        setID(other.getID());
        tmpl_type = other.getTemplateType();
        avail = other.getAvail();
    }
    return *this;
}
void cTemplateAvailDataCategory::setTemplateType(int i) { tmpl_type = i; }
int cTemplateAvailDataCategory::getTemplateType() { return tmpl_type; }
void cTemplateAvailDataCategory::setAvail(QString s) { avail = s; }
QString cTemplateAvailDataCategory::getAvail() { return avail; }
void cTemplateAvailDataCategory::retrieve(QSqlQuery& query)
{
    setID(query.value(0).toInt());
    setTemplateType(query.value(1).toInt());
    setAvail(query.value(2).toString());
}


//****************************************************************************
// cTemplate
//****************************************************************************
cTemplate::cTemplate() : cTableItem(myDB::TABLE_TEMPLATES)
{
    tmpl_type = -1;
    tmpl_subtype = 0;
    tmpl_flags = tmpl_ver = 0;
    tmpl_name = tmpl_hash = "";
    contents.clear();
}
void cTemplate::setTemplateType(int i) { tmpl_type = i; }
int cTemplate::getTemplateType() { return tmpl_type; }
void cTemplate::setTemplateSubtype(int i) { tmpl_subtype = i; }
int cTemplate::getTemplateSubtype() { return tmpl_subtype; }
void cTemplate::setTemplateVersion(int i) { tmpl_ver = i; }
int cTemplate::getTemplateVersion() { return tmpl_ver; }
void cTemplate::setTemplateFlags(int i) { tmpl_flags = i; }
int cTemplate::getTemplateFlags() { return tmpl_flags; }
void cTemplate::setHash(QString s) { tmpl_hash = s; }
QString cTemplate::getHash() { return tmpl_hash; }
void cTemplate::setName(QString s) { tmpl_name = s; }
QString cTemplate::getName() { return tmpl_name; }
void cTemplate::setContents(QByteArray qba) { contents = qba;}
QByteArray cTemplate::getContents() { return contents; }
void cTemplate::retrieve(QSqlQuery& query)
{
    setID(query.value(0).toInt());
    setTemplateType(query.value(1).toInt());
    setTemplateSubtype(query.value(2).toInt());
    setTemplateVersion(query.value(3).toInt());
    setTemplateFlags(query.value(4).toInt());
    setHash(query.value(5).toString());
    setName(query.value(6).toString());
    setContents(query.value(7).toByteArray());
}
void cTemplate::bind4store(QSqlQuery& query)
{
    query.addBindValue(getTemplateType());
    query.addBindValue(getTemplateSubtype());
    query.addBindValue(getTemplateVersion());
    query.addBindValue(getTemplateFlags());
    query.addBindValue(getHash());
    query.addBindValue(getName());
    query.addBindValue(getContents());
}
cTemplate& cTemplate::operator=(cTemplate& other)
{
    if (this != &other) {
        tmpl_type = other.getTemplateType();
        tmpl_subtype = other.getTemplateSubtype();
        tmpl_ver = other.getTemplateVersion();
        tmpl_flags = other.getTemplateFlags();
        tmpl_hash = other.getHash();
        tmpl_name = other.getName();
        contents = other.getContents();
        setID(other.getID());
    }
    return *this;
}

//****************************************************************************
// cProtectedFunc
//****************************************************************************
cProtectedFunc::cProtectedFunc() : cTableItem(myDB::TABLE_PROTECT) { name = ridlist = ""; cTableItem::reset(); }
void cProtectedFunc::setName(QString s) { name = s; }
QString cProtectedFunc::getName() { return name; }
void cProtectedFunc::setRidList(QString s) { ridlist = s; }
QString cProtectedFunc::getRidList() { return ridlist; }
void cProtectedFunc::retrieve(QSqlQuery& query)
{
    setID(query.value(0).toInt());
    setName(query.value(1).toString());
    setRidList(query.value(2).toString());
}
void cProtectedFunc::bind4store(QSqlQuery& query)
{
    query.addBindValue(getName());
    query.addBindValue(getRidList());
}


//****************************************************************************
// cDatabase
//****************************************************************************
cDatabase::cDatabase()
{
    reset();
}

cDatabase::~cDatabase()
{
}

void cDatabase::reset()
{
    driver = name = file = host = user = pw = "";
}

QString cDatabase::getDriver()
{
    return driver;
}

void cDatabase::setDriver(QString s)
{
    driver = s;
}

QString cDatabase::getName()
{
    return name;
}

void cDatabase::setName(QString s)
{
    name = s;
}

QString cDatabase::getFile()
{
    return file;
}

void cDatabase::setFile(QString s)
{
    file = s;
}

QString cDatabase::getHost()
{
    return host;
}

void cDatabase::setHost(QString s)
{
    host = s;
}

QString cDatabase::getUser()
{
    return user;
}

void cDatabase::setUser(QString s)
{
    user = s;
}

QString cDatabase::getPW()
{
    return pw;
}

void cDatabase::setPW(QString s)
{
    pw = s;
}

cDatabase& cDatabase::operator=(cDatabase& other)
{
    if (this != &other) {
	driver = other.getDriver();
	name = other.getName();
	file = other.getFile();
	host = other.getHost();
	user = other.getUser();
	pw = other.getPW();
    }
    return *this;
}

int cDatabase::getType()
{
    int db_type = -1;
    if (driver == "QSQLITE") db_type = DB_SQLITE;
    else if (driver == "QMYSQL") db_type = DB_MYSQL;
    return db_type;
}


//****************************************************************************
// cReaderDevice
//****************************************************************************
cReaderDevice::cReaderDevice() : cTableItem(myDB::TABLE_READER_DEVICES) { reset(); }
cReaderDevice::~cReaderDevice() {}
void cReaderDevice::reset()
{
    name = "";
    lib = "";
    api = CTAPI;
    term = 1;
    port = 1;
    readofs = 0;
    sel = false;
    cTableItem::reset();
}
cReaderDevice& cReaderDevice::operator=(cReaderDevice& other)
{
    if (this != &other) {
	setID(other.getID());
	name = other.getName();
	lib = other.getLib();
	api = other.getAPI();
	term = other.getTerminal();
	port = other.getPort();
	readofs = other.getReadOffset();
	sel = other.getSelect();
    }
    return *this;
}
void cReaderDevice::setName(QString s) { name = s; }
void cReaderDevice::setLib(QString s) { lib = s; }
void cReaderDevice::setAPI(int i) { api = i; }
void cReaderDevice::setTerminal(unsigned short i) { term = i; }
void cReaderDevice::setPort(unsigned short i) { port = i; }
void cReaderDevice::setReadOffset(unsigned i) { readofs = i; }
void cReaderDevice::setSelect(bool b) { sel = b; }
void cReaderDevice::setPropCTAPI(int t, int p, int ofs, bool s) { term = t; port = p; readofs = ofs; sel = s; }
QString cReaderDevice::getName() { return name; }
QString cReaderDevice::getLib() { return lib; }
int cReaderDevice::getAPI() { return api; }
unsigned short cReaderDevice::getTerminal() { return term; }
unsigned short cReaderDevice::getPort() { return port; }
unsigned short cReaderDevice::getReadOffset() { return readofs; }
bool cReaderDevice::getSelect() { return sel; }
void cReaderDevice::getPropCTAPI(int *t, int *p, int *ofs, bool *sel) { *t = term; *p = port; *ofs = readofs; *sel = sel; }
void cReaderDevice::retrieve(QSqlQuery& query)
{
    setID(query.value(0).toInt());
    setName(query.value(1).toString());
    setAPI(query.value(2).toInt());
    setLib(query.value(3).toString());
    setTerminal(query.value(4).toInt());
    setPort(query.value(5).toInt());
    setSelect(query.value(6).toInt());
    setReadOffset(query.value(7).toInt());
}
void cReaderDevice::bind4store(QSqlQuery& query)
{
    query.addBindValue(getName());
    query.addBindValue(getAPI());
    query.addBindValue(getLib());
    query.addBindValue(getTerminal());
    query.addBindValue(getPort());
    query.addBindValue(getSelect());
    query.addBindValue(getReadOffset());
}

#ifndef OPA
#ifdef USE_ASYNC_DB_ACCESS

//****************************************************************************
// QueryQueueElement
//****************************************************************************
QueryQueueElement::QueryQueueElement() { model = NULL; query = ""; }
QueryQueueElement::QueryQueueElement(const QueryQueueElement &other) { model = other.model; query = other.query; }
QueryQueueElement::~QueryQueueElement() {}
QueryQueueElement& QueryQueueElement::operator=(const QueryQueueElement& other)
{
    if (this != &other) { model = other.model; query = other.query; }
    return *this;
}


//****************************************************************************
// QueryThread
//****************************************************************************
QQueue<QueryQueueElement> queue;
QSemaphore sema;

QueryThread::QueryThread(QObject* parent)
{
    running = true;
}

bool QueryThread::init(cDatabase& d)
{
    bool bRet = true;

    db_handle = QSqlDatabase::addDatabase(d.getDriver(), "threaddb");
    if (!db_handle.isOpen()) {
	if (d.getDriver() == "QSQLITE") {
            QFile f;
            QString s = myConf.getConfigFilePath() + d.getFile();
            if (f.exists(s)) db_handle.setDatabaseName(s);
            else if (f.exists(d.getFile())) db_handle.setDatabaseName(d.getFile());
            else bRet = false;
            if (!bRet) return false;
	} else {
	    db_handle.setHostName(d.getHost());
	    db_handle.setDatabaseName(d.getName());
	    db_handle.setUserName(d.getUser());
	    db_handle.setPassword(d.getPW());
	}
	if (!db_handle.open()) {
	    bRet = false;
	    //qDebug() << db_handle.lastError();
	}
    }

    if (!running) return false;
    return bRet;
}


void QueryThread::run()
{
    while (running) {
        sema.acquire();
        if (!running) break;
        QueryQueueElement q = queue.dequeue();
        //qDebug() << "Executing:" << q.query;
        //qDebug() << "Start at:" << QTime::currentTime().toString("hh:mm:ss");
        QSqlQuery query(q.query, db_handle);
        QList<QSqlRecord> records;
        if (query.exec()) {
            //qDebug() << "End at:" << QTime::currentTime().toString("hh:mm:ss");
            // prepare results
            while (query.next()) {
                records.append(query.record());
            }
            //qDebug() << "Records parsing finished at:" << QTime::currentTime().toString("hh:mm:ss");
            if (!q.model.isNull()) q.model->dataArrivedFromThread(records, true);
            //else qDebug() << "MODEL does not exist!!!";
        } else {
            getError(q.query, query.lastError());
            // Even if an error occured we need to send a signal to the waiting model/tabel
            // e.g. in order to stop the load animation.
            if (!q.model.isNull()) q.model->dataArrivedFromThread(records, false);
        }
    }

    if (db_handle.isOpen()) {
        db_handle.close();
    }

}


void QueryThread::stop()
{
    running = false;
    sema.release();
}


void QueryThread::getError(QString qs, QSqlError e)
{
    if (myConf.isDbg()) {
	if (e.isValid()) {
	    qDebug() << "Error:";
	    qDebug() << e.text();
	    qDebug() << "Query:";
	    qDebug() << qs;
	}
    }
}

void QueryThread::slotExecQuery(CustomSqlModel* model, const QString& query)
{
    QueryQueueElement q;
    q.model = model;
    q.query = query;
    queue.enqueue(q);
    sema.release();
}

#endif // USE_ASYNC_DB_ACCESS

#endif // OPA



//****************************************************************************
//****************************************************************************
// myDB
//****************************************************************************
//****************************************************************************

#define QUERY_AND_CHECK() \
    if ( query.exec() == false) { getError(query.executedQuery(), query.lastError()); return false; }


myDB::myDB()
{
    QStringList listHeaders, listItems, listTableNames, listConditions;
    for (int i = 0; i < TABLE_DUMMY; i++) {
	listTables.append(new cTable());
    }

    listTables[TABLE_FINDINGS]->init("findings");
    *listTables[TABLE_FINDINGS] << "fdate" << "pid" << "eid" << "fdata";
    listHeaders << "Datum" << "Name" << "Vorname" << "Verfasser";
    listItems << "findings.id" << "fdate" << "patients.surname" << "patients.forename" << "employees.shortname" << "findings.pid";
    listTableNames << "findings, patients, employees";
    listConditions << "WHERE findings.pid=patients.id AND findings.eid=employees.id";
    listTables[TABLE_FINDINGS]->setTableProperties(&listHeaders, &listTableNames, &listItems, &listConditions);
    listHeaders.clear(); listItems.clear(); listTableNames.clear(); listConditions.clear();

    listTables[TABLE_REPORTS]->init("reports");
    *listTables[TABLE_REPORTS] << "r_date" << "r_type" << "pid" << "rid" << "phys_id" << "eid" << "rdata";
    listHeaders << "Datum" << "Name" << "Vorname";
    listItems << "reports.id" << "r_date" << "patients.surname" << "patients.forename" << "reports.pid" << "reports.rid";
    listTableNames << "reports, patients";
    listConditions << "WHERE reports.pid=patients.id";
    listTables[TABLE_REPORTS]->setTableProperties(&listHeaders, &listTableNames, &listItems, &listConditions);
    listHeaders.clear(); listItems.clear(); listTableNames.clear(); listConditions.clear();

    listTables[TABLE_REMINDERS]->init("reminders");
    *listTables[TABLE_REMINDERS] << "r_date" << "iid" << "pid" << "rid" << "cid" << "rdata";
    listHeaders << "Datum" << "Name" << "Vorname" << "Rechnungsnr.";
    listItems << "reminders.id" << "r_date" << "patients.surname" << "patients.forename" << "invoices.i_nr" << "reminders.pid" << "reminders.rid" << "reminders.cid";
    listTableNames << "reminders, patients, invoices";
    listConditions << "WHERE reminders.pid=patients.id AND reminders.iid=invoices.id";
    listTables[TABLE_REMINDERS]->setTableProperties(&listHeaders, &listTableNames, &listItems, &listConditions);
    listHeaders.clear(); listItems.clear(); listTableNames.clear(); listConditions.clear();

    listTables[TABLE_QUITTANCES]->init("quittances");
    *listTables[TABLE_QUITTANCES] << "q_date" << "q_nr" << "pid" << "rid" << "type" << "qdata";
    listHeaders << "Datum" << "Quittungsnr." << "Name" << "Vorname" << "Quittungstyp";
    listItems << "quittances.id" << "q_date" << "q_nr" << "patients.surname" << "patients.forename" << "quittance_types.qdesc" << "quittances.pid" << "quittances.rid";
    listTableNames << "quittances, patients, quittance_types";
    listConditions << "WHERE quittances.pid=patients.id AND quittances.type=quittance_types.id";
    listTables[TABLE_QUITTANCES]->setTableProperties(&listHeaders, &listTableNames, &listItems, &listConditions);
    listHeaders.clear(); listItems.clear(); listTableNames.clear(); listConditions.clear();

    listTables[TABLE_COURSES]->init("courses");
    *listTables[TABLE_COURSES] << "name" << "c_start" << "c_end" << "c_clock" << "units" << "duration" << "price" << "leader" << "pidlist" << "state";
    listHeaders << "Kursbeginn" << "Kursname" << "Status";
    listItems << "courses.id" << "c_start" << "name" << "receipe_states.sdesc";
    listTableNames << "courses, receipe_states";
    listConditions << "WHERE courses.state=receipe_states.id";
    listTables[TABLE_COURSES]->setTableProperties(&listHeaders, &listTableNames, &listItems, &listConditions);
    listHeaders.clear(); listItems.clear(); listTableNames.clear(); listConditions.clear();

    listTables[TABLE_INVOICES]->init("invoices");
    *listTables[TABLE_INVOICES] << "i_date" << "i_nr" << "amount" << "pid" << "rid" << "cid" << "xid" << "type" << "paystat" << "invdata" << "sdate" << "edate" << "maildate" << "rlist";
    listHeaders << "Datum" << "Rechnungsnr." << "Name" << "Vorname" << "Rechnungstyp" << "Status";
    listItems << "invoices.id" << "i_date" << "i_nr" << "patients.surname" << "patients.forename" << "invoice_types.tdesc" << "invoice_status.sdesc" << "invoices.pid" << "invoices.rid" << "invoices.cid";
    listTableNames << "invoices, invoice_types, invoice_status, patients";
    listConditions << "WHERE invoices.pid=patients.id AND invoices.type=invoice_types.id AND invoices.paystat=invoice_status.id";
    listTables[TABLE_INVOICES]->setTableProperties(&listHeaders, &listTableNames, &listItems, &listConditions);
    listHeaders.clear(); listItems.clear(); listTableNames.clear(); listConditions.clear();

    listTables[TABLE_PINVOICES]->init("invoices");
    *listTables[TABLE_PINVOICES] << "i_date" << "i_nr" << "amount" << "pid" << "rid" << "cid" << "xid" << "type" << "paystat" << "invdata" << "sdate" << "edate" << "maildate" << "rlist" ;
    listHeaders << "Datum" << "Rechnungsnr." << "Praxis" << "Inhaber" << "Rechnungstyp" << "Status";
    listItems << "invoices.id" << "i_date" << "i_nr" << "praxis.name1" << "praxis.name2" << "invoice_types.tdesc" << "invoice_status.sdesc"  << "invoices.xid";
    listTableNames << "invoices, invoice_types, invoice_status, praxis";
    listConditions << "WHERE invoices.xid=praxis.id AND invoices.type=invoice_types.id AND invoices.paystat=invoice_status.id";
    listTables[TABLE_PINVOICES]->setTableProperties(&listHeaders, &listTableNames, &listItems, &listConditions);
    listHeaders.clear(); listItems.clear(); listTableNames.clear(); listConditions.clear();

    QDate datum = QDate::currentDate();
    int16_t zielJahr = datum.year() + (datum.month() >= 8 ? 1 : 0);
    listTables[TABLE_PATIENTS]->init("patients");
    *listTables[TABLE_PATIENTS] << "surname" << "forename" << "addname" << "title" << "birthday" << "gender" << "street" << "city" << "zip" << "tel" << "tel_job" << "mobile" << "fax" << "email" << "insurance" << "vnr" << "status" << "kvknr" << "kvkvalid" << "notices" << "itype" << "pgroup" << "care_facility" << "distance" << "def_eid" << "def_pid" << "state" << "addpay_freed" << "freed_from" << "freed_until" << "contract_date" << "plan_date";
    listHeaders << "Name" << "Vorname" << "Straße" << "Wohnort" << "PLZ" << "Tel. privat" << "Tel. beruf" << "Handy" << "Mail" << "Vertrag" << "Plan " + QString::number(zielJahr);
    listItems << "id" << "surname" << "forename" << "street" << "city" << "zip" << "tel" << "tel_job" << "mobile" << "email" << "contract_date" << "plan_date";
    listTableNames << "patients";
    listTables[TABLE_PATIENTS]->setTableProperties(&listHeaders, &listTableNames, &listItems, &listConditions);
    listHeaders.clear(); listItems.clear(); listTableNames.clear(); listConditions.clear();

    listTables[TABLE_EMPLOYEES]->init("employees");
    *listTables[TABLE_EMPLOYEES] << "surname" << "forename" << "street" << "city" << "zip" << "tel" << "mobile" << "fax" << "email" << "birthday" << "shortname" << "quali" << "etype" << "grp" << "tgrp" << "flag" << "gender" << "persnr" << "state";
    listHeaders << "Name" << "Vorname" << "Strasse" << "Wohnort" <<  "Telefon" << "Handy" << /* QString::fromStdString */("Kürzel") << "Status";
    listItems << "employees.id" << "surname" << "forename" << "street" << "city" << "tel" << "mobile" << "shortname" << "descr";
    listTableNames << "employees, employee_states";
    listConditions << "WHERE employees.state=employee_states.id";
    listTables[TABLE_EMPLOYEES]->setTableProperties(&listHeaders, &listTableNames, &listItems, &listConditions);
    listHeaders.clear(); listItems.clear(); listTableNames.clear(); listConditions.clear();

    listTables[TABLE_THERAPY_GROUPS]->init("therapy_groups");
    *listTables[TABLE_THERAPY_GROUPS] << "t_name" << "t_desc";

    listTables[TABLE_EMPLOYEE_GROUPS]->init("employee_groups");
    *listTables[TABLE_EMPLOYEE_GROUPS] << "tdesc";

    listTables[TABLE_EMPLOYEE_TYPES]->init("employee_types");
    *listTables[TABLE_EMPLOYEE_TYPES] << "tdesc";

    listTables[TABLE_PHYSICIANS]->init("physicians");
    *listTables[TABLE_PHYSICIANS] << "surname" << "forename" << "title" << "gender" << "surgery" << "street" << "city" << "zip" << "tel" << "mobile" << "fax" << "email" << "ptype" << "bsnr" << "lanr" << "clinic";
    listHeaders << "Name" << "Vorname" << "Praxisname" << "Strasse" << "Ort" <<  "Telefon";
    listItems << "id" << "surname" << "forename" << "surgery" << "street" << "city" << "tel";
    listTableNames << "physicians";
    listTables[TABLE_PHYSICIANS]->setTableProperties(&listHeaders, &listTableNames, &listItems, &listConditions);
    listHeaders.clear(); listItems.clear(); listTableNames.clear(); listConditions.clear();

    listTables[TABLE_PHYSICIAN_TYPES]->init("physician_types");
    *listTables[TABLE_PHYSICIAN_TYPES] << "descr";

    listTables[TABLE_PRAXIS]->init("praxis");
    *listTables[TABLE_PRAXIS] << "name1" << "name2" << "surname" << "forename" << "title" << "gender" << "street" << "city" << "zip" << "tel" << "mobile" << "fax" << "email" << "web" << "prof" << "tariff_area" << "ik" << "eid" << "ppart" ;
    listHeaders << "Praxisname" << "Inhaber" << "Straße" << "Ort" <<  "Telefon" << "Mobil";
    listItems << "id" << "name1" << "name2" << "street" << "city" << "tel" << "mobile";
    listTableNames << "praxis";
    listTables[TABLE_PRAXIS]->setTableProperties(&listHeaders, &listTableNames, &listItems, &listConditions);
    listHeaders.clear(); listItems.clear(); listTableNames.clear(); listConditions.clear();

    listTables[TABLE_INSURANCES]->init("insurances");
    *listTables[TABLE_INSURANCES] << "ik" << "name_short" << "assoc" << "vdt" << "fkt" << "kto" << "vkg" << "nam" << "ans1" << "ans2" << "ans3" << "asp1" << "asp2" << "asp3" << "uem1" << "uem2" << "dfu1" << "dfu2";
    listHeaders << "Name" << "IK" << QString::fromUtf8("Zugehörigkeit");
    listItems << "insurances.id" << "insurances.name_short" << "ik" << "associations.name_short" << "insurances.assoc";
    listTableNames << "insurances, associations";
    listConditions << "WHERE insurances.assoc=associations.id";
    listTables[TABLE_INSURANCES]->setTableProperties(&listHeaders, &listTableNames, &listItems, &listConditions);
    listHeaders.clear(); listItems.clear(); listTableNames.clear(); listConditions.clear();

    listTables[TABLE_INSURANCE_TYPES]->init("ins_type");
    *listTables[TABLE_INSURANCE_TYPES] << "name";

    listTables[TABLE_RECEIPES]->init("receipes");
    *listTables[TABLE_RECEIPES] << "type" << "state" << "pid" << "physician" << "insurance" << "pricetab" << "pt_version" << "rdate" << "sdate" << "ind" << "diagnosis" << "flag_charge" << "flag_visit" << "flag_group" << "flag_accident" << "flag_report" << "distance" << "tos" << "positions" << "brutto" << "addpay" << "oflags" << "addpay_flag" << "addpay_date" << "bill_date" << "hb_count" << "hb_pos_flat" << "hb_pos_dist" << "prof" << "notes";
    listHeaders << "Datum" << "Rezepttyp" << "Name" << "Vorname" << "Rezeptwert" << "Status";
    listItems << "receipes.id" << "receipes.rdate" << "receipe_types.tdesc" << "patients.surname" << "patients.forename" << "receipes.brutto" << "receipe_states.sdesc" << "receipes.pid";
    listTableNames << "receipes, receipe_types, receipe_states, patients";
    listConditions << "WHERE receipes.pid=patients.id AND receipes.type=receipe_types.id AND receipes.state=receipe_states.id";
    listTables[TABLE_RECEIPES]->setTableProperties(&listHeaders, &listTableNames, &listItems, &listConditions);
    listHeaders.clear(); listItems.clear(); listTableNames.clear(); listConditions.clear();

    listTables[TABLE_RECEIPE_STATES]->init("receipe_states");
    *listTables[TABLE_RECEIPE_STATES] << "sdesc";

    listTables[TABLE_ROUTES]->init("routes");
    *listTables[TABLE_ROUTES] << "route" << "distance";
    listHeaders << "Route" << "Entfernung";
    listItems << "id" << "route" << "distance";
    listTableNames << "routes";
    listTables[TABLE_ROUTES]->setTableProperties(&listHeaders, &listTableNames, &listItems, &listConditions);
    listHeaders.clear(); listItems.clear(); listTableNames.clear(); listConditions.clear();

    listTables[TABLE_ROLES]->init("roles");
    *listTables[TABLE_ROLES] << "name" << "pwhash";

    listTables[TABLE_PROTECT]->init("protected_functions");
    *listTables[TABLE_PROTECT] << "name" << "ridlist";

    listTables[TABLE_ROOMS]->init("rooms");
    *listTables[TABLE_ROOMS] << "r_name" << "r_building" << "r_notice";

    listTables[TABLE_CARS]->init("cars");
    *listTables[TABLE_CARS] << "c_name" << "c_notice";

    listTables[TABLE_DATES]->init("dates");
    *listTables[TABLE_DATES] << "d_date" << "d_start" << "d_end" << "d_type" << "d_title" << "d_desc" << "d_content"  << "flag_fd" << "flag_act_physio" << "flag_act_ergo" << "flag_option" << "room" << "car" << "stat" << "mid" << "eid" << "pid" << "rid" << "cid";

    listTables[TABLE_SURGERY]->init("surgery");
    *listTables[TABLE_SURGERY] << "name1" <<  "name2" << "street" << "city" << "zip" << "tel" << "mobile" << "fax" << "email" << "web" << "server" << "name" << "password" <<"prof" << "tariff_area" << "ik_physio" << "ik_ergo" << "ik_logo" << "bank" << "bankcode" << "account" << "s_union" << "taxid" << "sales_taxid" << "hrb" << "tax_liability" << "tax_percent";

    listTables[TABLE_SURGERY_BRANCHES]->init("surgery_branches");
    *listTables[TABLE_SURGERY_BRANCHES] << "name1" <<  "name2" << "street" << "city" << "zip" << "tel" << "mobile" << "fax" << "ik";

    listTables[TABLE_PRICETABLES]->init("price_tables");
    *listTables[TABLE_PRICETABLES] << "prof" << "area" << "assoc" << "unions" << "valid_from" << "name" << "descr";

    listTables[TABLE_INDICATORS]->init("indicators");
    *listTables[TABLE_INDICATORS] << "ind";

    listTables[TABLE_TARIFF_AREAS]->init("tariff_areas");
    *listTables[TABLE_TARIFF_AREAS] << "scope" << "name";

    listTables[TABLE_TARIFF_SCOPES]->init("tariff_scopes");
    *listTables[TABLE_TARIFF_SCOPES] << "scope_id" << "name";

    listTables[TABLE_UNIONS]->init("unions");
    *listTables[TABLE_UNIONS] << "prof" << "name_short" << "name_long";

    listTables[TABLE_ASSOC]->init("associations");
    *listTables[TABLE_ASSOC] << "name_short" << "name_long";

    listTables[TABLE_PROFESSIONS]->init("profession");
    *listTables[TABLE_PROFESSIONS] << "name";

    listTables[TABLE_INVOICE_STATS]->init("invoice_status");
    *listTables[TABLE_INVOICE_STATS] << "sdesc";

    listTables[TABLE_PINVOICE_STATS]->init("invoice_status");
    *listTables[TABLE_PINVOICE_STATS] << "sdesc";

#ifdef Q_WS_WIN
    listTables[TABLE_READER_DEVICES]->init("readers_win");
#elif defined Q_OS_LINUX
    listTables[TABLE_READER_DEVICES]->init("readers_lin");
#else
    listTables[TABLE_READER_DEVICES]->init("readers_mac");
#endif
    *listTables[TABLE_READER_DEVICES] << "name" << "api" << "lib" << "term" << "port" << "sel" << "readofs";

    // FIXME: The current version of the HM position directory needs to be read from the database.
    //        However, this will only apply if a new version of that directory is published.
    listTables[TABLE_HMPOS]->init("hm_pos_20060920");
    *listTables[TABLE_INVOICE_STATS] << "posnr" << "flag_physio" << "flag_masseur" << "flag_logo" << "flag_ergo" << "category" << "descr_short" << "descr_long";

    *listTables[TABLE_PINVOICE_STATS] << "posnr" << "flag_physio" << "flag_masseur" << "flag_logo" << "flag_ergo" << "category" << "descr_short" << "descr_long";

    listTables[TABLE_CASHPOS]->init("cash_positions");
    *listTables[TABLE_CASHPOS] << "cp_desc";

    listTables[TABLE_CASH]->init("cash");
    *listTables[TABLE_CASH] << "cpid" << "cdesc" << "paydate" << "amount" << "pid" << "rid" << "cid";
    listHeaders << "Datum" << "Position" << "Betrag" << "Beschreibung";
    listItems << "cash.id" << "paydate" << "cash_positions.cp_desc" << "amount" << "cdesc";
    listTableNames << "cash, cash_positions";
    listConditions << "WHERE cash.cpid=cash_positions.id";
    listTables[TABLE_CASH]->setTableProperties(&listHeaders, &listTableNames, &listItems, &listConditions);
    listHeaders.clear(); listItems.clear(); listTableNames.clear(); listConditions.clear();

    listTables[TABLE_CASH_ADDPAY]->init("receipes");
    *listTables[TABLE_CASH_ADDPAY] << "type" << "state" << "pid" << "physician" << "insurance" << "pricetab" << "pt_version" << "rdate" << "ind" << "diagnosis" << "flag_charge" << "flag_visit" << "flag_group" << "flag_accident" << "flag_report" << "distance" << "tos" << "positions" << "brutto" << "addpay" << "oflags" << "addpay_flag" << "addpay_date" << "bill_date" << "hb_count";
    listHeaders << "Rezeptdatum" << "Name" << "Vorname" << "Rezeptwert" << "Offene Zuzahlung";
    listItems << "receipes.id" << "receipes.rdate" << "patients.surname" << "patients.forename" << "receipes.brutto" << "receipes.addpay" << "receipes.pid";
    listTableNames << "receipes, patients";
    listConditions << "WHERE receipes.type=0 AND receipes.flag_charge=1 AND receipes.addpay_flag=0 AND receipes.pid=patients.id";
    listTables[TABLE_CASH_ADDPAY]->setTableProperties(&listHeaders, &listTableNames, &listItems, &listConditions);
    listHeaders.clear(); listItems.clear(); listTableNames.clear(); listConditions.clear();

    listTables[TABLE_CASH_SELFPAY]->init("receipes");
    *listTables[TABLE_CASH_SELFPAY] << "type" << "state" << "pid" << "physician" << "insurance" << "pricetab" << "pt_version" << "rdate" << "ind" << "diagnosis" << "flag_charge" << "flag_visit" << "flag_group" << "flag_accident" << "flag_report" << "distance" << "tos" << "positions" << "brutto" << "addpay" << "oflags" << "addpay_flag" << "addpay_date" << "bill_date" << "hb_count";
    listHeaders << "Datum" << "Name" << "Vorname" << "Rezeptwert" << "Status";
    listItems << "receipes.id" << "receipes.rdate" << "patients.surname" << "patients.forename" << "receipes.brutto" << "receipe_states.sdesc" << "receipes.pid";
    listTableNames << "receipes, receipe_states, patients";
    listConditions << "WHERE receipes.type=4 AND receipes.state < 3 AND receipes.state=receipe_states.id AND receipes.pid=patients.id";
    listTables[TABLE_CASH_SELFPAY]->setTableProperties(&listHeaders, &listTableNames, &listItems, &listConditions);
    listHeaders.clear(); listItems.clear(); listTableNames.clear(); listConditions.clear();

    listTables[TABLE_CASH_PRIVRECEIPE]->init("receipes");
    *listTables[TABLE_CASH_PRIVRECEIPE] << "type" << "state" << "pid" << "physician" << "insurance" << "pricetab" << "pt_version" << "rdate" << "ind" << "diagnosis" << "flag_charge" << "flag_visit" << "flag_group" << "flag_accident" << "flag_report" << "distance" << "tos" << "positions" << "brutto" << "addpay" << "oflags" << "addpay_flag" << "addpay_date" << "bill_date" << "hb_count";
    listHeaders << "Datum" << "Name" << "Vorname" << "Rezeptwert" << "Status";
    listItems << "receipes.id" << "receipes.rdate" << "patients.surname" << "patients.forename" << "receipes.brutto" << "receipe_states.sdesc" << "receipes.pid";
    listTableNames << "receipes, receipe_states, patients";
    listConditions << "WHERE (receipes.type=3 OR receipes.type=5) AND receipes.state < 3 AND receipes.state=receipe_states.id AND receipes.pid=patients.id";
    listTables[TABLE_CASH_PRIVRECEIPE]->setTableProperties(&listHeaders, &listTableNames, &listItems, &listConditions);
    listHeaders.clear(); listItems.clear(); listTableNames.clear(); listConditions.clear();

    listTables[TABLE_QUALIFICATIONS]->init("qualifications");
    *listTables[TABLE_QUALIFICATIONS] << "prof" << "q_name";

    listTables[TABLE_DATE_TYPES]->init("date_types");
    *listTables[TABLE_DATE_TYPES] << "dt_desc";

    listTables[TABLE_META_DATES]->init("meta_dates");
    *listTables[TABLE_META_DATES] << "md_start" << "md_end" << "flag_repeat";

    listTables[TABLE_DATE_STATES]->init("date_states");
    *listTables[TABLE_DATE_STATES] << "sdesc";

    listTables[TABLE_PAPPENDIX]->init("appendix");
    *listTables[TABLE_PAPPENDIX] << "id" << "pid" << "sdate" << "insurance" << "pricetab" << "positions" << "brutto" ;

    listTables[TABLE_RECEIPE_SHORTCUTS]->init("receipe_shortcuts");
    *listTables[TABLE_RECEIPE_SHORTCUTS] << "prof" << "amount" << "descr" << "posnr";

    listTables[TABLE_CLINICS]->init("clinics");
    *listTables[TABLE_CLINICS] << "name1" << "name2" << "city" << "street" << "zip" << "tel" << "fax" << "email";
    listHeaders << "Name1" << "Name2" << "Straße" << "Stadt" << "PLZ";
    listItems << "id" << "name1" << "name2" << "street" << "city" << "zip";
    listTableNames << "clinics";
    listTables[TABLE_CLINICS]->setTableProperties(&listHeaders, &listTableNames, &listItems, &listConditions);
    listHeaders.clear(); listItems.clear(); listTableNames.clear(); listConditions.clear();

    listTables[TABLE_HM_CAT_DOMAINS]->init("hm_cat_domains");
    *listTables[TABLE_HM_CAT_DOMAINS] << "name" << "positions";

    listTables[TABLE_HM_CAT]->init("hm_cat");
    *listTables[TABLE_HM_CAT] << "ind" << "hm_primary" << "hm_optional" << "hm_additional" << "maxcount" << "erstvo" << "folgevo" << "child_only";

    listTables[TABLE_TEMPLATE_TYPES]->init("template_types");
    *listTables[TABLE_TEMPLATE_TYPES] << "t_name";

    listTables[TABLE_TEMPLATE_DATA_CATEGORIES]->init("template_data_categories");
    *listTables[TABLE_TEMPLATE_DATA_CATEGORIES] << "c_name" << "c_shortname" << "c_fields";

    listTables[TABLE_TEMPLATE_DATA_MAPPINGS]->init("template_data_mapping");
    *listTables[TABLE_TEMPLATE_DATA_MAPPINGS] << "c_shortname" << "c_name";

    listTables[TABLE_TEMPLATE_AVAIL_DATA_CATEGORIES]->init("template_available_data_categories");
    *listTables[TABLE_TEMPLATE_AVAIL_DATA_CATEGORIES] << "tmpl_type" << "avail";

    listTables[TABLE_TEMPLATES]->init("doc_templates");
    *listTables[TABLE_TEMPLATES] << "tmpl_type" << "tmpl_subtype" << "tmpl_ver" << "tmpl_flags" << "tmpl_hash" << "tmpl_name" << "contents";

    listTables[TABLE_DATE_ACTIVITIES]->init("date_activities");
    *listTables[TABLE_DATE_ACTIVITIES] << "prof" << "a_id" << "a_name" << "a_enabled";

    listTables[TABLE_ZIPCODES]->init("zipcodes_germany");
    *listTables[TABLE_ZIPCODES] << "zip" << "city";

    listTables[TABLE_TEXT_MODULES]->init("text_modules");
    *listTables[TABLE_TEXT_MODULES] << "tm_domain" << "tm_text";
}


myDB::~myDB()
{
#ifndef OPA
    while (!listTables.isEmpty()) delete listTables.takeFirst();
    while (!listEmpl.isEmpty()) delete listEmpl.takeFirst();
    while (!table_hm_pos.isEmpty()) delete table_hm_pos.takeFirst();
    while (!rooms.isEmpty()) delete rooms.takeFirst();
    while (!listCars.isEmpty()) delete listCars.takeFirst();
    while (!dateTypes.isEmpty()) delete dateTypes.takeFirst();
    while (!listAssociations.isEmpty()) delete listAssociations.takeFirst();
    while (!listIndicators.isEmpty()) delete listIndicators.takeFirst();
    while (!listPhysTypes.isEmpty()) delete listPhysTypes.takeFirst();
    while (!listEmployeeGroups.isEmpty()) delete listEmployeeGroups.takeFirst();
    while (!listTherapyGroups.isEmpty()) delete listTherapyGroups.takeFirst();
    while (!listReceipeStates.isEmpty()) delete listReceipeStates.takeFirst();
    while (!listInvoiceStates.isEmpty()) delete listInvoiceStates.takeFirst();
    while (!listPraxisInvoiceStates.isEmpty()) delete listPraxisInvoiceStates.takeFirst();
    while (!listCashPositions.isEmpty()) delete listCashPositions.takeFirst();
    while (!listInsuranceTypes.isEmpty()) delete listInsuranceTypes.takeFirst();
    while (!listProfessions.isEmpty()) delete listProfessions.takeFirst();
    while (!listQualifications.isEmpty()) delete listQualifications.takeFirst();
    while (!listTemplates.isEmpty()) delete listTemplates.takeFirst();
    while (!listZipCodes.isEmpty()) delete listZipCodes.takeFirst();
    while (!listInsurances.isEmpty()) delete listInsurances.takeFirst();
#endif
    while (!listReceipeShortcuts.isEmpty()) delete listReceipeShortcuts.takeFirst();
}


bool myDB::DB_connect(cDatabase& d)
{
    bool bRet = true;

    // 1. Alte Verbindung entfernen, falls vorhanden
    if (QSqlDatabase::contains("openphysio"))
        QSqlDatabase::removeDatabase("openphysio");

    db_handle = QSqlDatabase::addDatabase( "QMYSQL" /*d.getDriver()*/, "openphysio");

    if (!db_handle.isOpen()) {
	if (d.getDriver() == "QSQLITE") {
            // Check whether the database file exists
            // (if no db file exists QSQLITE would create an empty file and returns true ... ).
            //
            // First we suggest that the path to the database (stored in the config) is relative to the
            // config file path.
            // If that does not apply we try to use the path in absolute manner.
            QFile f;
            QString s = myConf.getConfigFilePath() + d.getFile();
            if (f.exists(s)) db_handle.setDatabaseName(s);
            else if (f.exists(d.getFile())) db_handle.setDatabaseName(d.getFile());
            else bRet = false;
            db_properties = d;
            db_properties.setFile(db_handle.databaseName());
    } else {
	    db_handle.setHostName(d.getHost());
	    db_handle.setDatabaseName(d.getName());
	    db_handle.setUserName(d.getUser());
	    db_handle.setPassword(d.getPW());
        db_handle.setPort(3306);
#ifdef linux

        // Pfade zu den bestehenden SSL-Dateien
        QString sslCertPath = QDir::homePath() + "/ssl/client-cert.pem";
        QString sslKeyPath  = QDir::homePath() + "/ssl/client-key.pem";
        QString sslCAPath   = QDir::homePath() + "/ssl/ca-cert.pem";

#else

        QString sslKeyPath = "C:/ssl/client-key.pem";
        QString sslCertPath = "C:/ssl/client-cert.pem";
        QString sslCAPath   = "C:/ssl/ca-cert.pem";

#endif

        if (!QFile::exists(sslCAPath)) {
                qWarning() << "CA file does not exist:" << sslCAPath;
                return false;
            }

        QStringList options;
            options << QString("SSL_CA=%1").arg(sslCAPath);

        // Optional: Client-Zertifikat/Key nur, wenn vorhanden
        if (QFile::exists(sslCertPath) && QFile::exists(sslKeyPath)) {
            options << QString("SSL_CERT=%1").arg(sslCertPath)
                    << QString("SSL_KEY=%1").arg(sslKeyPath);
        } else {
            qDebug() << "Client cert/key not found, using CA only";
        }

        db_handle.setConnectOptions(options.join(";"));


        if (!db_handle.open()) {
            bRet = false;
            qDebug() << db_handle.lastError();
#ifndef OPA
            QMessageBox::critical(NULL, "OpenPhysio - Fehler", "Keine Verbindung zur Datenbank!", QMessageBox::Ok);
#endif
        }
#ifndef OPA
        if (bRet) {
#ifdef USE_ASYNC_DB_ACCESS
            if (thread.init(d)) thread.start();
#endif
            connect(this, &myDB::sigDBChange, this, &myDB::slotUpdateDB);
        }
#endif // OPA
    }
    }
    return bRet;
}


cDatabase myDB::DB_getProperties()
{
    return db_properties;
}


void myDB::DB_disconnect()
{
#ifdef USE_ASYNC_DB_ACCESS
#ifndef OPA
    thread.stop();
    thread.wait(10000);
#endif // OPA
#endif // USE_ASYNC_DB_ACCESS
    {
	if (db_handle.isOpen()) db_handle.close();
    }
    QSqlDatabase::removeDatabase("openhpysio");
}


bool myDB::isConnected()
{
    return db_handle.isOpen();
}


void myDB::getError(QString qs, QSqlError e)
{
    if (myConf.isDbg()) {
	if (e.isValid()) {
	    qDebug() << "Error:";
	    qDebug() << e.text();
	    qDebug() << "Query:";
	    qDebug() << qs;
	}
    }
}


int myDB::currentDatabaseVersion()
{
    if (!db_handle.isOpen()) return 0;

    QSqlQuery query(db_handle);
    query.prepare("SELECT db_ver FROM current_db_version WHERE id=0");
    QUERY_AND_CHECK();

    query.first();
    if (query.isValid()) {
        return query.value(0).toInt();
    }
    return 0;
}

#ifndef OPA

void myDB::loadTable(int table_id, CustomSqlModel *model, QString filter)
{
    //qDebug() << __FUNCTION__ << table_id;
    //qDebug() << "Filter: " << filter;

    if ( (table_id < 0) || (table_id >= TABLE_DUMMY) ) return;
    if (model == NULL) return;
    if (!db_handle.isOpen()) return;

    if (listTables.at(table_id)->getQueryTables().isEmpty()) return;

    QString query, arg;

    if (listTables.at(table_id)->getQueryConditions().count() > 0)
	arg = " " + listTables.at(table_id)->getQueryConditions().join(" ");
    else arg = "";

    if (!filter.isEmpty()) {
        if (arg.isEmpty()) arg = " WHERE " + filter;
        else arg += (" AND " + filter);
    }

    QSqlQuery tquery(db_handle);
    QString s = "SELECT delflag FROM " + listTables.at(table_id)->tabName() ;
    tquery.prepare(s);
    if (tquery.exec()) {
        // delete flag from "base" table is enougth
        QString tmp = listTables.at(table_id)->tabName() + ".delflag=0";
        if (arg.isEmpty()) arg = " WHERE " + tmp;
        else arg += " AND " + tmp;
    }
    query = "SELECT " + listTables.at(table_id)->getQueryItems().join(",") + " FROM " + listTables.at(table_id)->getQueryTables().join(",") + arg;
    qDebug() << query;
#ifdef USE_ASYNC_DB_ACCESS
    qDebug() << "Task at:" << QTime::currentTime().toString("hh:mm:ss");
    thread.slotExecQuery(model, query);
#else
    //qDebug() << "Start at:" << QTime::currentTime().toString("hh:mm:ss");
    model->setQuery(query, db_handle);
    //qDebug() << "End at:" << QTime::currentTime().toString("hh:mm:ss");
    while (model->canFetchMore()) {
        model->fetchMore();
    }
    //qDebug() << "Parsing finished at:" << QTime::currentTime().toString("hh:mm:ss");

    QStringList listHeaders = getTableHeaders(table_id);
    for (int i = 0; i < listHeaders.count(); i++) {
        model->setHeaderData(i + 1, Qt::Horizontal, listHeaders.at(i));
    }
// j
#endif
}


void myDB::loadBaseTypes()
{
    loadEmployees();
    // load tariff areas before price tables because loading price tables depends on tariff areas
    retrieveItemList(TABLE_TARIFF_AREAS, reinterpret_cast<QList<cTableItem*>&>(listTariffAreas));
    loadPriceTableNames();
    retrieveItemList(TABLE_HMPOS, reinterpret_cast<QList<cTableItem*>&>(table_hm_pos));
    retrieveItemList(TABLE_ROOMS, reinterpret_cast<QList<cTableItem*>&>(rooms));
    retrieveItemList(TABLE_CARS, reinterpret_cast<QList<cTableItem*>&>(listCars));
    retrieveItemList(TABLE_DATE_TYPES, reinterpret_cast<QList<cTableItem*>&>(dateTypes));
    retrieveItemList(TABLE_ASSOC, reinterpret_cast<QList<cTableItem*>&>(listAssociations));
    retrieveItemList(TABLE_INDICATORS, reinterpret_cast<QList<cTableItem*>&>(listIndicators));
    retrieveItemList(TABLE_PHYSICIAN_TYPES, reinterpret_cast<QList<cTableItem*>&>(listPhysTypes));
    retrieveItemList(TABLE_EMPLOYEE_GROUPS, reinterpret_cast<QList<cTableItem*>&>(listEmployeeGroups));
    retrieveItemList(TABLE_THERAPY_GROUPS, reinterpret_cast<QList<cTableItem*>&>(listTherapyGroups));
    retrieveItemList(TABLE_RECEIPE_STATES, reinterpret_cast<QList<cTableItem*>&>(listReceipeStates));
    retrieveItemList(TABLE_INVOICE_STATS, reinterpret_cast<QList<cTableItem*>&>(listInvoiceStates));
    retrieveItemList(TABLE_PINVOICE_STATS, reinterpret_cast<QList<cTableItem*>&>(listPraxisInvoiceStates));
    retrieveItemList(TABLE_CASHPOS, reinterpret_cast<QList<cTableItem*>&>(listCashPositions));
    retrieveItemList(TABLE_INSURANCE_TYPES, reinterpret_cast<QList<cTableItem*>&>(listInsuranceTypes));
    retrieveItemList(TABLE_PROFESSIONS, reinterpret_cast<QList<cTableItem*>&>(listProfessions));
    retrieveItemList(TABLE_QUALIFICATIONS, reinterpret_cast<QList<cTableItem*>&>(listQualifications));
    retrieveItemList(TABLE_DATE_STATES, reinterpret_cast<QList<cTableItem*>&>(listDateStates));
    retrieveItemList(TABLE_RECEIPE_SHORTCUTS, reinterpret_cast<QList<cTableItem*>&>(listReceipeShortcuts));

    retrieveItemList(TABLE_TEMPLATES, reinterpret_cast<QList<cTableItem*>&>(listTemplates));
    // decompress template data
    for (int i = 0; i < listTemplates.count(); i++) {
        QByteArray qba = QByteArray::fromBase64(listTemplates.at(i)->getContents());
        qba = qUncompress(qba);
        listTemplates.at(i)->setContents(qba);
    }

    retrieveItemList(TABLE_TEMPLATE_TYPES, reinterpret_cast<QList<cTableItem*>&>(listTemplateTypes));
    retrieveItemList(TABLE_TEMPLATE_DATA_CATEGORIES, reinterpret_cast<QList<cTableItem*>&>(listCategories));
    retrieveItemList(TABLE_TEMPLATE_AVAIL_DATA_CATEGORIES, reinterpret_cast<QList<cTableItem*>&>(listAvailCategories));
    retrieveItemList(TABLE_TEMPLATE_DATA_MAPPINGS, reinterpret_cast<QList<cTableItem*>&>(listMappings));
    retrieveItemList(TABLE_DATE_ACTIVITIES, reinterpret_cast<QList<cTableItem*>&>(listDateActivities));
}


QStringList myDB::getTableHeaders(int tab)
{
    QStringList tmp;
    if ( (tab < 0) || (tab >= TABLE_DUMMY) ) return tmp;
    return listTables.at(tab)->getHeaders();
}


QStringList myDB::getTableItems(int tab)
{
    QStringList tmp;
    if ( (tab < 0) || (tab >= TABLE_DUMMY) ) return tmp;
    return listTables.at(tab)->getQueryItems();
}

#endif // OPA


#ifndef OPA

bool myDB::existPatient(cPatient& p, int *id)
{
    bool bRet = false;

    if (!db_handle.isOpen()) return false;

    QSqlQuery query(db_handle);
    query.prepare("SELECT id, surname, forename FROM patients WHERE birthday=?");
    query.addBindValue(p.getBirthday().toString("yyyy-MM-dd"));
    QUERY_AND_CHECK();

    query.first();
    while (query.isValid()) {
	if ( query.value(1).toString().toLower() == p.getName().toLower() &&
		query.value(2).toString().toLower() == p.getForename().toLower() )
	{
	    *id = query.value(0).toInt();
	    bRet = true;
	    break;
	}
	query.next();
    }

    return bRet;
}

#endif // OPA


bool myDB::existDocTemplate(QString tmpl_name, int *id)
{
    if (!db_handle.isOpen()) return false;

    QSqlQuery query(db_handle);
    query.prepare("SELECT id FROM doc_templates WHERE tmpl_name=?");
    query.addBindValue(tmpl_name);
    QUERY_AND_CHECK();

    query.first();
    if (query.isValid()) {
        *id = query.value(0).toInt();
        return true;
    } else *id = -1;
    return false;
}


bool myDB::existPerson(cTableItem* cti, int *id)
{
    bool bRet = false;
    if (cti == NULL) return false;
    if (!db_handle.isOpen()) return false;
    int table = cti->getTableID();
    if ( (table < 0) || (table >= TABLE_DUMMY) ) return false;

    QString surname, forename;
    switch(table) {
	case TABLE_PATIENTS:
	    surname = ((cPatient*)cti)->getName().toLower();
	    forename = ((cPatient*)cti)->getForename().toLower();
	    break;
	case TABLE_PHYSICIANS:
	    surname = ((cPhys*)cti)->getName().toLower();
	    forename = ((cPhys*)cti)->getForename().toLower();
	    break;
#ifndef OPA
        case TABLE_EMPLOYEES:
            surname = ((cEmployee*)cti)->getName().toLower();
            forename = ((cEmployee*)cti)->getForename().toLower();
            break;
#endif
        default:
	    return false;
	    break;
    }

    QSqlQuery query(db_handle);
    QString s = "SELECT id, surname, forename FROM " + listTables.at(table)->tabName();
    s += " WHERE delflag=0";
    query.prepare(s);
    QUERY_AND_CHECK();

    query.first();
    while (query.isValid()) {
	if ( query.value(1).toString().toLower() == surname &&
		query.value(2).toString().toLower() == forename )
	{
	    *id = query.value(0).toInt();
	    bRet = true;
	    break;
	}
	query.next();
    }

    return bRet;
}


#ifndef OPA

bool myDB::loadEmployees()
{
    if (!db_handle.isOpen()) return false;

    // reset the list of employees
    while (!listEmpl.isEmpty()) delete listEmpl.takeFirst();

    QSqlQuery query(db_handle);
    // FIXME: Maybe we should choose another ORDER criteria?
    query.prepare("SELECT * FROM employees ORDER BY id");
    QUERY_AND_CHECK();

    query.first();
    while (query.isValid()) {
	listEmpl.append(new cEmployee());
        listEmpl.last()->retrieve(query);
	query.next();
    }
    return true;
}


bool myDB::loadReceipesForPatient(int pid, QList<cReceipe *> &receipes)
{
    if (!db_handle.isOpen()) return false;

    QSqlQuery query(db_handle);
    query.prepare("SELECT * FROM receipes WHERE pid=? ORDER BY rdate DESC LIMIT 5");
    query.addBindValue(pid);
    QUERY_AND_CHECK();

    query.first();
    while (query.isValid()) {
        receipes.append(new cReceipe());
        receipes.last()->retrieve(query);
        query.next();
    }
    return true;
}

bool myDB::setRidInDatesForPid(int pid, int rid)
{
    if (!db_handle.isOpen()) return false;

    QSqlQuery query(db_handle);
    query.prepare("UPDATE dates SET rid = ?  WHERE ((rid = -1) AND (pid = ?))");
    query.addBindValue(rid);
    query.addBindValue(pid);
    QUERY_AND_CHECK();

    query.first();
/*    while (query.isValid()) {
        receipes.append(new cReceipe());
        receipes.last()->retrieve(query);
        query.next();
    } */
    return true;
}



#endif // OPA


bool myDB::getInsurance(QString ik, cInsurance& i)
{
    if (!db_handle.isOpen()) return false;

    QSqlQuery query(db_handle);
    query.prepare("SELECT name_short, assoc FROM insurances where ik=?");
    query.addBindValue(ik);
    QUERY_AND_CHECK();

    query.first();
    if (query.isValid()) {
	i.setIK(ik);
	i.setShortName(query.value(0).toString());
	i.setAssoc(query.value(1).toInt());
    } else {
	getError(query.executedQuery(), query.lastError());
	return false;
    }
    return true;
}


bool myDB::saveInsurances(QList<cInsurance*> *l)
{
    if (l == NULL) return false;
    if (!db_handle.isOpen()) return false;

    QSqlQuery query(db_handle);
    QString s;
    if (db_properties.getType() == cDatabase::DB_SQLITE) s = "BEGIN TRANSACTION";
    else s = "START TRANSACTION";
    query.prepare(s);
    QUERY_AND_CHECK();
    bool bRet = true;
    for (int i = 0; i < l->count(); i++) {
        if (!saveItem(*l->at(i), false)) bRet = false;
    }
    query.clear();
    if (db_properties.getType() == cDatabase::DB_SQLITE) s = "END TRANSACTION";
    else s = "COMMIT";
    query.prepare(s);
    QUERY_AND_CHECK();
    return bRet;
}


bool myDB::loadPriceTableNames()
{
    if (!db_handle.isOpen()) return false;

    while (!listPriceTablesNames.isEmpty()) delete listPriceTablesNames.takeFirst();

    int area = surg.getArea();
    int scope = 0;
    for (int i = 0; i < listTariffAreas.count(); i++) {
        cTariffArea *cta = listTariffAreas.at(i);
        if (cta->getID() == area) {
            scope = cta->getScope();
        }
    }
    if (scope == cTariffScope::TARIFF_SCOPE_EAST) scope = cTariffScope::TARIFF_SCOPE_ID_EAST;
    else scope = cTariffScope::TARIFF_SCOPE_ID_WEST;

    QSqlQuery query(db_handle);
    query.prepare("SELECT * FROM price_tables WHERE area=? OR area=? OR area=? ORDER BY valid_from DESC");
    query.addBindValue(area);
    query.addBindValue(scope);
    query.addBindValue(cTariffScope::TARIFF_SCOPE_ID_GERMANY);
    QUERY_AND_CHECK();

    query.first();
    while (query.isValid()) {
	listPriceTablesNames.append(new cPriceTableNames());
        listPriceTablesNames.last()->retrieve(query);
	query.next();
    }
    return true;
}


QString myDB::lookupPriceTableNameAndID(int prof, int assoc, int *id)
{
    QString qRet = "";
    int tid = -1;
    int tmp_assoc = assoc;

    // Currently BPol maps to VDEK prices
    // However, this might chance for whatever reasons, so keep an eye on this!
    if (tmp_assoc == cAssoc::BPOL) tmp_assoc = cAssoc::VDEK;

    QString u = QString::number(surg.getUnionID(prof));

    for (int i = 0; i < listPriceTablesNames.count(); i++) {

        // check if price table matches the requested profession
        if (listPriceTablesNames.at(i)->getProfession() != prof) continue;

        // check if price table matches the requested association
        if (listPriceTablesNames.at(i)->getAssoc() != tmp_assoc) continue;

        // check if price table is already marked as valid
	if (listPriceTablesNames.at(i)->getValidFrom() > QDate::currentDate()) continue;

	QString s_union = listPriceTablesNames.at(i)->getUnions();
	// if surgery does not belong to a specific union (0) and the pricetable is marked
	// as unspecific => return that pricetable name OR
	// if surgery belongs to a specific union and the pricetable is marked for this specific
	// union => return that pricetable name
	if (u == s_union) {
	    qRet = listPriceTablesNames.at(i)->getName();
            tid = listPriceTablesNames.at(i)->getID();
	    break;
	}

	// check if pricetable is marked for multiple unions
	QStringList list = s_union.split(":");
	bool flag_exit = false;
	for (int j = 0; j < list.count(); j++) {
	    if (u == list[j]) {
		qRet = listPriceTablesNames.at(i)->getName();
                tid = listPriceTablesNames.at(i)->getID();
		flag_exit = true;
		break;
	    }
	}
	if (flag_exit) break;

	// finally, if the price table is marked for being globally valid for that association
	// just select that one
	if (s_union == "0") {
	    qRet = listPriceTablesNames.at(i)->getName();
            tid =  listPriceTablesNames.at(i)->getID();
	    break;
	}
    }

    *id = tid;
    return qRet;
}


QString myDB::getPriceTableName(int id)
{
    for (int i = 0; i < listPriceTablesNames.count(); i++) {
	if (listPriceTablesNames.at(i)->getID() == id) {
	    return listPriceTablesNames.at(i)->getName();
	}
    }
    return "";
}


QString myDB::getPriceTableDescription(int id)
{
    for (int i = 0; i < listPriceTablesNames.count(); i++) {
	if (listPriceTablesNames.at(i)->getID() == id) {
	    return listPriceTablesNames.at(i)->getDescription();
	    break;
	}
    }
    return "";
}


bool myDB::loadPriceTable(QString tab, cPriceTable& cpt, int prof)
{
    if (!db_handle.isOpen()) return false;
	
    QSqlQuery query(db_handle);
    query.prepare("SELECT pos_nr, price, addpay FROM " + tab);
    QUERY_AND_CHECK();

    query.first();
    while (query.isValid()) {
	cpt.append(query.value(0).toString(), query.value(1).toString(), query.value(2).toString());
	query.next();
    }
    // match positions of this price table to the global position table (table_hm_pos)
    cpt.prepare(prof);

    return true;
}


int myDB::loadPriceTablePrivate(int id, int pt_type, cPriceTablePriv& cpt)
{
    if (!db_handle.isOpen()) return -1;

    QString tab_name = "";
    QString valid_from = "";
    int tab_id = -1;
    int tab_type = -1;

    QSqlQuery query(db_handle);
    if (id < 0) { // load current version of price table
        query.prepare("SELECT id, pt_type, name, valid_from FROM private_price_tables WHERE pt_type=? ORDER BY valid_from DESC");
	query.addBindValue(pt_type);
    } else {  // load specific price table
        query.prepare("SELECT id, pt_type, name, valid_from FROM private_price_tables WHERE id=?");
	query.addBindValue(id);
    }
    QUERY_AND_CHECK();

    query.first();
    if (query.isValid()) {
        tab_id = query.value(0).toInt();
        tab_name = query.value(2).toString();
        valid_from = query.value(3).toString();
    } else
        return -1;

    // qDebug() << __FUNCTION__ << tab_id << tab_name << valid_from;
    cpt.setID(tab_id);
    cpt.setType(tab_type);
    //cpt.setName(tab_name);
    cpt.setVersion(valid_from);

    query.clear();
    query.prepare("SELECT pos_name, price FROM " + tab_name + " WHERE pt_version=?");
    query.addBindValue(valid_from);
    if ( query.exec() == false) {
        getError(query.executedQuery(), query.lastError());
	return -1;
    }
    query.first();
    while (query.isValid()) {
        cpt.addItem(query.value(0).toString(), query.value(1).toString());
        query.next();
    }
    cpt.setType(pt_type); /* von mir eingefügt */
    return tab_id;
}


bool myDB::savePriceTablePrivate(cPriceTablePriv& cpt)
{
    if (!db_handle.isOpen()) return false;
    if (cpt.count() <= 0) return false;

    // get current price talbe
    QSqlQuery query(db_handle);
    query.prepare("SELECT valid_from, name, descr FROM private_price_tables WHERE pt_type=? ORDER BY valid_from DESC");
    query.addBindValue(cpt.getType());
    QUERY_AND_CHECK();

    QDate cur;
    QString cur1, pt_name, pt_descr;
    query.first();
    if (query.isValid()) {
        cur = QDate::fromString(query.value(0).toString(), "yyyy-MM-dd");
        pt_name = query.value(1).toString();
        pt_descr = query.value(2).toString();
    } else return false;
    if (!pt_descr.isEmpty()) {
	QStringList qsl = pt_descr.split(" ");
    if (qsl.count() > 0) pt_descr = qsl[0] + " ab " + QDate::currentDate().toString("dd.MM.yyyy");
    }

    query.clear();
    // only create a new entry if the date is different
    if (cur != QDate::currentDate()) {
	query.prepare("INSERT INTO private_price_tables VALUES(NULL,?,?,?,?)");
	query.addBindValue(cpt.getType());
	query.addBindValue(QDate::currentDate().toString("yyyy-MM-dd"));
	query.addBindValue(pt_name);
	query.addBindValue(pt_descr);
    } else { // remove existing entries from today first before inserting new entries
        // FIXME: if there are receipes that use the positions we are going to delete now
        //        we should do something about it !!!
	QString s = "DELETE FROM " + pt_name + " WHERE pt_version=?";
	query.prepare(s);
	query.addBindValue(QDate::currentDate().toString("yyyy-MM-dd"));
    }
    QUERY_AND_CHECK();

    // save new positions and prices
    bool bRet = true;
    QString q = "INSERT INTO " + pt_name + " VALUES(NULL,?,?,?)";
    for (int i = 0; i < cpt.count(); i++) {
        query.clear();
        query.prepare(q);
        query.addBindValue(QDate::currentDate().toString("yyyy-MM-dd"));
        query.addBindValue(cpt.qslPosName[i]);
        query.addBindValue(cpt.qslPrice[i]);
	if (query.exec() == false) bRet = false;

    }
    return bRet;
}


bool myDB::getAvailablePriceTablesForGKV(QList<int>& listID, QStringList& listDescr)
{
    for (int i = 0; i < listPriceTablesNames.count(); i++) {
        if (listPriceTablesNames.at(i)->getAssoc() < cAssoc::ASSOC_COUNT) {
	    // filter redundant entries
	    bool redundant = false;
	    for (int j = 0; j < listDescr.count(); j++) {
		if (listDescr.at(j) == listPriceTablesNames.at(i)->getDescription()) {
		    redundant = true;
		    break;
		}
	    }
	    if (!redundant) {
		listID.append(listPriceTablesNames.at(i)->getID());
		listDescr.append(listPriceTablesNames.at(i)->getDescription());
	    }
	}
    }

    return true;
}


bool myDB::priceTableUsed(QString s)
{
    QSqlQuery query(db_handle);
    query.prepare("SELECT id FROM price_tables WHERE name=?");
    query.addBindValue(s);
    QUERY_AND_CHECK();

    QStringList list;
    query.first();
    while (query.isValid()) {
        list << query.value(0).toString();
        query.next();
    }
    if (list.isEmpty()) return false;

    QString q = "SELECT max(id) FROM receipes WHERE type<3 and ";
    if (list.count() == 1) q+= "pricetab=" + list.at(0);
    else {
        q += "(pricetab=";
        q += list.join(" OR pricetab=");
        q += ")";
    }
    query.prepare(q);
    QUERY_AND_CHECK();
    query.first();
    if (query.isValid()) {
        if (!query.value(0).toString().isEmpty()) return true;
    }
    return false;
}


#ifdef OPA
bool myDB::deletePriceTable(QString pt_name)
{
    if (!db_handle.isOpen()) return false;

    QSqlQuery query(db_handle);
    query.prepare("DELETE FROM price_tables WHERE name=?");
    query.addBindValue(pt_name);
    QUERY_AND_CHECK();

    query.clear();
    query.prepare("DROP TABLE " + pt_name);
    QUERY_AND_CHECK();
    return true;
}
#endif


#ifndef OPA

int myDB::getNumOfBilledReceipes(QDate d_start, QDate d_end, float *f)
{
    int co = 0;
    double sum = 0;
	
    if (!db_handle.isOpen()) return co;

    QSqlQuery query(db_handle);
    // delete all therapies associated with this receipe
    query.prepare("SELECT id, brutto FROM receipes WHERE state=3 AND rdate>=? AND rdate<=? AND delflag=0");
    query.addBindValue(d_start.toString("yyyy-MM-dd"));
    query.addBindValue(d_end.toString("yyyy-MM-dd"));

    if ( query.exec() == false) {
	getError(query.executedQuery(), query.lastError());
	return co;
    }
    // for some reason, query.size() does not work => we just enumerate through results
    // in order to retrieve the number of rows
    query.first();
    while (query.isValid()) {
	co++;
	sum = sum + query.value(1).toDouble();
	query.next();
    }
	
    *f = sum;
    return co;
}

#endif // OPA


bool myDB::getReceipeTypes(int t, QStringList& qsl)
{
    if (!db_handle.isOpen()) return false;
	
    QSqlQuery query(db_handle);
    switch (t) {
	case RT_GKV:
	    query.prepare("SELECT tdesc FROM receipe_types WHERE id < 3");
	    break;
	case RT_PRIVATE:
	    query.prepare("SELECT tdesc FROM receipe_types WHERE id > 2");
	    break;
	default: return false;
	    break;
    }
    QUERY_AND_CHECK();

    query.first();
    while (query.isValid()) {
	qsl.append(query.value(0).toString());
	query.next();
    }
    return true;
}


bool myDB::initSurgery()
{
    return retrieveItem(surg, 1);
}


cSurgery myDB::getSurgery()
{
    return surg;
}


bool myDB::getInvoiceNumbers(int rid, QStringList& qsl)
{
    if (!db_handle.isOpen()) return false;
	
    QSqlQuery query(db_handle);
    query.prepare("SELECT i_nr FROM invoices WHERE rid=? AND delflag=0");
    query.addBindValue(rid);
    QUERY_AND_CHECK();

    query.first();
    while (query.isValid()) {
	qsl.append(query.value(0).toString());
	query.next();
    }
    return true;
}


QString myDB::getCurrentNumbering(int nr_type)
{
    QString sRet = "";
	
    if (!db_handle.isOpen()) return sRet;

    QSqlQuery query(db_handle);
    QString s_query;
    if (nr_type == NR_INVOICE) s_query = "SELECT curinvoice FROM invoice_numbering";
    else if (nr_type == NR_QUITTANCE) s_query = "SELECT curquittance FROM quittance_numbering";
    query.prepare(s_query);
    if ( query.exec() == false) {
        getError(query.executedQuery(), query.lastError());
        return "";
    }

    query.first();
    if (query.isValid()) {
	sRet = query.value(0).toString();
    } else {
	getError(query.executedQuery(), query.lastError());
    }
    return sRet;
}


bool myDB::setCurrentNumbering(QString s, int nr_type)
{
    if (!db_handle.isOpen()) return false;

    bool bRet = true;

    QSqlQuery query(db_handle);
    QString s_query;
    if (nr_type == NR_INVOICE) s_query = "UPDATE invoice_numbering SET curinvoice=?";
    else if (nr_type == NR_QUITTANCE) s_query = "UPDATE quittance_numbering SET curquittance=?";
    query.prepare(s_query);
    query.addBindValue(s);
    if ( query.exec() == false) {
        getError(query.executedQuery(), query.lastError());
        bRet = false;
    }
    // for some reasons a failed update will not be reported (at least for sqlite)
    // => we read the number to be sure it was written to the database
    // UPDATE might fail because an entry for the selected surgery [sid] does not yet exist)
    QString compare = getCurrentNumbering(nr_type);
    if ( (bRet == false) || (compare != s) ) {
        query.clear();
        if (nr_type == NR_INVOICE) s_query = "INSERT INTO invoice_numbering VALUES(NULL,?,?)";
        else if (nr_type == NR_QUITTANCE) s_query = "INSERT INTO quittance_numbering VALUES(NULL,?,?)";
        query.prepare(s_query);
        query.addBindValue(s);
        QUERY_AND_CHECK();
    }
    return true;
}


bool myDB::loadHotKeyFunctions(QStringList& listHKF)
{
    if (!db_handle.isOpen()) return false;

    bool bRet = true;
    QSqlQuery query(db_handle);

    query.prepare("SELECT descr FROM hotkey_functions ORDER BY id");
    QUERY_AND_CHECK();

    query.first();
    while (query.isValid()) {
	listHKF << query.value(0).toString();
	query.next();
    }
    return bRet;
}


bool myDB::updateProtectedFuncs(QStringList& qslRID)
{
    if (!db_handle.isOpen()) return false;
	
    bool bRet = true;
    QSqlQuery query(db_handle);

    for (int x = 0; x < qslRID.count(); x++) {
	query.prepare("UPDATE protected_functions SET ridlist=? WHERE id=?");
	query.addBindValue(qslRID.at(x));
	query.addBindValue(x);
	QUERY_AND_CHECK();
	query.clear();
    }
	
    emit sigDBChange(TABLE_PROTECT, OP_UPDATE, -1);
    return bRet;
}


#ifndef OPA

bool myDB::loadDates(QList<cDateItem *> &list, QDate from, QDate until)
{
    if (!db_handle.isOpen()) return false;

    QSqlQuery query(db_handle);
    if (from == until) {
        query.prepare("SELECT * FROM dates WHERE d_date=? AND delflag=0");
        query.addBindValue(from.toString("yyyy-MM-dd"));
    } else {
        query.prepare("SELECT * FROM dates WHERE d_date>=? AND d_date<=? AND delflag=0");
        query.addBindValue(from.toString("yyyy-MM-dd"));
        query.addBindValue(until.toString("yyyy-MM-dd"));
    }
    QUERY_AND_CHECK();

    query.first();
    while (query.isValid()) {
        list.append(new cDateItem());
        list.last()->retrieve(query);
        query.next();
    }
    return true;
}


bool myDB::loadDates(QList<cDateItem *> &list, QDate from, QDate until, int eid)
{
    if (!db_handle.isOpen()) return false;

    QSqlQuery query(db_handle);
    if (from == until) {
        query.prepare("SELECT * FROM dates WHERE eid=? AND d_date=? AND delflag=0 ORDER BY d_start");
        query.addBindValue(eid);
        query.addBindValue(from.toString("yyyy-MM-dd"));
    } else {
        query.prepare("SELECT * FROM dates WHERE eid=? AND d_date>=? AND d_date<=? AND delflag=0 ORDER BY d_date, d_start");
        query.addBindValue(eid);
        query.addBindValue(from.toString("yyyy-MM-dd"));
        query.addBindValue(until.toString("yyyy-MM-dd"));
    }
    QUERY_AND_CHECK();

    query.first();
    while (query.isValid()) {
        list.append(new cDateItem());
        list.last()->retrieve(query);
        query.next();
    }
    return true;
}

bool myDB::loadDates(QList<cDateItem *> &list, QDate from, int uid)
{
    if (!db_handle.isOpen()) return false;

    QSqlQuery query(db_handle);

    query.prepare("SELECT * FROM dates WHERE ?>0 AND pid=? AND d_date>=? AND delflag=0 ORDER BY d_date, d_start");
    query.addBindValue(uid);
    query.addBindValue(uid);
    query.addBindValue(from.toString("yyyy-MM-dd"));

    QUERY_AND_CHECK();

    query.first();
    if (!query.isValid())
        return false;

    while (query.isValid()) {
        list.append(new cDateItem());
        list.last()->retrieve(query);
        query.next();
    }
    return true;
}
bool myDB::loadDates(QList<cDateItem *> &list, QDate from, int uid, int pid, int rid)
{
    if (!db_handle.isOpen()) return false;

    QSqlQuery query(db_handle);

    query.prepare("SELECT * FROM dates WHERE ?>0 AND pid=? AND rid=? AND d_date>=? AND delflag=0 ORDER BY d_date, d_start");
    query.addBindValue(uid);
    query.addBindValue(pid);
    query.addBindValue(rid);
    query.addBindValue(from.toString("yyyy-MM-dd"));

    QUERY_AND_CHECK();

    query.first();
    if (!query.isValid())
        return false;

    while (query.isValid()) {
        list.append(new cDateItem());
        list.last()->retrieve(query);
        query.next();
    }
    return true;
}

bool myDB::getDateList(QList<cDateItem*> &list, int dtype, int id)
{
    if (!db_handle.isOpen()) return false;

    bool bRet = true;
    QSqlQuery query(db_handle);

    switch(dtype) {
        case cDateType::DT_THERAPY:
            query.prepare("SELECT * FROM dates WHERE d_type=? AND dates.rid=? AND delflag=0 ORDER BY d_date, d_start ASC");
            break;
        case cDateType::DT_COURSE:
            query.prepare("SELECT * FROM dates WHERE d_type=? AND dates.cid=? AND delflag=0 ORDER BY d_date, d_start");
            break;
        default:
            bRet = false;
            break;
    }
    if (!bRet) return bRet;

    query.addBindValue(dtype);
    query.addBindValue(id);
    QUERY_AND_CHECK();

    query.first();
    while (query.isValid()) {
        list.append(new cDateItem());
        list.last()->retrieve(query);
        query.next();
    }

    return bRet;
}


bool myDB::saveDateList(QList<cDateItem*> &list)
{
    if (!db_handle.isOpen()) return false;

    bool bRet = true;
    bool modified = false;

    QSqlQuery query(db_handle);
    QString s;
    if (db_properties.getType() == cDatabase::DB_SQLITE) s = "BEGIN TRANSACTION";
    else s = "START TRANSACTION";
    query.prepare(s);
    QUERY_AND_CHECK();

    for (int i = 0; i < list.count(); i++) {
        cDateItem *cdi = list.at(i);
        if (cdi == NULL) continue;
        switch(cdi->getFlag()) {
            case cTableItem::FLAG_NEW:
                int id;
                if (storeItem(*cdi, &id, false)) {
                    cdi->setID(id);
                    cdi->setFlag(cTableItem::FLAG_ORG);
                    modified = true;
                }
                break;
            case cTableItem::FLAG_MODIFIED:
                if (updateItem(*cdi, false)) {
                    cdi->setFlag(cTableItem::FLAG_ORG);
                    modified = true;
                }
                break;
            case cTableItem::FLAG_DEL:
                deleteItem(TABLE_DATES, cdi->getID(), false);
                modified = true;
                break;
            default:
                break;
        }
    }

    query.clear();
    if (db_properties.getType() == cDatabase::DB_SQLITE) s = "END TRANSACTION";
    else s = "COMMIT";
    query.prepare(s);
    QUERY_AND_CHECK();

    if (modified) {
        emit sigDBChange(TABLE_DATES, OP_ANY, -1);
    }
    return bRet;
}


bool myDB::delMetaDates(int id)
{
    if (!db_handle.isOpen()) return false;

    QSqlQuery query(db_handle);
    //query.prepare("DELETE FROM dates WHERE mid=?");
    query.prepare("UPDATE dates SET delflag=1 WHERE mid=?");
    query.addBindValue(id);
    QUERY_AND_CHECK();

    query.clear();
//    query.prepare("DELETE FROM meta_dates WHERE id=?");
    query.prepare("UPDATE dates SET delflag=1 WHERE mid=?");
    query.addBindValue(id);
    QUERY_AND_CHECK();

    emit sigDBChange(TABLE_DATES, OP_DELETE, id);
    return true;
}


#endif // OPA


int myDB::checkDocTemplate(QString name)
{
    if (!db_handle.isOpen()) return -1;

    QSqlQuery query(db_handle);
    query.prepare("SELECT * FROM doc_templates WHERE tmpl_name=?");
    query.addBindValue(name);
    if ( query.exec() == false) return -1;
    query.first();

    return query.value(0).toInt();
}


bool myDB::retrieveItem(cTableItem& cti, int id)
{
    if (!db_handle.isOpen()) return false;
    int table = cti.getTableID();
    if ( (table < 0) || (table >= TABLE_DUMMY) ) return false;

    QSqlQuery query(db_handle);
    QString s = "SELECT delflag FROM " + listTables.at(table)->tabName() ;
    query.prepare(s);
    query.addBindValue(id);
    if (!query.exec()) {
        s = "SELECT * FROM " + listTables.at(table)->tabName() + " WHERE id=?";
    } else {
        s = "SELECT * FROM " + listTables.at(table)->tabName() + " WHERE id=? AND delflag=0";
    }
    query.prepare(s);
    query.addBindValue(id);
    QUERY_AND_CHECK();

    query.first();
    if (query.isValid()) {
	cti.retrieve(query);
	return true;
    } else {
	getError(query.executedQuery(), query.lastError());
	return false;
    }
}


bool myDB::saveItem(cTableItem& cti, bool emit_signal)
{
    if (cti.getID() < 0) {
        int id = -1;
        return storeItem(cti, &id, emit_signal);
    } else {
        return updateItem(cti, emit_signal);
    }
}


bool myDB::storeItem(cTableItem& cti, int *id, bool emit_signal)
{
    if (!db_handle.isOpen()) return false;
    int table = cti.getTableID();
    if ( (table < 0) || (table >= TABLE_DUMMY) ) return false;

    QSqlQuery query(db_handle);
    //s = "UPDATE " + listTables.at(table)->tabName() + " SET delflag=1 WHERE id=?";

    QString s = "SELECT delflag FROM " + listTables.at(table)->tabName() ;
    query.prepare(s);
    if (!query.exec()) {
       s = "INSERT INTO " + listTables.at(table)->tabName() + " VALUES (NULL";
       for (int i = 0; i < listTables.at(table)->tabNumEntries(); i++) s += ",?";
       s += ")";
    } else {
       s = "INSERT INTO " + listTables.at(table)->tabName() + " VALUES (NULL";
       for (int i = 0; i < listTables.at(table)->tabNumEntries(); i++) s += ",?";
       s += ", CURRENT_TIMESTAMP, 0)";
    }
    query.prepare(s);
    cti.bind4store(query);

    if ( query.exec() == false) {
	getError(query.executedQuery(), query.lastError());
	*id = -1;
	return false;
    }

    *id = query.lastInsertId().toInt();
    if (emit_signal) emit sigDBChange(table, OP_INSERT, -1);
    return true;
}


bool myDB::storeItem(cTableItem& cti, int *id)
{
    return storeItem(cti, id, true);
}


bool myDB::updateItem(cTableItem& cti, bool emit_signal)
{
    if (!db_handle.isOpen()) return false;
    int table = cti.getTableID();
    if ( (table < 0) || (table >= TABLE_DUMMY) ) return false;

    QSqlQuery query(db_handle);
    QString s = "UPDATE " + listTables.at(table)->tabName() + " SET ";
    s += listTables.at(table)->tabEntries().join("=?,");
    QString s1 = "=? WHERE delflag = 0 AND id=?";
    QString s2 = "=? WHERE id=?";
    query.prepare(s + s1);
    cti.bind4store(query);
    query.addBindValue(cti.getID());
    if (!query.exec()) {
        query.prepare(s + s2);
        cti.bind4store(query);
        query.addBindValue(cti.getID());
        QUERY_AND_CHECK();
    }
    if (emit_signal) emit sigDBChange(table, OP_UPDATE, cti.getID());
    return true;
}


bool myDB::updateItem(cTableItem& cti)
{
    return updateItem(cti, true);
}


bool myDB::deleteItem(int table, int id, bool emit_signal)
{
    if ( (table < 0) || (table >= TABLE_DUMMY) ) return false;
    if (!db_handle.isOpen()) return false;

    QSqlQuery query(db_handle);
    QString s = "SELECT delflag FROM " + listTables.at(table)->tabName();
    query.prepare(s);
    if (!query.exec())
       s = "DELETE FROM " + listTables.at(table)->tabName() + " WHERE id=?";
    else
       s = "UPDATE " + listTables.at(table)->tabName() + " SET delflag=1 WHERE id=?";
    query.prepare(s);
    query.addBindValue(id);
    QUERY_AND_CHECK();

    if (emit_signal) emit sigDBChange(table, OP_DELETE, id);
    return true;
}


bool myDB::deleteItem(int table, int id)
{
    return deleteItem(table, id, true);
}

bool myDB::deleteItemPid(int id)
{
    if (!db_handle.isOpen()) return false;

    QSqlQuery query(db_handle);
//    QString s = "DELETE FROM " + listTables.at(table)->tabName() + " WHERE id=?";
    QString s = "UPDATE findings SET delflag=1 WHERE pid=?";
    query.prepare(s);
    query.addBindValue(id);
    QUERY_AND_CHECK();

    emit sigDBChange(TABLE_FINDINGS, OP_DELETE, -1);

//    QString s = "DELETE FROM " + listTables.at(table)->tabName() + " WHERE id=?";
    s = "UPDATE receipes SET delflag=1 WHERE pid=?";
    query.prepare(s);
    query.addBindValue(id);
    QUERY_AND_CHECK();

    emit sigDBChange(TABLE_RECEIPES, OP_DELETE, -1);

//    QString s = "DELETE FROM " + listTables.at(table)->tabName() + " WHERE id=?";
    s = "UPDATE dates SET delflag=1 WHERE pid=?";
    query.prepare(s);
    query.addBindValue(id);
    QUERY_AND_CHECK();

    emit sigDBChange(TABLE_DATES, OP_DELETE, -1);

    return true;
}

bool myDB::deleteItemRid(int id)
{
    if (!db_handle.isOpen()) return false;

    QSqlQuery query(db_handle);
//    QString s = "DELETE FROM " + listTables.at(table)->tabName() + " WHERE id=?";
    QString s = "UPDATE dates SET delflag=1 WHERE rid=?";
    query.prepare(s);
    query.addBindValue(id);
    QUERY_AND_CHECK();

    emit sigDBChange(TABLE_DATES, OP_DELETE, -1);

    return true;
}


bool myDB::retrieveItemList(int table, QList<cTableItem *> &items)
{
    // TODO: consider ORDER list !!!
    if ( (table < 0) || (table >= TABLE_DUMMY) ) return false;
    if (!db_handle.isOpen()) return false;

    while (!items.isEmpty()) delete items.takeFirst();

    QSqlQuery query(db_handle);
    QString s = "SELECT delflag FROM " + listTables.at(table)->tabName();
    query.prepare(s);
    if (!query.exec())
        s = "SELECT * FROM " + listTables.at(table)->tabName();
    else
        s = "SELECT * FROM " + listTables.at(table)->tabName() + " WHERE delflag=0";
    if (table == TABLE_PATIENTS)
        s += " ORDER BY surname";

    query.prepare(s);
    QUERY_AND_CHECK();
    query.first();

    while (query.isValid()) {
	switch (table) {
#ifndef OPA
	    case TABLE_PHYSICIAN_TYPES: items.append(new cPhysType()); break;
	    case TABLE_EMPLOYEE_GROUPS: items.append(new cEmployeeGroup()); break;
	    case TABLE_EMPLOYEE_TYPES: items.append(new cEmployeeType()); break;
	    case TABLE_INDICATORS: items.append(new cIndicator()); break;
	    case TABLE_RECEIPE_STATES: items.append(new cReceipeState()); break;
	    case TABLE_INVOICE_STATS: items.append(new cInvoiceStat()); break;
        case TABLE_PINVOICE_STATS: items.append(new cPraxisInvoiceStat()); break;
	    case TABLE_ASSOC: items.append(new cAssoc()); break;
        case TABLE_CASHPOS: items.append(new cCashPosition()); break;
        case TABLE_DATE_TYPES: items.append(new cDateType()); break;
        case TABLE_DATE_STATES: items.append(new cDateState()); break;
        case TABLE_HM_CAT: items.append(new cHmCat()); break;
        case TABLE_HM_CAT_DOMAINS: items.append(new cHmCatDomain()); break;
        case TABLE_DATE_ACTIVITIES: items.append(new cDateActivity()); break;
        case TABLE_ZIPCODES: items.append(new cZipCode()); break;
#endif // OPA
        case TABLE_THERAPY_GROUPS: items.append(new cTherapyGroup()); break;
        case TABLE_TARIFF_AREAS: items.append(new cTariffArea()); break;
	    case TABLE_UNIONS: items.append(new cUnion()); break;
	    case TABLE_PROFESSIONS: items.append(new cProfession()); break;
	    case TABLE_INSURANCES: items.append(new cInsurance()); break;
	    case TABLE_INSURANCE_TYPES: items.append(new cInsuranceType()); break;
	    case TABLE_READER_DEVICES: items.append(new cReaderDevice()); break;
	    case TABLE_ROLES: items.append(new cRole()); break;
	    case TABLE_PROTECT: items.append(new cProtectedFunc()); break;
	    case TABLE_HMPOS: items.append(new cHMPos()); break;
        case TABLE_QUALIFICATIONS: items.append(new cQualification()); break;
        case TABLE_ROOMS: items.append(new cRoom()); break;
        case TABLE_CARS: items.append(new cCar()); break;
        case TABLE_RECEIPE_SHORTCUTS: items.append(new cReceipeShortcut()); break;
        case TABLE_CLINICS: items.append(new cClinic()); break;
        case TABLE_PRICETABLES: items.append(new cPriceTableNames()); break;
        case TABLE_TEMPLATE_TYPES: items.append(new cTemplateType()); break;
        case TABLE_TEMPLATE_DATA_CATEGORIES: items.append(new cTemplateDataCategory()); break;
        case TABLE_TEMPLATE_DATA_MAPPINGS: items.append(new cTemplateDataMapping()); break;
        case TABLE_TEMPLATES: items.append(new cTemplate()); break;
        case TABLE_TEMPLATE_AVAIL_DATA_CATEGORIES: items.append(new cTemplateAvailDataCategory()); break;
        case TABLE_TEXT_MODULES: items.append(new cTextModule()); break;
        case TABLE_SURGERY: items.append(new cSurgery()); break;
        case TABLE_SURGERY_BRANCHES: items.append(new cSurgeryBranch()); break;
        default: items.append(new cTableItem(table)); break;
	}
	items.last()->retrieve(query);
	query.next();
    }
    return true;
}


bool myDB::saveItemList(int table, QList<cTableItem *> &items, bool use_transaction)
{
    if ( (table < 0) || (table >= TABLE_DUMMY) ) return false;
    if (!db_handle.isOpen()) return false;

    QSqlQuery query(db_handle);
    if (use_transaction) {
        QString s;
        if (db_properties.getType() == cDatabase::DB_SQLITE) s = "BEGIN TRANSACTION";
        else s = "START TRANSACTION";
        query.prepare(s);
        QUERY_AND_CHECK();
    }

    bool bRet = true;

    for (int i = 0; i < items.count(); i++) {
        int f = items.at(i)->getFlag();
        if (f == cTableItem::FLAG_MODIFIED) {  // update entry
            if (!updateItem(*items.at(i), false)) bRet = false;
        } else if (f == cTableItem::FLAG_NEW) {  // insert new entry
            int new_id;
            if (storeItem(*items.at(i), &new_id, false)) items.at(i)->setID(new_id);
            else bRet = false;
        } else if (f == cTableItem::FLAG_DEL) {  // delete entry
            if (!deleteItem(table, items.at(i)->getID(), false)) bRet = false;
        }
    }

    if (use_transaction) {
        query.clear();
        QString s;
        if (db_properties.getType() == cDatabase::DB_SQLITE) s = "END TRANSACTION";
        else s = "COMMIT";
        query.prepare(s);
        QUERY_AND_CHECK();
    }

    for (int i = 0; i < items.count(); i++) {
        if (items.at(i)->getFlag() == cTableItem::FLAG_DEL) {
            delete items.at(i);
            items.removeAt(i);
        }
    }

    emit sigDBChange(table, OP_ANY, -1);
    return bRet;
}


bool myDB::executeBulk(QStringList& statements, bool use_transaction, bool stopAtFirstError)
{
    bool bRet = true;
    QSqlQuery query(db_handle);

    if (use_transaction) {
        QString s;
        if (db_properties.getType() == cDatabase::DB_SQLITE) s = "BEGIN TRANSACTION";
        else s = "START TRANSACTION";
        query.prepare(s);
        QUERY_AND_CHECK();
    }
    for (int i = 0; i < statements.count(); i++) {
        query.clear();
        if ( query.exec(statements.at(i)) == false) {
            getError(query.executedQuery(), query.lastError());
            if (stopAtFirstError) return false;
            else bRet = false;
        }

    }
    if (use_transaction) {
        query.clear();
        QString s;
        if (db_properties.getType() == cDatabase::DB_SQLITE) s = "END TRANSACTION";
        else s = "COMMIT";
        query.prepare(s);
        QUERY_AND_CHECK();
    }
    return bRet;
}


void myDB::updateDependencies(int table, int op)
{
    // here not used the sql dependecy mechanism!
    if (op == OP_DELETE) {
        switch(table) {
        case TABLE_PATIENTS:

        case TABLE_RECEIPES:

        default: break;
        }
        return;
    }
    return;

    // Consider the operation (parameter "op") since we use some
    // database triggers that will perform additional delete operations
    // after we have delete a certain object (such as a patient or receipe)
    // => we need to update certain tables
    // TODO: Check whether the database trigger runs asynchronously.
    //       If so, we will have a problem because the time we reload some
    //       tables might be too early (as the trigger might not be completed)
    //       Alternative: implements the triggers here instead of using db triggers
    if (op == OP_DELETE) {
        switch(table) {
        case TABLE_PATIENTS:
            emit sigDBChangeExternal(TABLE_FINDINGS, -1, -1);
            emit sigDBChangeExternal(TABLE_DATES, -1, -1);
            emit sigDBChangeExternal(TABLE_RECEIPES, -1, -1);
            break;
        case TABLE_RECEIPES:
            emit sigDBChangeExternal(TABLE_DATES, -1, -1);
            emit sigDBChangeExternal(TABLE_CASH_ADDPAY, -1, -1);
            emit sigDBChangeExternal(TABLE_CASH_SELFPAY, -1, -1);
            emit sigDBChangeExternal(TABLE_CASH_PRIVRECEIPE, -1, -1);
            break;
        default: break;
        }
        return;
    }

    // we need to consider some special cases, e.g. modifying a receipe
    // requires to update TABLE_RECEIPES as well as TABLE_CASH_ADDPAY
    if (table == TABLE_RECEIPES) {
        emit sigDBChangeExternal(TABLE_CASH_ADDPAY, -1, -1);
        emit sigDBChangeExternal(TABLE_CASH_SELFPAY, -1, -1);
        emit sigDBChangeExternal(TABLE_CASH_PRIVRECEIPE, -1, -1);
    }
    if (table == TABLE_CASH_ADDPAY) {
        emit sigDBChangeExternal(TABLE_RECEIPES, -1, -1);
        emit sigDBChangeExternal(TABLE_CASH_SELFPAY, -1, -1);
        emit sigDBChangeExternal(TABLE_CASH_PRIVRECEIPE, -1, -1);
    }
    if (table == TABLE_CASH_SELFPAY) {
        emit sigDBChangeExternal(TABLE_RECEIPES, -1, -1);
        emit sigDBChangeExternal(TABLE_CASH_ADDPAY, -1, -1);
        emit sigDBChangeExternal(TABLE_CASH_PRIVRECEIPE, -1, -1);
    }
    if (table == TABLE_CASH_PRIVRECEIPE) {
        emit sigDBChangeExternal(TABLE_RECEIPES, -1, -1);
        emit sigDBChangeExternal(TABLE_CASH_ADDPAY, -1, -1);
        emit sigDBChangeExternal(TABLE_CASH_SELFPAY, -1, -1);
    }
}


#ifndef OPA
void myDB::slotUpdateDB(int table, int op, int item_id)
{
    if (table == TABLE_EMPLOYEES) loadEmployees();
    else if (table == TABLE_ROOMS) {
        retrieveItemList(TABLE_ROOMS, reinterpret_cast<QList<cTableItem*>&>(rooms));
    } else if (table == TABLE_THERAPY_GROUPS) {
        retrieveItemList(TABLE_THERAPY_GROUPS, reinterpret_cast<QList<cTableItem*>&>(listTherapyGroups));
    } else if (table == TABLE_CARS) {
        retrieveItemList(TABLE_CARS, reinterpret_cast<QList<cTableItem*>&>(listCars));
    }

    if ( (table < 0) || (table >= TABLE_DUMMY) ) return;
    emit sigDBChangeExternal(table, op, item_id);
    updateDependencies(table, op);
}


bool myDB::loadPraxisInvoiceAppendix(QList<cAppendixItem *> &list, QString slist, QDate from, QDate until)
{
    QString s;
    QSqlQuery query(db_handle);
    QSqlQuery query1(db_handle);


    if (!db_handle.isOpen()) return false;

    if (!(slist.isEmpty())) {
       s = "SELECT id, pid, rdate, insurance, pricetab, positions, brutto, bill_date FROM receipes WHERE id IN ("
           + slist.replace(":", ",") + ") AND delflag=0";
       query.prepare(s);

       QUERY_AND_CHECK();
       query.executedQuery();
       query.first();
       while (query.isValid()) {
           list.append(new cAppendixItem());
           list.last()->retrieve(query);
           query.next();
       }
    } else {
        s = "SELECT receipes.id, receipes.pid, rdate, insurance, pricetab, positions, brutto, bill_date FROM receipes ";
        s = s + "LEFT JOIN dates ON dates.rid=receipes.id ";
        s = s + "WHERE receipes.state >= 2 AND dates.eid =1 AND dates.d_date >=? AND dates.d_date <=? AND receipes.delflag=0 AND dates.delflag=0 ";
        s = s + "GROUP BY receipes.id";

        //FIXME
        // eid and stae over parameters!!!

        query.prepare(s);
        query.addBindValue(from.toString("yyyy-MM-dd"));
        query.addBindValue(until.toString("yyyy-MM-dd"));
        QUERY_AND_CHECK();
        query.executedQuery();

        query.first();
        while (query.isValid()) {
            query1.prepare("SELECT d_date FROM dates WHERE rid=? AND d_date >? AND delflag=0");
            query1.addBindValue(query.value(0).toInt());
            query1.addBindValue(until.toString("yyyy-MM-dd"));
            if ( query1.exec() == false) { getError(query1.executedQuery(), query1.lastError()); return false; }
            query1.executedQuery();
            query1.last();
            if (!query1.isValid()) {
                list.append(new cAppendixItem());
                list.last()->retrieve(query);
            }
            query1.clear();
            query.next();
        }
    }
    return true;
}
#endif

bool myDB::saveBackupDB(QString filename)
{
    int ret = 1;

    if (!db_handle.isOpen()) return 0;
    QString s;
    QSqlQuery query(db_handle);

    s = "SELECT * FROM dates INTO OUTFILE \"" + filename +"dat.csv \" FIELDS TERMINATED BY '\t' ENCLOSED BY '\"' LINES TERMINATED BY '\r\n'";
    query.prepare(s);

    QUERY_AND_CHECK();

    query.executedQuery();
    if (query.isValid()) {
         ret = 0;
    }
    s = "SELECT * FROM patients INTO OUTFILE \"" + filename +"pat.csv \" FIELDS TERMINATED BY '\t' ENCLOSED BY '\"' LINES TERMINATED BY '\r\n'";
    query.prepare(s);

    QUERY_AND_CHECK();

    query.executedQuery();
    if (query.isValid()) {
         ret = 0;
    }
    s = "SELECT * FROM receipes INTO OUTFILE \"" + filename +"rec.csv \" FIELDS TERMINATED BY '\t' ENCLOSED BY '\"' LINES TERMINATED BY '\r\n'";
    query.prepare(s);

    QUERY_AND_CHECK();

    query.executedQuery();
    if (query.isValid()) {
         return 0;
    }

    return ret;
}
