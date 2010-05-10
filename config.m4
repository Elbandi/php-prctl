dnl $Id$
dnl config.m4 for extension prctl

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(prctl, for prctl support,
dnl Make sure that the comment is aligned:
dnl [  --with-prctl             Include prctl support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(prctl, whether to enable prctl support,
dnl Make sure that the comment is aligned:
[  --enable-prctl           Enable prctl support])

if test "$PHP_PRCTL" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-prctl -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/prctl.h"  # you most likely want to change this
  dnl if test -r $PHP_PRCTL/$SEARCH_FOR; then # path given as parameter
  dnl   PRCTL_DIR=$PHP_PRCTL
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for prctl files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       PRCTL_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$PRCTL_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the prctl distribution])
  dnl fi

  dnl # --with-prctl -> add include path
  dnl PHP_ADD_INCLUDE($PRCTL_DIR/include)

  dnl # --with-prctl -> check for lib and symbol presence
  dnl LIBNAME=prctl # you may want to change this
  dnl LIBSYMBOL=prctl # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $PRCTL_DIR/lib, PRCTL_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_PRCTLLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong prctl lib version or lib not found])
  dnl ],[
  dnl   -L$PRCTL_DIR/lib -lm
  dnl ])
  dnl
  dnl PHP_SUBST(PRCTL_SHARED_LIBADD)

  PHP_NEW_EXTENSION(prctl, prctl.c, $ext_shared)
fi
