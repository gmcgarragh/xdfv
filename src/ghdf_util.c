/*******************************************************************************
**
**    Copyright (C) 1998-2018 Greg McGarragh <greg.mcgarragh@colostate.edu>
**
**    This source code is licensed under the GNU General Public License (GPL),
**    Version 3.  See the file COPYING for more details.
**
*******************************************************************************/

#include "gutil.h"
#include "ghdf.h"

#include <netcdf.h>
#include <hdf.h>
#include <mfhdf.h>


/*******************************************************************************
 *
 ******************************************************************************/
const char *hdf_data_type_name(int32 hdf_type) {

     switch(hdf_type) {
          case DFNT_CHAR8:
               return "DFNT_CHAR8";
          case DFNT_UCHAR8:
               return "DFNT_UCHAR8";
          case DFNT_INT8:
               return "DFNT_INT8";
          case DFNT_UINT8:
               return "DFNT_UINT8";
          case DFNT_INT16:
               return "DFNT_INT16";
          case DFNT_UINT16:
               return "DFNT_UINT16";
          case DFNT_INT32:
               return "DFNT_INT32";
          case DFNT_UINT32:
               return "DFNT_UINT32";
          case DFNT_INT64:
               return "DFNT_INT64";
          case DFNT_UINT64:
               return "DFNT_UINT64";
          case DFNT_FLOAT32:
               return "DFNT_FLOAT32";
          case DFNT_FLOAT64:
               return "DFNT_FLOAT64";
          default:
               fprintf(stderr, "ERROR: Unknown HDF data type: %ld\n", (long) hdf_type);
               return NULL;
     }
}



/*******************************************************************************
 *
 ******************************************************************************/
size_t hdf_data_type_size(int32 hdf_type) {

     switch(hdf_type) {
          case DFNT_CHAR8:
               return 1;
          case DFNT_UCHAR8:
               return 1;
          case DFNT_INT8:
               return 1;
          case DFNT_UINT8:
               return 1;
          case DFNT_INT16:
               return 2;
          case DFNT_UINT16:
               return 2;
          case DFNT_INT32:
               return 4;
          case DFNT_UINT32:
               return 4;
          case DFNT_INT64:
               return 8;
          case DFNT_UINT64:
               return 8;
          case DFNT_FLOAT32:
               return 4;
          case DFNT_FLOAT64:
               return 8;
          default:
               fprintf(stderr, "ERROR: Unknown HDF data type: %ld\n", (long) hdf_type);
               return 0;
     }
}



/*******************************************************************************
 *
 ******************************************************************************/
int hdf_scaler_to_string(int32 data_type, void *ptr, int i, char *temp, int length) {

     int n;

     switch(data_type) {
          case DFNT_INT8:
               n = snprintf(temp, length, "%d",   ((int8 *) ptr)[i]);
               break;
          case DFNT_UINT8:
               n = snprintf(temp, length, "%u",   ((uint8 *) ptr)[i]);
               break;
          case DFNT_INT16:
               n = snprintf(temp, length, "%d",   ((int16 *) ptr)[i]);
               break;
          case DFNT_UINT16:
               n = snprintf(temp, length, "%u",   ((uint16 *) ptr)[i]);
               break;
          case DFNT_INT32:
               n = snprintf(temp, length, "%ld",  (long) ((int32 *) ptr)[i]);
               break;
          case DFNT_UINT32:
               n = snprintf(temp, length, "%lu",  (ulong) ((uint32 *) ptr)[i]);
               break;
/*
          case DFNT_INT64:
               n = snprintf(temp, length, "%lld", (LONG_LONG) ((int64 *) ptr)[i]);
               break;
          case DFNT_UINT64:
               n = snprintf(temp, length, "%llu", (ULONG_LONG) ((uint64 *) ptr)[i]);
               break;
*/
          case DFNT_FLOAT32:
               n = snprintf(temp, length, "%e",   ((float32 *) ptr)[i]);
               break;
          case DFNT_FLOAT64:
               n = snprintf(temp, length, "%e",   ((float64 *) ptr)[i]);
               break;
          default:
               fprintf(stderr, "ERROR: Unknown HDF data type: %ld\n", (long) data_type);
               return -1;
     }

     return MIN(n, length - 1);
}



/*******************************************************************************
 *
 ******************************************************************************/
int hdf_array_to_string(int32 data_type, void *ptr, int32 count, char *string, int length) {

     char temp[LN];

     int32 i;
     int32 n;

     int32 n_save = 0;

     string[0] = '\0';

     if (data_type == DFNT_CHAR8 || data_type == DFNT_UCHAR8) {
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

               if ((n += hdf_scaler_to_string(data_type, ptr, i, temp, LN)) < 0) {
                    fprintf(stderr, "ERROR: hdf_scaler_to_string()\n");
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
