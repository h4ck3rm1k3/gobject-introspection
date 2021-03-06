#!/usr/bin/env python
# Scan glib sources.
# e.g.:
#   ./update-glib-annotations.py ../../glib ../../glib/_build

import os
import sys
import subprocess

possible_builddirs = ['../_build/', '..']
builddir = None
for d in possible_builddirs:
    if os.path.isfile(os.path.join(d, 'g-ir-annotation-tool')):
        builddir = d
        break
assert builddir is not None
annotation_tool_base_args = [os.path.join(builddir, 'g-ir-annotation-tool'), '--extract']

def directory_includes(dirs, srcdir, builddir):
    result = []
    result.append('-I' + srcdir)
    if srcdir != builddir:
        result.append('-I' + builddir)
    for name in dirs:
        result.append('-I' + os.path.join(srcdir, name))
        if srcdir != builddir:
            result.append('-I' + os.path.join(builddir, name))
    return result

def extract_glib_annotations(srcdir, builddir, outfile):
    projname = 'glib'
    headersfile = os.path.join(builddir, projname, projname + '-public-headers.txt')
    subdir = os.path.join(srcdir, projname)
    f = open(headersfile)
    line = f.read()
    f.close()
    sources = []
    for headername in line.split(' '):
        headername = headername.strip()
        if headername == 'gi18n-lib.h':
            continue
        sources.append(os.path.join(subdir, headername))
    for sourcename in os.listdir(subdir):
        if sourcename.endswith('.c'):
            sources.append(os.path.join(subdir, sourcename))
    return subprocess.check_call(annotation_tool_base_args +
                                 ['-DGLIB_COMPILATION'] + directory_includes(['glib', 'gmodule'], srcdir, builddir) + sources,
                                 stdout=outfile)

def extract_gobject_annotations(srcdir, builddir, outfile):
    projname = 'gobject'
    headersfile = os.path.join(builddir, projname, projname + '-public-headers.txt')
    subdir = os.path.join(srcdir, projname)
    f = open(headersfile)
    line = f.read()
    f.close()
    sources = []
    for headername in line.split(' '):
        headername = headername.strip()
        sources.append(os.path.join(subdir, headername))
    for sourcename in os.listdir(subdir):
        if sourcename.endswith('.c'):
            sources.append(os.path.join(subdir, sourcename))
    return subprocess.check_call(annotation_tool_base_args +
                                 ['-DGOBJECT_COMPILATION'] + directory_includes(['glib', 'gobject', 'gmodule'], srcdir, builddir) + sources,
                                 stdout=outfile)

def extract_gio_annotations(srcdir, builddir, outfile):
    projname = 'gio'
    headersfile = os.path.join(builddir, projname, projname + '-public-headers.txt')
    subdir = os.path.join(srcdir, projname)
    f = open(headersfile)
    line = f.read()
    f.close()
    sources = []
    for headername in line.split(' '):
        headername = headername.strip()
        sources.append(os.path.join(subdir, headername))
    for sourcename in os.listdir(subdir):
        if sourcename.endswith('.c'):
            sources.append(os.path.join(subdir, sourcename))
    return subprocess.check_call(annotation_tool_base_args +
                                 ['-DGOBJECT_COMPILATION',
                                  '-DGIO_COMPILATION'] + directory_includes(['glib', 'gmodule', 'gobject', 'gio'], srcdir, builddir) + sources,
                                 stdout=outfile)

if __name__ == '__main__':
    srcdir = sys.argv[1]
    if len(sys.argv) == 3:
        builddir = sys.argv[2]
    else:
        builddir = srcdir

    print "Using source directory: %r build directory: %r" % (srcdir, builddir)

    srcname = '../gir/glib-2.0.c'
    srcfile = open(srcname + '.tmp', 'w')
    extract_glib_annotations(srcdir, builddir, srcfile)
    srcfile.close()
    os.rename(srcname + '.tmp', srcname)

    print "Updated %r" % (srcname, )

    srcname = '../gir/gobject-2.0.c'
    srcfile = open(srcname + '.tmp', 'w')
    extract_gobject_annotations(srcdir, builddir, srcfile)
    srcfile.close()
    os.rename(srcname + '.tmp', srcname)

    print "Updated %r" % (srcname, )

    srcname = '../gir/gio-2.0.c'
    srcfile = open(srcname + '.tmp', 'w')
    extract_gio_annotations(srcdir, builddir, srcfile)
    srcfile.close()
    os.rename(srcname + '.tmp', srcname)

    print "Updated %r" % (srcname, )

    print "Done; run \"git diff\" to see any changes."

