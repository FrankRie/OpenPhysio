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

#ifndef CONFTAKEOVER_H
#define CONFTAKEOVER_H

#include "confbase.h"

namespace Ui {
    class ConfTakeover;
}

class ConfTakeover : public ConfBase {
    Q_OBJECT
    Q_DISABLE_COPY(ConfTakeover)
public:
    explicit ConfTakeover(QWidget *parent = 0, int l = -1, int a = -1);
    virtual ~ConfTakeover();
    void init();
    bool write();

private:
    Ui::ConfTakeover *m_ui;
    enum progs { PRG_SP = 0 };
    enum patient_items {PAT_SURNAME = 0, PAT_FORENAME, PAT_ADDNAME, PAT_TITLE, PAT_BIRTHDAY, PAT_GENDER,
			PAT_STREET, PAT_CITY, PAT_ZIP, PAT_TEL, PAT_TELMOBILE, PAT_FAX, PAT_EMAIL, PAT_INS,
			PAT_VNR, PAT_KVKVALID, PAT_STATUS, PAT_NOTES,
			PAT_ADDR_REF, PAT_ATTR_NUMBER};
    enum addr_items { ADDR_REF = 0, ADDR_CITY, ADDR_STREET, ADDR_ZIP, ADDR_ATTR_NUMBER };
    enum phys_items { PHYS_SURNAME, PHYS_FORENAME, PHYS_TITLE, PHYS_GENDER, PHYS_TEL,
		      PHYS_ADDR_REF, PHYS_ATTR_NUMBER };
    bool handle_sp(QString dir);

private slots:
    void slotStart();
    void slotSelectDirectory();
};

#endif // CONFTAKEOVER_H
