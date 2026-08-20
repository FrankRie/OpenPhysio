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

//
// TODO: make templates depend on surgery
//

#include "dlgdoceditor.h"
#include "ui_dlgdoceditor.h"
#include "dlgdoceditdata.h"
#include "dlgdoceditnew.h"
#include "dlgdocopen.h"
#include "dlgdocsave.h"
#include "msgbox.h"


//
// EditorView
//
EditorView::EditorView(QWidget *parent)
    : QGraphicsView(parent)
{
}

EditorView::~EditorView()
{
}

QRectF EditorView::visibleRect()
{
    QPointF tl(horizontalScrollBar()->value(), verticalScrollBar()->value());
    QPointF br = tl + viewport()->rect().bottomRight();
    QTransform m = transform().inverted();
    return m.mapRect(QRectF(tl,br));
}



//
// DlgDocEditor
//
DlgDocEditor::DlgDocEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgDocEditor)
{
    ui->setupUi(this);
    orgDlgTitle = windowTitle();

    listFactors << 0.25 << 0.5 << 0.75 << 1.0 << 1.25 << 1.5 << 2.0 << 4.0;
    docTemplate = new cDocTemplate();
    doc = new cDoc(docTemplate);
    docTemplate->setDoc(doc);
    docTemplate->setMode(cDocTemplate::MODE_EDIT);
    docTemplate->editor = this;

    view = new EditorView(this);
    QGridLayout *layout = (QGridLayout*)this->layout();
    if (layout) layout->addWidget(view, 1, 0, 1, 1);
    view->setScene(doc->scene);
    view->setEnabled(false);

    setCurrentColor(docTemplate->curColor);

    ui->fontComboBox->setFontFilters(QFontComboBox::AllFonts);
    ui->fontComboBox->setCurrentFont(docTemplate->curFont);
    ui->spFontSize->setValue(docTemplate->curFont.pointSize());

    connect(ui->btnExit, &QPushButton::clicked, this, &DlgDocEditor::slotExit);
    connect(ui->btnNew, &QPushButton::clicked, this, &DlgDocEditor::slotNew);
    connect(ui->btnOpen, &QPushButton::clicked, this, &DlgDocEditor::slotOpen);
    connect(ui->btnImport, &QPushButton::clicked, this, &DlgDocEditor::slotImportFile);
    connect(ui->btnExport, &QPushButton::clicked, this, &DlgDocEditor::slotExportFile);
    connect(ui->btnSave, &QPushButton::clicked, this, &DlgDocEditor::slotSave);
    connect(ui->btnText, &QPushButton::clicked, this, &DlgDocEditor::slotText);
    connect(ui->btnPicture, &QPushButton::clicked, this, &DlgDocEditor::slotPicture);
    connect(ui->btnDataField, &QPushButton::clicked, this, &DlgDocEditor::slotDataField);
    connect(ui->btnLine, &QPushButton::clicked, this, &DlgDocEditor::slotDrawLine);
    connect(ui->fontComboBox, &QFontComboBox::currentFontChanged, this, &DlgDocEditor::slotFontChanged);
    connect(ui->spFontSize, &QSpinBox::valueChanged, this, &DlgDocEditor::slotFontSizeChange);
    connect(ui->btnFontBold, &QPushButton::clicked, this, &DlgDocEditor::slotFontBold);
    connect(ui->btnFontItalic, &QPushButton::clicked, this, &DlgDocEditor::slotFontItalic);
    connect(ui->btnFontUnderline, &QPushButton::clicked, this, &DlgDocEditor::slotFontUnderline);
    connect(ui->btnColor, &QPushButton::clicked, this, &DlgDocEditor::slotColor);
    connect(ui->btnPrint, &QPushButton::clicked, this, &DlgDocEditor::slotPrint);

    ui->cbZoom->setCurrentIndex(cDoc::ZOOM_100);
    connect(ui->cbZoom, &QComboBox::currentIndexChanged, this, &DlgDocEditor::slotZoom);

    installEventFilter(this);
}


DlgDocEditor::~DlgDocEditor()
{
    delete doc;
    delete ui;
}


bool DlgDocEditor::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *ke = static_cast<QKeyEvent *>(event);
        if (ke->key() == Qt::Key_F1 || ke->key() == Qt::Key_Help) {
            //helpWrapper.showHelpForKeyword("openphysio_doceditor", true);
            return true;
        }
    }
    return QDialog::eventFilter(obj, event);
}


QFont DlgDocEditor::fontDialog(QFont f)
{
    QFontDialog dlg;
    dlg.setCurrentFont(f);
    if (dlg.exec() == QDialog::Accepted) {
        ui->fontComboBox->setCurrentFont(dlg.selectedFont());
        docTemplate->curFont = dlg.selectedFont();
        ui->spFontSize->setValue(docTemplate->curFont.pointSize());
        updateFontButtons();
        return dlg.selectedFont();
    } else return docTemplate->curFont;
}


void DlgDocEditor::prepareView()
{
    doc->deleteElements();
    doc->createPageRect();
    view->setEnabled(true);
}


void DlgDocEditor::slotExit()
{
    bool bRet = true;
    if (view->isEnabled()) {
        MsgBox box;
        if (box.execYesNoBox("Wollen Sie die aktuelle Vorlage speichern?", BTN_YES) == BTN_YES) {
            slotSave();
        }
    }
    if (bRet) this->accept();
}


void DlgDocEditor::slotNew()
{
    if (view->isEnabled()) {
        MsgBox box;
        if (box.execYesNoBox("Wollen Sie die aktuelle Vorlage speichern?", BTN_YES) == BTN_YES) {
            slotSave();
        }
    }

    doc->deleteElements();
    view->setEnabled(false);

    DlgDocEditNew dlg;
    dlg.init(docTemplate->getTemplateNames());
    docTemplate->curTemplate.setID(-1);
    if (dlg.exec() != QDialog::Accepted) {
        docTemplate->curTemplate.setName("");
        updateDialogTitle();
        return;
    }
    docTemplate->curTemplate.setTemplateType(docTemplate->getTemplateID(dlg.getSelectedDocType()));
    docTemplate->curTemplate.setName(dlg.getName());
    docTemplate->default_height=docTemplate->height_array[dlg.getSelectedPageSize()];
    docTemplate->default_width=docTemplate->width_array[dlg.getSelectedPageSize()];
    updateDialogTitle();
    doc->filename = "";

    // set default page size
    doc->width = docTemplate->default_width;
    doc->height = docTemplate->default_height;
    prepareView();
    doc->flag_change = false;
}


void DlgDocEditor::slotOpen()
{
    DlgDocOpen *dlg = new DlgDocOpen(this);
    if (dlg->exec() == QDialog::Accepted) {
        cTemplate c = dlg->getSelectedTemplate();
        if (c.getID() < 0) return;
        docTemplate->curTemplate = c;
        doc->deleteElements();
        //prepareView();
        docTemplate->readTemplateFromByteArray(docTemplate->curTemplate.getContents());
        //docTemplate->default_width = doc->width;
        //docTemplate->default_height = doc->height;
        updateDialogTitle();
        view->setEnabled(true);
    }
    delete dlg;
}


void DlgDocEditor::slotImportFile()
{
    if (view->isEnabled()) {
        MsgBox box;
        if (box.execYesNoBox("Wollen Sie die aktuelle Vorlage speichern?", BTN_YES) == BTN_YES) {
            slotSave();
        }

    }

    QString filename = QFileDialog::getOpenFileName(this, QString::fromUtf8("Datei öffnen"),
                                                    "", tr("OpenPhysio Dokumentenvorlage (*.odv);;Vorlage (*.txt)"));
    if (!filename.isEmpty()) {
        doc->filename = filename;
        docTemplate->curTemplate.setID(-1);
        if (!docTemplate->readTemplateFromFile(filename)) {
            // TODO: error handling
        }
        prepareView();
        docTemplate->readTemplateFromFile(filename);
        updateDialogTitle();
    }
}


void DlgDocEditor::slotExportFile()
{
    if (!view->isEnabled()) return;
    QFileDialog dlg(this);
    dlg.setAcceptMode(QFileDialog::AcceptSave);
    dlg.setNameFilter(tr("OpenPhysio Dokumentenvorlage (*.odv)"));
    dlg.setDefaultSuffix("odv");
    if (dlg.exec() == QDialog::Accepted) {
        QStringList fileNames = dlg.selectedFiles();
        if (fileNames.count() > 0) {
            doc->filename = fileNames.at(0);
            // TODO: Ask user for a (different) template name before start saving
            docTemplate->saveTemplate(true);
            // TODO: add error handling
        }
    }
}


bool DlgDocEditor::slotSave()
{
    if (!view->isEnabled()) return false;

    DlgDocSave dlg;
    QString old_name = docTemplate->curTemplate.getName();
    dlg.init(old_name);
    if (dlg.exec() == QDialog::Accepted) {
        QString new_name = dlg.getTemplateName();
        if (!old_name.isEmpty() && old_name != new_name) {
            // user changed the name of an existing template
            // => we need to indicate that this item will be saved as a new entry in the database
            docTemplate->curTemplate.setID(-1);
        }
        docTemplate->curTemplate.setName(new_name);
        updateDialogTitle();
        return docTemplate->saveTemplate(false);
    } else return false;
}


void DlgDocEditor::setCurrentColor(QColor c)
{
    docTemplate->curColor = c;
    QPalette pal;
    pal = ui->widgetColor->palette();
    pal.setColor(QPalette::Window, docTemplate->curColor);
    ui->widgetColor->setPalette(pal);
}



void DlgDocEditor::slotText()
{
    if (!view->isEnabled()) return;
    QRectF r = view->visibleRect();
    cEditorTextElement* te = doc->createTextItem(r.x(), r.y(), "Textfeld", docTemplate->curFont, docTemplate->curColor);
    connect(te, &cEditorTextElement::sigSelectTextItem, this, &DlgDocEditor::slotTextItemSelected);
    doc->flag_change = true;
}


void DlgDocEditor::slotPicture()
{
    if (!view->isEnabled()) return;
    QString filename = QFileDialog::getOpenFileName(this, QString::fromUtf8("Bild öffnen"),
                                                    "", tr("Bild (*.jpg *.png);;Vektorgrafik (*.svg)"));
    if (!filename.isEmpty()) {
        QRectF r = view->visibleRect();
        if (filename.contains(".svg")) {
            doc->createSvg(r.x(), r.y(), filename);
        } else {
            doc->createPixmap(0, 0, filename);
        }
        doc->flag_change = true;
    }

}


void DlgDocEditor::slotDrawLine()
{
    if (!view->isEnabled()) return;
    QMessageBox::information(this, "OpenPhysio", "Funktion noch nicht implementiert!", QMessageBox::Ok);
}


void DlgDocEditor::slotDataField()
{
    if (!view->isEnabled()) return;
    DlgDocEditData dlg;
    dlg.init(docTemplate);
    if (dlg.exec() == QDialog::Accepted) {
        QString s = dlg.getSelectedDataField();
        QRectF r = view->visibleRect();
        int w = docTemplate->default_datafield_width;
        cEditorDataElement* de = doc->createDataItem(r.x(), r.y(), w, 0, docTemplate->getShortName(s), s, docTemplate->curFont, docTemplate->curColor);
        connect(de, &cEditorDataElement::sigSelectDataItem, this, &DlgDocEditor::slotTextItemSelected);
    }
}


void DlgDocEditor::slotFontChanged(QFont f)
{
    docTemplate->curFont.setFamily(f.family());
    doc->updateSelectedTextElements();
}


void DlgDocEditor::slotFontSizeChange(int i)
{
    docTemplate->curFont.setPointSize(i);
    doc->updateSelectedTextElements();
}


void DlgDocEditor::slotFontBold()
{
    docTemplate->curFont.setBold(!docTemplate->curFont.bold());
    updateFontButtons();
    doc->updateSelectedTextElements();
}


void DlgDocEditor::slotFontItalic()
{
    docTemplate->curFont.setItalic(!docTemplate->curFont.italic());
    updateFontButtons();
    doc->updateSelectedTextElements();
}


void DlgDocEditor::slotFontUnderline()
{
    docTemplate->curFont.setUnderline(!docTemplate->curFont.underline());
    updateFontButtons();
    doc->updateSelectedTextElements();
}


void DlgDocEditor::slotZoom(int i)
{
    if (i < 0) return;
    if (i < listFactors.count()) doc->factor = listFactors.at(i);
    else if (i == cDoc::ZOOM_HEIGHT) doc->factor = (view->height() - 10) / doc->scene->height();
    else if (i == cDoc::ZOOM_WIDTH) doc->factor = (view->width() - 20) / doc->scene->width();
    else doc->factor = 1;

    view->setTransform(QTransform());
    view->scale(doc->factor, doc->factor);
}


void DlgDocEditor::updateFontButtons()
{
    if (docTemplate->curFont.bold()) ui->btnFontBold->setIcon(QIcon(":/docedit/images/bold_enabled.png"));
    else ui->btnFontBold->setIcon(QIcon(":/docedit/images/bold.png"));
    if (docTemplate->curFont.italic()) ui->btnFontItalic->setIcon(QIcon(":/docedit/images/italic_enabled.png"));
    else ui->btnFontItalic->setIcon(QIcon(":/docedit/images/italic.png"));
    if (docTemplate->curFont.underline()) ui->btnFontUnderline->setIcon(QIcon(":/docedit/images/underline_enabled.png"));
    else ui->btnFontUnderline->setIcon(QIcon(":/docedit/images/underline.png"));
}


void DlgDocEditor::slotColor()
{
    QColorDialog dlg;
    dlg.setCurrentColor(docTemplate->curColor);
    if (dlg.exec() == QDialog::Accepted) {
        setCurrentColor(dlg.selectedColor());
        doc->updateSelectedTextElements();
    }

}


void DlgDocEditor::slotPrint()
{
    if (!view->isEnabled()) return;

    QPrinter printer(QPrinter::ScreenResolution);
    printer.setPageSize(QPageSize(QPageSize::A4));
    if (doc->height > doc->scene->width()) printer.setPageOrientation(QPageLayout::Portrait);
    else printer.setPageOrientation(QPageLayout::Landscape);
    QPrintDialog *dialog = new QPrintDialog(&printer, this);
    dialog->setWindowTitle("Drucken");
    dialog->setOption(QAbstractPrintDialog::PrintSelection, true);
    if (dialog->exec() != QDialog::Accepted) return;

    //doc->docRect->hide();
    QPainter painter(&printer);
    doc->scene->render(&painter, doc->scene->sceneRect(), doc->scene->sceneRect(), Qt::KeepAspectRatio);
    if (dialog != NULL) delete dialog;
    //doc->docRect->show();
}


void DlgDocEditor::slotTextItemSelected(QFont f, QColor c)
{
    // if a text item was selected use this font a the default
    disconnect(ui->fontComboBox, &QFontComboBox::currentFontChanged, this, &DlgDocEditor::slotFontChanged);
    disconnect(ui->spFontSize, &QSpinBox::valueChanged, this, &DlgDocEditor::slotFontSizeChange);
    docTemplate->curFont = f;
    ui->fontComboBox->setCurrentFont(f);
    ui->spFontSize->setValue(f.pointSize());
    setCurrentColor(c);
    updateFontButtons();
    connect(ui->spFontSize, &QSpinBox::valueChanged, this, &DlgDocEditor::slotFontSizeChange);
    connect(ui->fontComboBox, &QFontComboBox::currentFontChanged, this, &DlgDocEditor::slotFontChanged);
}


void DlgDocEditor::slotLineItemSelected(QColor c)
{
    setCurrentColor(c);
}


void DlgDocEditor::updateDialogTitle()
{
    QString s = docTemplate->curTemplate.getName();
    if (s.isEmpty()) {
        setWindowTitle(orgDlgTitle + " (*neue Vorlage*)");
    } else {
        setWindowTitle(orgDlgTitle + " (" + s + ")");
    }
}
