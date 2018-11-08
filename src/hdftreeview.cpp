/*******************************************************************************
 *
 *    Copyright (C) 2015-2018 Greg McGarragh <greg.mcgarragh@colostate.edu>
 *
 *    This source code is licensed under the GNU General Public License (GPL),
 *    Version 3.  See the file COPYING for more details.
 *
 ******************************************************************************/

#include <ghdf.h>

#include <qheaderview.h>

#include "xdfv.h"
#include "hdftableview.h"
#include "hdftreeview.h"


enum tree_view_fields {
    FIELD_Name,
    FIELD_Description,
    FIELD_Index,
    FIELD_ID,
    FIELD_Reference_Number,
    FIELD_Data_Type,
    FIELD_Dimensions,
    FIELD_Chunking,
    FIELD_Compression,
    FIELD_Factor,
    FIELD_Error,
    FIELD_Offset,
    FIELD_Offset_Error,
    FIELD_Raw_Data_Type,
    FIELD_Label,
    FIELD_Unit,
    FIELD_Format,
    FIELD_Coordsys,
    FIELD_Value,

    FIELD_END
};


HDFTreeViewItem::HDFTreeViewItem(HDFTreeView *parent, ItemType type,
                                 const char *name)
    : XDFTreeViewItem(parent, name), type_(type)
{

}



HDFTreeViewItem::HDFTreeViewItem(HDFTreeViewItem *parent, HDFTreeViewItem *after,
                                 ItemType type, const char *name)
    : XDFTreeViewItem(parent, after, name), type_(type)
{

}



HDFTreeViewItem::~HDFTreeViewItem()
{

}



HDFTreeViewItem::ItemType HDFTreeViewItem::type()
{
    return type_;
}



HDFTreeView::HDFTreeView(const char *file_name, int sds, QWidget *parent)
    : XDFTreeView(parent), file_name(file_name)
{
    char *temp;

    int status;

    HDFTreeViewItem *item;

    temp = (char *) malloc(LN * sizeof(char));

    setHeaderLabels(QStringList() << "Name"
                                  << "Description"
                                  << "Index"
                                  << "ID"
                                  << "Ref. #"
                                  << "Data Type"
                                  << "Dimensions"
                                  << "Chunking"
                                  << "Compression"
                                  << "Factor"
                                  << "Error"
                                  << "Offset"
                                  << "Offset Error"
                                  << "Raw Data Type"
                                  << "Label"
                                  << "Unit"
                                  << "Format"
                                  << "Coordsys"
                                  << "Values from the first leading edge");


    header()->resizeSection(FIELD_Name,  350);
    header()->resizeSection(FIELD_Value, 4096);

    setColumnHidden(FIELD_Name,             false);
    setColumnHidden(FIELD_Description,      false);
    setColumnHidden(FIELD_Index,            false);
    setColumnHidden(FIELD_ID,               false);
    setColumnHidden(FIELD_Reference_Number, false);
    setColumnHidden(FIELD_Data_Type,        false);
    setColumnHidden(FIELD_Dimensions,       false);
    setColumnHidden(FIELD_Chunking,         false);
    setColumnHidden(FIELD_Compression,      false);
    setColumnHidden(FIELD_Factor,           false);
    setColumnHidden(FIELD_Error,            false);
    setColumnHidden(FIELD_Offset,           false);
    setColumnHidden(FIELD_Offset_Error,     false);
    setColumnHidden(FIELD_Raw_Data_Type,    false);
    setColumnHidden(FIELD_Value,            false);

    file_color = QColor(0,     0,   0);
    v_color    = QColor(224,   0,   0);
    sd_color   = QColor(  0, 224,   0);
    vs_color   = QColor(  0, 224,   0);
    dim_color  = QColor(224,   0, 224);
    attr_color = QColor(  0,   0, 224);

    snprintf(temp, LN, "%s - %s", program_name, file_name);
    setWindowTitle(temp);

    item = new HDFTreeViewItem(this, HDFTreeViewItem::File, file_name);

    item->setText(0, file_name);

    status = procHDFFile(file_name, NULL, item, sds);
    if (status != 0)
        throw status;

    header()->resizeSection(0, 350);

    sd_id = SDstart(file_name, DFACC_READ);
    if (sd_id == FAIL) {
        fprintf(stderr, "ERROR: SDstart(), file_name = %s\n", file_name);
        exit(1);
    }

    free(temp);
}



HDFTreeView::~HDFTreeView()
{
    if (SDend(sd_id) == FAIL) {
        fprintf(stderr, "ERROR: SDend(), file_name = %s\n", file_name);
        exit(1);
    }
}



void *HDFTreeView::functionSDDim(const void *parent, const void *after,
                                 int dim_index, int32 dim_id, const int32 *flags)
{
    char *temp;

    char dim_name[MAX_NC_NAME];

    int32 length;
    int32 data_type;
    int32 num_attrs;

    HDFTreeViewItem *item;

    temp = (char *) malloc(LN * sizeof(char));

    if (SDdiminfo(dim_id, dim_name, &length, &data_type, &num_attrs) == FAIL) {
        fprintf(stderr, "ERROR: SDdiminfo()\n");
        return NULL;
    }

    item = new HDFTreeViewItem((HDFTreeViewItem *) parent, (HDFTreeViewItem *) after,
                               HDFTreeViewItem::Dimension, dim_name);

    item->setText(FIELD_Name, dim_name);
    item->setText(FIELD_Description, "SD Dimension");

    snprintf(temp, LN, "%ld", (long) dim_index);
    item->setText(FIELD_Index, temp);

    snprintf(temp, LN, "%ld", (long) dim_id);
    item->setText(FIELD_ID, temp);

    snprintf(temp, LN, "%ld", (long) length);
    item->setText(FIELD_Value, temp);

    free(temp);

    return (void *) item;
}



void *HDFTreeView::functionSDAttrs(const void *parent, const void *after,
                                   int32 id, int32 attr_index, const int32 *flags)
{
    char *temp;

    char attr_name[MAX_NC_NAME];

    int32 i;
    int32 n;

    int32 data_type;
    int32 count;

    void *data;

    HDFTreeViewItem *item;

    temp = (char *) malloc(LN * sizeof(char));

    if (SDattrinfo(id, attr_index, attr_name, &data_type, &count) == FAIL) {
        fprintf(stderr, "ERROR: SDattrinfo()\n");
        return NULL;
    }

    data = (void *) malloc(DFKNTsize(data_type) * count);
    if (data == NULL) {
        fprintf(stderr, "ERROR: Memory allocation failed, attr_name = %s\n", attr_name);
        return NULL;
    }

    if (SDreadattr(id, attr_index, data) == FAIL) {
        fprintf(stderr, "ERROR: SDreadattr(), attr_name = %s\n", attr_name);
        return NULL;
    }

    item = new HDFTreeViewItem((HDFTreeViewItem *) parent, (HDFTreeViewItem *) after,
                               HDFTreeViewItem::Attribute, attr_name);

    item->setText(FIELD_Name, attr_name);
    item->setText(FIELD_Description, "SD Attribute");

    snprintf(temp, LN, "%ld", (long) attr_index);
    item->setText(FIELD_Index, temp);

    item->setText(FIELD_Data_Type, hdf_data_type_name(data_type));

    snprintf(temp, LN, "%ld", (long) count);
    item->setText(FIELD_Dimensions, temp);

    n = hdf_array_to_string(data_type, data, count, temp, LN);
    if (n < 0) {
        fprintf(stderr, "ERROR: hdf_array_to_string(), attr_name = %s\n", attr_name);
        return NULL;
    }
    if (data_type == DFNT_CHAR8 || data_type == DFNT_UCHAR8) {
        for (i = 0; i < n; ++i) {
            if (temp[i] == '\n')
                temp[i] = '\\';
        }
    }
    item->setText(FIELD_Value, temp);

    free(data);
    free(temp);

    return (void *) item;
}



void *HDFTreeView::functionSDIndex(const void *parent, const void *after,
                                   int32 sds_index, int32 sds_id, const int32 *flags)
{
    char *temp;

    char sds_name[MAX_NC_NAME];

    char *label;
    char *unit;
    char *format;
    char *coordsys;

    int data_size;

    int32 i;
    int32 n;

    int32 flag;

    int32 ref;

    int32 rank;
    int32 dim_sizes[MAX_VAR_DIMS];

    int32 data_type;
    int32 num_attrs;

    int32 start[MAX_VAR_DIMS];
    int32 edge [MAX_VAR_DIMS];

    float64 factor;
    float64 error;
    float64 offset;
    float64 offset_error;
    int32 data_type_raw;

    void *data;

    size_t length;

    HDF_CHUNK_DEF cdef;

    HDFTreeViewItem *item;

    temp = (char *) malloc(LN * sizeof(char));

    if (SDgetinfo(sds_id, sds_name, &rank, dim_sizes, &data_type, &num_attrs) == FAIL) {
        fprintf(stderr, "ERROR: SDgetinfo()\n");
        return NULL;
    }

    item = new HDFTreeViewItem((HDFTreeViewItem *) parent, (HDFTreeViewItem *) after,
                               HDFTreeViewItem::Dataset, sds_name);
    item->setHasDataTable(true);

    item->setText(FIELD_Name, sds_name);
    item->setText(FIELD_Description, "SD");

    snprintf(temp, LN, "%ld", (long) sds_index);
    item->setText(FIELD_Index, temp);

    snprintf(temp, LN, "%ld", (long) sds_id);
    item->setText(FIELD_ID, temp);

    ref = SDidtoref(sds_id);
    if (ref == FAIL) {
        fprintf(stderr, "ERROR: SDidtoref()\n");
        return NULL;
    }
    snprintf(temp, LN, "%ld", (long) ref);
    item->setText(FIELD_Reference_Number, temp);

    item->setText(FIELD_Data_Type, hdf_data_type_name(data_type));

    n = 0;
    for (i = 0; i < rank-1; ++i)
        n += snprintf(temp+n, LN-(n+1), "%ld, ", (long) dim_sizes[i]);
    n += snprintf(temp+n, LN-(n+1), "%ld", (long) dim_sizes[i]);
    item->setText(FIELD_Dimensions, temp);

    if (SDgetchunkinfo(sds_id, &cdef, &flag) == FAIL) {
        fprintf(stderr, "ERROR: SDgetchunkinfo()\n");
        return NULL;
    }

    n = 0;
    if (flag == HDF_NONE)
        n += snprintf(temp+n, LN - n, "None");
    else if (flag ==  HDF_CHUNK ||
            flag == (HDF_CHUNK | HDF_COMP) ||
            flag == (HDF_CHUNK | HDF_NBIT))
        n += snprintf(temp+n, LN - n, "Chunked");
    else {
        fprintf(stderr, "ERROR: SDgetchunkinfo(), invalid flag: %d\n", flag);
        return NULL;
    }
    item->setText(FIELD_Chunking, temp);

    n = 0;
    if (flag == HDF_NONE ||
        flag == HDF_CHUNK)
        n += snprintf(temp+n, LN - n, "None");
    else if (flag == (HDF_CHUNK | HDF_COMP))
        n += snprintf(temp+n, LN - n, "RLE/GZIP");
    else if (flag == (HDF_CHUNK | HDF_NBIT))
        n += snprintf(temp+n, LN - n, "NBIT");
    else {
        fprintf(stderr, "ERROR: SDgetchunkinfo(), invalid flag: %d\n", flag);
        return NULL;
    }
    item->setText(FIELD_Compression, temp);


    if (SDgetcal(sds_id, &factor, &error, &offset, &offset_error, &data_type_raw) != FAIL) {
        snprintf(temp, LN, "%e", factor);
        item->setText(FIELD_Factor, temp);

        snprintf(temp, LN, "%e", error);
        item->setText(FIELD_Error, temp);

        snprintf(temp, LN, "%e", offset);
        item->setText(FIELD_Offset, temp);

        snprintf(temp, LN, "%e", offset_error);
        item->setText(FIELD_Offset_Error, temp);

        item->setText(FIELD_Raw_Data_Type, hdf_data_type_name(data_type_raw));
    }
    else {
        item->setText(FIELD_Factor, "n/a");
        item->setText(FIELD_Error, "n/a");
        item->setText(FIELD_Offset, "n/a");
        item->setText(FIELD_Offset_Error, "n/a");
        item->setText(FIELD_Raw_Data_Type, "n/a");
    }

    label    = (char *) malloc(LN * sizeof(char));
    unit     = (char *) malloc(LN * sizeof(char));
    format   = (char *) malloc(LN * sizeof(char));
    coordsys = (char *) malloc(LN * sizeof(char));

    if (SDgetdatastrs(sds_id, label, unit, format, coordsys, LN) == FAIL) {
        fprintf(stderr, "ERROR: SDgetdatastrs()\n");
        return NULL;
    }

    item->setText(FIELD_Label,    label[0]    != '\0' ? label    : "n/a");
    item->setText(FIELD_Unit,     unit[0]     != '\0' ? unit     : "n/a");
    item->setText(FIELD_Format,   format[0]   != '\0' ? format   : "n/a");
    item->setText(FIELD_Coordsys, coordsys[0] != '\0' ? coordsys : "n/a");

    free(label);
    free(unit);
    free(format);
    free(coordsys);

    length = dim_sizes[rank - 1];
/*
    length = MIN(64, dim_sizes[rank - 1]);
*/
    data_size = hdf_data_type_size(data_type);
    if (data_size == 0) {
        fprintf(stderr, "ERROR: hdf_data_type_size(), sds_name = %s\n", sds_name);
        return NULL;
    }

    data = malloc(length * data_size);
    if (data == NULL) {
        fprintf(stderr, "ERROR: Memory allocation failed, sds_name = %s\n", sds_name);
        return NULL;
    }

    for (i = 0; i < rank; ++i) {
        start[i] = 0;
        edge[i] = 1;
    }

    edge[i - 1] = length;

    if (SDreaddata(sds_id, start, NULL, edge, data) == FAIL) {
        fprintf(stderr, "ERROR: SDreaddata(), sds_name = %s\n", sds_name);
        return NULL;
    }

    n = hdf_array_to_string(data_type, data, length, temp, LN);
    if (n < 0) {
        fprintf(stderr, "ERROR: hdf_array_to_string(), sds_name = %s\n", sds_name);
        return NULL;
    }
    if (data_type == DFNT_CHAR8 || data_type == DFNT_UCHAR8) {
        for (i = 0; i < n; ++i) {
            if (temp[i] == '\n')
                temp[i] = '\\';
        }
    }

    item->setText(FIELD_Value, temp);

    free(data);
    free(temp);

    return (void *) item;
}



void *HDFTreeView::functionVRef(const void *parent, const void *after, void **after2,
                                int32 vgroup_id, const int32 *flags)
{
    char vgroup_name[VGNAMELENMAX];

    char attr_name[MAX_NC_NAME];

    int32 i;

    int32 num_attrs;

    int32 data_type;
    int32 count;
    int32 length;

    HDFTreeViewItem *item;
    HDFTreeViewItem *item2;

    if (Vgetname(vgroup_id, vgroup_name) == FAIL) {
        fprintf(stderr, "ERROR: Vgetname()\n");
        return NULL;
    }

    item = new HDFTreeViewItem((HDFTreeViewItem *) parent, (HDFTreeViewItem *) after,
                               HDFTreeViewItem::VGroup, vgroup_name);

    item->setText(FIELD_Name, vgroup_name);
    item->setText(FIELD_Description, "V");

    num_attrs = Vnattrs(vgroup_id);
    if (num_attrs == FAIL) {
        fprintf(stderr, "ERROR: Vnattrs(), vgroup_name = %s\n", vgroup_name);
        return NULL;
    }

    item2 = NULL;
    for (i = 0; i < num_attrs; ++i) {
        if (Vattrinfo(vgroup_id, i, attr_name, &data_type, &count, &length) == FAIL) {
            fprintf(stderr, "ERROR: Vattrinfo(), vgroup_name = %s\n", vgroup_name);
            return NULL;
        }

        item2 = new HDFTreeViewItem(item, item2, HDFTreeViewItem::Attribute, attr_name);

        item2->setText(FIELD_Name, attr_name);
        item2->setText(FIELD_Description, "V Attribute");
        item2->setText(FIELD_Data_Type, hdf_data_type_name(data_type));
    }

    *after2 = NULL;

    return (void *) item;
}



void *HDFTreeView::functionVSRef(const void *parent, const void *after,
                                 int32 vdata_id, const int32 *flags)
{
    char *temp;

    char vdata_name[VSNAMELENMAX];
    char attr_name[MAX_NC_NAME];

    char field_name_list[VSFIELDMAX * (FIELDNAMELENMAX + 1)];

    int32 i;
    int32 j;
    int32 n;

    int32 n_fields;
    int32 n_records;
    int32 interlace_mode;
    int32 vdata_size;

    int32 num_attrs;

    int32 data_type;
    int32 count;
    int32 length;

    void *data;

    HDFTreeViewItem *item;
    HDFTreeViewItem *item2;

    temp = (char *) malloc(LN * sizeof(char));

    n_fields = VSinquire(vdata_id, &n_records, &interlace_mode,
                     field_name_list, &vdata_size, vdata_name);
    if (n_fields == FAIL) {
        fprintf(stderr, "ERROR: VSinquire()\n");
        return NULL;
    }

    item = new HDFTreeViewItem((HDFTreeViewItem *) parent, (HDFTreeViewItem *) after,
                               HDFTreeViewItem::VData, vdata_name);

    item->setText(FIELD_Name, vdata_name);
    item->setText(FIELD_Description, "VS");

    data_type = VFfieldtype(vdata_id, 0);
    if (data_type == FAIL) {
        fprintf(stderr, "ERROR: VFfieldtype(), vdata_name = %s\n", vdata_name);
        return NULL;
    }
    item->setText(FIELD_Data_Type, hdf_data_type_name(data_type));

    snprintf(temp, LN, "N: %ld, Size: %ld", (long) n_records, (long) vdata_size);
    item->setText(FIELD_Dimensions, temp);


    num_attrs = VSnattrs(vdata_id);
    if (num_attrs == FAIL) {
        fprintf(stderr, "ERROR: VSnattrs(), vdata_name = %s\n", vdata_name);
        return NULL;
    }

    item2 = NULL;
    for (i = 0; i < num_attrs; ++i) {
        if (VSattrinfo(vdata_id, -1, i, attr_name, &data_type, &count, &length) == FAIL) {
            fprintf(stderr, "ERROR: VSattrinfo(), vdata_name = %s\n", vdata_name);
            return NULL;
        }

        data = (void *) malloc(DFKNTsize(data_type) * count);
        if (data == NULL) {
            fprintf(stderr, "ERROR: Memory allocation failed, vdata_name = %s, attr_name = %s\n",
                    vdata_name, attr_name);
            return NULL;
        }

        if (VSgetattr(vdata_id, -1, i, data) == FAIL) {
            fprintf(stderr, "ERROR: VSgetattr(), vdata_name = %s, attr_name = %s\n",
                    vdata_name, attr_name);
            return NULL;
        }

        item2 = new HDFTreeViewItem(item, item2, HDFTreeViewItem::Attribute, attr_name);

        item2->setText(FIELD_Name, attr_name);
        item2->setText(FIELD_Description, "VS Attribute");
        item2->setText(FIELD_Data_Type, hdf_data_type_name(data_type));

        snprintf(temp, LN, "%ld", (long) count);
        item2->setText(FIELD_Dimensions, temp);

        n = hdf_array_to_string(data_type, data, length, temp, LN);
        if (n < 0) {
            fprintf(stderr, "ERROR: hdf_array_to_string(), vdata_name = %s, attr_name = %s\n",
                    vdata_name, attr_name);
            return NULL;
        }
        if (data_type == DFNT_CHAR8 || data_type == DFNT_UCHAR8) {
            for (j = 0; j < n; ++j) {
                if (temp[j] == '\n') {
                    temp[j] = '\\';
                }
            }
        }

        item2->setText(FIELD_Value, temp);

        free(data);
    }

    free(temp);

    return (void *) item;
}



void HDFTreeView::colorize(QTreeWidgetItem *item_, bool color)
{
    HDFTreeViewItem *item = (HDFTreeViewItem *) item_;

    if (! color)
        item->setTextColor(FIELD_Name, QColor(0, 0, 0));
    else {
        switch(item->type()) {
            case HDFTreeViewItem::File:
                item->setTextColor(FIELD_Name, file_color);
                break;
            case HDFTreeViewItem::VGroup:
                item->setTextColor(FIELD_Name, v_color);
                break;
            case HDFTreeViewItem::Dataset:
                item->setTextColor(FIELD_Name, sd_color);
                break;
            case HDFTreeViewItem::VData:
                item->setTextColor(FIELD_Name, vs_color);
                break;
            case HDFTreeViewItem::Dimension:
                item->setTextColor(FIELD_Name, dim_color);
                break;
            case HDFTreeViewItem::Attribute:
                item->setTextColor(FIELD_Name, attr_color);
                break;
            default:
                break;
        }
    }
}



void HDFTreeView::showDataTable()
{
    showDataTable((HDFTreeViewItem *) currentItem(), 0);
}



void HDFTreeView::showDataTable(HDFTreeViewItem *item, int column)
{
    if (item->type() != HDFTreeViewItem::Dataset)
        return;

    HDFTableView *t = new HDFTableView(sd_id, item->name, 0);

    t->setAttribute(Qt::WA_QuitOnClose, false);
    t->setAttribute(Qt::WA_DeleteOnClose, true);
    t->show();
}
