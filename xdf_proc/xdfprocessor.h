/*******************************************************************************
 *
 *    Copyright (C) 2015-2018 Greg McGarragh <greg.mcgarragh@colostate.edu>
 *
 *    This source code is licensed under the GNU General Public License (GPL),
 *    Version 3.  See the file COPYING for more details.
 *
 ******************************************************************************/

#ifndef XDFPROCESSOR_H
#define XDFPROCESSOR_H


#define MAX_DEPTH 64


class XDFProcessor
{
public:
    enum ErrorCode {
         FileNotFound = 1,
         UnableToOpenFile
    };
};

#endif /* XDFPROCESSOR_H */
