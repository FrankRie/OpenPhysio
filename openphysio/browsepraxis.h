#ifndef BROWSEPRAXIS_H
#define BROWSEPRAXIS_H

#include <QtGui>
#include "ui_browsepraxis.h"
#include "browse.h"
#include "dlgpraxis.h"


class BrowsePraxis : public Browse
{
    Q_OBJECT
public:
    BrowsePraxis(int mode = MODE_FULL, QWidget *parent = 0);
    ~BrowsePraxis();
    void showDlg(cPraxis *p);
    void setupDefaultButtons();
    cPraxis& getSelPraxis();

private:
    Ui::BrowsePraxisClass ui;
    QAction *actPraxisAdd, *actPraxisEdit, *actPraxisDel;
    cPraxis selPraxis;

public slots:
    void slotPraxisAdd();

private slots:
    void slotPraxisSelected();
    void slotPraxisEdit();
    void slotPraxisDel();

};
#endif // BROWSEPRAXIS_H
