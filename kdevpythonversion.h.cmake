/*
    SPDX-FileCopyrightText: 2012 Sven Brauch <svenbrauch@googlemail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KDEVPYTHON_VERSION_H
#define KDEVPYTHON_VERSION_H

#define PYTHON_VERSION_MINOR @PYTHON_VERSION_MINOR@
#define PYTHON_VERSION_MINOR_STR "@PYTHON_VERSION_MINOR@"

#define PYTHON_VERSION_MAJOR @PYTHON_VERSION_MAJOR@
#define PYTHON_VERSION_MAJOR_STR "@PYTHON_VERSION_MAJOR@"

#define PYTHON_VERSION QT_VERSION_CHECK(@PYTHON_VERSION_MAJOR@, @PYTHON_VERSION_MINOR@, @PYTHON_VERSION_PATCH@)
#define PYTHON_VERSION_STR "@PYTHON_VERSION_MAJOR@.@PYTHON_VERSION_MINOR@"

#define PYTHON_EXECUTABLE "@PYTHON_EXECUTABLE@"
#define PYTHON_LIBRARY "@PYTHON_LIBRARIES@"

#endif
