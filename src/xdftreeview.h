/*******************************************************************************
 *
 *    Copyright (C) 2015-2018 Greg McGarragh <greg.mcgarragh@colostate.edu>
 *
 *    This source code is licensed under the GNU General Public License (GPL),
 *    Version 3.  See the file COPYING for more details.
 *
 ******************************************************************************/

#ifndef XDFTREEVIEW_H
#define XDFTREEVIEW_H

#include <qstandarditemmodel.h>
#include <qtreewidget.h>


class XDFTreeViewItem;


class XDFTreeView : public QTreeWidget
{
    Q_OBJECT

    QStandardItemModel *model;

private:
    bool is_colorized;

    void mousePressEvent(QMouseEvent *event);

    void colorizeAll(bool color);
    void colorizeAll(QTreeWidgetItem *item, bool color);

protected:
    virtual void colorize(QTreeWidgetItem *item, bool color);

public:
    XDFTreeView(QWidget *parent = 0);
    ~XDFTreeView();

public slots:
    void copyItemName();
    void copyItemName(XDFTreeViewItem *item, int column);

    void find(QString &name);
/*
    void find(QTreeWidgetItem *item, QString &name);
*/
    void findPrev(QString &name);

    void selectAll(QString &name);
/*
    bool selectAll(QTreeWidgetItem *item, QString &name, bool flag);
*/
    void expandAll();
/*
    void expandAll(QTreeWidgetItem *item);
*/
    void collapseAll();
/*
    void collapseAll(QTreeWidgetItem *item);
*/
    void prepareMenu(const QPoint &pos);
    virtual void showDataTable();
    virtual void showDataTable(XDFTreeViewItem *item, int column);

    void setFontSize(int size);
    void changeFontSize(int delta);

    void setColorized(bool colorize);
};


class XDFTreeViewItem : public QTreeWidgetItem
{
private:
    bool has_data_table;

public:
    char *name;

    XDFTreeViewItem(XDFTreeView *parent, const char *name_);
    XDFTreeViewItem(XDFTreeViewItem *parent, XDFTreeViewItem *after, const char *name_);
    ~XDFTreeViewItem();

    bool hasDataTable();
    void setHasDataTable(bool has);
};

#endif /* XDFTREEVIEW_H */
