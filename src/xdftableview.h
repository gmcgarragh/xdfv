/*******************************************************************************
 *
 *    Copyright (C) 2015-2018 Greg McGarragh <greg.mcgarragh@colostate.edu>
 *
 *    This source code is licensed under the GNU General Public License (GPL),
 *    Version 3.  See the file COPYING for more details.
 *
 ******************************************************************************/

#ifndef XDFTABLEVIEW_H
#define XDFTABLEVIEW_H

#include <qlineedit.h>
#include <qtablewidget.h>
#include <qwidget.h>

class XDFTableView : public QWidget
{
    Q_OBJECT

private:
    int column_width;

    QLineEdit *lineEdit[8];
    QTableWidget *table_widget;

    int indexStringToSize_t(QString s, int i_dimen, size_t n, size_t *i);
    int parseRange(int i, size_t dim, size_t *offset, size_t *count);

protected:
    QTableWidget *tableWidget();
    void buildWidget(const char *, int n);
    int parseSlice(int n_dims, const size_t *dims, int *i_row, int *n_rows,
                   int *i_col, int *n_cols, size_t *offset, size_t *count,
                   size_t *length);
    void configureTable(int i_row, int n_rows, int i_col, int n_cols);

public:
    XDFTableView(QWidget *parent = 0);
    ~XDFTableView();

    int columnWidth();

public slots:
    void refreshTable();
};

#endif /* XDFTABLEVIEW_H */
