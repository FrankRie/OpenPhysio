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

#ifndef DATETOOLS_H
#define DATETOOLS_H

#ifndef OPA

#include <QtCore>
#include <QtWidgets>

#include "mydb.h"

class DateTools
{
public:
    DateTools();
    bool OverlapCheck(cDateItem& cdi);
    bool OverlapCheck(cDateItem& cdi, QList<cDateItem*> &listDates);
    QString DateDiff(QDate d_from, QDate d_until);

private:
    int overlap(cDateItem& cdi, QList<cDateItem*> &listDates, int *first_overlap);
};


class DateValidator : public QValidator
{
    Q_OBJECT
public:
    DateValidator();
    void fixup(QString& input) const;
    QValidator::State validate(QString& input, int& pos) const;
    bool isValidDate(QString input);

signals:
    void invalid(bool b);
};


class TimeValidator : public QValidator
{
    Q_OBJECT
public:
    TimeValidator();
    void fixup(QString& input) const;
    QValidator::State validate(QString& input, int& pos) const;

signals:
    void invalid(bool b);
};


class CalendarPopup : public QWidget
{
    Q_OBJECT
public:
    CalendarPopup(QWidget *parent = 0);
    ~CalendarPopup();
    void setDate(const QDate &date);
    QDate date();

protected:
    void mousePressEvent(QMouseEvent *e);
    void hideEvent(QHideEvent *);

private:
    QCalendarWidget *calendar;
    QDate oldDate;
    bool dateChanged;

private slots:
    void dateSelected(const QDate &date);
    void dateSelectionChanged();

signals:
    void activated(const QDate &date);
    void newDateSelected(const QDate &newDate);
    void hidingCalendar(const QDate &oldDate);
    void resetButton();
};


class DateEdit : public QLineEdit
{
    Q_OBJECT
public:
    DateEdit(QWidget *parent = 0);
    ~DateEdit();
    void positionCalendarPopup();
    QDate date();
    void setDate(QDate d);

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);

private:
    CalendarPopup *popup;
    DateValidator *dv;
    QStyle::StateFlag arrowState;
    QPalette palInvalid;
    QPalette palValid;

private slots:
    void slotResetState();
    void slotSetDate(const QDate &date);
    void slotInvalid(bool b);

signals:
    void dateChanged(const QDate &date);
};


class TimeEdit : public QLineEdit
{
    Q_OBJECT
public:
    TimeEdit(QWidget *parent = 0);
    ~TimeEdit();
    QTime time();
    void setTime(QTime time);

protected:
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void wheelEvent(QWheelEvent * event);

private:
    TimeValidator *tv;
    QPalette palInvalid;
    QPalette palValid;
    void stepBy(int delta);

private slots:
    void slotInvalid(bool b);

signals:
    void timeChanged(const QTime &time);
};

#endif

#endif // DATETOOLS_H
