/*******************************************************************************
**
**    Copyright (C) 1998-2018 Greg McGarragh <greg.mcgarragh@colostate.edu>
**
**    This source code is licensed under the GNU General Public License (GPL),
**    Version 3.  See the file COPYING for more details.
**
*******************************************************************************/

#ifndef GHDF5_H
#define GHDF5_H

#include <hdf5.h>

#ifdef __cplusplus
extern "C" {
#endif


/* **** ghdf5_util.c **** */

const char *hdf5_data_class_name(H5T_class_t);
int hdf5_scaler_to_string(hid_t datatype_id, H5T_class_t data_class,
                          size_t data_size, void *ptr, int i, char *temp,
                          int length);
int hdf5_array_to_string(hid_t datatype_id, void *ptr, hsize_t count,
                         char *string, int length);


#ifdef __cplusplus
}
#endif

#endif /* GHDF5_H */
