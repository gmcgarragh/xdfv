/*******************************************************************************
 *
 *    Copyright (C) 2015-2018 Greg McGarragh <greg.mcgarragh@colostate.edu>
 *
 *    This source code is licensed under the GNU General Public License (GPL),
 *    Version 3.  See the file COPYING for more details.
 *
 ******************************************************************************/

#include "xdfv.h"
#include "xdftabtreeview.h"
#include "xdftreeview.h"


XDFTabTreeView::XDFTabTreeView(QWidget *parent)
    : QTabWidget(parent), default_expanded(false), is_colorized(false)
{
    setTabsClosable(true);
/*
    setTabShape(QTabWidget::Triangular);
*/
    QFont font;
    default_font_size = font.pointSize();

    font_size = default_font_size;

    QObject::connect(this, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
}



XDFTabTreeView::~XDFTabTreeView()
{

}



void XDFTabTreeView::copyItemName()
{
    if (count() > 0)
        ((XDFTreeView *) currentWidget())->copyItemName();
}



void XDFTabTreeView::find(QString &name)
{
    if (count() > 0)
        ((XDFTreeView *) currentWidget())->find(name);
}



void XDFTabTreeView::findPrev(QString &name)
{
    if (count() > 0)
        ((XDFTreeView *) currentWidget())->findPrev(name);
}



void XDFTabTreeView::selectAll(QString &name)
{
    if (count() > 0)
        ((XDFTreeView *) currentWidget())->selectAll(name);
}



void XDFTabTreeView::setDefaultExpanded(bool expanded)
{
     default_expanded = expanded;
}



bool XDFTabTreeView::defaultExpanded()
{
     return default_expanded;
}



void XDFTabTreeView::expandAll()
{
    if (count() > 0)
        ((XDFTreeView *) currentWidget())->expandAll();
}



void XDFTabTreeView::expandAllTabs()
{
    for (int i = 0; i < count(); ++i)
        ((XDFTreeView *) widget(i))->expandAll();
}



void XDFTabTreeView::collapseAll()
{
    if (count() > 0)
        ((XDFTreeView *) currentWidget())->collapseAll();
}



void XDFTabTreeView::collapseAllTabs()
{
    for (int i = 0; i < count(); ++i)
        ((XDFTreeView *) widget(i))->collapseAll();
}



void XDFTabTreeView::showDataTable()
{
    if (count() > 0)
        ((XDFTreeView *) currentWidget())->showDataTable();
}



void XDFTabTreeView::changeToNextTab()
{
    setCurrentIndex(currentIndex() + 1);
}



void XDFTabTreeView::changeToPreviousTab()
{
    setCurrentIndex(currentIndex() - 1);

}



void XDFTabTreeView::closeTab(int index)
{
        removeTab(index);
}



void XDFTabTreeView::closeCurrentTab()
{
    if (count() > 0)
        removeTab(indexOf(currentWidget()));
}


void XDFTabTreeView::setFontSize(int size)
{
    font_size = size;

    for (int i = 0; i < count(); ++i)
        ((XDFTreeView *) widget(i))->setFontSize(size);
}



int XDFTabTreeView::fontSize()
{
    return font_size;
}



void XDFTabTreeView::changeFontSize(int delta)
{
    font_size += delta;

    for (int i = 0; i < count(); ++i)
        ((XDFTreeView *) widget(i))->changeFontSize(delta);
}



void XDFTabTreeView::increaseFontSize()
{
    changeFontSize(+1);
}



void XDFTabTreeView::decreaseFontSize()
{
    changeFontSize(-1);
}



int XDFTabTreeView::defaultFontSize()
{
    return default_font_size;
}



void XDFTabTreeView::setDefaultFontSize(int size)
{
    default_font_size = size;
}



void XDFTabTreeView::useDefaultFontSize()
{
    setFontSize(default_font_size);
}



bool XDFTabTreeView::isColorized()
{
     return is_colorized;
}



void XDFTabTreeView::setColorized(bool colorize)
{
    if (colorize == is_colorized)
        return;

    is_colorized = colorize;

    for (int i = 0; i < count(); ++i)
        ((XDFTreeView *) widget(i))->colorizeAll(colorize);

    emit(colorizedChanged(colorize));
}
