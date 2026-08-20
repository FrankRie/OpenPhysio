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

#include <QtPrintSupport/QtPrintSupport>
#include <QtWidgets>
#include "dlgdocviewer.h"
#include "protect.h"
#include "ui_dlgdocviewer.h"
#include "config.h"
#include "msgbox.h"
#include "helperwrapper.h"
#include "gendoc.h"
#include "mydb.h"
#include "smtpclient.h"
#include "mimemessage.h"
#include "mimetext.h"
#include "mimeattachment.h"

DlgDocViewer::DlgDocViewer(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::DlgDocViewer)
{
    m_ui->setupUi(this);
    this->setWindowFlags(Qt::Window);

    change_tracking = true;
    saving_enabled = true;
    print_scale = false;

    connect(m_ui->pbCancel, &QPushButton::clicked, this, &DlgDocViewer::slotCancel);
    connect(m_ui->pbPrint, &QPushButton::clicked, this, &DlgDocViewer::slotPrint);
    connect(m_ui->pbPDF, &QPushButton::clicked, this, &DlgDocViewer::slotPDF);
    connect(m_ui->pbMail, &QPushButton::clicked, this, &DlgDocViewer::slotMail);
    connect(m_ui->pbChange, &QPushButton::clicked, this, &DlgDocViewer::slotChange);
    connect(m_ui->pbSave, &QPushButton::clicked, this, &DlgDocViewer::slotSave);
    connect(m_ui->pbZoomIn, &QPushButton::clicked, this, &DlgDocViewer::slotZoomIn);
    connect(m_ui->pbZoomOut, &QPushButton::clicked, this, &DlgDocViewer::slotZoomOut);
    connect(m_ui->pbPrevPage, &QPushButton::clicked, this, &DlgDocViewer::slotPrevPage);
    connect(m_ui->pbNextPage, &QPushButton::clicked, this, &DlgDocViewer::slotNextPage);
    connect(m_ui->cbMarkFields, &QCheckBox::toggled, this, &DlgDocViewer::slotMarkField);

    prn_mode = ConfXML::PRN_DOC;
    curPage = -1;
    listFactors << 0.25 << 0.5 << 0.75 << 1.0 << 1.25 << 1.5 << 2.0 << 4.0;
    connect(m_ui->cbFactor, &QComboBox::currentIndexChanged, this, &DlgDocViewer::slotFactor);
    installEventFilter(this);
}


DlgDocViewer::~DlgDocViewer()
{
    delete m_ui;
}


void DlgDocViewer::addDoc(cDoc *doc)
{
    if (doc == NULL) return;

    listDocs.append(doc);
    QGraphicsScene* s = doc->scene;
    scenes.append(s);
    if (scenes.count() == 1) {
	m_ui->graphicsView->setScene(s);
	curPage = 0;
	// set default zoom
	//if (s->height() > s->width()) m_ui->cbFactor->setCurrentIndex(ZOOM_HEIGHT);
	//else m_ui->cbFactor->setCurrentIndex(ZOOM_WIDTH);
	m_ui->graphicsView->centerOn(0, 0);
        if (s->height() < s->width()) m_ui->cbFactor->setCurrentIndex(cDoc::ZOOM_WIDTH);
        else m_ui->cbFactor->setCurrentIndex(cDoc::ZOOM_100);
    }
    setPageLabel();
}


void DlgDocViewer::reject()
{
    if (changed()) {
	MsgBox box;
        if (box.execYesNoBox(QString::fromUtf8("Sie haben neue Dokumente erzeugt oder Änderungen vorgenommen. Sollen diese gespeichert werden?"), MsgBox::BTN_YES) == MsgBox::BTN_YES) {
	    QDialog::accept();
	} else {
	    QDialog::reject();
	}
    }
    else QDialog::reject();
}


void DlgDocViewer::setSavingEnabled(bool b)
{
    saving_enabled = b;
    m_ui->pbSave->setEnabled(saving_enabled);
}


void DlgDocViewer::setChangeTrackingEnabled(bool b)
{
    change_tracking = b;
}


bool DlgDocViewer::changed()
{
    if (!change_tracking) return false;

    bool flag_change = false;

    for (int i = 0; i < listDocs.count(); i++) {
	cDoc *d = listDocs.at(i);
	if (d->getFlagNew()) {
	    flag_change = true;
	    break;
	}
	if (listDocs.at(i)->changed()) {
	    flag_change = true;
	    break;
	}
    }

    return flag_change;
}


void DlgDocViewer::slotCancel()
{
    this->reject();
}


void DlgDocViewer::setPrintScale(bool b)
{
    print_scale = b;
}


void DlgDocViewer::setPrinterMode(int prn)
{
    prn_mode = prn;
}


void DlgDocViewer::slotPrint()
{
    QGraphicsScene *s = m_ui->graphicsView->scene();
    if (s == NULL) return;
    if (listDocs.count() < 1) return;
    cDoc *doc = listDocs.at(0);
    if (doc == NULL) return;

    QList<QPrinterInfo> listPrinters = QPrinterInfo::availablePrinters();
    if (listPrinters.count() < 1) {
        MsgBox msg;
        msg.error("Es ist konnte kein Drucker im System gefunden werden!");
        return;
    }
    int def_prn = myConf.getDefaultPrinter(prn_mode);
    if (def_prn < 0 || def_prn >= listPrinters.count()) def_prn = 0;

    QPrinter printer(listPrinters.at(def_prn), QPrinter::ScreenResolution);

    //printer.setColorMode(QPrinter::GrayScale);
    if (s->height() > s->width()) printer.setPageOrientation(QPageLayout::Portrait);
    else printer.setPageOrientation(QPageLayout::Landscape);
    //printer.setOutputFormat(QPrinter::NativeFormat);
    //printer.setFullPage(true);
    QPageLayout layout = printer.pageLayout();

    layout.setUnits(QPageLayout::Millimeter);
    //layout.setMargins(QMarginsF(left, top, right, bottom));

    printer.setPageLayout(layout);

    //qDebug() << printer.paperSize(QPrinter::Millimeter);
    //qDebug() << printer.paperSize(QPrinter::Pica);
    if (doc->getTemplateType() == cTemplateType::TT_RECEIPETMPL) {
        layout.setMargins(QMarginsF(myConf.getPrintOffsetX(), myConf.getPrintOffsetY(), 0, 0));
        //printer.setPaperSize(QPrinter::A5);
    } else if (doc->getTemplateType() == cTemplateType::TT_PATIENTDATA) {
        printer.setFullPage(true);
        layout.setMargins(QMarginsF(myConf.getPrintOffsetX(), myConf.getPrintOffsetY(), 0, 0));
        //printer.setPaperSize(QPrinter::A5);
    } else{
        layout.setMargins(QMarginsF(myConf.getPrintDocOffsetX(), myConf.getPrintDocOffsetY(), 0, 0));
        printer.setPageSize(QPageSize(QPageSize::A4));
    }
    //qDebug() << printer.paperSize(QPrinter::Millimeter);

    QPrintDialog *dialog = NULL;
    if (myConf.isPrnDlg(ConfXML::PRN_DOC)) {
        dialog = new QPrintDialog(&printer, this);
        dialog->setWindowTitle("Drucken");
        dialog->setOption(QPrintDialog::PrintSelection, true);
        if (dialog->exec() != QDialog::Accepted) return;
    }

    // disable data field marking before printing
    m_ui->cbMarkFields->setChecked(false);

    // for template "muster 13/14" we do only print the data elements
    if ((doc->getTemplateType() == cTemplateType::TT_RECEIPETMPL) || (doc->getTemplateType() == cTemplateType::TT_PATIENTDATA)) {
        QList<QGraphicsItem*> l = s->items();
        for (int i = 0; i < l.count(); i++) {
            l.at(i)->hide();
        }
        for (int i = 0; i < doc->listDataElements.count(); i++) {
            doc->listDataElements.at(i)->show();
        }
    }
    s->clearSelection();
    s->clearFocus();

    doc->showPageRect(false);
    QPainter painter(&printer);
    //qDebug() << print_scale << s->sceneRect() << (QRectF)painter.viewport();
    //s->render(&painter);
    //if (print_scale) s->render(&painter, (QRectF)painter.viewport(), s->sceneRect(), Qt::KeepAspectRatio);
/*    if (print_scale) s->render(&painter, s->sceneRect(), (QRectF)painter.viewport(), Qt::KeepAspectRatio);
    //else s->render(&painter);
    else s->render(&painter, s->sceneRect(), s->sceneRect(), Qt::KeepAspectRatio);
*/
    s->render(&painter, s->sceneRect(), s->sceneRect(), Qt::KeepAspectRatio);

    if (doc->getTemplateType() == cTemplateType::TT_RECEIPETMPL) {
        QList<QGraphicsItem*> l = s->items();
        for (int i = 0; i < l.count(); i++) {
            l.at(i)->show();
        }
    }
    doc->showPageRect(true);

    if (dialog != NULL) delete dialog;
}

void DlgDocViewer::generatePdf()
{
    QGraphicsScene *s = m_ui->graphicsView->scene();
    if (s == NULL) return;
    if (listDocs.count() < 1) return;
    cDoc *doc = listDocs.at(0);
    if (doc == NULL) return;

    if (doc->getTemplateType() != cTemplateType::TT_INVOICE) {
        // MsgBox::info("Nur für Rechnungen möglich!");
        return;
    }
    QPrinter printer(QPrinter::ScreenResolution /*Hi0ghResolution */);
    //printer.setColorMode(QPrinter::GrayScale);
    if (s->height() > s->width()) printer.setPageOrientation(QPageLayout::Portrait);
    else printer.setPageOrientation(QPageLayout::Landscape);
   //printer.setFullPage(true);
    printer.setOutputFormat(QPrinter::PdfFormat);
#ifdef linux
    QString filename = "/home/Frank/" + doc->getCompData(2) + ".pdf";
#else
    QString filename = "F:/Rechnungen/" + (doc->getCompData(2)).left(4) + "/" + doc->getCompData(2) + ".pdf";
#endif
    printer.setOutputFileName(filename);
    printer.setPageSize(QPageSize::A4);
    printer.setPdfVersion(QPagedPaintDevice::PdfVersion_1_6);
    // disable data field marking before printing
    m_ui->cbMarkFields->setChecked(false);

    s->clearSelection();
    s->clearFocus();

    doc->showPageRect(false);

    QPainter painter(&printer);
    s->render(&painter, s->sceneRect(), s->sceneRect(), Qt::KeepAspectRatio);
}

void DlgDocViewer::slotPDF()
{
    generatePdf();
    cDoc *doc = listDocs.at(0);
    if (doc == NULL) return;

#ifdef linux
    QString filename = "/home/Frank/" + doc->getCompData(2) + ".pdf";
#else
    QString filename = "F:/Rechnungen/" + (doc->getCompData(2)).left(4) + "/" + doc->getCompData(2) + ".pdf";
#endif

    // Check for exist file
    QFile file( filename );
    if(!file.open(QIODevice::ReadOnly))
      MsgBox::error("Dokument " + filename + " konnte nicht gespeichert werden!");
    else
      MsgBox::info("Dokument " + filename + " gespeichert.");

    doc->showPageRect(true);
}

QString DlgDocViewer::transliterateUmlauts(const QString& input) {
    QString output = input;
    output.replace("ä", "ae");
    output.replace("ö", "oe");
    output.replace("ü", "ue");
    output.replace("Ä", "Ae");
    output.replace("Ö", "Oe");
    output.replace("Ü", "Ue");
    output.replace("ß", "ss");
    return output;
}

QByteArray DlgDocViewer::toAsciiRfc5322(const QString& input) {
    QString asciiOnly = transliterateUmlauts(input);
    return asciiOnly.toLatin1();  // ergibt einen reinen 7-Bit ASCII-ByteArray
}

void DlgDocViewer::slotMail()
{
    if (changed()) {
        MsgBox box;
        if (box.execYesNoBox(QString::fromUtf8("Sie haben neue Dokumente erzeugt oder Änderungen vorgenommen. Sollen diese gespeichert werden?"), MsgBox::BTN_YES) == MsgBox::BTN_YES) {
            QDialog::accept();
        } else {
            QDialog::reject();
        }
    }

    DlgDocViewer::generatePdf();

    cDoc *doc = listDocs.at(0);
    if (doc == NULL) return;
    GenDoc gendoc;
    int id = gendoc.getFId();

#ifdef linux
    QString filename = "/home/Frank/" + doc->getCompData(2) + ".pdf";
#else
    QString filename = "F:/Rechnungen/" + (doc->getCompData(2)).left(4) + "/" + doc->getCompData(2) + ".pdf";
#endif

    QList<cInvoice *> invoices;
    invoices.append(new cInvoice());
    db.retrieveItem(*invoices.first(), id);

    cPatient patient;
    db.retrieveItem(patient, invoices[0]->getPatientID());

    QString email = patient.getEMail();
    if (email == "") {
        MsgBox::info("Keine EMail Adresse hinterlegt!");
        return;
    }

    QFile file (filename);
    if(!file.open(QIODevice::ReadOnly)) {
        MsgBox::error("Dokument " + filename + "ist nicht gespeichert");
        return;
    }

    cSurgery praxis = db.getSurgery();

    QString user= praxis.getName();
    QString passwd= praxis.getPassword();
    QString host= praxis.getServer();

    MimeMessage message;

    QByteArray abs = toAsciiRfc5322(praxis.getName1());

    EmailAddress sender(user, abs);
    message.setSender(sender);

    abs = toAsciiRfc5322(patient.getName());

    EmailAddress to(email, abs);

    message.addRecipient(to);

    message.setSubject("Rechnung");

    QString anrede;
    if (patient.getGender() ==0)
        anrede = "Sehr geehrter Herr " + patient.getName() + ",";
    else
        anrede = "Sehr geehrte Frau " + patient.getName() + ",";
    QDate date = QDate::currentDate();
    if (date.year() - patient.getBirthday().year() < 19)
        anrede = "Sehr geehrte Familie " + patient.getName() + ",";

    MimeText text;
    text.setText(anrede + "\n\nin der Anlage übersende ich Ihnen wie vereinbart die Rechnung.\n\nMit freundlichen Grüßen\nAnja Bilgett\n\n" +
        "Praxis für Osteopathie Anja Bilgett\n01187 DRESDEN\nKaitzer Str. 109\nTel.: +49 351 3090 2133\nE-Mail: info@anja-bilgett.de\nWeb: https://anja-bilgett.de\n");

    // Now add it to the mail
    message.addPart(&text);

    //QFile document(file);
    MimeAttachment documentAttachment(&file);
    message.addPart(&documentAttachment);

    // Now we can send the mail
    SmtpClient smtp(host, 465, SmtpClient::SslConnection);

    smtp.connectToHost();
    if (!smtp.waitForReadyConnected()) {
        qDebug() << "Failed to connect to host!";
        return;
    }

    smtp.login(user, passwd);
    if (!smtp.waitForAuthenticated()) {
        qDebug() << "Failed to login!";
        return;
    }

    smtp.sendMail(message);
    if (!smtp.waitForMailSent()) {
        qDebug() << "Failed to send mail!";
        return;
    }

    MsgBox::info("EMail erfolgreich gesendet!");
    m_ui->pbMail->setStyleSheet("background-color: green;");

    //set date or send
    invoices[0]->setMailDate(QDate::currentDate());
//    db.storeItem(*invoices[0], &id);
    db.saveItem(*invoices[0]);
    this->reject();
}

void DlgDocViewer::slotChange()
{
    QString sTpl = "";

    QGraphicsScene *s = m_ui->graphicsView->scene();
    if (s == NULL) return;
    if (listDocs.count() < 1) return;
    cDoc *doc = listDocs.at(0);
    if (doc == NULL) return;

    GUARD(Protect::FUNC_INV);

    this->reject();

    GenDoc gendoc;
    int id = gendoc.getFId();

    QList<cInvoice *> invoices;
    invoices.append(new cInvoice());
    db.retrieveItem(*invoices.first(), id);

    QByteArray& ins = invoices[0]->getInvData();
    QXmlStreamReader xml(ins);
    QString look_for = "tpl";
    while (!xml.atEnd()) {
        xml.readNext();
        if (xml.isStartElement()) {
            if (xml.name() == look_for) {
                sTpl = xml.readElementText();
                break;
            }
        }
    }

    int i = ins.indexOf("<tpl>", 0);
    if (sTpl == "7")
    {
        for (int j = ins.size(); j > i+5; j--)
        {
            ins[j+1] = ins[j];
        }
        ins[i+5] = '4';
        ins[i+6] = '4';
    }
    if (sTpl == "19")
    {
        ins[i+5] = '4';
        ins[i+6] = '5';
    }
    if (sTpl == "45")
    {
        ins[i+5] = '1';
        ins[i+6] = '9';
    }
    if (sTpl == "42")
    {
        ins[i+5] = '4';
        ins[i+6] = '4';
    }
    if (sTpl == "44")
    {
        ins[i+5] = '4';
        ins[i+6] = '2';
    }
    if (sTpl == "43")
    {
        ins[i+5] = '4';
        ins[i+6] = '4';
    }

    invoices[0]->setInvData(ins);
    //db.saveItem(*invoices[0]);
    gendoc.genInvoice(invoices, id);
    delete invoices.takeFirst();

    //this->accept();

    return;
}

void DlgDocViewer::resizeEvent(QResizeEvent* event)
{
    if ( (m_ui->cbFactor->currentIndex() == cDoc::ZOOM_WIDTH) || (m_ui->cbFactor->currentIndex() == cDoc::ZOOM_HEIGHT) ) {
	slotFactor(m_ui->cbFactor->currentIndex());
    }
    QWidget::resizeEvent(event);
}


void DlgDocViewer::slotSave()
{
    slotPDF();
    this->accept();
}


void DlgDocViewer::slotZoomIn()
{
    if (listDocs.count() < 1) return;
    cDoc *doc = listDocs.at(0);
    if (doc == NULL) return;

    if (m_ui->cbFactor->currentIndex() < listFactors.count() - 1) {
	m_ui->cbFactor->setCurrentIndex(m_ui->cbFactor->currentIndex() + 1);
    } else {
	for (int i = 0; i < listFactors.count(); i++) {
            if (doc->factor < listFactors.at(i)) {
		m_ui->cbFactor->setCurrentIndex(i);
		break;
	    }
	}
    }
}


void DlgDocViewer::slotZoomOut()
{
    if (listDocs.count() < 1) return;
    cDoc *doc = listDocs.at(0);
    if (doc == NULL) return;

    if ( (m_ui->cbFactor->currentIndex() > 0) && (m_ui->cbFactor->currentIndex() < listFactors.count()) ) {
	m_ui->cbFactor->setCurrentIndex(m_ui->cbFactor->currentIndex() - 1);
    } else {
	for (int i = listFactors.count() - 1; i >= 0; i--) {
            if (doc->factor > listFactors.at(i)) {
		m_ui->cbFactor->setCurrentIndex(i);
		return;
	    }
	}
    }
}


void DlgDocViewer::slotFactor(int i)
{
    if (i < 0) return;
    if (listDocs.count() < 1) return;
    cDoc *doc = listDocs.at(0);
    if (doc == NULL) return;

    qreal oldFactor = doc->factor;
    if (i < listFactors.count()) doc->factor = listFactors.at(i);
    else if (i == cDoc::ZOOM_HEIGHT) doc->factor = (m_ui->graphicsView->height() - 10) / m_ui->graphicsView->scene()->height();
    else if (i == cDoc::ZOOM_WIDTH) doc->factor = (m_ui->graphicsView->width() - 20) / m_ui->graphicsView->scene()->width();
    else doc->factor = 1;

    m_ui->graphicsView->resetTransform();
    m_ui->graphicsView->scale(doc->factor, doc->factor);

    // We will not scale the text-based elements because scaling done by QGraphicsView
    // will render the fonts quite ugly.
    // Instead of scaling we simply calculate the proper font size
    if (listDocs.count() > 0 && oldFactor != 0 && doc->factor != 0) {
        cDoc *doc = listDocs.at(0);
        if (doc == NULL) return;
        for (int x = 0; x < doc->listTextElements.count(); x++) {
            cEditorTextElement *di = doc->listTextElements.at(x);
            QFont f = di->getFont();
            f.setPointSizeF(di->getBaseFontSize() * doc->factor);
            //di->setTextWidth(di->getBaseWidth() * doc->factor);
            di->setFont(f);
        }
        for (int x = 0; x < doc->listDataElements.count(); x++) {
            cEditorDataElement *di = doc->listDataElements.at(x);
            QFont f = di->getFont();
            f.setPointSizeF(di->getBaseFontSize() * doc->factor);
            di->setTextWidth(di->getBaseWidth() * doc->factor);
            di->setFont(f);
        }
    }
}


void DlgDocViewer::slotPrevPage()
{
    if ( (scenes.count() > 1) && (curPage > 0) ){
	curPage--;
	m_ui->graphicsView->setScene(scenes.at(curPage));
	setPageLabel();
    }
}


void DlgDocViewer::slotNextPage()
{
    if ( (scenes.count() > 1) && (curPage < scenes.count() - 1) ){
	curPage++;
	m_ui->graphicsView->setScene(scenes.at(curPage));
	setPageLabel();
    }
}


void DlgDocViewer::setPageLabel()
{
    if (scenes.count() < 1) m_ui->labPage->setText("");
    else m_ui->labPage->setText("Seite " + QString::number(curPage + 1) + "/" + QString::number(scenes.count()));
}


void DlgDocViewer::prepare()
{
    if (!saving_enabled) m_ui->pbSave->hide();
    if (listDocs.count() < 2) {
        m_ui->pbNextPage->hide();
        m_ui->pbPrevPage->hide();
        m_ui->labPage->hide();
    }

    QGraphicsScene *s = m_ui->graphicsView->scene();
    if (s == NULL) return;
    if (listDocs.count() < 1) return;
    cDoc *doc = listDocs.at(0);
    if (doc == NULL) return;

    if (doc->getTemplateType() != cTemplateType::TT_INVOICE) {
        m_ui->pbPDF->hide();
        m_ui->pbMail->hide();
        m_ui->pbChange->hide();
        m_ui->pbChange->hide();
    }

    GenDoc gendoc;
    int id = gendoc.getFId();

    QList<cInvoice *> invoices;
    invoices.append(new cInvoice());
    db.retrieveItem(*invoices.first(), id);

    cPatient patient;
    db.retrieveItem(patient, invoices[0]->getPatientID());

    // check is send
    if (!(invoices[0]->getMailDate().isNull()))
        m_ui->pbMail->setStyleSheet("background-color: green;");

    QString email = patient.getEMail();
    if (email == "") {
        m_ui->pbMail->hide();
        return;
    }

}


void DlgDocViewer::slotMarkField(bool b)
{
    int co = listDocs.count();
    for (int i = 0; i < co; i++ ) {
        int x = listDocs.at(i)->listDataElements.count();
        for (int j = 0; j < x; j++) {
            listDocs.at(i)->listDataElements.at(j)->setMarkField(b);
        }
    }
    m_ui->graphicsView->scene()->update();
}


bool DlgDocViewer::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *ke = static_cast<QKeyEvent *>(event);
        if (ke->key() == Qt::Key_F1 || ke->key() == Qt::Key_Help) {
            helpWrapper.showHelpForKeyword("openphysio_viewer", true);
            return true;
        }
    }
    return QDialog::eventFilter(obj, event);
}
