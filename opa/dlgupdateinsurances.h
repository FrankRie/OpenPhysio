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

#ifndef DLGUPDATEINSURANCES_H
#define DLGUPDATEINSURANCES_H

#include <QtGui>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QMessageBox>
#include <mydb.h>


namespace Ui {
    class DlgUpdateInsurances;
}


class DlgUpdateInsurances : public QDialog {
    Q_OBJECT
    Q_DISABLE_COPY(DlgUpdateInsurances)
public:
    explicit DlgUpdateInsurances(QWidget *parent = 0);
    virtual ~DlgUpdateInsurances();
    bool init(QStringList list);

private:
    enum states { STATE_INIT = 0, STATE_BEGIN, STATE_END };
    enum opcodes { UNA, UNB, UNH, UNT, UNZ, IDK, VDT, FKT, KTO, VKG, NAM, ANS, ASP, UEM, DFU };
    QString token_separator;
    QString token_segment_end;
    int assoc;
    QList<cTableItem*> ins_existing;
    QList<cInsurance*> ins_history;
    QList<cInsurance*> ins_new;
    Ui::DlgUpdateInsurances *m_ui;
    QStringList fnames;
    bool analyzeFilename(QString s);
    int getOpcode(QString s);
    void parseLine(int opcode, QString &line, cInsurance* ins);
    void merge(cInsurance *ins);
    bool perform(QString fname);

private slots:
    void slotStart();
    void slotCancel();
};

#endif // DLGUPDATEINSURANCES_H
