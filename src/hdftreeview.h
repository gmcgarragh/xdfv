/*******************************************************************************
 *
 *    Copyright (C) 2015-2018 Greg McGarragh <greg.mcgarragh@colostate.edu>
 *
 *    This source code is licensed under the GNU General Public License (GPL),
 *    Version 3.  See the file COPYING for more details.
 *
 ******************************************************************************/

#ifndef HDFTREEVIEW_H
#define HDFTREEVIEW_H

#include <netcdf.h>
#include <hdf.h>
#include <mfhdf.h>

#include <hdfprocessor.h>

#include <qstandarditemmodel.h>

#include "xdftreeview.h"


class HDFTreeViewItem;


class HDFTreeView : public XDFTreeView, HDFProcessor
{
    Q_OBJECT

private:
    const char *file_name;

    int sd_id;

    QColor file_color;
    QColor v_color;
    QColor sd_color;
    QColor vs_color;
    QColor dim_color;
    QColor attr_color;

    QStandardItemModel *model;

    void *functionSDDim(const void *parent, const void *after,
                        int dim_index, int32 dim_id, const int32 *flags);
    void *functionSDAttrs(const void *parent, const void *after,
                          int32 id, int32 attr_index, const int32 *flags);
    void *functionSDIndex(const void *parent, const void *after,
                          int32 sds_index, int32 sds_id, const int32 *flags);
    void *functionVRef(const void *parent, const void *after, void **after2,
                       int32 vgroup_id, const int32 *flags);
    void *functionVSRef(const void *parent, const void *after,
                        int32 vdata_id, const int32 *flags);

    void colorize(QTreeWidgetItem *item, bool color);

public:
    HDFTreeView(const char *file_name, int sds, QWidget *parent = 0);
    ~HDFTreeView();

public slots:
    void showDataTable();
    void showDataTable(HDFTreeViewItem *item, int column);
};


class HDFTreeViewItem : public XDFTreeViewItem
{
public:
    enum ItemType {
        File,
        VGroup,
        Dataset,
        VData,
        Dimension,
        Attribute
    };

private:
    ItemType type_;

public:
    HDFTreeViewItem(HDFTreeView *parent, ItemType type_,
                    const char *name_);
    HDFTreeViewItem(HDFTreeViewItem *parent, HDFTreeViewItem *after,
                    ItemType type_, const char *name_);
    ~HDFTreeViewItem();

    ItemType type();
};

#endif /* HDFTREEVIEW_H */
