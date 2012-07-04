

themedir = $(datadir)/themes/Adwaita/gtk-2.0
theme_DATA = gtkrc

asset_dirs = "Arrows" "Buttons" "Check-Radio" "Entry" "Expanders" \
	"Handles" "Lines" "Menu-Menubar" "Others" "ProgressBar" \
	"Range" "Scrollbars" "Shadows" "Spin" "Tabs"
asset_data =
	Arrows/arrow-down-insens.png \
	Arrows/arrow-down.png \
	Arrows/arrow-down-prelight.png \
	Arrows/arrow-down-pressed.png \
	Arrows/arrow-left.png \
	Arrows/arrow-right.png \
	Arrows/arrow-right-prelight.png \
	Arrows/arrow-up.png \
	Arrows/menu-arrow.png \
	Arrows/menu-arrow-prelight.png \
	Buttons/button-default.png \
	Buttons/button-insensitive.png \
	Buttons/button-normal.png \
	Buttons/button-prelight.png \
	Buttons/button-pressed.png \
	Check-Radio/check1.png \
	Check-Radio/check2.png \
	Check-Radio/menucheck.png \
	Check-Radio/menucheck_prelight.png \
	Check-Radio/menuoption.png \
	Check-Radio/menuoption_prelight.png \
	Check-Radio/option1.png \
	Check-Radio/option2.png \
	Entry/entry.png \
	Entry/entry-active.png \
	Expanders/ani.png \
	Expanders/minus.png \
	Expanders/plus.png \
	Handles/handle-h.png \
	Handles/handle-v.png \
	Lines/line-h.png \
	Lines/line-v.png \
	Lines/menu_line_h.png \
	Menu-Menubar/menubar_button.png \
	Menu-Menubar/menubar.png \
	Others/bg.png \
	Others/focus.png \
	Others/null.png \
	Others/ruler.png \
	Others/tree_header.png \
	ProgressBar/progressbar.png \
	ProgressBar/progressbar_v.png \
	ProgressBar/trough-progressbar.png \
	Range/slider-horiz.png \
	Range/slider-horiz-prelight.png \
	Range/slider-vert.png \
	Range/slider-vert-prelight.png \
	Range/trough-horizontal.png \
	Range/trough-vertical.png \
	Scrollbars/slider-horiz-active.png \
	Scrollbars/slider-horiz.png \
	Scrollbars/slider-vert-active.png \
        Scrollbars/slider-vert.png \
	Scrollbars/slider-horiz-insens.png \
	Scrollbars/slider-horiz-prelight.png \
	Scrollbars/slider-vert-insens.png \
	Scrollbars/slider-vert-prelight.png \
        Scrollbars/trough-scrollbar-horiz.png \
	Scrollbars/trough-scrollbar-vert.png \
	Shadows/frame.png \
	Spin/arrow-down-disable.png \
	Spin/arrow-down.png \
	Spin/arrow-down-prelight.png \
	Spin/arrow-up-disable.png \
	Spin/arrow-up.png \
	Spin/arrow-up-prelight.png \
	Spin/down-background.png \
	Spin/up-background.png
	Tabs/notebook-gap-bottom.png \
	Tabs/notebook-gap-right.png \
	Tabs/notebook-gap-left.png \
	Tabs/notebook-gap-top.png \
	Tabs/notebook.png \
	Tabs/tab-bottom.png \
	Tabs/tab-left.png \
	Tabs/tab-right.png \
	Tabs/tab-top.png \
	Tabs/tab-bottom-active.png \
	Tabs/tab-left-active.png \
	Tabs/tab-right-active.png \
	Tabs/tab-top-active.png
install-data-local:
	for dir in $(asset_dirs); do \
		for file in `cd $(srcdir)/$$dir && find . -maxdepth 1 \( -name "*.png" \)`; do \
			$(mkdir_p) $(DESTDIR)$(datadir)/themes/Adwaita/gtk-2.0/$$dir; \
			$(install_sh_DATA) $(srcdir)/$$dir/$$file $(DESTDIR)$(themedir)/$$dir/$$file; \
		done; \
	done

uninstall-local:
	for dir in $(asset_dirs); do \
		for file in `cd $(srcdir)/$$dir && find . -maxdepth 1 \( -name "*.png" \)`; do \
			rm -f $(DESTDIR)$(datadir)/themes/Adwaita/gtk-2.0/$$dir; \
		done; \
	done

EXTRA_DIST = $(theme_DATA) $(asset_data)

-include $(top_srcdir)/git.mk
