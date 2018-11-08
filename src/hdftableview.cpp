/*******************************************************************************
 *
 *    Copyright (C) 2015-2018 Greg McGarragh <greg.mcgarragh@colostate.edu>
 *
 *    This source code is licensed under the GNU General Public License (GPL),
 *    Version 3.  See the file COPYING for more details.
 *
 ******************************************************************************/

#include <ghdf.h>

#include "xdfv.h"
#include "hdftableview.h"


HDFTableView::HDFTableView(int32 sd_id, const char *sds_name, QWidget *parent)
    : XDFTableView(parent), sd_id(sd_id), sds_name(sds_name)
{
    int32 sds_index;
    int32 sds_id;

    int32 rank;
    int32 dim_sizes[MAX_VAR_DIMS];

    int32 data_type;
    int32 num_attrs;

    sds_index = SDnametoindex(sd_id, sds_name);
    if (sds_index == FAIL) {
        fprintf(stderr, "ERROR: SDnametoindex(), sds_name = %s\n", sds_name);
        exit(1);
    }

    sds_id = SDselect(sd_id, sds_index);
    if (sds_id == FAIL) {
        fprintf(stderr, "ERROR: SDselect()\n");
        exit(1);
    }

    if (SDgetinfo(sds_id, NULL, &rank, dim_sizes, &data_type, &num_attrs) == FAIL) {
        fprintf(stderr, "ERROR: SDgetinfo()\n");
        exit(1);
    }

    if (SDendaccess(sds_id) == FAIL) {
        fprintf(stderr, "ERROR: SDgetinfo(), sds_name = %s\n", sds_name);
        exit(1);
    }

    buildWidget(sds_name, rank);

    refreshTable();
}



HDFTableView::~HDFTableView()
{

}



int HDFTableView::parseSlice(int32 n_dims, const int32 *dims,
                             int *i_row, int *n_rows, int *i_col, int *n_cols,
                             int32 *offset, int32 *count, int32 *length)
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



void HDFTableView::refreshTable()
{
    char *temp;

    int i;
    int j;

    int data_size;

    int i_row;
    int n_rows;
    int i_col;
    int n_cols;

    void *data;
    void *ptr;

    int32 sds_index;
    int32 sds_id;

    int32 rank;
    int32 dim_sizes[MAX_VAR_DIMS];

    int32 data_type;
    int32 num_attrs;

    int32 start[MAX_VAR_DIMS];
    int32 edge [MAX_VAR_DIMS];

    int32 length;

    temp = (char *) malloc(LN * sizeof(char));

    sds_index = SDnametoindex(sd_id, sds_name);
    if (sds_index == FAIL) {
        fprintf(stderr, "ERROR: SDnametoindex(), sds_name = %s\n", sds_name);
        exit(1);
    }

    sds_id = SDselect(sd_id, sds_index);
    if (sds_id == FAIL) {
        fprintf(stderr, "ERROR: SDselect()\n");
        exit(1);
    }

    if (SDgetinfo(sds_id, NULL, &rank, dim_sizes, &data_type, &num_attrs) == FAIL) {
        fprintf(stderr, "ERROR: SDgetinfo()\n");
        exit(1);
    }

    if (! parseSlice(rank, dim_sizes, &i_row, &n_rows, &i_col, &n_cols, start, edge, &length)) {
        data_size = hdf_data_type_size(data_type);
        if (data_size == 0) {
            fprintf(stderr, "ERROR: hdf_data_type_size(), sds_name = %s\n", sds_name);
            exit(1);
        }

        data = malloc(length * data_size);
        if (data == NULL) {
            fprintf(stderr, "ERROR: Memory allocation failed, sds_name = %s\n", sds_name);
            exit(1);
        }

        if (SDreaddata(sds_id, start, NULL, edge, data) == FAIL) {
            fprintf(stderr, "ERROR: SDreaddata(), sds_name = %s\n", sds_name);
            exit(1);
        }

        configureTable(i_row, n_rows, i_col, n_cols);

        for (i = 0; i < n_rows; ++i) {
            for (j = 0; j < n_cols; ++j) {
                ptr = ((char *) data) + (i * n_cols + j) * data_size;
                hdf_scaler_to_string(data_type, ptr, 0, temp, LN);
                tableWidget()->setItem(i, j, new QTableWidgetItem(temp));
            }
        }

        free(data);
    }

    if (SDendaccess(sds_id) == FAIL) {
        fprintf(stderr, "ERROR: SDgetinfo(), sds_name = %s\n", sds_name);
        exit(1);
    }

    free(temp);
}
