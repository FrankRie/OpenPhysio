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

#ifndef DOCTOOLS_H
#define DOCTOOLS_H

#include <QtSvg>
#include <QtWidgets>
#include <QSvgRenderer>
#include <QGraphicsSvgItem>
#include "mydb.h"

class EditorView;
class DlgDocEditor;
class cDoc;
class cEditorTextElement;
class cEditorLineElement;
class cEditorPixmapElement;
class cEditorSvgElement;
class cEditorDataElement;


//
// cDocTemplate
//
class cDocTemplate: public QObject
{
    Q_OBJECT

private:
    //QStringList *listCategoryItems[cTemplateDataCategory::CAT_COUNT];
    //QStringList listCategoryItems[cTemplateDataCategory::CAT_COUNT];
    QList<cTemplateType*> listTemplateTypes;
    QList<cTemplateAvailDataCategory*> listAvailCategories;
    QList<cTemplateDataCategory*> listCategories;
    QList<cTemplateDataMapping*> listMappings;
    QMap<QString, QString> mapNames;
    enum opcodes { OPC_INVALID = -1, OPC_PAGE = 0, OPC_FONT, OPC_TEXT, OPC_DATA, OPC_LINE, OPC_IMAGE, OPC_SVG };

public:
    enum modes { MODE_EDIT, MODE_VIEW };
    QMap<int, QString> mapCategories;
    QFont curFont;
    QColor curColor;
    int curMode;
    DlgDocEditor *editor;
    cTemplate curTemplate;
    cDoc *doc;
    int default_datafield_width;
    int width_array[4] = {1000 /*744*/, 1052, 186, 93};
    int height_array[4] = {1414 /*1052*/, 744, 263, 132};
    int default_width, default_height;

    cDocTemplate();
    ~cDocTemplate();
    void setMode(int mode);
    int getTemplateID(QString s);
    int getCategoryID(QString s);
    QString getShortName(QString s);
    QString getLongName(QString s);
    QList<int> getAllowedCategoryIDs(int tmpl_type);
    QStringList getCategoryItems(int cat_id);
    QStringList getTemplateNames();
    QString getTemplateName(int tmpl_type);

    void setDoc(cDoc *d);
    bool parseDocProperties(QXmlStreamReader& xml);
    bool parsePageProperties(QXmlStreamReader& xml);
    bool parseTextProperties(QXmlStreamReader& xml, int t);
    bool parseLineProperties(QXmlStreamReader& xml);
    bool parseImageProperties(QXmlStreamReader& xml, int t);
    bool readTemplateFromFile(QString filename);
    bool readTemplateFromByteArray(QByteArray qba);
    bool readTemplate(QXmlStreamReader& xml);
    bool saveTemplate(bool save_to_file = false);
    QString getColorString(QColor c);
    QColor getColorFromString(QString s);
};


//
// cDoc
//
class cDoc: public QObject
{
    Q_OBJECT

private:
    cDocTemplate *docTemplate;
    bool flag_new;
    QStringList compare_data;

public:
    enum ITEM_TYPES { TYPE_TEXT, TYPE_GFX, TYPE_SVG, TYPE_DATA, TYPE_LINE, TYPE_RECT };
    enum factors { ZOOM_25 = 0, ZOOM_50, ZOOM_75, ZOOM_100, ZOOM_125, ZOOM_150, ZOOM_200,
                   ZOOM_400, ZOOM_HEIGHT, ZOOM_WIDTH };
    qreal factor;
    int width, height;
    QString descr;
    QString filename;
    bool flag_change;
    QGraphicsScene *scene;
    QGraphicsRectItem *docRect;
    QList<cEditorTextElement*> listTextElements;
    QList<cEditorLineElement*> listLineElements;
    QList<cEditorPixmapElement*> listPixmapElements;
    QList<cEditorSvgElement*> listSvgElements;
    QList<cEditorDataElement*> listDataElements;

    cDoc(cDocTemplate *cdt);
    ~cDoc();
    void deleteElements();
    void updateSelectedTextElements();
    void createPageRect();
    void showPageRect(bool b);
    cEditorTextElement* createTextItem(int x, int y, QString s, QFont f, QColor c);
    cEditorLineElement* createLine(int x1, int y1, int x2, int y2, int style, QColor c);
    cEditorPixmapElement* createPixmap(int x, int y, QString s);
    cEditorPixmapElement* createPixmap(int x, int y, QByteArray qba);
    cEditorSvgElement* createSvg(int x, int y, QString s);
    cEditorSvgElement* createSvg(int x, int y, QByteArray qba);
    cEditorDataElement* createDataItem(int x, int y, int w, int align, QString s_name, QString l_name, QFont f, QColor c);
    void setFlagNew(bool b);
    bool getFlagNew();
    QString getResult(QString token);
    bool changed();
    void replace(QString token, QString s);
    void clearDataToken();
    void finish();
    int getTemplateType();
    QString getCompData(int i);

private slots:
    void slotDeleteItem(int t);
};


//
// cEditorTextElement
//
class cEditorTextElement: public QGraphicsTextItem
{
    Q_OBJECT

private:
    QFont font;
    cDocTemplate *docTemplate;
    enum CMC { CMC_FONT, CMC_FONT_DLG, CMC_DEL };
    qreal base_size;
    //int base_width;

public:
    cEditorTextElement(cDocTemplate *cdt, QGraphicsItem *parent = NULL);
    void setFont(QFont f);
    QFont getFont();
    qreal getBaseFontSize();
    void setBaseFontSize(qreal bs);
    //int getBaseWidth();
    //void setBaseWidth(int w);

#ifdef OPA
protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent*event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);
    void focusOutEvent(QFocusEvent* event);
    void focusInEvent(QFocusEvent* event);
#endif

signals:
    void sigSelectTextItem(const QFont &font, const QColor &color);
    void sigDeleteItem(int t);
};


//
// cEditorLineElement
//
class cEditorLineElement: public QObject, public QGraphicsLineItem
{
    Q_OBJECT

private:
    enum CMC { CMC_DEL, CMC_PROPERTIES };
    cDocTemplate *docTemplate;

public:
    cEditorLineElement(cDocTemplate *cdt, QGraphicsItem *parent = NULL);

#ifdef OPA
protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
    void focusInEvent(QFocusEvent* event);
#endif

signals:
    void sigSelectLineItem(QColor);
    void sigDeleteItem(int);

};


//
// cEditorPixmapElement
//
class cEditorPixmapElement: public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

private:
    enum CMC { CMC_DEL };
    cDocTemplate *docTemplate;

public:
    cEditorPixmapElement(cDocTemplate *cdt, const QString & fileName, QGraphicsItem *parent = NULL);
    cEditorPixmapElement(cDocTemplate *cdt, QByteArray qba, QGraphicsItem *parent = NULL);
    QByteArray getData();

#ifdef OPA
protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

signals:
    void sigDeleteItem(int);
#endif
};


//
// cEditorSvgElement
//
class cEditorSvgElement: public QGraphicsSvgItem
{
    Q_OBJECT

private:
    enum CMC { CMC_DEL };
    QByteArray qbaSvgData;
    cDocTemplate *docTemplate;

public:
    cEditorSvgElement(cDocTemplate *cdt, const QString & fileName, QGraphicsItem *parent = NULL);
    cEditorSvgElement(cDocTemplate *cdt, QByteArray qba, QGraphicsItem *parent = NULL);
    QByteArray getData();

#ifdef OPA
protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

signals:
    void sigDeleteItem(int);
#endif
};


//
// ResizeDateRect
//
#ifdef OPA
class ResizeDataRect : public QGraphicsRectItem
{
public:
    enum positions {LEFT, RIGHT};
    enum { Type = UserType + 1 };

    ResizeDataRect(int pos, QGraphicsItem *parent);
    ~ResizeDataRect();

    int type() const;
    void setMyPos(int pos);
    int getMyPos();
    void locate();

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);

private:
    int my_pos;
    qreal wsize;
};
#endif


//
// cEditorDataElement
//
class cEditorDataElement: public QGraphicsTextItem
{
    Q_OBJECT

private:
    enum CMC { CMC_DEL, CMC_FONT_DLG, CMC_ALIGN_LEFT, CMC_ALIGN_RIGHT, CMC_ALIGN_CENTER };
    enum modes { MODE_MOVE, MODE_RESIZE };
    cDocTemplate *docTemplate;
    int align;
    int width, height;
    QFont font;
    QString token;
    bool mark_field;
    qreal base_size;
    int base_width;
#ifdef OPA
    QList<ResizeDataRect*> resizeRects;
    ResizeDataRect *curResizeRect;
    int curMode;
    QPointF lastPoint;
    QGraphicsRectItem *dashRect;
    void showResizeRect(bool b);
#endif

public:
    cEditorDataElement(cDocTemplate *tc, QGraphicsItem *parent = NULL);
#ifdef OPA
    QRectF boundingRect() const;
#endif
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *);
    void setAlignment(int i);
    int getAlignment();
    void setFont(QFont f);
    QFont getFont();
    void setSize(int w, int h);
    void setToken(QString s);
    QString getToken();
    void setMarkField(bool b);
    qreal getBaseFontSize();
    void setBaseFontSize(qreal bs);
    int getBaseWidth();
    void setBaseWidth(int w);

#ifdef OPA
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
    void focusInEvent(QFocusEvent* event);
#endif

signals:
    void sigDeleteItem(int);
    void sigSelectDataItem(QFont f, QColor c);

};


#endif // DOCTOOLS_H
