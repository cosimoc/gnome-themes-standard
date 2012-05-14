SUBDIRS = gtk-3.0 gtk-2.0 icons

THEME_NAME=HighContrast
THEME_IN_FILES=index.theme.in

include Makefile-icons.am

%.theme:   %.theme.in $(INTLTOOL_MERGE) $(wildcard $(top_srcdir)/po/*.po) ; $(INTLTOOL_MERGE) $(top_srcdir)/po $< $@ -d -u -c $(top_builddir)/po/.intltool-merge-cache

themedir = $(datadir)/themes/$(THEME_NAME)

theme_DATA = index.theme

noinst_PROGRAMS =
if GTK3
noinst_PROGRAMS += create-highcontrast
endif

create_highcontrast_SOURCES = create-highcontrast.c
create_highcontrast_CFLAGS = $(HIGHCONTRAST_CFLAGS)
create_highcontrast_LDADD = $(HIGHCONTRAST_LIBS) -lm

DISTCLEANFILES = index.theme
EXTRA_DIST += index.theme.in index.theme

-include $(top_srcdir)/git.mk
