themedir = $(datadir)/icons/HighContrast
theme_DATA = index.theme

render_sizes = "16x16" "22x22" "24x24" "32x32" "48x48" "256x256" "scalable"

THEME_DIRS=$(shell SIZES="$(render_sizes)"; for size in $$SIZES; do for dir in `find $(srcdir)/$$size/* -type d`; do printf "$$dir,"; done; done;)

%.theme: %.theme.in Makefile
	dirs="`echo $(THEME_DIRS) | sed -e 's#\./##g'`"; \
	sed -e "s|\@THEME_DIRS\@|$$dirs|g" < $< > $@; \
	echo "" >> $@; \
	for dir in `echo $$dirs | sed -e "s/,/ /g"`; do \
		sizefull="`dirname $$dir`"; \
		if test "$$sizefull" = "scalable"; then \
			size="16"; \
		else \
			size="`echo $$sizefull | sed -e 's/x.*$$//g'`"; \
		fi; \
		context="`basename $$dir`"; \
		echo "[$$dir]" >> $@; \
		if test "$$context" = "actions"; then \
			echo "Context=Actions" >> $@; \
		fi; \
		if test "$$context" = "animations"; then \
			echo "Context=Animations" >> $@; \
		fi; \
		if test "$$context" = "apps"; then \
			echo "Context=Applications" >> $@; \
		fi; \
		if test "$$context" = "categories"; then \
			echo "Context=Categories" >> $@; \
		fi; \
		if test "$$context" = "devices"; then \
			echo "Context=Devices" >> $@; \
		fi; \
		if test "$$context" = "emblems"; then \
			echo "Context=Emblems" >> $@; \
		fi; \
		if test "$$context" = "emotes"; then \
			echo "Context=Emotes" >> $@; \
		fi; \
		if test "$$context" = "intl"; then \
			echo "Context=International" >> $@; \
		fi; \
		if test "$$context" = "mimetypes"; then \
			echo "Context=MimeTypes" >> $@; \
		fi; \
		if test "$$context" = "places"; then \
			echo "Context=Places" >> $@; \
		fi; \
		if test "$$context" = "status"; then \
			echo "Context=Status" >> $@; \
		fi; \
		if test "$$context" = "stock"; then \
			echo "Context=Stock" >> $@; \
		fi; \
		echo "Size=$$size" >> $@; \
		if test "$$sizefull" = "scalable"; then \
			echo "MinSize=8" >> $@; \
			echo "MaxSize=512" >> $@; \
			echo "Type=Scalable" >> $@; \
		elif test "$$size" = "256"; then \
			echo "MinSize=56" >> $@; \
			echo "MaxSize=512" >> $@; \
			echo "Type=Scalable" >> $@; \
		elif test "$$context" = "stock"; then \
			echo "Type=Scalable" >> $@; \
		else \
			echo "Type=Fixed" >> $@; \
		fi; \
		echo "" >> $@; \
	done

@INTLTOOL_THEME_RULE@

install-data-local:
	for size in $(render_sizes); do \
		echo -e "Going to copy files for $$size"; \
		for file in `cd $(srcdir)/$$size && find . \( -name "*.png" -or -name "*.svg" \)`; do \
			context="`dirname $$file`"; \
			$(mkdir_p) $(DESTDIR)$(themedir)/$$size/$$context; \
			$(install_sh_DATA) $(srcdir)/$$size/$$file $(DESTDIR)$(themedir)/$$size/$$file; \
		done; \
	done

uninstall-local:
	for size in $(render_sizes); do \
		for file in `cd $(srcdir)/$$size && find . \( -name "*.png" -or -name "*.svg" \)`; do \
			rm -f $(DESTDIR)$(themedir)/$$size/$$file; \
		done; \
	done

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

EXTRA_DIST = \
	index.theme.in \
	index.theme

DISTCLEANFILES = index.theme
