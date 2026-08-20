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

#ifndef DLGDOCEDITOR_H
#define DLGDOCEDITOR_H

#include <QtSvg>
#include <QtWidgets>
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPrintDialog>


#include "doctools.h"


namespace Ui {
    class DlgDocEditor;
}


//
// EditorView
//
class EditorView : public QGraphicsView
{
    Q_OBJECT

public:
    EditorView(QWidget *parent = 0);
    ~EditorView();
    QRectF visibleRect();
};



//
// DlgDocEditor
//
class DlgDocEditor : public QDialog
{
    Q_OBJECT

public:
    explicit DlgDocEditor(QWidget *parent = 0);
    ~DlgDocEditor();
    QFont fontDialog(QFont f);

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private:
    enum buttons {BTN_YES, BTN_NO};

    Ui::DlgDocEditor *ui;
    cDocTemplate *docTemplate;
    cDoc *doc;
    EditorView *view;
    QList<qreal> listFactors;
    QString orgDlgTitle;

    void prepareView();
    void updateFontButtons();
    void setCurrentColor(QColor c);
    void updateDialogTitle();

private slots:
    void slotExit();
    void slotNew();
    void slotOpen();
    void slotImportFile();
    void slotExportFile();
    bool slotSave();
    void slotText();
    void slotPicture();
    void slotDrawLine();
    void slotDataField();
    void slotFontChanged(QFont f);
    void slotFontSizeChange(int i);
    void slotFontBold();
    void slotFontItalic();
    void slotFontUnderline();
    void slotZoom(int i);
    void slotColor();
    void slotPrint();
    void slotLineItemSelected(QColor c);

public slots:                // <-- nicht private
    void slotTextItemSelected(QFont f, QColor c);
};

#endif // DLGDOCEDITOR_H
