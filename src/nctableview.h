/*******************************************************************************
 *
 *    Copyright (C) 2015-2018 Greg McGarragh <greg.mcgarragh@colostate.edu>
 *
 *    This source code is licensed under the GNU General Public License (GPL),
 *    Version 3.  See the file COPYING for more details.
 *
 ******************************************************************************/

#ifndef NCTABLEVIEW_H
#define NCTABLEVIEW_H

#include <netcdf.h>

#include "xdftableview.h"


class NCTableView : public XDFTableView
{
    Q_OBJECT

private:
    int nc_id;
    const char *var_name;

public:
    NCTableView(int nc_id_, const char *var_name_, QWidget *parent = 0);
    ~NCTableView();

public slots:
    void refreshTable();
};

#endif /* NCTABLEVIEW_H */
