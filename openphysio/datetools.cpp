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
#include "datetools.h"
#include "msgbox.h"
#include "employeetools.h"


DateTools::DateTools()
{
}


// we will return a bit mask that indicates for what resource type(s) an overlap was detected
int DateTools::overlap(cDateItem& cdi, QList<cDateItem*> &listDates, int *first_overlap)
{
    int co = listDates.count();
    if (co < 1) return 0;

    bool overlap_eid = false;
    bool overlap_room = false;
    bool overlap_car = false;

    int s_t1 = abs(cdi.getStart().secsTo(QTime(0,0)));
    int s_t2 = abs(cdi.getEnd().secsTo(QTime(0,0)));
    for (int i = 0; i < co; i++) {
        cDateItem *c = listDates.at(i);
        if (cdi.getID() == c->getID()) continue;
        if (cdi.getFullDay() != c->getFullDay()) continue;
        if (cdi.getDate() != c->getDate()) continue;
        bool detected_overlap = false;
        if (!cdi.getFullDay()) {
            int s_start = abs(c->getStart().secsTo(QTime(0,0)));
            int s_end = abs(c->getEnd().secsTo(QTime(0,0)));
            if ( (s_t1 >= s_start) && (s_t1 < s_end) ) detected_overlap = true;
            if ( (s_t2 > s_start) && (s_t2 < s_end) ) detected_overlap = true;
        } else detected_overlap = true;
        // now check for which resource an overlap occured
        if (detected_overlap) {
            if (cdi.getEID() == c->getEID() && cdi.getEID() != -1) overlap_eid = true;
            if (cdi.getRoomID() == c->getRoomID() && cdi.getRoomID() != -1) overlap_room = true;
            if (cdi.getCarID() == c->getCarID() && cdi.getCarID() != -1) overlap_car = true;
            *first_overlap = i;
            break;
        }
    }

    int iRet = ((int)overlap_eid) | ((int)overlap_room << 1) | ((int)overlap_car << 2);
    return iRet;
}


bool DateTools::OverlapCheck(cDateItem& cdi, QList<cDateItem*> &listDates)
{
    bool bRet = true;
    // load dates from the db that matches our day

    int first_overlap = -1;
    int ol = overlap(cdi, listDates, &first_overlap);
    if (ol != 0)  {
        bool overlap_eid = (ol & 1);
        bool overlap_room = ( (ol >> 1) & 1);
        bool overlap_car = ( (ol >> 2) & 1);
        QString s;
        s = QString::fromUtf8("Der Termin überschneidet sich hinsichtlich ");
        QStringList strList;
        if (overlap_eid) strList << QString::fromUtf8("Mitarbeiterverfügbarkeit");
        if (overlap_room) strList << QString::fromUtf8("Raumnutzung");
        if (overlap_car) strList << QString::fromUtf8("Fahrzeugnutzung");
        s += strList.join(", ");
        s += " mindestens mit folgendem Termin:\n\n";
        if (first_overlap >= 0 && first_overlap < listDates.count()) {
            cDateItem *c = listDates.at(first_overlap);
            s += c->getTitle() + " am " + c->getDate().toString("dd.MM.yyyy");
            if (!c->getFullDay()) s += " von " + c->getStart().toString("hh:mm") + " bis " + c->getEnd().toString("hh:mm") + " Uhr";
            if (cdi.getEID() != -1 && overlap_eid) s += "\nMitarbeiter: " + EmplTool.getEmplFullName(c->getEID());
            if (cdi.getRoomID() != -1 && overlap_room) {
                for (int j = 0; j < db.rooms.count(); j++) {
                    if (db.rooms.at(j)->getID() == cdi.getRoomID()) {
                        s += "\nRaum: " + db.rooms.at(j)->getName();
                        break;
                    }
                }
            }
            if (cdi.getCarID() != -1 && overlap_car) {
                for (int j = 0; j < db.listCars.count(); j++) {
                    if (db.listCars.at(j)->getID() == cdi.getCarID()) {
                        s += "\nFahrzeug: " + db.listCars.at(j)->getName();
                        break;
                    }
                }
            }
        }
        s += QString::fromUtf8("\n\nWollen Sie trotzdem fortfahren?");
        MsgBox msg;
        if (msg.execYesNoBox(s, MsgBox::BTN_YES) == MsgBox::BTN_NO) {
            bRet = false;
        }
    }

    return bRet;
}


bool DateTools::OverlapCheck(cDateItem& cdi)
{
    QList<cDateItem *> listDates;
    if (!db.loadDates(listDates, cdi.getDate(), cdi.getDate())) return false;
    bool bRet = OverlapCheck(cdi, listDates);
    while (!listDates.isEmpty()) delete listDates.takeFirst();
    return bRet;
}


QString DateTools::DateDiff(QDate d_from, QDate d_until)
{
    QDate d1, d2;
    if (d_until < d_from) {
        d1 = d_from;
        d2 = d_until;
    } else {
        d1 = d_until;
        d2 = d_from;
    }
    int j = d1.year() - d2.year();
    int m = d1.month() - d2.month();
    int t = d1.day() - d2.day();
    if (j != 0) {
        if (t < 0) {
            m--;
            d1.setDate(d1.year(), m, 0);
            t += d1.daysInMonth();
        }
        if (m < 0) {
            m += 12;
            j--;
        }
    }

    QString s = "Alter:";
    if (j > 0) s += " " + QString::number(j) + " Jahr(e)";
    if (m > 0) s += " " + QString::number(m) + " Monat(e)";
    if (t > 0) s += " " + QString::number(t) + " Tag(e)";
    if (j == 0 && m == 0 && t == 0) s += " 0 Tage";
    return s;
}


//****************************************************************************
// DateValidator
//****************************************************************************
DateValidator::DateValidator()
{

}

void DateValidator::fixup(QString& input) const
{
    // h => heute
    // m => morgen
    // ü => übermorgen
    // g => gestern
    // v => vorgestern
    // h+<x> => heute + x Tage
    // h-<x> => heute - x Tage
    //
    // ab.cd.efgh
    // a.bc.defg
    // ab.c.defg
    // a.b.cdef
    // a.b.cd
    // ab.c.de
    // a.bc.de
    // a.b.
    // ab.c.
    // a.bc.
    // abcdefgh
    // efghcdab
    if (input.isEmpty()) return;
    QDate d = QDate::currentDate();
    if (input.length() == 1) {
        if (input == "h") input = d.toString("dd.MM.yyyy");
        else if (input == "m") input = d.addDays(1).toString("dd.MM.yyyy");
        else if (input == QString::fromUtf8("ü")) input = d.addDays(2).toString("dd.MM.yyyy");
        else if (input == "g") input = d.addDays(-1).toString("dd.MM.yyyy");
        else if (input == "v") input = d.addDays(-2).toString("dd.MM.yyyy");
        return;
    }
    QRegularExpression regexp;
    regexp.setPattern("^[0-9]{1,2}\\.[0-9]{1,2}\\.(([0-9][0-9]){0,2})$");
    QRegularExpressionMatch match = regexp.match(input);
    if (match.hasMatch()) {
        QStringList l;
        l = input.split("."); // we know we have at least to dots
        if (l.count() != 3 ) return;
        int day = QString(l.at(0)).toInt();
        int month = QString(l.at(1)).toInt();
        int year = QString(l.at(2)).toInt();
        if (QString(l.at(2)).length() == 0) year = d.year();
        else if (QString(l.at(2)).length() == 2) {
            if (QDate::currentDate().year() < year + 2000) year = 1900 + year;
            else year = 2000 + year;
        }
        if (year < 1800 || year > 2050) return;
        if (month < 1 || month > 12) return;
        QDate tmp(year, month, 1);
        int dim = tmp.daysInMonth();
        if (day < 1 || day > dim) return;
        tmp.setDate(year, month, day);
        input = tmp.toString("dd.MM.yyyy");
        return;
    }
    regexp.setPattern("^[0-9]{8}$");
    match = regexp.match(input);
    if (match.hasMatch()) {
        QString s = input;
        int year = (s.left(4)).toInt();
        int month = input.mid(4, 2).toInt();
        int day = (s.right(2)).toInt();
        if (year < 1800 || year > 2050) return;
        if (month < 1 || month > 12) return;
        QDate tmp(year, month, 1);
        int dim = tmp.daysInMonth();
        if (day < 1 || day > dim) return;
        tmp.setDate(year, month, day);
        input = tmp.toString("dd.MM.yyyy");
        return;
    }
    regexp.setPattern("^h\\+[0-9]{1,3}");
    match = regexp.match(input);
    if (match.hasMatch()) {
        int ofs = input.remove(0, 2).toInt();
        input = d.addDays(ofs).toString("dd.MM.yyyy");
        return;
    }
    regexp.setPattern("^h\\-[0-9]{1,3}");
    match = regexp.match(input);
    if (match.hasMatch()) {
        int ofs = 0;
        ofs -= input.remove(0, 2).toInt();
        input = d.addDays(ofs).toString("dd.MM.yyyy");
        return;
    }
}


QValidator::State DateValidator::validate(QString& input, int& pos) const
{
    QRegularExpression regexp("^[0-9]{2}.[0-9]{2}.[0-9]{4}$");
    QRegularExpressionMatch match = regexp.match(input);
    if (match.hasMatch()) {
        emit const_cast<DateValidator*>(this)->invalid(false);
        return QValidator::Acceptable;
    }
    emit const_cast<DateValidator*>(this)->invalid(!input.isEmpty());
    return QValidator::Intermediate;
}


bool DateValidator::isValidDate(QString input)
{
    QRegularExpression regexp("^[0-9]{2}.[0-9]{2}.[0-9]{4}$");
    QRegularExpressionMatch match = regexp.match(input);
    if (match.hasMatch()) return true;
    return false;
}


//****************************************************************************
// TimeValidator
//****************************************************************************
TimeValidator::TimeValidator()
{
}


void TimeValidator::fixup(QString& input) const
{
    // j => jetzt
    //
    // a
    // ab
    // a:b
    // a:bc
    // ab:c
    // ab:cd
    // abcd
    if (input.isEmpty()) return;
    if (input.length() == 1 && input == "j") {
        input = QTime::currentTime().toString("hh:mm");
        return;
    }
    QRegularExpression  regexp;
    regexp.setPattern("^[0-9]{1,2}$");
    QRegularExpressionMatch match = regexp.match(input);
    if (match.hasMatch()) {
        int hour = input.toInt();
        if (hour < 0 || hour > 23) return;
        input = QString::number(hour) + ":00";
        return;
    }
    regexp.setPattern("^[0-9]{1,2}:[0-9]{1,2}$");
    match = regexp.match(input);
    if (match.hasMatch()) {
        QStringList l = input.split(":");
        int hour = QString(l.at(0)).toInt();
        int minute = QString(l.at(1)).toInt();
        if (hour < 0 || hour > 23 || minute < 0 || minute > 59) return;
        QTime t(hour, minute);
        input = t.toString("hh:mm");
        return;
    }
    regexp.setPattern("^[0-9]{4}$");
    match = regexp.match(input);
    if (match.hasMatch()) {
        int hour = input.left(2).toInt();
        if (hour < 0 || hour > 23) return;
        int minute = input.right(2).toInt();
        if (minute < 0 || minute > 59) return;
        QTime t(hour, minute);
        input = t.toString("hh:mm");
        return;
    }
}


QValidator::State TimeValidator::validate(QString& input, int& pos) const
{
    QRegularExpression regexp("^[0-9]{1,2}:[0-9]{2}$");
    QRegularExpressionMatch match = regexp.match(input);
    if (match.hasMatch()) {
        QStringList l = input.split(":");
        int hour = QString(l.at(0)).toInt();
        int minute = QString(l.at(1)).toInt();
        if (hour < 0 || hour > 23 || minute < 0 || minute > 59) return QValidator::Intermediate;
        emit const_cast<TimeValidator*>(this)->invalid(false);
        return QValidator::Acceptable;
    }
    emit const_cast<TimeValidator*>(this)->invalid(!input.isEmpty());
    return QValidator::Intermediate;
}


//****************************************************************************
// CalendarPopup
//****************************************************************************
CalendarPopup::CalendarPopup(QWidget * parent)
    : QWidget(parent, Qt::Popup), calendar(0)
{
    setAttribute(Qt::WA_WindowPropagation);
    dateChanged = false;
    calendar = new QCalendarWidget(this);
    QVBoxLayout *widgetLayout = qobject_cast<QVBoxLayout*>(layout());
    if (!widgetLayout) {
        widgetLayout = new QVBoxLayout(this);
        widgetLayout->setContentsMargins(0, 0, 0, 0);
        widgetLayout->setSpacing(0);
    }
    calendar->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    calendar->setFirstDayOfWeek(Qt::Monday);
    calendar->setGridVisible(true);
    widgetLayout->addWidget(calendar);

    connect(calendar, SIGNAL(activated(QDate)), this, SLOT(dateSelected(QDate)));
    connect(calendar, SIGNAL(clicked(QDate)), this, SLOT(dateSelected(QDate)));
    connect(calendar, SIGNAL(selectionChanged()), this, SLOT(dateSelectionChanged()));

    calendar->setFocus();
 }


CalendarPopup::~CalendarPopup()
{
    delete calendar;
}


QDate CalendarPopup::date()
{
    return calendar->selectedDate();
}


void CalendarPopup::mousePressEvent(QMouseEvent *event)
{
    QLineEdit *le = qobject_cast<QLineEdit *>(parentWidget());
    if (le) {
        QStyleOptionComboBox opt;
        opt.initFrom(le);
        QRect arrowRect = le->style()->subControlRect(QStyle::CC_ComboBox, &opt,
                                                            QStyle::SC_ComboBoxArrow, le);
        arrowRect.moveTo(le->mapToGlobal(arrowRect.topLeft()));
        if (arrowRect.contains(event->globalPosition().toPoint()) || rect().contains(event->position().toPoint()))
            setAttribute(Qt::WA_NoMouseReplay);
    }
    QWidget::mousePressEvent(event);
}


void CalendarPopup::hideEvent(QHideEvent *)
{
    emit resetButton();
    if (!dateChanged)
        emit hidingCalendar(oldDate);
}


void CalendarPopup::dateSelectionChanged()
{
    dateChanged = true;
    emit newDateSelected(calendar->selectedDate());
}


void CalendarPopup::dateSelected(const QDate &date)
{
    dateChanged = true;
    emit activated(date);
    close();
}


void CalendarPopup::setDate(const QDate &date)
{
    oldDate = date;
    calendar->setSelectedDate(date);
}


//****************************************************************************
// DateEdit
//****************************************************************************
DateEdit::DateEdit(QWidget *parent)
    : QLineEdit(parent)
{
    popup = new CalendarPopup(this);
    arrowState = QStyle::State_None;

    palValid = QApplication::palette();;
    palInvalid = QApplication::palette();
    palInvalid.setColor(QPalette::Base, QColor(255, 137, 137, 255));

    dv = new DateValidator();
    setValidator(dv);
    connect(dv, SIGNAL(invalid(bool)), this, SLOT(slotInvalid(bool)));
    connect(popup, SIGNAL(resetButton()), this, SLOT(slotResetState()));
    connect(popup, SIGNAL(newDateSelected(QDate)), this, SLOT(slotSetDate(QDate)));
    connect(popup, SIGNAL(hidingCalendar(QDate)), this, SLOT(slotSetDate(QDate)));
    connect(popup, SIGNAL(activated(QDate)), this, SLOT(slotSetDate(QDate)));
    connect(popup, SIGNAL(activated(QDate)), popup, SLOT(close()));
}


DateEdit::~DateEdit()
{
    delete popup;
    setValidator(NULL);
    delete dv;
}


void DateEdit::paintEvent(QPaintEvent *event)
{
    QLineEdit::paintEvent(event);

    //QStyleOptionFrameV2 opt;
    //opt.initFrom(this);

    QStyleOptionComboBox optCombo;
    optCombo.initFrom(this);
    optCombo.editable = true;
    //optCombo.rect.adjust(this->width() - 40, 0, 20, 0);
    //optCombo.rect.adjust(this->width() - 20, 0, 0, 0);
    int i = style()->subControlRect(QStyle::CC_ComboBox, &optCombo, QStyle::SC_ComboBoxArrow, this).width();
    optCombo.rect.adjust(this->width() - i, 0, 0, 0);
    //optCombo.subControls = QStyle::SC_ComboBoxFrame | QStyle::SC_ComboBoxEditField | QStyle::SC_ComboBoxArrow;
    //optCombo.state = opt.state;
    optCombo.activeSubControls = QStyle::SC_ComboBoxArrow;
    if (arrowState == QStyle::State_Sunken) optCombo.state |= QStyle::State_Sunken;
    else optCombo.state &= ~QStyle::State_Sunken;

    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_IndicatorArrowDown, &optCombo, &painter, this);
    //style()->drawComplexControl(QStyle::CC_ComboBox, &optCombo, &painter, this);
}


void DateEdit::positionCalendarPopup()
{
    QPoint pos = (this->layoutDirection() == Qt::RightToLeft) ? this->rect().bottomRight() : this->rect().bottomLeft();
    QPoint pos2 = (this->layoutDirection() == Qt::RightToLeft) ? this->rect().topRight() : this->rect().topLeft();
    pos = this->mapToGlobal(pos);
    pos2 = this->mapToGlobal(pos2);
    QSize size = popup->sizeHint();
    QScreen *screen = QGuiApplication::screenAt(pos);
    if (!screen) {
        screen = QGuiApplication::primaryScreen();
    }
    QRect screenGeom = screen->availableGeometry();

    if (this->layoutDirection() == Qt::RightToLeft) {
        pos.setX(pos.x() - size.width());
        pos2.setX(pos2.x() - size.width());
        if (pos.x() < screenGeom.left())
            pos.setX(qMax(pos.x(), screenGeom.left()));
        else if (pos.x() + size.width() > screenGeom.right())
            pos.setX(qMax(pos.x() - size.width(), screenGeom.right() - size.width()));
    } else {
        if (pos.x() + size.width() > screenGeom.right())
            pos.setX(screenGeom.right()-size.width());
        pos.setX(qMax(pos.x(), screenGeom.left()));
    }
    if (pos.y() + size.height() > screenGeom.bottom())
        pos.setY(pos2.y() - size.height());
    else if (pos.y() < screenGeom.top())
        pos.setY(screenGeom.top());
    if (pos.y() < screenGeom.top())
        pos.setY(screenGeom.top());
    if (pos.y() + size.height() > screenGeom.bottom())
        pos.setY(screenGeom.bottom() - size.height());
    popup->move(pos);
}


void DateEdit::mousePressEvent(QMouseEvent *event)
{
    QStyleOptionComboBox optCombo;
    optCombo.initFrom(this);
    optCombo.editable = true;
    optCombo.subControls = QStyle::SC_All;
    QStyle::SubControl hoverControl = style()->hitTestComplexControl(QStyle::CC_ComboBox, &optCombo, event->pos(), this);
    if (hoverControl == QStyle::SC_ComboBoxArrow) {
        event->accept();
        if (dv->isValidDate(text())) {
            popup->setDate(QDate::fromString(text(), "dd.MM.yyyy"));
        }
        positionCalendarPopup();
        arrowState = QStyle::State_Sunken;
        update();
        popup->show();
    } else {
        QLineEdit::mousePressEvent(event);
    }
}


void DateEdit::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space) {
        event->accept();
        QString s = text();
        dv->fixup(s);
        setText(s);
        if (dv->isValidDate(s)) popup->setDate(QDate::fromString(s, "dd.MM.yyyy"));
    } else QLineEdit::keyPressEvent(event);
}


void DateEdit::slotResetState()
{
    arrowState = QStyle::State_None;
    update();
}


void DateEdit::slotSetDate(const QDate &date)
{
    setText(date.toString("dd.MM.yyyy"));
    emit dateChanged(this->date());
}


QDate DateEdit::date()
{
    if (dv->isValidDate(text())) {
        return QDate::fromString(text(), "dd.MM.yyyy");
    } else {
        return QDate(0, 0, 0);
    }
}


void DateEdit::setDate(QDate d)
{
    //popup->setDate(d);
    setText(d.toString("dd.MM.yyyy"));
}


void DateEdit::slotInvalid(bool b)
{
    if (b) setPalette(palInvalid);
    else setPalette(palValid);
}


//****************************************************************************
// TimeEdit
//****************************************************************************
TimeEdit::TimeEdit(QWidget *parent)
    : QLineEdit(parent)
{
    palValid = QApplication::palette();;
    palInvalid = QApplication::palette();
    palInvalid.setColor(QPalette::Base, QColor(255, 137, 137, 255));

    tv = new TimeValidator();
    setValidator(tv);
    connect(tv, SIGNAL(invalid(bool)), this, SLOT(slotInvalid(bool)));
}


TimeEdit::~TimeEdit()
{
    setValidator(NULL);
    delete tv;
}


void TimeEdit::keyPressEvent(QKeyEvent *event)
{
    switch(event->key()) {
        case Qt::Key_Space: {
                event->accept();
                QString s = text();
                tv->fixup(s);
                setText(s);
                emit timeChanged(QTime::fromString(s));
            }
            break;
            case Qt::Key_Up: {
                event->accept();
                stepBy(1);
            }
            break;
            case Qt::Key_Down: {
                event->accept();
                stepBy(-1);
            }
            break;
        default:
            QLineEdit::keyPressEvent(event);
            break;
    }
}


void TimeEdit::wheelEvent(QWheelEvent * event)
{
    stepBy((event->hasPixelDelta() > 0 ? 1 : -1));
    event->accept();
}


void TimeEdit::stepBy(int delta)
{
    QString t = text();
    if (t.isEmpty()) return;

    QStringList l = t.split(":");
    if (l.count() == 1) {
        QRegularExpression regexp;
        regexp.setPattern("^[0-9]{1,2}$");
        QRegularExpressionMatch match = regexp.match(t);
        if (match.hasMatch()) {
            int n = t.toInt() + delta;
            if (n < 0) n = 0;
            if (n > 23) n = 23;
            setText(QString::number(n) + ":00");
            setCursorPosition(0);
        }
    } else if (l.count() == 2) {
        int c = cursorPosition();
        int ofs = QString(l.at(0)).length() >= c ? 0 : 1;
        int n = QString(l.at(ofs)).toInt() + delta;
        if (n < 0) n = 0;
        else if (ofs && n > 59) n = 59;
        else if (!ofs && n > 23) n = 23;
        QString s = QString::number(n);
        if (ofs && s.length() == 1) s = "0" + s;
        if (ofs) s = l.at(0) + ":" + s;
        else s = s + ":" + l.at(1);
        setText(s);
        setCursorPosition(c);
        emit timeChanged(QTime::fromString(s));
    }
}


void TimeEdit::slotInvalid(bool b)
{
    if (b) setPalette(palInvalid);
    else setPalette(palValid);
}


QTime TimeEdit::time()
{
    return QTime::fromString(text(), "hh:mm");
}


void TimeEdit::setTime(QTime time)
{
    setText(time.toString("hh:mm"));
    emit timeChanged(time);
}
