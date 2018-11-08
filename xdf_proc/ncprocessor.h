/*******************************************************************************
 *
 *    Copyright (C) 2015-2018 Greg McGarragh <greg.mcgarragh@colostate.edu>
 *
 *    This source code is licensed under the GNU General Public License (GPL),
 *    Version 3.  See the file COPYING for more details.
 *
 ******************************************************************************/

#ifndef NCPROCESSOR_H
#define NCPROCESSOR_H

#include <netcdf.h>

#include "xdfprocessor.h"


class NCProcessor : public XDFProcessor
{
private:
    int processDims(const void *parent, int var_id, int n_dims,
                    const int *dim_ids, int flags);
    int processAttrs(const void *parent, int id, int num_attrs, int flags);
    int processVarID(const void *parent, void **item,
                     int var_id, char **path, int flags);

protected:
    int nc_id;

    virtual int functionInit(int var_id);
    virtual int functionFree(int var_id);
    virtual void *functionDim(const void *parent, const void *after,
                              int dim_id, const int *flags);
    virtual void *functionAttrs(const void *parent, const void *after,
                                int id, int num_attrs, const int *flags);
    virtual void *functionVarID(const void *parent, const void *after,
                                int var_id, const int *flags);
    virtual int functionSetAfter(const void *item, const void *after);

public:
    virtual ~NCProcessor() { }

    virtual int procNCFile(const char *file_name, const char *path, const void *parent);
};

#endif /* NCPROCESSOR_H */
