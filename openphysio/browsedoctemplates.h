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

#ifndef BROWSEDOCTEMPLATES_H
#define BROWSEDOCTEMPLATES_H

#include <QtCore>
#include <QtWidgets>
#include "ui_browsedoctemplates.h"

class DocTemplateItem : public QTreeWidgetItem
{
private:
    int doc_id;
    bool extended_options;

public:
    DocTemplateItem(QTreeWidgetItem *parent = 0);
    ~DocTemplateItem();
    void setDocTemplateID(int id);
    int getDocTemplateID();
    void enableExtendedOptions(bool b);
    bool enableExtendedOptions();
};


class BrowseDocTemplates : public QWidget
{
    Q_OBJECT
public:
    BrowseDocTemplates(QWidget *parent = 0);
    ~BrowseDocTemplates();

private:
    enum docRoot {DOC_REPORTS = 0, DOC_INVOICES, DOC_PREPRINTS, ROOT_ELEM_COUNT};

    Ui::BrowseDocTemplatesClass ui;
    QList<QTreeWidgetItem *> rootItems;
    QList<DocTemplateItem *> templates;
    QList<QIcon *> docIcons;
    int curTemplate;
    QList<QLineEdit*> edits;
    QList<QLabel*> labels;
    QList<QDateEdit*> date_edits;
    QList<QPushButton*> buttons;
    QList<QComboBox *> boxes;

    void addDocTemplate(int category, QString tname, int t_id, bool ext_opt = false);
    void setupPartList(QWidget *parent, QGridLayout *layout);
    void finalizePartList();

private slots:
    void slotDocSelected(QTreeWidgetItem *w, int column);
    void slotSelectCourse();
};

#endif // BROWSEDOCTEMPLATES_H
