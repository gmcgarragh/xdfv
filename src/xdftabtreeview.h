/*******************************************************************************
 *
 *    Copyright (C) 2015-2018 Greg McGarragh <greg.mcgarragh@colostate.edu>
 *
 *    This source code is licensed under the GNU General Public License (GPL),
 *    Version 3.  See the file COPYING for more details.
 *
 ******************************************************************************/

#ifndef XDFTABTREEVIEW_H
#define XDFTABTREEVIEW_H

#include <qtabwidget.h>


class XDFTabTreeView : public QTabWidget
{
    Q_OBJECT

private:
    bool default_expanded;

    int font_size;
    int default_font_size;

    bool is_colorized;

public:
    XDFTabTreeView(QWidget *parent = 0);
    ~XDFTabTreeView();

    bool getDefaultExpanded();

    int getFontSize();
    int getDefaultFontSize();

    bool isColorized();

signals:
    void colorizedChanged(bool);

public slots:
    void copyItemName();

    void find(QString &name);
    void findPrev(QString &name);
    void selectAll(QString &name);

    void setDefaultExpanded(bool expanded);
    void expandAll();
    void expandAllTabs();
    void collapseAll();
    void collapseAllTabs();

    void showDataTable();

    void closeCurrentTab();
    void closeCurrentTab(int index);

    void setDefaultFontSize(int size);
    void useDefaultFontSize();
    void setFontSize(int size);
    void changeFontSize(int delta);
    void increaseFontSize();
    void decreaseFontSize();

    void setColorized(bool colorize);
};

#endif /* XDFTABTREEVIEW_H */
