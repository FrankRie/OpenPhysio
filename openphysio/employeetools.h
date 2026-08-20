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

#ifndef EMPLOYEETOOLS_H
#define EMPLOYEETOOLS_H

#ifndef OPA

#include <QtCore>
#include <QtGui>

#include "mydb.h"

class EmployeeTools
{
public:
    enum flags { FLAG_FULLNAME = 0, FLAG_PID };
    EmployeeTools();
    void getActiveEmployees(QList<cEmployee*> &list, int group = cEmployeeGroup::GROUP_ALL);
    QStringList getActiveEmployeesList(int flag = FLAG_FULLNAME);
    void fillComboBox(QComboBox *cb, int group = cEmployeeGroup::GROUP_ALL, int flag = FLAG_FULLNAME);
    void fillComboBox(QComboBox *cb, QList<cEmployee*> &list, int flag = FLAG_FULLNAME);
    int getIndexOfEmployee(int id, QList<cEmployee *> &list);
    QString getEmplPID(int id);
    QString getEmplFullName(int id);
    QString getEmplName(int id, bool gender);
    int getEmplID(QString pid);
    bool isEmplActive(int id);
};

extern EmployeeTools EmplTool;

#endif

#endif // EMPLOYEETOOLS_H
