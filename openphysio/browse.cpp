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

#include "browse.h"
#include "mydb.h"


TableFilter::TableFilter(int tid)
{
    table_id = tid;
    table_items = db.getTableItems(table_id);
    default_cj = CJ_AND;
}


TableFilter::~TableFilter()
{
}


void TableFilter::clear()
{
    filter.clear();
}


void TableFilter::clearStaticFilter()
{
    filter_static.clear();
}


TableFilter& TableFilter::operator=(TableFilter& other)
{
    if (this != &other) {
        table_id = other.getTableID();
        filter = other.getFilterList();
        filter_static = other.getStaticFilterList();
        table_items = other.getTableItems();
    }
    return *this;
}


int TableFilter::getTableID()
{
    return table_id;
}


QStringList TableFilter::getTableItems()
{
    return table_items;
}


QString TableFilter::getField(int f)
{
    QString s = "";

    if ( (f >= 0) && (f < table_items.count()) ) {
        s = table_items.at(f);
    } else {
        switch(f) {
        case F_PAT_ID: s = "patients.id"; break;
        case F_PAT_STATE: s = "patients.state"; break;
        case F_INSURANCE_AID: s = "insurances.assoc"; break;
        /*case F_PAT_SURNAME: s = "patients.surname"; break;
        case F_PAT_FORENAME: s = "patients.forename"; break;
        case F_PAT_BIRTHDAY: s = "patients.birthday"; break;
        case F_PAT_GENDER: s = "patients.gender"; break;
        case F_PAT_STREET: s = "patients.street"; break;
        case F_PAT_CITY: s = "patients.city"; break;
        case F_PAT_ZIP: s  = "patients.zip"; break;
        case F_REC_ID: s = "receipes.id"; break;
        case F_REC_TYPE: s = "receipes.type"; break;
        case F_REC_STATE: s = "receipes.state"; break;
        case F_REC_RDATE: s = "receipes.rdate"; break;
        case F_REC_ADDPAYDATE: s = "receipes.addpay_date"; break;
        case F_REC_BILLDATE: s = "receipes.bill_date"; break;
        case F_INVOICE_RID: s = "invoices.rid"; break;
        case F_REMINDER_RID: s = "reminders.rid"; break;
        case F_QUITTANCE_RID: s = "quittances.rid"; break;
        case F_REPORT_RID: s = "reports.rid"; break;
        case F_INVOICE_PID: s = "invoices.pid"; break;
        case F_REMINDER_PID: s = "reminders.pid"; break;
        case F_QUITTANCE_PID: s = "quittances.pid"; break;
        case F_REPORT_PID: s = "reports.pid"; break;
        case F_FINDING_PID: s = "findings.pid"; break;*/
        default: break;
        }
    }

    return s;
}


QStringList TableFilter::getFilterList()
{
    return filter;
}


QStringList TableFilter::getStaticFilterList()
{
    return filter_static;
}


QString TableFilter::getFilterString()
{
    // FIXME: The "OR" case is not working yet

    QString s = "";

    if (filter_static.count() > 0) {
        if (default_cj == CJ_AND) s = filter_static.join(" AND ");
        else s = filter_static.join(" OR ");
        if (filter.count() > 0) s = s + " AND ";
    }

    if (filter.count() > 0) {
        if (default_cj == CJ_AND) s = s + filter.join(" AND ");
        else s = s + filter.join(" OR ");
    }
    return s;
}


void TableFilter::addFilterExpr(bool stat, int field, int dt, int op, QString expr)
{
    QString s = getField(field);

    switch(op) {
        case OP_EQUAL:
            s += "=";
            break;
        case OP_NOTEQUAL:
            s += "!=";
            break;
        case OP_GREATER:
            s += ">";
            break;
        case OP_GREATER_OR_EQUAL:
            s += ">=";
            break;
        case OP_LOWER:
            s += "<";
            break;
        case OP_LOWER_OR_EQUAL:
            s += "<=";
            break;
        case OP_CONTAIN:
            s += (" LIKE '" + expr + "%'");
            break;
        default: break;
    }

    if (op != OP_CONTAIN) {
        switch(dt) {
        case DT_STRING:
            s += ("'" + expr + "'");
            break;
        case DT_INTEGER:
            s += (expr);
            break;
        default: break;
        }
    }

    if (stat) filter_static.append(s);
    else filter.append(s);
}


void TableFilter::addFilterExpr(bool stat, int field, int op, QString expr)
{
    addFilterExpr(stat, field, DT_STRING, op, expr);
}


void TableFilter::addFilterExpr(bool stat, int field, int op, int expr)
{
    addFilterExpr(stat, field, DT_INTEGER, op, QString::number(expr));
}


//****************************************************************************
// CustomSqlModel
//****************************************************************************

CustomSqlModel::CustomSqlModel(QObject *parent)
#ifdef USE_ASYNC_DB_ACCESS
#else
        : QSqlQueryModel(parent)
#endif
{
    date_field = -1;
    color_mode_bg = false;
    color_mode_fg = false;
    color_column_bg = 0;
    color_column_fg = 0;
    color_keywords_bg.clear();
    color_keywords_fg.clear();
    colors_bg.clear();
    colors_fg.clear();
    color_marker = new QColor();
    color_marker->setRgb(0x00, 0xff, 0xff, 0xff);
}


CustomSqlModel::~CustomSqlModel()
{
    if (color_marker != NULL) delete color_marker;
}


QVariant CustomSqlModel::data(const QModelIndex &index, int role) const
{
#ifdef USE_ASYNC_DB_ACCESS
    if (!index.isValid()) return QVariant();

    if (role == Qt::DisplayRole) {
        if (index.row() < content.count()) {
            QSqlRecord r = content.at(index.row());
            if (index.column() < r.count()) {
                QString s = r.value(index.column()).toString();
                if ( (date_field >= 0) && (index.column() == date_field) ) {
                    QDate d = QDate::fromString(s, "yyyy-MM-dd");
                    return d;
                }
                return s;
            }
        }
        return QVariant();
    } else if ( (color_mode_bg) && (role == Qt::BackgroundColorRole) ) {
        QString s = this->index(index.row(), color_column_bg).data().toString();
        int c = color_keywords_bg.indexOf(s);
        if (c >= 0) *color_marker = colors_bg.at(c);
        else color_marker->setRgb(0xff, 0xff, 0xff, 0xff);
        return *color_marker;
    } else if ( (color_mode_fg) && (role == Qt::TextColorRole) ) {
        QString s = this->index(index.row(), color_column_fg).data().toString();
        int c = color_keywords_fg.indexOf(s);
        if (c >= 0) *color_marker = colors_fg.at(c);
        else color_marker->setRgb(0x00, 0x00, 0x00, 0xff);
        return *color_marker;
    }

    return QVariant();
#else
    QVariant value;
    value = QSqlQueryModel::data(index, role);
    if ( (color_mode_bg) && (role == Qt::BackgroundRole) ) {
        // only check once (if first column is displayed) to reduce overhead
        // only check once (if first column is displayed) to reduce overhead
        // FIXME: for some reasons this does not work with Linux
#ifndef Q_OS_LINUX
        if (index.column() == 1) {
#endif
            QString s = this->index(index.row(), color_column_bg).data().toString();
	    int c = color_keywords_bg.indexOf(s);
	    if (c >= 0) *color_marker = colors_bg.at(c);
	    else color_marker->setRgb(0xff, 0xff, 0xff, 0xff);
#ifndef Q_OS_LINUX
        }
#endif
        return *color_marker;
    }

    if ( (color_mode_fg) && (role == Qt::BackgroundRole) ) {
#ifndef Q_OS_LINUX
        if (index.column() == 1) {
#endif
            QString s = this->index(index.row(), color_column_fg).data().toString();
	    int c = color_keywords_fg.indexOf(s);
	    if (c >= 0) *color_marker = colors_fg.at(c);
	    else color_marker->setRgb(0x00, 0x00, 0x00, 0xff);
#ifndef Q_OS_LINUX
        }
#endif
        return *color_marker;
    }

    if (date_field >= 0) {
        if (value.isValid() && role == Qt::DisplayRole) {
            if (index.column() == date_field) {
		QDate d = QDate::fromString(value.toString(), "yyyy-MM-dd");
		return d;
		//return d.toString("dd.MM.yyyy");
            }
        }
    }
    return value;
#endif
}

#ifdef USE_ASYNC_DB_ACCESS
int CustomSqlModel::columnCount(const QModelIndex & parent) const
{
    if (content.count() > 0) {
        return content.at(0).count();
    }
    return 0;
}

int CustomSqlModel::columnCount()
{
    if (content.count() > 0) {
        return content.at(0).count();
    }
    return 0;
}

int CustomSqlModel::rowCount(const QModelIndex & parent) const
{
    return content.count();
}

QVariant CustomSqlModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal) {
        if ( (role == Qt::DisplayRole) && (section > 0) && (section <= headers.count()) )
            return headers.at(section - 1);
    }
    return QAbstractItemModel::headerData(section, orientation, role);
    //return QVariant();
}

void CustomSqlModel::dataArrivedFromThread(const QList<QSqlRecord>& records, bool success)
{
    // FIXME: if !success => inform the user ?
    content.clear();
    content = records;
    reset;
    emit sigDataAvailable();
}

void CustomSqlModel::setHeaders(QStringList& list)
{
    headers = list;
}

void CustomSqlModel::removeContent()
{
    content.clear();
    reset;
}

#endif // USE_ASYNC_DB_ACCESS

void CustomSqlModel::setDateField(int f)
{
    date_field = f;
}

void CustomSqlModel::enableBackgroundColorMode(bool activate, int keycolumn)
{
    color_mode_bg = activate;
    color_column_bg = keycolumn;
}

void CustomSqlModel::setBackgroundKeywords(QStringList& keywords, QList<QColor>& colors)
{
    color_keywords_bg = keywords;
    colors_bg = colors;
}

void CustomSqlModel::enableForegroundColorMode(bool activate, int keycolumn)
{
    color_mode_fg = activate;
    color_column_fg = keycolumn;
}
void CustomSqlModel::setForegroundKeywords(QStringList& keywords, QList<QColor>& colors)
{
    color_keywords_fg = keywords;
    colors_fg = colors;
}



//****************************************************************************
// CustomProxyModel
//****************************************************************************

CustomProxyModel::CustomProxyModel(QObject *parent)
	: QSortFilterProxyModel(parent)
{
    filter = false;
    field = id = op = 0;
}

CustomProxyModel::~CustomProxyModel() {}

bool CustomProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    if (filter) {
	QModelIndex index0 = sourceModel()->index(source_row, field, source_parent);
	int i = sourceModel()->data(index0).toInt();
	switch(op) {
	    case Browse::OP_EQUAL: if (i != id) return false; break;
	    case Browse::OP_NOTEQUAL: if (i == id) return false; break;
	    default: break;
	}
    }
    return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
}

void CustomProxyModel::setFilter(int f, int i, int o)
{
    field = f;
    id = i;
    op = o;
}

void CustomProxyModel::enableFilter(bool b)
{
    filter = b;
}



//****************************************************************************
// TableViewAnimated
//****************************************************************************
TableViewAnimated::TableViewAnimated(QWidget *parent)
    : QTableView(parent), movie(0)
{
    movie = new QMovie(":/load.gif");
    animationVisible = false;
    connect(movie, &QMovie::frameChanged, this->viewport(), QOverload<>::of(&QWidget::repaint));
}


TableViewAnimated::~TableViewAnimated()
{
    if (movie != NULL) delete movie;
}


void TableViewAnimated::paintEvent(QPaintEvent* event)
{
    if (animationVisible && movie != NULL) {
        QPixmap movieFrame = movie->currentPixmap();

        QRect movieRect = movieFrame.rect();
        movieRect.moveCenter(rect().center());

        if (movieRect.intersects(event->rect())) {
            QPainter painter(viewport());
            painter.drawPixmap(movieRect.left(), movieRect.top(), movieFrame);
        }
    }
    // For whatever reason it's not a good idea to call paintEvent()
    // when the load animation is rendered. It will lead to crashes.
    // But as an alternativ we can clear the model (see ReloadTable()) and
    // avoiding the check here.
    //if (!animationVisible)
        QTableView::paintEvent(event);
}


void TableViewAnimated::startAnimation()
{
    if (movie != NULL) {
        movie->start();
        animationVisible = true;
    }
}


void TableViewAnimated::stopAnimation()
{
    if (movie != NULL) {
        animationVisible = false;
        movie->stop();
    }
}


//****************************************************************************
// Browse
//****************************************************************************

Browse::Browse(int mode, QWidget *parent)
    : QWidget(parent)
{
    curMode = mode;
    view = NULL;
    tableFilter = NULL;
    table_type = -1;
    last_active_item = -1;
    sort_column = 0;
    sort_order = 0;


    // 1. Modelle erstellen (Nur eine Instanz für den Proxy!)
    model = new CustomSqlModel(this); // Tipp: Parent mitgeben wegen Speicherverwaltung
    proxyModel = new CustomProxyModel(this);

    // 2. WICHTIG: Das echte Modell in das Proxy-Modell einhängen
    proxyModel->setSourceModel(model);
    proxyModel->setDynamicSortFilter(true);

    browseToolBar.setIconSize(QSize(24, 24));
    browseToolBar.setToolButtonStyle(Qt::ToolButtonIconOnly);
}


Browse::~Browse()
{
    delete proxyModel;
    delete model;
    if (tableFilter != NULL) delete tableFilter;
}


//void Browse::init(QTableView *tv, int t, int def_filter_header)
void Browse::init(TableViewAnimated *tv, int t, int def_filter_header)
{
    init(tv, t, 1, Qt::AscendingOrder, def_filter_header);
}


//void Browse::init(QTableView *tv, int t, int s_column, int s_order, int def_filter_header)
void Browse::init(TableViewAnimated *tv, int t, int s_column, int s_order, int def_filter_header)
{
    if (tv == NULL) return;
    view = tv;
    table_type = t;

    sort_column = s_column;
    sort_order = s_order;
    view->horizontalHeader()->setSortIndicatorShown(true);
    view->horizontalHeader()->setSortIndicator(sort_column, (Qt::SortOrder)sort_order);
    connect(view->horizontalHeader(), &QHeaderView::clicked, this, &Browse::slotSortItems);
    view->verticalHeader()->hide();

    proxyModel->sort(sort_column, (Qt::SortOrder)sort_order);
    tv->setSortingEnabled(false);
//    tv->setSortingEnabled(false);
    view->horizontalHeader()->setSortIndicatorShown(true);
    proxyModel->setSourceModel(model);
    tv->setModel((QAbstractItemModel*)proxyModel);
    tv->setSortingEnabled(true);
/*
    model = db.getModel(table_type);
    proxyModel->sort(sort_column, (Qt::SortOrder)sort_order);
    tv->setSortingEnabled(false);
    proxyModel->setSourceModel(model);
    tv->setModel((QAbstractItemModel*)proxyModel);
*/
    listHeaders = db.getTableHeaders(table_type);
#ifdef USE_ASYNC_DB_ACCESS
    model->setHeaders(listHeaders);
#endif
    displayFilter.init(listHeaders, def_filter_header);

    //connect(&displayFilter, SIGNAL(filterChanged(QString, int)), this, SLOT(slotDisplayFilter(QString,int)));
    connect(&displayFilter, &DisplayFilter::filterChanged, this, &Browse::slotDisplayFilter);
    connect(&db, &myDB::sigDBChangeExternal, this, &Browse::slotUpdate);
#ifdef USE_ASYNC_DB_ACCESS
    connect(model, &myDB::sigDataAvailable, this, &Browse::slotDataArrived);
#endif

    // enable date conversion for certan columns of certain tables
    // (because QSqlQueryModel is read-only CustomSqlModel is implemented which
    //  converts date on-the-fly)
    if (model == NULL) return;
    switch(table_type) {
        case myDB::TABLE_INVOICES:
            model->setDateField(1);
            break;
        case myDB::TABLE_PINVOICES:
            model->setDateField(1);
            break;
        case myDB::TABLE_RECEIPES:
            model->setDateField(1);
            break;
        case myDB::TABLE_COURSES:
            model->setDateField(1);
            break;
        case myDB::TABLE_QUITTANCES:
            model->setDateField(1);
            break;
        case myDB::TABLE_REMINDERS:
            model->setDateField(1);
            break;
        case myDB::TABLE_REPORTS:
            model->setDateField(1);
            break;
        case myDB::TABLE_FINDINGS:
            model->setDateField(1);
            break;
        case myDB::TABLE_CASH:
            model->setDateField(1);
            break;
        case myDB::TABLE_CASH_ADDPAY:
            model->setDateField(1);
            break;
        case myDB::TABLE_CASH_SELFPAY:
            model->setDateField(1);
            break;
        case myDB::TABLE_CASH_PRIVRECEIPE:
            model->setDateField(1);
            break;
        default: break;
    }
}


void Browse::enableBackgroundColorMode(bool activate, int keycolumn)
{
    if (model != NULL) model->enableBackgroundColorMode(activate, keycolumn);
}


void Browse::setBackgroundKeywords(QStringList& keywords, QList<QColor>& colors)
{
    if (model != NULL) model->setBackgroundKeywords(keywords, colors);
}


void Browse::enableForegroundColorMode(bool activate, int keycolumn)
{
    if (model != NULL) model->enableForegroundColorMode(activate, keycolumn);
}


void Browse::setForegroundKeywords(QStringList& keywords, QList<QColor>& colors)
{
    if (model != NULL) model->setForegroundKeywords(keywords, colors);
}


void Browse::ReloadTable()
{
    // actually we do not reload the table from the database but
    // only refresh to table displayed
    // the data from database is automatically loaded by the database layer
    // as soon as the database content changes
    if ( (view != NULL) && (model != NULL) && (tableFilter != NULL) ) {
        if (table_type == myDB::TABLE_PATIENTS || table_type == myDB::TABLE_RECEIPES) {
            //qDebug() << __FUNCTION__;
            // clear the model (and the tableview) to avoid a selection of an object
            // during the reload of the table
#ifdef USE_ASYNC_DB_ACCESS
            model->removeContent();
            view->startAnimation();
#endif
        }
        db.loadTable(table_type, model, tableFilter->getFilterString());
#ifndef USE_ASYNC_DB_ACCESS
        proxyModel->invalidate();
        hideHelperColumns();
#endif
    }
}


int Browse::getTableType()
{
    return table_type;
}


void Browse::hideHelperColumns()
{
    // it seems that invalidate() leads to reenabling the columns that
    // have been hidden before => we need to hide the helper columns again
    // TODO: check performance

    if ((view == NULL) || (model == NULL)) return;
    // do not show the first column containing the item IDs
    view->setColumnHidden(0, true);
    // do not show columns that have no header text
    // (these columns are used for internal filtering)
    for (int i = listHeaders.count() + 1; i < model->columnCount(); i++) {
	view->setColumnHidden(i, true);
    }
}


void Browse::slotSortItems()
{
    if (view == NULL) return;
    QHeaderView *qhv = (QHeaderView *)sender();
    if (qhv != NULL) {
        //if ( (sort_column == qhv->sortIndicatorSection()) && (sort_order == qhv->sortIndicatorOrder()) ) return;
	sort_column = qhv->sortIndicatorSection();
	sort_order = qhv->sortIndicatorOrder();
	proxyModel->sort(sort_column, qhv->sortIndicatorOrder());
    }
}


void Browse::preload()
{
    displayFilter.setFilterString("*");
}


void Browse::setFilter(TableFilter *tf)
{
    if ( (tf == NULL) || (table_type < 0) ) return;
    if (tableFilter == NULL) tableFilter = new TableFilter(table_type);
    *tableFilter = *tf;
    ReloadTable();
}

/*
void Browse::setFilter(int field, int id, int op)
{
    if (field >= 0) {
	proxyModel->setFilter(field, id, op);
	proxyModel->enableFilter(true);
    } else {
	proxyModel->enableFilter(false);
    }
    ReloadTable();
}
*/

int Browse::getSelectedItem()
{
    if (view == NULL) return -1;
    QModelIndexList indexes = view->selectionModel()->selection().indexes();
    if (indexes.count() <= 0) return -1;

    return indexes.at(0).data().toInt();
}


void Browse::slotUpdate(int area, int op, int item_id)
{
    //qDebug() << __FUNCTION__ << area;
    if (area == table_type) {
        // only remember the last active item if a new item was inserted or if an item was updated
        if (op == myDB::OP_ANY || op == myDB::OP_DELETE) last_active_item = -1;
        else last_active_item = item_id;
        ReloadTable();
    }
}


void Browse::slotDisplayFilter(QString s, int column)
{
    if (s.isEmpty()) return;
    if (tableFilter == NULL) tableFilter = new TableFilter(table_type);

    // filter out unwanted characters to avoid invalid queries or SQL injection
    // FIXME: implement a smarter and safer way
    if (s.contains("'")) s = s.replace("'", "");
    if (s.contains(";")) s = s.replace(";", "");
    if (s.contains("%")) s = s.replace("%", "");
    if (s.contains("/")) s = s.replace("/", "");
    if (s.contains("--")) s = s.replace("--", "");
    if (s.isEmpty()) return;

    s = s.replace("*", "");


    QStringList l = tableFilter->getTableItems();

    // column parameter will not consider the "id" column
    // => we have to pay attention here (add 1)
    if ( (column >= 0) && (column < l.count() - 1) ) {
        tableFilter->clear();
        tableFilter->addFilterExpr(false, column + 1, TableFilter::OP_CONTAIN, s);
        ReloadTable();
    }
}


void Browse::slotDataArrived()
{
    if (table_type == myDB::TABLE_PATIENTS || table_type == myDB::TABLE_RECEIPES) {
        //qDebug() << __FUNCTION__;
        view->stopAnimation();
    }
    hideHelperColumns();
    proxyModel->sort(sort_column, (Qt::SortOrder)sort_order);
    if (last_active_item >= 0) {
        // search for last active item and mark it as selected
        // TODO: is it possible to speed up the lookup process?
        QModelIndex idx = proxyModel->index(0, 0);
        QModelIndexList qil = proxyModel->match(idx, Qt::DisplayRole, QVariant::fromValue(last_active_item));
        if (qil.count() > 0) {
            view->setCurrentIndex(qil.at(0));
            // scrollTo does not work, that's why it's not called here at the moment
            //view->scrollTo(view->currentIndex());
        }
    }

    if (displayFilter.useAutoKeyboardFocus()) displayFilter.setKeyboardFocus();
}


int Browse::getMode()
{
    return curMode;
}

