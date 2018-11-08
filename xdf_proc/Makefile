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

SUBDIRS =

OBJECTS = hdfprocessor.o \
          hdf5processor.o \
          ncprocessor.o

BINARIES =

PRODUCTS =

all: libxdf_proc.a

libxdf_proc.a: ${OBJECTS}
	ar -rs libxdf_proc.a ${OBJECTS}

include depend.inc
