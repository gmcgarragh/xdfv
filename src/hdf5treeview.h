/*******************************************************************************
 *
 *    Copyright (C) 2015-2018 Greg McGarragh <greg.mcgarragh@colostate.edu>
 *
 *    This source code is licensed under the GNU General Public License (GPL),
 *    Version 3.  See the file COPYING for more details.
 *
 ******************************************************************************/

#ifndef HDF5TREEVIEW_H
#define HDF5TREEVIEW_H

#include <hdf5.h>

#include <hdf5processor.h>

#include <qstandarditemmodel.h>

#include "xdftreeview.h"


class HDF5TreeViewItem;


class HDF5TreeView : public XDFTreeView, HDF5Processor
{
    Q_OBJECT

private:
    hid_t file_id;

    QColor file_color;
    QColor group_color;
    QColor dataset_color;
    QColor attr_color;
    QColor dataspace_color;

    QStandardItemModel *model;

    void *functionH5A(const void *parent, const void *after,
                      hid_t attr_id, const char *attr_name);
    void *functionH5D(const void *parent, const void *after,
                      hid_t dataset_id, const char *dataset_name);
    void *functionH5G(const void *parent, const void *after,
                      hid_t group_id, const char *group_name);

    void colorize(QTreeWidgetItem *item, bool color);

public:
    HDF5TreeView(const char *file_name_, QWidget *parent = 0);
    ~HDF5TreeView();

    void load();

public slots:
    void showDataTable();
    void showDataTable(HDF5TreeViewItem *item, int column);
};


class HDF5TreeViewItem : public XDFTreeViewItem
{
public:
    enum ItemType {
        File,
        Group,
        Dataset,
        Attribute
    };

private:
    ItemType type_;

public:
    HDF5TreeViewItem(HDF5TreeView *parent, ItemType type_, const char *name_);
    HDF5TreeViewItem(HDF5TreeViewItem *parent, HDF5TreeViewItem *after,
                     ItemType type_, const char *name_);
    ~HDF5TreeViewItem();

    ItemType type();
};

#endif /* HDF5TREEVIEW_H */
