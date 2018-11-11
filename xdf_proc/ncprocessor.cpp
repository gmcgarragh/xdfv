/*******************************************************************************
 *
 *    Copyright (C) 2015-2018 Greg McGarragh <greg.mcgarragh@colostate.edu>
 *
 *    This source code is licensed under the GNU General Public License (GPL),
 *    Version 3.  See the file COPYING for more details.
 *
 ******************************************************************************/

#include <netcdf.h>

#include <qregexp.h>

#include "ncprocessor.h"


int NCProcessor::procNCFile(const char *file_name, const char *path, const void *parent)
{
    char *token;
    char *lasts;

    char *path2 = NULL;
    char *path_nodes[MAX_DEPTH];

    int i;

    int status;

    int n_dims;
    int n_vars;
    int n_gatts;
    int unlimdim_id;

    void *item;

    FILE *fp;


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

    status = nc_open(file_name, NC_NOWRITE, &nc_id);
    if (status != NC_NOERR) {
/*
        fprintf(stderr, "ERROR: nc_open(), file_name = %s, %s\n", file_name,
               nc_strerror(status));
*/
        return UnableToOpenFile;
    }


    /*--------------------------------------------------------------------------
     *
     *------------------------------------------------------------------------*/
    if (functionInit(nc_id)) {
        fprintf(stderr, "ERROR: functionInit(), file_name = %s\n", file_name);
        return -1;
    }


    /*--------------------------------------------------------------------------
     *
     *------------------------------------------------------------------------*/
    status = nc_inq(nc_id, &n_dims, &n_vars, &n_gatts, &unlimdim_id);
    if (status != NC_NOERR) {
        fprintf(stderr, "ERROR: nc_inq(), %s\n", nc_strerror(status));
        return -1;
    }

    if (processAttrs(parent, NC_GLOBAL, n_gatts, 0)) {
        fprintf(stderr, "ERROR: processAttrs(), file_name = %s\n", file_name);
        return -1;
    }


    /*--------------------------------------------------------------------------
     *
     *------------------------------------------------------------------------*/
    item = NULL;
    for (int i = 0; i < n_vars; ++i) {
        if (processVarID(parent, &item, i, path_nodes, 0) < 0) {
            fprintf(stderr, "ERROR: processVarID(), file_name = %s\n", file_name);
            return -1;
        }
    }


    /*--------------------------------------------------------------------------
     *
     *------------------------------------------------------------------------*/
    if (functionFree(nc_id)) {
        fprintf(stderr, "ERROR: functionFree(), file_name = %s\n", file_name);
        return -1;
    }


    /*--------------------------------------------------------------------------
     *
     *------------------------------------------------------------------------*/
    status = nc_close(nc_id);
    if (status != NC_NOERR) {
        fprintf(stderr, "ERROR: nc_close(), file_name = %s, %s\n", file_name,
               nc_strerror(status));
        return -1;
    }

    free(path2);

    return 0;
}



int NCProcessor::processDims(const void *parent, int var_id, int n_dims,
                             const int *dim_ids, int flags)
{
    void *item;

    item = NULL;
    for (int i = 0; i < n_dims; ++i) {
        item = functionDim(parent, item, dim_ids[i], &flags);
        if (item == NULL) {
            fprintf(stderr, "ERROR: functionDim()\n");
            return -1;
        }
    }

    return 0;
}



int NCProcessor::processAttrs(const void *parent, int id, int num_attrs, int flags)
{
     void *item;

     item = NULL;
     for (int i = 0; i < num_attrs; ++i) {
          item = functionAttrs(parent, item, id, i, &flags);
          if (item == NULL) {
               fprintf(stderr, "ERROR: functionAttrs()\n");
               return -1;
          }
     }

     return 0;
}



int NCProcessor::processVarID(const void *parent, void **item,
                              int var_id, char **path, int flags)
{
     char var_name[MAX_NC_NAME];

     int retval;

     int status;

     int n_dims;
     int dim_ids[NC_MAX_VAR_DIMS];

     int n_atts;

     nc_type xtype;

     QRegExp regexp;

     status = nc_inq_var(nc_id, var_id, var_name, &xtype, &n_dims, dim_ids, &n_atts);
     if (status != NC_NOERR) {
          fprintf(stderr, "ERROR: nc_inq_var(), %s\n", nc_strerror(status));
          return -1;
     }

     if (*path)
          regexp = QRegExp(*path, Qt::CaseSensitive, QRegExp::Wildcard);

     if (*path == NULL || regexp.exactMatch(var_name)) {
          *item = functionVarID(parent, *item, var_id, &flags);
          if (*item == NULL) {
               fprintf(stderr, "ERROR: functionVarID(), var_name = %s\n", var_name);
               return -1;
          }

          if (processDims(*item, var_id, n_dims, dim_ids, flags)) {
               fprintf(stderr, "ERROR: processDims(), var_name = %s\n", var_name);
               return -1;
          }

          if (processAttrs(*item, var_id, n_atts, flags)) {
               fprintf(stderr, "ERROR: processAttrs(), var_name = %s\n", var_name);
               return -1;
          }

          retval = 0;
     }
     else
          retval = 1;

     return retval;
}



int NCProcessor::functionInit(int var_id)
{
     return 0;
}



int NCProcessor::functionFree(int var_id)
{
     return 0;
}



void *NCProcessor::functionDim(const void *parent, const void *after,
                               int dim_id, const int *flags)
{
     return NULL;
}



void *NCProcessor::functionAttrs(const void *parent, const void *after,
                                 int id, int num_attrs, const int *flags)
{
     return NULL;
}



void *NCProcessor::functionVarID(const void *parent, const void *after,
                                 int var_id, const int *flags)
{
     return NULL;
}



int NCProcessor::functionSetAfter(const void *item, const void *after)
{
     return 0;
}
