AM_CFLAGS = $(GOBJECT_CFLAGS) 
AM_LDFLAGS = -module -avoid-version 
LIBS = $(GOBJECT_LIBS)

check_PROGRAMS = gitestrepo gitestthrows gitypelibtest gitestwrite gitestenum

gitestrepo_SOURCES = $(srcdir)/gitestrepo.c
gitestrepo_CPPFLAGS = $(GIREPO_CFLAGS) -I$(top_srcdir)/girepository
gitestrepo_LDADD = $(top_builddir)/libgirepository-1.0.la $(GIREPO_LIBS)

gitestwrite_SOURCES = $(srcdir)/gitestwrite.c
gitestwrite_CPPFLAGS = $(GIREPO_CFLAGS) -I$(top_srcdir)/girepository
gitestwrite_LDADD = $(top_builddir)/libgirepository-1.0.la $(GIREPO_LIBS)

gitestenum_SOURCES = $(srcdir)/gitestenum.c
gitestenum_CPPFLAGS = $(GIREPO_CFLAGS) -I$(top_srcdir)/girepository
gitestenum_LDADD = $(top_builddir)/libgirepository-1.0.la $(top_builddir)/libgirepository-internals.la $(top_builddir)/libgirepository-gthash.la $(GIREPO_LIBS)

gitestthrows_SOURCES = $(srcdir)/gitestthrows.c
gitestthrows_CPPFLAGS = $(GIREPO_CFLAGS) -I$(top_srcdir)/girepository
gitestthrows_LDADD = $(top_builddir)/libgirepository-1.0.la $(GIREPO_LIBS)

gitypelibtest_SOURCES = $(srcdir)/gitypelibtest.c
gitypelibtest_CPPFLAGS = $(GIREPO_CFLAGS) -I$(top_srcdir)/girepository
gitypelibtest_LDADD = $(top_builddir)/libgirepository-1.0.la $(GIREPO_LIBS)

TESTS = gitestwrite gitestrepo gitestthrows gitypelibtest gitestenum
TESTS_ENVIRONMENT=env GI_TYPELIB_PATH=$(top_builddir):$(top_builddir)/gir:$(top_builddir)/tests:$(top_builddir)/tests/scanner: \
   XDG_DATA_DIRS="$(top_srcdir)/gir:$(XDG_DATA_DIRS)" $(DEBUG)
