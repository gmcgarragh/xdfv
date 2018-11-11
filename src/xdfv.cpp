/*******************************************************************************
 *
 *    Copyright (C) 2015-2018 Greg McGarragh <greg.mcgarragh@colostate.edu>
 *
 *    This source code is licensed under the GNU General Public License (GPL),
 *    Version 3.  See the file COPYING for more details.
 *
 ******************************************************************************/

#include <qapplication.h>

#include "version.h"
#include "xdfv.h"
#include "xdfmainwindow.h"


const char *program_name = "xdfv";
const char *PROGRAM_NAME = "XDFV";


#define MAX_FILES 128


int string_to_int (const std::string &s);
double string_to_double(const std::string &s);
void usage();
void version();


int main(int argc, char *argv[])
{
    char *file_names[MAX_FILES];

    int i_file;
    int n_files;
    int view_in_color;
    int expand_all;
    int collapse_all;
    int font_size;

    int window_width;
    int window_height;

    int assume_sds[MAX_FILES];

    XDFMainWindow *main_window;

    XDFV::FileType file_types[MAX_FILES];


    /*--------------------------------------------------------------------------
     *
     *------------------------------------------------------------------------*/
    i_file = -1;

    expand_all    = 1;
    collapse_all  = 0;
    font_size     = 0;
    view_in_color = 1;
    window_width  = 850;
    window_height = 400;

    for (int i = 0; i < MAX_FILES; ++i)
        assume_sds[i] = 0;

    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] == '-') {
            if (strcmp(argv[i], "--expand_all") == 0) {
                expand_all   = 1;
                collapse_all = 0;
            }
            else if (strcmp(argv[i], "--collapse_all") == 0) {
                expand_all   = 0;
                collapse_all = 1;
            }
            else if (strcmp(argv[i], "--font_size") == 0) {
                try {
                    font_size = string_to_int(argv[++i]);
                }
                catch (...) {
                    fprintf(stderr, "ERROR: Invalid value for --font_size <size>: %s\n", argv[i]);
                    exit(1);
                }
            }
            else if (strcmp(argv[i], "--hdf4") == 0) {
                i_file++;
                file_types[i_file] = XDFV::HDF4;
                file_names[i_file] = argv[++i];
            }
            else if (strcmp(argv[i], "--hdf5") == 0) {
                i_file++;
                file_types[i_file] = XDFV::HDF5;
                file_names[i_file] = argv[++i];
            }
            else if (strcmp(argv[i], "--netcdf") == 0) {
                i_file++;
                file_types[i_file] = XDFV::NetCDF;
                file_names[i_file] = argv[++i];
            }
            else if (strcmp(argv[i], "--help") == 0) {
                usage();
                exit(0);
            }
            else if (strcmp(argv[i], "--sds") == 0)
                assume_sds[i_file] = 1;
            else if (strcmp(argv[i], "--vgroups") == 0)
                assume_sds[i_file] = 0;
            else if (strcmp(argv[i], "--view_in_color") == 0)
                view_in_color = 1;
            else if (strcmp(argv[i], "--no-view_in_color") == 0)
                view_in_color = 0;
            else if (strcmp(argv[i], "--version") == 0) {
                version();
                exit(0);
            }
            else if (strcmp(argv[i], "--window_size") == 0) {
                try {
                    window_width = string_to_int(argv[++i]);
                }
                catch (...) {
                    fprintf(stderr, "ERROR: Invalid value for --window_size <width>: %s\n", argv[i]);
                    exit(1);
                }
                try {
                    window_height = string_to_int(argv[++i]);
                }
                catch (...) {
                    fprintf(stderr, "ERROR: Invalid value for --window_size <height>: %s\n", argv[i]);
                    exit(1);
                }
            }
            else {
                printf("ERROR: Invalid option: %s, use --help for more information\n", argv[i]);
                exit(1);
            }
        }
        else {
            i_file++;
            file_types[i_file] = XDFV::Unknown;
            file_names[i_file] = argv[i];
        }
    }

    n_files = i_file + 1;


    /*--------------------------------------------------------------------------
     *
     *------------------------------------------------------------------------*/
    QApplication a(argc, argv);

    main_window = new XDFMainWindow();
    main_window->resize(window_width, window_height);

    for (int i = 0; i < n_files; ++i) {
        try {
            if (file_types[i] == XDFV::Unknown)
                main_window->openFile(file_names[i], assume_sds[i]);
            else
                main_window->openFile(file_types[i], file_names[i], assume_sds[i]);
        }
        catch (XDFMainWindow::ErrorCode e) {
            if (e == XDFMainWindow::FileNotFound)
                printf("ERROR: File does not exist: %s\n", file_names[i]);
            else if (e == XDFMainWindow::UnknownFileExtension)
                printf("ERROR: Unknown file extension: %s\n", file_names[i]);
            else if (e == XDFMainWindow::UnableToOpenFile)
                printf("ERROR: Unable to open file, invalid format or file corrupt: %s\n",
                       file_names[i]);
            exit(1);
        }
    }

    if (expand_all) {
        main_window->tabTreeView()->expandAllTabs();
        main_window->tabTreeView()->setDefaultExpanded(true);
    }
    if (collapse_all)
        main_window->tabTreeView()->collapseAllTabs();
    if (font_size > 0) {
        main_window->tabTreeView()->setFontSize(font_size);
        main_window->tabTreeView()->setDefaultFontSize(font_size);
    }
    main_window->tabTreeView()->setColorized(view_in_color);

    main_window->show();

    a.exec();

    a.~QApplication();

    exit(0);
}



int string_to_int(const std::string &s)
{
    int result;
    std::size_t pos;

    try {
        result = std::stoi(s, &pos);
    }
    catch (...) {
        throw -1;
    }
    if (pos != s.size())
        throw -1;

    return result;
}



double string_to_double(const std::string &s)
{
    double result;
    std::size_t pos;

    try {
        result = std::stod(s, &pos);
    }
    catch (...) {
        throw -1;
    }
    if (pos != s.size())
        throw -1;

    return result;
}



void usage()
{

    version();
    printf("\n");
    printf("Usage: xdfv [OPTIONS] [FILE 1 | FILE 2 | FILE 3 | ...]\n");
    printf("\n");
    printf("Options:\n");
    printf("    --expand_all:          Start with the tree view expanded.\n");
    printf("    --collapse_all:        Start with the tree view collapsed (default).\n");
    printf("    --font_size <size>:    Font point size.\n");
    printf("    --hdf4   <filename>:   Open \"filename\" as an HDF4 file.\n");
    printf("    --hdf5   <filename>:   Open \"filename\" as an HDF5 file.\n");
    printf("    --netcdf <filename>:   Open \"filename\" as a NetCDF file.\n");
    printf("    --help:                Print this help content.\n");
    printf("    --sds:                 Scan HDF4 file as a set of SDS's, ignore VGroups.\n");
    printf("    --vgroups:             Scan HDF4 through VGroups (default).\n");
    printf("    --view_in_color:       Use color for the tree view (default).\n");
    printf("    --no-view_in_color:    Use b/w for the tree view.\n");
    printf("    --version:             Print source Git hash and build date information.\n");
    printf("    --window_size <w> <h>: Start up window width (w) and height (h).\n");
    printf("\n");
}



void version()
{
    printf("%s, %s, %s\n", PROGRAM_NAME, build_sha_1, build_date);
}
