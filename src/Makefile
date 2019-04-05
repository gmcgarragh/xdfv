#*******************************************************************************
#
# Copyright (C) 2015-2018 Greg McGarragh <greg.mcgarragh@colostate.edu>
#
# This source code is licensed under the GNU General Public License (GPL),
# Version 3.  See the file COPYING for more details.
#
#*******************************************************************************

include ../make.inc
include ../common.inc

INCDIRS += -I. -I../xdf_proc

SUBDIRS =

OBJECTS = xdfv.o \
          ghdf_util.o \
          ghdf5_util.o \
          gnetcdf_util.o \
          hdftableview.o \
          hdftableview_moc.o \
          hdftreeview.o \
          hdftreeview_moc.o \
          hdf5tableview.o \
          hdf5tableview_moc.o \
          hdf5treeview.o \
          hdf5treeview_moc.o \
          nctableview.o \
          nctableview_moc.o \
          nctreeview.o \
          nctreeview_moc.o \
          xdfmainwindow.o \
          xdfmainwindow_moc.o \
          xdftableview.o \
          xdftableview_moc.o \
          xdftabtreeview.o \
          xdftabtreeview_moc.o \
          xdftreeview.o \
          xdftreeview_moc.o \
          version.o

MOC_PRODUCTS = hdftableview_moc.cpp \
               hdftreeview_moc.cpp \
               hdf5tableview_moc.cpp \
               hdf5treeview_moc.cpp \
               nctableview_moc.cpp \
               nctreeview_moc.cpp \
               xdfmainwindow_moc.cpp \
               xdftableview_moc.cpp \
               xdftabtreeview_moc.cpp \
               xdftreeview_moc.cpp

BINARIES = ../xdfv

PRODUCTS = $(MOC_PRODUCTS) version.cpp

all: $(BINARIES)

../xdfv: $(OBJECTS) ../xdf_proc/libxdf_proc.a
	$(CXX) $(CXXFLAGS) $(CXXDEFINES) -o ../xdfv $(OBJECTS) \
        $(INCDIRS) $(LIBDIRS) $(LINKS) ../xdf_proc/libxdf_proc.a

version.o: version.cpp version.h

# Requires UNIX/LINUX date command and git
version.cpp:
	echo "#include \"version.h\"" > version.cpp; \
        echo "" >> version.cpp; \
        echo "const char *build_date  = \"`date +"%F %X %z"`\";" >> version.cpp; \
        echo "const char *build_sha_1 = \"`git describe --abbrev=7 --dirty --always --tags`\";" >> version.cpp

hdftableview_moc.cpp: hdftableview.h
	${MOC} hdftableview.h -o hdftableview_moc.cpp

hdftreeview_moc.cpp: hdftreeview.h
	${MOC} hdftreeview.h -o hdftreeview_moc.cpp

hdf5tableview_moc.cpp: hdf5tableview.h
	${MOC} hdf5tableview.h -o hdf5tableview_moc.cpp

hdf5treeview_moc.cpp: hdf5treeview.h
	${MOC} hdf5treeview.h -o hdf5treeview_moc.cpp

nctableview_moc.cpp: nctableview.h
	${MOC} nctableview.h -o nctableview_moc.cpp

nctreeview_moc.cpp: nctreeview.h
	${MOC} nctreeview.h -o nctreeview_moc.cpp

xdfmainwindow_moc.cpp: xdfmainwindow.h
	${MOC} xdfmainwindow.h -o xdfmainwindow_moc.cpp

xdftableview_moc.cpp: xdftableview.h
	${MOC} xdftableview.h -o xdftableview_moc.cpp

xdftabtreeview_moc.cpp: xdftabtreeview.h
	${MOC} xdftabtreeview.h -o xdftabtreeview_moc.cpp

xdftreeview_moc.cpp: xdftreeview.h
	${MOC} xdftreeview.h -o xdftreeview_moc.cpp

include depend.inc
