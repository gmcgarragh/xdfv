/*******************************************************************************
**
**    Copyright (C) 1998-2018 Greg McGarragh <greg.mcgarragh@colostate.edu>
**
**    This source code is licensed under the GNU General Public License (GPL),
**    Version 3.  See the file COPYING for more details.
**
*******************************************************************************/

#include "gutil.h"
#include "gnetcdf.h"

#include <netcdf.h>


/*******************************************************************************
 *
 ******************************************************************************/
const char *netcdf_data_type_name(nc_type xtype) {

     switch(xtype) {
          case NC_BYTE:
               return "NC_BYTE";
          case NC_UBYTE:
               return "NC_UBYTE";
          case NC_CHAR:
               return "NC_CHAR";
          case NC_SHORT:
               return "NC_SHORT";
          case NC_USHORT:
               return "NC_USHORT";
          case NC_INT:
               return "NC_INT";
          case NC_UINT:
               return "NC_UINT";
          case NC_INT64:
               return "NC_INT64";
          case NC_UINT64:
               return "NC_UINT64";
          case NC_FLOAT:
               return "NC_FLOAT";
          case NC_DOUBLE:
               return "NC_DOUBLE";
          case NC_STRING:
               return "NC_STRING";
          default:
               fprintf(stderr, "ERROR: Unknown NetCDF data type: %ld\n", (long) xtype);
               return NULL;
     }
}



/*******************************************************************************
 *
 ******************************************************************************/
size_t netcdf_data_type_size(nc_type xtype) {

     switch(xtype) {
          case NC_BYTE:
          case NC_UBYTE:
               return 1;
          case NC_CHAR:
               return 1;
          case NC_SHORT:
          case NC_USHORT:
               return 2;
          case NC_INT:
          case NC_UINT:
               return 4;
          case NC_INT64:
          case NC_UINT64:
               return 8;
          case NC_FLOAT:
               return 4;
          case NC_DOUBLE:
               return 8;
          case NC_STRING:
               return sizeof(char *);
          default:
               fprintf(stderr, "ERROR: Unknown NetCDF data type: %ld\n", (long) xtype);
               return -1;
     }
}



/*******************************************************************************
 *
 ******************************************************************************/
int netcdf_scaler_to_string(nc_type data_type, void *ptr, int i, char *temp, int length) {

     int n;

     switch(data_type) {
          case NC_BYTE:
               n = snprintf(temp, length, "%d",  ((int8_t *) ptr)[i]);
               break;
          case NC_UBYTE:
               n = snprintf(temp, length, "%d",  ((uint8_t *) ptr)[i]);
               break;
          case NC_SHORT:
               n = snprintf(temp, length, "%d",  ((int16_t *) ptr)[i]);
               break;
          case NC_USHORT:
               n = snprintf(temp, length, "%d",  ((uint16_t *) ptr)[i]);
               break;
          case NC_INT:
               n = snprintf(temp, length, "%ld", (long) ((int32_t *) ptr)[i]);
               break;
          case NC_UINT:
               n = snprintf(temp, length, "%ld", (long) ((uint32_t *) ptr)[i]);
               break;
          case NC_INT64:
               n = snprintf(temp, length, "%ld", (long) ((int64_t *) ptr)[i]);
               break;
          case NC_UINT64:
               n = snprintf(temp, length, "%ld", (long) ((uint64_t *) ptr)[i]);
               break;
          case NC_FLOAT:
               n = snprintf(temp, length, "%e",  ((float32_t *) ptr)[i]);
               break;
          case NC_DOUBLE:
               n = snprintf(temp, length, "%e",  ((float64_t *) ptr)[i]);
               break;
          case NC_STRING:
               n = snprintf(temp, length, "%s",  ((char **) ptr)[i]);
               break;
          default:
               fprintf(stderr, "ERROR: Unknown NetCDF data type: %ld\n", (long) data_type);
               return -1;
     }

     return MIN(n, length - 1);
}



/*******************************************************************************
 *
 ******************************************************************************/
int netcdf_array_to_string(nc_type data_type, void *ptr, int count, char *string, int length) {

     char temp[LN];

     int i;
     int n;

     int n_save = 0;

     if (data_type == NC_CHAR) {
          n = length <= count ? length - 1 : count;

          strncpy(string, (char *) ptr, n);

          string[n] = '\0';

          n_save = n;
     }
     else {
          n = 0;

          for (i = 0; i < count; ++i) {
               if (i > 0) {
                    if (n + 2 < length - 1)
                         n += snprintf(string+n, length - 1 - n, ", ");
                    else
                         break;
               }

               n_save = n;

               if ((n += netcdf_scaler_to_string(data_type, ptr, i, temp, length)) < 0) {
                    fprintf(stderr, "ERROR: netcdf_scaler_to_string()\n");
                    return -1;
               }

               if (n < length - 1)
                    strncpy(string+n_save, temp, length - 1 - n_save);
               else
                    break;

               n_save = n;
          }

          string[n_save] = '\0';
     }

     return n_save;
}
