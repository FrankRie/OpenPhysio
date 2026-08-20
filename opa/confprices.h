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

#ifndef CONFPRICES_H
#define CONFPRICES_H

#include "confbase.h"

namespace Ui {
    class ConfPrices;
}

class ConfPrices : public ConfBase
{
    Q_OBJECT

public:
    explicit ConfPrices(QWidget *parent = 0, int l = - 1, int a = -1);
    ~ConfPrices();
    void init();
    bool write();

private:
    Ui::ConfPrices *ui;
    enum opcodes { OPC_INVALID = 1, OPC_ENTRY, OPC_DESCR, OPC_PROF, OPC_AREA, OPC_ASSOC, OPC_UNION, OPC_VALID,
                   OPC_SIZE };
    QList<cTableItem*> listTariffAreas;
    QList<cTableItem*> listPriceListsOrg;
    QList<cTableItem*> listPriceLists;
    QMap<int, QString> lookupArea;
    QStringList listPriceFiles;
    int getOpcode(QString s);
    QString getError(int opcode);
    int parse_pricelist_file(QString fname, QStringList &statements);
    void reloadPriceLists();
    void loadTable(int i);

private slots:
    void slotPrices();
    void slotSelectPriceFile();
    void slotProfession(int i);
    void slotViewList();
    void slotDelList();
};

#endif // CONFPRICES_H
