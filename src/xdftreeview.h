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

#include "xdfv.h"


class XDFTreeViewItem;


class XDFTreeView : public QTreeWidget
{
    Q_OBJECT

    QStandardItemModel *model;

private:
    char *file_name;
    XDFV::FileType file_type;

    void mousePressEvent(QMouseEvent *event);

protected:
    virtual void colorize(QTreeWidgetItem *item, bool color);

public:
    XDFTreeView(const char *file_name, XDFV::FileType file_type, QWidget *parent = 0);
    ~XDFTreeView();

    const char *filename();
    XDFV::FileType fileType();

    virtual void load();

public slots:
    void copyItemName();
    void copyItemName(XDFTreeViewItem *item, int column);

    void find(QString &name);
    void findPrev(QString &name);
    void selectAll(QString &name);

    void expandAll();
    void collapseAll();
    void colorizeAll(bool color);

    void prepareMenu(const QPoint &pos);
    virtual void showDataTable();
    virtual void showDataTable(XDFTreeViewItem *item, int column);

    void setFontSize(int size);
    void changeFontSize(int delta);
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
