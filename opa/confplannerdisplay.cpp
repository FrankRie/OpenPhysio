/****************************************************************************
**
** Copyright (C) 2013 Sebastian Vandersee
** All rights reserved.
**
** This file is part of OpenPhysio.
**
** Please refer to the LICENSE file for terms of use.
**
****************************************************************************/

#include "confplannerdisplay.h"
#include "ui_confplannerdisplay.h"


ConfPlannerDisplay::ConfPlannerDisplay(QWidget *parent, int l, int a) :
    ConfBase(parent, l, a),
    ui(new Ui::ConfPlannerDisplay)
{
    ui->setupUi(this);
    setTitle("Terminplaner");
    setKeyword("opa_planner");
}


ConfPlannerDisplay::~ConfPlannerDisplay()
{
    delete ui;
}


void ConfPlannerDisplay::init()
{
    ui->spFrom->setValue(myConf.get(ConfXML::AREA_PLANNER, "work_start").toInt());
    ui->spTo->setValue(myConf.get(ConfXML::AREA_PLANNER, "work_end").toInt());
    ui->spWeekDays->setValue(myConf.get(ConfXML::AREA_PLANNER, "work_days").toInt());
    ui->spNumResDisplay->setValue(myConf.get(ConfXML::AREA_PLANNER, "maxresdisplay").toInt());
    ui->cbView->setCurrentIndex(myConf.get(ConfXML::AREA_PLANNER, "planview").toInt());
    ui->spWorkInterval->setValue(myConf.get(ConfXML::AREA_PLANNER, "work_interval").toInt());
    ui->cbShowStartEnd->setChecked((bool)myConf.get(ConfXML::AREA_PLANNER, "render_start_end").toInt());
    ui->cbStartPlanner->setChecked((bool)myConf.get(ConfXML::AREA_PLANNER, "planstart").toInt());
    ui->cbShowActivities->setChecked((bool)myConf.get(ConfXML::AREA_PLANNER, "render_activities").toInt());
    ui->cbShowFullName->setChecked((bool)myConf.get(ConfXML::AREA_PLANNER, "render_fullname").toInt());

    QPalette pal;
    pal = ui->widgetTherapy->palette();
    pal.setColor(QPalette::Window, myConf.getColorFromString(myConf.get(ConfXML::AREA_PLANNER, "colTherapy")));
    ui->widgetTherapy->setPalette(pal);

    pal = ui->widgetCourse->palette();
    pal.setColor(QPalette::Window, myConf.getColorFromString(myConf.get(ConfXML::AREA_PLANNER, "colCourse")));
    ui->widgetCourse->setPalette(pal);

    pal = ui->widgetMeeting->palette();
    pal.setColor(QPalette::Window, myConf.getColorFromString(myConf.get(ConfXML::AREA_PLANNER, "colMeeting")));
    ui->widgetMeeting->setPalette(pal);

    pal = ui->widgetTraining->palette();
    pal.setColor(QPalette::Window, myConf.getColorFromString(myConf.get(ConfXML::AREA_PLANNER, "colTraining")));
    ui->widgetTraining->setPalette(pal);

    pal = ui->widgetHoliday->palette();
    pal.setColor(QPalette::Window, myConf.getColorFromString(myConf.get(ConfXML::AREA_PLANNER, "colHoliday")));
    ui->widgetHoliday->setPalette(pal);

    pal = ui->widgetCorpHoliday->palette();
    pal.setColor(QPalette::Window, myConf.getColorFromString(myConf.get(ConfXML::AREA_PLANNER, "colCorpHoliday")));
    ui->widgetCorpHoliday->setPalette(pal);

    pal = ui->widgetVisit->palette();
    pal.setColor(QPalette::Window, myConf.getColorFromString(myConf.get(ConfXML::AREA_PLANNER, "colVisit")));
    ui->widgetVisit->setPalette(pal);

    pal = ui->widgetBreak->palette();
    pal.setColor(QPalette::Window, myConf.getColorFromString(myConf.get(ConfXML::AREA_PLANNER, "colBreak")));
    ui->widgetBreak->setPalette(pal);

    pal = ui->widgetOther->palette();
    pal.setColor(QPalette::Window, myConf.getColorFromString(myConf.get(ConfXML::AREA_PLANNER, "colOther")));
    ui->widgetOther->setPalette(pal);

    pal = ui->widgetDrive->palette();
    pal.setColor(QPalette::Window, myConf.getColorFromString(myConf.get(ConfXML::AREA_PLANNER, "colDrive")));
    ui->widgetDrive->setPalette(pal);

    connect(ui->pbTherapy, &QPushButton::clicked, this, &ConfPlannerDisplay::slotColor);
    connect(ui->pbCourse, &QPushButton::clicked, this, &ConfPlannerDisplay::slotColor);
    connect(ui->pbMeeting, &QPushButton::clicked, this, &ConfPlannerDisplay::slotColor);
    connect(ui->pbTraining, &QPushButton::clicked, this, &ConfPlannerDisplay::slotColor);
    connect(ui->pbHoliday, &QPushButton::clicked, this, &ConfPlannerDisplay::slotColor);
    connect(ui->pbCorpHoliday, &QPushButton::clicked, this, &ConfPlannerDisplay::slotColor);
    connect(ui->pbVisit, &QPushButton::clicked, this, &ConfPlannerDisplay::slotColor);
    connect(ui->pbBreak, &QPushButton::clicked, this, &ConfPlannerDisplay::slotColor);
    connect(ui->pbOther, &QPushButton::clicked, this, &ConfPlannerDisplay::slotColor);
    connect(ui->pbDrive, &QPushButton::clicked, this, &ConfPlannerDisplay::slotColor);
}


bool ConfPlannerDisplay::write()
{
    myConf.set(ConfXML::AREA_PLANNER, "work_start", QString::number(ui->spFrom->value()));
    myConf.set(ConfXML::AREA_PLANNER, "work_end", QString::number(ui->spTo->value()));
    myConf.set(ConfXML::AREA_PLANNER, "work_days", QString::number(ui->spWeekDays->value()));
    myConf.set(ConfXML::AREA_PLANNER, "work_interval", QString::number(ui->spWorkInterval->value()));
    myConf.set(ConfXML::AREA_PLANNER, "maxresdisplay", QString::number(ui->spNumResDisplay->value()));
    if (ui->cbView->currentIndex() >= 0) {
        myConf.set(ConfXML::AREA_PLANNER, "planview", QString::number(ui->cbView->currentIndex()));
    } else {
        myConf.set(ConfXML::AREA_PLANNER, "planview", "0");
    }
    if (ui->cbShowStartEnd->isChecked()) myConf.set(ConfXML::AREA_PLANNER, "render_start_end", "1");
    else myConf.set(ConfXML::AREA_PLANNER, "render_start_end", "0");
    if (ui->cbShowActivities->isChecked()) myConf.set(ConfXML::AREA_PLANNER, "render_activities", "1");
    else myConf.set(ConfXML::AREA_PLANNER, "render_activities", "0");
    if (ui->cbStartPlanner->isChecked()) myConf.set(ConfXML::AREA_PLANNER, "planstart", "1");
    else myConf.set(ConfXML::AREA_PLANNER, "planstart", "0");
    if (ui->cbShowFullName->isChecked()) myConf.set(ConfXML::AREA_PLANNER, "render_fullname", "1");
    else myConf.set(ConfXML::AREA_PLANNER, "render_fullname", "0");

    myConf.set(ConfXML::AREA_PLANNER, "colTherapy", myConf.getStringFromColor(ui->widgetTherapy->palette().color(QPalette::Window)));
    myConf.set(ConfXML::AREA_PLANNER, "colCourse", myConf.getStringFromColor(ui->widgetCourse->palette().color(QPalette::Window)));
    myConf.set(ConfXML::AREA_PLANNER, "colMeeting", myConf.getStringFromColor(ui->widgetMeeting->palette().color(QPalette::Window)));
    myConf.set(ConfXML::AREA_PLANNER, "colTraining", myConf.getStringFromColor(ui->widgetTraining->palette().color(QPalette::Window)));
    myConf.set(ConfXML::AREA_PLANNER, "colHoliday", myConf.getStringFromColor(ui->widgetHoliday->palette().color(QPalette::Window)));
    myConf.set(ConfXML::AREA_PLANNER, "colCorpHoliday", myConf.getStringFromColor(ui->widgetCorpHoliday->palette().color(QPalette::Window)));
    myConf.set(ConfXML::AREA_PLANNER, "colVisit", myConf.getStringFromColor(ui->widgetVisit->palette().color(QPalette::Window)));
    myConf.set(ConfXML::AREA_PLANNER, "colBreak", myConf.getStringFromColor(ui->widgetBreak->palette().color(QPalette::Window)));
    myConf.set(ConfXML::AREA_PLANNER, "colOther", myConf.getStringFromColor(ui->widgetOther->palette().color(QPalette::Window)));
    myConf.set(ConfXML::AREA_PLANNER, "colDrive", myConf.getStringFromColor(ui->widgetDrive->palette().color(QPalette::Window)));

    return true;
}

void ConfPlannerDisplay::slotColor()
{
    QString s = this->sender()->objectName();
    // cut of "pb" from button name and add "widget" as prefix
    // (e.g. pbRunning => widgetRunning)
    s = "widget" + s.right(s.length() - 2);
    QWidget *w = this->findChild<QWidget *>(s);
    if (w == NULL) return;

    QColorDialog dlg;
    dlg.setCurrentColor(w->palette().color(QPalette::Window));
    if (dlg.exec() == QDialog::Accepted) {
        QPalette pal = w->palette();
        pal.setColor(QPalette::Window, dlg.selectedColor());
        w->setPalette(pal);
    }
}
