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

#ifndef DLGDOCVIEWER_H
#define DLGDOCVIEWER_H

#include <QtGui>

#include "src_docedit/doctools.h"


namespace Ui {
    class DlgDocViewer;
}

class DlgDocViewer : public QDialog {
    Q_OBJECT
    Q_DISABLE_COPY(DlgDocViewer)
public:
    explicit DlgDocViewer(QWidget *parent = 0);
    virtual ~DlgDocViewer();
    void addDoc(cDoc *doc);
    void setChangeTrackingEnabled(bool b);
    void setSavingEnabled(bool b);
    void prepare();
    void setPrintScale(bool b);
    void setPrinterMode(int prn);

private:
    Ui::DlgDocViewer *m_ui;
    QList<cDoc*> listDocs;
    QList<qreal> listFactors;
    QList<cInvoice> inv;
    QList<QGraphicsScene*> scenes;
    bool change_tracking;
    bool saving_enabled;
    bool print_scale;
    int curPage;
    int prn_mode;

    void setPageLabel();
    bool changed();
    QString transliterateUmlauts(const QString& input);
    QByteArray toAsciiRfc5322(const QString& input);
    void generatePdf();

protected:
    void resizeEvent(QResizeEvent* event);
    bool eventFilter(QObject *obj, QEvent *event);

public slots:
    virtual void reject();

private slots:
    void slotCancel();
    void slotPrint();
    void slotPDF();
    void slotMail();
    void slotChange();
    void slotSave();
    void slotZoomIn();
    void slotZoomOut();
    void slotFactor(int i);
    void slotPrevPage();
    void slotNextPage();
    void slotMarkField(bool b);
};

#endif // DLGDOCVIEWER_H
