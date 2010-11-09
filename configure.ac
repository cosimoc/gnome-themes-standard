dnl Process this file with autoconf to produce a configure script.
AC_PREREQ(2.53)

AC_INIT([gnome-themes-standard], [2.91.2],
        [http://bugzilla.gnome.org/enter_bug.cgi?product=gnome-themes-standard])

AM_INIT_AUTOMAKE([1.9 tar-ustar foreign dist-bzip2 no-dist-gzip])

PKG_PROG_PKG_CONFIG([0.19])
AC_PROG_SED
IT_PROG_INTLTOOL([0.35.0])

GETTEXT_PACKAGE="${PACKAGE}"
AC_SUBST(GETTEXT_PACKAGE)

AM_GLIB_GNU_GETTEXT

# Workaround to make aclocal get the right flags
AC_SUBST(ACLOCAL_AMFLAGS, "\${ACLOCAL_FLAGS}")

AC_CONFIG_FILES([
Makefile
themes/Makefile
themes/Adwaita/Makefile
themes/Adwaita/backgrounds/Makefile
themes/Adwaita/gtk-3.0/Makefile
themes/Adwaita/metacity-1/Makefile
po/Makefile.in
])

AC_OUTPUT
