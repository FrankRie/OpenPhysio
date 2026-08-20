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

#ifndef DISPLAYFILTER_H
#define DISPLAYFILTER_H

#include <QtWidgets>

namespace Ui {
    class DisplayFilter;
}

class DisplayFilter : public QWidget {
    Q_OBJECT
    Q_DISABLE_COPY(DisplayFilter)
public:
    explicit DisplayFilter(QWidget *parent = 0, bool auto_keyboard_focus = false);
    virtual ~DisplayFilter();
    void init(QStringList& headers, int default_header);
    void setFilterString(QString s);
    void clearFilterField();
    void setKeyboardFocus();
    bool useAutoKeyboardFocus();
    void useAutoKeyboardFocus(bool auto_keyboard_focus);

private:
    Ui::DisplayFilter *m_ui;
    bool auto_kbd_focus;

private slots:
    void slotStart();

signals:
    void filterChanged(QString s, int column);
};

#endif // DISPLAYFILTER_H
