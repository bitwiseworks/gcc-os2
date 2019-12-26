#!/bin/sh

#
# Regenerates all Autotools files using platform-specific GNU Autoconf
# Automake and Libtool. This is necessary on platforms not officially
# supported by GNU Autotools (like OS/2).
#
# See https://gcc.gnu.org/wiki/Regenerating_GCC_Configuration for some hints.
#
# Note that in general the project is not very well organized in this regard.
# It has some contadictions in m4 macro file locations and such (in
# particular, libtool macros live in the root directory and not in ./config
# as all other macros) and there is no clearly defined bootstrap steps.
# This results in some hacks we apply below in order to properly generate
# all necessary files from scratch (they are removed from the repository to
# avoid seeing constant differences with upstream versions because of using
# platform-specific Autoconf tools).
#

#
# 0. Some old scripts still need mkinstalldirs. But since it's deprecated now,
# Automake doesn't copy it by default. Pretend we have it to make it do so.
#

touch mkinstalldirs

#
# 1. Bootstrap libssp first. It will put libtool macros and some tools to
# an expected location (root dir) before they are needed by others.
#

echo "Bootstrapping libssp:"
(cd libssp && autoreconf -fvi) || exit

#
# 2. Bootstrap Automake subprojects.
#

for f in \
  gotools libatomic libbacktrace libcc1 libffi libgfortran libgo libgomp \
  libhsail-rt libitm liboffloadmic liboffloadmic/plugin libphobos libquadmath \
  libsanitizer libsanitizer libstdc++-v3 libvtv lto-plugin zlib
do
  echo "Bootstrapping $f:"
  (cd $f && autoreconf -fvi) || exit
done

#
# 3. Bootstrap Autoconf projects.
#

for f in \
  fixincludes gcc intl libcpp libdecnumber libgcc libiberty \
  libobjc
do
  echo "Bootstrapping $f:"
  case "$f" in
  libiberty)
    # libiberty has a custom aclocal.m4, don't touch it
    (cd $f && autoheader -f && autoconf -f) || exit ;;
  *)
    (cd $f && aclocal -I .. -I ../config && autoheader -f && autoconf -f) || exit ;;
  esac
done


#
# 4. Bootstrap Autoconf projects w/o aclocal and autoheader (including root).
#

for f in \
  gnattools libada .
do
  echo "Bootstrapping $f:"
  (cd $f && autoconf -f) || exit
done

echo "Done."
