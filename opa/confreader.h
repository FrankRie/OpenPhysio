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

#ifndef CONFREADER_H
#define CONFREADER_H

#include "confbase.h"
#include <QtWidgets/QFileDialog>

namespace Ui {
    class ConfReader;
}

class ConfReader : public ConfBase {
    Q_OBJECT
    Q_DISABLE_COPY(ConfReader)
public:
    explicit ConfReader(QWidget *parent = 0, int l = -1, int a = -1);
    virtual ~ConfReader();
    void init();
    bool write();

private:
    Ui::ConfReader *m_ui;
    QList<cTableItem*> list_readers;
    cReaderDevice curReader;

private slots:
    void slotSetupReader(int i);
    void slotSelectLib();
    void slotEditLib();
    void slotTestReader();
    void slotChangeTerminal(int i);
    void slotChangePort(int i);
    void slotChangeOffset(int i);
    void slotChangeAPI(int i);
    void slotUseSelect(bool b);
};

#endif // CONFREADER_H
