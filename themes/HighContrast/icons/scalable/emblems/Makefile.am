themedir = $(datadir)/icons/HighContrast
size = scalable
context = emblems

icondir = $(themedir)/$(size)/$(context)

icon_DATA = \
	emblem-default.svg \
	emblem-documents.svg \
	emblem-downloads.svg \
	emblem-favorite.svg \
	emblem-important.svg \
	emblem-mail.svg \
	emblem-photos.svg \
	emblem-readonly.svg \
	emblem-symbolic-link.svg \
	emblem-system.svg \
	emblem-unreadable.svg

placeholder_FILES = \
	emblem-shared.svg \
	emblem-synchronized.svg

if PLACEHOLDERS
icon_DATA += $(placeholder_FILES)
endif

EXTRA_DIST = \
	$(icon_DATA) \
	$(placeholder_FILES)

uninstall-am: uninstall-iconDATA
