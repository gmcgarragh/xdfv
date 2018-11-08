/*******************************************************************************
 *
 *    Copyright (C) 2015-2018 Greg McGarragh <greg.mcgarragh@colostate.edu>
 *
 *    This source code is licensed under the GNU General Public License (GPL),
 *    Version 3.  See the file COPYING for more details.
 *
 ******************************************************************************/

#ifndef HDF5PROCESSOR_H
#define HDF5PROCESSOR_H

#include <hdf5.h>

#include "xdfprocessor.h"


class HDF5Processor : public XDFProcessor
{
private:
    struct operator_data_type {
        int depth;

        void *parent;
        void *after;

        HDF5Processor *object;
    };

    static const char *indent_string(int depth);

    static herr_t H5Aiterate_operator(hid_t loc_id, const char *attr_name,
                                      void *operator_data);
    static herr_t H5Giterate_operator(hid_t group_id, const char *member_name,
                                      void *operator_data);

    int processH5A(const void *parent, void **after, hid_t loc_id,
                   const char *attr_name, int depth);
    int processH5D(const void *parent, void **after, hid_t loc_id,
                   const char *dataset_name, int depth);
    int processH5G(const void *parent, void **after, hid_t loc_id,
                   const char *group_name, int depth);

protected:
    virtual int functionInit(hid_t loc_id);
    virtual int functionFree(hid_t loc_id);
    virtual void *functionH5A(const void *parent, const void *after,
                              hid_t attr_id, const char *attr_name);
    virtual void *functionH5D(const void *parent, const void *after,
                              hid_t dataset_id, const char *dataset_name);
    virtual void *functionH5G(const void *parent, const void *after,
                              hid_t group_id, const char *group_name);

public:
    virtual ~HDF5Processor() { }

    int procHDF5File(const char *file_name, const void *parent);
};

#endif /* HDF5PROCESSOR_H */
