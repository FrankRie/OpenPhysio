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

#ifndef BROWSE_H_
#define BROWSE_H_

#include <QtSql>
#include <QtWidgets>
#include <QProxyStyle> // WICHTIG: Für den Stil-Proxy
#include "displayfilter.h"
#include "mydb.h"

// Vorwärtsdeklarationen für den Compiler
class TableFilter;
class QSortFilterProxyModel;
class CustomSqlModel;
// class QSqlQueryModel; // bzw. CustomSqlModel, falls das existiert
// class TableViewAnimated; // Das ist Ihre eigene Animations-Klasse!



// 1. Die Stil-Klasse für das gesamte Projekt bereitstellen
class NoFocusStyle : public QProxyStyle {
public:
    void drawPrimitive(PrimitiveElement element, const QStyleOption *option,
                       QPainter *painter, const QWidget *widget = nullptr) const override {
        // Unterdrückt den Fokus-Balken (|) unter Windows vollständig
        if (element == QStyle::PE_FrameFocusRect) {
            return;
        }
        QProxyStyle::drawPrimitive(element, option, painter, widget);
    }
};


class TableFilter
{
private:
    QStringList filter;
    QStringList filter_static;
    int table_id;
    QStringList table_items;
    int default_cj;

    QString getField(int f);
    void addFilterExpr(bool stat, int field, int dt, int op, QString expr);

public:
    enum operators {OP_EQUAL = 1, OP_NOTEQUAL, OP_GREATER, OP_GREATER_OR_EQUAL, OP_LOWER, OP_LOWER_OR_EQUAL, OP_CONTAIN};
    enum datatype {DT_STRING = 1, DT_INTEGER};
    enum conjunction {CJ_AND = 1, CJ_OR};
    enum fields { F_PAT_ID = 1000, F_INSURANCE_AID, F_PAT_STATE };

    TableFilter(int tid = -1);
    ~TableFilter();
    void clear();
    void clearStaticFilter();
    TableFilter& operator=(TableFilter& other);
    int getTableID();
    QStringList getTableItems();

    QStringList getFilterList();
    QStringList getStaticFilterList();
    QString getFilterString();
    void addFilterExpr(bool stat, int field, int op, QString expr);
    void addFilterExpr(bool stat, int field, int op, int expr);
};



class CustomProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    CustomProxyModel(QObject *parent = 0);
    ~CustomProxyModel();
    void setFilter(int f, int i, int o);
    void enableFilter(bool f);

protected:
    virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;

private:
    bool filter;
    int field, id;
    int op;
};


/* class CustomSqlModel : public QSqlQueryModel
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
*/

class TableViewAnimated: public QTableView //, public BackgroundAnimation
{
    Q_OBJECT
public:
   TableViewAnimated(QWidget *parent = 0);
   virtual ~TableViewAnimated();

   void startAnimation();
   void stopAnimation();

protected:
   virtual void paintEvent(QPaintEvent *event);

private:
   QMovie *movie;
   bool animationVisible;
};


class Browse : public QWidget
{
    Q_OBJECT
private:
    TableFilter *tableFilter;
    int table_type, sort_column, sort_order;
    int last_active_item;
//    QSortFilterProxyModel *proxyModel;
 //   CustomProxyModel *proxyModel;
//    CustomSqlModel *model;
//    QTableView *view;
//    TableViewAnimated *view;
//    QStringList listHeaders;


    // Nur noch EINE Variable pro Typ, strictly Qt-Standard:
    QSortFilterProxyModel *proxyModel;
    CustomSqlModel *model; // Oder QSqlTableModel, falls Sie direkt editieren
    TableViewAnimated *view; // Hier nutzen wir Ihre animierte View!

    QStringList listHeaders;


    void hideHelperColumns();

public:
    enum fields { F_REC_PID = 7,
		  F_REPORT_PID = 4, F_REPORT_RID = 5,
		  F_FINDING_PID = 5,
		  F_INVOICE_PID = 7, F_INVOICE_RID = 8, F_INVOICE_CID = 9,
		  F_REMINDER_PID = 5, F_REMINDER_RID = 6, F_REMINDER_CID = 7,
		  F_QUITTANCE_PID = 6, F_QUITTANCE_RID = 7,
                  F_INSURANCE_AID = 4,
                  F_CASH_ADDPAY_PID = 7
	      };
    enum operators { OP_EQUAL, OP_NOTEQUAL };
    enum modes { MODE_FULL = 0, MODE_SELECT, MODE_MULTISELECT, MODE_CASH, MODE_BILL, MODE_EMBEDDED,
                 MODE_CASH_ADDPAY, MODE_CASH_SELFPAY, MODE_CASH_PRIVRECEIPE };
    Browse(int mode = 0, QWidget *parent = 0);
    virtual ~Browse();
//    void init(QTableView *tv, int t, int def_filter_header = 0);
//    void init(QTableView *tv, int t, int s_column, int s_order, int def_filter_header = 0);
    void init(TableViewAnimated *tv, int t, int def_filter_header = 0);
    void init(TableViewAnimated *tv, int t, int s_column, int s_order, int def_filter_header = 0);
    void enableColorMode(bool b);
    void enableBackgroundColorMode(bool activate, int keycolumn);
    void setBackgroundKeywords(QStringList& keywords, QList<QColor>& colors);
    void enableForegroundColorMode(bool activate, int keycolumn);
    void setForegroundKeywords(QStringList& keywords, QList<QColor>& colors);
    void preload();
    void setFilter(TableFilter *filter);
    //void setFilter(int field, int id, int op);
    int getSelectedItem();
    int getTableType();
    int getMode();

protected:
    DisplayFilter displayFilter;
    QToolBar browseToolBar;
    int curMode;
    void ReloadTable();

public slots:
    void slotSortItems();
    void slotUpdate(int area, int op, int item_id);
    void slotDisplayFilter(QString s, int column);
    void slotDataArrived();

signals:
    void sigItemSelected();
};

#endif /*BROWSE_H_*/
