/*******************************************************************************
 *
 *    Copyright (C) 2015-2018 Greg McGarragh <greg.mcgarragh@colostate.edu>
 *
 *    This source code is licensed under the GNU General Public License (GPL),
 *    Version 3.  See the file COPYING for more details.
 *
 ******************************************************************************/

#ifndef HDF5TABLEVIEW_H
#define HDF5TABLEVIEW_H

#include <hdf5.h>

#include "xdftableview.h"


class HDF5TableView : public XDFTableView
{
    Q_OBJECT

private:
    const char *file_name;
    const char *dataset_name;

    int parseSlice(int n_dims, const hsize_t *dims,
                   int *i_row, int *n_rows, int *i_col, int *n_cols,
                   hsize_t *offset, hsize_t *count, hsize_t *length);
public:
    HDF5TableView(const char *file_name, const char *dataset_name, QWidget *parent = 0);
    ~HDF5TableView();

public slots:
    void refreshTable();
};

#endif /* HDF5TABLEVIEW_H */
