#*******************************************************************************
#
#*******************************************************************************
CCFLAGS += -DGBYTE_ORDER=BYTE_ORDER_LE -DNATIVE_SIZE=NATIVE_SIZE_64_BIT -DPLATFORM=LINUX_GCC -DUNIX

MAKE_SUBDIRS  = + \
                @target=`echo $@ | sed s/-subdirs//`; \
                list='$(SUBDIRS)'; \
                for subdir in $$list; do \
                     echo "Making $$target in $$subdir"; \
                     (cd $$subdir && $(MAKE) $$target) || exit 1; \
                done;

all: all-subdirs

all-subdirs:
	$(MAKE_SUBDIRS)

clean: clean-subdirs
	rm -f *.a *.o *.so lib*.so.* $(OBJECTS) $(BINARIES) $(PRODUCTS)

clean-subdirs:
	$(MAKE_SUBDIRS)

README: readme_source.txt
	fold --spaces --width=80 readme_source.txt > README
	sed -i 's/[ \t]*$$//' README

.SUFFIXES: .c .cpp

.c.o:
	$(CC) $(CCFLAGS) -c $(CCDEFINES) $(INCDIRS) -o $*.o $<

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $(CCDEFINES) $(INCDIRS) -o $*.o $<

# Requires gcc
depend: depend-subdirs
	@files=`find . -maxdepth 1 -name "*.c" | sort`; \
        if test $${#files} != 0; then \
             if (eval gcc -v 1> /dev/null 2>&1); then \
                  echo gcc -MM -w $$files "> depend.inc"; \
                  gcc -MM -w $$files > depend.inc; \
             else \
                  echo makedepend -f- -Y -- -- $$files "> depend.inc"; \
                  makedepend -f- -Y -- -- $$files > depend.inc; \
             fi \
        else \
             echo -n '' > depend.inc; \
        fi
	@files=`find . -maxdepth 1 -name "*.cpp" | sort`; \
        if test $${#files} != 0; then \
             if (eval gcc -v 1> /dev/null 2>&1); then \
                  echo gcc -MM -std=c++0x -w ${CCDEFINES} $$files ">> depend.inc"; \
                  gcc -MM -std=c++0x -w ${CCDEFINES} $$files >> depend.inc; \
             else \
                  echo makedepend -f- -Y -- ${CCDEFINES} -- $$files ">> depend.inc"; \
                  makedepend -f- -Y -- ${CCDEFINES} -- $$files >> depend.inc; \
             fi \
        else \
             echo -n '' >> depend.inc; \
        fi

depend-subdirs:
	${MAKE_SUBDIRS}
