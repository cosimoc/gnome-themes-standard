dnl Process this file with autoconf to produce a configure script.
AC_PREREQ(2.53)

AC_INIT([gnome-themes-standard], [2.91.91],
        [http://bugzilla.gnome.org/enter_bug.cgi?product=gnome-themes-standard])

AM_INIT_AUTOMAKE([1.9 tar-ustar foreign dist-bzip2 no-dist-gzip])

PKG_PROG_PKG_CONFIG([0.19])

AC_PROG_SED
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AM_PROG_CC_C_O
AM_PATH_GLIB_2_0

dnl Initialize libtool
AM_DISABLE_STATIC
AC_LIBTOOL_WIN32_DLL
AM_PROG_LIBTOOL

dnl Initialize intltool
IT_PROG_INTLTOOL([0.40.0])

GETTEXT_PACKAGE="${PACKAGE}"
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE],["$GETTEXT_PACKAGE"],[Gettext package])
AM_GLIB_GNU_GETTEXT

# Don't forget to update the README when changing this.
PKG_CHECK_MODULES(GTK, gtk+-3.0 >= 3.0.2,,
                  AC_MSG_ERROR([GTK+-3.0 is required to compile gnome-themes-standard]))

AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)

GTK_VERSION=`$PKG_CONFIG --variable=gtk_binary_version gtk+-3.0`
AC_SUBST(GTK_VERSION)

# Configure option to force placeholders
AC_ARG_ENABLE([placeholders],
              AC_HELP_STRING([--enable-placeholders],
                             [Enable placeholder icons [default=no]]),
              enable_placeholders=$enableval,
              enable_placeholders=no)
AM_CONDITIONAL(PLACEHOLDERS, test "x$enable_placeholders" != "xno")

# Workaround to make aclocal get the right flags
AC_SUBST(ACLOCAL_AMFLAGS, "\${ACLOCAL_FLAGS}")

AC_CONFIG_FILES([
Makefile
themes/Makefile
themes/Adwaita/Makefile
themes/Adwaita/backgrounds/Makefile
themes/Adwaita/cursors/Makefile
themes/Adwaita/gtk-2.0/Makefile
themes/Adwaita/gtk-3.0/Makefile
themes/Adwaita/gtk-3.0/assets/Makefile
themes/Adwaita/metacity-1/Makefile
themes/LowContrast/Makefile
themes/LowContrast/gtk-2.0/Makefile
themes/LowContrast/gtk-3.0/Makefile
themes/LowContrast/icons/Makefile
themes/LowContrast/icons/48x48/Makefile
themes/LowContrast/icons/48x48/stock/Makefile
themes/HighContrast/Makefile
themes/HighContrast/gtk-2.0/Makefile
themes/HighContrast/gtk-3.0/Makefile
themes/HighContrast/icons/Makefile
themes/HighContrast/icons/48x48/Makefile
themes/HighContrast/icons/48x48/animations/Makefile
themes/HighContrast/icons/48x48/stock/Makefile
themes/HighContrast/icons/scalable/Makefile
themes/HighContrast/icons/scalable/actions/Makefile
themes/HighContrast/icons/scalable/apps/Makefile
themes/HighContrast/icons/scalable/categories/Makefile
themes/HighContrast/icons/scalable/devices/Makefile
themes/HighContrast/icons/scalable/emblems/Makefile
themes/HighContrast/icons/scalable/emotes/Makefile
themes/HighContrast/icons/scalable/mimetypes/Makefile
themes/HighContrast/icons/scalable/places/Makefile
themes/HighContrast/icons/scalable/status/Makefile
themes/HighContrast/icons/scalable/actions-extra/Makefile
themes/HighContrast/icons/scalable/apps-extra/Makefile
themes/HighContrast/icons/scalable/categories-extra/Makefile
themes/HighContrast/icons/scalable/devices-extra/Makefile
themes/HighContrast/icons/scalable/emblems-extra/Makefile
themes/HighContrast/icons/scalable/emotes-extra/Makefile
themes/HighContrast/icons/scalable/mimetypes-extra/Makefile
themes/HighContrast/icons/scalable/places-extra/Makefile
themes/HighContrast/icons/scalable/status-extra/Makefile
themes/HighContrastInverse/Makefile
themes/HighContrastInverse/gtk-2.0/Makefile
themes/HighContrastInverse/gtk-3.0/Makefile
themes/HighContrastInverse/icons/Makefile
themes/HighContrastInverse/icons/48x48/Makefile
themes/HighContrastInverse/icons/48x48/stock/Makefile
src/Makefile
po/Makefile.in
])

AC_OUTPUT
