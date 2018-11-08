/*******************************************************************************
 *
 *    Copyright (C) 2015-2018 Greg McGarragh <greg.mcgarragh@colostate.edu>
 *
 *    This source code is licensed under the GNU General Public License (GPL),
 *    Version 3.  See the file COPYING for more details.
 *
 ******************************************************************************/

#include <hdf5.h>

#include <qregexp.h>

#include "hdf5processor.h"


int HDF5Processor::procHDF5File(const char *file_name, const void *parent)
{
    void *item;

    FILE *fp;

    hid_t file_id;

    H5E_auto2_t error_func;
    void *error_client_data;


    /*--------------------------------------------------------------------------
     *
     *------------------------------------------------------------------------*/
    fp = fopen(file_name, "r");
    if (fp == NULL)
        return FileNotFound;
    fclose(fp);

    H5Eget_auto(H5E_DEFAULT, &error_func, &error_client_data);
    H5Eset_auto(H5E_DEFAULT, NULL, NULL);
    file_id = H5Fopen(file_name, H5F_ACC_RDONLY, H5P_DEFAULT);
    H5Eset_auto(H5E_DEFAULT, error_func, error_client_data);
    if (file_id < 0) {
/*
        fprintf(stderr, "ERROR: H5Fopen(), file_name = %s\n", file_name);
*/
        return UnableToOpenFile;
    }


    /*--------------------------------------------------------------------------
     *
     *------------------------------------------------------------------------*/
    if (functionInit(file_id)) {
        fprintf(stderr, "ERROR: functionInit(), file_name = %s\n", file_name);
        return -1;
    }


    /*--------------------------------------------------------------------------
     *
     *------------------------------------------------------------------------*/
    item = NULL;
    if (processH5G(parent, &item, file_id, "/", 0) < 0) {
        fprintf(stderr, "ERROR: processH5G(), file_name = %s\n", file_name);
        return -1;
    }


    /*--------------------------------------------------------------------------
     *
     *------------------------------------------------------------------------*/
    if (functionFree(file_id)) {
        fprintf(stderr, "ERROR: functionFree(), file_name = %s\n", file_name);
        return -1;
    }


    /*--------------------------------------------------------------------------
     *
     *------------------------------------------------------------------------*/
    if (H5Fclose(file_id) < 0) {
        fprintf(stderr, "ERROR: H5Fclose(), file_name = %s\n", file_name);
        return -1;
    }


    return 0;
}



const char *HDF5Processor::indent_string(int depth)
{
    static const char *strings[] = {"",
                                    "     ",
                                    "          ",
                                    "                    "};

    return strings[depth];
}



herr_t HDF5Processor::H5Aiterate_operator(hid_t loc_id, const char *attr_name,
                                          void *operator_data)
{
    struct operator_data_type *operator_data2;

    operator_data2 = (struct operator_data_type *) operator_data;

    if (operator_data2->object->processH5A(operator_data2->parent,
        &operator_data2->after, loc_id, attr_name, operator_data2->depth) < 0) {
        fprintf(stderr, "ERROR: processH5A(), attr_name = %s\n", attr_name);
        return -1;
    }

    return 0;
}



herr_t HDF5Processor::H5Giterate_operator(hid_t group_id, const char *member_name,
                                          void *operator_data)
{
    H5G_stat_t statbuf;

    struct operator_data_type *operator_data2;

    if (H5Gget_objinfo(group_id, member_name, 0, &statbuf) < 0) {
        fprintf(stderr, "ERROR: processH5D(), member_name = %s\n", member_name);
        return -1;
    }

    operator_data2 = (struct operator_data_type *) operator_data;

    switch (statbuf.type) {
        case H5G_GROUP:
            if (operator_data2->object->processH5G(operator_data2->parent,
                &operator_data2->after, group_id, member_name, operator_data2->depth) < 0) {
                fprintf(stderr, "ERROR: processH5G(), member_name = %s\n", member_name);
                return -1;
            }
            break;
        case H5G_DATASET:
            if (operator_data2->object->processH5D(operator_data2->parent,
                &operator_data2->after, group_id, member_name, operator_data2->depth) < 0) {
                fprintf(stderr, "ERROR: processH5D(), member_name = %s\n", member_name);
                return -1;
            }
            break;
        case H5G_TYPE:
            fprintf(stderr, "WARNING: Named datatype processing not supported, "
                            "member_name = %s\n", member_name);
            break;
        case H5G_LINK:
            fprintf(stderr, "WARNING: Symbolic link processing not supported, "
                            "member_name = %s\n", member_name);
            break;
        case H5G_UDLINK:
            fprintf(stderr, "WARNING: User-defined link processing not supported, "
                            "member_name = %s\n", member_name);
            break;
        default:
            fprintf(stderr, "WARNING: Unable to identify an object, "
                            "member_name = %s\n", member_name);
    }

    return 0;
}



int HDF5Processor::processH5A(const void *parent, void **after, hid_t loc_id,
                              const char *attr_name, int depth)
{
    void *item;

    hid_t attr_id;

    attr_id = H5Aopen(loc_id, attr_name, H5P_DEFAULT);
    if (attr_id < 0) {
        fprintf(stderr, "ERROR: H5Aopen(), attribute_name = %s\n", attr_name);
        return -1;
    }

    item = functionH5A(parent, *after, attr_id, attr_name);
    if (item == NULL) {
        fprintf(stderr, "ERROR: functionH5A(), attribute_name = %s\n", attr_name);
        return -1;
    }

    *after = item;

    if (H5Aclose(attr_id) < 0) {
        fprintf(stderr, "ERROR: H5Aclose(), attribute_name = %s\n", attr_name);
        return -1;
    }

    return 0;
}



int HDF5Processor::processH5D(const void *parent, void **after, hid_t loc_id,
                              const char *dataset_name, int depth)
{
    void *item;

    hid_t dataset_id;

    operator_data_type operator_data;

    dataset_id = H5Dopen(loc_id, dataset_name, H5P_DEFAULT);
    if (dataset_id < 0) {
        fprintf(stderr, "ERROR: H5Dopen(), dataset_name = %s\n", dataset_name);
        return -1;
    }

    item = functionH5D(parent, *after, dataset_id, dataset_name);
    if (item == NULL) {
        fprintf(stderr, "ERROR: functionH5D(), dataset_name = %s\n", dataset_name);
        return -1;
    }

    *after = item;

    operator_data.depth  = depth + 1;
    operator_data.parent = item;
    operator_data.after  = NULL;
    operator_data.object = this;

    if (H5Aiterate1(dataset_id, 0, HDF5Processor::H5Aiterate_operator,
        &operator_data) < 0) {
        fprintf(stderr, "ERROR: H5Aiterate1(), dataset_name = %s\n", dataset_name);
        return -1;
    }

    if (H5Dclose(dataset_id) < 0) {
        fprintf(stderr, "ERROR: H5Dclose(), dataset_name = %s\n", dataset_name);
        return -1;
    }

    return 0;
}



int HDF5Processor::processH5G(const void *parent, void **after, hid_t loc_id,
                              const char *group_name, int depth)
{
    void *item;

    hid_t group_id;

    operator_data_type operator_data;

    group_id = H5Gopen(loc_id, group_name, H5P_DEFAULT);
    if (group_id < 0) {
        fprintf(stderr, "ERROR: H5Gopen(), group_name = %s\n", group_name);
        return -1;
    }

    item = functionH5G(parent, *after, group_id, group_name);
    if (item == NULL) {
        fprintf(stderr, "ERROR: functionH5G(), group_name = %s\n", group_name);
        return -1;
    }

    *after = item;

    operator_data.depth  = depth + 1;
    operator_data.parent = item;
    operator_data.after  = NULL;
    operator_data.object = this;

    if (H5Giterate(loc_id, group_name, NULL, HDF5Processor::H5Giterate_operator,
        &operator_data) < 0) {
        fprintf(stderr, "ERROR: H5Giterate(), group_name = %s\n", group_name);
        return -1;
    }

    operator_data.depth  = depth + 1;
    operator_data.parent = item;
    operator_data.after  = NULL;
    operator_data.object = this;

    if (H5Aiterate1(group_id, 0, HDF5Processor::H5Aiterate_operator,
        &operator_data) < 0) {
        fprintf(stderr, "ERROR: H5Aiterate1(), group_name = %s\n", group_name);
        return -1;
    }

    if (H5Gclose(group_id) < 0) {
        fprintf(stderr, "ERROR: H5Gclose(), group_name = %s\n", group_name);
        return -1;
    }

    return 0;
}



int HDF5Processor::functionInit(hid_t loc_id)
{
    return 0;
}



int HDF5Processor::functionFree(hid_t loc_id)
{
    return 0;
}



void *HDF5Processor::functionH5A(const void *parent, const void *after,
                                 hid_t attr_id, const char *attr_name)
{
    return NULL;
}



void *HDF5Processor::functionH5D(const void *parent, const void *after,
                                 hid_t dataset_id, const char *dataset_name)
{
    return NULL;
}



void *HDF5Processor::functionH5G(const void *parent, const void *after,
                                 hid_t group_id, const char *group_name)
{
    return NULL;
}
