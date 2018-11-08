/*******************************************************************************
**
**    Copyright (C) 1998-2018 Greg McGarragh <greg.mcgarragh@colostate.edu>
**
**    This source code is licensed under the GNU General Public License (GPL),
**    Version 3.  See the file COPYING for more details.
**
*******************************************************************************/

#ifndef GNETCDF_H
#define GNETCDF_H

#include <netcdf.h>

#ifdef __cplusplus
extern "C" {
#endif


const char *netcdf_data_type_name(nc_type);
size_t netcdf_data_type_size(nc_type);
int netcdf_scaler_to_string(nc_type data_type, void *ptr, int i, char *temp, int length);
int netcdf_array_to_string(nc_type data_type, void *ptr, int count, char *string, int length);


#ifdef __cplusplus
}
#endif

#endif /* GNETCDF_H */
