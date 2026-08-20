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

#include "config.h"
#include <QtXml>
#include "msgbox.h"

ConfXML myConf;


ConfXmlItem::ConfXmlItem()
{
    reset();
}

ConfXmlItem::ConfXmlItem(int a, QString t, QString v)
{
    area = a;
    tag = t;
    hash["value"] = v;
    count = 1;
}

ConfXmlItem::~ConfXmlItem() {}

void ConfXmlItem::reset()
{
    area = -1;
    count = 0;
    tag = "";
    hash.clear();
}

void ConfXmlItem::clear() { hash.clear(); count = 0; }

void ConfXmlItem::setArea(int a) { area = a; }

int ConfXmlItem::getArea() { return area; }

void ConfXmlItem::setTag(QString t) { tag = t; }

QString ConfXmlItem::getTag() { return tag; }

void ConfXmlItem::setValue(QString v) { hash["value"] = v; count = 1; }

QString ConfXmlItem::getValue() { return hash["value"]; }

QString ConfXmlItem::getValue(QString key) { return hash[key]; }

void ConfXmlItem::setCount(int i) { count = i; }

int ConfXmlItem::getCount() { return count; }

void ConfXmlItem::append(QString key, QString val) { hash[key] = val; }


ConfXML::ConfXML()
{
}


ConfXML::~ConfXML()
{
    while (!xmlItems.isEmpty()) delete xmlItems.takeFirst();
}


void ConfXML::init()
{
    for (int i = 0; i < AREA_NUMBER; i++) {
        nameAreas.append("");
    }
    nameAreas.replace(AREA_ADMIN, "admin");
    nameAreas.replace(AREA_DATABASE, "database");
    nameAreas.replace(AREA_SURGERY, "surgery");
    nameAreas.replace(AREA_DISPLAY, "display");
    nameAreas.replace(AREA_PRINTER, "printer");
    nameAreas.replace(AREA_READER, "reader");
    nameAreas.replace(AREA_ROLES, "roles");
    nameAreas.replace(AREA_BACKUP, "backup");
    nameAreas.replace(AREA_UPDATE, "update");
    nameAreas.replace(AREA_DOCS, "docs");
    nameAreas.replace(AREA_APP, "app");
    nameAreas.replace(AREA_PLANNER, "planner");
    nameAreas.replace(AREA_COURSES, "courses");
    nameAreas.replace(AREA_PERMISSIONS, "perm");
    nameAreas.replace(AREA_PRICES_PRIV, "prices");
    nameAreas.replace(AREA_HOTKEYS, "hotkeys");
    nameAreas.replace(AREA_TAKEOVER, "takeover");
    nameAreas.replace(AREA_OTHER, "other");
    nameAreas.replace(AREA_SHORTCUTS, "shortcuts");
    nameAreas.replace(AREA_TEXT_MODULES, "textmodules");

    // set default configuration values
    xmlItems.append(new ConfXmlItem(AREA_ADMIN, "access", ""));

    xmlItems.append(new ConfXmlItem(AREA_DATABASE, "driver", "QSQLITE"));
    xmlItems.append(new ConfXmlItem(AREA_DATABASE, "name", "openphysio"));
    xmlItems.append(new ConfXmlItem(AREA_DATABASE, "file", "database/openphysio.db"));
    xmlItems.append(new ConfXmlItem(AREA_DATABASE, "host", "localhost"));
    xmlItems.append(new ConfXmlItem(AREA_DATABASE, "user", "root"));
    xmlItems.append(new ConfXmlItem(AREA_DATABASE, "pw", "abc1234"));

    xmlItems.append(new ConfXmlItem(AREA_SURGERY, "surgid", "1"));

    xmlItems.append(new ConfXmlItem(AREA_APP, "debug", "true"));
    xmlItems.append(new ConfXmlItem(AREA_APP, "dir_doc", "doc/"));

    xmlItems.append(new ConfXmlItem(AREA_READER, "rdevice", "-1"));

#ifdef Q_WS_WIN
    // use CTAPI as default
    xmlItems.append(new ConfXmlItem(AREA_READER, "api", "0"));
#elif defined Q_OS_LINUX
    // use MCARD as default
    xmlItems.append(new ConfXmlItem(AREA_READER, "api", "1"));
#else
    // use MCARD as default
    xmlItems.append(new ConfXmlItem(AREA_READER, "api", "1"));
#endif

    xmlItems.append(new ConfXmlItem(AREA_READER, "lib", ""));
    xmlItems.append(new ConfXmlItem(AREA_READER, "readofs", "0"));
    xmlItems.append(new ConfXmlItem(AREA_READER, "port", "1"));
    xmlItems.append(new ConfXmlItem(AREA_READER, "select", "false"));
    xmlItems.append(new ConfXmlItem(AREA_READER, "term", "1"));

    xmlItems.append(new ConfXmlItem(AREA_PRINTER, "prndev_receipe", "0"));
    xmlItems.append(new ConfXmlItem(AREA_PRINTER, "prndev_doc", "0"));
    xmlItems.append(new ConfXmlItem(AREA_PRINTER, "prndev_dates", "0"));
    xmlItems.append(new ConfXmlItem(AREA_PRINTER, "prndlg_receipe", "true"));
    xmlItems.append(new ConfXmlItem(AREA_PRINTER, "prndlg_doc", "true"));
    xmlItems.append(new ConfXmlItem(AREA_PRINTER, "prndlg_dates", "true"));
    xmlItems.append(new ConfXmlItem(AREA_PRINTER, "ofx", "0"));
    xmlItems.append(new ConfXmlItem(AREA_PRINTER, "ofy", "0"));
    xmlItems.append(new ConfXmlItem(AREA_PRINTER, "doc_ofx", "0"));
    xmlItems.append(new ConfXmlItem(AREA_PRINTER, "doc_ofy", "0"));

    xmlItems.append(new ConfXmlItem(AREA_DISPLAY, "table_colormode", "true"));
    xmlItems.append(new ConfXmlItem(AREA_DISPLAY, "colRunning", "255,255,255,255"));
    xmlItems.append(new ConfXmlItem(AREA_DISPLAY, "colDone", "189,194,231,255"));
    xmlItems.append(new ConfXmlItem(AREA_DISPLAY, "colReminder", "231,98,45,255"));
    xmlItems.append(new ConfXmlItem(AREA_DISPLAY, "colPayed", "255,255,255,255"));
    xmlItems.append(new ConfXmlItem(AREA_DISPLAY, "colCleared", "129,220,157,255"));
    xmlItems.append(new ConfXmlItem(AREA_DISPLAY, "colOpen", "231,189,45,255"));
    xmlItems.append(new ConfXmlItem(AREA_DISPLAY, "colCanceled", "189,194,231,255"));
    xmlItems.append(new ConfXmlItem(AREA_DISPLAY, "colInactive", "180,180,180,255"));
    xmlItems.append(new ConfXmlItem(AREA_DISPLAY, "fontsize_findings", "10"));

    xmlItems.append(new ConfXmlItem(AREA_ROLES, "default_role", "0"));

    xmlItems.append(new ConfXmlItem(AREA_COURSES, "default_duration", "60"));
    xmlItems.append(new ConfXmlItem(AREA_COURSES, "default_units", "10"));

    xmlItems.append(new ConfXmlItem(AREA_PLANNER, "work_start", "7"));
    xmlItems.append(new ConfXmlItem(AREA_PLANNER, "work_end", "20"));
    xmlItems.append(new ConfXmlItem(AREA_PLANNER, "work_days", "5"));
    xmlItems.append(new ConfXmlItem(AREA_PLANNER, "work_interval", "25"));
    xmlItems.append(new ConfXmlItem(AREA_PLANNER, "planview", "0"));
    xmlItems.append(new ConfXmlItem(AREA_PLANNER, "planstart", "0"));
    xmlItems.append(new ConfXmlItem(AREA_PLANNER, "maxresdisplay", "6"));
    xmlItems.append(new ConfXmlItem(AREA_PLANNER, "render_start_end", "1"));
    xmlItems.append(new ConfXmlItem(AREA_PLANNER, "render_activities", "1"));
    xmlItems.append(new ConfXmlItem(AREA_PLANNER, "render_fullname", "0"));
    xmlItems.append(new ConfXmlItem(AREA_PLANNER, "colTherapy", "207,216,230,255"));
    xmlItems.append(new ConfXmlItem(AREA_PLANNER, "colCourse", "227,203,214,255"));
    xmlItems.append(new ConfXmlItem(AREA_PLANNER, "colMeeting", "213,236,188,255"));
    xmlItems.append(new ConfXmlItem(AREA_PLANNER, "colTraining", "255,255,255,255"));
    xmlItems.append(new ConfXmlItem(AREA_PLANNER, "colHoliday", "255,255,255,255"));
    xmlItems.append(new ConfXmlItem(AREA_PLANNER, "colCorpHoliday", "255,255,255,255"));
    xmlItems.append(new ConfXmlItem(AREA_PLANNER, "colVisit", "231,189,45,255"));
    xmlItems.append(new ConfXmlItem(AREA_PLANNER, "colBreak", "255,255,255,255"));
    xmlItems.append(new ConfXmlItem(AREA_PLANNER, "colOther", "255,255,255,255"));
    xmlItems.append(new ConfXmlItem(AREA_PLANNER, "colDrive", "255,255,255,255"));

    xmlItems.append(new ConfXmlItem(AREA_HOTKEYS, "F01", QString::number(HKF_HELP)));
    xmlItems.append(new ConfXmlItem(AREA_HOTKEYS, "F02", QString::number(HKF_PLAN)));
    xmlItems.append(new ConfXmlItem(AREA_HOTKEYS, "F03", QString::number(HKF_RECEIPE)));
    xmlItems.append(new ConfXmlItem(AREA_HOTKEYS, "F04", QString::number(HKF_RECEIPE_PRIV)));
    xmlItems.append(new ConfXmlItem(AREA_HOTKEYS, "F05", QString::number(HKF_DATE)));
    xmlItems.append(new ConfXmlItem(AREA_HOTKEYS, "F06", QString::number(HKF_PATIENT)));
    xmlItems.append(new ConfXmlItem(AREA_HOTKEYS, "F07", QString::number(HKF_COURSE)));
    xmlItems.append(new ConfXmlItem(AREA_HOTKEYS, "F08", QString::number(HKF_NONE)));
    xmlItems.append(new ConfXmlItem(AREA_HOTKEYS, "F09", QString::number(HKF_NONE)));
    xmlItems.append(new ConfXmlItem(AREA_HOTKEYS, "F10", QString::number(HKF_BACKUP)));
    xmlItems.append(new ConfXmlItem(AREA_HOTKEYS, "F11", QString::number(HKF_NONE)));
    xmlItems.append(new ConfXmlItem(AREA_HOTKEYS, "F12", QString::number(HKF_CHIP)));

    xmlItems.append(new ConfXmlItem(AREA_BACKUP, "dir_backup", ""));

    xmlItems.append(new ConfXmlItem(AREA_OTHER, "preload", "1"));
    xmlItems.append(new ConfXmlItem(AREA_OTHER, "reminder_period", "14"));

    xmlItems.append(new ConfXmlItem(AREA_TEXT_MODULES, "autodate", "1"));

    preload = false;
    conf_file = CONFIG_XML_FILE;
    conf_path = "";
}


int ConfXML::getCount(int area, QString tag)
{
    int count = 0;

    for (int i = 0; i < xmlItems.count(); i++) {
        if ( (xmlItems.at(i)->getArea() == area) && (xmlItems.at(i)->getTag() == tag) ) {
            count = xmlItems.at(i)->getCount();
            break;
        }
    }
    return count;
}


QString ConfXML::get(int area, QString tag, QString key)
{
    QString s = "";
    for (int i = 0; i < xmlItems.count(); i++) {
        if ( (xmlItems.at(i)->getArea() == area) && (xmlItems.at(i)->getTag() == tag) ) {
            if (key.isEmpty()) s = xmlItems.at(i)->getValue();
            else s = xmlItems.at(i)->getValue(key);
	    break;
	}
    }

    return s;
}


bool ConfXML::set(int area, QString tag, QString val)
{
    bool bRet = false;
    for (int i = 0; i < xmlItems.count(); i++) {
        if ( (xmlItems.at(i)->getArea() == area) && (xmlItems.at(i)->getTag() == tag) ) {
            xmlItems.at(i)->setValue(val);
	    bRet = true;
	    break;
	}
    }
    return bRet;
}


void ConfXML::searchAndSetConfigFilePath()
{
    QString path = "";
    QFile f;

    // check whether config file exists in current directory
    if (f.exists(CONFIG_XML_FILE)) {
        conf_path = path;
        return;
    }

    path = QDir::currentPath();
    if (f.exists(path + "/" + CONFIG_XML_FILE)) {
        conf_path = path + "/";
        return;
    }

#ifdef Q_OS_WIN
    // check whether config file exists in user directory
    QSettings ini2(QSettings::IniFormat, QSettings::UserScope, "OpenPhysio", "OpenPhysio");
    path = QFileInfo(ini2.fileName()).absolutePath();
    if (f.exists(path + "/" + CONFIG_XML_FILE)) {
        conf_path = path + "/";
        return;
    }

    // check whether config file exists in all users directory
    QSettings ini(QSettings::IniFormat, QSettings::SystemScope, "OpenPhysio", "OpenPhysio");
    path = QFileInfo(ini.fileName()).absolutePath();
    if (f.exists(path + "/" + CONFIG_XML_FILE)) {
        conf_path = path + "/";
        return;
    }
#endif

#ifdef Q_OS_LINUX
    path = "/etc/openphysio/";
    if (f.exists(path + CONFIG_XML_FILE)) {
        conf_path = path;
        return;
    }
#endif

#ifdef Q_WS_MAC
    path = QApplication::applicationDirPath() + "/";
    if (f.exists(path + CONFIG_XML_FILE)) {
        conf_path = path;
        return;
    }
    conf_path = path + "../../../openphysio.app/Contents/MacOS/";
#endif
}


QString ConfXML::getConfigFilePath()
{
    return conf_path;
}


QString ConfXML::getConfigFile()
{
    return conf_path + conf_file;
}

bool ConfXML::readXML()
{
    QFile file(getConfigFile());
    if (!file.open(QFile::ReadOnly | QFile::Text)) return false;

    QXmlStreamReader xml_reader(&file);
    while (!xml_reader.atEnd()) {
        if (xml_reader.readNext() == QXmlStreamReader::Invalid) {
            MsgBox msgBox;
            msgBox.error("Beim Lesen der Konfigurationsdatei trat folgender Fehler auf:\n\n" + xml_reader.errorString());
            break;
        }
	if (xml_reader.isStartElement()) {
            for (int i = 0; i < xmlItems.count(); i++) {
                if ( xml_reader.name() == xmlItems.at(i)->getTag() ) {
                    QXmlStreamAttributes attribs = xml_reader.attributes();
                    if (attribs.hasAttribute("count")) {
                        int co = ((QString)attribs.value("count").toString()).toInt();
                        if (co < 1) continue;
                        xmlItems.at(i)->clear();
                        xmlItems.at(i)->setCount(co);
                        for (int j = 0; j < co; j++) {
                            QString nr = QString::number(j+1);
                            xmlItems.at(i)->append("value" + nr, attribs.value("value" + nr).toString());
                            xmlItems.at(i)->append("descr" + nr, attribs.value("descr" + nr).toString());
                        }
                    } else xmlItems.at(i)->setValue(attribs.value("value").toString());
		}
	    }
	}
    }

    file.close();

    if (get(AREA_OTHER, "preload").toInt() > 0) preload = true;
    else preload = false;

    for (int i = 1; i <= KEY_NUMBER; i++) {
        QString s;
        if (i < 10) s = "F0" + QString::number(i);
        else s = "F" + QString::number(i);
        listHotkeyFunctionIDs << get(AREA_HOTKEYS, s).toInt();
    }
    return true;
}


bool ConfXML::writeXML()
{
    QFile file(getConfigFile());
    if (!file.open(QFile::WriteOnly | QFile::Text)) return false;

    QXmlStreamWriter xml_writer(&file);
    xml_writer.setAutoFormatting(true);
    xml_writer.writeStartDocument();
    xml_writer.writeStartElement("config");

    for (int i = 0; i < AREA_NUMBER; i++) {
	bool found_at_least_one = false;
	for (int j = 0; j < xmlItems.count(); j++) {
	    if (xmlItems.at(j)->getArea() == i) {
		if (!found_at_least_one) {
		    xml_writer.writeStartElement(nameAreas.at(i));
		    found_at_least_one = true;
		}
                xml_writer.writeStartElement(xmlItems.at(j)->getTag());
                int co = xmlItems.at(j)->getCount();
                if (co == 1 && xmlItems.at(j)->getArea() != AREA_DOCS)
                    xml_writer.writeAttribute("value", xmlItems.at(j)->getValue());
                else {
                    xml_writer.writeAttribute("count", QString::number(co));
                    for (int x = 0; x < co; x++) {
                        QString nr = QString::number(x + 1);
                        xml_writer.writeAttribute("value" + nr, xmlItems.at(j)->getValue("value" + nr));
                        xml_writer.writeAttribute("descr" + nr, xmlItems.at(j)->getValue("descr" + nr));
                    }
                }
                xml_writer.writeEndElement();
	    }
	}
	if (found_at_least_one) xml_writer.writeEndElement();
    }

    xml_writer.writeEndElement();
    xml_writer.writeEndDocument();

    file.close();
    return true;
}


QString ConfXML::getAreaName(int a)
{
    if ( (a >= 0) && (a < AREA_NUMBER) ) return nameAreas.at(a);
    return "";
}


cDatabase& ConfXML::getDatabase()
{
    dbx.reset();
    dbx.setDriver(get(AREA_DATABASE, "driver"));
    dbx.setFile(get(AREA_DATABASE, "file"));
    dbx.setName(get(AREA_DATABASE, "name"));
    dbx.setHost(get(AREA_DATABASE, "host"));
    dbx.setUser(get(AREA_DATABASE, "user"));
    QString pw_decoded = QByteArray::fromBase64(myConf.get(ConfXML::AREA_DATABASE, "pw").toUtf8());
    dbx.setPW(pw_decoded);
    return dbx;
}


cReaderDevice& ConfXML::getReader()
{
    reader.reset();
    //reader.setID(get(AREA_READER, "device").toInt());
    reader.setAPI(get(AREA_READER, "api").toInt());
    reader.setLib(get(AREA_READER, "lib"));
    reader.setReadOffset(get(AREA_READER, "readofs").toInt());
    reader.setPort(get(AREA_READER, "port").toInt());
    if (get(AREA_READER, "select") == "true") reader.setSelect(true);
    else reader.setSelect(false);
    reader.setTerminal(get(AREA_READER, "term").toInt());
    return reader;
}


int ConfXML::getDefaultSurgeryID()
{
    return get(AREA_SURGERY, "surgid").toInt();
}


QString ConfXML::getAppPath()
{
#ifdef Q_OS_MACX
    return QApplication::applicationDirPath() + "/";
#else
    return QDir::currentPath() + "/";
#endif
}


QString ConfXML::getDocDir()
{
     return getAppPath() + get(AREA_APP, "dir_doc");
}


bool ConfXML::isDbg()
{
    if (get(AREA_APP, "debug") == "true") return true;
    return false;
}


bool ConfXML::usePreload()
{
    return preload;
}


bool ConfXML::isTableColorMode()
{
    if (get(AREA_DISPLAY, "table_colormode") == "true") return true;
    return false;
}


QColor ConfXML::getColor(QString s)
{
    if (s == "Laufend") return getColorFromString(get(AREA_DISPLAY, "colRunning"));
    if (s == "Abgeschlossen") return getColorFromString(get(AREA_DISPLAY, "colDone"));
    if (s == "Abgerechnet") return getColorFromString(get(AREA_DISPLAY, "colCleared"));
    if (s == "Offen") return getColorFromString(get(AREA_DISPLAY, "colOpen"));
    if (s == "Angemahnt") return getColorFromString(get(AREA_DISPLAY, "colReminder"));
    if (s == "Bezahlt") return getColorFromString(get(AREA_DISPLAY, "colPayed"));
    if (s == "Abgebrochen") return getColorFromString(get(AREA_DISPLAY, "colCanceled"));
    if (s == "inaktiv") return getColorFromString(get(AREA_DISPLAY, "colInactive"));
    return getColorFromString("");
}


QColor ConfXML::getColorFromString(QString s)
{
    QColor c(255,255,255,255);
    if (s.isEmpty()) return c;

    QStringList list = s.split(",");
    if (list.count() != 4) return c;

    c.setRed(((QString)list[0]).toInt());
    c.setGreen(((QString)list[1]).toInt());
    c.setBlue(((QString)list[2]).toInt());
    c.setAlpha(((QString)list[3]).toInt());
    return c;
}


QString ConfXML::getStringFromColor(QColor c)
{
    return QString::number(c.red()) + "," + QString::number(c.green()) + "," +
	    QString::number(c.blue()) + "," + QString::number(c.alpha());
}


double ConfXML::getPrintOffsetX()
{
    return get(AREA_PRINTER, "ofx").toInt();
}


double ConfXML::getPrintOffsetY()
{
    return get(AREA_PRINTER, "ofy").toInt();
}


double ConfXML::getPrintDocOffsetX()
{
    return get(AREA_PRINTER, "doc_ofx").toInt();
}


double ConfXML::getPrintDocOffsetY()
{
    return get(AREA_PRINTER, "doc_ofy").toInt();
}


bool ConfXML::isPrnDlg(int prn)
{
    switch (prn) {
    case PRN_RECEIPE:
        if (get(AREA_PRINTER, "prndlg_receipe") == "true") return true;
        break;
    case PRN_DOC:
        if (get(AREA_PRINTER, "prndlg_doc") == "true") return true;
        break;
    case PRN_DATES:
        if (get(AREA_PRINTER, "prndlg_dates") == "true") return true;
        break;
    default: break;
    }

    return false;
}


int ConfXML::getDefaultPrinter(int prn)
{
    switch (prn) {
    case PRN_RECEIPE:
        return get(AREA_PRINTER, "prndev_receipe").toInt();
        break;
    case PRN_DOC:
        return get(AREA_PRINTER, "prndev_doc").toInt();
        break;
    case PRN_DATES:
        return get(AREA_PRINTER, "prndev_dates").toInt();
        break;
    default: break;
    }

    return 0;
}


int ConfXML::getDefaultRole()
{
    return get(AREA_ROLES, "default_role").toInt();
}


int ConfXML::lookupHotkeyFunction(int qt_key)
{
    int iRet = 0;

    switch (qt_key) {
	case Qt::Key_F1: iRet = listHotkeyFunctionIDs.at(KEY_F1); break;
	case Qt::Key_F2: iRet = listHotkeyFunctionIDs.at(KEY_F2); break;
	case Qt::Key_F3: iRet = listHotkeyFunctionIDs.at(KEY_F3); break;
	case Qt::Key_F4: iRet = listHotkeyFunctionIDs.at(KEY_F4); break;
	case Qt::Key_F5: iRet = listHotkeyFunctionIDs.at(KEY_F5); break;
	case Qt::Key_F6: iRet = listHotkeyFunctionIDs.at(KEY_F6); break;
	case Qt::Key_F7: iRet = listHotkeyFunctionIDs.at(KEY_F7); break;
	case Qt::Key_F8: iRet = listHotkeyFunctionIDs.at(KEY_F8); break;
	case Qt::Key_F9: iRet = listHotkeyFunctionIDs.at(KEY_F9); break;
	case Qt::Key_F10: iRet = listHotkeyFunctionIDs.at(KEY_F10); break;
	case Qt::Key_F11: iRet = listHotkeyFunctionIDs.at(KEY_F11); break;
	case Qt::Key_F12: iRet = listHotkeyFunctionIDs.at(KEY_F12); break;
	default: break;
    }
    return iRet;
}


QString ConfXML::getHotkeyLabelString()
{
    QStringList qsl, tmp;
    if (!db.loadHotKeyFunctions(qsl)) return "";
    for (int i = 0; i < KEY_NUMBER; i++) {
	int hkf = listHotkeyFunctionIDs.at(i);
	if (hkf != 0) {
	    if ( (hkf > 0) && (hkf < qsl.count()) ) {
		tmp << "[F" + QString::number(i + 1) + "] " + qsl.at(hkf);
	    }
	}
    }
    return tmp.join(" | ");
}


QString ConfXML::getBackupDir()
{
    QString dir = get(AREA_BACKUP, "dir_backup");
    if (!dir.isEmpty() && dir.right(1) != "/") dir += "/";
    return dir;
}
