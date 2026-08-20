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

#ifndef DLGPATIENT_H
#define DLGPATIENT_H

#include <QtWidgets>
#include "ui_dlgpatient.h"
#include "mydb.h"
#include "browseinsurances.h"
#include "browsedocs.h"
#include "datetools.h"
#include "therapy.h"


class DelegateFindings : public QItemDelegate
{
public:
    DelegateFindings(QObject* parent)
    : QItemDelegate( parent )
    {
    }

    QWidget *createEditor( QWidget* parent, const QStyleOptionViewItem&,
                            const QModelIndex& index ) const
    {
        QWidget* w;
        switch( index.column() ) {
            case 0: {
                QDateEdit *editor = new QDateEdit(parent);
                editor->setDate(QDate::currentDate());
                editor->setDisplayFormat("dd.MM.yyyy");
                w = editor;
                break;
            }
            case 1: {
                QComboBox* editor = new QComboBox(parent);
                for (int i = 0; i < db.listEmpl.count(); i++) {
                    editor->addItem(db.listEmpl.at(i)->getShortName());
                }
                w = editor;
                break;
            }
            default:
                w = 0;
                break;
         }
         return w;
     }

    void setEditorData( QWidget* editor, const QModelIndex& index ) const
    {
        QVariant value = index.model()->data( index, Qt::DisplayRole );

        switch( index.column() ) {
            case 0: {
                static_cast<QDateEdit*>( editor )->setDate(value.toDate());
                break;
            }
            case 1: {
                QComboBox* box = static_cast<QComboBox*>( editor );
                box->setCurrentIndex( box->findText( value.toString() ) );
                break;
            }
            default: break;
        }
    }

    void setModelData( QWidget* editor, QAbstractItemModel* model,
                       const QModelIndex& index ) const
    {
        QVariant value;

        switch( index.column() ) {
            case 0: {
                QDate qd = static_cast<QDateEdit*>( editor )->date();
                value = qd.toString("dd.MM.yyyy");
                //value = static_cast<QDateEdit*>( editor )->date();
                break;
            }
            case 1: {
                value = static_cast<QComboBox*>( editor )->currentText();
                break;
            }
            default: break;
        }
        model->setData( index, value );
    }

    void updateEditorGeometry( QWidget* editor,
                               const QStyleOptionViewItem& option,
                               const QModelIndex& ) const
                               {
        editor->setGeometry( option.rect );
    }

};


class DlgPatient : public QDialog
{
    Q_OBJECT

private:
    Ui::DlgPatientClass ui;
    cPatient curPatient;
    BrowseDocs *browseDocs;
    QList<cEmployee*> list_empl;
    DateTools dateTools;
    DateEdit *de_birthday, *de_valid, *de_freed_from, *de_freed_until, *de_contract, *de_plan;
    Therapy therapy;
    bool flag_change;

    bool savePatient();
    void checkInput();
    bool lookupInsurance();

public:
    DlgPatient(QWidget *parent = 0);
    ~DlgPatient();
    void initPatient(cPatient *p);
    bool readChip();

protected:
    bool eventFilter(QObject *obj, QEvent *event);

public slots:
    void reject();

private slots:
    void slotSavePatient();
    void slotSelectInsurance();
    void slotReadChip();
    void slotModified();
    void slotModifiedWithCheck();
    void slotAge(QDate d);
    void slotSelectPhysician();
    void slotDelPhysician();
    void slotDelInsurance();
    void slotZipCode();
    void slotTextModule();
    void slotPrint();
    void slotPrintTherapyDates();
    void on_tabDates_destroyed();
    void slotReport();
};

#endif // DLGPATIENT_H
