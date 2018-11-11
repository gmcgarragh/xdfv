/*******************************************************************************
 *
 *    Copyright (C) 2015-2018 Greg McGarragh <greg.mcgarragh@colostate.edu>
 *
 *    This source code is licensed under the GNU General Public License (GPL),
 *    Version 3.  See the file COPYING for more details.
 *
 ******************************************************************************/

#include <ghdf5.h>

#include <qheaderview.h>

#include "xdfv.h"
#include "hdf5tableview.h"
#include "hdf5treeview.h"


enum tree_view_fields {
    FIELD_Name,
    FIELD_Description,
    FIELD_N_Objects,
    FIELD_Data_Class,
    FIELD_Data_Size,
    FIELD_Dimensions,
    FIELD_Value,

    FIELD_END
};


HDF5TreeViewItem::HDF5TreeViewItem(HDF5TreeView *parent, ItemType type,
                                   const char *name)
    : XDFTreeViewItem(parent, name), type_(type)
{

}



HDF5TreeViewItem::HDF5TreeViewItem(HDF5TreeViewItem *parent, HDF5TreeViewItem *after,
                                   ItemType type, const char *name)
    : XDFTreeViewItem(parent, after, name), type_(type)
{

}



HDF5TreeViewItem::~HDF5TreeViewItem()
{

}



HDF5TreeViewItem::ItemType HDF5TreeViewItem::type()
{
    return type_;
}



HDF5TreeView::HDF5TreeView(const char *file_name, QWidget *parent)
    : XDFTreeView(file_name, XDFV::HDF5, parent)
{
    load();
}



void HDF5TreeView::load()
{
    char *temp;

    int status;

    HDF5TreeViewItem *item;

    temp = (char *) malloc(LN * sizeof(char));

    setHeaderLabels(QStringList() << "Name"
                                  << "Description"
                                  << "N Objects"
                                  << "Data Class"
                                  << "Data Size"
                                  << "Dimensions"
                                  << "Values from the first leading edge");

    header()->resizeSection(FIELD_Name,  350);
    header()->resizeSection(FIELD_Value, 4096);

    setColumnHidden(FIELD_Name,        false);
    setColumnHidden(FIELD_Description, false);
    setColumnHidden(FIELD_N_Objects,   false);
    setColumnHidden(FIELD_Data_Class,  false);
    setColumnHidden(FIELD_Data_Size,   false);
    setColumnHidden(FIELD_Dimensions,  false);
    setColumnHidden(FIELD_Value,       false);

    file_color      = QColor(  0, 0,   0);
    group_color     = QColor(224, 0,   0);
    dataset_color   = QColor(  0, 224, 0);
    attr_color      = QColor(  0, 0,   224);
    dataspace_color = QColor(244, 0,   224);

    snprintf(temp, LN, "%s - %s", program_name, filename());
    setWindowTitle(temp);

    item = new HDF5TreeViewItem(this, HDF5TreeViewItem::File, filename());
    item->setText(0, filename());

    status = procHDF5File(filename(), item);
    if (status != 0)
        throw status;

    free(temp);
}



HDF5TreeView::~HDF5TreeView()
{

}



void *HDF5TreeView::functionH5A(const void *parent, const void *after,
                                hid_t attr_id, const char *attr_name)
{
    char *temp;

    const char *class_name;

    int i;
    int n;

    int n_dims;

    size_t data_size;

    void *data;

    hid_t datatype_id;
    hid_t dataspace_id;

    hsize_t length;

    hsize_t *dims;
    hsize_t count[8];
    hsize_t offset[8];

    HDF5TreeViewItem *item;

    temp = (char *) malloc(LN * sizeof(char));
/*
    if (H5Iget_name(attr_id, temp, LN) < 0) {
        fprintf(stderr, "ERROR: H5Iget_name()\n");
        return NULL;
    }
*/
    item = new HDF5TreeViewItem((HDF5TreeViewItem *) parent, (HDF5TreeViewItem *) after,
                                HDF5TreeViewItem::Attribute, attr_name);
    item->setText(FIELD_Name, attr_name);

    item->setText(FIELD_Description, "Attribute");

    datatype_id = H5Aget_type(attr_id);
    if (datatype_id < 0) {
        fprintf(stderr, "ERROR: H5Aget_type(), attr_name = %s\n", attr_name);
        return NULL;
    }

    class_name = hdf5_data_class_name(H5Tget_class(datatype_id));
    if (class_name == NULL) {
        fprintf(stderr, "ERROR: hdf5_data_class_name(), attr_name = %s\n", attr_name);
        return NULL;
    }

    data_size = H5Tget_size(datatype_id);
    if (data_size == 0) {
        fprintf(stderr, "ERROR: H5Tget_size(), attr_name = %s\n", attr_name);
        return NULL;
    }

    snprintf(temp, LN, "%s", class_name);
    item->setText(FIELD_Data_Class, temp);

    snprintf(temp, LN, "%ld", data_size);
    item->setText(FIELD_Data_Size, temp);

    dataspace_id = H5Aget_space(attr_id);
    if (dataspace_id < 0) {
        fprintf(stderr, "ERROR: H5Aget_space(), attr_name = %s\n", attr_name);
        return NULL;
    }

    n_dims = H5Sget_simple_extent_ndims(dataspace_id);
    if (n_dims < 0) {
        fprintf(stderr, "ERROR: H5Sget_simple_extent_ndims(), attr_name = %s\n", attr_name);
        return NULL;
    }
    dims = (hsize_t *) malloc(n_dims * sizeof(hsize_t));
    n_dims = H5Sget_simple_extent_dims(dataspace_id, dims, NULL);
    if (n_dims < 0) {
        fprintf(stderr, "ERROR: H5Sget_simple_extent_dims(), attr_name = %s\n", attr_name);
        return NULL;
    }

    if (n_dims == 0)
        item->setText(FIELD_Dimensions, "Scalar");
    else {
        n = 0;
        for (i = 0; i < n_dims; ++i) {
            n += snprintf(temp+n, LN - n, "%ld", (long) dims[i]);
            if (i < n_dims - 1)
                n += snprintf(temp+n, LN - n, ", ");
        }
        item->setText(FIELD_Dimensions, temp);
    }
    if (n_dims == 0)
        length = 1;
    else
        length = MIN(64, dims[n_dims - 1]);

    data = malloc(length * data_size);
    if (data == NULL) {
        fprintf(stderr, "ERROR: Memory allocation failed, attr_name = %s\n", attr_name);
        return NULL;
    }

    if (n_dims > 0) {
        for (i = 0; i < n_dims; ++i) {
            offset[i] = 0;
            count [i] = 1;
        }

        count[i - 1] = length;

        if (H5Sselect_hyperslab(dataspace_id, H5S_SELECT_SET, offset, NULL, count, NULL) < 0) {
            fprintf(stderr, "ERROR: H5Sselect_hyperslab(), attr_name = %s\n", attr_name);
            return NULL;
        }
    }

    if (H5Aread(attr_id, datatype_id, data) < 0) {
        fprintf(stderr, "ERROR: H5Dread(), attr_name = %s\n", attr_name);
        return NULL;
    }

    if (hdf5_array_to_string(datatype_id, data, length, temp, LN) < 0) {
        fprintf(stderr, "ERROR: hdf5_data_to_string(), attr_name = %s\n", attr_name);
        return NULL;
    }
    item->setText(FIELD_Value, temp);

    free(data);
    free(dims);

    if (H5Sclose(dataspace_id) < 0) {
        fprintf(stderr, "ERROR: H5Sclose(), attr_name = %s\n", attr_name);
        return NULL;
    }

    if (H5Tclose(datatype_id) < 0) {
        fprintf(stderr, "ERROR: H5Tclose(), attr_name = %s\n", attr_name);
        return NULL;
    }

    free(temp);

    return (void *) item;
}



void *HDF5TreeView::functionH5D(const void *parent, const void *after,
                                hid_t dataset_id, const char *dataset_name)
{
    char *temp;

    const char *class_name;

    int i;
    int n;

    int flag;

    int n_attr;
    int n_dims;

    size_t data_size;

    void *data;

    hid_t datatype_id;
    hid_t dataspace_id;

    hsize_t length;

    hsize_t *dims;
    hsize_t count[8];
    hsize_t offset[8];

    HDF5TreeViewItem *item;

    temp = (char *) malloc(LN * sizeof(char));

    if (H5Iget_name(dataset_id, temp, LN) < 0) {
        fprintf(stderr, "ERROR: H5Iget_name()\n");
        return NULL;
    }

    item = new HDF5TreeViewItem((HDF5TreeViewItem *) parent, (HDF5TreeViewItem *) after,
                                HDF5TreeViewItem::Dataset, temp);
    item->setHasDataTable(true);

    item->setText(FIELD_Name, dataset_name);

    item->setText(FIELD_Description, "Dataset");

    n_attr = H5Aget_num_attrs(dataset_id);
    if (n_attr < 0) {
        fprintf(stderr, "ERROR: H5Aget_num_attrs(), dataset_name = %s\n", dataset_name);
        return NULL;
    }
    snprintf(temp, LN, "%ld", (long) n_attr);
    item->setText(FIELD_N_Objects, temp);

    datatype_id = H5Dget_type(dataset_id);
    if (datatype_id < 0) {
        fprintf(stderr, "ERROR: H5Dget_type(), dataset_name = %s\n", dataset_name);
        return NULL;
    }

    class_name = hdf5_data_class_name(H5Tget_class(datatype_id));
    if (class_name == NULL) {
        fprintf(stderr, "ERROR: hdf5_data_class_name(), dataset_name = %s\n", dataset_name);
        return NULL;
    }

    data_size = H5Tget_size(datatype_id);
    if (data_size == 0) {
        fprintf(stderr, "ERROR: H5Tget_size(), dataset_name = %s\n", dataset_name);
        return NULL;
    }

    snprintf(temp, LN, "%s", class_name);
    item->setText(FIELD_Data_Class, temp);

    snprintf(temp, LN, "%ld", data_size);
    item->setText(FIELD_Data_Size, temp);

    dataspace_id = H5Dget_space(dataset_id);
    if (dataspace_id < 0) {
        fprintf(stderr, "ERROR: H5Dget_space(), dataset_name = %s\n", dataset_name);
        return NULL;
    }

    n_dims = H5Sget_simple_extent_ndims(dataspace_id);
    if (n_dims < 0) {
        fprintf(stderr, "ERROR: H5Sget_simple_extent_ndims(), dataset_name = %s\n", dataset_name);
        return NULL;
    }
    dims = (hsize_t *) malloc(n_dims * sizeof(hsize_t));
    n_dims = H5Sget_simple_extent_dims(dataspace_id, dims, NULL);
    if (n_dims < 0) {
        fprintf(stderr, "ERROR: H5Sget_simple_extent_dims(), dataset_name = %s\n", dataset_name);
        return NULL;
    }

    if (n_dims == 0)
        item->setText(FIELD_Dimensions, "Scalar");
    else {
        n = 0;
        for (i = 0; i < n_dims; ++i) {
            n += snprintf(temp+n, LN - n, "%ld", (long) dims[i]);
            if (i < n_dims - 1)
                n += snprintf(temp+n, LN - n, ", ");
        }
        item->setText(FIELD_Dimensions, temp);
    }

    flag = 0;
    for (i = 0; i < n_dims; ++i) {
        if (dims[i] == 0) {
            flag = 1;
            break;
        }
    }
    if (! flag) {
        if (n_dims == 0)
            length = 1;
        else
            length = dims[n_dims - 1];
/*
            length = MIN(64, dims[n_dims - 1]);
*/
        data = malloc(length * data_size);
        if (data == NULL) {
            fprintf(stderr, "ERROR: Memory allocation failed, dataset_name = %s\n", dataset_name);
            return NULL;
        }

        if (n_dims > 0) {
            for (i = 0; i < n_dims; ++i) {
                offset[i] = 0;
                count [i] = 1;
            }

            count[i - 1] = length;

            if (H5Sselect_hyperslab(dataspace_id, H5S_SELECT_SET, offset, NULL, count, NULL) < 0) {
                fprintf(stderr, "ERROR: H5Sselect_hyperslab(), dataset_name = %s\n", dataset_name);
                return NULL;
            }
        }

        if (H5Dread(dataset_id, datatype_id, H5S_ALL, dataspace_id, H5P_DEFAULT, data) < 0) {
            fprintf(stderr, "ERROR: H5Dread(), dataset_name = %s\n", dataset_name);
            return NULL;
        }

        if (hdf5_array_to_string(datatype_id, data, length, temp, LN) < 0) {
            fprintf(stderr, "ERROR: hdf5_data_to_string(), dataset_name = %s\n", dataset_name);
            return NULL;
        }
        item->setText(FIELD_Value, temp);

        free(data);
    }

    free(dims);

    if (H5Sclose(dataspace_id) < 0) {
        fprintf(stderr, "ERROR: H5Sclose(), dataset_name = %s\n", dataset_name);
        return NULL;
    }

    if (H5Tclose(datatype_id) < 0) {
        fprintf(stderr, "ERROR: H5Tclose(), dataset_name = %s\n", dataset_name);
        return NULL;
    }

    free(temp);

    return (void *) item;
}



void *HDF5TreeView::functionH5G(const void *parent, const void *after,
                                hid_t group_id, const char *group_name)
{
    char *temp;

    hsize_t n_obj;

    HDF5TreeViewItem *item;

    temp = (char *) malloc(LN * sizeof(char));

    if (H5Iget_name(group_id, temp, LN) < 0) {
        fprintf(stderr, "ERROR: H5Iget_name()\n");
        return NULL;
    }

    item = new HDF5TreeViewItem((HDF5TreeViewItem *) parent, (HDF5TreeViewItem *) after,
                                HDF5TreeViewItem::Group, temp);
    item->setText(FIELD_Name, group_name);

    item->setText(FIELD_Description, "Group");

    if (H5Gget_num_objs(group_id, &n_obj) < 0) {
        fprintf(stderr, "ERROR: H5Gget_num_objs(), group_name = %s\n", group_name);
        return NULL;
    }
    snprintf(temp, LN, "%ld", (long) n_obj);
    item->setText(FIELD_N_Objects, temp);

    free(temp);

    return (void *) item;
}



void HDF5TreeView::colorize(QTreeWidgetItem *item_, bool color)
{
    HDF5TreeViewItem *item = (HDF5TreeViewItem *) item_;

    if (! color)
        item->setTextColor(FIELD_Name, QColor(0, 0, 0));
    else {
        switch(item->type()) {
            case HDF5TreeViewItem::File:
                item->setTextColor(FIELD_Name, file_color);
                break;
            case HDF5TreeViewItem::Group:
                item->setTextColor(FIELD_Name, group_color);
                break;
            case HDF5TreeViewItem::Dataset:
                item->setTextColor(FIELD_Name, dataset_color);
                break;
            case HDF5TreeViewItem::Attribute:
                item->setTextColor(FIELD_Name, attr_color);
                break;
            default:
                break;
        }
    }
}



void HDF5TreeView::showDataTable()
{
    showDataTable((HDF5TreeViewItem *) currentItem(), 0);
}



void HDF5TreeView::showDataTable(HDF5TreeViewItem *item, int column)
{
    if (item->type() != HDF5TreeViewItem::Dataset)
        return;

    HDF5TableView *t = new HDF5TableView(filename(), item->name, 0);
    t->setAttribute(Qt::WA_QuitOnClose, false);
    t->setAttribute(Qt::WA_DeleteOnClose, true);
    t->show();
}
