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

#include "employeetools.h"
#include "mydb.h"

#ifndef OPA

EmployeeTools EmplTool;


EmployeeTools::EmployeeTools()
{
}


void EmployeeTools::getActiveEmployees(QList<cEmployee *> &list, int group)
{
    int co = db.listEmpl.count();
    for (int i = 0; i < co; i++) {
        cEmployee *e = db.listEmpl.at(i);
        if ((e->getState() == cEmployee::STATE_ACTIVE) &&
            (group == cEmployeeGroup::GROUP_ALL || e->getGroupID() == group))
        {
            list.append(new cEmployee());
            *list.last() = *e;
        }
    }
}


QStringList EmployeeTools::getActiveEmployeesList(int flag)
{
    QStringList l;
    int co = db.listEmpl.count();
    for (int i = 0; i < co; i++) {
        cEmployee *e = db.listEmpl.at(i);
        if (e->getState() == cEmployee::STATE_ACTIVE) {
            if (flag == FLAG_FULLNAME) l << e->getFullName();
            else if (flag == FLAG_PID) l << e->getShortName();
        }
    }
    return l;

}


void EmployeeTools::fillComboBox(QComboBox *cb, int group, int flag)
{
    if (cb == NULL) return;
    int co = db.listEmpl.count();
    if (co < 1) return;
    QStringList l;
    for (int i = 0; i < co; i++) {
        cEmployee *e = db.listEmpl.at(i);
        if (e->getState() != cEmployee::STATE_ACTIVE) continue;
        if (group != cEmployeeGroup::GROUP_ALL)
            if (e->getGroupID() != group) continue;
        if (flag == FLAG_FULLNAME) l << e->getFullName();
        else if (flag == FLAG_PID) l << e->getShortName();
    }
    cb->addItems(l);
}


void EmployeeTools::fillComboBox(QComboBox *cb, QList<cEmployee*> &list, int flag)
{
    if (cb == NULL) return;
    int co = list.count();
    if (co < 1) return;
    QStringList l;
    for (int i = 0; i < co; i++) {
        cEmployee *e = list.at(i);
        if (flag == FLAG_FULLNAME) l << e->getFullName();
        else if (flag == FLAG_PID) l << e->getShortName();
    }
    cb->addItems(l);
}


int EmployeeTools::getIndexOfEmployee(int id, QList<cEmployee *> &list)
{
    if (id < 0) return -1;
    for (int i = 0; i < list.count(); i++) {
        if (list.at(i)->getID() == id) return i;
    }
    return -1;
}


QString EmployeeTools::getEmplPID(int id)
{
    for (int i = 0; i < db.listEmpl.count(); i++) {
        if (db.listEmpl.at(i)->getID() == id)
            return db.listEmpl.at(i)->getShortName();
    }
    return "";
}


QString EmployeeTools::getEmplFullName(int id)
{
    for (int i = 0; i < db.listEmpl.count(); i++) {
        if (db.listEmpl.at(i)->getID() == id)
            return db.listEmpl.at(i)->getFullName();
    }
    return "";
}


QString EmployeeTools::getEmplName(int id, bool gender)
{
    for (int i = 0; i < db.listEmpl.count(); i++) {
        if (db.listEmpl.at(i)->getID() == id) {
            QString prefix = "";
            if (gender) {
                if (db.listEmpl.at(i)->getGender() == GENDER_MALE) prefix = "Hr. ";
                else if (db.listEmpl.at(i)->getGender() == GENDER_FEMALE) prefix = "Fr. ";
            }
            return prefix + db.listEmpl.at(i)->getName();
        }
    }
    return "";
}


int EmployeeTools::getEmplID(QString pid)
{
    for (int e = 0; e < db.listEmpl.count(); e++) {
        if ( db.listEmpl.at(e)->getShortName() == pid )
            return db.listEmpl.at(e)->getID();
    }
    return -1;
}


bool EmployeeTools::isEmplActive(int id)
{
    for (int i = 0; i < db.listEmpl.count(); i++) {
        if (db.listEmpl.at(i)->getID() == id) {
            if (db.listEmpl.at(i)->getState() == cEmployee::STATE_ACTIVE) return true;
            return false;
        }
    }
    return false;
}

#endif
