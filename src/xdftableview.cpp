/*******************************************************************************
 *
 *    Copyright (C) 2015-2018 Greg McGarragh <greg.mcgarragh@colostate.edu>
 *
 *    This source code is licensed under the GNU General Public License (GPL),
 *    Version 3.  See the file COPYING for more details.
 *
 ******************************************************************************/

#include <qboxlayout.h>
#include <qframe.h>
#include <qgroupbox.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include <qpushbutton.h>

#include "xdfv.h"
#include "xdftableview.h"


XDFTableView::XDFTableView(QWidget *parent)
    : QWidget(parent), column_width(110)
{

}



XDFTableView::~XDFTableView()
{

}



int XDFTableView::columnWidth()
{
    return column_width;
}



QTableWidget *XDFTableView::tableWidget()
{
    return table_widget;
}



void XDFTableView::buildWidget(const char *name, int n)
{
    int i;

    const char *temp;

    QVBoxLayout *verticalLayout;
    QFrame *frame;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer1;
    QPushButton *pushButton;
    QSpacerItem *horizontalSpacer2;

    verticalLayout = new QVBoxLayout(this);
    verticalLayout->setSpacing(0);
    verticalLayout->setContentsMargins(0,0,0,0);

    frame = new QFrame(this);
    verticalLayout->addWidget(frame);

    horizontalLayout = new QHBoxLayout(frame);

    horizontalSpacer1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    horizontalLayout->addItem(horizontalSpacer1);

    for (i = 0; i < n; ++i) {
        if (i < n - 2)
            temp = "0";
        else
            temp = ":";
        lineEdit[i] = new QLineEdit(temp, frame);
        horizontalLayout->addWidget(lineEdit[i]);
        QObject::connect(lineEdit[i], SIGNAL(returnPressed()), this, SLOT(refreshTable()));
    }

    pushButton = new QPushButton("Refresh", frame);
    horizontalLayout->addWidget(pushButton);
    QObject::connect(pushButton, SIGNAL(clicked()), this, SLOT(refreshTable()));

    horizontalSpacer2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    horizontalLayout->addItem(horizontalSpacer2);

    table_widget = new QTableWidget(this);
    verticalLayout->addWidget(table_widget);

    this->setWindowTitle(name);
}



int XDFTableView::indexStringToSize_t(QString s, int i_dimen, size_t n, size_t *i)
{
    bool ok;

    *i = s.trimmed().toULong(&ok);
    if (! ok) {
        QMessageBox crap(QMessageBox::Critical, "",
            QString("Invalid range index for dimension %1: %2.").
            arg(i_dimen).arg(s.trimmed()), QMessageBox::Ok, this);
        crap.exec();
        return -1;
    }

    if (*i < 0 || *i >= n) {
        QMessageBox crap(QMessageBox::Critical, "",
            QString("Range index %1 for dimension %2 out of range (0 to %3).").
            arg(*i).arg(i_dimen).arg(n - 1), QMessageBox::Ok, this);
        crap.exec();
        return -1;
    }

    return 0;
}



int XDFTableView::parseRange(int i, size_t dim, size_t *offset, size_t *count)
{
    int r;

    QStringList list = lineEdit[i]->text().split(":");

    if (list.count() > 2) {
        QMessageBox crap(QMessageBox::Critical, "",
            QString("Invalid range: %1.").arg(lineEdit[i]->text().trimmed()),
            QMessageBox::Ok, this);
        crap.exec();
        return -1;
    }

    if (list.count() == 1) {
        r = indexStringToSize_t(list[0], i, dim, offset);
        if (r) return r;
        *count  = 1;
    }
    else {
        if (list[0].trimmed() == "")
            *offset = 0;
        else {
            r = indexStringToSize_t(list[0], i, dim, offset);
            if (r) return r;
        }

        if (list[1].trimmed() == "")
            *count = dim - *offset;
        else {
            r = indexStringToSize_t(list[1], i, dim, count);
            if (r) return r;
            *count = *count - *offset + 1;
        }
    }

    return 0;
}



int XDFTableView::parseSlice(int n_dims, const size_t *dims, int *i_row, int *n_rows,
                             int *i_col, int *n_cols, size_t *offset, size_t *count,
                             size_t *length)
{
    int i;

    int count2;

    if (n_dims == 0) {
        *i_row  = 0;
        *i_col  = 0;

        *n_rows = 1;
        *n_cols = 1;

        *length = 1;
    }
    else if (n_dims == 1) {
        if (parseRange(0, dims[0], &offset[0], &count[0]))
            return -1;

        *i_row  = 0;
        *i_col  = offset[0];

        *n_rows = 1;
        *n_cols = count[0];

        *length = count[0];
    }
    else {
        count2 = 0;
        for (i = 0; i < n_dims; ++i) {
            offset[i] = 0;
            count [i] = 1;
            if (parseRange(i, dims[i], &offset[i], &count[i]))
                return -1;
            if (count[i] > 1)
                count2++;
        }

        if (count2 > 2) {
            QMessageBox crap(QMessageBox::Critical, "",
                "Only two dimensions are allowed to have ranges of more than a single element.",
                QMessageBox::Ok, this);
            crap.exec();
            return -1;
        }

        *i_col  = 0;
        *n_cols = 1;

        for (i = n_dims - 1; i >= 0; --i) {
            *i_col  = offset[i];
            if (count[i] > 1) {
                *n_cols = count[i];
                --i;
                break;
            }
        }

        *i_row  = 0;
        *n_rows = 1;

        for (           ; i >= 0; --i) {
            *i_row  = offset[i];
            if (count[i] > 1)
                *n_rows = count [i];
        }

        *length = 1;
        for (i = 0; i < n_dims; ++i)
            if (count[i] > 1)
                *length *= dims[i];
    }

    return 0;
}



void XDFTableView::refreshTable()
{

}



void XDFTableView::configureTable(int i_row, int n_rows, int i_col, int n_cols)
{
    int i;

    tableWidget()->setRowCount(n_rows);
    tableWidget()->setColumnCount(n_cols);

    QStringList v_labels;
    for (i = 0; i < n_rows; ++i)
        v_labels << QString("%1").arg(i_row + i);
    tableWidget()->setVerticalHeaderLabels(v_labels);

    QStringList h_labels;
    for (i = 0; i < n_cols; ++i)
        h_labels << QString("%1").arg(i_col + i);
    tableWidget()->setHorizontalHeaderLabels(h_labels);

    for (i = 0; i < n_cols; ++i)
        tableWidget()->setColumnWidth(i, columnWidth());
}
