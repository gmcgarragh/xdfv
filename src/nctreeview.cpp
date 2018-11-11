/*******************************************************************************
 *
 *    Copyright (C) 2015-2018 Greg McGarragh <greg.mcgarragh@colostate.edu>
 *
 *    This source code is licensed under the GNU General Public License (GPL),
 *    Version 3.  See the file COPYING for more details.
 *
 ******************************************************************************/

#include <gnetcdf.h>

#include <qheaderview.h>

#include "xdfv.h"
#include "nctableview.h"
#include "nctreeview.h"


enum tree_view_fields {
    FIELD_Name,
    FIELD_Description,
    FIELD_Var_ID,
    FIELD_Data_Type,
    FIELD_Dimensions,
    FIELD_Deflate,
    FIELD_Chunking,
    FIELD_Value,

    FIELD_END
};


NCTreeViewItem::NCTreeViewItem(NCTreeView *parent, ItemType type,
                               const char *name)
    : XDFTreeViewItem(parent, name), type_(type)
{

}



NCTreeViewItem::NCTreeViewItem(NCTreeViewItem *parent, NCTreeViewItem *after,
                               ItemType type, const char *name)
    : XDFTreeViewItem(parent, after, name), type_(type)
{

}



NCTreeViewItem::~NCTreeViewItem()
{

}



NCTreeViewItem::ItemType NCTreeViewItem::type()
{
    return type_;
}



NCTreeView::NCTreeView(const char *file_name, QWidget *parent)
    : XDFTreeView(file_name, XDFV::NetCDF, parent)
{
/*
    nc_id2 = -1;
*/
    load();
}



void NCTreeView::load()
{
    char *temp;

    int status;

    NCTreeViewItem *item;

    temp = (char *) malloc(LN * sizeof(char));

    setHeaderLabels(QStringList() << "Name"
                                  << "Description"
                                  << "ID"
                                  << "Data Type"
                                  << "Dimensions"
                                  << "Chunking"
                                  << "Deflate"
                                  << "Values from the first leading edge");


    header()->resizeSection(FIELD_Name, 350);
    header()->resizeSection(FIELD_Value, 4096);

    setColumnHidden(FIELD_Name,        false);
    setColumnHidden(FIELD_Description, false);
    setColumnHidden(FIELD_Var_ID,      false);
    setColumnHidden(FIELD_Data_Type,   false);
    setColumnHidden(FIELD_Dimensions,  false);
    setColumnHidden(FIELD_Chunking,    false);
    setColumnHidden(FIELD_Deflate,     false);
    setColumnHidden(FIELD_Value,       false);

    file_color = QColor(  0,   0,   0);
    var_color  = QColor(  0, 224,   0);
    dim_color  = QColor(224,   0, 224);
    attr_color = QColor(  0,   0, 224);

    snprintf(temp, LN, "%s - %s", program_name, filename());
    setWindowTitle(temp);

    item = new NCTreeViewItem(this, NCTreeViewItem::File, filename());
    item->setText(0, filename());

    status = procNCFile(filename(), NULL, item);
    if (status != 0)
        throw status;

    header()->resizeSection(0, 350);

    free(temp);
}



NCTreeView::~NCTreeView()
{

}



void *NCTreeView::functionDim(const void *parent, const void *after,
                              int dim_id, const int *flags)
{
    char *temp;

    char dim_name[MAX_NC_NAME];

    int status;

    size_t length;

    NCTreeViewItem *item;

    temp = (char *) malloc(LN * sizeof(char));

    status = nc_inq_dim(nc_id, dim_id, dim_name, &length);
    if (status != NC_NOERR) {
        fprintf(stderr, "ERROR: nc_inq_dim(), %s\n", nc_strerror(status));
        return NULL;
    }

    item = new NCTreeViewItem((NCTreeViewItem *) parent, (NCTreeViewItem *) after,
                              NCTreeViewItem::Dimension, dim_name);

    item->setText(FIELD_Name, dim_name);
    item->setText(FIELD_Description, "Dimension");

    snprintf(temp, LN, "%d", dim_id);
    item->setText(FIELD_Var_ID, temp);

    snprintf(temp, LN, "%ld", (long) length);
    item->setText(FIELD_Value, temp);

    free(temp);

    return (void *) item;
}



void *NCTreeView::functionAttrs(const void *parent, const void *after,
                                int id, int att_num, const int *flags)
{
    char *temp;

    char att_name[MAX_NC_NAME];

    int n;

    int status;

    void *data;

    size_t length;

    nc_type xtype;

    NCTreeViewItem *item;

    temp = (char *) malloc(LN * sizeof(char));

    status = nc_inq_attname(nc_id, id, att_num, att_name);
    if (status != NC_NOERR) {
        fprintf(stderr, "ERROR: nc_inq_attname(), %s\n", nc_strerror(status));
        return NULL;
    }

    status = nc_inq_att(nc_id, id, att_name, &xtype, &length);
    if (status != NC_NOERR) {
        fprintf(stderr, "ERROR: nc_inq_att(), %s\n", nc_strerror(status));
        return NULL;
    }

    data = (void *) malloc(netcdf_data_type_size(xtype) * length);
    if (data == NULL) {
        fprintf(stderr, "ERROR: Memory allocation failed, att_name = %s\n", att_name);
        return NULL;
    }

    status = nc_get_att(nc_id, id, att_name, data);
    if (status != NC_NOERR) {
        fprintf(stderr, "ERROR: nc_get_att(), %s\n", nc_strerror(status));
        return NULL;
    }

    item = new NCTreeViewItem((NCTreeViewItem *) parent, (NCTreeViewItem *) after,
                              NCTreeViewItem::Attribute, att_name);

    item->setText(FIELD_Name, att_name);
    item->setText(FIELD_Description, "Attribute");

    snprintf(temp, LN, "%d", att_num);
    item->setText(FIELD_Var_ID, temp);

    item->setText(FIELD_Data_Type, netcdf_data_type_name(xtype));

    snprintf(temp, LN, "%ld", (long) length);
    item->setText(FIELD_Dimensions, temp);

    n = netcdf_array_to_string(xtype, data, length, temp, LN);
    if (n < 0) {
        fprintf(stderr, "ERROR: netcdf_data_to_string(), att_name = %s\n", att_name);
        return NULL;
    }
    if (xtype == NC_CHAR) {
        for (int i = 0; i < n; ++i) {
            if (temp[i] == '\n')
                temp[i] = '\\';
        }
    }

    item->setText(FIELD_Value, temp);

    free(data);
    free(temp);

    return (void *) item;
}



void *NCTreeView::functionVarID(const void *parent, const void *after,
                                int var_id, const int *flags)
{
    char *temp;

    char var_name[MAX_NC_NAME];

    int i;
    int n;

    int status;

    int n_dims;
    int dim_ids[NC_MAX_VAR_DIMS];

    int n_atts;

    int data_size;

    int shuffle;
    int deflate;
    int deflate_level;

    int storage;

    void *data;

    size_t length;

    size_t dimlen[MAX_VAR_DIMS];

    size_t chunksizesp[MAX_VAR_DIMS];

    size_t start[MAX_VAR_DIMS];
    size_t count[MAX_VAR_DIMS];

    nc_type xtype;

    NCTreeViewItem *item;

    temp = (char *) malloc(LN * sizeof(char));

    status = nc_inq_var(nc_id, var_id, var_name, &xtype, &n_dims, dim_ids, &n_atts);
    if (status != NC_NOERR) {
        fprintf(stderr, "ERROR: nc_inq_var(), %s\n", nc_strerror(status));
        return NULL;
    }

    item = new NCTreeViewItem((NCTreeViewItem *) parent, (NCTreeViewItem *) after,
                              NCTreeViewItem::Variable, var_name);
    item->setHasDataTable(true);

    item->setText(FIELD_Name, var_name);

    item->setText(FIELD_Description, "Variable");

    snprintf(temp, LN, "%d", var_id);
    item->setText(FIELD_Var_ID, temp);

    item->setText(FIELD_Data_Type, netcdf_data_type_name(xtype));

    n = 0;
    for (int i = 0; i < n_dims; ++i) {
        status = nc_inq_dimlen(nc_id, dim_ids[i], &dimlen[i]);
        if (status != NC_NOERR) {
            fprintf(stderr, "ERROR: nc_inq_dimlen(), %s\n", nc_strerror(status));
            return NULL;
        }

        n += snprintf(temp+n, LN - n, "%ld", (long) dimlen[i]);

        if (i < n_dims-1)
            n += snprintf(temp+n, LN - n, ", ");
    }

    item->setText(FIELD_Dimensions, temp);

    status = nc_inq_var_chunking(nc_id, var_id, &storage, chunksizesp);
    if (status != NC_NOERR) {
        fprintf(stderr, "ERROR: nc_inq_var_chunking(), %s\n", nc_strerror(status));
        return NULL;
    }

    n = 0;
    if (storage == NC_CONTIGUOUS)
        n += snprintf(temp+n, LN - n, "Contiguous");
    else if (storage == NC_CHUNKED) {
        n += snprintf(temp+n, LN - n, "Chunked");
        for (int i = 0; i < n_dims; ++i)
            n += snprintf(temp+n, LN - n, ", %ld", chunksizesp[i]);
    }
    else {
        fprintf(stderr, "ERROR: nc_inq_var_chunking(), invalid chunking value: %d\n", storage);
        return NULL;
    }

    item->setText(FIELD_Chunking, temp);

    status = nc_inq_var_deflate(nc_id, var_id, &shuffle, &deflate, &deflate_level);
    if (status != NC_NOERR) {
        fprintf(stderr, "ERROR: nc_inq_var_deflate(), %s\n", nc_strerror(status));
        return NULL;
    }

    n = 0;
    n += snprintf(temp+n, LN - n, "%d,", shuffle);
    n += snprintf(temp+n, LN - n, "%d,", deflate);
    n += snprintf(temp+n, LN - n, "%d", deflate_level);

    item->setText(FIELD_Deflate, temp);

    length = dimlen[n_dims - 1];
/*
    length = MIN(64, dimlen[n_dims - 1]);
*/
    data_size = netcdf_data_type_size(xtype);
    if (data_size == 0) {
        fprintf(stderr, "ERROR: netcdf_data_type_size(), var_name = %s\n", var_name);
        exit(1);
    }

    data = malloc(length * data_size);
    if (data == NULL) {
        fprintf(stderr, "ERROR: Memory allocation failed, var_name = %s\n", var_name);
        return NULL;
    }

    for (i = 0; i < n_dims; ++i) {
        start[i] = 0;
        count[i] = 1;
    }

    count[i - 1] = length;

    status = nc_get_vara(nc_id, var_id, start, count, data);
    if (status != NC_NOERR) {
        fprintf(stderr, "ERROR: nc_get_vara(), %s\n", nc_strerror(status));
        return NULL;
    }

    n = netcdf_array_to_string(xtype, data, length, temp, LN);
    if (n < 0) {
        fprintf(stderr, "ERROR: netcdf_data_to_string(), var_name = %s\n", var_name);
        return NULL;
    }
    if (xtype == NC_CHAR) {
        for (int i = 0; i < n; ++i) {
            if (temp[i] == '\n')
                temp[i] = '\\';
        }
    }

    item->setText(FIELD_Value, temp);

    free(data);
    free(temp);

    return (void *) item;
}



void NCTreeView::colorize(QTreeWidgetItem *item_, bool color)
{
    NCTreeViewItem *item = (NCTreeViewItem *) item_;

    if (! color)
        item->setTextColor(FIELD_Name, QColor(0, 0, 0));
    else {
        switch(item->type()) {
            case NCTreeViewItem::File:
                item->setTextColor(FIELD_Name, file_color);
                break;
            case NCTreeViewItem::Variable:
                item->setTextColor(FIELD_Name, var_color);
                break;
            case NCTreeViewItem::Attribute:
                item->setTextColor(FIELD_Name, attr_color);
                break;
            case NCTreeViewItem::Dimension:
                item->setTextColor(FIELD_Name, dim_color);
                break;
            default:
                break;
        }
    }
}



void NCTreeView::showDataTable()
{
    showDataTable((NCTreeViewItem *) currentItem(), 0);
}



void NCTreeView::showDataTable(NCTreeViewItem *item, int column)
{
    if (item->type() != NCTreeViewItem::Variable)
        return;

    NCTableView *t = new NCTableView(filename(), item->name, 0);
    t->setAttribute(Qt::WA_QuitOnClose, false);
    t->setAttribute(Qt::WA_DeleteOnClose, true);
    t->show();
}
