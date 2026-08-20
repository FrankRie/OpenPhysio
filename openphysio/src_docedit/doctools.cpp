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

#include "doctools.h"
#include "dlgdoceditor.h"


// cDocTemplate
//
cDocTemplate::cDocTemplate()
{
    doc = NULL;
    editor = NULL;

    // set default page size
    // TODO: make this configurable
    default_width = width_array[0];
    default_height = height_array[0];
    default_datafield_width = 100;

    // default font
    // TODO: make this configurable
    curFont.setFamily("Arial");
    curFont.setPointSize(10);
    curFont.setBold(false);
    curFont.setItalic(false);
    curColor.setRgb(0, 0, 0, 255);

    curMode = MODE_VIEW;
#ifdef OPA
    db.retrieveItemList(myDB::TABLE_TEMPLATE_TYPES, reinterpret_cast<QList<cTableItem*>&>(listTemplateTypes));
    db.retrieveItemList(myDB::TABLE_TEMPLATE_DATA_CATEGORIES, reinterpret_cast<QList<cTableItem*>&>(listCategories));
    db.retrieveItemList(myDB::TABLE_TEMPLATE_AVAIL_DATA_CATEGORIES, reinterpret_cast<QList<cTableItem*>&>(listAvailCategories));
    db.retrieveItemList(myDB::TABLE_TEMPLATE_DATA_MAPPINGS, reinterpret_cast<QList<cTableItem*>&>(listMappings));
#else
    // the db layer already loads the required tables during startup
    // => we don't need to query the db again, instead we just copy the objects provided by the db layer
    for (int i = 0; i < db.listTemplateTypes.count(); i++) {
        listTemplateTypes.append(new cTemplateType());
        *listTemplateTypes.last() = *db.listTemplateTypes.at(i);
    }
    for (int i = 0; i < db.listCategories.count(); i++) {
        listCategories.append(new cTemplateDataCategory());
        *listCategories.last() = *db.listCategories.at(i);
    }
    for (int i = 0; i < db.listAvailCategories.count(); i++) {
        listAvailCategories.append(new cTemplateAvailDataCategory());
        *listAvailCategories.last() = *db.listAvailCategories.at(i);
    }
    for (int i = 0; i < db.listMappings.count(); i++) {
        listMappings.append(new cTemplateDataMapping());
        *listMappings.last() = *db.listMappings.at(i);
    }
#endif

    for (int i = 0; i < listCategories.count(); i++) {
        mapCategories[i] = listCategories.at(i)->getName();
    }
    for (int i = 0; i < listMappings.count(); i++) {
        mapNames[listMappings.at(i)->getShortName()] = listMappings.at(i)->getName();
    }
}


cDocTemplate::~cDocTemplate()
{
    while (listMappings.count() > 0) delete listMappings.takeLast();
    while (listAvailCategories.count() > 0) delete listAvailCategories.takeLast();
    while (listCategories.count() > 0) delete listCategories.takeLast();
    while (listTemplateTypes.count() > 0) delete listTemplateTypes.takeLast();
}


void cDocTemplate::setMode(int mode)
{
    curMode = mode;
}


int cDocTemplate::getTemplateID(QString s)
{
    int iRet = -1;
    for (int i = 0; i < listTemplateTypes.count(); i++) {
        if (s == listTemplateTypes.at(i)->getName()) {
            iRet = listTemplateTypes.at(i)->getID();
        }
    }
    return iRet;
}


int cDocTemplate::getCategoryID(QString s)
{
    return mapCategories.key(s, -1);
}


QString cDocTemplate::getShortName(QString s)
{
    return mapNames.key(s, "");
}


QString cDocTemplate::getLongName(QString s)
{
    return mapNames.value(s, "");
}


QList<int> cDocTemplate::getAllowedCategoryIDs(int tmpl_type)
{
    QList<int> list;
    for (int i = 0; i < listAvailCategories.count(); i++) {
        cTemplateAvailDataCategory *c = listAvailCategories.at(i);
        if (c->getTemplateType() == tmpl_type) {
            QString s = c->getAvail();
            if (s.isEmpty()) break;
            QStringList l = s.split(":");
            for (int j = 0; j < l.count(); j++) {
                list.append(((QString)l.at(j)).toInt());
            }
            break;
        }
    }
    return list;
}


QStringList cDocTemplate::getCategoryItems(int cat_id)
{
    QStringList l;
    if (cat_id < 0 || cat_id >= cTemplateDataCategory::CAT_COUNT) return l;
    QString s = listCategories[cat_id]->getFields();
    return s.split(":");
}


QStringList cDocTemplate::getTemplateNames()
{
    QStringList l;
    for (int i = 0; i < listTemplateTypes.count(); i++) {
        l << listTemplateTypes.at(i)->getName();
    }
    if (l.count()) l.sort();
    return l;
}


QString cDocTemplate::getTemplateName(int tmpl_type)
{
    if (tmpl_type < 0 || tmpl_type >= cTemplateType::TT_COUNT) return "";
    return listTemplateTypes.at(tmpl_type)->getName();
}


void cDocTemplate::setDoc(cDoc *d)
{
    doc = d;
}


bool cDocTemplate::parseDocProperties(QXmlStreamReader& xml)
{
    if (doc == NULL) return false;

    xml.readNext();
    while(!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "doc")) {
        if(xml.tokenType() == QXmlStreamReader::StartElement) {
            if (xml.name() == "tmplver") {
                xml.readNext();
                if(xml.tokenType() == QXmlStreamReader::Characters) {
                    curTemplate.setTemplateVersion(((QString)xml.text().toString()).toInt());
                }
            } else if (xml.name() == "type") {
                xml.readNext();
                if(xml.tokenType() == QXmlStreamReader::Characters) {
                    curTemplate.setTemplateType(((QString)xml.text().toString()).toInt());
                }
            } else if (xml.name() == "subtype") {
                xml.readNext();
                if(xml.tokenType() == QXmlStreamReader::Characters) {
                    curTemplate.setTemplateSubtype(((QString)xml.text().toString()).toInt());
                }
            } else if (xml.name() == "descr") {
                xml.readNext();
                if(xml.tokenType() == QXmlStreamReader::Characters) {
                    curTemplate.setName(xml.text().toString());
                }
            }
        }
        xml.readNext();
    }
    return true;
}


bool cDocTemplate::parsePageProperties(QXmlStreamReader& xml)
{
    if (doc == NULL) return false;

    xml.readNext();
    while(!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "page")) {
        if(xml.tokenType() == QXmlStreamReader::StartElement) {
            if (xml.name() == "width") {
                xml.readNext();
                if(xml.tokenType() == QXmlStreamReader::Characters) {
                    doc->width = ((QString)xml.text().toString()).toInt();
                }
            } else if (xml.name() == "height") {
                xml.readNext();
                if(xml.tokenType() == QXmlStreamReader::Characters) {
                    doc->height = ((QString)xml.text().toString()).toInt();
                }
            }
        }
        xml.readNext();
    }
    doc->createPageRect();
    return true;
}


bool cDocTemplate::parseTextProperties(QXmlStreamReader& xml, int t)
{
    if (doc == NULL) return false;

    enum stat { STAT_TEXT_SEARCH, STAT_TEXT_FOUND };
    enum tokens { TOK_POS_X = 0, TOK_POS_Y, TOK_CONTENT, TOK_FONT, TOK_FONTSIZE, TOK_BOLD, TOK_ITALIC,
                  TOK_UNDERLINE, TOK_COLOR, TOK_WIDTH, TOK_ALIGN, TOK_SPACING };
    int x = 0;
    int y = 0;
    int w = default_datafield_width;
    int a = 0;
    QString content;
    QFont f;
    QColor c;
    c.setRgb(0, 0, 0, 255);
    int curState = STAT_TEXT_SEARCH;

    QString main_token = t == 0 ? "texts" : "fields";
    QString sub_token = t == 0 ? "text" : "field";

    xml.readNext();
    while(!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == main_token)) {
        if (xml.tokenType() == QXmlStreamReader::StartElement && xml.name() == sub_token) {
            curState = STAT_TEXT_FOUND;
            xml.readNext();
        }
        if (curState == STAT_TEXT_FOUND && xml.tokenType() == QXmlStreamReader::StartElement) {
            int tok = -1;
            if (xml.name() == "pos_x") tok = TOK_POS_X;
            else if (xml.name() == "pos_y") tok = TOK_POS_Y;
            else if (xml.name() == "content") tok = TOK_CONTENT;
            else if (xml.name() == "font") tok = TOK_FONT;
            else if (xml.name() == "fontsize") tok = TOK_FONTSIZE;
            else if (xml.name() == "bold") tok = TOK_BOLD;
            else if (xml.name() == "italic") tok = TOK_ITALIC;
            else if (xml.name() == "underline") tok = TOK_UNDERLINE;
            else if (xml.name() == "color") tok = TOK_COLOR;
            else if (xml.name() == "align") tok = TOK_ALIGN;
            else if (xml.name() == "width") tok = TOK_WIDTH;
            else if (xml.name() == "spacing") tok = TOK_SPACING;
            if (tok >= 0) {
                xml.readNext();
                if(xml.tokenType() == QXmlStreamReader::Characters) {
                    switch(tok) {
                        case TOK_POS_X: x = ((QString)xml.text().toString()).toInt(); break;
                        case TOK_POS_Y: y = ((QString)xml.text().toString()).toInt(); break;
                        case TOK_CONTENT: content = xml.text().toString(); break;
                        case TOK_FONT: f.setFamily(xml.text().toString()); break;
                        case TOK_FONTSIZE: f.setPointSize(((QString)xml.text().toString()).toInt()); break;
                        case TOK_BOLD: f.setBold((bool)((QString)xml.text().toString()).toInt()); break;
                        case TOK_ITALIC: f.setItalic((bool)((QString)xml.text().toString()).toInt()); break;
                        case TOK_UNDERLINE: f.setUnderline((bool)((QString)xml.text().toString()).toInt()); break;
                        case TOK_COLOR: c = getColorFromString(xml.text().toString()); break;
                        case TOK_ALIGN: a = ((QString)xml.text().toString()).toInt(); break;
                        case TOK_WIDTH: w = ((QString)xml.text().toString()).toInt(); break;
                        case TOK_SPACING: f.setLetterSpacing(QFont::AbsoluteSpacing, ((QString)xml.text().toString()).toInt());  break;
                        default: break;
                    }
                }
            }
        }
        if (xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == sub_token) {
            if (t == 0) {
                cEditorTextElement* te = doc->createTextItem(x, y, content, f, c);
                if (editor != NULL) {
                    auto xe = qobject_cast<cEditorTextElement*>(te);
                    connect(xe, &cEditorTextElement::sigSelectTextItem, editor, &DlgDocEditor::slotTextItemSelected);
                }
            } else {
                cEditorDataElement* de = doc->createDataItem(x, y, w, a, content, getLongName(content), f, c);
                if (editor != NULL)  {
                    auto xe = qobject_cast<cEditorDataElement*>(de);
                    connect(xe, &cEditorDataElement::sigSelectDataItem, editor, &DlgDocEditor::slotTextItemSelected);
                }
            }
            x = y = a = 0;
            w = default_datafield_width;
            content = "";
            c.setRgb(0, 0, 0, 255);
            curState = STAT_TEXT_SEARCH;
        }
        xml.readNext();
    }
    return true;
}


bool cDocTemplate::parseLineProperties(QXmlStreamReader& xml)
{
    if (doc == NULL) return false;

    enum stat { STAT_SEARCH, STAT_FOUND };
    enum tokens { TOK_X1, TOK_X2, TOK_Y1, TOK_Y2, TOK_COLOR, TOK_STYLE };
    int x1, x2, y1, y2, style;
    QColor c;

    x1 = x2 = y1 = y2 = style = 0;
    c.setRgb(0, 0, 0, 255);
    int curState = STAT_SEARCH;

    xml.readNext();
    while(!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "lines")) {
        if (xml.tokenType() == QXmlStreamReader::StartElement && xml.name() == "line") {
            curState = STAT_FOUND;
            xml.readNext();
        }
        if (curState == STAT_FOUND && xml.tokenType() == QXmlStreamReader::StartElement) {
            int tok = -1;
            if (xml.name() == "pos_x1") tok = TOK_X1;
            else if (xml.name() == "pos_x2") tok = TOK_X2;
            else if (xml.name() == "pos_y1") tok = TOK_Y1;
            else if (xml.name() == "pos_y2") tok = TOK_Y2;
            else if (xml.name() == "style") tok = TOK_STYLE;
            else if (xml.name() == "color") tok = TOK_COLOR;
            if (tok >= 0) {
                xml.readNext();
                if(xml.tokenType() == QXmlStreamReader::Characters) {
                    switch(tok) {
                        case TOK_X1: x1 = ((QString)xml.text().toString()).toInt(); break;
                        case TOK_X2: x2 = ((QString)xml.text().toString()).toInt(); break;
                        case TOK_Y1: y1 = ((QString)xml.text().toString()).toInt(); break;
                        case TOK_Y2: y2 = ((QString)xml.text().toString()).toInt(); break;
                        case TOK_STYLE: style = ((QString)xml.text().toString()).toInt(); break;
                        case TOK_COLOR: c = getColorFromString(xml.text().toString()); break;
                        default: break;
                    }
                }
            }
        }
        if (xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "line") {
            cEditorLineElement* le = doc->createLine(x1, y1, x2, y2, style, c);

            if (editor != NULL) {
                auto de = qobject_cast<cEditorDataElement*>(le);
                connect(de, &cEditorDataElement::sigSelectDataItem, editor, &DlgDocEditor::slotTextItemSelected);
            }
            x1 = x2 = y1 = y2 = 0;
            style = 0;
            c.setRgb(0, 0, 0, 255);
                        curState = STAT_SEARCH;
        }
        xml.readNext();
    }
    return true;
}


bool cDocTemplate::parseImageProperties(QXmlStreamReader& xml, int t)
{
    if (doc == NULL) return false;

    enum stat { STAT_SEARCH, STAT_FOUND };
    enum tokens { TOK_POS_X, TOK_POS_Y, TOK_FILE, TOK_DATA };
    int x, y;
    QString file;
    QByteArray qba;

    x = y = 0;
    int curState = STAT_SEARCH;

    QString main_token = t == 0 ? "images" : "vectors";
    QString sub_token = t == 0 ? "image" : "vector";

    xml.readNext();
    while(!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == main_token)) {
        if (xml.tokenType() == QXmlStreamReader::StartElement && xml.name() == sub_token) {
            curState = STAT_FOUND;
            xml.readNext();
        }
        if (curState == STAT_FOUND && xml.tokenType() == QXmlStreamReader::StartElement) {
            int tok = -1;
            if (xml.name() == "pos_x") tok = TOK_POS_X;
            else if (xml.name() == "pos_y") tok = TOK_POS_Y;
            else if (xml.name() == "data") tok = TOK_DATA;
            else if (xml.name() == "file") tok = TOK_FILE;
            if (tok >= 0) {
                xml.readNext();
                if(xml.tokenType() == QXmlStreamReader::Characters) {
                    switch(tok) {
                        case TOK_POS_X: x = ((QString)xml.text().toString()).toInt(); break;
                        case TOK_POS_Y: y = ((QString)xml.text().toString()).toInt(); break;
                        case TOK_DATA: {
                            qba = QByteArray::fromBase64(xml.text().toString().toUtf8());
                            qba = qUncompress(qba);
                            break;
                        }
                        case TOK_FILE: file = xml.text().toString(); break;
                        default: break;
                    }
                }
            }
        }
        if (xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == sub_token) {
            if (file.isEmpty()) {
                if (t == 0 ) {
                    doc->createPixmap(x, y, qba);
                } else {
                    doc->createSvg(x, y, qba);
                }
            } else {
                if (t == 0 ) {
                    doc->createPixmap(x, y, file);
                } else {
                    doc->createSvg(x, y, file);
                }
            }
            x = y = 0;
            file = "";
            qba.clear();
            curState = STAT_SEARCH;
        }
        xml.readNext();
    }
    return true;
}


bool cDocTemplate::readTemplateFromFile(QString filename)
{
    if (doc == NULL) return false;

    QFile file(filename);
    if (!file.open(QFile::ReadOnly | QFile::Text)) return false;

    QXmlStreamReader xml(&file);
    bool bRet = readTemplate(xml);
    file.close();
    return bRet;
}


bool cDocTemplate::readTemplateFromByteArray(QByteArray qba)
{
    if (doc == NULL) return false;

    QBuffer buffer(&qba);
    buffer.open(QBuffer::ReadOnly);
    QXmlStreamReader xml(&buffer);
    bool bRet = readTemplate(xml);
    buffer.close();
    return bRet;
}


bool cDocTemplate::readTemplate(QXmlStreamReader& xml)
{
    if (doc == NULL) return false;

    if (xml.readNextStartElement()) {
        if (xml.name() != "template") {
            xml.raiseError("Keine Dokumentenvorlage");
            QMessageBox::warning(NULL, "OpenPhysio", QString::fromUtf8("Beim Lesen der Dokumentenvorlage trat folgender Fehler auf:\n\n") + xml.errorString());
            return false;
        }
    }

    while (xml.readNextStartElement()) {
        if (xml.name() == "doc") {
            parseDocProperties(xml);
        } else if (xml.name() == "page") {
            parsePageProperties(xml);
        } else if (xml.name() == "texts") {
             parseTextProperties(xml, 0);
        } else if (xml.name() == "lines") {
            parseLineProperties(xml);
        } else if (xml.name() == "images") {
            parseImageProperties(xml, 0);
        } else if (xml.name() == "vectors") {
            parseImageProperties(xml, 1);
        } else if (xml.name() == "fields") {
            parseTextProperties(xml, 1);
        } else xml.skipCurrentElement();
    }

    doc->flag_change = false;
    return true;
}


bool cDocTemplate::saveTemplate(bool save_to_file)
{
    QString s = doc->filename;
    QFile file(s);
    QByteArray array;
    QBuffer buffer(&array);
    QXmlStreamWriter xml_writer;

    if (save_to_file) {
        if (doc->filename.isEmpty()) return false;
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            return false;

        xml_writer.setDevice(&file);           // nur QIODevice
    } else {
        if (!buffer.open(QIODevice::WriteOnly))
            return false;

        xml_writer.setDevice(&buffer);         // nur QIODevice
    }

    xml_writer.setAutoFormatting(true);
    xml_writer.writeStartDocument();
    xml_writer.writeStartElement("template");

    xml_writer.writeStartElement("doc");
    xml_writer.writeTextElement("tmplver", QString::number(curTemplate.getTemplateVersion()));
    xml_writer.writeTextElement("type", QString::number(curTemplate.getTemplateType()));
    xml_writer.writeTextElement("subtype", QString::number(curTemplate.getTemplateSubtype()));
    xml_writer.writeTextElement("descr", curTemplate.getName());
    xml_writer.writeEndElement();

    xml_writer.writeStartElement("page");
    xml_writer.writeTextElement("width", QString::number(doc->width));
    xml_writer.writeTextElement("height", QString::number(doc->height));
    xml_writer.writeEndElement();

    xml_writer.writeStartElement("texts");
    for (int i = 0; i < doc->listTextElements.count(); i++) {
        cEditorTextElement *e = doc->listTextElements.at(i);
        QFont f = e->getFont();
        xml_writer.writeStartElement("text");
        xml_writer.writeTextElement("pos_x", QString::number(e->pos().x()));
        xml_writer.writeTextElement("pos_y", QString::number(e->pos().y()));
        xml_writer.writeTextElement("content", e->toPlainText());
        xml_writer.writeTextElement("font", f.family());
        xml_writer.writeTextElement("fontsize", QString::number(f.pointSize()));
        xml_writer.writeTextElement("bold", f.bold() ? "1" : "0");
        xml_writer.writeTextElement("italic", f.italic() ? "1" : "0");
        xml_writer.writeTextElement("underline", f.underline() ? "1" : "0");
        xml_writer.writeTextElement("color", getColorString(e->defaultTextColor()));
        if (f.letterSpacing() > 0)
            xml_writer.writeTextElement("spacing", QString::number(f.letterSpacing()));
        xml_writer.writeEndElement();
    }
    xml_writer.writeEndElement(); // tag: texts

    xml_writer.writeStartElement("lines");
    for (int i = 0; i < doc->listLineElements.count(); i++) {
        cEditorLineElement *e = doc->listLineElements.at(i);
        QLineF l = e->line();
        xml_writer.writeStartElement("line");
        xml_writer.writeTextElement("pos_x1", QString::number(l.x1()));
        xml_writer.writeTextElement("pos_y1", QString::number(l.y1()));
        xml_writer.writeTextElement("pos_x2", QString::number(l.x2()));
        xml_writer.writeTextElement("pos_y2", QString::number(l.y2()));
        xml_writer.writeTextElement("style", QString::number(e->pen().style()));
        xml_writer.writeTextElement("color", getColorString(e->pen().color()));
        xml_writer.writeEndElement();
    }
    xml_writer.writeEndElement(); // tag: lines

    xml_writer.writeStartElement("images");
    for (int i = 0; i < doc->listPixmapElements.count(); i++) {
        cEditorPixmapElement *e = doc->listPixmapElements.at(i);
        xml_writer.writeStartElement("image");
        xml_writer.writeTextElement("pos_x", QString::number(e->pos().x()));
        xml_writer.writeTextElement("pos_y", QString::number(e->pos().y()));
        QString bin(qCompress(e->getData(), 9).toBase64());
        xml_writer.writeTextElement("data", bin);
        xml_writer.writeEndElement();
    }
    xml_writer.writeEndElement(); // tag: images

    xml_writer.writeStartElement("vectors");
    for (int i = 0; i < doc->listSvgElements.count(); i++) {
        cEditorSvgElement *e = doc->listSvgElements.at(i);
        xml_writer.writeStartElement("vector");
        xml_writer.writeTextElement("pos_x", QString::number(e->pos().x()));
        xml_writer.writeTextElement("pos_y", QString::number(e->pos().y()));
        QString bin(qCompress(e->getData(), 9).toBase64());
        xml_writer.writeTextElement("data", bin);
        xml_writer.writeEndElement();
    }
    xml_writer.writeEndElement(); // tag: images

    xml_writer.writeStartElement("fields");
    for (int i = 0; i < doc->listDataElements.count(); i++) {
        cEditorDataElement *e = doc->listDataElements.at(i);
        QFont f = e->getFont();
        xml_writer.writeStartElement("field");
        xml_writer.writeTextElement("pos_x", QString::number(e->pos().x()));
        xml_writer.writeTextElement("pos_y", QString::number(e->pos().y()));
        xml_writer.writeTextElement("width", QString::number(e->boundingRect().width()));
        xml_writer.writeTextElement("content", e->toPlainText());
        xml_writer.writeTextElement("align", QString::number(e->getAlignment()));
        xml_writer.writeTextElement("font", f.family());
        xml_writer.writeTextElement("fontsize", QString::number(f.pointSize()));
        xml_writer.writeTextElement("bold", f.bold() ? "1" : "0");
        xml_writer.writeTextElement("italic", f.italic() ? "1" : "0");
        xml_writer.writeTextElement("underline", f.underline() ? "1" : "0");
        xml_writer.writeTextElement("color", getColorString(e->defaultTextColor()));
        if (f.letterSpacing() > 0)
            xml_writer.writeTextElement("spacing", QString::number(f.letterSpacing()));
        xml_writer.writeEndElement();
    }
    xml_writer.writeEndElement(); // tag: fields

    xml_writer.writeEndElement(); // tag: template
    xml_writer.writeEndDocument();
    if (save_to_file) {
        file.close();
        doc->flag_change = false;
        return true;
    } else {
        bool bRet = false;
        // compress the template before writing it to the database
        QByteArray save_array(qCompress(array, 9).toBase64());
        curTemplate.setContents(save_array);
        if (curTemplate.getID() < 0) {
            int tid = - 1;
            db.existDocTemplate(curTemplate.getName(), &tid);
            if (tid > 0) {
                curTemplate.setID(tid);
                bRet = db.updateItem(curTemplate);
            } else {
                bRet = db.storeItem(curTemplate, &tid);
                curTemplate.setID(tid);
            }
        } else {
            bRet = db.updateItem(curTemplate);
        }
        buffer.close();
        return bRet;
    }
}


QString cDocTemplate::getColorString(QColor c)
{
    QStringList l;
    l << QString::number(c.red()) << QString::number(c.green()) << QString::number(c.blue()) << QString::number(c.alpha());
    return l.join(",");
}


QColor cDocTemplate::getColorFromString(QString s)
{
    QColor c;
    c.setRgb(0, 0, 0, 255);

    if (s.isEmpty()) return c;
    QStringList l = s.split(",");
    if (l.count() != 4) return c;
    int r = ((QString)(l.at(0))).toInt();
    int g = ((QString)(l.at(1))).toInt();
    int b = ((QString)(l.at(2))).toInt();
    int a = ((QString)(l.at(3))).toInt();
    c.setRgb(r, g, b, a);
    return c;
}


//
// cDoc
//
cDoc::cDoc(cDocTemplate *cdt)
    : QObject(NULL)
{
   docTemplate = cdt;

   scene = new QGraphicsScene();
   width = docTemplate->default_width;
   height = docTemplate->default_height;
   factor = 1.0;

   scene->setSceneRect(0, 0, width, height);
   docRect = NULL;

   flag_new = false;
   flag_change = false;
}


cDoc::~cDoc()
{
    deleteElements();
}


void cDoc::deleteElements()
{
    while (listTextElements.count() > 0) {
        cEditorTextElement *te = listTextElements.takeLast();
        scene->removeItem(te);
        delete te;
    }
    while (listLineElements.count() > 0) {
        cEditorLineElement *le = listLineElements.takeLast();
        scene->removeItem(le);
        delete le;
    }
    while (listPixmapElements.count() > 0) {
        cEditorPixmapElement *pe = listPixmapElements.takeLast();
        scene->removeItem(pe);
        delete pe;
    }
    while (listSvgElements.count() > 0) {
        cEditorSvgElement *se = listSvgElements.takeLast();
        scene->removeItem(se);
        delete se;
    }
    while (listDataElements.count() > 0) {
        cEditorDataElement *de = listDataElements.takeLast();
        scene->removeItem(de);
        delete de;
    }
    scene->clear();
}


void cDoc::updateSelectedTextElements()
{
    for (int i = 0; i < listTextElements.count(); i++) {
        cEditorTextElement *te = listTextElements.at(i);
        if (te->isSelected()) {
            te->setDefaultTextColor(docTemplate->curColor);
            te->setFont(docTemplate->curFont);
            flag_change = true;
        }
    }
    for (int i = 0; i < listDataElements.count(); i++) {
        cEditorDataElement *de = listDataElements.at(i);
        if (de->isSelected()) {
            de->setDefaultTextColor(docTemplate->curColor);
            de->setFont(docTemplate->curFont);
            flag_change = true;
        }
    }
}


void cDoc::createPageRect()
{
    if (docRect == NULL) {
        scene->setSceneRect(0, 0, width, height);
        docRect = new QGraphicsRectItem(1, 1, width - 2, height - 2);
        scene->addItem(docRect);
    }
}


void cDoc::showPageRect(bool b)
{
    if (docRect == NULL) return;
    if (b) docRect->show();
    else docRect->hide();
}


cEditorTextElement* cDoc::createTextItem(int x, int y, QString s, QFont f, QColor c)
{
    listTextElements.append(new cEditorTextElement(docTemplate));
    cEditorTextElement *te = listTextElements.last();
    te->setPlainText(s);
    te->setFont(f);
    te->setBaseFontSize(f.pointSizeF());
    te->setDefaultTextColor(c);
    scene->addItem(te);
    te->setPos(x, y);
#ifdef OPA
    auto de = qobject_cast<cEditorDataElement*>(te);
    connect(de, &cEditorDataElement::sigDeleteItem, this, &cDoc::slotDeleteItem);
#endif
    //te->setBaseWidth(te->boundingRect().width());
    return te;
}


cEditorLineElement* cDoc::createLine(int x1, int y1, int x2, int y2, int style, QColor c)
{
    listLineElements.append(new cEditorLineElement(docTemplate));
    cEditorLineElement *le = listLineElements.last();
    QPen pen;
    pen.setStyle((Qt::PenStyle)(style));
    pen.setColor(c);
    le->setPen(pen);
    le->setLine(x1, y1, x2, y2);
    scene->addItem(le);
#ifdef OPA
    connect(le, &cEditorLineElement::sigDeleteItem, this, &cDoc::slotDeleteItem);
#endif
    return le;
}


cEditorPixmapElement* cDoc::createPixmap(int x, int y, QString s)
{
    listPixmapElements.append(new cEditorPixmapElement(docTemplate, s));
    cEditorPixmapElement *pe = listPixmapElements.last();
    pe->setPos(x, y);
#ifdef OPA
    pe->setToolTip(s);
    connect(pe, &cEditorPixmapElement::sigDeleteItem, this, &cDoc::slotDeleteItem);
#endif
    scene->addItem(pe);
    return pe;
}


cEditorPixmapElement* cDoc::createPixmap(int x, int y, QByteArray qba)
{
    listPixmapElements.append(new cEditorPixmapElement(docTemplate, qba));
    cEditorPixmapElement *pe = listPixmapElements.last();
    pe->setPos(x, y);
    scene->addItem(pe);
#ifdef OPA
    connect(pe,  &cEditorPixmapElement::sigDeleteItem, this, &cDoc::slotDeleteItem);
#endif
    return pe;
}

class MySvgElement : public cEditorSvgElement {
public:
    MySvgElement(cDocTemplate* tpl, const QString &s)
        : cEditorSvgElement(tpl, s) {}

    QRectF boundingRect() const override {
        // deine Berechnung hier
        return QRectF(0, 0, 100, 100);
    }

    void paint(QPainter *p, const QStyleOptionGraphicsItem *opt, QWidget *w) override {
        // deine Mal-Routine
        p->drawRect(boundingRect());
    }
};

cEditorSvgElement* cDoc::createSvg(int x, int y, QString s)
{
    listSvgElements.append(new MySvgElement(docTemplate, s));
    cEditorSvgElement *se = listSvgElements.last();
    se->setPos(x, y);
#ifdef OPA
    se->setToolTip(s);
    connect(se, &cEditorSvgElement::sigDeleteItem, this, &cDoc::slotDeleteItem);
#endif
    scene->addItem(se);
    return se;
}


cEditorSvgElement* cDoc::createSvg(int x, int y, QByteArray qba)
{
    listSvgElements.append(new MySvgElement(docTemplate, qba));
    cEditorSvgElement *se = listSvgElements.last();
    se->setPos(x, y);
    scene->addItem(se);
#ifdef OPA
    connect(se, &cEditorSvgElement::sigDeleteItem, this, &cDoc::slotDeleteItem);
#endif
    return se;
}


cEditorDataElement* cDoc::createDataItem(int x, int y, int w, int align, QString s_name, QString l_name, QFont f, QColor c)
{
    listDataElements.append(new cEditorDataElement(docTemplate));
    cEditorDataElement *de = listDataElements.last();
    //QTextOption opt;
    //opt.setWrapMode(QTextOption::NoWrap);
    //de->document()->setDefaultTextOption(opt);
    de->setPos(x, y);
    de->setFont(f);
    de->setBaseFontSize(f.pointSizeF());
    de->setTextWidth(w);
    de->setBaseWidth(w);
    de->setPlainText(s_name);
    de->setDefaultTextColor(c);
    de->setAlignment(align);
#ifdef OPA
    de->setToolTip(l_name);
    // init the size (required for resizing the item later using the mouse
    de->setSize(w, de->document()->size().height());
    connect(de, &cEditorDataElement::sigDeleteItem, this, &cDoc::slotDeleteItem);
#endif
    de->setToken(s_name);
    scene->addItem(de);
    compare_data << s_name;
    return de;
}


void cDoc::slotDeleteItem(int t)
{
#ifdef OPA
    if (t == TYPE_TEXT) {
        cEditorTextElement *te = (cEditorTextElement *)(this->sender());
        for (int i = 0; i < listTextElements.count(); i++) {
            if (te == listTextElements.at(i)) {
                scene->removeItem(te);
                delete listTextElements.takeAt(i);
                flag_change = true;
                break;
            }
        }
    } else if (t == TYPE_LINE) {
        cEditorLineElement *le = (cEditorLineElement *)(this->sender());
        for (int i = 0; i < listLineElements.count(); i++) {
            if (le == listLineElements.at(i)) {
                scene->removeItem(le);
                delete listLineElements.takeAt(i);
                flag_change = true;
                break;
            }
        }
    } else if (t == TYPE_GFX) {
        cEditorPixmapElement *pe = (cEditorPixmapElement *)(this->sender());
        for (int i = 0; i < listPixmapElements.count(); i++) {
            if (pe == listPixmapElements.at(i)) {
                scene->removeItem(pe);
                delete listPixmapElements.takeAt(i);
                flag_change = true;
                break;
            }
        }
    } else if (t == TYPE_SVG) {
        cEditorSvgElement *se = (cEditorSvgElement *)(this->sender());
        for (int i = 0; i < listSvgElements.count(); i++) {
            if (se == listSvgElements.at(i)) {
                scene->removeItem(se);
                delete listSvgElements.takeAt(i);
                flag_change = true;
                break;
            }
        }
    } else if (t == TYPE_DATA) {
        cEditorDataElement *de = (cEditorDataElement *)(this->sender());
        for (int i = 0; i < listDataElements.count(); i++) {
            if (de == listDataElements.at(i)) {
                scene->removeItem(de);
                delete listDataElements.takeAt(i);
                flag_change = true;
                break;
            }
        }
    }
#endif
}


void cDoc::setFlagNew(bool b)
{
    flag_new = b;
}


bool cDoc::getFlagNew()
{
    return flag_new;
}


QString cDoc::getResult(QString token)
{
    QString qRet = "";

    for (int i = 0; i < listDataElements.count(); i++) {
        if (listDataElements.at(i)->getToken() == token) {
            qRet = listDataElements.at(i)->toPlainText();
            break;
        }
    }
    return qRet;
}


bool cDoc::changed()
{
    for (int i = 0; i < listDataElements.count(); i++) {
        if (listDataElements.at(i)->toPlainText() != compare_data.at(i)) return true;
    }
    return false;
}


void cDoc::replace(QString token, QString s)
{
    for (int i = 0; i < listDataElements.count(); i++) {
        if (listDataElements.at(i)->toPlainText() == token) {
            listDataElements.at(i)->setPlainText(s);
            compare_data.replace(i, s);
            break;
        }
    }
}

void cDoc::clearDataToken()
{
    for (int i = 0; i < listDataElements.count(); i++) {
        listDataElements.at(i)->setPlainText("");
    }
}


void cDoc::finish()
{
    for (int i = 0; i < listDataElements.count(); i++) {
        listDataElements.at(i)->setAlignment((Qt::Alignment)listDataElements.at(i)->getAlignment());
    }
    createPageRect();
// /    showPageRect(1);   // //////
}


int cDoc::getTemplateType()
{
    if (docTemplate == NULL) return -1;
    return docTemplate->curTemplate.getTemplateType();
}

QString cDoc::getCompData(int i)
{
    return compare_data[i];
}



//
// cEditorTextElement
//
cEditorTextElement::cEditorTextElement(cDocTemplate *cdt, QGraphicsItem *parent)
    : QGraphicsTextItem(parent)
{
    docTemplate = cdt;
    if (docTemplate->curMode == cDocTemplate::MODE_EDIT) {
        setFlag(QGraphicsItem::ItemIsMovable, true);
        setFlag(QGraphicsItem::ItemIsSelectable, true);
        setFlag(QGraphicsItem::ItemIsFocusable, true);
    } else {
        setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
    }
}


void cEditorTextElement::setFont(QFont f)
{
    font = f;
    QGraphicsTextItem::setFont(font);
}


QFont cEditorTextElement::getFont()
{
    return font;
}


qreal cEditorTextElement::getBaseFontSize()
{
    return base_size;
}


void cEditorTextElement::setBaseFontSize(qreal bs)
{
    base_size = bs;
}


/*int cEditorTextElement::getBaseWidth()
{
    return base_width;
}

void cEditorTextElement::setBaseWidth(int w)
{
    base_width = w;
}*/

#ifdef OPA
void cEditorTextElement::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    if (docTemplate->curMode == cDocTemplate::MODE_VIEW) {
        QGraphicsTextItem::contextMenuEvent(event);
        return;
    }

    QMenu menu;
    //QAction *actFont = menu.addAction(QIcon(":/docedit/images/applyfont.png"), QString::fromUtf8("Schriftart aktualisieren"));
    //actFont->setData(CMC_FONT);
    QAction *actFontDlg = menu.addAction(QIcon(":/docedit/images/fonts.png"), QString::fromUtf8("Schriftdialog"));
    actFontDlg->setData(CMC_FONT_DLG);
    QAction *actDel = menu.addAction(QIcon(":/docedit/images/cancel.png"), QString::fromUtf8("Löschen"));
    actDel->setData(CMC_DEL);

    QAction *selAct = menu.exec(event->screenPos());
    if (selAct != NULL) {
        switch(selAct->data().toInt()) {
            //case CMC_FONT:
            //    setFont(config->curFont);
            //    break;
            case CMC_FONT_DLG:
                setFont(docTemplate->editor->fontDialog(font));
                break;
            case CMC_DEL:
                emit sigDeleteItem(cDoc::TYPE_TEXT);
                break;
            default: break;
        }
    }
}


void cEditorTextElement::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event)
{
    if (textInteractionFlags() == Qt::NoTextInteraction)
        setTextInteractionFlags(Qt::TextEditorInteraction);

    // TODO: reset the selection somehow and set the cursor position
/*    QTextCursor c = textCursor();
    c.clearSelection();
    c.setPosition(c.position());
    this->setTextCursor(c);*/
    //c.select(QTextCursor::WordUnderCursor);

    QGraphicsTextItem::mouseDoubleClickEvent(event);
}


void cEditorTextElement::focusOutEvent(QFocusEvent * event)
{
    if (textInteractionFlags() == Qt::TextEditorInteraction) {
        // reset the cursor position in order to hide the cursor
        QTextCursor c = textCursor();
        c.setPosition(0);
        setTextCursor(c);
        setTextInteractionFlags(Qt::NoTextInteraction);
        // need to re-set the focusable flag since Qt::NoTextInteraction will
        // have an influence on that flag
        setFlag(QGraphicsItem::ItemIsFocusable, true);
    }
    QGraphicsTextItem::focusOutEvent(event);
}


void cEditorTextElement::focusInEvent(QFocusEvent* event)
{
    emit sigSelectTextItem(font, defaultTextColor());
    QGraphicsTextItem::focusInEvent(event);
}
#endif


//
// cEditorLineElement
//
cEditorLineElement::cEditorLineElement(cDocTemplate *cdt, QGraphicsItem *parent)
    : QGraphicsLineItem(parent)
{
    docTemplate = cdt;
    if (docTemplate->curMode == cDocTemplate::MODE_EDIT) {
        setFlag(QGraphicsItem::ItemIsMovable, true);
        setFlag(QGraphicsItem::ItemIsSelectable, true);
        setFlag(QGraphicsItem::ItemIsFocusable, true);
    }
}

#ifdef OPA
void cEditorLineElement::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    if (docTemplate->curMode == cDocTemplate::MODE_VIEW) {
        QGraphicsLineItem::contextMenuEvent(event);
        return;
    }

    QMenu menu;
    QAction *actProp = menu.addAction(QIcon(":/docedit/images/open.png"), QString::fromUtf8("Eigenschaften"));
    actProp->setData(CMC_PROPERTIES);
    QAction *actDel = menu.addAction(QIcon(":/docedit/images/cancel.png"), QString::fromUtf8("Löschen"));
    actDel->setData(CMC_DEL);

    QAction *selAct = menu.exec(event->screenPos());
    if (selAct != NULL) {
        switch(selAct->data().toInt()) {
            case CMC_PROPERTIES:
                break;
            case CMC_DEL:
                emit sigDeleteItem(cDoc::TYPE_LINE);
                break;
            default: break;
        }
    }
}


void cEditorLineElement::focusInEvent(QFocusEvent* event)
{
    emit sigSelectLineItem(this->pen().color());
    QGraphicsLineItem::focusInEvent(event);
}
#endif


//
// cEditorPixmapElement
//
cEditorPixmapElement::cEditorPixmapElement(cDocTemplate *cdt, const QString & fileName, QGraphicsItem *parent)
    : QGraphicsPixmapItem(QPixmap::fromImage(QImage(fileName)), parent)
{
    docTemplate = cdt;
    if (docTemplate->curMode == cDocTemplate::MODE_EDIT) {
        setFlag(QGraphicsItem::ItemIsMovable, true);
        setFlag(QGraphicsItem::ItemIsSelectable, true);
    }
}


cEditorPixmapElement::cEditorPixmapElement(cDocTemplate *cdt, QByteArray qba, QGraphicsItem *parent)
    : QGraphicsPixmapItem(QPixmap::fromImage(QImage::fromData(qba)), parent)
{

    docTemplate = cdt;
    if (docTemplate->curMode == cDocTemplate::MODE_EDIT) {
        setFlag(QGraphicsItem::ItemIsMovable, true);
        setFlag(QGraphicsItem::ItemIsSelectable, true);
    }
}


QByteArray cEditorPixmapElement::getData()
{
    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    pixmap().save(&buffer, "PNG");
    buffer.close();
    return ba;
}

#ifdef OPA
void cEditorPixmapElement::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    if (docTemplate->curMode == cDocTemplate::MODE_VIEW) {
        QGraphicsPixmapItem::contextMenuEvent(event);
        return;
    }

    QMenu menu;
    QAction *actDel = menu.addAction(QIcon(":/docedit/images/cancel.png"), QString::fromUtf8("Löschen"));
    actDel->setData(CMC_DEL);

    QAction *selAct = menu.exec(event->screenPos());
    if (selAct != NULL) {
        switch(selAct->data().toInt()) {
            case CMC_DEL:
                emit sigDeleteItem(cDoc::TYPE_GFX);
                break;
            default: break;
        }
    }
}
#endif

//
// cEditorSvgElement
//
//cEditorSvgElement::cEditorSvgElement(cDocTemplate* t, const QString& s, QGraphicsItem* parent)
//    : QGraphicsItem(parent), docTemplate(t), fileName(s)
cEditorSvgElement::cEditorSvgElement(cDocTemplate *cdt, const QString & fileName, QGraphicsItem *parent)
    : QGraphicsSvgItem(parent)
{
    docTemplate = cdt;
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly)) {
        qbaSvgData = file.readAll();
        file.close();
    }
    QSvgRenderer *renderer = new QSvgRenderer(qbaSvgData);
    setSharedRenderer(renderer);
    if (docTemplate->curMode == cDocTemplate::MODE_EDIT) {
        setFlag(QGraphicsItem::ItemIsMovable, true);
        setFlag(QGraphicsItem::ItemIsSelectable, true);
    }
}


cEditorSvgElement::cEditorSvgElement(cDocTemplate *cdt, QByteArray qba, QGraphicsItem *parent)
    : QGraphicsSvgItem(parent)
{
    docTemplate = cdt;
    qbaSvgData = qba;
    QSvgRenderer *renderer = new QSvgRenderer(qbaSvgData);
    setSharedRenderer(renderer);

    if (docTemplate->curMode == cDocTemplate::MODE_EDIT) {
        setFlag(QGraphicsItem::ItemIsMovable, true);
        setFlag(QGraphicsItem::ItemIsSelectable, true);
    }
}


QByteArray cEditorSvgElement::getData()
{
    return qbaSvgData;
}

#ifdef OPA
void cEditorSvgElement::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    if (docTemplate->curMode == cDocTemplate::MODE_VIEW) {
        QGraphicsSvgItem::contextMenuEvent(event);
        return;
    }

    QMenu menu;
    QAction *actDel = menu.addAction(QIcon(":/docedit/images/cancel.png"), QString::fromUtf8("Löschen"));
    actDel->setData(CMC_DEL);

    QAction *selAct = menu.exec(event->screenPos());
    if (selAct != NULL) {
        switch(selAct->data().toInt()) {
            case CMC_DEL:
                emit sigDeleteItem(cDoc::TYPE_SVG);
                break;
            default: break;
        }
    }
}
#endif


//
// ResizeDataRect
//
#ifdef OPA
ResizeDataRect::ResizeDataRect(int pos, QGraphicsItem *parent)
    : QGraphicsRectItem(0, 0, 6, 6, parent)
{
    my_pos = pos;
    wsize = 6;
    //setAcceptsHoverEvents(true);
    setVisible(false);
    locate();
}

ResizeDataRect::~ResizeDataRect()
{
}

int ResizeDataRect::type() const { return Type; }
void ResizeDataRect::setMyPos(int pos) { my_pos = pos; }
int ResizeDataRect::getMyPos() { return my_pos; }

void ResizeDataRect::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
    setCursor(Qt::SizeHorCursor);
    QGraphicsRectItem::hoverEnterEvent(event);
}

void ResizeDataRect::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
    QGraphicsRectItem::hoverLeaveEvent(event);
}

void ResizeDataRect::locate()
{
    const QRectF parentRect = this->parentItem()->boundingRect();
    qreal x = 0, y = 0;

    switch(my_pos){
        case RIGHT:
            x = parentRect.width() - 4;
            //x = parentRect.width() - wsize;
            y = parentRect.height() / 2 - wsize / 2;
            break;
        case LEFT:
            x = -3;
            // x = 0;
            y = parentRect.height() / 2 - wsize / 2;
            break;
        default: break;
    }
    setPos(x, y);
}
#endif


//
// cEditorDataElement
//
cEditorDataElement::cEditorDataElement(cDocTemplate *cdt, QGraphicsItem *parent)
    : QGraphicsTextItem(parent)
{
    docTemplate = cdt;
    width = 100;
    height = 20;
    token = "";

    if (docTemplate->curMode == cDocTemplate::MODE_EDIT) {
        setFlag(QGraphicsItem::ItemIsMovable, true);
        setFlag(QGraphicsItem::ItemIsSelectable, true);
        setFlag(QGraphicsItem::ItemIsFocusable, true);
#ifdef OPA
        ResizeDataRect *left = new ResizeDataRect(ResizeDataRect::LEFT, this);
        resizeRects.append(left);
        ResizeDataRect *right = new ResizeDataRect(ResizeDataRect::RIGHT,this);
        resizeRects.append(right);
#endif
        mark_field = true;
    } else {
        setTextInteractionFlags(Qt::TextEditorInteraction);
        setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
        mark_field = false;
    }
}


#ifdef OPA
void cEditorDataElement::setSize(int w, int h)
{
    width = w;
    height = h;
}

QRectF cEditorDataElement::boundingRect() const
{
    return QRectF(0, 0, width, height);
}

void cEditorDataElement::showResizeRect(bool b)
{
    if (docTemplate->curMode == cDocTemplate::MODE_VIEW) return;
    for(int i = 0; i < resizeRects.count(); i++) {
        resizeRects.at(i)->locate();
        resizeRects.at(i)->setVisible(b);
    }
}
#endif


void cEditorDataElement::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QPen pen;
    QColor backgroundColor(219, 223, 233, 128);
    if (option->state & QStyle::State_Selected) {
        pen.setColor(Qt::black);
        pen.setStyle(Qt::DotLine);
        pen.setWidth(1);
    } else {
        pen.setColor(Qt::white);
    }

#ifdef OPA
    if (docTemplate->curMode == cDocTemplate::MODE_EDIT) {
        if(option->state & QStyle::State_Selected) showResizeRect(true);
        else showResizeRect(false);
    }
#endif

    painter->setPen(pen);
    if (mark_field) {
        painter->setBrush(backgroundColor);
        QRectF rect = this->boundingRect();
        painter->drawRect(rect);
    }
    QGraphicsTextItem::paint(painter, option, widget);
}

#ifdef OPA
void cEditorDataElement::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (docTemplate->curMode == cDocTemplate::MODE_VIEW) {
        QGraphicsItem::mousePressEvent(event);
        return;
    }

    curResizeRect = qgraphicsitem_cast<ResizeDataRect*>(scene()->itemAt(event->scenePos(), QTransform()));
    if(curResizeRect) {
        curMode = MODE_RESIZE;
        lastPoint.setX(event->scenePos().x());
        lastPoint.setY(event->scenePos().y());
        dashRect = new QGraphicsRectItem(boundingRect(), this);
        dashRect->setPen(QPen(Qt::DashLine));
    } else {
        curMode = MODE_MOVE;
        QGraphicsItem::mousePressEvent(event);
    }
}


void cEditorDataElement::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (docTemplate->curMode == cDocTemplate::MODE_VIEW) {
        QGraphicsItem::mouseMoveEvent(event);
        return;
    }

    if(curMode == MODE_RESIZE) {
        QPointF curPoint(event->scenePos());
        qreal curX = 0;
        qreal curWidth = width;
        qreal distance = curPoint.x() - lastPoint.x();

        if (curResizeRect->getMyPos() == ResizeDataRect::LEFT) {
            curX += distance;
            curWidth-= distance;
        } else curWidth += distance;

        if (curWidth < 12) return;

        // update the content (text), too
        setTextWidth(curWidth);
        height = document()->size().height();
        dashRect->setRect(curX, 0, curWidth, height);
    } else QGraphicsItem::mouseMoveEvent(event);
}


void cEditorDataElement::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (docTemplate->curMode == cDocTemplate::MODE_VIEW) {
        QGraphicsItem::mouseReleaseEvent(event);
        return;
    }

    if(curMode == MODE_RESIZE) {
        width = dashRect->rect().width();
        height = dashRect->rect().height();
        qreal curX = dashRect->rect().left();
        qreal curY = dashRect->rect().top();
        QPointF curPos(curX, curY);
        setPos(mapToScene(curPos));
        delete dashRect;
        curMode = MODE_MOVE;
        scene()->update();
    } else QGraphicsItem::mouseReleaseEvent(event);
}


void cEditorDataElement::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    if (docTemplate->curMode == cDocTemplate::MODE_VIEW) {
        QGraphicsItem::contextMenuEvent(event);
        return;
    }

    QMenu menu;
    QMenu menu_align("Ausrichtung");
    QAction *actAlignLeft = menu_align.addAction(QString::fromUtf8("links"));
    actAlignLeft->setData(CMC_ALIGN_LEFT);
    QAction *actAlignCenter = menu_align.addAction(QString::fromUtf8("zentriert"));
    actAlignCenter->setData(CMC_ALIGN_CENTER);
    QAction *actAlignRight = menu_align.addAction(QString::fromUtf8("rechts"));
    actAlignRight->setData(CMC_ALIGN_RIGHT);
    menu.addMenu(&menu_align);

    QAction *actFontDlg = menu.addAction(QIcon(":/docedit/images/fonts.png"), QString::fromUtf8("Schriftdialog"));
    actFontDlg->setData(CMC_FONT_DLG);
    QAction *actDel = menu.addAction(QIcon(":/docedit/images/cancel.png"), QString::fromUtf8("Löschen"));
    actDel->setData(CMC_DEL);

    QAction *selAct = menu.exec(event->screenPos());
    if (selAct != NULL) {
        switch(selAct->data().toInt()) {
            case CMC_FONT_DLG:
                setFont(docTemplate->editor->fontDialog(font));
                break;
            case CMC_DEL:
                emit sigDeleteItem(cDoc::TYPE_DATA);
                break;
            case CMC_ALIGN_LEFT:
                setAlignment(Qt::AlignLeft);
                break;
            case CMC_ALIGN_CENTER:
                setAlignment(Qt::AlignCenter);
                break;
            case CMC_ALIGN_RIGHT:
                setAlignment(Qt::AlignRight);
                break;
            default: break;
        }
    }
}


void cEditorDataElement::focusInEvent(QFocusEvent* event)
{
    if (docTemplate->curMode == cDocTemplate::MODE_EDIT) {
        emit sigSelectDataItem(font, defaultTextColor());
    }
    QGraphicsTextItem::focusInEvent(event);
}
#endif


void cEditorDataElement::setAlignment(int i)
{
    align = i;

    QTextCursor cursor = textCursor();
    cursor.select(QTextCursor::Document);
    QTextBlockFormat bfmt = cursor.blockFormat();
    bfmt.setAlignment((Qt::Alignment)align);
    cursor.setBlockFormat(bfmt);
    cursor.clearSelection();
    setTextCursor(cursor);
}


int cEditorDataElement::getAlignment()
{
    return align;
}


void cEditorDataElement::setFont(QFont f)
{
    font = f;
    QGraphicsTextItem::setFont(font);
}


QFont cEditorDataElement::getFont()
{
    return font;
}


void cEditorDataElement::setToken(QString s)
{
    token = s;
}


QString cEditorDataElement::getToken()
{
    return token;
}


void cEditorDataElement::setMarkField(bool b)
{
    mark_field = b;
}


qreal cEditorDataElement::getBaseFontSize()
{
    return base_size;
}


void cEditorDataElement::setBaseFontSize(qreal bs)
{
    base_size = bs;
}


int cEditorDataElement::getBaseWidth()
{
    return base_width;
}


void cEditorDataElement::setBaseWidth(int w)
{
    base_width = w;
}
