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

#ifndef BROWSEDOCS_H
#define BROWSEDOCS_H

#include <QtWidgets>
#include "browseinvoices.h"
#include "browsepinvoices.h"
#include "browsequittances.h"
#include "browsereminders.h"
#include "browsereports.h"
#include "browsefindings.h"
#include "browsedoctemplates.h"


namespace Ui {
    class BrowseDocs;
}

class BrowseDocs : public QWidget
{
//    Q_OBJECT
    Q_DISABLE_COPY(BrowseDocs)
public:
    enum modes { MODE_FULL, MODE_RECEIPE, MODE_PATIENT };
    enum mask { MASK_FULL = 255, MASK_INVOICES = 1, MASK_REMINDERS = 2, MASK_QUITTANCES = 4, MASK_REPORTS = 8,
            MASK_FINDINGS = 16, MASK_TEMPLATES = 32, MASK_PINVOICES = 64 };
    explicit BrowseDocs(QWidget *parent = 0, int mask = MASK_FULL, int fid = -1);
    virtual ~BrowseDocs();
    void setMode(int id);
    void setFilterID(int id);
    void Reload();
    void Preload();

private:
    QList<QTreeWidgetItem *> items;
    int curView;
    int mode;
    //TableFilter tf;
    int filter_id;
    QStringList qslNames, qslIcons;
    QList<int> listMask;
    Ui::BrowseDocs *m_ui;
    BrowseInvoices *browseInvoices;
    BrowsePraxisInvoices *browsePraxisInvoices;
    BrowseQuittances *browseQuittances;
    BrowseReminders *browseReminders;
    BrowseReports *browseReports;
    BrowseFindings *browseFindings;
    BrowseDocTemplates *browseTemplates;
    QWidget *last_inserted_widget;
    void removeCurrentView();
    void loadDocs(int d);

private slots:
    void slotDocSelected(QTreeWidgetItem *w);
};

#endif // BROWSEDOCS_H
