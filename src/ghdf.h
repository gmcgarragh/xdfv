/*******************************************************************************
**
**    Copyright (C) 1998-2018 Greg McGarragh <greg.mcgarragh@colostate.edu>
**
**    This source code is licensed under the GNU General Public License (GPL),
**    Version 3.  See the file COPYING for more details.
**
*******************************************************************************/

#ifndef GHDF_H
#define GHDF_H

#include <netcdf.h>
#include <hdf.h>
#include <mfhdf.h>

#ifdef __cplusplus
extern "C" {
#endif


const char *hdf_data_type_name(int32);
size_t hdf_data_type_size(int32);
int hdf_scaler_to_string(int32, void *, int, char *, int);
int hdf_array_to_string(int32, void *, int32, char *, int);


#ifdef __cplusplus
}
#endif

#endif /* GHDF_H */
