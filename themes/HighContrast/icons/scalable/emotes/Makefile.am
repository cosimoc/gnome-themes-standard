themedir = $(datadir)/icons/HighContrast
size = scalable
context = emotes

icondir = $(themedir)/$(size)/$(context)

icon_DATA = \
	face-cool.svg \
	face-plain.svg \
	face-sad.svg \
	face-smile.svg \
	face-smile-big.svg \
	face-surprise.svg \
	face-wink.svg

placeholder_FILES = \
	face-angel.svg \
	face-angry.svg \
	face-crying.svg \
	face-devilish.svg \
	face-embarrassed.svg \
	face-kiss.svg \
	face-laugh.svg \
	face-monkey.svg \
	face-raspberry.svg \
	face-sick.svg \
	face-smirk.svg \
	face-tired.svg \
	face-uncertain.svg \
	face-worried.svg

if PLACEHOLDERS
icon_DATA += $(placeholder_FILES)
endif

EXTRA_DIST = \
	$(icon_DATA) \
	$(placeholder_FILES)

uninstall-am: uninstall-iconDATA
