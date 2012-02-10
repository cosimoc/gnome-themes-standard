gtk.gresource: gtk.gresource.xml  $(shell $(GLIB_COMPILE_RESOURCES) --generate-dependencies $(srcdir)/gtk.gresource.xml)
	$(AM_V_GEN) $(GLIB_COMPILE_RESOURCES) --target=$@ --sourcedir=$(srcdir)  $<

themedir = $(datadir)/themes/HighContrast/gtk-3.0
theme_DATA = \
	gtk.css \
	gtk.gresource

EXTRA_DIST = \
	gtk.css \
	gtk.gresource.xml

-include $(top_srcdir)/git.mk
