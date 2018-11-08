/*******************************************************************************
 *
 *    Copyright (C) 2015-2018 Greg McGarragh <greg.mcgarragh@colostate.edu>
 *
 *    This source code is licensed under the GNU General Public License (GPL),
 *    Version 3.  See the file COPYING for more details.
 *
 ******************************************************************************/

#include <netcdf.h>
#include <hdf.h>
#include <mfhdf.h>

#include <qregexp.h>

#include "hdfprocessor.h"


int HDFProcessor::procHDFFile(const char *file_name, const char *path,
                              const void *parent, int sds_)
{
    char *token;
    char *lasts;

    char *path2 = NULL;
    char *path_nodes[MAX_DEPTH];

    int32 i;

    int32 num_datasets;
    int32 num_global_attrs;

    int32 n_refs;

    int32 *refs;

    void *item;

    FILE *fp;


    sds = sds_;


    /*--------------------------------------------------------------------------
     *
     *------------------------------------------------------------------------*/
    if (path == NULL)
        path_nodes[0] = NULL;
    else {
        path2 = strdup(path);

        i = 0;
        if ((token = strtok_r(path2, "/", &lasts))) {
            do {
                path_nodes[i++] = token;
            } while ((token = strtok_r(NULL, "/", &lasts)));
        }

        path_nodes[i++] = NULL;
    }


    /*--------------------------------------------------------------------------
     *
     *------------------------------------------------------------------------*/
    fp = fopen(file_name, "r");
    if (fp == NULL)
        return FileNotFound;

    fclose(fp);

    file_id = Hopen(file_name, DFACC_READ, DEF_NDDS);
    if (file_id == FAIL) {
/*
        fprintf(stderr, "ERROR: Hopen(), file_name = %s\n", file_name);
*/
        return UnableToOpenFile;
    }

    if (Vstart(file_id) == FAIL) {
        fprintf(stderr, "ERROR: Vstart(), file_name = %s\n", file_name);
        return -1;
    }

    sd_id = SDstart(file_name, DFACC_READ);
    if (sd_id == FAIL) {
        fprintf(stderr, "ERROR: SDstart(), file_name = %s\n", file_name);
        return -1;
    }


    /*--------------------------------------------------------------------------
     *
     *------------------------------------------------------------------------*/
    if (functionInit(file_id, sd_id)) {
        fprintf(stderr, "ERROR: functionInit(), file_name = %s\n", file_name);
        return -1;
    }


    /*--------------------------------------------------------------------------
     *
     *------------------------------------------------------------------------*/
    if (SDfileinfo(sd_id, &num_datasets, &num_global_attrs) == FAIL) {
        fprintf(stderr, "ERROR: SDfileinfo(), file_name = %s\n", file_name);
        return -1;
    }

    if (processSDAttrs(parent, sd_id, num_global_attrs, 0)) {
        fprintf(stderr, "ERROR: processSDAttrs(), file_name = %s\n", file_name);
        return -1;
    }


    /*--------------------------------------------------------------------------
     *
     *------------------------------------------------------------------------*/
    if (sds) {
        item = NULL;
        for (i = 0; i < num_datasets; ++i) {
            if (processSDIndex(parent, &item, i, path_nodes, 0) < 0) {
                fprintf(stderr, "ERROR: processSDIndex(), file_name = %s\n", file_name);
                return -1;
            }
        }
    }
    else {
        n_refs = Vlone(file_id, NULL, 0);
        if (n_refs == FAIL) {
            fprintf(stderr, "ERROR: Vlone(), file_name = %s\n", file_name);
            return -1;
        }

        refs = (int32 *) malloc(n_refs * sizeof(int32));
        if (refs == NULL) {
            fprintf(stderr, "ERROR: Memory allocation failed, file_name = %s\n", file_name);
            return -1;
        }

        n_refs = Vlone(file_id, refs, n_refs);
        if (n_refs == FAIL) {
            fprintf(stderr, "ERROR: Vlone(), file_name = %s\n", file_name);
            return -1;
        }

        item = NULL;
        for (i = 0; i < n_refs - 1; ++i) {
            if (processVRef(parent, &item, refs[i], path_nodes, 0) < 0) {
                fprintf(stderr, "ERROR: processVRef(), file_name = %s\n", file_name);
                return -1;
            }
        }

        free(refs);
    }


    /*--------------------------------------------------------------------------
     *
     *------------------------------------------------------------------------*/
    n_refs = VSlone(file_id, NULL, 0);
    if (n_refs == FAIL) {
        fprintf(stderr, "ERROR: VSlone(), file_name = %s\n", file_name);
        return -1;
    }

    refs = (int32 *) malloc(n_refs * sizeof(int32));
    if (refs == NULL) {
        fprintf(stderr, "ERROR: Memory allocation failed, file_name = %s\n", file_name);
        return -1;
    }


    n_refs = VSlone(file_id, refs, n_refs);
    if (n_refs == FAIL) {
        fprintf(stderr, "ERROR: VSlone(), file_name = %s\n", file_name);
        return -1;
    }

    item = NULL;
    for (i = 0; i < n_refs - 0; ++i) {
/*
        if (processVSRef(parent, &item, refs[i], path_nodes, 0)) {
            fprintf(stderr, "ERROR: processVSRef(), file_name = %s\n", file_name);
            return -1;
        }
*/
    }


    free(refs);


    /*--------------------------------------------------------------------------
     *
     *------------------------------------------------------------------------*/
    if (functionFree(file_id, sd_id)) {
        fprintf(stderr, "ERROR: functionFree(), file_name = %s\n", file_name);
        return -1;
    }


    /*--------------------------------------------------------------------------
     *
     *------------------------------------------------------------------------*/
    if (SDend(sd_id) == FAIL) {
        fprintf(stderr, "ERROR: SDend(), file_name = %s\n", file_name);
        return -1;
    }

    if (Vend(file_id) == FAIL) {
        fprintf(stderr, "ERROR: Vend(), file_name = %s\n", file_name);
        return -1;
    }

    if (Hclose(file_id) == FAIL) {
        fprintf(stderr, "ERROR: Hclose(), file_name = %s\n", file_name);
        return -1;
    }

    free(path2);

    return 0;
}



int HDFProcessor::processV(const void *parent, void **after,
                           int32 vgroup_id, char **path, int32 flags)
{
    int32 i;

    int32 n_pairs;

    int32 vgroup_tag;
    int32 vgroup_ref;

    n_pairs = Vntagrefs(vgroup_id);
    if (n_pairs == FAIL) {
        fprintf(stderr, "ERROR: Vntagrefs()\n");
        return -1;
    }

    for (i = 0; i < n_pairs; ++i) {
        if (Vgettagref(vgroup_id, i, &vgroup_tag, &vgroup_ref) == FAIL) {
            fprintf(stderr, "ERROR: Vgettagref()\n");
            return -1;
        }

        if (vgroup_tag == DFTAG_NDG || vgroup_tag == DFTAG_SD) {
            if (processSDRef(parent, after, vgroup_ref, path, flags) < 0) {
                fprintf(stderr, "ERROR: processSDRef()\n");
                return -1;
            }
        }
        else if (vgroup_tag == DFTAG_VG) {
            if (processVRef(parent, after, vgroup_ref, path, flags) < 0) {
                fprintf(stderr, "ERROR: processVRef()\n");
                return -1;
            }
        }
        else if (vgroup_tag == DFTAG_VH) {
            if (processVSRef(parent, after, vgroup_ref, path, flags) < 0) {
                fprintf(stderr, "ERROR: processVSRef()\n");
                return -1;
            }
        }
        else {
            fprintf(stderr, "ERROR: Unknown tag\n");
            return -1;
        }
    }

    return  0;
}



int HDFProcessor::processSDDims(const void *parent, int32 sds_id, int rank,
                                int32 flags)
{
    int i;

    int32 dim_id;

    void *item;

    item = NULL;
    for (i = 0; i < rank; ++i) {
        dim_id = SDgetdimid(sds_id, i);
        if (dim_id == FAIL) {
            fprintf(stderr, "ERROR: SDgetdimid()\n");
            return -1;
        }

        item = functionSDDim(parent, item, i, dim_id, &flags);
        if (item == NULL) {
/*
            fprintf(stderr, "ERROR: functionSDDim()\n");
            return -1;
*/
        }
    }

    return 0;
}



int HDFProcessor::processSDAttrs(const void *parent, int32 id, int num_attrs,
                                 int32 flags)
{
    int i;

    void *item;

    item = NULL;
    for (i = 0; i < num_attrs; ++i) {
        item = functionSDAttrs(parent, item, id, i, &flags);
        if (item == NULL) {
/*
            fprintf(stderr, "ERROR: functionSDAttrs()\n");
            return -1;
*/
        }
    }

    return 0;
}



int HDFProcessor::processSDRef(const void *parent, void **item,
                               int32 ref, char **path, int32 flags)
{
    int retval;

    int32 sds_index;

    sds_index = SDreftoindex(sd_id, ref);
    if (sds_index == FAIL) {
        fprintf(stderr, "ERROR: SDreftoindex()\n");
        return -1;
    }

    retval = processSDIndex(parent, item, sds_index, path, flags);
    if (retval < 0) {
        fprintf(stderr, "ERROR: processSDIndex()\n");
        return -1;
    }

    return retval;
}



int HDFProcessor::processSDIndex(const void *parent, void **item,
                                 int32 sds_index, char **path, int32 flags)
{
    char sds_name[MAX_NC_NAME];

    int retval;

    int32 sds_id;

    int32 rank;
    int32 dim_sizes[MAX_VAR_DIMS];

    int32 data_type;
    int32 num_attrs;

    QRegExp regexp;

    sds_id = SDselect(sd_id, sds_index);
    if (sds_id == FAIL) {
        fprintf(stderr, "ERROR: SDselect()\n");
        return -1;
    }

    if (SDgetinfo(sds_id, sds_name, &rank, dim_sizes, &data_type, &num_attrs) == FAIL) {
        fprintf(stderr, "ERROR: SDgetinfo()\n");
        return -1;
    }

    if (*path)
        regexp = QRegExp(*path, Qt::CaseSensitive, QRegExp::Wildcard);

    if (*path == NULL || regexp.exactMatch(sds_name)) {
        *item = functionSDIndex(parent, *item, sds_index, sds_id, &flags);
        if (*item == NULL) {
            fprintf(stderr, "ERROR: functionSDIndex(), sds_name = %s\n", sds_name);
            return -1;
        }

        if (processSDDims(*item, sds_id, rank, flags)) {
            fprintf(stderr, "ERROR: processSDDims(), sds_name = %s\n", sds_name);
            return -1;
        }

        if (processSDAttrs(*item, sds_id, num_attrs, flags)) {
            fprintf(stderr, "ERROR: processSDAttrs(), sds_name = %s\n", sds_name);
            return -1;
        }

        retval = 0;
    }
    else
        retval = 1;

    if (SDendaccess(sds_id) == FAIL) {
        fprintf(stderr, "ERROR: SDgetinfo(), sds_name = %s\n", sds_name);
        return -1;
    }

    return retval;
}



int HDFProcessor::processVRef(const void *parent, void **item,
                              int32 ref, char **path, int32 flags)
{
    char vgroup_name[VGNAMELENMAX];

    int retval;

    int32 vgroup_id;

    void *after;

    QRegExp regexp;

    vgroup_id = Vattach(file_id, ref, "r");
    if (vgroup_id == FAIL) {
        fprintf(stderr, "ERROR: Vattach()\n");
        return -1;
    }

    if (Vgetname(vgroup_id, vgroup_name) == FAIL) {
        fprintf(stderr, "ERROR: Vgetname()\n");
        return -1;
    }

    if (*path)
        regexp = QRegExp(*path, Qt::CaseSensitive, QRegExp::Wildcard);

    if (*path == NULL || regexp.exactMatch(vgroup_name)) {
        *item = functionVRef(parent, *item, &after, vgroup_id, &flags);
        if (*item == NULL) {
            fprintf(stderr, "ERROR: functionVRef(), vgroup_name = %s\n", vgroup_name);
            return -1;
        }

        if (processV(*item, &after, vgroup_id, (*path == NULL ? path : path + 1), flags)) {
            fprintf(stderr, "ERROR: processV(), vgroup_name = %s\n", vgroup_name);
            return -1;
        }

        if (functionSetAfter(*item, after)) {
            fprintf(stderr, "ERROR: functionSetAfter(), vgroup_name = %s\n", vgroup_name);
            return -1;
        }

        retval = 0;
    }
    else
        retval = 1;

    if (Vdetach(vgroup_id) == FAIL) {
        fprintf(stderr, "ERROR: Vdetach(), vgroup_name = %s\n", vgroup_name);
        return -1;
    }

    return retval;
}



int HDFProcessor::processVSRef(const void *parent, void **item,
                               int32 ref, char **path, int32 flags)
{
    char vdata_name[VSNAMELENMAX];

    int retval;

    int32 vdata_id;

    QRegExp regexp;

    vdata_id = VSattach(file_id, ref, "r");
    if (vdata_id == FAIL) {
        fprintf(stderr, "ERROR: VSattach()\n");
        return -1;
    }

    if (*path)
        regexp = QRegExp(*path, Qt::CaseSensitive, QRegExp::Wildcard);

    if (VSgetname(vdata_id, vdata_name) == FAIL) {
        fprintf(stderr, "ERROR: VSgetname()\n");
        return -1;
    }

    if (*path == NULL || regexp.exactMatch(vdata_name)) {
        *item = functionVSRef(parent, *item, vdata_id, &flags);
        if (*item == NULL) {
            fprintf(stderr, "ERROR: functionVSRef(), vdata_name = %s\n", vdata_name);
            return -1;
        }

        retval = 0;
    }
    else
        retval = 1;

    if (VSdetach(vdata_id) == FAIL) {
        fprintf(stderr, "ERROR: VSdetach(), vdata_name = %s\n", vdata_name);
        return -1;
    }

    return retval;
}



int HDFProcessor::functionInit(int32 file_id, int32 sd_id)
{
    return 0;
}



int HDFProcessor::functionFree(int32 file_id, int32 sd_id)
{
    return 0;
}



void *HDFProcessor::functionSDDim(const void *parent, const void *after,
                                  int dim_index, int32 dim_id, const int32 *flags)
{
    return NULL;
}



void *HDFProcessor::functionSDAttrs(const void *parent, const void *after,
                                    int32 id, int32 attr_index, const int32 *flags)
{
    return NULL;
}



void *HDFProcessor::functionSDIndex(const void *parent, const void *after,
                                    int32 sds_index, int32 sds_id, const int32 *flags)
{
    return NULL;
}



void *HDFProcessor::functionVRef(const void *parent, const void *after,
                                 void **after2, int32 vgroup_id, const int32 *flags)
{
    return NULL;
}



void *HDFProcessor::functionVSRef(const void *parent, const void *after,
                                  int32 vdata_id, const int32 *flags)
{
    return NULL;
}



int HDFProcessor::functionSetAfter(const void *item, const void *after)
{
    return 0;
}
