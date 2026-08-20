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

#ifndef CONFBASE_H
#define CONFBASE_H

#include <QtCore>
#include <QtWidgets/QWidget>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QColorDialog>
#include <mydb.h>
#include <config.h>


namespace Ui {
    class ConfBase;
}

class ConfBase : public QWidget {
    Q_OBJECT
    Q_DISABLE_COPY(ConfBase)

private:
    QString title, keyword;
    int area, layer;

public:
    enum layers { LAYER_ADMIN = 0, LAYER_DB = 1, LAYER_GLOBAL, LAYER_LOCAL, LAYER_DUMMY };
    ConfBase(QWidget *parent = 0, int l = -1, int a = -1);
    virtual ~ConfBase();
    void setTitle(QString s);
    QString getTitle();
    void setKeyword(QString s);
    QString getKeyword();
    int getArea();
    int getLayer();

    virtual void init();
    virtual bool write();

protected:
    bool flag_change;
};

#endif // CONFBASE_H
