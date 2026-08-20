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

#ifndef BROWSECOURSES_H
#define BROWSECOURSES_H

#include <QtWidgets>
#include "ui_browsecourses.h"
#include "browse.h"
#include "dlgcourse.h"


class BrowseCourses : public Browse
{
    Q_OBJECT
public:
    BrowseCourses(int mode = MODE_FULL, QWidget *parent = 0);
    ~BrowseCourses();
    cCourse& getSelCourses();

private:
    Ui::BrowseCoursesClass ui;
    QAction *actCourseAdd, *actCourseDel, *actCourseEdit;
    cCourse selCourse;

public slots:
    void slotCourseAdd();

private slots:
    void slotCourseSelected();
    void slotCourseDel();
    void slotCourseEdit();
};

#endif // BROWSECOURSES_H
