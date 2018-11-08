/*******************************************************************************
 *
 *    Copyright (C) 2015-2018 Greg McGarragh <greg.mcgarragh@colostate.edu>
 *
 *    This source code is licensed under the GNU General Public License (GPL),
 *    Version 3.  See the file COPYING for more details.
 *
 ******************************************************************************/

#include <ghdf5.h>

#include "xdfv.h"
#include "hdf5tableview.h"


HDF5TableView::HDF5TableView(hid_t file_id, const char *dataset_name_,
                             QWidget *parent)
    : XDFTableView(parent), file_id(file_id)
{
    int n_dims;

    hid_t dataset_id;
    hid_t dataspace_id;

    dataset_name = dataset_name_;

    dataset_id = H5Dopen(file_id, dataset_name, H5P_DEFAULT);
    if (dataset_id < 0) {
        fprintf(stderr, "ERROR: H5Dopen(), dataset_name = %s\n", dataset_name);
        exit(1);
    }

    dataspace_id = H5Dget_space(dataset_id);
    if (dataspace_id < 0) {
        fprintf(stderr, "ERROR: H5Dget_space(), dataset_name = %s\n", dataset_name);
        exit(1);
    }

    n_dims = H5Sget_simple_extent_ndims(dataspace_id);
    if (n_dims < 0) {
        fprintf(stderr, "ERROR: H5Sget_simple_extent_ndims(), dataset_name = %s\n", dataset_name);
        exit(1);
    }

    if (H5Sclose(dataspace_id) < 0) {
        fprintf(stderr, "ERROR: H5Sclose(), dataset_name = %s\n", dataset_name);
        exit(1);
    }

    if (H5Dclose(dataset_id) < 0) {
        fprintf(stderr, "ERROR: H5Dclose(), dataset_name = %s\n", dataset_name);
        exit(1);
    }

    buildWidget(dataset_name, n_dims);

    refreshTable();
}



HDF5TableView::~HDF5TableView()
{

}



int HDF5TableView::parseSlice(int n_dims, const hsize_t *dims,
                              int *i_row, int *n_rows, int *i_col, int *n_cols,
                              hsize_t *offset, hsize_t *count, hsize_t *length)
{
    int i;

    int r = 0;

    size_t *dims_   = (size_t *) malloc(n_dims * sizeof(size_t));
    size_t *offset_ = (size_t *) malloc(n_dims * sizeof(size_t));
    size_t *count_  = (size_t *) malloc(n_dims * sizeof(size_t));
    size_t length_;

    for (i = 0; i < n_dims; ++i)
        dims_[i] = dims[i];

    if (! XDFTableView::parseSlice(n_dims, dims_, i_row, n_rows, i_col, n_cols,
                                   offset_, count_, &length_)) {
        for (i = 0; i < n_dims; ++i) {
            offset[i] = offset_[i];
            count [i] = count_ [i];
        }

        *length = length_;
    }
    else
        r = -1;

    free(dims_);
    free(offset_);
    free(count_);

    return r;
}



void HDF5TableView::refreshTable()
{
    char *temp;

    int i;
    int j;

    int i_row;
    int n_rows;
    int i_col;
    int n_cols;

    int n_dims;

    size_t data_size;

    void *data;
    void *ptr;

    hid_t dataset_id;
    hid_t datatype_id;
    hid_t filespace_id;
    hid_t memspace_id;

    hsize_t length;

    hsize_t *dims;
    hsize_t offset[8];
    hsize_t count[8];

    H5T_class_t data_class;

    temp = (char *) malloc(LN * sizeof(char));

    dataset_id = H5Dopen(file_id, dataset_name, H5P_DEFAULT);
    if (dataset_id < 0) {
        fprintf(stderr, "ERROR: H5Dopen(), dataset_name = %s\n", dataset_name);
        exit(1);
    }

    datatype_id = H5Dget_type(dataset_id);
    if (datatype_id < 0) {
        fprintf(stderr, "ERROR: H5Dget_type(), dataset_name = %s\n", dataset_name);
        exit(1);
    }

    data_class = H5Tget_class(datatype_id);
    if (data_class < 0) {
        fprintf(stderr, "ERROR: H5Tget_size(), attr_name = %s\n", dataset_name);
        exit(1);
    }

    data_size = H5Tget_size(datatype_id);
    if (data_size == 0) {
        fprintf(stderr, "ERROR: H5Tget_size(), attr_name = %s\n", dataset_name);
        exit(1);
    }

    filespace_id = H5Dget_space(dataset_id);
    if (filespace_id < 0) {
        fprintf(stderr, "ERROR: H5Dget_space(), dataset_name = %s\n", dataset_name);
        exit(1);
    }

    n_dims = H5Sget_simple_extent_ndims(filespace_id);
    if (n_dims < 0) {
        fprintf(stderr, "ERROR: H5Sget_simple_extent_ndims(), dataset_name = %s\n", dataset_name);
        exit(1);
    }
    dims = (hsize_t *) malloc(n_dims * sizeof(hsize_t));
    n_dims = H5Sget_simple_extent_dims(filespace_id, dims, NULL);
    if (n_dims < 0) {
        fprintf(stderr, "ERROR: H5Sget_simple_extent_dims(), dataset_name = %s\n", dataset_name);
        exit(1);
    }

    if (! parseSlice(n_dims, dims, &i_row, &n_rows, &i_col, &n_cols, offset, count, &length)) {

        memspace_id = H5Screate_simple(n_dims, count, NULL);
        if (memspace_id < 0) {
            fprintf(stderr, "ERROR: H5Screate_simple(), dataset_name = %s\n", dataset_name);
            exit(1);
        }

        data = malloc(length * data_size);
        if (data == NULL) {
            fprintf(stderr, "ERROR: Memory allocation failed, attr_name = %s\n", dataset_name);
            exit(1);
        }

        if (n_dims > 0) {
            if (H5Sselect_hyperslab(filespace_id, H5S_SELECT_SET, offset, NULL, count, NULL) < 0) {
                fprintf(stderr, "ERROR: H5Sselect_hyperslab(), dataset_name = %s\n", dataset_name);
                exit(1);
            }
        }

        if (H5Dread(dataset_id, datatype_id, memspace_id, filespace_id, H5P_DEFAULT, data) < 0) {
            fprintf(stderr, "ERROR: H5Dread(), dataset_name = %s\n", dataset_name);
            exit(1);
        }

        configureTable(i_row, n_rows, i_col, n_cols);

        for (i = 0; i < n_rows; ++i) {
            for (j = 0; j < n_cols; ++j) {
                ptr = ((char *) data) + (i * n_cols + j) * data_size;
                hdf5_scaler_to_string(datatype_id, data_class, data_size, ptr, 0, temp, LN);
                tableWidget()->setItem(i, j, new QTableWidgetItem(temp));
            }
        }

        free(data);

        if (H5Sclose(memspace_id) < 0) {
            fprintf(stderr, "ERROR: H5Sclose(), dataset_name = %s\n", dataset_name);
            exit(1);
        }
    }

    free(dims);

    if (H5Sclose(filespace_id) < 0) {
        fprintf(stderr, "ERROR: H5Sclose(), dataset_name = %s\n", dataset_name);
        exit(1);
    }

    if (H5Tclose(datatype_id) < 0) {
        fprintf(stderr, "ERROR: H5Tclose(), dataset_name = %s\n", dataset_name);
        exit(1);
    }

    if (H5Dclose(dataset_id) < 0) {
        fprintf(stderr, "ERROR: H5Dclose(), dataset_name = %s\n", dataset_name);
        exit(1);
    }

    free(temp);
}
