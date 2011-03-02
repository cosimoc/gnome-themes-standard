themedir = $(datadir)/icons/HighContrast
size = scalable
context = categories

icondir = $(themedir)/$(size)/$(context)

icon_DATA = \
	applications-accessories.svg \
	applications-development.svg \
	applications-games.svg \
	applications-graphics.svg \
	applications-internet.svg \
	applications-multimedia.svg \
	applications-system.svg \
	preferences-desktop-peripherals.svg \
	preferences-desktop.svg \
	preferences-system.svg

placeholder_FILES = \
	applications-engineering.svg \
	applications-office.svg \
	applications-other.svg \
	applications-science.svg \
	applications-utilities.svg \
	preferences-desktop-personal.svg \
	preferences-other.svg \
	preferences-system-network.svg \
	system-help.svg

if PLACEHOLDERS
icon_DATA += $(placeholder_FILES)
endif

EXTRA_DIST = \
	$(icon_DATA)
	$(placeholder_FILES)

uninstall-am: uninstall-iconDATA
