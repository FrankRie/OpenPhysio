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

#include "helperwidget.h"
#include "config.h"
#include <QHelpEngine>

HelperWidget::HelperWidget(QWidget *parent)
    : QTextBrowser(parent)
{
    m_helpEngine = new QHelpEngine(myConf.getDocDir() + QLatin1String(DOC_HELP), this);
    if (!m_helpEngine->setupData()) {
	delete m_helpEngine;
	m_helpEngine = NULL;
    } else {
       /////////// connect(m_helpEngine->contentWidget(), &QHelpContentWidget::linkActivated, this, &HelperWidget::setSource);
    }
}


HelperWidget::~HelperWidget()
{
    if (m_helpEngine != NULL) delete m_helpEngine;
}

void HelperWidget::showHelpForKeyword(const QString &id)
{
    if (m_helpEngine != nullptr) {
        QList<QHelpLink> docs = m_helpEngine->documentsForIdentifier(id);
        //QMap<QString, QUrl> links = m_helpEngine->documentationFileName(id);
        QMap<QString, QUrl> links;
        for (const QHelpLink &hl : docs) {
            links.insert(hl.title, hl.url);
        }

        if (!links.isEmpty()) {
            setSource(links.constBegin().value());
        }
    }
}

QVariant HelperWidget::loadResource(int type, const QUrl &url)
{
    if (m_helpEngine != NULL) {
	if (url.scheme() == "qthelp")
	    return QVariant(m_helpEngine->fileData(url));
	else
	    return QTextBrowser::loadResource(type, url);
    }
    return "";
}
