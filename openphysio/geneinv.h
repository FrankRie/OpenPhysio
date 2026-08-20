#ifndef GENEINV_H
#define GENEINV_H

#include <QObject>
#include "config.h"
#include "mydb.h"
#include "dlgdocviewer.h"

class GenEInv: public QObject
{
    Q_OBJECT

public:
    GenEInv();
    ~GenEInv();

    bool genEInvoice(QByteArray& invdat, cPatient *p);

private:
    QString getInvValue(QString muster, QByteArray& invdat);
};

extern GenEInv geneinv;

#endif // GENEINV_H
