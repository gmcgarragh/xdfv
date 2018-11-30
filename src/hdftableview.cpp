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


HDFTableView::HDFTableView(const char *file_name, const char *object_name,
                           HDFTreeViewItem::ItemType type, QWidget *parent)
    : XDFTableView(parent), file_name(file_name), object_name(object_name), type(type)
{
    char field_name_list[VSFIELDMAX * (FIELDNAMELENMAX + 1)];

    int32 sd_id;
    int32 sds_index;
    int32 sds_id;

    int32 rank;
    int32 dim_sizes[MAX_VAR_DIMS];
    int32 data_type;
    int32 num_attrs;

    int32 file_id;
    int32 vdata_ref;
    int32 vdata_id;

    int32 n_records;
    int32 n_fields;
    int32 vdata_size;

    if (type == HDFTreeViewItem::Dataset) {
        sd_id = SDstart(file_name, DFACC_READ);
        if (sd_id == FAIL) {
            fprintf(stderr, "ERROR: SDstart(), file_name = %s\n", file_name);
            exit(1);
        }

        sds_index = SDnametoindex(sd_id, object_name);
        if (sds_index == FAIL) {
            fprintf(stderr, "ERROR: SDnametoindex(), sds_name = %s\n", object_name);
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
            fprintf(stderr, "ERROR: SDgetinfo(), sds_name = %s\n", object_name);
            exit(1);
        }

        if (SDend(sd_id) == FAIL) {
            fprintf(stderr, "ERROR: SDend(), file_name = %s\n", file_name);
            exit(1);
        }
    }
    else if (type == HDFTreeViewItem::VData) {
        file_id = Hopen(file_name, DFACC_READ, DEF_NDDS);
        if (file_id == FAIL) {
            fprintf(stderr, "ERROR: Hopen(), file_name = %s\n", file_name);
            exit(1);
        }

        if (Vstart(file_id) == FAIL) {
            fprintf(stderr, "ERROR: Vstart(), file_name = %s\n", file_name);
            exit(1);
        }

        vdata_ref = VSfind(file_id, object_name);
        if (vdata_ref == FAIL) {
            fprintf(stderr, "ERROR: VSfind()\n");
            exit(1);
        }

        vdata_id = VSattach(file_id, vdata_ref, "r");
        if (vdata_id == FAIL) {
            fprintf(stderr, "ERROR: VSattach()\n");
            exit(1);
        }

        rank = 2;

        if (VSinquire(vdata_id, &n_records, NULL, NULL, &vdata_size, NULL) == FAIL) {
            fprintf(stderr, "ERROR: VSinquire()\n");
            exit(1);
        }

        n_fields = VSgetfields(vdata_id, field_name_list);
        if (n_fields == FAIL) {
            fprintf(stderr, "ERROR: VSgetfields(), vdata_name = %s\n", object_name);
            exit(1);
        }

        if (VSdetach(vdata_id) == FAIL) {
            fprintf(stderr, "ERROR: VSdetach(), vdata_name = %s\n", object_name);
            exit(1);
        }

        if (Vend(file_id) == FAIL) {
            fprintf(stderr, "ERROR: Vend(), file_name = %s\n", file_name);
            exit(1);
        }

        if (Hclose(file_id) == FAIL) {
            fprintf(stderr, "ERROR: Hclose(), file_name = %s\n", file_name);
            exit(1);
        }
    }
    else {
        fprintf(stderr, "ERROR: Unsupported HDFTreeViewItem::ItemType\n");
        exit(1);
    }

    buildWidget(object_name, rank);

    refreshTable();
}



HDFTableView::~HDFTableView()
{

}



int HDFTableView::parseSlice(int32 n_dims, const int32 *dims,
                             int *i_row, int *n_rows, int *i_col, int *n_cols,
                             int32 *offset, int32 *count, int32 *length)
{
    int r = 0;

    size_t *dims_   = (size_t *) malloc(n_dims * sizeof(size_t));
    size_t *offset_ = (size_t *) malloc(n_dims * sizeof(size_t));
    size_t *count_  = (size_t *) malloc(n_dims * sizeof(size_t));
    size_t length_;

    for (int i = 0; i < n_dims; ++i)
        dims_[i] = dims[i];

    if (! XDFTableView::parseSlice(n_dims, dims_, i_row, n_rows, i_col, n_cols,
                                   offset_, count_, &length_)) {
        for (int i = 0; i < n_dims; ++i) {
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

    char field_name_list[VSFIELDMAX * (FIELDNAMELENMAX + 1)];

    int data_size;

    int i_row;
    int n_rows;
    int i_col;
    int n_cols;

    void *data;
    void *ptr;

    int32 sd_id;
    int32 sds_index;
    int32 sds_id;

    int32 rank;
    int32 dim_sizes[MAX_VAR_DIMS];

    int32 data_type;
    int32 num_attrs;

    int32 start[MAX_VAR_DIMS];
    int32 edge [MAX_VAR_DIMS];

    int32 file_id;
    int32 vdata_ref;
    int32 vdata_id;

    int32 n_records;
    int32 n_records2;
    int32 n_fields;
    int32 vdata_size;

    int32 length;

    temp = (char *) malloc(LN * sizeof(char));

    if (type == HDFTreeViewItem::Dataset) {
        sd_id = SDstart(file_name, DFACC_READ);
        if (sd_id == FAIL) {
            fprintf(stderr, "ERROR: SDstart(), file_name = %s\n", file_name);
            exit(1);
        }

        sds_index = SDnametoindex(sd_id, object_name);
        if (sds_index == FAIL) {
            fprintf(stderr, "ERROR: SDnametoindex(), sds_name = %s\n", object_name);
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
                fprintf(stderr, "ERROR: hdf_data_type_size(), sds_name = %s\n", object_name);
                exit(1);
            }

            data = malloc(length * data_size);
            if (data == NULL) {
                fprintf(stderr, "ERROR: Memory allocation failed, sds_name = %s\n", object_name);
                exit(1);
            }

            if (SDreaddata(sds_id, start, NULL, edge, data) == FAIL) {
                fprintf(stderr, "ERROR: SDreaddata(), sds_name = %s\n", object_name);
                exit(1);
            }

            configureTable(i_row, n_rows, i_col, n_cols);

            for (int i = 0; i < n_rows; ++i) {
                for (int j = 0; j < n_cols; ++j) {
                    ptr = ((char *) data) + (i * n_cols + j) * data_size;
                    hdf_scaler_to_string(data_type, ptr, 0, temp, LN);
                    tableWidget()->setItem(i, j, new QTableWidgetItem(temp));
                }
            }

            free(data);
        }

        if (SDendaccess(sds_id) == FAIL) {
            fprintf(stderr, "ERROR: SDgetinfo(), sds_name = %s\n", object_name);
            exit(1);
        }

        if (SDend(sd_id) == FAIL) {
            fprintf(stderr, "ERROR: SDend(), file_name = %s\n", file_name);
            exit(1);
        }
    }
    else if (type == HDFTreeViewItem::VData) {
        file_id = Hopen(file_name, DFACC_READ, DEF_NDDS);
        if (file_id == FAIL) {
            fprintf(stderr, "ERROR: Hopen(), file_name = %s\n", file_name);
            exit(1);
        }

        if (Vstart(file_id) == FAIL) {
            fprintf(stderr, "ERROR: Vstart(), file_name = %s\n", file_name);
            exit(1);
        }

        vdata_ref = VSfind(file_id, object_name);
        if (vdata_ref == FAIL) {
            fprintf(stderr, "ERROR: VSfind()\n");
            exit(1);
        }

        vdata_id = VSattach(file_id, vdata_ref, "r");
        if (vdata_id == FAIL) {
            fprintf(stderr, "ERROR: VSattach()\n");
            exit(1);
        }

        if (VSinquire(vdata_id, &n_records, NULL, NULL, &vdata_size, NULL) == FAIL) {
            fprintf(stderr, "ERROR: VSinquire()\n");
            exit(1);
        }

        n_fields = VSgetfields(vdata_id, field_name_list);
        if (n_fields == FAIL) {
            fprintf(stderr, "ERROR: VSgetfields(), vdata_name = %s\n", object_name);
            exit(1);
        }

        rank = 2;
        dim_sizes[0] = n_records;
        dim_sizes[1] = n_fields;

        if (! parseSlice(rank, dim_sizes, &i_row, &n_rows, &i_col, &n_cols, start, edge, &length)) {

            data_type = VFfieldtype(vdata_id, 0);
            if (data_type == FAIL) {
                fprintf(stderr, "ERROR: VFfieldtype(), vdata_name = %s\n", object_name);
                exit(1);
            }

            length *= n_records * vdata_size;

            data = malloc(length);
            if (data == NULL) {
                fprintf(stderr, "ERROR: Memory allocation failed, sds_name = %s\n", object_name);
                exit(1);
            }

            if (VSseek(vdata_id, i_row) == FAIL) {
                fprintf(stderr, "ERROR: VSseek(), vdata_name = %s\n", object_name);
                exit(1);
            }

            n_records2 = VSread(vdata_id, (uint8 *) data, n_records - i_row, FULL_INTERLACE);
            if (n_records2 < n_records - i_row) {
                fprintf(stderr, "ERROR: VSread(), vdata_name = %s\n", object_name);
                exit(1);
            }

            QStringList h_labels;
            for (int i = i_col; i < n_cols - i_col; ++i)
                h_labels << VFfieldname(vdata_id, i);
            configureTable(i_row, n_rows, i_col, n_cols, NULL, &h_labels);

            for (int i = 0; i < n_rows; ++i) {
                for (int j = i_col; j < n_cols - i_col; ++j) {
                    ptr = ((char *) data) + (i * n_cols + j) * vdata_size;
                    hdf_scaler_to_string(data_type, ptr, 0, temp, LN);
                    tableWidget()->setItem(i, j, new QTableWidgetItem(temp));
                }
            }

            free(data);
        }

        if (VSdetach(vdata_id) == FAIL) {
            fprintf(stderr, "ERROR: VSdetach(), vdata_name = %s\n", object_name);
            exit(1);
        }

        if (Vend(file_id) == FAIL) {
            fprintf(stderr, "ERROR: Vend(), file_name = %s\n", file_name);
            exit(1);
        }

        if (Hclose(file_id) == FAIL) {
            fprintf(stderr, "ERROR: Hclose(), file_name = %s\n", file_name);
            exit(1);
        }
    }

    free(temp);
}
