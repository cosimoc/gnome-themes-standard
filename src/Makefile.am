
INCLUDES = $(GTK_CFLAGS)

enginedir = $(libdir)/gtk-3.0/$(GTK_VERSION)/theming-engines
engine_LTLIBRARIES = libadwaita.la

libadwaita_la_SOURCES =			\
	adwaita_engine.c

libadwaita_la_LDFLAGS = -module -avoid-version -no-undefined -export-symbols $(top_srcdir)/src/engine.symbols
libadwaita_la_LIBADD =  $(GTK_LIBS)

EXTRA_DIST = engine.symbols

-include $(top_srcdir)/git.mk
