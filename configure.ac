dnl Process this file with autoconf to produce a configure script.
AC_PREREQ(2.53)

AC_INIT([gnome-themes-standard], [3.5.1],
        [http://bugzilla.gnome.org/enter_bug.cgi?product=gnome-themes-standard])

AM_INIT_AUTOMAKE([1.9 tar-ustar foreign dist-xz no-dist-gzip])

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

AC_ARG_ENABLE([gtk3-engines],
              AC_HELP_STRING([--disable-gtk3-engines],
                             [Disable Gtk+ 3.x engines [default=no]]),
              enable_gtk3=$enableval,
              enable_gtk3=yes)
AC_MSG_CHECKING([building gtk3 engines])
AM_CONDITIONAL(GTK3, test "x$enable_gtk3" = "xyes")
AC_MSG_RESULT([$enable_gtk3])
GTK_VERSION_REQUIRED=3.3.14

if test "x$enable_gtk3" = "xyes"; then
  PKG_CHECK_MODULES([DEPENDENCIES], [gtk+-3.0 >= $GTK_VERSION_REQUIRED librsvg-2.0])
  PKG_CHECK_MODULES([HIGHCONTRAST], [cairo gio-2.0 gdk-pixbuf-2.0 gdk-3.0])
fi
AC_SUBST(DEPENDENCIES_CFLAGS)
AC_SUBST(DEPENDENCIES_LIBS)

GTK_VERSION=`$PKG_CONFIG --variable=gtk_binary_version gtk+-3.0`
AC_SUBST(GTK_VERSION)

GLIB_COMPILE_RESOURCES=`$PKG_CONFIG --variable glib_compile_resources gio-2.0`
AC_SUBST(GLIB_COMPILE_RESOURCES)

# Configure option to force placeholders
AC_ARG_ENABLE([placeholders],
              AC_HELP_STRING([--enable-placeholders],
                             [Enable placeholder icons [default=no]]),
              enable_placeholders=$enableval,
              enable_placeholders=no)
AM_CONDITIONAL(PLACEHOLDERS, test "x$enable_placeholders" != "xno")

AC_PATH_PROG(GTK_UPDATE_ICON_CACHE, gtk-update-icon-cache)
if test -z "$GTK_UPDATE_ICON_CACHE"; then
    AC_MSG_ERROR([Could not find gtk-update-icon-cache])
fi

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
themes/Adwaita/gtk-3.0/borders/Makefile
themes/Adwaita/metacity-1/Makefile
themes/LowContrast/Makefile
themes/LowContrast/gtk-2.0/Makefile
themes/LowContrast/gtk-3.0/Makefile
themes/LowContrast/icons/Makefile
themes/LowContrast/icons/48x48/Makefile
themes/LowContrast/icons/48x48/actions/Makefile
themes/LowContrast/icons/48x48/apps/Makefile
themes/LowContrast/icons/48x48/categories/Makefile
themes/LowContrast/icons/48x48/devices/Makefile
themes/LowContrast/icons/48x48/stock/Makefile
themes/HighContrast/Makefile
themes/HighContrast/gtk-2.0/Makefile
themes/HighContrast/gtk-3.0/Makefile
themes/HighContrast/icons/Makefile
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
