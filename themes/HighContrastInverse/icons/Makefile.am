themedir = $(datadir)/icons/HighContrastInverse

theme_DATA = index.theme

SUBDIRS = 48x48

gtk_update_icon_cache = $(GTK_UPDATE_ICON_CACHE) -f -t $(themedir)

install-data-hook: update-icon-cache
uninstall-hook: update-icon-cache

update-icon-cache:
	@-if test -z "$(DESTDIR)"; then \
		echo "Updating Gtk icon cache."; \
		$(gtk_update_icon_cache); \
	else \
		echo "*** Icon cache not updated.  After (un)install, run this:"; \
		echo "***   $(gtk_update_icon_cache)"; \
	fi

EXTRA_DIST = index.theme

