/*******************************************************************************
**
**    Copyright (C) 1998-2018 Greg McGarragh <greg.mcgarragh@colostate.edu>
**
**    This source code is licensed under the GNU General Public License (GPL),
**    Version 3.  See the file COPYING for more details.
**
*******************************************************************************/

#include "gutil.h"
#include "ghdf5.h"

#include <hdf5.h>


/*******************************************************************************
 *
 ******************************************************************************/
const char *hdf5_data_class_name(H5T_class_t data_class) {

     switch(data_class) {
          case H5T_INTEGER:
               return "H5T_INTEGER";
          case H5T_FLOAT:
               return "H5T_FLOAT";
          case H5T_STRING:
               return "H5T_STRING";
          case H5T_BITFIELD:
               return "H5T_BITFIELD";
          case H5T_OPAQUE:
               return "H5T_OPAQUE";
          case H5T_COMPOUND:
               return "H5T_COMPOUND";
          case H5T_REFERENCE:
               return "H5T_REFERENCE";
          case H5T_ENUM:
               return "H5T_ENUM";
          case H5T_VLEN:
               return "H5T_VLEN";
          case H5T_ARRAY:
               return "H5T_ARRAY";
          default:
               fprintf(stderr, "ERROR: Invalid HDF5 data class: %d\n", data_class);
               return NULL;
     }
}



/*******************************************************************************
 *
 ******************************************************************************/
int hdf5_scaler_to_string(hid_t datatype_id, H5T_class_t data_class,
                          size_t data_size, void *ptr, int i, char *temp,
                          int length) {

     int j;
     int n = 0;

     hid_t native_type;

     switch(data_class) {
          case H5T_INTEGER:
          case H5T_FLOAT:
               native_type = H5Tget_native_type(datatype_id, H5T_DIR_ASCEND);
               if (H5Tequal(native_type, H5T_NATIVE_CHAR))
                    n = snprintf(temp, length, "%d",   ((char *)       ptr)[i]);
               else if (H5Tequal(native_type, H5T_NATIVE_SHORT))
                    n = snprintf(temp, length, "%d",   ((short *)      ptr)[i]);
               else if (H5Tequal(native_type, H5T_NATIVE_INT))
                    n = snprintf(temp, length, "%d",   ((int *)        ptr)[i]);
               else if (H5Tequal(native_type, H5T_NATIVE_LONG))
                    n = snprintf(temp, length, "%ld",  ((long *)       ptr)[i]);
               else if (H5Tequal(native_type, H5T_NATIVE_LLONG))
                    n = snprintf(temp, length, "%lld", ((LONG_LONG *)  ptr)[i]);
               else if (H5Tequal(native_type, H5T_NATIVE_UCHAR))
                    n = snprintf(temp, length, "%u",   ((uchar *)      ptr)[i]);
               else if (H5Tequal(native_type, H5T_NATIVE_USHORT))
                    n = snprintf(temp, length, "%u",   ((ushort *)     ptr)[i]);
               else if (H5Tequal(native_type, H5T_NATIVE_UINT))
                    n = snprintf(temp, length, "%u",   ((uint *)       ptr)[i]);
               else if (H5Tequal(native_type, H5T_NATIVE_ULONG))
                    n = snprintf(temp, length, "%lu",  ((ulong *)      ptr)[i]);
               else if (H5Tequal(native_type, H5T_NATIVE_ULLONG))
                    n = snprintf(temp, length, "%llu", ((ULONG_LONG *) ptr)[i]);
               else if (H5Tequal(native_type, H5T_NATIVE_FLOAT))
                    n = snprintf(temp, length, "%e",   ((float *)      ptr)[i]);
               else if (H5Tequal(native_type, H5T_NATIVE_DOUBLE))
                    n = snprintf(temp, length, "%e",   ((double *)     ptr)[i]);
/*
               else if (H5Tequal(native_type, H5T_NATIVE_LDOUBLE))
                    n = snprintf(temp, length, "%e",  ((long double *) ptr)[i]);
*/
               else {
                    fprintf(stderr, "ERROR: Invalid HDF5 native type.\n");
                    return -1;
               }
               break;
          case H5T_STRING:
               n = 0;
               if (n + 1 == length) {
                    temp[n] = '\0';
                    break;
               }
               temp[n++] = '\'';
               if (n + 1 == length) {
                    temp[n] = '\0';
                    break;
               }
/*
               while (n - 1 < data_size) {
                    if (H5Tis_variable_str(datatype_id))
                         temp[n] = ((char **) ptr)[i][n-1];
                    else
                         temp[n] = ((char *)  ptr)   [n-1];
                    n++;
                    if (n + 1 == length) {
                         temp[n] = '\0';
                         break;
                    }
               }
*/
               while (1) {
                    if (H5Tis_variable_str(datatype_id))
                         temp[n] = ((char **) ptr)[i][n-1];
                    else
                         temp[n] = ((char *)  ptr)   [n-1];
                    if (temp[n] == '\0')
                         break;
                    n++;
                    if (n + 1 == length) {
                         temp[n] = '\0';
                         break;
                    }
               }

               temp[n++] = '\'';
               if (n + 1 == length) {
                    temp[n] = '\0';
                    break;
               }
               temp[n++] = '\0';
               n--;
               for (j = 0; j < n; ++j) {
                    if (temp[j] == '\0') {
                         temp[j] = '\'';
                         j++;
                         temp[j] = '\0';
                         break;
                    }
               }
               n = j;
               break;
          case H5T_BITFIELD:
               break;
          case H5T_OPAQUE:
               break;
          case H5T_COMPOUND:
               break;
          case H5T_REFERENCE:
               break;
          case H5T_ENUM:
               break;
          case H5T_VLEN:
               break;
          case H5T_ARRAY:
               break;
          default:
               fprintf(stderr, "ERROR: Invalid HDF5 data class: %d\n", data_class);
               return -1;
     }

     return MIN(n, length - 1);
}



/*******************************************************************************
 *
 ******************************************************************************/
int hdf5_array_to_string(hid_t datatype_id, void *ptr, hsize_t count,
                         char *string, int length) {

     char temp[LN];

     hsize_t i;
     hsize_t n;

     hsize_t n_save = 0;

     size_t data_size;

     H5T_class_t data_class;

     data_class = H5Tget_class(datatype_id);
     data_size  = H5Tget_size(datatype_id);

     string[0] = '\0';

     n = 0;

     for (i = 0; i < count; ++i) {
          if (i > 0) {
               if (n + 2 < length - 1)
                    n += snprintf(string+n, length - 1 - n, ", ");
               else
                    break;
          }

          n_save = n;

          if ((n += hdf5_scaler_to_string(datatype_id, data_class,
                                          data_size, ptr, i, temp, LN)) < 0) {
               fprintf(stderr, "ERROR: hdf5_scaler_to_string()\n");
               return -1;
          }

          if (n < length - 1)
               strncpy(string+n_save, temp, length - 1 - n_save);
          else
               break;

          n_save = n;
     }

     string[n_save] = '\0';

     return n_save;
}
