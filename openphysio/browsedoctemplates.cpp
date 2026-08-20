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

#include "browsedoctemplates.h"
#include "config.h"
#include "mydb.h"
#include "gendoc.h"
#include "dlgbrowse.h"
#include "browsecourses.h"


DocTemplateItem::DocTemplateItem(QTreeWidgetItem *parent)
	: QTreeWidgetItem(parent, QTreeWidgetItem::UserType)
{
    doc_id = -1;
}


DocTemplateItem::~DocTemplateItem()
{
}


void DocTemplateItem::setDocTemplateID(int id)
{
    doc_id = id;
}


int DocTemplateItem::getDocTemplateID()
{
    return doc_id;
}


void DocTemplateItem::enableExtendedOptions(bool b)
{
    extended_options = b;
}


bool DocTemplateItem::enableExtendedOptions()
{
    return extended_options;
}


BrowseDocTemplates::BrowseDocTemplates(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    QStringList qsl;
    qsl << "Berichte" << "Rechnungen und Quittungen" << "Sonstige Vordrucke";
    for (int x = 0; x < ROOT_ELEM_COUNT; x++) {
        rootItems.append(new QTreeWidgetItem(ui.treeTemplates));
        rootItems.at(x)->setExpanded(true);
        rootItems.at(x)->setText(0, qsl.at(x));
    }
    ui.treeTemplates->setIconSize(QSize(32, 32));
    docIcons.append(new QIcon(":/report.png"));
    docIcons.append(new QIcon(":/invoice.png"));
    docIcons.append(new QIcon(":/template.png"));

    addDocTemplate(DOC_REPORTS, /* QString::fromStdString */("Therapiebericht"), cTemplateType::TT_REPORT);
    addDocTemplate(DOC_INVOICES, /* QString::fromStdString */("Rechnung"), cTemplateType::TT_INVOICE);
    addDocTemplate(DOC_INVOICES, /* QString::fromStdString */("Praxisrechnung"), cTemplateType::TT_INVOICE);
    addDocTemplate(DOC_INVOICES, /* QString::fromStdString */("Mahnung"), cTemplateType::TT_REMINDER);
    addDocTemplate(DOC_INVOICES, /* QString::fromStdString */("Praxisanhang"), cTemplateType::TT_INVOICE);//TT_PAPPENDIX);
    addDocTemplate(DOC_INVOICES, /* QString::fromStdString */("Ausfallgebühr"), cTemplateType::TT_QUITTANCE);
    //addDocTemplate(DOC_INVOICES, /* QString::fromStdString */("Zuzahlungsquittung"), ConfXML::DOC_QUITTANCE_ADDPAY);
    //addDocTemplate(DOC_PREPRINTS, /* QString::fromStdString */("Patientenanmeldung"), ConfXML::);
    //addDocTemplate(DOC_PREPRINTS, /* QString::fromStdString */("Honorarvereinbarung"), ConfXML::);
    //addDocTemplate(DOC_PREPRINTS, /* QString::fromStdString */("Patienteninformation"), ConfXML::);
    addDocTemplate(DOC_PREPRINTS, /* QString::fromStdString */("Teilnehmerliste"), cTemplateType::TT_PARTLIST, true);
    addDocTemplate(DOC_PREPRINTS, /* QString::fromStdString */("Teilnahmebestätigung"), cTemplateType::TT_PARTICIPANCE);
    addDocTemplate(DOC_PREPRINTS, /* QString::fromStdString */("Rezeptkorrektur"), cTemplateType::TT_CORRECTION);

    connect(ui.treeTemplates, &QTreeWidget::itemDoubleClicked, this, &BrowseDocTemplates::slotDocSelected);
    //connect(ui.treeTemplates, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(slotDocSelected(QTreeWidgetItem *, int)));
}


BrowseDocTemplates::~BrowseDocTemplates()
{
    while (!templates.isEmpty()) delete templates.takeFirst();
    while (!rootItems.isEmpty()) delete rootItems.takeFirst();
    while (!docIcons.isEmpty()) delete docIcons.takeFirst();
}


void BrowseDocTemplates::addDocTemplate(int category, QString tname, int t_id, bool ext_opt)
{
    if ((category < 0) || (category >= ROOT_ELEM_COUNT)) return;
    templates.append(new DocTemplateItem(rootItems.at(category)));
    DocTemplateItem *dti = templates.last();
    dti->setIcon(0, *(docIcons.at(category)));
    dti->setText(0, tname);
    dti->setDocTemplateID(t_id);
    dti->enableExtendedOptions(ext_opt);
}


void BrowseDocTemplates::slotDocSelected(QTreeWidgetItem *w, int column)
{
    if ( (w == NULL) || (column != 0)) return;
    DocTemplateItem *item = (DocTemplateItem*)w;

    curTemplate = item->getDocTemplateID();
    if (item->enableExtendedOptions()) {
        QDialog *dlg = new QDialog();
        dlg->setWindowIcon(QIcon(":/favicon.png"));
        dlg->setWindowTitle("Vorlage drucken: " + item->text(0));
        QGridLayout *layout = new QGridLayout(dlg);
        layout->setContentsMargins(0,0,0,0);

        QWidget *widget_content = new QWidget(dlg);
        QGridLayout *content_layout = new QGridLayout(widget_content);

        switch(curTemplate) {
            case cTemplateType::TT_PARTLIST: setupPartList(widget_content, content_layout); break;
            default: break;
        }

        for (int i = 0; i < date_edits.count(); i++) {
            date_edits.at(i)->setCalendarPopup(true);
            date_edits.at(i)->calendarWidget()->setFirstDayOfWeek(Qt::Monday);
            date_edits.at(i)->calendarWidget()->setGridVisible(true);
        }

        layout->addWidget(widget_content, 0, 0);

        QWidget *widget_button = new QWidget(dlg);
        QGridLayout *widget_layout = new QGridLayout(widget_button);

        QPushButton *button_cancel = new QPushButton("Abbrechen", widget_button);
        button_cancel->setIcon(QIcon(":/cancel.png"));
        QPushButton *button_create = new QPushButton("Erzeugen", widget_button);
        button_create->setIcon(QIcon(":/template.png"));
        widget_layout->addWidget(button_cancel, 0, 0);
        widget_layout->addWidget(button_create, 0, 1);
        button_create->setAutoDefault(true);
        QObject::connect(button_create, &QPushButton::clicked, dlg, &QDialog::accept);
        QObject::connect(button_cancel, &QPushButton::clicked, dlg, &QDialog::reject);
        layout->addWidget(widget_button, 1, 0);

        dlg->exec();
        if (dlg->result() == QDialog::Accepted) {
            switch(curTemplate) {
                case cTemplateType::TT_PARTLIST: finalizePartList(); break;
                default: break;
            }
        }

        delete dlg;

        // we need to clear the widget lists because the widget objects seem to be deleted
        // when the dialog is deleted => invalid pointers would remain in widget lists
        labels.clear();
        edits.clear();
        date_edits.clear();
        buttons.clear();
        boxes.clear();
    } else {
        gendoc.genEmptyTemplate(curTemplate, item->text(0));
    }
}


void BrowseDocTemplates::setupPartList(QWidget *parent, QGridLayout *layout)
{
    labels.append(new QLabel("Kursname", parent));
    labels.append(new QLabel("Kursleiter", parent));
    labels.append(new QLabel("Kursbeginn", parent));
    edits.append(new QLineEdit(parent));
    date_edits.append(new QDateEdit(QDate::currentDate(), parent));
    buttons.append(new QPushButton("..", parent)); // course name
    buttons.last()->setMaximumSize(20, 20);

    connect(buttons.last(), &QPushButton::clicked, this, &BrowseDocTemplates::slotSelectCourse);
    //   connect(buttons.last(), &QPushButton::clicked, this, SLOT(slotSelectCourse()));
    boxes.append(new QComboBox(parent));         // course leader
    for (int i = 0; i < db.listEmpl.count(); i++) {
        if (db.listEmpl.at(i)->getGroupID() == cEmployeeGroup::GROUP_THERAPY) {
            boxes.last()->addItem(db.listEmpl.at(i)->getFullName());
        }
    }
    boxes.last()->setCurrentIndex(-1);
    layout->addWidget(labels.at(0), 0, 0);
    layout->addWidget(labels.at(1), 1, 0);
    layout->addWidget(labels.at(2), 2, 0);
    layout->addWidget(edits.at(0), 0, 1);
    layout->addWidget(boxes.at(0), 1, 1);
    layout->addWidget(date_edits.at(0), 2, 1);
    layout->addWidget(buttons.at(0), 0, 2);
}


void BrowseDocTemplates::finalizePartList()
{
    cCourse course;
    QList<cPatient*> patients;
    course.setName(edits.at(0)->text());
    course.setStartDate(date_edits.at(0)->date());
    gendoc.genPartList(course, patients, boxes.at(0)->currentText());
}


void BrowseDocTemplates::slotSelectCourse()
{
    if (curTemplate == cTemplateType::TT_PARTLIST) {
        BrowseCourses *browseCourses = new BrowseCourses(Browse::MODE_SELECT);
        if (myConf.usePreload()) browseCourses->preload();
        DlgBrowse browse(browseCourses);
        if (browse.exec() == QDialog::Accepted) {
            cCourse course = browseCourses->getSelCourses();
            if (course.getID() > 0) {
                edits.at(0)->setText(course.getName());
                date_edits.at(0)->setDate(course.getStartDate());
                QString strLeader;
                cEmployee empl;
                if (db.retrieveItem(empl, course.getLeaderID())) strLeader = empl.getFullName();
                if (!strLeader.isEmpty()) {
                    for (int i = 0; i < boxes.at(0)->count(); i++) {
                        if (boxes.at(0)->itemText(i) == empl.getFullName()) {
                            boxes.at(0)->setCurrentIndex(i);
                            break;
                        }
                    }
                }
            }
        }
        delete browseCourses;
    }
}

