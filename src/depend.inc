ghdf5_util.o: ghdf5_util.c gutil.h ghdf5.h
ghdf_util.o: ghdf_util.c gutil.h ghdf.h
gnetcdf_util.o: gnetcdf_util.c gutil.h gnetcdf.h
hdf5tableview.o: hdf5tableview.cpp xdfv.h hdf5tableview.h xdftableview.h
hdf5treeview.o: hdf5treeview.cpp xdfv.h hdf5tableview.h xdftableview.h \
 hdf5treeview.h xdftreeview.h
hdftableview.o: hdftableview.cpp xdfv.h hdftableview.h xdftableview.h
hdftreeview.o: hdftreeview.cpp xdfv.h hdftableview.h xdftableview.h \
 hdftreeview.h xdftreeview.h
nctableview.o: nctableview.cpp xdfv.h nctableview.h xdftableview.h
nctreeview.o: nctreeview.cpp xdfv.h nctableview.h xdftableview.h \
 nctreeview.h xdftreeview.h
xdfmainwindow.o: xdfmainwindow.cpp xdfv.h version.h hdftreeview.h \
 xdftreeview.h hdf5treeview.h nctreeview.h xdfmainwindow.h \
 xdftabtreeview.h
xdftableview.o: xdftableview.cpp xdfv.h xdftableview.h
xdftabtreeview.o: xdftabtreeview.cpp xdfv.h xdftabtreeview.h \
 xdftreeview.h
xdftreeview.o: xdftreeview.cpp xdfv.h xdftreeview.h
xdfv.o: xdfv.cpp version.h xdfv.h xdfmainwindow.h xdftabtreeview.h \
 xdftreeview.h
