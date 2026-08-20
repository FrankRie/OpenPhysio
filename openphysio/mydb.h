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

#ifndef MYDB_H_
#define MYDB_H_

#include <QtWidgets>
#include <QtSql>

#define REQUIRED_DB_VERSION 7
// #define USE_ASYNC_DB_ACCESS 1

#define RECEIPE_TYPE_GKV  0
#define RECEIPE_TYPE_PKV  1

#define PRICETABLE_TYPE_PRIVAT        3
#define PRICETABLE_TYPE_SELBSTZAHLER  4
#define PRICETABLE_TYPE_BEIHILFE      5
#define PRICETABLE_TYPE_HEILPRAKTIKER 6

#define GENDER_MALE   0
#define GENDER_FEMALE 1

#define THERAPIE_ITEMS  5

// bit positions of flags in option flags bit field of receipes
#define POS_FLAG_MANUAL_CALC     0
#define POS_FLAG_MANUAL_CALC_HB  1

//#define USE_ASYNC_DB_ACCESS 1

class cTable
{
private:
    QString tabname;
    QStringList listEntries;
    QStringList listHeaders;
    QStringList listQueryTables;
    QStringList listQueryItems;
    QStringList listQueryConditions;

public:
    cTable();
    virtual ~cTable();
    void init(QString s);
    int tabNumEntries();
    QString tabName();
    QStringList tabEntries();
    QStringList& operator<< (const QString & str);
    void setTableProperties(QStringList *qslHeaders, QStringList *qslTables,
			    QStringList *qslItems, QStringList *qslConditions);
    void setHeaders(QStringList *qsl);
    void setQueryTables(QStringList *qsl);
    void setQueryItems(QStringList *qsl);
    void setQueryConditions(QStringList *qsl);
    QStringList& getHeaders();
    QStringList& getQueryTables();
    QStringList& getQueryItems();
    QStringList& getQueryConditions();
};


class cTableItem
{
private:
    int id, table, flag;

public:
    enum flags { FLAG_ORG, FLAG_MODIFIED, FLAG_NEW, FLAG_DEL };
    cTableItem(int t);
    virtual ~cTableItem();
    virtual void reset();
    int getID();
    void setID(int i);
    int getTableID();
    void setFlag(int i);
    int getFlag();
    virtual void retrieve(QSqlQuery& query);
    virtual void bind4store(QSqlQuery& query);
    virtual QString getProperty(QString strProp);
};


class cSurgery : public cTableItem
{
private:
    QString name1, name2, street, city, zip, tel, mobile, fax, email, website;
    QString server, name, password;
    QString ik_physio, ik_ergo, ik_logo;
    QString bank, bankcode, account, taxid, sales_taxid, hrb;
    QString s_union;
    int group, area;
    bool tax_liability;
    int tax_percent;

public:
    cSurgery();
    virtual ~cSurgery();
    void reset();
    cSurgery& operator=(cSurgery& other);

    void setName1(QString s);
    QString getName1();
    void setName2(QString s);
    QString getName2();
    void setStreet(QString s);
    QString getStreet();
    void setCity(QString s);
    QString getCity();
    void setZip(QString s);
    QString getZip();
    void setTel(QString s);
    QString getTel();
    void setMobile(QString s);
    QString getMobile();
    void setFax(QString s);
    QString getFax();
    void setEMail(QString s);
    QString getEMail();
    void setWebsite(QString s);
    QString getWebsite();
    void setServer(QString s);
    QString getServer();
    void setName(QString s);
    QString getName();
    void setPassword(QString s);
    QString getPassword();
    void setPhysioIK(QString s);
    QString getPhysioIK();
    void setErgoIK(QString s);
    QString getErgoIK();
    void setLogoIK(QString s);
    QString getLogoIK();
    void setProfession(int i);
    int getProfession();
    void setArea(int i);
    int getArea();
    void setBank(QString s);
    QString getBank();
    void setBankCode(QString s);
    QString getBankCode();
    void setAccount(QString s);
    QString getAccount();
    void setUnion(QString s);
    QString getUnion();
    void setTaxID(QString s);
    QString getTaxID();
    void setSalesTaxID(QString s);
    QString getSalesTaxID();
    void setTradeRegister(QString s);
    QString getTradeRegister();
    void setTaxLiability(bool b);
    bool getTaxLiability();
    void setTaxPercent(int i);
    int getTaxPercent();

    virtual void retrieve(QSqlQuery& query);
    virtual void bind4store(QSqlQuery& query);

    virtual QString getProperty(QString strProp);
    int getUnionID(int profession);
    bool isProfession(int profession);
};


class cSurgeryBranch : public cTableItem
{
private:
    QString name1, name2, street, city, zip, tel, mobile, fax, ik;

public:
    cSurgeryBranch();
    virtual ~cSurgeryBranch();
    void reset();
    cSurgeryBranch& operator=(cSurgeryBranch& other);

    void setName1(QString s);
    QString getName1();
    void setName2(QString s);
    QString getName2();
    void setStreet(QString s);
    QString getStreet();
    void setCity(QString s);
    QString getCity();
    void setZip(QString s);
    QString getZip();
    void setTel(QString s);
    QString getTel();
    void setMobile(QString s);
    QString getMobile();
    void setFax(QString s);
    QString getFax();
    void setIK(QString s);
    QString getIK();

    virtual void retrieve(QSqlQuery& query);
    virtual void bind4store(QSqlQuery& query);
};


class cPatient : public cTableItem
{
private:
    int gender, ins_type, group, default_employee, care_facility, distance, state;
    int def_phys, addpay_freed;
    QString surname, forename, addname, title, street, city, zip, tel, tel_job, tel_mobile, fax, email, insurance;
    QString vnr, kvknr, vknr, notices, status;
    QDate kvk_valid, birthday, freed_from, freed_until, date_contract, date_plan;

public:
    enum gender { MALE = 0, FEMALE };
    enum stat { STATE_ACTIVE = 0, STATE_INACTIVE };
    cPatient();
    virtual ~cPatient();
    void reset();
    cPatient& operator=(cPatient& other);
    void setName(QString qs);
    void setForename(QString qs);
    void setAddName(QString qs);
    void setTitle(QString qs);
    void setBirthday(QDate qd);
    void setGender(int i);
    void setStreet(QString qs);
    void setCity(QString qs);
    void setZipcode(QString qs);
    void setTelephone(QString qs);
    void setTelephoneJob(QString qs);
    void setTelephoneMobile(QString qs);
    void setFax(QString qs);
    void setEMail(QString qs);
    void setInsurance(QString qs);
    void setVNR(QString qs);
    void setKVK(QString qs);
    void setValidKVK(QDate qd);
    void setVKNR(QString qs);
    void setGroup(int i);
    void setInsType(int i);
    void setNotices(QString qs);
    void setStatus(QString qs);
    void setCareFacility(int i);
    void setDefaultEmployeeID(int i);
    void setDefaultPhysicianID(int i);
    void setDistance(int i);
    void setState(int i);
    void setAddpayFreed(int i);
    void setFreedFrom(QDate d);
    void setFreedUntil(QDate d);
    void setContract(QDate d);
    void setPlan(QDate d);

    QString getName();
    QString getForename();
    QString getAddName();
    QString getTitle();
    QDate getBirthday();
    int getGender();
    QString getStreet();
    QString getCity();
    QString getZipcode();
    QString getTelephone();
    QString getTelephoneJob();
    QString getTelephoneMobile();
    QString getFax();
    QString getEMail();
    QString getInsurance();
    QString getVNR();
    QString getKVK();
    QDate getValidKVK();
    QString getVKNR();
    int getGroup();
    int getInsType();
    QString getNotices();
    QString getStatus();
    int getCareFacility();
    int getDefaultEmployeeID();
    int getDefaultPhysicianID();
    int getDistance();
    int getState();
    int getAddpayFreed();
    QDate getFreedFrom();
    QDate getFreedUntil();
    QDate getContract();
    QDate getPlan();

    QString getFullName();

    virtual void retrieve(QSqlQuery& query);
    virtual void bind4store(QSqlQuery& query);

    virtual QString getProperty(QString strProp);
};


class cTherapyGroup : public cTableItem
{
private:
    QString t_name, t_desc;

public:
    cTherapyGroup();
    void setName(QString s);
    QString getName();
    void setDescription(QString s);
    QString getDescription();
    virtual void retrieve(QSqlQuery& query);
    virtual void bind4store(QSqlQuery& query);
};


#ifndef OPA
class cEmployeeGroup : public cTableItem
{
private:
    QString tdesc;

public:
    enum groups { GROUP_ALL = 0, GROUP_THERAPY, GROUP_OFFICE, GROUP_CLEAN, GROUP_OTHER };
    cEmployeeGroup();
    void setDescription(QString s);
    QString getDescription();
    virtual void retrieve(QSqlQuery& query);
    virtual void bind4store(QSqlQuery& query);
};
#endif // OPA


#ifndef OPA
class cEmployeeType : public cTableItem
{
private:
    QString tdesc;

public:
    cEmployeeType();
    void setDescription(QString s);
    QString getDescription();
    virtual void retrieve(QSqlQuery& query);
    virtual void bind4store(QSqlQuery& query);
};
#endif // OPA

#ifndef OPA

class cEmployee : public cTableItem
{
private:
    int id, etype, id_grp, id_tgrp, flag, gender, state;
    QString surname, forename, street, city, zip, tel, tel_mobile, fax, email, shortname, persnr;
    QDate birthday;
    QList<int> listQuali;

public:
    enum estates { STATE_ACTIVE = 0, STATE_INACTIVE };
    cEmployee();
    virtual ~cEmployee();
    void reset();
    cEmployee& operator=(cEmployee& other);
    void setName(QString qs);
    void setForename(QString qs);
    void setBirthday(QDate qd) ;
    void setStreet(QString qs);
    void setCity(QString qs);
    void setZipcode(QString qs);
    void setTelephone(QString qs);
    void setTelephoneMobile(QString qs);
    void setFax(QString qs);
    void setEMail(QString qs);
    void setShortName(QString qs);
    void setType(int s);
    void setGroupID(int i);
    void setTherapyGroupID(int i);
    void setFlag(int i);
    void setPersonellNumber(QString s);
    QString getName();
    QString getForename();
    QDate getBirthday();
    QString getStreet();
    QString getCity();
    QString getZipcode();
    QString getTelephone();
    QString getTelephoneMobile();
    QString getFax();
    QString getEMail();
    QString getShortName();
    int getType();
    int getGroupID();
    int getTherapyGroupID();
    int getFlag();
    QString getPersonellNumber();
    QList<int>& getQualifications();
    void setQualifications(QList<int> l);
    int getGender();
    void setGender(int i);
    int getState();
    void setState(int i);

    QString getFullName();
    virtual void retrieve(QSqlQuery& query);
    virtual void bind4store(QSqlQuery& query);

    virtual QString getProperty(QString strProp);
};

#endif // OPA



class cInsuranceType : public cTableItem
{
private:
    QString name;

public:
    enum itypes { IT_GKV = 0, IT_PKV, IT_BEIHILFE };
    cInsuranceType();
    void setDescription(QString s);
    QString getDescription();
    virtual void retrieve(QSqlQuery& query);
    virtual void bind4store(QSqlQuery& query);
};


class cInsurance : public cTableItem
{
private:
    QString ik;
    QString name_short;
    QString vdt;
    QString fkt;
    QString kto;
    QString vkg;
    QString nam;
    QString ans1, ans2, ans3;
    QString asp1, asp2, asp3;
    QString uem1, uem2;
    QString dfu1, dfu2;
    int assoc;

public:
    cInsurance();
    virtual ~cInsurance();
    void reset();
    cInsurance& operator=(cInsurance& other);
    void setAssoc(int i);
    int getAssoc();
    void setIK(QString qs);
    QString getIK();
    void setShortName(QString qs);
    QString getShortName();

    void setVDT(QString s);
    QString getVDT();
    void setFKT(QString s);
    QString getFKT();
    void setKTO(QString s);
    QString getKTO();
    void setVKG(QString s);
    QString getVKG();
    void setNAM(QString s);
    QString getNAM();
    void setANS1(QString s);
    QString getANS1();
    void setANS2(QString s);
    QString getANS2();
    void setANS3(QString s);
    QString getANS3();
    void setASP1(QString s);
    QString getASP1();
    void setASP2(QString s);
    QString getASP2();
    void setASP3(QString s);
    QString getASP3();
    void setUEM1(QString s);
    QString getUEM1();
    void setUEM2(QString s);
    QString getUEM2();
    void setDFU1(QString s);
    QString getDFU1();
    void setDFU2(QString s);
    QString getDFU2();

    virtual void retrieve(QSqlQuery& query);
    virtual void bind4store(QSqlQuery& query);
};


class cRole : public cTableItem
{
private:
    QString name, pw;

public:
    cRole();
    cRole(int i, QString n, QString p);
    virtual ~cRole();
    void reset();
    cRole& operator=(cRole& other);
    void setName(QString s);
    QString getName();
    void setPW(QString s);
    QString getPW();

    virtual void retrieve(QSqlQuery& query);
    virtual void bind4store(QSqlQuery& query);
};


#ifndef OPA

class cReport : public cTableItem
{
private:
    int pid, rid, phys_id, type, eid;
    QDate r_date;
    QByteArray rdata;

public:
    cReport();
    virtual ~cReport();
    void reset();
    cReport& operator=(cReport& other);
    int getReceipeID();
    void setReceipeID(int i);
    int getPatientID();
    void setPatientID(int i);
    int getPhysID();
    void setPhysID(int i);
    QDate getDate();
    void setDate(QDate t);
    int getType();
    void setType(int i);
    QByteArray& getRData();
    void setRData(QByteArray& buf);
    int getAuthorID();
    void setAuthorID(int i);

    virtual void retrieve(QSqlQuery& query);
    virtual void bind4store(QSqlQuery& query);

    virtual QString getProperty(QString strProp);
};

#endif // OPA

#ifndef OPA

class cFinding : public cTableItem
{
private:
    int pid, eid;
    QDate f_date;
    QString fdata;

public:
    cFinding();
    virtual ~cFinding();
    void reset();
    cFinding& operator=(cFinding& other);
    int getPatientID();
    void setPatientID(int i);
    int getEmplID();
    void setEmplID(int i);
    QDate getDate();
    void setDate(QDate t);
    QString getFData();
    void setFData(QString buf);

    virtual void retrieve(QSqlQuery& query);
    virtual void bind4store(QSqlQuery& query);
};

#endif // OPA

#ifndef OPA

class cReminder : public cTableItem
{
private:
    int iid, pid, rid, cid, xid;
    QDate r_date;
    QByteArray rdata;

public:
    cReminder();
    virtual ~cReminder();
    void reset();
    cReminder& operator=(cReminder& other);
    int getInvoiceID();
    void setInvoiceID(int i);
    int getPraxisInvoiceID();
    void setPraxisInvoiceID(int i);
    int getPatientID();
    void setPatientID(int i);
    int getReceipeID();
    void setReceipeID(int i);
    int getCourseID();
    void setCourseID(int i);
    int getPraxisID();
    void setPraxisID(int i);
    QDate getDate();
    void setDate(QDate t);
    QByteArray& getRData();
    void setRData(QByteArray& buf);

    virtual void retrieve(QSqlQuery& query);
    virtual void bind4store(QSqlQuery& query);

    virtual QString getProperty(QString strProp);
};

#endif // OPA

#ifndef OPA

class cQuittance : public cTableItem
{
private:
    int pid, rid, type;
    QString q_nr;
    QDate q_date;
    QByteArray qdata;

public:
    enum qtypes { Q_ADDPAY = 0, Q_COURSE, Q_LOSS, Q_SELFPAY, Q_PRIVRECEIPE, Q_VOUCHER, Q_ARTICLE };
    cQuittance();
    virtual ~cQuittance();
    void reset();
    cQuittance& operator=(cQuittance& other);
    int getReceipeID();
    void setReceipeID(int i);
    int getPatientID();
    void setPatientID(int i);
    QDate getDate();
    void setDate(QDate t);
    QString getNumber();
    void setNumber(QString s);
    int getType();
    QString getTypeString();
    void setType(int i);
    QByteArray& getQData();
    void setQData(QByteArray& buf);

    virtual void retrieve(QSqlQuery& query);
    virtual void bind4store(QSqlQuery& query);

    virtual QString getProperty(QString strProp);
};

#endif

#ifndef OPA

class cCourse : public cTableItem
{
private:
    int state, duration, leader, units;
    QString name, price;
    QList<int> pidlist;
    QDate c_start, c_end;
    QTime c_clock;

public:
    cCourse();
    virtual ~cCourse();
    void reset();
    cCourse& operator=(cCourse& other);
    QString getName();
    void setName(QString s);
    QDate getStartDate();
    void setStartDate(QDate t);
    QDate getEndDate();
    void setEndDate(QDate t);
    int getState();
    void setState(int s);
    int getLeaderID();
    void setLeaderID(int l);
    int getDuration();
    void setDuration(int d);
    QString getPrice();
    void setPrice(QString p);
    QList<int> getPidList();
    void setPidList(QList<int> l);
    int getUnits();
    void setUnits(int u);
    QTime getStartTime();
    void setStartTime(QTime t);

    void addPid(int pid);
    void delPidAt(int pos);
    void delPid(int pid);
    QString getAmount();

    virtual void retrieve(QSqlQuery& query);
    virtual void bind4store(QSqlQuery& query);

    virtual QString getProperty(QString strProp);
};

#endif // OPA


#ifndef OPA

class cInvoice : public cTableItem
{
private:
    int rid, pid, cid, xid, type, paystat;
    QString i_nr, fname, amount, rlist;
    QDate i_date, s_date, e_date, maildate;
    QByteArray invdata;

public:
    enum invoice_types { INVOICE_RECEIPE = 0, INVOICE_COURSE, INVOICE_SHORTFAIL, INVOICE_PRAXIS, INVOICE_OTHER };
    cInvoice();
    virtual ~cInvoice();
    void reset();
    cInvoice& operator=(cInvoice& other);
    int getReceipeID();
    void setReceipeID(int i);
    int getPatientID();
    void setPatientID(int i);
    int getCourseID();
    void setCourseID(int i);
    int getPraxisID();
    void setPraxisID(int i);
    int getType();
    void setType(int i);
    int getPayStat();
    void setPayStat(int i);
    QDate getDate();
    void setDate(QDate t);
    QString getNumber();
    void setNumber(QString s);
    QString getAmount();
    void setAmount(QString s);
    QByteArray& getInvData();
    void setInvData(QByteArray& buf);
    QDate getSdate();
    void setSdate(QDate t);
    QDate getEdate();
    void setEdate(QDate t);
    QDate getMailDate();
    void setMailDate(QDate t);
    QString getRlist();
    void setRlist(QString s);

    QString getFileName();

    virtual void retrieve(QSqlQuery& query);
    virtual void bind4store(QSqlQuery& query);

    virtual QString getProperty(QString strProp);
};

#endif // OPA

#ifndef OPA

class cPraxisInvoice : public cTableItem
{
private:
    int rid, pid, cid, xid, type, paystat;
    QString i_nr, fname, amount, rlist;
    QDate i_date, s_date, e_date;
    QByteArray invdata;

public:
    enum invoice_types { INVOICE_RECEIPE = 0, INVOICE_COURSE, INVOICE_SHORTFAIL, INVOICE_PRAXIS, INVOICE_OTHER };
    cPraxisInvoice();
    virtual ~cPraxisInvoice();
    void reset();
    cPraxisInvoice& operator=(cPraxisInvoice& other);
    int getReceipeID();
    void setReceipeID(int i);
    int getPatientID();
    void setPatientID(int i);
    int getCourseID();
    void setCourseID(int i);
    int getPraxisID();
    void setPraxisID(int i);
    int getType();
    void setType(int i);
    int getPayStat();
    void setPayStat(int i);
    QDate getDate();
    void setDate(QDate t);
    QString getNumber();
    void setNumber(QString s);
    QString getAmount();
    void setAmount(QString s);
    QByteArray& getInvData();
    void setInvData(QByteArray& buf);
    QDate getSdate();
    void setSdate(QDate t);
    QDate getEdate();
    void setEdate(QDate t);
    QString getRlist();
    void setRlist(QString s);

    QString getFileName();

    virtual void retrieve(QSqlQuery& query);
    virtual void bind4store(QSqlQuery& query);

    virtual QString getProperty(QString strProp);
};

#endif // OPA

#ifndef OPA
class cPhysType : public cTableItem
{
private:
    QString descr;
public:
    cPhysType();
    cPhysType& operator=(cPhysType& other);
    void setDescription(QString s);
    QString getDescription();
    virtual void retrieve(QSqlQuery& query);
    virtual void bind4store(QSqlQuery& query);
};

#endif // OPA

class cPraxis : public cTableItem
{
private:
    int snum, prof, gender, tariff_area, eid, ppart;
    QString name1, name2, surname, forename, title, street;
    QString city, zip, tel, mobile, fax, email;
    QString web, ik;

public:
    cPraxis();
    void reset();
    cPraxis& operator=(cPraxis& other);
    void setSnum(int);
    void setName1(QString);
    void setName2(QString);
    void setSurname(QString);
    void setForename(QString);
    void setTitle(QString);
    void setGender(int);
    void setStreet(QString);
    void setCity(QString);
    void setZip(QString);
    void setTel(QString);
    void setMobile (QString);
    void setFax (QString);
    void setEmail(QString);
    void setWeb(QString);
    void setProf(int) ;
    void setTariffArea(int);
    void setIk(QString);
    void setEid(int);
    void setPpart(int);

    int getSnum();
    QString getName1();
    QString getName2();
    QString getSurname();
    QString getForename();
    QString getTitle();
    int getGender();
    QString getStreet();
    QString getCity();
    QString getZip();
    QString getTel();
    QString getMobile();
    QString getFax();
    QString getEmail();
    QString getWeb() ;
    int getProf();
    int getTariffArea();
    QString getIk();
    int getEid();
    int getPpart();

    virtual void retrieve(QSqlQuery& query);
    virtual void bind4store(QSqlQuery& query);

    virtual QString getProperty(QString strProp);
};


class cPhys : public cTableItem
{
private:
    int gender, ptype, clinic;
    QString surname, forename, surgery, title, street, city, zip, tel, tel_mobile, fax, email;
    QString bsnr, lanr;

public:
    cPhys();
    void reset();
    cPhys& operator=(cPhys& other);
    void setName(QString qs);
    void setForename(QString qs);
    void setSurgery(QString qs);
    void setTitle(QString qs);
    void setGender(int i);
    void setStreet(QString qs);
    void setCity(QString qs);
    void setZipcode(QString qs);
    void setTelephone(QString qs);
    void setTelephoneMobile(QString qs);
    void setFax(QString qs);
    void setEMail(QString qs);
    void setPhysType(int i);
    void setBSNR(QString s);
    void setLANR(QString s);
    void setClinicID(int i);

    QString getName();
    QString getForename();
    QString getSurgery();
    QString getTitle();
    int getGender();
    QString getStreet();
    QString getCity();
    QString getZipcode();
    QString getTelephone();
    QString getTelephoneMobile();
    QString getFax();
    QString getEMail();
    int getPhysType();
    QString getBSNR();
    QString getLANR();
    int getClinicID();

    virtual void retrieve(QSqlQuery& query);
    virtual void bind4store(QSqlQuery& query);

    virtual QString getProperty(QString strProp);
};


class cClinic : public cTableItem
{
private:
    QString name1, name2, street, city, zip, tel, fax, email;

public:
    cClinic();
    void reset();
    cClinic& operator=(cClinic& other);
    void setName1(QString qs);
    void setName2(QString qs);
    void setStreet(QString qs);
    void setCity(QString qs);
    void setZipcode(QString qs);
    void setTelephone(QString qs);
    void setFax(QString qs);
    void setEMail(QString qs);

    QString getName1();
    QString getName2();
    QString getStreet();
    QString getCity();
    QString getZipcode();
    QString getTelephone();
    QString getFax();
    QString getEMail();

    virtual void retrieve(QSqlQuery& query);
    virtual void bind4store(QSqlQuery& query);
};


#ifndef OPA
class cIndicator : public cTableItem
{
private:
    QString ind;
public:
    cIndicator();
    void setDescription(QString s);
    QString getDescription();
    virtual void retrieve(QSqlQuery& query);
    virtual void bind4store(QSqlQuery& query);
};
#endif // OPA


#ifndef OPA
class cReceipeState : public cTableItem
{
private:
    QString descr;
public:
    enum receipe_states { RECEIPE_STATE_RUNNING = 0, RECEIPE_STATE_CANCELED,
                          RECEIPE_STATE_READY ,RECEIPE_STATE_SETTLED };
    cReceipeState();
    void setDescription(QString s);
    QString getDescription();
    virtual void retrieve(QSqlQuery& query);
    virtual void bind4store(QSqlQuery& query);
};
#endif // OPA


class cReceipeTemplate : public cTableItem
{
private:
    QString name;
    QByteArray tdata;
public:
    cReceipeTemplate();
    void setName(QString s);
    QString getName();
    QByteArray& getTemplateData();
    void setTemplateData(QByteArray& buf);

    virtual void retrieve(QSqlQuery& query);
    virtual void bind4store(QSqlQuery& query);
};


class cReceipeShortcut : public cTableItem
{
private:
    int prof, amount;
    QString descr, posnr;
public:
    cReceipeShortcut();
    void reset();
    void setProfession(int i);
    int getProfession();
    void setAmount(int i);
    int getAmount();
    void setDescription(QString s);
    QString getDescription();
    void setPosNr(QString s);
    QString getPosNr();
    virtual void retrieve(QSqlQuery& query);
    virtual void bind4store(QSqlQuery& query);
};


class cReceipe : public cTableItem
{
private:
    QString insurant, ik, diag, pos, brutto, addpay;
    int rtype, rstate, pid, physid, ind, f_charge, f_visit, f_group, f_accident, f_report, tos, dist, pricetab, oflags;
    int addpay_flag, hb_count, prof, hb_pos_flat, hb_pos_dist;
    QString pt_version;
    QDate rdate, sdate, addpay_date, bill_date;
    QString notes;

public:
    enum tos { ERSTVO = 0, FOLGEVO, OUT_OF_RULE };
    cReceipe();
    virtual ~cReceipe();
    cReceipe& operator=(cReceipe& other);
    void reset();
    QString getInsurant();

    int getType();
    void setType(int t);
    int getState();
    void setState(int s);
    int getPatientID();
    void setPatientID(int p);
    int getPhysID();
    void setPhysID(int p);
    QString getIK();
    void setIK(QString s);
    QDate getDate();
    void setDate(QDate d);
    QDate getStartDate();
    void setStartDate(QDate d);
    int getIndID();
    void setIndID(int i);
    QString getDiagnosis();
    void setDiagnosis(QString s);
    int getFlagCharge();
    void setFlagCharge(int f);
    int getFlagVisit();
    void setFlagVisit(int f);
    int getFlagGroup();
    void setFlagGroup(int f);
    int getFlagAccident();
    void setFlagAccident(int f);
    int getFlagReport();
    void setFlagReport(int f);
    int getTOS();
    void setTOS(int t);
    int getDistance();
    void setDistance(int d);
    QString getPositions();
    void setPositions(QString p);
    QString getBrutto();
    void setBrutto(QString b);
    QString getAddpay();
    void setAddpay(QString b);
    void setPriceTabID(int t);
    int getPriceTabID();
    void setOptionFlags(int i);
    int getOptionFlags();
    void setAddPayFlag(int i);
    int getAddPayFlag();
    void setAddPayDate(QDate d);
    QDate getAddPayDate();
    void setBillDate(QDate d);
    QDate getBillDate();
    QString getPricetableVersion();
    void setPricetableVersion(QString v);
    int getHBCount();
    void setHBCount(int i);
    int getProfession();
    void setProfession(int i);
    QString getNotes();
    void setNotes(QString s);
    int getHBPosFlat();
    void setHBPosFlat(int i);
    int getHBPosDist();
    void setHBPosDist(int i);

    bool isSetManualCalc();
    void setManualCalc(bool b);
    bool isSetManualCalcHB();
    void setManualCalcHB(bool b);

    virtual void retrieve(QSqlQuery& query);
    virtual void bind4store(QSqlQuery& query);

    virtual QString getProperty(QString strProp);
};


#ifndef OPA


class cDateType : public cTableItem
{
private:
    QString dt_desc;

public:
    enum date_types { DT_THERAPY = 0, DT_COURSE, DT_MEETING, DT_TRAINING, DT_HOLIDAY,
                      DT_CORPORATE_HOLIDY, DT_BREAK, DT_OTHER, DT_DRIVE, DT_COUNT, DT_PATIENT };
    cDateType();
    ~cDateType();
    void setDescription(QString s);
    QString getDescription();
    virtual void retrieve(QSqlQuery& query);
    virtual void bind4store(QSqlQuery& query);
};

class cDateState : public cTableItem
{
private:
    QString descr;
public:
    enum date_states { DS_TERM = 0, DS_DONE, DS_CANCEL, DS_REJECT };
    cDateState();
    void setDescription(QString s);
    QString getDescription();
    virtual void retrieve(QSqlQuery& query);
    virtual void bind4store(QSqlQuery& query);
};

class cDateActivity : public cTableItem
{
private:
    int a_id, prof;
    bool a_enabled;
    QString a_name;

public:
    cDateActivity();
    void setProfession(int i);
    int getProfession();
    void setActivityID(int i);
    int getActivityID();
    void setActivityName(QString s);
    QString getActivityName();
    void setActivityEnabled(bool b);
    bool getActivityEnabled();
    virtual void retrieve(QSqlQuery& query);
    virtual void bind4store(QSqlQuery& query);
};

class cMetaDate : public cTableItem
{
private:
    QDate md_start, md_end;
    int flag_repeat;

public:
    cMetaDate();
    ~cMetaDate();
    cMetaDate& operator=(cMetaDate& other);
    void reset();
    void setStart(QDate d);
    void setEnd(QDate d);
    void setRepeatFlag(int i);
    QDate getStart();
    QDate getEnd();
    int getRepeatFlag();

    virtual void retrieve(QSqlQuery& query);
    virtual void bind4store(QSqlQuery& query);
};

class cAppendixItem : public cTableItem
{
private:
    int id, pricetab, pid;
    QString insurance, positions, brutto;
    QDate rdate, bill_date;

public:
    cAppendixItem();
    ~cAppendixItem();
    void reset();
    void setId(int i);
    int getId();
    void setPid(int i);
    int getPid();
    void setRdate(QDate d);
    QDate getRdate();
    void setInsurance(QString s);
    QString getInsurance();
    void setPricetab(int i);
    int getPricetab();
    void setPositions(QString s);
    QString getPositions();
    void setBrutto(QString s);
    QString getBrutto();
    void setBillDate(QDate d);
    QDate getBillDdate();

    cAppendixItem& operator=(cAppendixItem& other);
    virtual void retrieve(QSqlQuery& query);
};


class cDateItem : public cTableItem
{
private:
    int d_type, eid, pid, rid, cid, xid, room, act_physio, act_ergo, car, mid, state, options;
    QDate date;
    QTime t_start, t_end;
    QString desc, content, title, activities;
    bool full_day;

public:
    enum options { OPT_VISIT = 0 };
    cDateItem();
    ~cDateItem();
    void reset();
    void setType(int i);
    int getType();
    void setEID(int i);
    int getEID();
    void setPID(int i);
    int getPID();
    void setRID(int i);
    int getRID();
    void setCID(int i);
    int getCID();
    void setXID(int i);
    int getXID();
    void setDate(QDate d);
    QDate getDate();
    void setStart(QTime t);
    QTime getStart();
    void setEnd(QTime t);
    QTime getEnd();
    void setTitle(QString s);
    QString getTitle();
    void setDesc(QString s);
    QString getDesc();
    void setContent(QString s);
    QString getContent();
    void setRoomID(int i);
    int getRoomID();
    void setFullDay(bool b);
    bool getFullDay();
    void setActPhysio(int i);
    int getActPhysio();
    void setActErgo(int i);
    int getActErgo();
    void setCarID(int i);
    int getCarID();
    void setMetaID(int i);
    int getMetaID();
    void setStateID(int i);
    int getStateID();
    void setOptions(int i);
    int getOptions();

    QString getTitleAndTime();
    QString getTitle(bool incl_activities, bool incl_time, bool line_break = true);
    void decodeActivities();
    QString getActivityString();
    cDateItem& operator=(cDateItem& other);
    bool overlap(QTime t1, QTime t2);

    virtual void retrieve(QSqlQuery& query);
    virtual void bind4store(QSqlQuery& query);
};


#endif // OPA


class cAssoc : public cTableItem
{
private:
    QString name_short, name_long;
public:
    // assocs must be database entries
    enum assocs { AOK = 1, BKK, IKK, LKK, KBS, VDEK, BG, POSTA, BPOL, ASSOC_COUNT };
    cAssoc();
    ~cAssoc();
    void reset();
    void setNameShort(QString s);
    void setNameLong(QString s);
    QString getNameShort();
    QString getNameLong();

    virtual void retrieve(QSqlQuery& query);
    virtual void bind4store(QSqlQuery& query);
};



class cHMPos : public cTableItem
{
private:
    QString posnr, category, descr_short, descr_long;
    int flag_physio, flag_masseur, flag_logo, flag_ergo;
public:
    cHMPos();
    ~cHMPos();
    void reset();
    void setPosNr(QString s);
    QString getPosNr();
    void setCategory(QString s);
    QString getCategory();
    void setDescriptionShort(QString s);
    QString getDescriptionShort();
    void setDescriptionLong(QString s);
    QString getDescriptionLong();
    void setFlagPhysio(int i);
    int getFlagPhysio();
    void setFlagMasseur(int i);
    int getFlagMasseur();
    void setFlagLogo(int i);
    int getFlagLogo();
    void setFlagErgo(int i);
    int getFlagErgo();

    virtual void retrieve(QSqlQuery& query);
    virtual void bind4store(QSqlQuery& query);
};


class cPriceTableNames : public cTableItem
{
private:
    int prof, area, assoc;
    QString unions, name, descr;
    QDate valid_from;

public:
    cPriceTableNames();
    ~cPriceTableNames();
    void reset();
    void setProfession(int i);
    int getProfession();
    void setTariffArea(int i);
    int getTariffArea();
    void setAssoc(int i);
    int getAssoc();
    void setUnions(QString s);
    QString getUnions();
    void setName(QString s);
    QString getName();
    void setDescription(QString s);
    QString getDescription();
    void setValidFrom(QDate d);
    QDate getValidFrom();

    virtual void retrieve(QSqlQuery& query);
    virtual void bind4store(QSqlQuery& query);
};


class cPriceTableItem
{
private:
    QString pos_nr, price, addpay;
    int ref_id;

public:
    cPriceTableItem();
    cPriceTableItem(QString pos, QString p, QString a);
    ~cPriceTableItem();
    void setPosNr(QString s);
    QString getPosNr();
    void setPrice(QString s);
    QString getPrice();
    void setAddpay(QString s);
    QString getAddpay();
    void setRefID(int i);
    int getRefID();

    QString getPosDescr();
    QString getPosCategory();
};


class cPriceTable
{
private:
    QList<cPriceTableItem*> items;
    bool hb_flat;
    int id_hb, id_hb_group, id_distance, id_report;
    QList<int> listPosHB;
public:
    cPriceTable();
    ~cPriceTable();
    void reset();
    bool isFlatHB();
    int posHB();
    int posHBgrp();
    int posHBdist();
    int posReport();
    QString getPosNr(int i);
    QString strPosHB();
    QString strPosHBgrp();
    QString strPosHBdist();
    QString strPosReport();

    void append(QString pos, QString pos_price, QString pos_addpay);
    cPriceTableItem* at(int i);
    int indexOf(QString pos_nr);
    QString getPrice(int i);
    QString getAddpay(int i);
    QStringList getPositionDescriptions();
    QStringList getPositionNumbers();
    QStringList getHBPositionNumbers();
    void prepare(int prof);
    void clear();
};


class cPriceTablePriv
{
private:
    int id, pt_type;
    QString pt_version;

public:
    QStringList qslPosName, qslPrice;

    cPriceTablePriv();
    virtual ~cPriceTablePriv();
    void setID(int i);
    int getID();
    void addItem(QString pos_name, QString pos_price);
    QString getPos(int i);
    QString getPrice(int i);
    void reset();
    void clear();
    int getType();
    void setType(int t);
    QString getVersion();
    void setVersion(QString v);
    int count();
    QString getSum(int pos, int cnt);
    QStringList getFilteredPosNames();
};


class cTariffScope : public cTableItem
{
private:
    QString name;
    int scope_id;
public:
    enum scopes { TARIFF_SCOPE_LOCAL = 0, TARIFF_SCOPE_EAST = 1, TARIFF_SCOPE_WEST = 2,
                  TARIFF_SCOPE_ID_GERMANY = 900, TARIFF_SCOPE_ID_EAST = 901, TARIFF_SCOPE_ID_WEST = 902 };
    cTariffScope();
    void setName(QString s);
    QString getName();
    void setScopeID(int i);
    int getScopeID();
    virtual void retrieve(QSqlQuery& query);
    virtual void bind4store(QSqlQuery& query);
};


class cTariffArea : public cTableItem
{
private:
    QString name;
    int scope;
public:
    cTariffArea();
    void setName(QString s);
    QString getName();
    void setScope(int i);
    int getScope();
    virtual void retrieve(QSqlQuery& query);
    virtual void bind4store(QSqlQuery& query);
};


class cUnion : public cTableItem
{
private:
    int prof;
    QString name_short, name_long;
public:
    enum unions { NONE = 0, IFK = 1, VDB = 2, VPT = 3, ZVK = 4, DVE = 11, BED = 12 };
    cUnion();
    void setProfession(int i);
    int getProfession();
    void setNameShort(QString s);
    QString getNameShort();
    void setNameLong(QString s);
    QString getNameLong();
    virtual void retrieve(QSqlQuery& query);
    virtual void bind4store(QSqlQuery& query);
};


class cProfession : public cTableItem
{
private:
    QString name;
public:
    // PROF_MASSEUR
    enum professions { PROF_PHYSIOTHERAPY = 0, PROF_ERGOTHERAPY, PROF_LOGOPEDICS, PROF_COUNT };
    cProfession();
    void setName(QString s);
    QString getName();
    virtual void retrieve(QSqlQuery& query);
    virtual void bind4store(QSqlQuery& query);
};


class cQualification : public cTableItem
{
private:
    int profession;
    QString name;
public:
    cQualification();
    QString getName();
    int getProfession();
    void setName(QString s);
    void setProfession(int i);
    virtual void retrieve(QSqlQuery& query);
    virtual void bind4store(QSqlQuery& query);
};


class cRoom : public cTableItem
{
private:
    QString name, building, notice;
public:
    cRoom();
    void setName(QString s);
    void setBuilding(QString s);
    void setNotice(QString s);
    QString getName();
    QString getBuilding();
    QString getNotice();
    virtual void retrieve(QSqlQuery& query);
    virtual void bind4store(QSqlQuery& query);
};


class cCar : public cTableItem
{
private:
    QString name, notice;
public:
    cCar();
    void setName(QString s);
    void setNotice(QString s);
    QString getName();
    QString getNotice();
    virtual void retrieve(QSqlQuery& query);
    virtual void bind4store(QSqlQuery& query);
};

#ifndef OPA
class cInvoiceStat : public cTableItem
{
private:
    QString name;
public:
    cInvoiceStat();
    void setName(QString s);
    QString getName();
    virtual void retrieve(QSqlQuery& query);
    virtual void bind4store(QSqlQuery& query);
};
#endif // OPA

#ifndef OPA
class cPraxisInvoiceStat : public cTableItem
{
private:
    QString name;
public:
    cPraxisInvoiceStat();
    void setName(QString s);
    QString getName();
    virtual void retrieve(QSqlQuery& query);
    virtual void bind4store(QSqlQuery& query);
};
#endif // OPA

#ifndef OPA
class cZipCode : public cTableItem
{
private:
    QString zip, city;
public:
    cZipCode();
    void setZip(QString s);
    QString getZip();
    void setCity(QString s);
    QString getCity();
    virtual void retrieve(QSqlQuery& query);
    virtual void bind4store(QSqlQuery& query);
};
#endif


class cTextModule : public cTableItem
{
private:
    int tm_domain;
    QString tm_text;
public:
    enum domains { TM_DATES = 0, TM_PATIENTS };
    cTextModule();
    void setDomain(int i);
    int getDomain();
    void setText(QString s);
    QString getText();
    virtual void retrieve(QSqlQuery& query);
    virtual void bind4store(QSqlQuery& query);
};


class cProtectedFunc : public cTableItem
{
private:
    QString name, ridlist;
public:
    cProtectedFunc();
    void setName(QString s);
    QString getName();
    void setRidList(QString s);
    QString getRidList();
    virtual void retrieve(QSqlQuery& query);
    virtual void bind4store(QSqlQuery& query);
};


class cDatabase
{
private:
    QString driver, name, file, host, user, pw;

public:
    enum db_type { DB_SQLITE = 0, DB_MYSQL = 1 };
    cDatabase();
    ~cDatabase();
    void reset();
    QString getDriver();
    void setDriver(QString s);
    QString getName();
    void setName(QString s);
    QString getFile();
    void setFile(QString s);
    QString getHost();
    void setHost(QString s);
    QString getUser();
    void setUser(QString s);
    QString getPW();
    void setPW(QString s);

    cDatabase& operator=(cDatabase& other);
    int getType();
};


class cReaderDevice : public cTableItem
{
private:
    QString name, lib;
    int api;
    unsigned short term, port, readofs;
    bool sel;

public:
    enum RD_API { CTAPI = 0, MCARD };
    cReaderDevice();
    ~cReaderDevice();
    void reset();
    cReaderDevice& operator=(cReaderDevice& other);
    void setName(QString s);
    void setLib(QString s);
    void setAPI(int i);
    void setTerminal(unsigned short i);
    void setPort(unsigned short i);
    void setReadOffset(unsigned i);
    void setSelect(bool b);
    void setPropCTAPI(int t, int p, int ofs, bool s);
    QString getName();
    QString getLib();
    int getAPI();
    unsigned short getTerminal();
    unsigned short getPort();
    unsigned short getReadOffset();
    bool getSelect();
    void getPropCTAPI(int *t, int *p, int *ofs, bool *sel);

    virtual void retrieve(QSqlQuery& query);
    virtual void bind4store(QSqlQuery& query);
};


#ifndef OPA
class cCashPosition : public cTableItem
{
private:
    QString cp_desc;

public:
    enum pos { CP_OVERVIEW = 0, CP_ADDPAY, CP_COURSE, CP_LOSS, CP_SELFPAY, CP_PRIVRECEIPE, CP_VOUCHER, CP_ARTICLE };
    cCashPosition();
    void setDescription(QString s);
    QString getDescription();
    virtual void retrieve(QSqlQuery& query);
    virtual void bind4store(QSqlQuery& query);
};


class cCash : public cTableItem
{
private:
    int cpid, pid, rid, cid, xid;
    QString cdesc, amount;
    QDate paydate;

public:
    cCash();
    void reset();
    void setPositionID(int i);
    void setDescription(QString s);
    void setPayDate(QDate d);
    void setAmount(QString s);
    void setPID(int i);
    void setRID(int i);
    void setCID(int i);
    void setXID(int i);
    int getPositionID();
    QString getDescription();
    QDate getPayDate();
    QString getAmount();
    int getPID();
    int getRID();
    int getCID();
    int getXID();
    virtual void retrieve(QSqlQuery& query);
    virtual void bind4store(QSqlQuery& query);
};


class cHmCatDomain : public cTableItem
{
private:
    QString name, positions;

public:
    cHmCatDomain();
    void setName(QString s);
    void setPositions(QString s);
    QString getName();
    QString getPositions();
    virtual void retrieve(QSqlQuery& query);
};


class cHmCat : public cTableItem
{
private:
    int maxcount, erstvo, folgevo, child_only;
    QString ind, hm_primary, hm_optional, hm_additional;

public:
    cHmCat();
    void reset();
    void setMaxCount(int i);
    void setErstVO(int i);
    void setFolgeVO(int i);
    void setIndicator(QString s);
    void setHmPrimary(QString s);
    void setHmOptional(QString s);
    void setHmAdditional(QString s);
    void setChildOnly(int i);
    int getMaxCount();
    int getErstVO();
    int getFolgeVO();
    QString getIndicator();
    QString getHmPrimary();
    QString getHmOptional();
    QString getHmAdditional();
    int getChildOnly();
    virtual void retrieve(QSqlQuery& query);
};

#endif // OPA


class cTemplateType : public cTableItem
{
private:
    QString t_name;

public:
    enum template_types { TT_REPORT = 0, TT_PARTICIPANCE, TT_PARTLIST, TT_REMINDER,
                          TT_PATIENTDATA, TT_QUITTANCE, TT_INVOICE, TT_CORRECTION,
                          TT_RECEIPETMPL, TT_SCHEDULE, TT_DATES, TT_VARIOUS, TT_COUNT};
    cTemplateType();
    cTemplateType& operator=(cTemplateType& other);
    void setName(QString s);
    QString getName();
    virtual void retrieve(QSqlQuery& query);
};


class cTemplateDataCategory : public cTableItem
{
private:
    QString c_name, c_shortname, c_fields;

public:
    enum categories { CAT_GENERAL = 0, CAT_SURGERY, CAT_REMINDER, CAT_PATIENT, CAT_INVOICE, CAT_QUITTANCE,
                      CAT_RECEIPE, CAT_COURSE, CAT_REPORT, CAT_PHYSICIAN, CAT_EMPLOYEE, CAT_SCHEDULE, CAT_DATES, CAT_COUNT };
    cTemplateDataCategory();
    cTemplateDataCategory& operator=(cTemplateDataCategory& other);
    void setName(QString s);
    QString getName();
    void setShortName(QString s);
    QString getShortName();
    void setFields(QString s);
    QString getFields();
    virtual void retrieve(QSqlQuery& query);
};


class cTemplateDataMapping : public cTableItem
{
private:
    QString c_name, c_shortname;

public:
    cTemplateDataMapping();
    cTemplateDataMapping& operator=(cTemplateDataMapping& other);
    void setName(QString s);
    QString getName();
    void setShortName(QString s);
    QString getShortName();
    virtual void retrieve(QSqlQuery& query);
};


class cTemplateAvailDataCategory : public cTableItem
{
private:
    int tmpl_type;
    QString avail;

public:
    cTemplateAvailDataCategory();
    cTemplateAvailDataCategory& operator=(cTemplateAvailDataCategory& other);
    void setTemplateType(int i);
    int getTemplateType();
    void setAvail(QString s);
    QString getAvail();
    virtual void retrieve(QSqlQuery& query);
};


class cTemplate : public cTableItem
{
private:
    int tmpl_type, tmpl_subtype, tmpl_ver, tmpl_flags;
    QString tmpl_hash, tmpl_name;
    QByteArray contents;

public:
    cTemplate();
    void setTemplateType(int i);
    int getTemplateType();
    void setTemplateSubtype(int i);
    int getTemplateSubtype();
    void setTemplateVersion(int i);
    int getTemplateVersion();
    void setTemplateFlags(int i);
    int getTemplateFlags();
    void setHash(QString s);
    QString getHash();
    void setName(QString s);
    QString getName();
    void setContents(QByteArray qba);
    QByteArray getContents();
    virtual void retrieve(QSqlQuery& query);
    virtual void bind4store(QSqlQuery& query);
    cTemplate& operator=(cTemplate& other);
};

#ifndef OPA

class Browse;


#ifdef USE_ASYNC_DB_ACCESS

class CustomSqlModel : public QAbstractTableModel
{
    Q_OBJECT
private:
    QList<QSqlRecord> content;
    int date_field;
    int color_column_bg, color_column_fg;
    bool color_mode_bg, color_mode_fg;
    QStringList color_keywords_bg, color_keywords_fg;
    QList<QColor> colors_bg, colors_fg;
    QColor *color_marker;
    QStringList headers;

public:
    CustomSqlModel(QObject *parent = 0);
    ~CustomSqlModel();
    QVariant data(const QModelIndex &index, int role) const;
    int columnCount(const QModelIndex & parent) const;
    int columnCount();
    int rowCount(const QModelIndex & parent) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    void setDateField(int f);
    void enableBackgroundColorMode(bool activate, int keycolumn);
    void setBackgroundKeywords(QStringList& keywords, QList<QColor>& colors);
    void enableForegroundColorMode(bool activate, int keycolumn);
    void setForegroundKeywords(QStringList& keywords, QList<QColor>& colors);
    void setHeaders(QStringList& list);
    void removeContent();

public slots:
    void dataArrivedFromThread(const QList<QSqlRecord>& records, bool success);

signals:
    void sigDataAvailable();
};


class QueryQueueElement : public QObject
{
    Q_OBJECT

public:
    QString query;
    QPointer<CustomSqlModel> model;

    QueryQueueElement();
    QueryQueueElement(const QueryQueueElement &other);
    ~QueryQueueElement();
    QueryQueueElement& operator=(const QueryQueueElement& other);
};


class QueryThread: public QThread
{
    Q_OBJECT

private:
    QSqlDatabase db_handle;
    bool running;
    QQueue<QueryQueueElement> queue;

public:
    QueryThread(QObject* parent = 0);
    bool init(cDatabase& d);
    void run();
    void stop();
    void getError(QString qs, QSqlError e);

public slots:
    void slotExecQuery(CustomSqlModel* model, const QString& query);
};

#else

class CustomSqlModel : public QSqlQueryModel
{
    Q_OBJECT
private:
    int date_field;
    int color_column_bg, color_column_fg;
    bool color_mode_bg, color_mode_fg;
    QStringList color_keywords_bg, color_keywords_fg;
    QList<QColor> colors_bg, colors_fg;
    QColor *color_marker;

public:
    CustomSqlModel(QObject *parent = 0);
    ~CustomSqlModel();
    QVariant data(const QModelIndex &item, int role) const;
    void setDateField(int f);
    void enableBackgroundColorMode(bool activate, int keycolumn);
    void setBackgroundKeywords(QStringList& keywords, QList<QColor>& colors);
    void enableForegroundColorMode(bool activate, int keycolumn);
    void setForegroundKeywords(QStringList& keywords, QList<QColor>& colors);
};

#endif // USE_ASYNC_DB_ACCESS

#endif // OPA


class myDB : public QObject
{
    Q_OBJECT
	
private:
    cSurgery surg;
    QList<cTable*> listTables;
    QList<cPriceTableNames*> listPriceTablesNames;
    QSqlDatabase db_handle;
    cDatabase db_properties;

#ifndef OPA
#ifdef USE_ASYNC_DB_ACCESS
    QueryThread thread;
#endif // USE_ASYNC_DB_ACCESS
#endif // OPA
    void updateDependencies(int table, int op);

public:
    enum receipe_types { RT_GKV, RT_PRIVATE };
    enum numbering_types { NR_INVOICE, NR_QUITTANCE };
    enum tables { TABLE_PATIENTS = 0, TABLE_EMPLOYEES, TABLE_THERAPY_GROUPS, TABLE_EMPLOYEE_GROUPS,
		  TABLE_EMPLOYEE_TYPES, TABLE_PHYSICIANS, TABLE_PHYSICIAN_TYPES, TABLE_INSURANCES,
		  TABLE_INSURANCE_TYPES, TABLE_RECEIPES, TABLE_INDICATORS, TABLE_RECEIPE_STATES,
                  TABLE_ROUTES, TABLE_INVOICES, TABLE_PINVOICES, TABLE_PRAXIS, TABLE_PAPPENDIX,
                  TABLE_COURSES, TABLE_QUITTANCES, TABLE_REMINDERS,
		  TABLE_REPORTS, TABLE_FINDINGS, TABLE_ROLES, TABLE_PROTECT, TABLE_DATES, TABLE_SURGERY,
		  TABLE_PRICETABLES, TABLE_TARIFF_AREAS, TABLE_UNIONS, TABLE_PROFESSIONS, TABLE_INVOICE_STATS,
                  TABLE_PINVOICE_STATS,
                  TABLE_READER_DEVICES, TABLE_HMPOS, TABLE_RECEIPE_TEMPLATES, TABLE_ASSOC, TABLE_CASHPOS,
                  TABLE_CASH, TABLE_CASH_ADDPAY, TABLE_CASH_SELFPAY, TABLE_CASH_PRIVRECEIPE,
                  TABLE_QUALIFICATIONS, TABLE_ROOMS, TABLE_DATE_TYPES, TABLE_CARS, TABLE_META_DATES,
                  TABLE_DATE_STATES, TABLE_TARIFF_SCOPES, TABLE_RECEIPE_SHORTCUTS, TABLE_CLINICS,
                  TABLE_HM_CAT, TABLE_HM_CAT_DOMAINS, TABLE_TEMPLATE_TYPES, TABLE_TEMPLATE_SUBTYPES,
                  TABLE_TEMPLATE_DATA_CATEGORIES, TABLE_TEMPLATE_DATA_MAPPINGS, TABLE_TEMPLATES,
                  TABLE_TEMPLATE_AVAIL_DATA_CATEGORIES, TABLE_DATE_ACTIVITIES, TABLE_ZIPCODES,
                  TABLE_TEXT_MODULES, TABLE_SURGERY_BRANCHES, TABLE_DUMMY };
    enum update_operations { OP_ANY, OP_INSERT, OP_UPDATE, OP_DELETE };

#ifndef OPA
    QList<cEmployee*> listEmpl;
    QList<cRoom*> rooms;
    QList<cCar*> listCars;
    QList<cDateType*> dateTypes;
    QList<cAssoc*> listAssociations;
    QList<cIndicator*> listIndicators;
    QList<cPhysType*> listPhysTypes;
    QList<cEmployeeGroup*> listEmployeeGroups;
    QList<cTherapyGroup*> listTherapyGroups;
    QList<cReceipeState*> listReceipeStates;
    QList<cInvoiceStat*> listInvoiceStates;
    QList<cPraxisInvoiceStat*> listPraxisInvoiceStates;
    QList<cCashPosition*> listCashPositions;
    QList<cInsuranceType*> listInsuranceTypes;
    QList<cProfession*> listProfessions;
    QList<cQualification*> listQualifications;
    QList<cDateState*> listDateStates;
    QList<cTemplate*> listTemplates;
    QList<cTemplateType*> listTemplateTypes;
    QList<cTemplateDataCategory*> listCategories;
    QList<cTemplateAvailDataCategory*> listAvailCategories;
    QList<cTemplateDataMapping*> listMappings;
    QList<cDateActivity*> listDateActivities;
    QList<cZipCode*> listZipCodes;
    QList<cInsurance*> listInsurances;
#endif
    QList<cReceipeShortcut*> listReceipeShortcuts;
    QList<cHMPos*> table_hm_pos;
    QList<cTariffArea*> listTariffAreas;

    myDB();
    ~myDB();
    bool DB_connect(cDatabase& d);
    cDatabase DB_getProperties();
    void DB_disconnect();
    bool isConnected();
    void getError(QString qs, QSqlError e);
    int currentDatabaseVersion();

#ifndef OPA
    void loadTable(int table_id, CustomSqlModel *model, QString filter);
    void loadBaseTypes();
    QStringList getTableHeaders(int tab);
    QStringList getTableItems(int tab);
#endif

#ifndef OPA
    bool existPatient(cPatient& p, int *id);
#endif
    bool existDocTemplate(QString tmpl_name, int *id);
    bool existPerson(cTableItem *cti, int *id);

#ifndef OPA
    bool loadEmployees();
    bool loadReceipesForPatient(int pid, QList<cReceipe *> &receipes);
    bool setRidInDatesForPid(int pid, int rid);
#endif

    bool getInsurance(QString ik, cInsurance& i);
    bool saveInsurances(QList<cInsurance*> *l);

    bool loadPriceTableNames();
    QString lookupPriceTableNameAndID(int prof, int assoc, int *id);

    QString getPriceTableName(int area, int assoc, int prof);
    QString getPriceTableName(int id);
    QString getPriceTableDescription(int id);
    bool loadPriceTable(QString tab, cPriceTable& cpt, int prof);
    int loadPriceTablePrivate(int id, int pt_type, cPriceTablePriv& cpt);
    bool savePriceTablePrivate(cPriceTablePriv& cpt);
    bool getAvailablePriceTablesForGKV(QList<int>& listID, QStringList& listDescr);
    bool priceTableUsed(QString s);
#ifdef OPA
    bool deletePriceTable(QString pt_name);
#endif

#ifndef OPA
    int getNumOfBilledReceipes(QDate d_start, QDate d_end, float *f);
#endif

    bool getReceipeTypes(int t, QStringList& qsl);

    bool initSurgery();
    int getCurrentSurgeryID();
    cSurgery getSurgery();

    bool getInvoiceNumbers(int rid, QStringList& qsl);
    bool getPraxisInvoiceNumbers(int rid, QStringList& qsl);
    QString getCurrentNumbering(int nr_type);
    bool setCurrentNumbering(QString s, int nr_type);

    bool loadHotKeyFunctions(QStringList& listHKF);
    bool updateProtectedFuncs(QStringList& qslRID);
    int checkDocTemplate(QString name);
#ifndef OPA
    bool loadDates(QList<cDateItem *> &list, QDate from, QDate until);
    bool loadDates(QList<cDateItem *> &list, QDate from, QDate until, int eid);
    bool loadDates(QList<cDateItem *> &list, QDate from, int uid);
    bool loadDates(QList<cDateItem *> &list, QDate from, int uid, int pid);
    bool loadDates(QList<cDateItem *> &list, QDate from, int uid, int pid, int rid);
    bool getDateList(QList<cDateItem*> &list, int dtype, int id);
    bool saveDateList(QList<cDateItem*> &list);
    bool delMetaDates(int id);
    bool loadPraxisInvoiceAppendix(QList<cAppendixItem *> &list, QString slist, QDate from, QDate until);
#endif

    bool retrieveItem(cTableItem& cti, int id);
    bool saveItem(cTableItem& cti, bool emit_signal = true);
    bool storeItem(cTableItem& cti, int *id, bool emit_signal);
    bool storeItem(cTableItem& cti, int *id);
    bool updateItem(cTableItem& cti, bool emit_signal);
    bool updateItem(cTableItem& cti);
    bool deleteItem(int table, int id, bool emit_signal);
    bool deleteItem(int table, int id);
    bool deleteItemPid(int id);
    bool deleteItemRid(int id);
    bool retrieveItemList(int table, QList<cTableItem *> &items);
    bool saveItemList(int table, QList<cTableItem *> &items, bool use_transaction = true);
    bool executeBulk(QStringList& statements, bool use_transaction = true, bool stopAtFirstError = false);
    bool saveBackupDB(QString filename);

signals:
    void sigDBChange(int table, int op, int item_id);
    void sigDBChangeExternal(int table, int op, int item_id);

#ifndef OPA
private slots:
    void slotUpdateDB(int table, int op, int item_id);
#endif
};

extern myDB db;



#endif /*MYDB_H_*/
