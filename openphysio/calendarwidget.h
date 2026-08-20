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

#ifndef CALENDARWIDGET_H
#define CALENDARWIDGET_H

#include <QtWidgets>
#include "mydb.h"

namespace Ui {
    class CalendarWidget;
}


#define MAX_RENDER_DAYS 7

class CalendarView;
class CalendarGrid;
class CalendarWidget;
class CalendarDockWidget;


//
// cResource
//
// This is a meta class for representing employees or rooms depending on the
// display mode.
//
class cResource
{
private:
    QString short_name, long_name;
    int id;

public:
    cResource() {}
    cResource(int i, QString s, QString l) { id = i; short_name = s; long_name = l; }
    QString getShortName() { return short_name; }
    void setShortName(QString s) { short_name = s; }
    QString getLongName() { return long_name; }
    void setLongName(QString s) { long_name = s; }
    int getID() { return id; }
    void setID(int i) { id = i; }
};


class CalendarConfig
{
public:
    enum render_modes { RENDER_MODE_DAY = 0, RENDER_MODE_WEEK, RENDER_MODE_COUNT };
    enum resource_modes { RES_MODE_EMPLOYEES = 0, RES_MODE_ROOMS, RES_MODE_CARS };
    int render_mode;
    int res_mode;
    int numRenderDays;
    int horizontal_header_height;
    int horizontal_overall_header_height;
    int vertical_header_width;
    int fulldayheader_height;
    int hour_start, hour_end;
    int hour_height_base;
    int hour_height;
    int scroll_step;
    int scrollbar_width;
    QList<cResource*> resources;
    int max_resources_to_display;
    int cur_resources_to_display;
    QMap<int, int> mapResource;
    int group, therapy_group;
    QLinearGradient headerGradient;
    QPen penFrame, penGridLight, penScrollArrow, penScrollbar;
    QBrush brushGridDark, brushHover, brushScrollbar, brushScollContainer, brushHoverRes;
    QBrush brushDates[cDateType::DT_COUNT];
    QBrush brushVisit;
    QFont fontDate, fontResHeader, fontHeader, fontGrid;
    CalendarView *view;
    CalendarGrid *grid;
    CalendarDockWidget *dock;
    CalendarWidget *calWidget;
    bool render_start_end;
    bool render_activities;

    CalendarConfig();
    ~CalendarConfig();
    QBrush getDateBrush(cDateItem& cdi);
    void setRenderHours(int start, int end);
    int getNumResourcesToRender();
    void setNumResourcesToRender(int i);
    int getNumResourcesAvailable();
    QStringList getResourceNames();
    QString getResourceShortName(int column);
    QString getResourceLongName(int column);
    int getResourceID(int column);
    int getResMode();
    void setResMode(int i);
};


//
// CalendarItem
//
class CalendarItem : public QGraphicsItem
{
public:
    CalendarItem(CalendarConfig *cc, QGraphicsItem *parent = 0);
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF boundingRect() const;
    void setSize(int w, int h);
    virtual void renderItem();

protected:
    QRectF bRect;
    CalendarConfig *config;
};


//
// CalendarDate
//
class CalendarDate : public CalendarItem
{
public:
    cDateItem cdi;

    CalendarDate(CalendarConfig *cc, cDateItem *c, QGraphicsItem *parent = 0);
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QString getDateDescription();

protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);
    void contextMenuEvent(QGraphicsSceneContextMenuEvent * event);

private:
    bool dragging;
    bool ignore_move_event;
    CalendarGrid *grid;
    QRectF clipRect;
};


//
// FullDayItem
//
class FullDayHeaderItem : public CalendarItem
{
public:
    FullDayHeaderItem(CalendarConfig *cc, QGraphicsItem *parent = 0);
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent * event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    void wheelEvent(QGraphicsSceneWheelEvent *event);
};


//
// CalendarHeader
//
class CalendarHeader : public CalendarItem
{
public:
    CalendarHeader(CalendarConfig *cc, QGraphicsItem *parent = 0);
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    virtual void renderItem();

protected:
    void wheelEvent(QGraphicsSceneWheelEvent *event);
};


//
// CalendarResHeaderItem
//
class CalendarResHeaderItem : public CalendarItem
{
public:
    CalendarResHeaderItem(CalendarConfig *cc, int header_id, int column_id, QGraphicsItem *parent = 0);
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

private:
    QString title;
    int id_header;
    int id_column;
};


//
// CalendarHeaderItem
//
class CalendarHeaderItem : public CalendarItem
{
public:
    CalendarHeaderItem(CalendarConfig *cc, int header_id, QGraphicsItem *parent = 0);
    ~CalendarHeaderItem();
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void setDate(QDate d);
    virtual void renderItem();
    int getColumnWidth(int col);
    int getColumnPosX(int col);
    QList<int> getColumnPositions();
    QDate getDate();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
    QRectF fontRect;
    QString title, title_short;
    QDate date;
    int id;
    QList<CalendarResHeaderItem*> res;
    QList<int> columnWidths;
};


//
// CalendarGrid
//
class CalendarGrid : public CalendarItem
{
public:
    CalendarGrid(CalendarConfig *cc, QGraphicsItem *parent = 0);
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void setVerticalGrid(QList<int> vg);
    int getTimeOffset(QTime t);
    QTime getTimeForOffset(QPointF point);
    QString getResourceForOffset(QPointF point, int *column);
    int getDayOffsetForPos(QPointF pos);
    cDateItem prepareNewDate(QPointF pos);

protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent * event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

private:
    QList<int> verticalGrid;
};



//
// CalendarScrollbar
//
class CalendarScrollbar : public CalendarItem
{
public:
    CalendarScrollbar(CalendarConfig *cc, QGraphicsItem *parent = 0);
    ~CalendarScrollbar();
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    virtual void renderItem();

private:
    QGraphicsPolygonItem *arrow_top;
    QGraphicsPolygonItem *arrow_bottom;
};


//
// CalendarScrollContainer
//
class CalendarScrollContainer : public CalendarItem
{
public:
    CalendarScrollContainer(CalendarConfig *cc, QGraphicsItem *parent = 0);
    ~CalendarScrollContainer();
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void setScrollItem(CalendarItem *item);
    virtual void renderItem();
    void scroll(int delta);

protected:
    void wheelEvent(QGraphicsSceneWheelEvent *event);

private:
    CalendarItem *scrollItem;
    CalendarScrollbar *scrollbar;
};


//
// CalendarView
//
class CalendarView : public QGraphicsView
{
    Q_OBJECT
public:
    QList<CalendarHeaderItem*> headerItems;
    CalendarScrollContainer* scrollContainer;
    FullDayHeaderItem *fullDayHeaderItem;

    CalendarView(CalendarConfig *cc, QWidget *parent = 0);
    ~CalendarView();
    void setRenderMode(int m);
    void setDate(QDate d);
    QDate getDate();
    void setRenderDaysForWeek(int i);
    void expandHeader(int id, bool no_inverse);
    void startSelectResource(int header_id, int column_id);
    void dateEdit(cDateItem& cdi);
    void dateDel(cDateItem& cdi);
    void dateUpdateStatus(cDateItem& cdi);
    void renderCalendar();
    QDate getDateForHeader(int header_id);
    void contextMenu(cDateItem cdi);
    void updateComboBox();
    void buildDates();
    bool detectAndAckOverlap(cDateItem& cdi);

protected:
    void resizeEvent(QResizeEvent * event);
    void keyPressEvent(QKeyEvent * event);

private:
    // context menu commands
    enum cmc { CMC_NEXT_DAY, CMC_PREV_DAY, CMC_NEXT_WEEK, CMC_PREV_WEEK, CMC_EDIT, CMC_NEW, CMC_DEL,
               CMC_MOVE, CMC_FOLLOWUP, CMC_EDIT_PATIENT, CMC_EDIT_RECEIPE, CMC_VIEW, CMC_STATE };
    CalendarConfig *config;
    CalendarHeader *header;
    CalendarGrid *calGrid;
    int expandedHeader;
    QDate curDate;
    QComboBox combo;
    QGraphicsItem *ptrCombo;
    QList<cDateItem *> listDates;
    QList<CalendarDate*> calDates;

private slots:
    void slotResourceSelected(int i);
};


//
// CalendarDockWidget
//
class CalendarDockWidget : public QDockWidget
{
    Q_OBJECT
public:
    QBoxLayout::Direction direction;
    QWidget *content;
    QWidget *part1, *part2, *part3, *part4;
    QFrame *line_1, *line_2, *line_3;
    QBoxLayout *boxLayout;
    QCalendarWidget *calendarWidget;
    QComboBox *cbView, *cbResMode, *cbGroup, *cbTherapyGroup;
    QSpinBox *spFrom, *spTo, *spWeekDays, *spMaxRes;
    QPushButton *pbMinus, *pbPlus, *pbEmployees, *pbTherapyGroups, *pbRooms, *pbCars;
    QPushButton *pbDateNew, *pbDatePrint;
    //QPushButton *pbDateSearch;
    //QSpacerItem *spacer_h, *spacer_v;

    explicit CalendarDockWidget(QWidget *parent = 0);
    ~CalendarDockWidget();

private slots:
    void slotLocationChanged(Qt::DockWidgetArea area);
};


//
// CalendarWidget
//
class CalendarWidget : public QWidget {
    Q_OBJECT
public:
    CalendarWidget(CalendarDockWidget *d, QWidget *parent = 0);
    ~CalendarWidget();

    void createNewDate(cDateItem& cdi);
    QToolBar* getToolBar();
    void enableToolBar(bool b);
    void MoveDate(cDateItem& cdi);
    void FollowUpDate(cDateItem& cdi);
    void restoreRowHeight();
    int getRowHeight();
    void editPatient(cDateItem& cdi);
    void editReceipe(cDateItem& cdi);

private:
    QGridLayout *layout;
    CalendarDockWidget *dock;
    CalendarConfig *config;
    CalendarView *view;
    QToolBar *calToolBar;

    void buildToolBar();
    void updateEmployees();

public slots:
    void slotSelectView(int i);
    void slotDateNew();

private slots:
    void slotDateChange();
    void slotHourChange();
    void slotResourceView(int i);
    void slotResourceNumbers(int i);
    void slotRowIncreaseHeight();
    void slotRowDecreaseHeight();
    void slotWeekDays(int i);
    void slotDatabaseChanged(int t);
    //void slotDateSearch();
    void slotDatePrint();
    void slotGroup(int i);
    void slotTherapyGroup(int i);
    void slotManageEmployees();
    void slotManageRooms();
    void slotManageTherapyGroups();
    void slotManageCars();
    void slotNextDay();
    void slotPrevDay();
    void slotToday();
    void slotModeDay();
    void slotModeWeek();
    void slotModeFullWeek();
    void slotToolBarLocationChanged(Qt::Orientation orientation);

signals:
    void sigManageEmployees();
};

#endif // CALENDARWIDGET_H
