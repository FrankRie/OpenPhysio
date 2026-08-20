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

#ifndef CONFXML_H
#define CONFXML_H

#include <QObject>
#include <QtCore>

#include "mydb.h"


#define CONFIG_XML_FILE "config.xml"

#define DOC_HELP "helpcollection.qhc"

#define FORM_NONE    0
#define FORM_REZEPT  1
#define FORM_KARTEI  2
#define FORM_PATIENT 3

#define ROLE_ADMIN  0


class ConfXmlItem
{
private:
    int area, count;
    QString tag;
    QHash<QString, QString> hash;

public:
    ConfXmlItem();
    ConfXmlItem(int a, QString t, QString v);
    ~ConfXmlItem();
    void reset();
    void clear();
    void setArea(int a);
    int getArea();
    void setTag(QString t);
    QString getTag();
    void setValue(QString v);
    QString getValue();
    QString getValue(QString key);
    void setCount(int i);
    int getCount();
    void append(QString key, QString val);
};


class ConfXML : public QObject
{
private:
    QList<ConfXmlItem*> xmlItems;
    QStringList nameAreas;
    cDatabase dbx;
    cReaderDevice reader;
    QList<int> listHotkeyFunctionIDs;
    bool preload;
    QString conf_file, conf_path;

    QString getTemplateTag(int template_id);

public:
    enum areas { AREA_DATABASE = 0, AREA_SURGERY, AREA_DISPLAY, AREA_PRINTER, AREA_READER,
		 AREA_ROLES, AREA_BACKUP, AREA_UPDATE, AREA_DOCS, AREA_APP,
                 AREA_COURSES, AREA_PLANNER, AREA_PERMISSIONS, AREA_PRICES, AREA_PRICES_PRIV,
                 AREA_HOTKEYS, AREA_TAKEOVER, AREA_OTHER, AREA_ADMIN, AREA_SHORTCUTS,
                 AREA_TEXT_MODULES, AREA_DATE_ACTIVITIES, AREA_NUMBER };
    enum hotkeys { KEY_F1 = 0, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9,
		   KEY_F10, KEY_F11, KEY_F12, KEY_NUMBER };
    enum hotkey_funcs { HKF_NONE = 0, HKF_CHIP, HKF_PATIENT, HKF_RECEIPE, HKF_RECEIPE_PRIV,
                        HKF_DATE, HKF_COURSE, HKF_PHYS, HKF_PLAN, HKF_BACKUP, HKF_HELP };
    enum printers { PRN_RECEIPE = 0, PRN_DOC, PRN_DATES };

    ConfXML();
    ~ConfXML();
    void init();
    int getCount(int area, QString tag);
    QString get(int area, QString tag, QString key = "");
    bool set(int area, QString tag, QString val);
    void searchAndSetConfigFilePath();
    QString getConfigFilePath();
    QString getConfigFile();
    bool readXML();
    bool writeXML();
    QString getAreaName(int a);
    cDatabase& getDatabase();
    cReaderDevice& getReader();
    int getDefaultSurgeryID();

    bool usePreload();

    // app stuff
    QString getAppPath();
    QString getDocDir();
    bool isDbg();

    // display stuff
    bool isTableColorMode();
    QColor getColor(QString s);
    QColor getColorFromString(QString s);
    QString getStringFromColor(QColor c);

    // printer stuff
    double getPrintOffsetX();
    double getPrintOffsetY();
    double getPrintDocOffsetX();
    double getPrintDocOffsetY();
    bool isPrnDlg(int prn);
    int getDefaultPrinter(int prn);

    // role stuff
    int getDefaultRole();

    // hotkey stuff
    int lookupHotkeyFunction(int qt_key);
    QString getHotkeyLabelString();

    // backup stuff
    QString getBackupDir();
};


extern ConfXML myConf;

#endif // CONFXML_H
