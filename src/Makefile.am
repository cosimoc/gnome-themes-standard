enginedir = $(libdir)/gtk-3.0/$(GTK_VERSION)/theming-engines
engine_LTLIBRARIES = libadwaita.la

assetsdir = $(datadir)/themes/Adwaita/gtk-3.0/assets

INCLUDES = \
	-DASSETS_DIR=\""$(assetsdir)"\" \
	$(DEPENDENCIES_CFLAGS)

libadwaita_la_SOURCES =			\
	adwaita_utils.h			\
	adwaita_utils.c			\
	adwaita_engine.c

libadwaita_la_LDFLAGS = -module -avoid-version -no-undefined -export-symbols $(top_srcdir)/src/engine.symbols
libadwaita_la_LIBADD =  $(DEPENDENCIES_LIBS)

EXTRA_DIST = engine.symbols

-include $(top_srcdir)/git.mk
