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

#include <QtWidgets>
#include "calendarwidget.h"
#include "config.h"
#include "dlgdate.h"
#include "msgbox.h"
#include "dlgmanagerooms.h"
#include "dlgmanagetherapygroups.h"
#include "dlgmanagecars.h"
#include "protect.h"
#include "dlgdatemove.h"
#include "employeetools.h"
#include "dlgschedule.h"
#include "datetools.h"
#include "dlgpatient.h"
#include "dlgreceipe.h"
#include "dlgreceipepriv.h"


//
// CalendarConfig
//
CalendarConfig::CalendarConfig()
{
    view = NULL;
    grid = NULL;

    horizontal_header_height = 48;
    vertical_header_width = 36;
    fulldayheader_height = 24;
    horizontal_overall_header_height = horizontal_header_height + fulldayheader_height;
    setRenderHours(myConf.get(ConfXML::AREA_PLANNER, "work_start").toInt(),
                   myConf.get(ConfXML::AREA_PLANNER, "work_end").toInt());
    numRenderDays = myConf.get(ConfXML::AREA_PLANNER, "work_days").toInt();
    int d = myConf.get(ConfXML::AREA_PLANNER, "maxresdisplay").toInt();
    if (d > 0) max_resources_to_display = d;
    else max_resources_to_display = 6;

    hour_height_base = 12;
    hour_height = 12 * 5;
    scroll_step = 20;
    scrollbar_width = 16;
    cur_resources_to_display = 0;

    render_start_end = (bool)myConf.get(ConfXML::AREA_PLANNER, "render_start_end").toInt();
    render_activities = (bool)myConf.get(ConfXML::AREA_PLANNER, "render_activities").toInt();

    group = 0;
    therapy_group = 0;

    headerGradient.setColorAt(0, QColor(255, 255, 255));
    headerGradient.setColorAt(1, QColor(210, 210, 210));
    headerGradient.setStart(0, 0);

    penFrame.setColor(QColor(0, 0, 0, 255));
    penFrame.setStyle(Qt::SolidLine);
    penGridLight.setColor(QColor(178, 178, 178, 255));
    penGridLight.setStyle(Qt::SolidLine);
    penScrollArrow.setColor(QColor(100, 100, 100, 255));
    penScrollArrow.setStyle(Qt::SolidLine);
    penScrollbar.setColor(QColor(0, 0, 0, 255));
    penScrollbar.setStyle(Qt::SolidLine);

    brushGridDark.setColor(QColor(210, 210, 210, 255));
    brushGridDark.setStyle(Qt::SolidPattern);
    brushHover.setColor(QColor(60, 128, 190, 128));
    brushHover.setStyle(Qt::SolidPattern);
    brushHoverRes.setColor(QColor(60, 190, 128, 128));
    brushHoverRes.setStyle(Qt::SolidPattern);
    brushScrollbar.setColor(QColor(210, 210, 210, 255));
    brushScrollbar.setStyle(Qt::SolidPattern);
    brushScollContainer.setColor(QColor(245, 245, 230, 255));
    brushScollContainer.setStyle(Qt::SolidPattern);

    for (int i = 0; i < cDateType::DT_COUNT; i++) {
        brushDates[i].setColor(QColor(255, 255, 255, 255));
        brushDates[i].setStyle(Qt::SolidPattern);
    }
    brushDates[cDateType::DT_THERAPY].setColor(myConf.getColorFromString(myConf.get(ConfXML::AREA_PLANNER, "colTherapy")));
    brushDates[cDateType::DT_COURSE].setColor(myConf.getColorFromString(myConf.get(ConfXML::AREA_PLANNER, "colCourse")));
    brushDates[cDateType::DT_MEETING].setColor(myConf.getColorFromString(myConf.get(ConfXML::AREA_PLANNER, "colMeeting")));
    brushDates[cDateType::DT_TRAINING].setColor(myConf.getColorFromString(myConf.get(ConfXML::AREA_PLANNER, "colTraining")));
    brushDates[cDateType::DT_HOLIDAY].setColor(myConf.getColorFromString(myConf.get(ConfXML::AREA_PLANNER, "colHoliday")));
    brushDates[cDateType::DT_CORPORATE_HOLIDY].setColor(myConf.getColorFromString(myConf.get(ConfXML::AREA_PLANNER, "colCorpHoliday")));
    brushDates[cDateType::DT_BREAK].setColor(myConf.getColorFromString(myConf.get(ConfXML::AREA_PLANNER, "colBreak")));
    brushDates[cDateType::DT_OTHER].setColor(myConf.getColorFromString(myConf.get(ConfXML::AREA_PLANNER, "colOther")));
    brushDates[cDateType::DT_DRIVE].setColor(myConf.getColorFromString(myConf.get(ConfXML::AREA_PLANNER, "colDrive")));

    brushVisit.setColor(myConf.getColorFromString(myConf.get(ConfXML::AREA_PLANNER, "colVisit")));
    brushVisit.setStyle(Qt::SolidPattern);

    fontDate.setFamily("Arial");
    fontDate.setPixelSize(11);
    fontResHeader.setFamily("Arial");
    fontResHeader.setPixelSize(11);
    fontHeader.setFamily("Arial");
    fontHeader.setPixelSize(11);
    fontGrid.setFamily("Arial");
    fontGrid.setPixelSize(11);
}

CalendarConfig::~CalendarConfig()
{
    while (!resources.isEmpty()) delete resources.takeFirst();
}

QBrush CalendarConfig::getDateBrush(cDateItem& cdi)
{
    int d_type = cdi.getType();
    int state = cdi.getStateID();

    QBrush brush;
    if ( (d_type >= 0) && (d_type < cDateType::DT_COUNT) ) {
        if (d_type == cDateType::DT_THERAPY && (cdi.getOptions() & 1) ) brush = brushVisit;
        else brush = brushDates[d_type];
    } else brush = brushDates[0];

    switch(state) {
        case cDateState::DS_DONE:   // state: "Wahrgenommen"
            brush.setStyle(Qt::Dense1Pattern);
            break;
        case cDateState::DS_CANCEL: // state: "Ausgefallen"
        case cDateState::DS_REJECT: // state: "Abgesagt"
            brush.setColor(Qt::gray);
            brush.setStyle(Qt::DiagCrossPattern);
            break;
        default: break;
    }
    return brush;
}

void CalendarConfig::setRenderHours(int start, int end)
{
    if (hour_start > hour_end) hour_start = hour_end;
    else {
        hour_start = start;
        hour_end = end;
    }
}

int CalendarConfig::getNumResourcesToRender()
{
    return cur_resources_to_display;
}

int CalendarConfig::getNumResourcesAvailable()
{
    return resources.count();
}

void CalendarConfig::setNumResourcesToRender(int i)
{
    cur_resources_to_display = i;
    if (cur_resources_to_display >= resources.count()) cur_resources_to_display = resources.count();
    if (cur_resources_to_display < 0) cur_resources_to_display = 0;
    if (cur_resources_to_display > max_resources_to_display) cur_resources_to_display = max_resources_to_display;
}

QStringList CalendarConfig::getResourceNames()
{
    QStringList list;
    if (res_mode == RES_MODE_EMPLOYEES) {
        for (int i = 0; i < resources.count(); i++)
            list << (resources.at(i)->getLongName() + " (" + resources.at(i)->getShortName() + ")");
    } else {
        for (int i = 0; i < resources.count(); i++)
            list << resources.at(i)->getLongName();
    }
    return list;
}

QString CalendarConfig::getResourceShortName(int column)
{
    int x = mapResource.value(column); // get ID of ressource for column i
    for (int y = 0; y < resources.count(); y++) { // search for a resource that fits the ID
        if (resources.at(y)->getID() == x) return resources.at(y)->getShortName();
    }
    return "";
}

QString CalendarConfig::getResourceLongName(int column)
{
    int x = mapResource.value(column); // get ID of ressource for column i
    for (int y = 0; y < resources.count(); y++) { // search for a resource that fits the ID
        if (resources.at(y)->getID() == x) return resources.at(y)->getLongName();
    }
    return "";
}

int CalendarConfig::getResourceID(int column)
{
    return mapResource.value(column);
}

int CalendarConfig::getResMode()
{
    return res_mode;
}

void CalendarConfig::setResMode(int i)
{
    res_mode = i;
    while (!resources.isEmpty()) delete resources.takeFirst();
    mapResource.clear();

    if (i == RES_MODE_EMPLOYEES) {
        int co = 0;
        QList<cEmployee*> listEmpl;
        EmplTool.getActiveEmployees(listEmpl);
        for (int x = 0; x < listEmpl.count(); x++) {
            bool append = false;
            cEmployee* e = listEmpl.at(x);
            if (group == cEmployeeGroup::GROUP_ALL) append = true;
            else if (group == cEmployeeGroup::GROUP_THERAPY) {
                if ( (therapy_group == 0) && (e->getGroupID() == group) ) append = true;
                else if (therapy_group == e->getTherapyGroupID()) append = true;
            } else {
                if (group == e->getGroupID()) append = true;
            }
            if (append) {
                resources.append(new cResource(e->getID(), e->getShortName(), e->getFullName()));
                mapResource[co] = e->getID();
                co++;
            }
        }
        while (!listEmpl.isEmpty()) delete listEmpl.takeFirst();
    } else if (i == RES_MODE_ROOMS) {
        for (int x = 0; x < db.rooms.count(); x++) {
            cRoom* r = db.rooms.at(x);
            QString s = r->getBuilding();
            if (!s.isEmpty()) s = r->getName() + " (in " + s + ")";
            else s = r->getName();
            resources.append(new cResource(r->getID(), r->getName(), s));
            mapResource[x] = r->getID();
        }
    } else if (i == RES_MODE_CARS) {
        for (int x = 0; x < db.listCars.count(); x++) {
            cCar* c = db.listCars.at(x);
            resources.append(new cResource(c->getID(), c->getName(), c->getName()));
            mapResource[x] = c->getID();
        }
    }

    setNumResourcesToRender(resources.count());
}


//
// CalenderItem
//
CalendarItem::CalendarItem(CalendarConfig *cc, QGraphicsItem *parent)
        : QGraphicsItem(parent),
        config(cc)
{
}

void CalendarItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
}

QRectF CalendarItem::boundingRect() const
{
    return bRect;
}

void CalendarItem::setSize(int w, int h)
{
    bRect.setWidth(w);
    bRect.setHeight(h);
}

void CalendarItem::renderItem()
{
}


//
// CalendarDate
//
CalendarDate::CalendarDate(CalendarConfig *cc, cDateItem *c, QGraphicsItem *parent)
        : CalendarItem(cc, parent)
{
    setFlag(QGraphicsItem::ItemIsSelectable);
    dragging = false;
    ignore_move_event = false;
    if (c != NULL) cdi = *c;
    grid = config->grid;
}

void CalendarDate::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QPainterPath path;
    QRectF rectf = bRect;

    // perform clipping
    // Actually we don't need to clip since the scollContainer is drawn before the header items
    // but calculating the clip rect helps to center the text of date item the the center of the visible part
    // during the dragging process.
    // Besides if the calendar grid is smaller than the scrollContainer we need to clip date items to prevent
    // rendering outside the grid.

    if (!cdi.getFullDay()) {  // clipping is not performed for full day dates
        bool modified_by_clipping = false;
        int parent_y = parentItem()->pos().y();
        int parent_h = parentItem()->boundingRect().height();
        int pos_y = this->pos().y();
        int x = rectf.x();
        int y = rectf.y();
        int h = rectf.height();
        int w = rectf.width();

        if (pos_y + h > parent_h ) { // perform clipping if date item exceeds the lower boundary of the calendar grid
            int delta = parent_h - pos_y;
            if (delta > 0) rectf = QRectF(x, y, w + 1, delta);
            else rectf = QRectF(0,0,0,0);
            modified_by_clipping = true;
        } else if (pos_y < 0) { // clip when exceeding the upper boundary of the grid
            int delta = pos_y + h - parent_y;
            if (delta > 0) rectf = QRectF(x, h - delta, w + 1, delta);
            else rectf = QRectF(0,0,0,0);
            modified_by_clipping = true;
        }
        if (modified_by_clipping) {
            painter->setClipRect(rectf);
            if (rectf.width() > 0 && rectf.height() > 0) {
                rectf.setWidth(rectf.width() - 1);
                rectf.setHeight(rectf.height() - 1);
            }
        }
    }

    clipRect = rectf; // save current clip rect for other purposes (e.g. evaluating mouse double clicks)

    // finally, paint the date item
    painter->setPen(config->penFrame);
//    painter->setBrush(dragging ? config->brushHover : config->getDateBrush(cdi));
    if (dragging) {
        QColor c = config->getDateBrush(cdi).color();
        c.setAlpha(200);
        painter->setBrush(c);
    } else {
        painter->setBrush(config->getDateBrush(cdi));

    }
    painter->setFont(config->fontDate);
    painter->drawRect(rectf);
    if (dragging) {
        // if dragging is going on we paint the dragged date item with another color/alpha and
        // another string (time + short name of resource)
        QString s;
        int a;
        if (!cdi.getFullDay()) {
            QString s_time;
            QTime t = grid->getTimeForOffset(pos());
            if (!t.isValid()) {
                painter->setBrush(QColor(255, 0, 0, 170));
                painter->drawRect(rectf);
            } else s_time = t.toString("hh:mm");
            s = s_time + " (" + grid->getResourceForOffset(pos(), &a) + ")";
        } else {
            s = grid->getResourceForOffset(pos(), &a);
        }
        painter->drawText(rectf, Qt::AlignCenter | Qt::TextWordWrap, s);
    } else {
        if (!cdi.getFullDay()) {
            //painter->drawText(bRect, Qt::AlignCenter | Qt::TextWordWrap, cdi.getTitleAndTime());
            painter->drawText(bRect, Qt::AlignCenter | Qt::TextWordWrap, cdi.getTitle(config->render_activities, config->render_start_end));
        } else {
            painter->drawText(bRect, Qt::AlignCenter | Qt::TextWordWrap, cdi.getTitle(config->render_activities, false));
            //painter->drawText(bRect, Qt::AlignCenter | Qt::TextWordWrap, cdi.getTitle());
        }
    }

}

void CalendarDate::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    // Only call dateEdit() if we clicked on a visible part of the
    // item (some parts might be clipped ...)
    if (event->pos().y() <= clipRect.height())
        if (config->view != NULL) config->view->dateEdit(cdi);
}

void CalendarDate::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (ignore_move_event) {
        ignore_move_event = false;
        return;
    }

    if (QLineF(event->screenPos(), event->buttonDownScreenPos(Qt::LeftButton))
             .length() < QApplication::startDragDistance()) {
        dragging = false;
        return;
    }

    if (!dragging) {
        setZValue(1);    // ensure that the dragged item is on top of other items by setting the z-value
        dragging = true; // enable dragging mode
        //setCursor(Qt::BlankCursor);  // hide mouse cursor
    }

    int delta_x = event->pos().x() - event->lastPos().x();
    int delta_y = event->pos().y() - event->lastPos().y();
    int pos_y = this->pos().y();
    int pos_x = this->pos().x();
    int h = this->boundingRect().height();

    if (!cdi.getFullDay()) { // implementation for moving date items on the grid follows ...
        QGraphicsItem *grid = parentItem();
        int grid_y = grid->y();
        int grid_x = grid->x();
        int container_h = config->view->scrollContainer->boundingRect().height();
        int grid_h = grid->boundingRect().height();
        int grid_w = grid->boundingRect().width();
        int scroll_y = 12;
        int abs_delta_y = abs(delta_y);

        // boundary checking for x-position
        if (pos_x + delta_x < config->vertical_header_width) {
            setPos(config->vertical_header_width, pos_y);
            delta_x = 0;
        } else if (pos_x + delta_x > grid_w - 4) {
            setPos(grid_w -4, pos_y);
            delta_x = 0;
        }

        if ( (delta_y < 0) && (grid_y < 0) && (grid_y + pos_y < 2) ) { // need to scroll the grid up
            int new_pos_y = grid_y + scroll_y;
            if (new_pos_y > 0) new_pos_y = 0;
            grid->setPos(grid_x, new_pos_y);
            int new_delta_y = -scroll_y + abs_delta_y;
            // prevent that we move out of the upper grid boundary
            if (new_pos_y == 0 && config->hour_start == 0) setPos(pos_x + delta_x, abs_delta_y);
            else moveBy(delta_x, new_delta_y);
            ignore_move_event = true;
            abs_delta_y = (abs_delta_y > 1) ? (abs_delta_y * 2)  : 1;
            QCursor::setPos(QCursor::pos().x(), QCursor::pos().y() + abs_delta_y);
        } if ( (delta_y > 0) && (container_h - h < pos_y + grid_y + 3) && (pos_y + h < grid_h ) ) { // need to scroll the grid down
            int new_pos_y = grid_y - scroll_y;
            if (new_pos_y < (grid_h - container_h) * -1) {
                new_pos_y = (grid_h - container_h) * -1;
            }
            if (new_pos_y != grid_y) {
                grid->setPos(grid_x, new_pos_y);
                ignore_move_event = true;
                abs_delta_y = (abs_delta_y > 1) ? (abs_delta_y * 2)  : 1;
                QCursor::setPos(QCursor::pos().x(), QCursor::pos().y() - abs_delta_y);
            } else scroll_y = delta_y;
            if (pos_y + scroll_y < grid_h) moveBy(delta_x, scroll_y);
        } else { // do not scroll the grid but simply move the dragged date item
            // do not move if we reached the upper boundary of the grid (time 00:00)
            if ( delta_y < 0 && (pos_y + delta_y) <= 0 && config->hour_start == 0) delta_y = 0;
            moveBy(delta_x, delta_y);
        }
    } else {  // move full day date items is handled differently

        // boundary checking for x-position
        int w = this->boundingRect().width();
        int boundary_x;
        if (config->render_mode == CalendarConfig::RENDER_MODE_DAY)
            boundary_x = parentItem()->boundingRect().width() - config->scrollbar_width - w - 2;
        else boundary_x = parentItem()->boundingRect().width() - config->scrollbar_width - 2;
        if (pos_x + delta_x < config->vertical_header_width) {
            setPos(config->vertical_header_width, 2);
        } else if (pos_x + delta_x > boundary_x) {
            setPos(boundary_x, 2);
        } else moveBy(delta_x, 0);
    }
}

void CalendarDate::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
    if (dragging) {
        setCursor(Qt::ArrowCursor);
        dragging = false;
        setZValue(0);  // reset z-value to default
        CalendarView *view = config->view;
        if (view == NULL) return;

        int day_offset, column;
        grid->getResourceForOffset(pos(), &column);
        day_offset = grid->getDayOffsetForPos(pos());
        if (! (day_offset >= 0 && column >= 0) ) return;
        cDateItem cdi_tmp;
        cdi_tmp = cdi;
        cdi_tmp.setDate(view->getDateForHeader(day_offset));
        if (!cdi_tmp.getFullDay()) { // handle drop of date items on the calendar grid
            QTime t = grid->getTimeForOffset(pos());

            int secs = cdi_tmp.getStart().secsTo(cdi_tmp.getEnd());
            cdi_tmp.setStart(t);
            cdi_tmp.setEnd(t.addSecs(secs));

            // TODO: Ask user if the end time should be modified
            if (cdi_tmp.getStart() > cdi_tmp.getEnd()) cdi_tmp.setEnd(QTime(23, 59, 0, 0));
        }

        switch(config->res_mode) {
            case CalendarConfig::RES_MODE_EMPLOYEES: cdi_tmp.setEID(config->mapResource.value(column)); break;
            case CalendarConfig::RES_MODE_ROOMS: cdi_tmp.setRoomID(config->mapResource.value(column)); break;
            case CalendarConfig::RES_MODE_CARS: cdi_tmp.setCarID(config->mapResource.value(column)); break;
            default: break;
        }

        // check whether the new properties of the date item overlaps with existing date items
        if (view->detectAndAckOverlap(cdi_tmp)) {
            cdi = cdi_tmp;
            // Save the date item in the database.
            // Normally, the database will emit a signal after the update is done. This will
            // lead to an auto-reload of the calendar. But since we modified only a known date
            // it will be faster to avoid auto-reload of all date (of the day/week) and instead
            // just re-render the calendar.
            db.updateItem(cdi, false);
            setToolTip(getDateDescription()); // update the tool tip (set new start and end time)
        }
        config->view->renderCalendar();
    }
}

void CalendarDate::contextMenuEvent(QGraphicsSceneContextMenuEvent * event)
{
    // Only call dateEdit() if we clicked on a visible part of the
    // item (some parts might be clipped ...)
    if (event->pos().y() <= clipRect.height()) {
        config->view->contextMenu(cdi);
    }
}

QString CalendarDate::getDateDescription()
{
    QString s = "";

    if (cdi.getID() > 0) {
        if (!cdi.getFullDay()) s = cdi.getStart().toString("hh:mm") + " - " + cdi.getEnd().toString("hh:mm") + "\n";
        s += cdi.getTitle();
        int state = cdi.getStateID();
        if (state >= 0 && state < db.listDateStates.count())
            s += ("\n" + db.listDateStates.at(state)->getDescription());
    }
    return s;
}

//
// CalendarResHeaderItem
//
CalendarResHeaderItem::CalendarResHeaderItem(CalendarConfig *cc, int header_id, int column_id, QGraphicsItem *parent)
        : CalendarItem(cc, parent)
{
    //setAcceptsHoverEvents(true);

    id_header = header_id;
    id_column = column_id;
    title = "";
}

void CalendarResHeaderItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setPen(config->penFrame);
    painter->setFont(config->fontResHeader);

    if (bRect.width() < 100) {
        painter->drawText(bRect, Qt::AlignCenter | Qt::TextWordWrap, config->getResourceShortName(id_column));
    } else {
        painter->drawText(bRect, Qt::AlignCenter | Qt::TextWordWrap, config->getResourceLongName(id_column));
    }
    if(option->state & QStyle::State_MouseOver) {
        painter->setBrush(config->brushHoverRes);
        painter->drawRect(0, 0, (int)bRect.width(),(int)bRect.height());
    }
}

void CalendarResHeaderItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) return;
    if (config->view != NULL) config->view->startSelectResource(id_header, id_column);
}


//
// FullDayItem
//
FullDayHeaderItem::FullDayHeaderItem(CalendarConfig *cc, QGraphicsItem *parent)
    : CalendarItem(cc, parent)
{
}

void FullDayHeaderItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setPen(config->penFrame);
    painter->setBrush(config->brushGridDark);
    painter->drawRect(0, 0, (int)bRect.width(), (int)bRect.height());
}

void FullDayHeaderItem::contextMenuEvent(QGraphicsSceneContextMenuEvent * event)
{
    cDateItem cdi = config->grid->prepareNewDate(event->pos());
    cdi.setFullDay(true);
    cdi.setStart(QTime(0, 0, 0, 0));
    cdi.setEnd(QTime(0, 0, 0, 0));
    config->view->contextMenu(cdi);
}

void FullDayHeaderItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    // dummy function
    // required for receiving double click events (at least with Kubuntu 09.04)
}

void FullDayHeaderItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    cDateItem cdi = config->grid->prepareNewDate(event->pos());
    cdi.setFullDay(true);
    cdi.setStart(QTime(0, 0, 0, 0));
    cdi.setEnd(QTime(0, 0, 0, 0));
    config->calWidget->createNewDate(cdi);
}

void FullDayHeaderItem::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    // dummy function to avoid scrolling
}


//
// CalendarHeader
//
CalendarHeader::CalendarHeader(CalendarConfig *cc, QGraphicsItem *parent)
    : CalendarItem(cc, parent)
{
}

void CalendarHeader::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setPen(config->penFrame);
    painter->setBrush(config->headerGradient);
    painter->drawRect(0, 0, (int)bRect.width(), (int)bRect.height());
}

void CalendarHeader::renderItem()
{
    config->headerGradient.setFinalStop(0, bRect.height());
//    update();
}

void CalendarHeader::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    // dummy function to avoid scrolling
}


//
// CalenderHeaderItem
//
CalendarHeaderItem::CalendarHeaderItem(CalendarConfig *cc, int header_id, QGraphicsItem *parent)
        : CalendarItem(cc, parent)
{
    title = "";
    title_short = "";
    id = header_id;
    //setAcceptsHoverEvents(true);

    for (int i = 0; i < config->max_resources_to_display; i++) {
        res.append(new CalendarResHeaderItem(cc, header_id, i, this));
        res.at(i)->setVisible(false);
    }
}

CalendarHeaderItem::~CalendarHeaderItem()
{
    while (!res.isEmpty()) delete res.takeFirst();
}

void CalendarHeaderItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setPen(config->penFrame);
    painter->setBrush(config->headerGradient);
    painter->drawRect(0, 0, (int)bRect.width(), (int)bRect.height());

    painter->setFont(config->fontHeader);
    if (bRect.width() < 100) painter->drawText(fontRect, Qt::AlignHCenter | Qt::AlignTop, title_short);
    else painter->drawText(fontRect, Qt::AlignHCenter | Qt::AlignTop, title);

    // only show hover if we render a week (because hover indicates expandiblity of day columns of a week)
    if ( (option->state & QStyle::State_MouseOver) && (config->render_mode == CalendarConfig::RENDER_MODE_WEEK) ) {
        painter->setBrush(config->brushHover);
        painter->drawRect(0, 0, (int)bRect.width(),(int)bRect.height() / 2);
    }
}

void CalendarHeaderItem::setDate(QDate d)
{
    date = d;
    title = QLocale().dayName(d.dayOfWeek(), QLocale::LongFormat) + ", " + d.toString("dd.MM.");
    title_short = QLocale().dayName(d.dayOfWeek(), QLocale::ShortFormat); + ", " + d.toString("dd.MM.");
}

void CalendarHeaderItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
}

void CalendarHeaderItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
}

void CalendarHeaderItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
        event->ignore();
        return;
    }

    if ( (event->pos().y() < bRect.height() / 2 ) && (config->render_mode != CalendarConfig::RENDER_MODE_DAY)) {
        if (config->view != NULL) config->view->expandHeader(id, false);
    }
}

void CalendarHeaderItem::renderItem()
{
    for (int i = 0; i < config->getNumResourcesToRender(); i++)
        res.at(i)->setVisible(true);
    for (int i = config->getNumResourcesToRender(); i < config->max_resources_to_display; i++)
        res.at(i)->setVisible(false);

    fontRect = bRect;
    fontRect.setTop(fontRect.top() + 2);

    columnWidths.clear();
    int r_count = config->getNumResourcesToRender();
    if ( (r_count > 0) && (r_count <= res.count()) ) {
        int pos_x = 0;
        int pos_y = (int)bRect.height() / 2;
        int r_ofs = (int)bRect.width() / r_count;
        int r_mod = (int)bRect.width() % r_count;
        for (int i = 0; i < r_count; i++) {
            res.at(i)->setPos(pos_x, pos_y);
            if (i == 0) {
                res.at(i)->setSize(r_ofs + r_mod, pos_y);
                pos_x += (r_ofs + r_mod);
            } else {
                res.at(i)->setSize(r_ofs, pos_y);
                pos_x += r_ofs;
            }
            columnWidths << (pos_x + this->pos().x());
        }
    }

    update();
}

QList<int> CalendarHeaderItem::getColumnPositions()
{
    return columnWidths;
}

int CalendarHeaderItem::getColumnWidth(int col)
{
    if ( (col >= 0) && (col < res.count()) ) return res.at(col)->boundingRect().width();
    else return 0;
}

int CalendarHeaderItem::getColumnPosX(int col)
{
    if ( (col >= 0) && (col < res.count()) ) return res.at(col)->pos().x() + this->pos().x();
    else return -1;
}

QDate CalendarHeaderItem::getDate()
{
    return date;
}


//
// CalendarGrid
//
CalendarGrid::CalendarGrid(CalendarConfig *cc, QGraphicsItem *parent)
    : CalendarItem(cc, parent)
{
    verticalGrid.clear();
}

void CalendarGrid::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    int h = (int)bRect.height();
    int w = (int)bRect.width();
    int offset_y;

    //QPainterPath path;
    //painter->setClipRect(QRectF(this->pos().x(), -1 * this->pos().y(), parentItem()->boundingRect().width(), parentItem()->boundingRect().height()));

    painter->setBrush(config->brushGridDark);
    painter->drawRect(0, 0, config->vertical_header_width, h);

    painter->setPen(config->penGridLight);
    offset_y = config->hour_height / 2;
    for (int i = config->hour_start; i < config->hour_end; i++) {
        painter->drawLine(1, offset_y, w - 2, offset_y);
        offset_y += config->hour_height;
    }

    painter->setPen(config->penFrame);
    painter->setFont(config->fontGrid);
    painter->drawLine(config->vertical_header_width, 0, config->vertical_header_width, h);
    QString s;
    offset_y = 0;
    int co = config->hour_start;
    for (int i = config->hour_start; i < config->hour_end; i++) {
        s.asprintf("%02d:00", co++);
        painter->drawText(4, offset_y, config->vertical_header_width, config->hour_height, Qt::AlignLeft, s);
        painter->drawLine(0, offset_y, w, offset_y);
        offset_y += config->hour_height;
    }
    painter->drawLine(0, offset_y, w, offset_y);

    for (int i = 0; i < verticalGrid.count(); i++)
        painter->drawLine(verticalGrid.at(i), 0, verticalGrid.at(i), h - 1);
}

void CalendarGrid::setVerticalGrid(QList<int> vg)
{
    verticalGrid = vg;
}


int CalendarGrid::getTimeOffset(QTime t)
{
    int row_height = config->hour_height;
    int m = t.minute();
    int h = t.hour();
/*
    int t_mod = m % 5;
    if ( (t_mod > 0) && (t_mod < 3) ) m = m - t_mod;
    else if (t_mod > 0) m = m + 5 - t_mod;
    if (m >= 60) {
        h++;
        m -= 60;
    }
*/
    int ofs = (h - config->hour_start) * row_height;
    if (m > 0) ofs = ofs + (m / 5) * (row_height / 12);
    return ofs;
}

QTime CalendarGrid::getTimeForOffset(QPointF point)
{
    if (config->hour_height <= 0) return QTime(0,0,0,0);

    QTime t;
    int h = (point.y() + (config->hour_start * config->hour_height)) / config->hour_height;
    int m = (int)(point.y() + (config->hour_start * config->hour_height)) % config->hour_height;

    m = (m * 60) / config->hour_height;
    int t_mod = m % 5;
    if (t_mod >= 3) m = m + (5 - t_mod);
    else m = m - t_mod;

    if (m >= 60) {
        h++;
        m -= 60;
    }

    t.setHMS(h, m, 0, 0);
    return t;
}

QString CalendarGrid::getResourceForOffset(QPointF point, int *column)
{
    int x = point.x();
    if ( (x < config->vertical_header_width) || (config->getNumResourcesToRender() <= 0)) {
        *column = -1;
        return "";
    }

    int col = verticalGrid.count() / config->getNumResourcesToRender();
    int j = verticalGrid.count() - 1;
    if (j <= 0) return "";
    for (int i = j; i >= 0 ; i--) {
        if ( x < verticalGrid.at(i) ) {
            col = i;
        }
    }

    if (verticalGrid.count() > config->getNumResourcesToRender())
        col = (col - 1) % (config->getNumResourcesToRender() + 1); // resource column

    *column = col;
    return config->getResourceShortName(col);
}

int CalendarGrid::getDayOffsetForPos(QPointF pos)
{
    int day_offset;
    if (config->render_mode == CalendarConfig::RENDER_MODE_DAY) {
        day_offset = 0;
    } else {
        day_offset = -1;
        for (int i = config->numRenderDays - 1; i >= 0; i--) {
            int w = (int)config->view->headerItems.at(i)->boundingRect().width();
            int x = (int)config->view->headerItems.at(i)->pos().x();
            int p = (int)pos.x();
            if (p >= x && p <= x +w) {
                day_offset = i;
                break;
            }
        }
    }
    return day_offset;
}

cDateItem CalendarGrid::prepareNewDate(QPointF pos)
{
    cDateItem cdi;
    int col;

    // get time of the mouse position
    QTime t = getTimeForOffset(pos);
    // get the day offset for the mouse position
    int day_offset = getDayOffsetForPos(pos);
    // get column in order to match it to a resource
    getResourceForOffset(pos, &col);

    if (! (day_offset >= 0 && col >= 0) ) return cdi;
    cdi.setDate(config->view->getDateForHeader(day_offset));

    // get ID of resouce
    int id = config->getResourceID(col);
    if (config->res_mode == CalendarConfig::RES_MODE_EMPLOYEES) {
        cdi.setEID(id);
    } else if (config->res_mode == CalendarConfig::RES_MODE_ROOMS) {
        cdi.setRoomID(id);
    } else if (config->res_mode == CalendarConfig::RES_MODE_CARS) {
        cdi.setCarID(id);
    }

    cdi.setStart(t);
    int time_span = myConf.get(ConfXML::AREA_PLANNER, "work_interval").toInt();
    cdi.setEnd(t.addSecs(60 * time_span));
    return cdi;
}

void CalendarGrid::contextMenuEvent(QGraphicsSceneContextMenuEvent * event)
{
    if (event->pos().x() > config->vertical_header_width) {
        config->view->contextMenu(prepareNewDate(event->pos()));
    }
}

void CalendarGrid::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    // dummy function
    // required for receiving double click events (at least with Kubuntu 09.04)
}

void CalendarGrid::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->pos().x() < config->vertical_header_width) return;
    cDateItem cdi = prepareNewDate(event->pos());
    config->calWidget->createNewDate(cdi);
}


//
// CalendarScrollbar
//
CalendarScrollbar::CalendarScrollbar(CalendarConfig *cc, QGraphicsItem *parent)
    : CalendarItem(cc, parent)
{
    arrow_top = new QGraphicsPolygonItem(this);
    arrow_bottom = new QGraphicsPolygonItem(this);
    arrow_top->setPen(config->penScrollArrow);
    arrow_bottom->setPen(config->penScrollArrow);
}

CalendarScrollbar::~CalendarScrollbar()
{
    if (arrow_top != NULL) delete arrow_top;
    if (arrow_bottom != NULL) delete arrow_bottom;

}

void CalendarScrollbar::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setPen(config->penScrollbar);
    painter->setBrush(config->brushScrollbar);
    int w = (int)bRect.width();
    int h = (int)bRect.height();
    painter->drawRect(0, 0, w, h);
}

void CalendarScrollbar::renderItem()
{
    int w = (int)bRect.width();
    int h = (int)bRect.height();
    QPolygonF polygon;
    polygon << QPointF((qreal)(w / 2), 4.0) << QPointF(2.0, 18.0) << QPointF((qreal)(w) - 2.0, 18.0);
    arrow_top->setPolygon(polygon);
    polygon.clear();
    polygon << QPointF((qreal)(w / 2), (qreal)h - 4.0) << QPointF(2.0, (qreal)h - 18.0) << QPointF((qreal)(w) - 2.0, (qreal)h - 18.0);
    arrow_bottom->setPolygon(polygon);
    update();
}


//
// ScrollContainer
//
CalendarScrollContainer::CalendarScrollContainer(CalendarConfig *cc, QGraphicsItem *parent)
    : CalendarItem(cc, parent)
{
    scrollItem = NULL;
    scrollbar = NULL;
    scrollbar = new CalendarScrollbar(cc, this);
}

CalendarScrollContainer::~CalendarScrollContainer()
{
    if (scrollbar != NULL) delete scrollbar;
}

void CalendarScrollContainer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setPen(config->penFrame);
    painter->setBrush(config->brushScollContainer);
    painter->drawRect(0, 0, (int)bRect.width(), (int)bRect.height());
}

void CalendarScrollContainer::setScrollItem(CalendarItem *item)
{
    if (item != NULL) {
        scrollItem = item;
        scrollItem->setParentItem(this);
    }
}

void CalendarScrollContainer::renderItem()
{
    if (scrollItem == NULL) return;
    int h = this->boundingRect().height();
    int w = this->boundingRect().width();


    scrollbar->setSize(config->scrollbar_width - 1, h - 1);
    scrollbar->setPos(w - config->scrollbar_width - 1, 0);
    scrollbar->renderItem();

    scrollItem->setSize(w - config->scrollbar_width - 1, (config->hour_end - config->hour_start) * config->hour_height);

    // if grid is smaller then our scroll container then just set grid to pos(0,0)
    if (scrollItem->boundingRect().height() < h ) {
        scrollItem->setPos(0, 0);
        scrollItem->update();
        return;
    }

    int cur_y = scrollItem->pos().y();
    if (cur_y < (scrollItem->boundingRect().height() - h) * -1)
        cur_y = (scrollItem->boundingRect().height() - h) * -1;
    scrollItem->setPos(0, cur_y);
    scrollItem->update();
}

void CalendarScrollContainer::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    scroll(event->delta());
}

void CalendarScrollContainer::scroll(int delta)
{
    if (scrollItem == NULL)  return;
    int cur_y = scrollItem->pos().y();
    int new_pos_y;

    if (delta > 0) {
        if (cur_y < 0) {
            new_pos_y = cur_y + config->scroll_step;
            if (new_pos_y > 0) new_pos_y = 0;
            scrollItem->setPos(0, new_pos_y);
        }
    } else {
        if (cur_y > (scrollItem->boundingRect().height() - this->boundingRect().height()) * -1) {
            new_pos_y = cur_y - config->scroll_step;
            if (new_pos_y < (scrollItem->boundingRect().height() - this->boundingRect().height()) * -1)
                new_pos_y = (scrollItem->boundingRect().height() - this->boundingRect().height()) * -1;
            scrollItem->setPos(0, new_pos_y);
        }
    }
}


//
// CalendarView
//
CalendarView::CalendarView(CalendarConfig *cc, QWidget *parent)
    : QGraphicsView(new QGraphicsScene(), parent)
{
    config = cc;
    config->view = this;

    setFrameShape(QFrame::Box);
    setFrameShadow(QFrame::Plain);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    scrollContainer = new CalendarScrollContainer(config);
    scene()->addItem(scrollContainer);

    header = new CalendarHeader(config);
    header->setPos(0, 0);

    // Workaround: The size has to be set in order to avoid distoration of the header when
    //             the calendar grid scrolls. This workaround seems to be necessary for Qt < 4.6
    header->setSize(1, 1);
    scene()->addItem(header);

    fullDayHeaderItem = new FullDayHeaderItem(config);
    scene()->addItem(fullDayHeaderItem);

    for (int i = 0; i < MAX_RENDER_DAYS; i++) {
        headerItems.append(new CalendarHeaderItem(config, i, header));
    }
    expandedHeader = -1;

     //scrollContainer->setFlag(QGraphicsItem::ItemClipsChildrenToShape, true);
    calGrid = new CalendarGrid(config);
    config->grid = calGrid;
    scrollContainer->setScrollItem(calGrid);
    calGrid->setPos(0, 0);

    updateComboBox();

    // insert combobox at the end and set Z-value in order to ensure visibility
    ptrCombo = scene()->addWidget(&combo);
    ptrCombo->setPos(-100, -100);
    ptrCombo->setZValue(1);
}


CalendarView::~CalendarView()
{
    while (!calDates.isEmpty()) delete calDates.takeFirst();
    while (!listDates.isEmpty()) delete listDates.takeFirst();
    while (!headerItems.isEmpty()) delete headerItems.takeFirst();
    if (calGrid != NULL) delete calGrid;
    if (fullDayHeaderItem != NULL) delete fullDayHeaderItem;
    if (header != NULL) delete header;
    if (scrollContainer != NULL) delete scrollContainer;
}


void CalendarView::resizeEvent(QResizeEvent * event)
{
    renderCalendar();
}


void CalendarView::keyPressEvent(QKeyEvent * event)
{
    switch (event->key()) {
    case Qt::Key_Left:
        break;
    case Qt::Key_Right:
        break;
     case Qt::Key_Down:
        scrollContainer->scroll(-1);
        break;
    case Qt::Key_PageDown:
       scrollContainer->scroll(-1);
       break;
     case Qt::Key_Up:
        scrollContainer->scroll(1);
        break;
    case Qt::Key_PageUp:
       scrollContainer->scroll(1);
       break;
     default:
        QGraphicsView::keyPressEvent(event);
     }
}


bool CalendarView::detectAndAckOverlap(cDateItem& cdi)
{
    DateTools dt;
    return dt.OverlapCheck(cdi, listDates);

}


void CalendarView::buildDates()
{
    while (!calDates.isEmpty()) delete calDates.takeFirst();
    while (!listDates.isEmpty()) delete listDates.takeFirst();

    // load dates from database
    if (config->render_mode == CalendarConfig::RENDER_MODE_DAY) {
        db.loadDates(listDates, curDate, curDate);
    } else if (config->render_mode == CalendarConfig::RENDER_MODE_WEEK) {
        int day_of_week = curDate.dayOfWeek();
        QDate from = curDate.addDays(1 - day_of_week);
        QDate until = curDate.addDays(7 - day_of_week);
        db.loadDates(listDates, from, until);
    }

    // create calendar date items from the dates retrieved from the database
    for (int i = 0; i < listDates.count(); i++) {
        //if (config->render_activities) listDates.at(i)->decodeActivities();
        if (listDates.at(i)->getFullDay()) calDates.append(new CalendarDate(config, listDates.at(i), fullDayHeaderItem));
        else calDates.append(new CalendarDate(config, listDates.at(i), calGrid));
        calDates.last()->setToolTip(calDates.last()->getDateDescription());
    }

}


void CalendarView::renderCalendar()
{
    int w = this->width();
    int h = this->height();

    setSceneRect(0, 0, w, h);

    // render base header
    header->setSize(w, config->horizontal_header_height);
    header->renderItem();

    fullDayHeaderItem->setPos(0, config->horizontal_header_height);
    fullDayHeaderItem->setSize(w, config->fulldayheader_height);

    QList<int> verticalGrid; // store positions for vertical grid lines to save calculation effort
    int ver_header_w = config->vertical_header_width;
    int hor_header_h = config->horizontal_header_height;
    int date_render_area = w - ver_header_w - config->scrollbar_width;

    // render header items (for each single day a header item has to be rendered)
    if (config->render_mode == CalendarConfig::RENDER_MODE_DAY) {
        headerItems.at(0)->setSize(date_render_area - 1, hor_header_h);
        headerItems.at(0)->setPos(ver_header_w, 0);
        headerItems.at(0)->renderItem();
        verticalGrid.append(headerItems.at(0)->getColumnPositions());
    } else if (config->render_mode == CalendarConfig::RENDER_MODE_WEEK) {
        int day_width, day_expanded_width, mod_day, tempExpandedHeader;

        if (config->numRenderDays <= 1) return; // avoid devision by zero a few lines later

        if (expandedHeader >= 0) { // one of the header items is expanded
            day_expanded_width = (date_render_area * 2) / 3;
            day_width = (date_render_area - day_expanded_width) / (config->numRenderDays - 1);
            mod_day = (date_render_area - day_expanded_width) % (config->numRenderDays - 1);
            day_expanded_width += mod_day;
            tempExpandedHeader = expandedHeader;
        } else { // every header item has the same width
            day_width = date_render_area / config->numRenderDays;
            mod_day = date_render_area % config->numRenderDays;
            day_expanded_width = day_width + mod_day;
            tempExpandedHeader = 0;
        }

        int day_offset = ver_header_w;
        for (int i = 0; i < config->numRenderDays; i++) {
            verticalGrid << day_offset;
            int dw = (i == tempExpandedHeader) ? day_expanded_width : day_width;
            dw = (i == config->numRenderDays - 1) ? dw -1 : dw;
            headerItems.at(i)->setSize(dw, hor_header_h);
            headerItems.at(i)->setPos(day_offset, 0);
            day_offset += dw;
            headerItems.at(i)->renderItem();
            verticalGrid.append(headerItems.at(i)->getColumnPositions());
        }
    }

    scrollContainer->setPos(0, config->horizontal_overall_header_height);
    scrollContainer->setSize(w, h - config->horizontal_overall_header_height - 1);
    calGrid->setVerticalGrid(verticalGrid);
    scrollContainer->renderItem();

    // calculate position and size of each date
    for (int i = 0; i < calDates.count(); i++) {
        calDates.at(i)->setVisible(false);
        cDateItem cdi = calDates.at(i)->cdi;

        int compare_id = -1;
        switch(config->res_mode) {
            case CalendarConfig::RES_MODE_EMPLOYEES: compare_id = cdi.getEID(); break;
            case CalendarConfig::RES_MODE_ROOMS: compare_id = cdi.getRoomID(); break;
            case CalendarConfig::RES_MODE_CARS: compare_id = cdi.getCarID(); break;
            default: break;
        }
        if (compare_id < 0) continue;

        // find the columns that belongs to the resource ID (employee / room)
        int col_id = config->mapResource.values().indexOf(compare_id);
        if ( (col_id < 0) || (col_id >= config->cur_resources_to_display) ) continue;

        if (config->render_mode == CalendarConfig::RENDER_MODE_DAY) {
            if (cdi.getDate() != curDate) continue;
            int col_width = headerItems.at(0)->getColumnWidth(col_id) - 4;
            int pos_x = headerItems.at(0)->getColumnPosX(col_id)+ 2;
            int pos_y = 2; // default for full day
            if (cdi.getFullDay()) { // full day dates a rendered separately on the fullDayHeaderItem
                calDates.at(i)->setSize(col_width, config->fulldayheader_height - 4);
            } else { // other dates are rendered on the calendarGrid
                pos_y = calGrid->getTimeOffset(cdi.getStart());
                calDates.at(i)->setSize(col_width, calGrid->getTimeOffset(cdi.getEnd()) - pos_y);
            }
            calDates.at(i)->setPos(pos_x, pos_y);
        } else {
            int day_offset = -1;
            for (int x = 0; x < headerItems.count(); x++) {
                if (headerItems.at(x)->getDate() == cdi.getDate() && (headerItems.at(x)->isVisible()) ) {
                    day_offset = x;
                    break;
                }
            }
            if (day_offset == -1) continue;
            int col_width = headerItems.at(day_offset)->getColumnWidth(col_id) - 4;
            int pos_x = headerItems.at(day_offset)->getColumnPosX(col_id)+ 2;
            int pos_y = 2; // default for full day
            if (cdi.getFullDay()) { // full day dates a rendered separately on the fullDayHeaderItem
                calDates.at(i)->setSize(col_width, config->fulldayheader_height - 4);
            } else {
                pos_y = calGrid->getTimeOffset(cdi.getStart());
                calDates.at(i)->setSize(col_width, calGrid->getTimeOffset(cdi.getEnd()) - pos_y);
            }
            calDates.at(i)->setPos(pos_x, pos_y);
        }
        calDates.at(i)->setVisible(true);
    }

}


QDate CalendarView::getDateForHeader(int header_id)
{
    if (header_id >= 0 && (header_id < headerItems.count())) return headerItems.at(header_id)->getDate();
    else return QDate::currentDate();
}


void CalendarView::dateEdit(cDateItem& cdi)
{
    GUARD(Protect::FUNC_PLAN);
    DlgDate dlg;
    dlg.init(cdi);
    dlg.exec();
}


void CalendarView::dateUpdateStatus(cDateItem& cdi)
{
    int id = cdi.getID();
    if (id < 0) return;
    // Do not emit a signal after the update since this would cause a reload
    // of the date items. Instead we're looking for the one item which has to
    // be updated.
    db.updateItem(cdi, false);
    for (int i = 0; i < calDates.count(); i++) {
        if (calDates.at(i)->cdi.getID() == id) {
            CalendarDate *d = calDates.at(i);
            // Just update the state ID since this function is only called
            // when the state was changed via the context menu
            d->cdi.setStateID(cdi.getStateID());
            d->setToolTip(d->getDateDescription());
            d->update();
            break;
        }
    }
}


void CalendarView::dateDel(cDateItem& cdi)
{
    if (cdi.getID() < 0) return;
    MsgBox box;
    if (cdi.getMetaID() > 0) {
        QMessageBox box;
        box.setText(QString::fromUtf8("Der Termin gehört zu einer Terminserie!"));
        box.setWindowTitle("OpenPhysio - Nachfrage");
        box.setIcon(QMessageBox::Question);
        box.setWindowIcon(QIcon(":/favicon.png"));
        QPushButton *cancelButton = box.addButton(QString::fromUtf8("Abbrechen"), QMessageBox::NoRole);
        QPushButton *singleButton = box.addButton(QString::fromUtf8("Einzeltermin löschen"), QMessageBox::NoRole);
        QPushButton *seriesButton = box.addButton(QString::fromUtf8("Terminserie löschen"), QMessageBox::NoRole);
        box.setDefaultButton(cancelButton);
        box.exec();
        if (box.clickedButton() == singleButton) {
            // FIXME: if this is the last member of a meta date => delete meta date, too
            db.deleteItem(myDB::TABLE_DATES, cdi.getID());
        } else if (box.clickedButton() == seriesButton) {
            db.delMetaDates(cdi.getMetaID());
        }
    } else {
        if (box.execYesNoBox(QString::fromUtf8("Termin löschen?"), MsgBox::BTN_YES) == MsgBox::BTN_YES) {
            db.deleteItem(myDB::TABLE_DATES, cdi.getID());
        }
    }
}


void CalendarView::setRenderMode(int m)
{
    config->render_mode = m;
    if (config->render_mode == CalendarConfig::RENDER_MODE_DAY) {
        headerItems.at(0)->setVisible(true);
        for (int i = 1; i < headerItems.count(); i++) headerItems.at(i)->setVisible(false);
    } else if (config->render_mode == CalendarConfig::RENDER_MODE_WEEK) {
        for (int i = 0; i < config->numRenderDays; i++) headerItems.at(i)->setVisible(true);
        for (int i = config->numRenderDays; i < headerItems.count(); i++) headerItems.at(i)->setVisible(false);
    }
}


void CalendarView::setDate(QDate d)
{
    curDate = d;
    if (config->render_mode == CalendarConfig::RENDER_MODE_DAY) {
        headerItems.at(0)->setDate(curDate);
    } else if (config->render_mode == CalendarConfig::RENDER_MODE_WEEK) {
        QDate week_start = curDate.addDays(1 - (curDate.dayOfWeek()));
        for (int i = 0; i < MAX_RENDER_DAYS; i++) headerItems.at(i)->setDate(week_start.addDays(i));
    }
}


QDate CalendarView::getDate()
{
    return curDate;
}


void CalendarView::setRenderDaysForWeek(int i)
{
    if (i >= MAX_RENDER_DAYS) config->numRenderDays = MAX_RENDER_DAYS;
    else if (i < 0) {
        config->numRenderDays = 1;
        setRenderMode(CalendarConfig::RENDER_MODE_DAY);
    } else config->numRenderDays = i;
    if (config->render_mode == CalendarConfig::RENDER_MODE_DAY) {
        headerItems.at(0)->setVisible(true);
        for (int i = 1; i < headerItems.count(); i++) headerItems.at(i)->setVisible(false);
    } else if (config->render_mode == CalendarConfig::RENDER_MODE_WEEK) {
        for (int i = 0; i < config->numRenderDays; i++) headerItems.at(i)->setVisible(true);
        for (int i = config->numRenderDays; i < headerItems.count(); i++) headerItems.at(i)->setVisible(false);
    }

    // a hidden header should not be expanded
    if (expandedHeader >= config->numRenderDays) expandedHeader = -1;
}


void CalendarView::expandHeader(int id, bool no_inverse)
{
    // mark a certain header as expanded
    if ( (id < 0) || (id >= headerItems.count()) ) {
        expandedHeader = -1;
        return;
    }
    if (no_inverse) expandedHeader = id;
    else expandedHeader = (id == expandedHeader) ? -1 : id;
    renderCalendar();
}


void CalendarView::startSelectResource(int header_id, int column_id)
{
    //qDebug() << __FUNCTION__ << header_id << column_id;
    if ( (header_id < 0) || (header_id >= headerItems.count()) ) return;
    if (ptrCombo != NULL) {
        int pos_x = headerItems.at(header_id)->getColumnPosX(column_id);
        int pos_y = headerItems.at(header_id)->y() + headerItems.at(header_id)->boundingRect().height() / 2;
        int w = headerItems.at(header_id)->getColumnWidth(column_id);
        if (w < combo.minimumSizeHint().width()) w = combo.minimumSizeHint().width();
        int h = headerItems.at(header_id)->boundingRect().height() / 2;
        if (h < combo.minimumSizeHint().height()) h = combo.minimumSizeHint().height();
        // avoid to move parts of the combobox out of the scrollContainer area
        if (pos_x + w > this->width()) pos_x -= (w - headerItems.at(header_id)->getColumnWidth(column_id));
        combo.setGeometry(pos_x, pos_y, w, h);
        ptrCombo->setVisible(true);
        ptrCombo->setData(0, column_id); // save the column; will be evaluated in slotResourceSelected
    }
}


void CalendarView::slotResourceSelected(int i)
{
    if ( (i < 0) || (i >= config->resources.count()) ) return;
    disconnect(&combo, &QComboBox::currentIndexChanged, this, &CalendarView::slotResourceSelected);

    // i = just the combo index of resource => but we need to know the actual ID of the column
    int id = config->resources.at(i)->getID();
    int column = ptrCombo->data(0).toInt();

    if (config->mapResource.value(column) != id) { // resource isn't already mapped to that column?
        // check if resource (id) is already registered for other columns
        // if so, remove the registration or exchange resource columns respectively
        int old_column = config->mapResource.values().indexOf(id);
        int old_resource = config->mapResource.value(column);
        if (old_column != -1) {
            //qDebug() << "mapping resource ID" << old_resource << "to column" << old_column;
            config->mapResource.insert(old_column, old_resource);
        }
        //qDebug() << "mapping resource ID" << id << "to column" << column;
        config->mapResource.insert(column, id);
    }

    combo.setVisible(false);
    combo.setCurrentIndex(-1);
    connect(&combo, &QComboBox::currentIndexChanged, this, &CalendarView::slotResourceSelected);
    renderCalendar();
}


void CalendarView::contextMenu(cDateItem cdi)
{
    QMenu menu;

    QAction actDateEdit(QIcon(":/edit.png"), QString::fromUtf8("Bearbeiten .."), this);
    QAction actDateDel(QIcon(":/cancel.png"), QString::fromUtf8("Löschen"), this);
    QAction actDateNew(QIcon(":/termine.png"), QString::fromUtf8("Neuer Termin .."), this);
    QAction actDayBefore(QIcon(":/leftarrow.png"), QString::fromUtf8("Tag zurück"), this);
    QAction actDayAfter(QIcon(":/rightarrow.png"), QString::fromUtf8("Tag vor"), this);
    QAction actWeek(QIcon(":/week.png"), QString::fromUtf8("Wochenansicht"), this);
    QAction actWeekBefore(QIcon(":/leftarrow.png"), QString::fromUtf8("Woche zurück"), this);
    QAction actWeekAfter(QIcon(":/rightarrow.png"), QString::fromUtf8("Woche vor"), this);
    QAction actDay(QIcon(":/day.png"), QString::fromUtf8("Tagesansicht"), this);
    QAction actDateMove(QIcon(":/date_move.png"), QString::fromUtf8("Termin verschieben .."), this);
    QAction actDateFollowUp(QIcon(":/date_followup.png"), QString::fromUtf8("Folgetermin .."), this);
    QAction actEditPatient(QIcon(":/edit_user.png"), QString::fromUtf8("Patientendaten .."), this);
    QAction actEditReceipe(QIcon(":/rezept.png"), QString::fromUtf8("Rezeptdaten .."), this);

    QMenu menu_state("Status");
    QList<QAction*> list_act_states;
    for (int i = 0; i < db.listDateStates.count(); i++) {
        list_act_states.append(new QAction(db.listDateStates.at(i)->getDescription(), this));
        list_act_states.last()->setData(CMC_STATE + i);
        list_act_states.last()->setCheckable(true);
    }
    menu_state.addActions(list_act_states);
    actDateEdit.setData(CMC_EDIT);
    actDateDel.setData(CMC_DEL);
    actDateNew.setData(CMC_NEW);
    actDayBefore.setData(CMC_PREV_DAY);
    actDayAfter.setData(CMC_NEXT_DAY);
    actWeek.setData(CMC_VIEW);
    actWeekBefore.setData(CMC_PREV_WEEK);
    actWeekAfter.setData(CMC_NEXT_WEEK);
    actDay.setData(CMC_VIEW);
    actDateMove.setData(CMC_MOVE);
    actDateFollowUp.setData(CMC_FOLLOWUP);
    actEditPatient.setData(CMC_EDIT_PATIENT);
    actEditReceipe.setData(CMC_EDIT_RECEIPE);

    if (cdi.getID() > 0) {  // show context menu for a certain date
        int state = cdi.getStateID();
        if (state >= 0 && state < list_act_states.count())
            list_act_states.at(state)->setChecked(true);
        menu.addAction(&actDateEdit);
        menu.addAction(&actDateMove);
        menu.addAction(&actDateFollowUp);
        menu.addAction(&actDateDel);
        menu.addMenu(&menu_state);
        menu.addSeparator();
    }
    menu.addAction(&actDateNew);

    if (cdi.getPID() > 0) {
        menu.addAction(&actEditPatient);
    }
    if (cdi.getRID() > 0) {
        menu.addAction(&actEditReceipe);
    }

    menu.addSeparator();

    if (config->render_mode == CalendarConfig::RENDER_MODE_DAY) {
        menu.addAction(&actDayBefore);
        menu.addAction(&actDayAfter);
        menu.addSeparator();
        menu.addAction(&actWeek);
    } else if (config->render_mode == CalendarConfig::RENDER_MODE_WEEK) {
        menu.addAction(&actWeekBefore);
        menu.addAction(&actWeekAfter);
        menu.addSeparator();
        menu.addAction(&actDay);
    }

    QAction *selAct = menu.exec(QCursor::pos());
    if (selAct == NULL) return;
    switch (selAct->data().toInt()) {
    case CMC_EDIT:
        if (cdi.getID() > 0) dateEdit(cdi);
        break;
    case CMC_NEW:
        // if the user clicked on an existing date but then chooses to create a new
        // date we need to create a new cdi with 5-minutes interval (done by calling slotDateNew())
        if (cdi.getID() > 0) config->calWidget->slotDateNew();
        else config->calWidget->createNewDate(cdi);
        break;
    case CMC_DEL:
        if (cdi.getID() > 0) dateDel(cdi);
        break;
    case CMC_MOVE:
        if (cdi.getID() > 0) config->calWidget->MoveDate(cdi);
        break;
    case CMC_FOLLOWUP:
        if (cdi.getID() > 0) config->calWidget->FollowUpDate(cdi);
        break;
    case CMC_NEXT_DAY:
        config->dock->calendarWidget->setSelectedDate(curDate.addDays(1));
        break;
    case CMC_PREV_DAY:
        config->dock->calendarWidget->setSelectedDate(curDate.addDays(-1));
        break;
    case CMC_NEXT_WEEK:
        config->dock->calendarWidget->setSelectedDate(curDate.addDays(7));
        break;
    case CMC_PREV_WEEK:
        config->dock->calendarWidget->setSelectedDate(curDate.addDays(-7));
        break;
    case CMC_EDIT_PATIENT:
        config->calWidget->editPatient(cdi);
        break;
    case CMC_EDIT_RECEIPE:
        config->calWidget->editReceipe(cdi);
        break;
    case CMC_VIEW:
        if (config->render_mode == CalendarConfig::RENDER_MODE_DAY)
            config->dock->cbView->setCurrentIndex(1);
        else if (config->render_mode == CalendarConfig::RENDER_MODE_WEEK)
            config->dock->cbView->setCurrentIndex(0);
        break;
    case (CMC_STATE + cDateState::DS_TERM):
    case (CMC_STATE + cDateState::DS_DONE):
    case (CMC_STATE + cDateState::DS_CANCEL):
    case (CMC_STATE + cDateState::DS_REJECT):
        cdi.setStateID(selAct->data().toInt() - CMC_STATE);
        dateUpdateStatus(cdi);
        break;
    default: break;
    }

}


void CalendarView::updateComboBox()
{
    disconnect(&combo, &QComboBox::currentIndexChanged, this, &CalendarView::slotResourceSelected);
    combo.setVisible(false);
    combo.clear();
    combo.addItems(config->getResourceNames());
    combo.setCurrentIndex(-1);
    connect(&combo, &QComboBox::currentIndexChanged, this, &CalendarView::slotResourceSelected);
}


//
// CalendarWidget
//
CalendarDockWidget::CalendarDockWidget(QWidget *parent) :
    QDockWidget(parent)
{
    setWindowTitle("Terminplan-Optionen");
    content = new QWidget();
    setWidget(content);

    boxLayout = new QBoxLayout(direction, content);
    //boxLayout->setMargin(0);
    boxLayout->setContentsMargins(0, 0, 0, 0);
    boxLayout->setSpacing(0);

    QFont font;
    font.setBold(true);
    font.setWeight(QFont::Bold); //   setWeight(75);

    // setup part 1
    part1 = new QWidget(content);
    QGridLayout *grid1 = new QGridLayout(part1);
    //grid1->setMargin(2);
    grid1->setContentsMargins(2, 0, 2, 0);
    grid1->setSpacing(0);
    boxLayout->addWidget(part1);
    calendarWidget = new QCalendarWidget(part1);
    calendarWidget->setFirstDayOfWeek(Qt::Monday);
    calendarWidget->setGridVisible(true);
    calendarWidget->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    grid1->addWidget(calendarWidget, 0, 0, 1, 1);

    // separator
    line_1 = new QFrame(content);
    line_1->setFrameShadow(QFrame::Sunken);
    boxLayout->addWidget(line_1);

    // setup part 2
    part2 = new QWidget(content);
    QGridLayout *grid2 = new QGridLayout(part2);
    grid2->setContentsMargins(2, 2, 2, 2);
    //grid2->setContentsMargins(0, 0, 0, 0);
    //grid2->setSpacing(4);
    grid2->setHorizontalSpacing(2);
    grid2->setVerticalSpacing(6);
    boxLayout->addWidget(part2);
    QLabel * label_1 = new QLabel(part2);
    label_1->setText("Grundeinstellungen");
    label_1->setFont(font);
    grid2->addWidget(label_1, 0, 0, 1, 3);
    cbView = new QComboBox(part2);
    cbView->addItem("Tagesansicht");
    cbView->addItem("Wochenansicht");
    grid2->addWidget(cbView, 1, 0, 1, 3);
    spFrom = new QSpinBox(part2);
    spFrom->setMinimum(0);
    spFrom->setMaximum(23);
    spFrom->setSuffix(" Uhr");
    grid2->addWidget(spFrom, 2, 0, 1, 1);
    QLabel * label_2 = new QLabel(part2);
    label_2->setText("bis");
    label_2->setAlignment(Qt::AlignCenter);
    grid2->addWidget(label_2, 2, 1, 1, 1);
    spTo = new QSpinBox(part2);
    spTo->setMinimum(1);
    spTo->setMaximum(24);
    spTo->setSuffix(" Uhr");
    grid2->addWidget(spTo, 2, 2, 1, 1);
    spWeekDays = new QSpinBox(part2);
    spWeekDays->setMinimum(2);
    spWeekDays->setMaximum(7);
    spWeekDays->setValue(7);
    grid2->addWidget(spWeekDays, 3, 0, 1, 1);
    QLabel *label_3 = new QLabel(part2);
    label_3->setText("Wochenarbeitstage");
    grid2->addWidget(label_3, 3, 1, 1, 2);
    QLabel *label_4 = new QLabel(part2);
    label_4->setText(QString::fromUtf8("Zeilenhöhe"));
    grid2->addWidget(label_4, 4, 0, 1, 1);
    pbMinus = new QPushButton(part2);
    pbMinus->setText("");
    pbMinus->setMaximumHeight(22);
    pbMinus->setMaximumWidth(22);
    pbMinus->setIcon(QIcon(QString::fromUtf8(":/minus.png")));
    grid2->addWidget(pbMinus, 4, 1, 1, 1);
    pbPlus = new QPushButton(part2);
    pbPlus->setText("");
    pbPlus->setMaximumHeight(22);
    pbPlus->setMaximumWidth(22);
    pbPlus->setIcon(QIcon(QString::fromUtf8(":/plus.png")));
    grid2->addWidget(pbPlus, 4, 2, 1, 1);
    QSpacerItem *spacer_1 = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);
    grid2->addItem(spacer_1, 5, 0, 1, 1);

    // separator
    line_2 = new QFrame(content);
    line_2->setFrameShadow(QFrame::Sunken);
    boxLayout->addWidget(line_2);

    // setup part 3
    part3 = new QWidget(content);
    QGridLayout *grid3 = new QGridLayout(part3);
    //grid3->setMargin(2);
    grid3->setContentsMargins(2, 2, 2, 2);
    //grid3->setSpacing(2);
    grid3->setHorizontalSpacing(2);
    grid3->setVerticalSpacing(6);
    boxLayout->addWidget(part3);
    QLabel * label_5 = new QLabel(part3);
    label_5->setText("Ressourcen");
    label_5->setFont(font);
    grid3->addWidget(label_5, 0, 0, 1, 4);
    cbResMode = new QComboBox(part3);
    cbResMode->addItem(QString::fromUtf8("Mitarbeiter"));
    cbResMode->addItem(QString::fromUtf8("Räume"));
    cbResMode->addItem(QString::fromUtf8("Firmenwagen"));
    grid3->addWidget(cbResMode, 1, 0, 1, 4);
    QLabel * label_6 = new QLabel(part3);
    label_6->setText("Gruppe");
    grid3->addWidget(label_6, 2, 0, 1, 1);
    cbGroup = new QComboBox(part3);
    grid3->addWidget(cbGroup, 2, 1, 1, 3);
    QLabel * label_7 = new QLabel(part3);
    label_7->setText("Therapeutengruppe");
    grid3->addWidget(label_7, 3, 0, 1, 1);
    cbTherapyGroup = new QComboBox(part3);
    grid3->addWidget(cbTherapyGroup, 3, 1, 1, 3);
    QLabel * label_8 = new QLabel(part3);
    label_8->setText("Parallel anzeigen");
    grid3->addWidget(label_8, 4, 0, 1, 2);
    spMaxRes = new QSpinBox(part3);
    spMaxRes->setMaximum(99);
    grid3->addWidget(spMaxRes, 4, 2, 1, 2);

    QSpacerItem *spacer_2 = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);
    grid3->addItem(spacer_2, 5, 0, 1, 1);

    // separator
    line_3 = new QFrame(content);
    line_3->setFrameShadow(QFrame::Sunken);
    boxLayout->addWidget(line_3);

    // setup part 4
    part4 = new QWidget(content);
    QGridLayout *grid4 = new QGridLayout(part4);
    //grid4->setMargin(2);
    //grid4->setSpacing(4);
    grid4->setContentsMargins(2, 4, 2, 4);
    grid4->setHorizontalSpacing(2);
    grid4->setVerticalSpacing(6);
    boxLayout->addWidget(part4);
    pbDateNew = new QPushButton(part4);
    pbDateNew->setText("Neuer &Termin");
    pbDateNew->setIcon(QIcon(QString::fromUtf8(":/termine.png")));
    pbDateNew->setIconSize(QSize(24, 24));
    grid4->addWidget(pbDateNew, 0, 0, 1, 1);
    /*pbDateSearch = new QPushButton(part4);
    pbDateSearch->setText("Termin&suche");
    pbDateSearch->setIcon(QIcon(QString::fromUtf8(":/edit_receipe.png")));
    pbDateSearch->setIconSize(QSize(24, 24));
    grid4->addWidget(pbDateSearch, 1, 0, 1, 1);*/
    pbDatePrint = new QPushButton(part4);
    pbDatePrint->setText("Termin&plan drucken");
    pbDatePrint->setIcon(QIcon(QString::fromUtf8(":/printer.png")));
    pbDatePrint->setIconSize(QSize(24, 24));
    grid4->addWidget(pbDatePrint, 1, 0, 1, 1);

    QWidget *widgetButtons = new QWidget(part4);
    QGridLayout *gridButtons = new QGridLayout(widgetButtons);
    gridButtons->setContentsMargins(0, 0, 0, 0);
    gridButtons->setSpacing(2);
    grid4->addWidget(widgetButtons, 2, 0, 1, 1);
    pbEmployees = new QPushButton(widgetButtons);
    pbEmployees->setText("");
    pbEmployees->setToolTip("Mitarbeiter verwalten");
    pbEmployees->setMaximumSize(QSize(28, 28));
    pbEmployees->setMinimumSize(QSize(28, 28));
    pbEmployees->setIcon(QIcon(QString::fromUtf8(":/patient.png")));
    pbEmployees->setIconSize(QSize(24, 24));
    gridButtons->addWidget(pbEmployees, 0, 0, 1, 1);
    pbTherapyGroups = new QPushButton(widgetButtons);
    pbTherapyGroups->setText("");
    pbTherapyGroups->setToolTip("Therapeutengruppe verwalten");
    pbTherapyGroups->setMaximumSize(QSize(28, 28));
    pbTherapyGroups->setMinimumSize(QSize(28, 28));
    pbTherapyGroups->setIcon(QIcon(QString::fromUtf8(":/group.png")));
    pbTherapyGroups->setIconSize(QSize(24, 24));
    gridButtons->addWidget(pbTherapyGroups, 0, 1, 1, 1);
    pbRooms = new QPushButton(widgetButtons);
    pbRooms->setText("");
    pbRooms->setToolTip(QString::fromUtf8("Räume verwalten"));
    pbRooms->setMaximumSize(QSize(28, 28));
    pbRooms->setMinimumSize(QSize(28, 28));
    pbRooms->setIcon(QIcon(QString::fromUtf8(":/room.png")));
    pbRooms->setIconSize(QSize(24, 24));
    gridButtons->addWidget(pbRooms, 0, 2, 1, 1);
    pbCars = new QPushButton(widgetButtons);
    pbCars->setText("");
    pbCars->setToolTip(QString::fromUtf8("Fahrzeuge verwalten"));
    pbCars->setMaximumSize(QSize(28, 28));
    pbCars->setMinimumSize(QSize(28, 28));
    pbCars->setIcon(QIcon(QString::fromUtf8(":/car.png")));
    pbCars->setIconSize(QSize(24, 24));
    gridButtons->addWidget(pbCars, 0, 3, 1, 1);
    QSpacerItem *spacer_3 = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);
    grid4->addItem(spacer_3, 3, 0, 1, 1);

    // add spacer items
    QWidget* spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    boxLayout->addWidget(spacer);

    /*spacer_h = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    boxLayout->addItem(spacer_h);
    spacer_v = new QSpacerItem(40, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);
    boxLayout->addItem(spacer_v);*/

    connect(this, &CalendarDockWidget::dockLocationChanged, this, &CalendarDockWidget::slotLocationChanged);
}


CalendarDockWidget::~CalendarDockWidget()
{

}


void CalendarDockWidget::slotLocationChanged(Qt::DockWidgetArea area)
{
    if (area == Qt::TopDockWidgetArea || area == Qt::BottomDockWidgetArea) {
        direction = QBoxLayout::LeftToRight;
        this->setMaximumHeight(168);
        this->setMinimumHeight(140);
        this->setMaximumWidth(16000); // //// 16000
        this->setMinimumWidth(0);
    } else {
        direction = QBoxLayout::TopToBottom;
        this->setMaximumHeight(16000);
        this->setMinimumHeight(0);
        this->setMaximumWidth(500); // 220
        this->setMinimumWidth(320); // 160
    }
    boxLayout->setDirection(direction);
    QFrame::Shape shape = direction == QBoxLayout::LeftToRight ? QFrame::VLine : QFrame::HLine;
    line_1->setFrameShape(shape);
    line_2->setFrameShape(shape);
    line_3->setFrameShape(shape);
}


//
// CalendarWidget
//
CalendarWidget::CalendarWidget(CalendarDockWidget *d, QWidget *parent) :
    QWidget(parent)
{
    dock = d;

#ifdef Q_OS_MACX
    dock->calendarWidget->setMaximumWidth(260);
    dock->pbEmployees->setIconSize(QSize(16,16));
    dock->pbTherapyGroups->setIconSize(QSize(16,16));
    dock->pbRooms->setIconSize(QSize(16,16));
#endif

    config = new CalendarConfig;
    config->dock = d;
    config->calWidget = this;
    config->setResMode(CalendarConfig::RES_MODE_EMPLOYEES);

    view = new CalendarView(config);
    layout = new QGridLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(view, 0, 0, 1, 1);

    int rm = myConf.get(ConfXML::AREA_PLANNER, "planview").toInt();
    if (rm >= 0 && rm < CalendarConfig::RENDER_MODE_COUNT) {
        dock->cbView->setCurrentIndex(rm);
        view->setRenderMode(rm);
    } else {
        dock->cbView->setCurrentIndex(0);
        view->setRenderMode(CalendarConfig::RENDER_MODE_DAY);
    }

    view->setDate(QDate::currentDate());

    slotResourceView(CalendarConfig::RES_MODE_EMPLOYEES);
    dock->spFrom->setValue(config->hour_start);
    dock->spTo->setValue(config->hour_end);
    dock->spWeekDays->setValue(config->numRenderDays);

    dock->cbGroup->addItem("Alle");
    for (int i = 0; i < db.listEmployeeGroups.count(); i++)
        dock->cbGroup->addItem(db.listEmployeeGroups.at(i)->getDescription());

    dock->cbTherapyGroup->addItem("Alle");
    for (int i = 0; i < db.listTherapyGroups.count(); i++)
        dock->cbTherapyGroup->addItem(db.listTherapyGroups.at(i)->getName());

    buildToolBar();

    view->buildDates();

    connect(dock->cbView, &QComboBox::currentIndexChanged, this, &CalendarWidget::slotSelectView);
    connect(dock->cbResMode, &QComboBox::currentIndexChanged, this, &CalendarWidget::slotResourceView);
    connect(dock->calendarWidget, &QCalendarWidget::selectionChanged, this, &CalendarWidget::slotDateChange);
    connect(dock->spFrom, &QSpinBox::valueChanged, this, &CalendarWidget::slotHourChange);
    connect(dock->spTo, &QSpinBox::valueChanged, this, &CalendarWidget::slotHourChange);
    connect(dock->spMaxRes, &QSpinBox::valueChanged, this, &CalendarWidget::slotResourceNumbers);
    connect(dock->pbMinus, &QPushButton::clicked, this, &CalendarWidget::slotRowDecreaseHeight);
    connect(dock->pbPlus, &QPushButton::clicked, this, &CalendarWidget::slotRowIncreaseHeight);
    connect(dock->spWeekDays, &QSpinBox::valueChanged, this, &CalendarWidget::slotWeekDays);

    connect(dock->pbDateNew, &QPushButton::clicked, this, &CalendarWidget::slotDateNew);
    //connect(dock->pbDateSearch, &QPushButton::clicked, this, &CalendarWidget::slotDateSearch);
    connect(dock->pbDatePrint, &QPushButton::clicked, this, &CalendarWidget::slotDatePrint);
    connect(dock->pbEmployees, &QPushButton::clicked, this, &CalendarWidget::slotManageEmployees);
    connect(dock->pbRooms, &QPushButton::clicked, this, &CalendarWidget::slotManageRooms);
    connect(dock->pbTherapyGroups, &QPushButton::clicked, this, &CalendarWidget::slotManageTherapyGroups);
    connect(dock->pbCars, &QPushButton::clicked, this, &CalendarWidget::slotManageCars);

    connect(dock->cbGroup, &QComboBox::currentIndexChanged, this, &CalendarWidget::slotGroup);
    connect(dock->cbTherapyGroup, &QComboBox::currentIndexChanged, this, &CalendarWidget::slotTherapyGroup);

    connect(&db, &myDB::sigDBChange, this, &CalendarWidget::slotDatabaseChanged);

    // simulate a date change just to trigger header expandation of the current date
    this->slotDateChange();
}


CalendarWidget::~CalendarWidget()
{
    delete view;
    delete config;
    if (calToolBar != NULL) delete calToolBar;
}


void CalendarWidget::enableToolBar(bool b)
{
    (b == true) ? calToolBar->show() : calToolBar->hide();
}


QToolBar* CalendarWidget::getToolBar()
{
    return calToolBar;
}


void CalendarWidget::buildToolBar()
{
    calToolBar = new QToolBar(QString::fromUtf8("Kalender-Toolbar"));
    calToolBar->setMovable(true);
    calToolBar->setIconSize(QSize(20, 20));
    calToolBar->setLayoutDirection(Qt::LeftToRight);
    calToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    calToolBar->addAction(QIcon(":/leftarrow.png"), QString::fromUtf8("Tag zurück"), this, &CalendarWidget::slotPrevDay);
    calToolBar->addAction(QIcon(":/rightarrow.png"), QString::fromUtf8("Tag vor"), this, &CalendarWidget::slotNextDay);
    calToolBar->addAction(QIcon(":/today.png"), QString::fromUtf8("Heute"), this, &CalendarWidget::slotToday);
    calToolBar->addAction(QIcon(":/day.png"), QString::fromUtf8("Tag"), this, &CalendarWidget::slotModeDay);
    calToolBar->addAction(QIcon(":/workweek.png"), QString::fromUtf8("Arbeitswoche"), this, &CalendarWidget::slotModeWeek);
    calToolBar->addAction(QIcon(":/week.png"), QString::fromUtf8("Woche"), this, &CalendarWidget::slotModeFullWeek);

    connect(calToolBar, &QToolBar::orientationChanged, this, &CalendarWidget::slotToolBarLocationChanged);
}


void CalendarWidget::slotToolBarLocationChanged(Qt::Orientation orientation)
{
    if (orientation == Qt::Horizontal) {
        calToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    } else {
        calToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    }
}


void CalendarWidget::slotSelectView(int i)
{
    if (i < 0) return;
    view->setRenderMode(i);
    view->updateComboBox();
    if (i == CalendarConfig::RENDER_MODE_WEEK) view->buildDates();
    view->setDate(dock->calendarWidget->selectedDate());
    view->renderCalendar();
}


void CalendarWidget::slotDateChange()
{
    QDate d = view->getDate();
    bool reload_from_db = true;
    view->setDate(dock->calendarWidget->selectedDate());

    // if we're rendering a week only rebuild date list if the week changed (or week is the same
    // but the year changed)
    if (config->render_mode == CalendarConfig::RENDER_MODE_WEEK) {
        QDate d_compare = dock->calendarWidget->selectedDate();
        if (d.weekNumber() == d_compare.weekNumber() && (d.year() == d_compare.year())) reload_from_db = false;
    }
    if (reload_from_db) view->buildDates();
    if (config->render_mode == CalendarConfig::RENDER_MODE_WEEK) {
        int i = dock->calendarWidget->selectedDate().dayOfWeek() - 1;
        if (i < dock->spWeekDays->value()) view->expandHeader(i, true);
    }
    view->renderCalendar();
}


void CalendarWidget::slotHourChange()
{
    config->setRenderHours(dock->spFrom->value(), dock->spTo->value());
    view->scrollContainer->update();
    view->renderCalendar();
}


void CalendarWidget::slotResourceView(int i)
{
    disconnect(dock->spMaxRes, &QSpinBox::valueChanged, this, &CalendarWidget::slotResourceNumbers);
    config->setResMode(i);
    int x = (config->getNumResourcesAvailable() > config->max_resources_to_display) ?
            config->max_resources_to_display : config->getNumResourcesAvailable();
    dock->spMaxRes->setMaximum(x);
    dock->spMaxRes->setValue(config->getNumResourcesToRender());
    if (i == CalendarConfig::RES_MODE_EMPLOYEES) {
        dock->cbGroup->setEnabled(true);
        if (dock->cbGroup->currentIndex() == cEmployeeGroup::GROUP_THERAPY) dock->cbTherapyGroup->setEnabled(true);
        else dock->cbTherapyGroup->setEnabled(false);
    } else {
        dock->cbGroup->setEnabled(false);
        dock->cbTherapyGroup->setEnabled(false);
    }
    view->updateComboBox();
    view->renderCalendar();
    connect(dock->spMaxRes, &QSpinBox::valueChanged, this, &CalendarWidget::slotResourceNumbers);
}


void CalendarWidget::slotResourceNumbers(int i)
{
    config->setNumResourcesToRender(i);
    view->renderCalendar();
}


void CalendarWidget::restoreRowHeight()
{
    QSettings settings("OpenPhysio", "OpenPhysio");
    int rh = settings.value("plannerRowHeight").toInt();
    if (rh <= 0) {
        config->hour_height = config->hour_height_base * 5;
        return;
    }
    int m = rh % config->hour_height_base;
    if (m != 0) rh = rh - m;
    if (rh < 2 * config->hour_height_base) rh = 2 * config->hour_height_base;
    config->hour_height = rh;
}


int CalendarWidget::getRowHeight()
{
    return config->hour_height;
}


void CalendarWidget::slotRowIncreaseHeight()
{
    config->hour_height += config->hour_height_base;
    view->scrollContainer->update();
    view->renderCalendar();
}


void CalendarWidget::slotRowDecreaseHeight()
{
    config->hour_height -= config->hour_height_base;
    if (config->hour_height < 2 * config->hour_height_base) config->hour_height = 2 * config->hour_height_base;
    view->scrollContainer->update();
    view->renderCalendar();
}


void CalendarWidget::slotWeekDays(int i)
{
    view->setRenderDaysForWeek(i);
    view->renderCalendar();
}


void CalendarWidget::createNewDate(cDateItem& cdi)
{
    GUARD(Protect::FUNC_PLAN);
    DlgDate dlg;
    dlg.init(cdi);
    dlg.exec();
}


void CalendarWidget::slotDateNew()
{
    cDateItem cdi;
    cdi.setDate(QDate::currentDate());

    // round up to 5-minutes
    QTime t = QTime::currentTime();
    int h = t.hour();
    int m = t.minute();

    int t_mod = m % 5;
    if ( (t_mod > 0) && (t_mod < 3) ) m = m - t_mod;
    else if (t_mod > 0) m = m + 5 - t_mod;
    if (m >= 60) {
        h++;
        m -= 60;
    }
    t.setHMS(h, m, 0, 0);
    cdi.setStart(t);
    int add_time = 60 * myConf.get(ConfXML::AREA_PLANNER, "work_interval").toInt();
    cdi.setEnd(t.addSecs(add_time));
    createNewDate(cdi);
}


void CalendarWidget::slotDatabaseChanged(int t)
{
    // if the database signals that the date table changed => reload it
    // (maybe we can optimize that procedure if we know what period of time actually changed
    //  thus avoiding unnessary reloads)
    if (t == myDB::TABLE_DATES) {
        view->buildDates();
        view->renderCalendar();
    } else if ( (t == myDB::TABLE_EMPLOYEES) && (config->res_mode == CalendarConfig::RES_MODE_EMPLOYEES) ) {
        // reload the employees as resources, rebuild the calendar
        slotResourceView(CalendarConfig::RES_MODE_EMPLOYEES);
    } else if ( (t == myDB::TABLE_ROOMS) && (config->res_mode == CalendarConfig::RES_MODE_ROOMS) ) {
        // reload the rooms as resources, rebuild the calendar
        slotResourceView(CalendarConfig::RES_MODE_ROOMS);
    } else if ( (t == myDB::TABLE_CARS) && (config->res_mode == CalendarConfig::RES_MODE_CARS) ) {
        // reload the cars as resources, rebuild the calendar
        slotResourceView(CalendarConfig::RES_MODE_CARS);
    } else if (t == myDB::TABLE_THERAPY_GROUPS) {
        dock->cbTherapyGroup->clear();
        dock->cbTherapyGroup->addItem("Alle");
        for (int i = 0; i < db.listTherapyGroups.count(); i++)
            dock->cbTherapyGroup->addItem(db.listTherapyGroups.at(i)->getName());
        if (config->res_mode == CalendarConfig::RES_MODE_EMPLOYEES)
            slotResourceView(CalendarConfig::RES_MODE_EMPLOYEES);
    }
}


/*void CalendarWidget::slotDateSearch()
{
    MsgBox::dummy();
}*/


void CalendarWidget::slotDatePrint()
{
    QDate d = view->getDate();
    QDate date_start, date_end;
    if (config->render_mode == CalendarConfig::RENDER_MODE_DAY) {
        date_start = d;
        date_end = d;
    } else if (config->render_mode ==CalendarConfig::RENDER_MODE_WEEK) {
        int day_of_week = d.dayOfWeek();
        date_start = d.addDays(1 - day_of_week);
        date_end = date_start.addDays(config->numRenderDays - 1);
    }
    QTime time_start(config->hour_start, 0, 0, 0);
    QTime time_end(config->hour_end, 0, 0, 0);
    DlgSchedule dlg;
    dlg.init(date_start, date_end, time_start, time_end);
    dlg.exec();
}


void CalendarWidget::updateEmployees()
{
    disconnect(dock->spMaxRes, &QSpinBox::valueChanged, this, &CalendarWidget::slotResourceNumbers);
    config->setResMode(CalendarConfig::RES_MODE_EMPLOYEES);
    int x = (config->getNumResourcesAvailable() > config->max_resources_to_display) ?
            config->max_resources_to_display : config->getNumResourcesAvailable();
    dock->spMaxRes->setMaximum(x);
    dock->spMaxRes->setValue(config->getNumResourcesToRender());
    view->updateComboBox();
    view->renderCalendar();
    connect(dock->spMaxRes, &QSpinBox::valueChanged, this, &CalendarWidget::slotResourceNumbers);
}


void CalendarWidget::slotGroup(int i)
{
    config->group = i;
    if (i == cEmployeeGroup::GROUP_THERAPY) {
        dock->cbTherapyGroup->setEnabled(true);
    } else {
        dock->cbTherapyGroup->setEnabled(false);
    }
    if (config->res_mode == CalendarConfig::RES_MODE_EMPLOYEES) updateEmployees();
}


void CalendarWidget::slotTherapyGroup(int i)
{
    if ( (i > 0) && (i <= db.listTherapyGroups.count()) ) { // > 0 because we've inserted "Alle"
        config->therapy_group = db.listTherapyGroups.at(i - 1)->getID();
        if (config->res_mode == CalendarConfig::RES_MODE_EMPLOYEES) updateEmployees();
    } else if (i == 0) {
        config->therapy_group = 0;
        if (config->res_mode == CalendarConfig::RES_MODE_EMPLOYEES) updateEmployees();
    }
}


void CalendarWidget::slotManageEmployees()
{
    // this will signal the main window (openphysio.cpp) that the
    // employees should be managed now
    emit sigManageEmployees();
}


void CalendarWidget::slotManageRooms()
{
    DlgManageRooms dlg;
    dlg.exec();
}


void CalendarWidget::slotManageTherapyGroups()
{
    DlgManageTherapyGroups dlg;
    dlg.exec();
}


void CalendarWidget::slotManageCars()
{
    DlgManageCars dlg;
    dlg.exec();
}


void CalendarWidget::slotPrevDay()
{
    dock->calendarWidget->setSelectedDate(view->getDate().addDays(-1));
}


void CalendarWidget::slotNextDay()
{
    dock->calendarWidget->setSelectedDate(view->getDate().addDays(1));
}


void CalendarWidget::slotToday()
{
    dock->calendarWidget->setSelectedDate(QDate::currentDate());
    int dow = QDate::currentDate().dayOfWeek();
    if (config->render_mode == CalendarConfig::RENDER_MODE_WEEK)
        view->expandHeader(dow - 1, true);
}


void CalendarWidget::slotModeDay()
{
   dock->cbView->setCurrentIndex(0);
}


void CalendarWidget::slotModeWeek()
{
    dock->cbView->setCurrentIndex(1);
    int i = dock->spWeekDays->value();
    if (i < 2 || i > 6) i = 5;
    dock->spWeekDays->setValue(i);
}


void CalendarWidget::slotModeFullWeek()
{
    dock->cbView->setCurrentIndex(1);
    if (dock->spWeekDays->value() != 7) dock->spWeekDays->setValue(7);
}



void CalendarWidget::MoveDate(cDateItem& cdi)
{
    DlgDateMove dlg;
    dlg.init(cdi);
    dlg.exec();
}

void CalendarWidget::FollowUpDate(cDateItem& cdi)
{
    DlgDate dlg;
    cdi.setID(-1);
    cdi.setFlag(cTableItem::FLAG_NEW);
    // remove content field from course entries since this contains the
    // participance information (which should not be cloned ...)
    if (cdi.getType() == cDateType::DT_COURSE && cdi.getCID() > 0) {
        cdi.setContent("");
    }
    dlg.init(cdi);
    dlg.exec();
}


void CalendarWidget::editPatient(cDateItem& cdi)
{
    cPatient p;
    if (db.retrieveItem(p, cdi.getPID()) == true) {
        DlgPatient dlgPatient;
        dlgPatient.initPatient(&p);
        dlgPatient.exec();
    }
}


void CalendarWidget::editReceipe(cDateItem& cdi)
{
    cReceipe r;
    db.retrieveItem(r, cdi.getRID());

    // FIXME: replace static value (3) !!!
    if (r.getType() < 3) {
        DlgReceipe dlgReceipe;
        dlgReceipe.initDlg(&r, r.getProfession());
        dlgReceipe.exec();
    } else {
        DlgReceipePriv dlgReceipe;
        dlgReceipe.initDlg(&r);
        dlgReceipe.exec();
    }
}

