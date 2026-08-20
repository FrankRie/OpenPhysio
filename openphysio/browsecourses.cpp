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

#include "browsecourses.h"
#include "mydb.h"
#include "protect.h"
#include "msgbox.h"


BrowseCourses::BrowseCourses(int mode, QWidget *parent)
    : Browse(mode, parent)
{
    ui.setupUi(this);

    init((TableViewAnimated*)ui.tvCourses, myDB::TABLE_COURSES, 1, Qt::DescendingOrder);

    // add display filter widget and toolbar
    QGridLayout *layout = (QGridLayout*) this->layout();
    layout->addWidget(&browseToolBar, 0, 0);
    layout->addWidget(&displayFilter, 0, 1);

    connect(ui.tvCourses, &QTableView::activated, this, &BrowseCourses::slotCourseSelected);

    actCourseAdd = new QAction(QIcon(":/new.png"), QString::fromUtf8("Neuer Kurs"), this);
    actCourseEdit = new QAction(QIcon(":/edit.png"), QString::fromUtf8("Kurs bearbeiten"), this);
    actCourseDel = new QAction(QIcon(":/delete.png"), QString::fromUtf8("Kurs löschen"), this);
    connect(actCourseAdd, &QAction::triggered, this, &BrowseCourses::slotCourseAdd);
    connect(actCourseEdit, &QAction::triggered, this, &BrowseCourses::slotCourseEdit);
    connect(actCourseDel, &QAction::triggered, this, &BrowseCourses::slotCourseDel);

    browseToolBar.addAction(actCourseAdd);
    browseToolBar.addSeparator();
    browseToolBar.addAction(actCourseEdit);
    browseToolBar.addSeparator();
    browseToolBar.addAction(actCourseDel);
    ui.tvCourses->addAction(actCourseAdd);
    ui.tvCourses->addAction(actCourseEdit);
    ui.tvCourses->addAction(actCourseDel);
    ui.tvCourses->setContextMenuPolicy(Qt::ActionsContextMenu);

    if (myConf.isTableColorMode()) {
	ui.tvCourses->setAlternatingRowColors(false);
	QStringList keywords;
	QList<QColor> colors;
	keywords << "Abgeschlossen" << "Abgebrochen" << "Abgerechnet";
	colors << myConf.getColor("Abgeschlossen") << myConf.getColor("Abgebrochen") << myConf.getColor("Abgerechnet");
	setBackgroundKeywords(keywords, colors);
	enableBackgroundColorMode(true, 3);
    }
}


BrowseCourses::~BrowseCourses()
{
    if (actCourseAdd != NULL) delete actCourseAdd;
    if (actCourseDel != NULL) delete actCourseDel;
    if (actCourseEdit != NULL) delete actCourseEdit;
}


void BrowseCourses::slotCourseSelected()
{
    if (curMode == MODE_FULL) {
        GUARD(Protect::FUNC_COU);
        slotCourseEdit();
    } else {
        int id = getSelectedItem();
        if (id < 0) return;
        db.retrieveItem(selCourse, id);
        emit sigItemSelected();
    }
}


void BrowseCourses::slotCourseAdd()
{
    GUARD(Protect::FUNC_COU);
	
    DlgCourse dlgCourse;
    dlgCourse.initDlg(NULL);
    dlgCourse.exec();
}


void BrowseCourses::slotCourseEdit()
{
    GUARD(Protect::FUNC_COU);
	
    int id = getSelectedItem();
    if (id < 0) {
        MsgBox::info(QString::fromUtf8("Bitte erst einen Kurs wählen!"));
        return;
    }

    cCourse c;
    db.retrieveItem(c, id);
    DlgCourse dlgCourse;
    dlgCourse.initDlg(&c);
    dlgCourse.exec();
}


void BrowseCourses::slotCourseDel()
{
    GUARD(Protect::FUNC_COU_DEL);

    int id = getSelectedItem();
    if (id < 0) {
        MsgBox::info(QString::fromUtf8("Bitte erst einen Kurs wählen!"));
        return;
    }

    MsgBox box;
    if (box.execYesNoBox(QString::fromUtf8("Den Kurs wirklich löschen?"), MsgBox::BTN_NO) == MsgBox::BTN_YES) {
	if (db.deleteItem(myDB::TABLE_COURSES, id) == false)
            MsgBox::error(QString::fromUtf8("Datenbankfehler - Kurs konnte nicht gelöscht werden."));
   }
}

cCourse& BrowseCourses::getSelCourses()
{
    return selCourse;
}
