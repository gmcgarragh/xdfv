/*******************************************************************************
 *
 *    Copyright (C) 2015-2018 Greg McGarragh <greg.mcgarragh@colostate.edu>
 *
 *    This source code is licensed under the GNU General Public License (GPL),
 *    Version 3.  See the file COPYING for more details.
 *
 ******************************************************************************/

#ifndef HDFPROCESSOR_H
#define HDFPROCESSOR_H

#include <netcdf.h>
#include <hdf.h>
#include <mfhdf.h>

#include "xdfprocessor.h"


class HDFProcessor : public XDFProcessor
{
private:
    int sds;

    int processV(const void *parent, void **after,
                 int32 vgroup_id, char **path, int32 flags);
    int processSDDims(const void *parent, int32 sds_id, int rank,
                      int32 flags);
    int processSDAttrs(const void *parent, int32 id, int num_attrs,
                       int32 flags);
    int processSDRef(const void *parent, void **item,
                     int32 ref, char **path, int32 flags);
    int processSDIndex(const void *parent, void **item,
                       int32 sds_index, char **path, int32 flags);
    int processVRef(const void *parent, void **item,
                    int32 ref, char **path, int32 flags);
    int processVSRef(const void *parent, void **item,
                    int32 ref, char **path, int32 flags);

protected:
    int32 file_id;
    int32 sd_id;

    virtual int functionInit(int32 file_id, int32 sd_id);
    virtual int functionFree(int32 file_id, int32 sd_id);
    virtual void *functionSDDim(const void *parent, const void *after,
                                int dim_index, int32 dim_id, const int32 *flags);
    virtual void *functionSDAttrs(const void *parent, const void *after,
                                  int32 id, int32 attr_index, const int32 *flags);
    virtual void *functionSDIndex(const void *parent, const void *after,
                                  int32 sds_index, int32 sds_id, const int32 *flags);
    virtual void *functionVRef(const void *parent, const void *after,
                               void **after2, int32 vgroup_id, const int32 *flags);
    virtual void *functionVSRef(const void *parent, const void *after,
                                int32 vdata_id, const int32 *flags);
    virtual int functionSetAfter(const void *item, const void *after);

public:
    virtual ~HDFProcessor() { }

    int procHDFFile(const char *file_name, const char *path,
                    const void *parent, int sds_);
};

#endif /* HDFPROCESSOR_H */
