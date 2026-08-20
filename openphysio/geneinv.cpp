#include "geneinv.h"
#include <QXmlStreamWriter>
#include "mimemultipart.h"
// #include <ctime>
#include "smtpclient.h"
#include "mimemessage.h"
#include "mimetext.h"
#include "mimeattachment.h"


GenEInv geneinv;
//int f_id;

GenEInv::GenEInv()
{
}


GenEInv::~GenEInv()
{
}


bool GenEInv::genEInvoice(QByteArray& invdat, cPatient *p )
{
    QString renr = getInvValue("nr", invdat);
    if (renr == "") return false;

#ifdef linux
    QString fileName = "/home/Frank/XRechnung_" + renr + ".xml";
#else
    QString fileName =  "F:/Rechnungen/XRechnung_" + renr + ".xml";
#endif

    QFile* file = new QFile(fileName);
    if (!file->open(QIODevice::WriteOnly)) {
        qWarning("Konnte die Datei nicht öffnen");
        return false;
    }

    QXmlStreamWriter xmlWriter(file);
    xmlWriter.setAutoFormatting(true);
    xmlWriter.writeStartDocument();

    xmlWriter.writeDefaultNamespace("urn:oasis:names:specification:ubl:schema:xsd:Invoice-2");
    xmlWriter.writeNamespace("urn:oasis:names:specification:ubl:schema:xsd:CommonAggregateComponents-2", "cac");
    xmlWriter.writeNamespace("urn:oasis:names:specification:ubl:schema:xsd:CommonExtensionComponents-2", "cec");
    xmlWriter.writeNamespace("urn:oasis:names:specification:ubl:schema:xsd:CommonBasicComponents-2", "cbc");

    xmlWriter.writeStartElement("Invoice");
        xmlWriter.writeTextElement("cbc:CustomizationID", "urn:cen.eu:en16931:2017#compliant#urn:xeinkauf.de:kosit:xrechnung_3.0");
        xmlWriter.writeTextElement("cbc:ProfileID", "urn:fdc:peppol.eu:2017:poacc:billing:01:1.0");

        // Nr. und Datum
        xmlWriter.writeTextElement("cbc:ID", renr);

    QString date = getInvValue("date", invdat);
    // Datum aus den Strings erstellen
    QDate iDate = QDate(date.mid(6, 4).toInt(), date.mid(3, 2).toInt(), date.mid(0,2).toInt());

        xmlWriter.writeTextElement("cbc:IssueDate", iDate.toString("yyyy-MM-dd"));

    QDate dDate = iDate.addDays(21);

        xmlWriter.writeTextElement("cbc:DueDate", dDate.toString("yyyy-MM-dd"));
        xmlWriter.writeTextElement("cbc:InvoiceTypeCode", "380");
        xmlWriter.writeTextElement("cbc:DocumentCurrencyCode", "EUR");
        xmlWriter.writeTextElement("cbc:BuyerReference", "B2B_Rechnung");

    // Rechnungssteller
        xmlWriter.writeStartElement("cac:AccountingSupplierParty");
            xmlWriter.writeStartElement("cac:Party");
                xmlWriter.writeStartElement("cbc:EndpointID");
                    xmlWriter.writeAttribute("schemeID", "EM");
                    xmlWriter.writeCharacters("rechnung@anja-bilgett.de");
                xmlWriter.writeEndElement();
                xmlWriter.writeStartElement("cac:PartyIdentification");
                    xmlWriter.writeTextElement("cbc:ID", "27303348");
                xmlWriter.writeEndElement();
                xmlWriter.writeStartElement("cac:PartyName");
                    xmlWriter.writeTextElement("cbc:Name", "Praxis für Osteopathie Anja Bilgett");
                xmlWriter.writeEndElement();
                xmlWriter.writeStartElement("cac:PostalAddress");
                    xmlWriter.writeTextElement("cbc:StreetName", "Kaitzer Str. 109");
                    xmlWriter.writeTextElement("cbc:CityName", "Dresden");
                    xmlWriter.writeTextElement("cbc:PostalZone", "01187");
                    xmlWriter.writeStartElement("cac:Country");
                        xmlWriter.writeTextElement("cbc:IdentificationCode", "DE");
                    xmlWriter.writeEndElement();
                xmlWriter.writeEndElement();
                xmlWriter.writeStartElement("cac:PartyTaxScheme");
                    xmlWriter.writeTextElement("cbc:CompanyID", "203/207/03396");
                    xmlWriter.writeStartElement("cac:TaxScheme");
                        xmlWriter.writeTextElement("cbc:ID", "FC");
                    xmlWriter.writeEndElement();
                xmlWriter.writeEndElement();
                xmlWriter.writeStartElement("cac:PartyLegalEntity");
                    xmlWriter.writeTextElement("cbc:RegistrationName", "Praxis für Osteopathie Anja Bilgett");
                xmlWriter.writeEndElement();
                xmlWriter.writeStartElement("cac:Contact");
                    xmlWriter.writeTextElement("cbc:Name", "Anja Bilgett");
                    xmlWriter.writeTextElement("cbc:Telephone", "+49 351 30 90 21 33");
                    xmlWriter.writeTextElement("cbc:ElectronicMail", "info@anja-bilgett.de");
                xmlWriter.writeEndElement();
            xmlWriter.writeEndElement();
        xmlWriter.writeEndElement();

    // Rechnungsempfänger
        xmlWriter.writeStartElement("cac:AccountingCustomerParty");
            xmlWriter.writeStartElement("cac:Party");
                xmlWriter.writeStartElement("cbc:EndpointID");
                    xmlWriter.writeAttribute("schemeID", "EM");

    QString eMail = p->getEMail();
    if (eMail == "") eMail= "rechnung@anja-bilgett.de";

                    xmlWriter.writeCharacters(eMail);
                xmlWriter.writeEndElement();
                xmlWriter.writeStartElement("cac:PartyName");
                    xmlWriter.writeTextElement("cbc:Name", p->getForename() + " " + p->getName());
                xmlWriter.writeEndElement();
                xmlWriter.writeStartElement("cac:PostalAddress");
                    xmlWriter.writeTextElement("cbc:StreetName", p->getStreet());
                    xmlWriter.writeTextElement("cbc:CityName", p->getCity());
                    xmlWriter.writeTextElement("cbc:PostalZone", p->getZipcode());
                    xmlWriter.writeStartElement("cac:Country");
                        xmlWriter.writeTextElement("cbc:IdentificationCode", "DE");
                    xmlWriter.writeEndElement();
                xmlWriter.writeEndElement();
                xmlWriter.writeStartElement("cac:PartyLegalEntity");
                    xmlWriter.writeTextElement("cbc:RegistrationName", p->getForename() + " " + p->getName());
                xmlWriter.writeEndElement();
            xmlWriter.writeEndElement();
        xmlWriter.writeEndElement();

        xmlWriter.writeStartElement("cac:Delivery");
            xmlWriter.writeTextElement("cbc:ActualDeliveryDate", iDate.toString("yyyy-MM-dd"));
        xmlWriter.writeEndElement();
        xmlWriter.writeStartElement("cac:PaymentMeans");
            xmlWriter.writeTextElement("cbc:PaymentMeansCode", "58");
            //xmlWriter.writeTextElement("cbc:PaymentID", renr);
            xmlWriter.writeStartElement("cac:PayeeFinancialAccount");
                xmlWriter.writeTextElement("cbc:ID", "DE87120300001007293515");
                xmlWriter.writeTextElement("cbc:Name", "Anja Bilgett");
                //xmlWriter.writeStartElement("cac:FinancialInstitutionBranch");
                //    xmlWriter.writeTextElement("cbc:Name", "Anja Bilgett");
                //    xmlWriter.writeStartElement("cac:FinancialInstitutionBranch");
                //        xmlWriter.writeTextElement("cbc:ID", "BYLADEM1001");
                //    xmlWriter.writeEndElement();
                //xmlWriter.writeEndElement();
            xmlWriter.writeEndElement();
        xmlWriter.writeEndElement();
        xmlWriter.writeStartElement("cac:PaymentTerms");
            xmlWriter.writeTextElement("cbc:Note", "Zahlbar innerhalb von 21 Tagen");
        xmlWriter.writeEndElement();

    // Rechnungsbetrag
    QString value = getInvValue("amount", invdat);
    int m = value.indexOf(' ');
    value = value.mid (0, m);

        xmlWriter.writeStartElement("cac:TaxTotal");
            xmlWriter.writeStartElement("cbc:TaxAmount");
                xmlWriter.writeAttribute("currencyID", "EUR");
                xmlWriter.writeCharacters("0.00");
            xmlWriter.writeEndElement();
            xmlWriter.writeStartElement("cac:TaxSubtotal");
                xmlWriter.writeStartElement("cbc:TaxableAmount");
                    xmlWriter.writeAttribute("currencyID", "EUR");
                    xmlWriter.writeCharacters(value);
                xmlWriter.writeEndElement();
                xmlWriter.writeStartElement("cbc:TaxAmount");
                    xmlWriter.writeAttribute("currencyID", "EUR");
                    xmlWriter.writeCharacters("0.00");
                xmlWriter.writeEndElement();
                xmlWriter.writeStartElement("cac:TaxCategory");
                    xmlWriter.writeTextElement("cbc:ID", "Z");
                    xmlWriter.writeTextElement("cbc:Percent", "0");
                    //xmlWriter.writeTextElement("cbc:TaxExemptionReason", "Kein Ausweis von Umsatzsteuer, da medizinische Dienstleistung gemäß § 4 UStG");
                    xmlWriter.writeStartElement("cac:TaxScheme");
                        xmlWriter.writeTextElement("cbc:ID", "VAT");
                    xmlWriter.writeEndElement();
                xmlWriter.writeEndElement();
            xmlWriter.writeEndElement();
        xmlWriter.writeEndElement();
        xmlWriter.writeStartElement("cac:LegalMonetaryTotal");
            xmlWriter.writeStartElement("cbc:LineExtensionAmount");
                xmlWriter.writeAttribute("currencyID", "EUR");
                xmlWriter.writeCharacters(value);
            xmlWriter.writeEndElement();
            xmlWriter.writeStartElement("cbc:TaxExclusiveAmount");
                xmlWriter.writeAttribute("currencyID", "EUR");
                xmlWriter.writeCharacters(value);
            xmlWriter.writeEndElement();
            xmlWriter.writeStartElement("cbc:TaxInclusiveAmount");
                xmlWriter.writeAttribute("currencyID", "EUR");
                xmlWriter.writeCharacters(value);
            xmlWriter.writeEndElement();
            xmlWriter.writeStartElement("cbc:AllowanceTotalAmount");
                xmlWriter.writeAttribute("currencyID", "EUR");
                xmlWriter.writeCharacters("0.00");
            xmlWriter.writeEndElement();
            xmlWriter.writeStartElement("cbc:PrepaidAmount");
                xmlWriter.writeAttribute("currencyID", "EUR");
                xmlWriter.writeCharacters("0.00");
            xmlWriter.writeEndElement();
            xmlWriter.writeStartElement("cbc:PayableAmount");
                xmlWriter.writeAttribute("currencyID", "EUR");
                xmlWriter.writeCharacters(value);
            xmlWriter.writeEndElement();
        xmlWriter.writeEndElement();

        xmlWriter.writeStartElement("cac:InvoiceLine");
            xmlWriter.writeTextElement("cbc:ID", "1");
            xmlWriter.writeStartElement("cbc:InvoicedQuantity");
                xmlWriter.writeAttribute("unitCode", "C62");
                xmlWriter.writeCharacters("1");
            xmlWriter.writeEndElement();
            xmlWriter.writeStartElement("cbc:LineExtensionAmount");
                xmlWriter.writeAttribute("currencyID", "EUR");
                xmlWriter.writeCharacters(value);
            xmlWriter.writeEndElement();
            xmlWriter.writeStartElement("cac:Item");
                //xmlWriter.writeTextElement("cbc:Description", "Osteopathische Behandlung");
                xmlWriter.writeTextElement("cbc:Name", "Osteopathie");
                xmlWriter.writeStartElement("cac:SellersItemIdentification");
                    xmlWriter.writeTextElement("cbc:ID", "1");
                xmlWriter.writeEndElement();
                xmlWriter.writeStartElement("cac:ClassifiedTaxCategory");
                    xmlWriter.writeTextElement("cbc:ID", "Z");
                    xmlWriter.writeTextElement("cbc:Percent", "0");
                    xmlWriter.writeStartElement("cac:TaxScheme");
                        xmlWriter.writeTextElement("cbc:ID", "VAT");
                    xmlWriter.writeEndElement();
                xmlWriter.writeEndElement();
            xmlWriter.writeEndElement();
            xmlWriter.writeStartElement("cac:Price");
                xmlWriter.writeStartElement("cbc:PriceAmount");
                    xmlWriter.writeAttribute("currencyID", "EUR");
                    xmlWriter.writeCharacters(value);
                xmlWriter.writeEndElement();
            xmlWriter.writeEndElement();
        xmlWriter.writeEndElement();
    xmlWriter.writeEndDocument();
    file->close();

    // Datei an sevdesk senden

    cSurgery praxis = db.getSurgery();

    QString user= praxis.getName();
    QString passwd= praxis.getPassword();
    QString host= praxis.getServer();
    QString name1= praxis.getName1();

    QString zmail = "autobox@sevdesk.email";
    QString empfaenger = "sevDesk"; // "autobox@sevdesk.email";

    MimeMessage message;


    EmailAddress sender(user, name1);
    message.setSender(sender);

    EmailAddress to(zmail, empfaenger);
    message.addRecipient(to);
    message.addTo(zmail);

    message.setSubject("XRechnung_" + renr);

    // Erzeuge multipart/mixed Container
    MimeMultiPart *mixed = new MimeMultiPart(MimeMultiPart::Mixed);

    // Textteil
    auto text = new MimeText;
    text->setText("Rechnung generiert von OpenPhysio\n");
    text->setContentType("text/plain; charset=UTF-8");
    mixed->addPart(text);


    // Now add it to the mail
//    message.addPart(&text);

    if (!file->open(QIODevice::ReadOnly)) {
        qDebug() << "Konnte Datei nicht öffnen!";
        return false;
    }

    // Konstruktor: Datei + Dateiname
    auto attachment = new MimeAttachment(file);

    // Content-Type explizit setzen
    // documentAttachment->setContentType("application/xml");
    attachment->setContentType( "text/xml; charset=UTF-8");
    attachment->setContentName(QFileInfo(*file).fileName());
    mixed->addPart(attachment);

//    message.addPart(documentAttachment);

    // Setze den Container als Content der Nachricht
    message.setContent(mixed);

    // zum Test:
    // Vor dem Senden: komplette MIME-Mail dumpen
    // qDebug().noquote() << message.toString();

    // Now we can send the mail
    SmtpClient smtp(host, 465, SmtpClient::SslConnection);

    smtp.connectToHost();
    if (!smtp.waitForReadyConnected()) {
        qDebug() << "Failed to connect to host!";
        return false;
    }

    smtp.login(user, passwd);
    if (!smtp.waitForAuthenticated()) {
        qDebug() << "Failed to login!";
        return false;
    }

    smtp.sendMail(message);
    if (!smtp.waitForMailSent()) {
        qDebug() << "Failed to send mail!";
        return false;
    }

    smtp.quit();
    return true;
}

QString GenEInv::getInvValue(QString muster, QByteArray& invdat)
{
    QString s = "";
    QXmlStreamReader xml(invdat);

    while (!xml.atEnd()) {
        xml.readNext();
        if (xml.isStartElement()) {
            if (xml.name() == muster) {
                s = xml.readElementText();
                break;
            }
        }
    }
    return s;
}


