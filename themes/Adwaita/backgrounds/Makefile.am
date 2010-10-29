
backgrounddir = $(datadir)/themes/Adwaita/backgrounds
background_DATA = \
        stripes.jpg

metadata_in_files = adwaita.xml.in
metadatadir = $(datadir)/gnome-background-properties
metadata_DATA = $(metadata_in_files:.xml.in=.xml)

adwaita.xml.in: adwaita.xml.in.in Makefile
	$(AM_V_GEN) $(SED) -e 's^\@datadir\@^$(datadir)^g' < $(srcdir)/adwaita.xml.in.in > adwaita.xml.in.tmp \
	&& mv adwaita.xml.in.tmp adwaita.xml.in

@INTLTOOL_XML_RULE@

noinst_DATA = adwaita.xml.in
CLEANFILES = adwaita.xml adwaita.xml.in

EXTRA_DIST = $(background_DATA) adwaita.xml.in.in
