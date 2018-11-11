/*******************************************************************************
 *
 *    Copyright (C) 2015-2018 Greg McGarragh <greg.mcgarragh@colostate.edu>
 *
 *    This source code is licensed under the GNU General Public License (GPL),
 *    Version 3.  See the file COPYING for more details.
 *
 ******************************************************************************/

#ifndef NCTREEVIEW_H
#define NCTREEVIEW_H

#include <netcdf.h>

#include <ncprocessor.h>

#include <qstandarditemmodel.h>

#include "xdftreeview.h"


class NCTreeViewItem;


class NCTreeView : public XDFTreeView, NCProcessor
{
    Q_OBJECT

private:
    QColor file_color;
    QColor var_color;
    QColor dim_color;
    QColor attr_color;

    QStandardItemModel *model;

    void *functionDim(const void *parent, const void *after,
                      int dim_id, const int *flags);
    void *functionAttrs(const void *parent, const void *after,
                        int id, int att_num, const int *flags);
    void *functionVarID(const void *parent, const void *after,
                        int var_id, const int *flags);

    void colorize(QTreeWidgetItem *item, bool color);

public:
    NCTreeView(const char *file_name, QWidget *parent = 0);
    ~NCTreeView();

    void load();

public slots:
    void showDataTable();
    void showDataTable(NCTreeViewItem *item, int column);
};


class NCTreeViewItem : public XDFTreeViewItem
{
public:
    enum ItemType {
        File,
        Variable,
        Dimension,
        Attribute
    };

private:
    ItemType type_;

public:
    NCTreeViewItem(NCTreeView *parent, ItemType type_,
                   const char *name_);
    NCTreeViewItem(NCTreeViewItem *parent, NCTreeViewItem *after,
                   ItemType type_, const char *name_);
    ~NCTreeViewItem();

    ItemType type();
};

#endif /* NCTREEVIEW_H */
