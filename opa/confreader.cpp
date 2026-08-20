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

#include "confreader.h"
#include "ui_confreader.h"
//#include "kvkread.h"


ConfReader::ConfReader(QWidget *parent, int l, int a) :
    ConfBase(parent, l, a),
    m_ui(new Ui::ConfReader)
{
    m_ui->setupUi(this);
    setTitle(QString::fromUtf8("Kartenlesegerät"));
    setKeyword("opa_reader");

    m_ui->cbAPI->clear();
    m_ui->cbAPI->addItem("CT-API");
#ifndef Q_WS_WIN
    // currently we do not support MCARD on Windows, but on Linux and Mac
    m_ui->cbAPI->addItem("MCARD");
#endif
}


ConfReader::~ConfReader()
{
    delete m_ui;
}

void ConfReader::init()
{
    connect(m_ui->pbTestReader, &QPushButton::clicked, this, &ConfReader::slotTestReader);
    connect(m_ui->pbLib, &QPushButton::clicked, this, &ConfReader::slotSelectLib);
    connect(m_ui->spTerm, &QSpinBox::valueChanged, this, &ConfReader::slotChangeTerminal);
    connect(m_ui->spRead, &QSpinBox::valueChanged, this, &ConfReader::slotChangeOffset);
    connect(m_ui->spPort, &QSpinBox::valueChanged, this, &ConfReader::slotChangePort);
    connect(m_ui->cbSelect, &QCheckBox::toggled, this, &ConfReader::slotUseSelect);
    connect(m_ui->leLib, &QLineEdit::textChanged, this, &ConfReader::slotEditLib);

    disconnect(m_ui->cbReader, &QComboBox::currentIndexChanged, this, &ConfReader::slotSetupReader);

    db.retrieveItemList(myDB::TABLE_READER_DEVICES, list_readers);
    if (list_readers.count() <= 0) return;

    for (int i = 0; i < list_readers.count(); i++) {
	m_ui->cbReader->addItem(((cReaderDevice*)(list_readers.at(i)))->getName());
    }
    int r = myConf.get(ConfXML::AREA_READER, "rdevice").toInt();
    if ( (r < 0) || (r >= list_readers.count()) ) {
	slotSetupReader(0);
	m_ui->cbReader->setCurrentIndex(0);
    } else {
	m_ui->cbReader->setCurrentIndex(r);
	curReader = myConf.getReader();
	if (curReader.getAPI() == cReaderDevice::CTAPI) {
	    m_ui->cbAPI->setCurrentIndex(cReaderDevice::CTAPI);
	    m_ui->leLib->setText(curReader.getLib());
	    m_ui->spTerm->setValue(curReader.getTerminal());
	    m_ui->spPort->setValue(curReader.getPort());
	    m_ui->spRead->setValue(curReader.getReadOffset());
	    if (curReader.getSelect()) m_ui->cbSelect->setCheckState(Qt::Checked);
	    else m_ui->cbSelect->setCheckState(Qt::Unchecked);
	    m_ui->widgetCTAPI->show();
	} else if (curReader.getAPI() == cReaderDevice::MCARD) {
	    m_ui->cbAPI->setCurrentIndex(cReaderDevice::MCARD);
	    m_ui->leLib->setText(curReader.getLib());
	    m_ui->widgetCTAPI->hide();            
        } else {
            m_ui->widgetCTAPI->hide();
	}
    }
    connect(m_ui->cbAPI, &QComboBox::currentIndexChanged, this, &ConfReader::slotChangeAPI);
    connect(m_ui->cbReader, &QComboBox::currentIndexChanged, this, &ConfReader::slotSetupReader);
}


bool ConfReader::write()
{
    myConf.set(ConfXML::AREA_READER, "rdevice", QString::number(m_ui->cbReader->currentIndex()));
    myConf.set(ConfXML::AREA_READER, "api", QString::number(m_ui->cbAPI->currentIndex()));
    myConf.set(ConfXML::AREA_READER, "lib", m_ui->leLib->text());
    myConf.set(ConfXML::AREA_READER, "readofs", QString::number(m_ui->spRead->value()));
    myConf.set(ConfXML::AREA_READER, "port", QString::number(m_ui->spPort->value()));
    myConf.set(ConfXML::AREA_READER, "term", QString::number(m_ui->spTerm->value()));
    if (m_ui->cbSelect->checkState() == Qt::Checked) myConf.set(ConfXML::AREA_READER, "select", "true");
    else myConf.set(ConfXML::AREA_READER, "select", "false");
    return true;
}


void ConfReader::slotSetupReader(int i)
{
    if ( (i < 0) || (i >= list_readers.count()) ) return;
    cReaderDevice *reader = (cReaderDevice*)list_readers.at(i);
    if (reader == NULL) return;
    curReader = *reader;
    if (reader->getAPI() == cReaderDevice::CTAPI) {
	m_ui->cbAPI->setCurrentIndex(cReaderDevice::CTAPI);
	m_ui->leLib->setText(reader->getLib());
	m_ui->spTerm->setValue(reader->getTerminal());
	m_ui->spPort->setValue(reader->getPort());
	m_ui->spRead->setValue(reader->getReadOffset());
	if (reader->getSelect()) m_ui->cbSelect->setCheckState(Qt::Checked);
	else m_ui->cbSelect->setCheckState(Qt::Unchecked);
	m_ui->widgetCTAPI->show();
    } else if (reader->getAPI() == cReaderDevice::MCARD) {
	m_ui->cbAPI->setCurrentIndex(cReaderDevice::MCARD);
	m_ui->leLib->setText(reader->getLib());
	m_ui->widgetCTAPI->hide();
    } else {
	m_ui->widgetCTAPI->hide();
    }
}


void ConfReader::slotSelectLib()
{
    QString filemask;
#ifdef Q_WS_WIN
    filemask = "*.dll";
#else
    filemask = "*";
#endif
    QString fileName = QFileDialog::getOpenFileName(this,
     "Bibliothek laden", ".", "Alle Dateien ("+filemask+")");
    curReader.setLib(fileName);
    m_ui->leLib->setText(fileName);
}


void ConfReader::slotEditLib()
{
    curReader.setLib(m_ui->leLib->text());
}


void ConfReader::slotChangeTerminal(int i)
{
    curReader.setTerminal(i);
}


void ConfReader::slotChangePort(int i)
{
    curReader.setPort(i);
}


void ConfReader::slotChangeOffset(int i)
{
    curReader.setReadOffset(i);
}


void ConfReader::slotTestReader()
{
    // KvkRead reader;

    m_ui->plainTextEdit->clear();
    m_ui->plainTextEdit->repaint();
    QString msg = "Kein Reader!"; //reader.test_reader(curReader);
    m_ui->plainTextEdit->appendPlainText(msg);
}


void ConfReader::slotChangeAPI(int i)
{
    curReader.setAPI(i);
    switch(i) {
        case cReaderDevice::CTAPI:
            m_ui->widgetCTAPI->show();
            break;
        case cReaderDevice::MCARD:
            m_ui->widgetCTAPI->hide();
            break;
        default: break;
    }
}


void ConfReader::slotUseSelect(bool b)
{
    curReader.setSelect(b);
}
