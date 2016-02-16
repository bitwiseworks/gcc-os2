// Locale support -*- C++ -*-

// Copyright (C) 2000-2014 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

//
// ISO C++ 14882: 22.1  Locales
//

// Information as gleaned from usr/include/_ctype.h

namespace std _GLIBCXX_VISIBILITY(default)
{
_GLIBCXX_BEGIN_NAMESPACE_VERSION

  /// @brief  Base class for ctype.
  struct ctype_base
  {
    // Non-standard typedefs.
    typedef const int* __to_type;

    // NB: Offsets into ctype<char>::_M_table force a particular size
    // on the mask type. Because of this, we don't use an enum.
    typedef unsigned mask;
    static const mask upper     = __CT_UPPER;
    static const mask lower     = __CT_LOWER;
    static const mask alpha     = __CT_ALPHA;
    static const mask digit     = __CT_DIGIT;
    static const mask xdigit    = __CT_XDIGIT;
    static const mask space     = __CT_SPACE;
    static const mask print     = __CT_PRINT;
    static const mask graph     = __CT_ALPHA | __CT_DIGIT | __CT_PUNCT;
    static const mask cntrl     = __CT_CNTRL;
    static const mask punct     = __CT_PUNCT;
    static const mask alnum     = __CT_ALPHA | __CT_DIGIT;
  };

_GLIBCXX_END_NAMESPACE_VERSION
} // namespace
