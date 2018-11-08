/*******************************************************************************
 *
 *    Copyright (C) 2015-2018 Greg McGarragh <greg.mcgarragh@colostate.edu>
 *
 *    This source code is licensed under the GNU General Public License (GPL),
 *    Version 3.  See the file COPYING for more details.
 *
 ******************************************************************************/

#ifndef XDFMAINWINDOW_H
#define XDFMAINWINDOW_H

#include <qlineedit.h>
#include <qmainwindow.h>
#include <qsplitter.h>
#include <qtabwidget.h>

#include "xdftabtreeview.h"


class XDFMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum FileType {
        HDF4,
        HDF5,
        NetCDF,
        Unknown
    };

    enum ErrorCode {
        FileNotFound = 1,
        UnknownFileExtension,
        UnableToOpenFile
    };

private:
    static const size_t max_tab_size = 32;

    QFrame *find_frame;
    QLineEdit *find_line_edit;

    XDFTabTreeView *tab_tree_view;

    char *cut_fn(const char *in, char *out);

    FileType file_type_from_extension(QString file_name);

public:
    XDFMainWindow(QWidget *parent = 0);
    ~XDFMainWindow();

    XDFTabTreeView *tabTreeView();

public slots:
    void openFile();
    void openFile(const char *file_name, int flag);
    void openFile(XDFMainWindow::FileType file_type, const char *file_name, int flag);

    void find();
    void findPrev();
    void findAll();

    void showAbout();
};

#endif /* XDFMAINWINDOW_H */
