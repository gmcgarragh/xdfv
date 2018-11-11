/*******************************************************************************
 *
 *    Copyright (C) 2015-2018 Greg McGarragh <greg.mcgarragh@colostate.edu>
 *
 *    This source code is licensed under the GNU General Public License (GPL),
 *    Version 3.  See the file COPYING for more details.
 *
 ******************************************************************************/

#ifndef HDFTABLEVIEW_H
#define HDFTABLEVIEW_H

#include <netcdf.h>
#include <hdf.h>
#include <mfhdf.h>

#include "xdftableview.h"


class HDFTableView : public XDFTableView
{
    Q_OBJECT

private:
    const char *file_name;
    const char *sds_name;

    int parseSlice(int32 n_dims, const int32 *dims,
                   int *i_row, int *n_rows, int *i_col, int *n_cols,
                   int32 *offset, int32 *count, int32 *length);

public:
    HDFTableView(const char *file_name, const char *sds_name, QWidget *parent = 0);
    ~HDFTableView();

public slots:
    void refreshTable();
};

#endif /* HDFTABLEVIEW_H */
