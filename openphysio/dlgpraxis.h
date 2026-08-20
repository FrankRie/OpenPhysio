#ifndef DLGPRAXIS_H
#define DLGPRAXIS_H

#include <QtWidgets>
#include <QMessageBox>
#include "ui_dlgpraxis.h"
#include "mydb.h"


class DlgPraxis : public QDialog
{
    Q_OBJECT

public:
    DlgPraxis(QWidget *parent = 0);
    ~DlgPraxis();
    void initPraxis(cPraxis *p);

private:
    Ui::DlgPraxisClass ui;
    cPraxis curPraxis;
    bool flag_change;
    bool savePraxis();
    void checkInput();

private slots:
    void slotSavePraxis();
    void slotCancelPraxis();
    void slotModified();
    void slotModifiedWithCheck();
    void slotReadEID();

public slots:
    void reject();

};

#endif // DLGPRAXIS_H
