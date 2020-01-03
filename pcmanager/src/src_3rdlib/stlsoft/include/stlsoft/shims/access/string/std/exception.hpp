/* /////////////////////////////////////////////////////////////////////////
 * File:        stlsoft/shims/access/string/std/exception.hpp
 *
 * Purpose:     Contains the string access shims for std::exception.
 *
 * Created:     2nd May 2003
 * Updated:     10th August 2009
 *
 * Home:        http://stlsoft.org/
 *
 * Copyright (c) 2003-2009, Matthew Wilson and Synesis Software
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * - Neither the name(s) of Matthew Wilson and Synesis Software nor the names of
 *   any contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * ////////////////////////////////////////////////////////////////////// */


/** \file stlsoft/shims/access/string/std/exception.hpp
 *
 * \brief [C++] Definition of the string access shims for
 *   <code>std::exception</code>
 *   (\ref group__concept__shim__string_access "String Access Shims" Concept).
 */

#ifndef STLSOFT_INCL_STLSOFT_SHIMS_ACCESS_STRING_STD_HPP_EXCEPTION
#define STLSOFT_INCL_STLSOFT_SHIMS_ACCESS_STRING_STD_HPP_EXCEPTION

#ifndef STLSOFT_DOCUMENTATION_SKIP_SECTION
# define _STLSOFT_VER_STLSOFT_SHIMS_ACCESS_STRING_STD_HPP_EXCEPTION_MAJOR       2
# define _STLSOFT_VER_STLSOFT_SHIMS_ACCESS_STRING_STD_HPP_EXCEPTION_MINOR       1
# define _STLSOFT_VER_STLSOFT_SHIMS_ACCESS_STRING_STD_HPP_EXCEPTION_REVISION    3
# define _STLSOFT_VER_STLSOFT_SHIMS_ACCESS_STRING_STD_HPP_EXCEPTION_EDIT        35
#endif /* !STLSOFT_DOCUMENTATION_SKIP_SECTION */

/* /////////////////////////////////////////////////////////////////////////
 * Includes
 */

#ifndef STLSOFT_INCL_STLSOFT_H_STLSOFT
# include <stlsoft/stlsoft.h>
#endif /* !STLSOFT_INCL_STLSOFT_H_STLSOFT */
#ifndef STLSOFT_INCL_STLSOFT_SHIMS_ACCESS_STRING_H_FWD
# include <stlsoft/shims/access/string/fwd.h>
#endif /* !STLSOFT_INCL_STLSOFT_SHIMS_ACCESS_STRING_H_FWD */
#ifndef STLSOFT_INCL_STLSOFT_SHIMS_ACCESS_STRING_STD_H_C_STRING
# include <stlsoft/shims/access/string/std/c_string.h>
#endif /* !STLSOFT_INCL_STLSOFT_SHIMS_ACCESS_STRING_STD_H_C_STRING */

#ifndef STLSOFT_INCL_EXCEPTION
# define STLSOFT_INCL_EXCEPTION
#  include <exception>              // for std::exception
#endif /* !STLSOFT_INCL_EXCEPTION */

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#ifndef _STLSOFT_NO_NAMESPACE
namespace stlsoft
{
#endif /* _STLSOFT_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * c_str_data
 *
 * This can be applied to an expression, and the return value is either a
 * pointer to the character string or to an empty string.
 */

/** \brief \ref group__concept__shim__string_access__c_str_data function
 *    for <code>std::exception</code>.
 *
 * \ingroup group__concept__shim__string_access
 *
 * \return None-NULL, non-mutating pointer to a C-style
 *   string of <code>char</code>.
 */
inline ss_char_a_t const* c_str_data_a(stlsoft_ns_qual_std(exception) const& x)
{
    return c_str_data_a(x.what());
}

/** \brief \ref group__concept__shim__string_access__c_str_data function
 *    for <code>std::exception</code>.
 *
 * \ingroup group__concept__shim__string_access
 *
 * \return None-NULL, non-mutating pointer to a C-style string.
 */
inline ss_char_a_t const* c_str_data(stlsoft_ns_qual_std(exception) const& x)
{
    return c_str_data(x);
}

/* /////////////////////////////////////////////////////////////////////////
 * c_str_len
 *
 * This can be applied to an expression, and the return value is the number of
 * characters in the character string in the expression.
 */

/** \brief \ref group__concept__shim__string_access__c_str_len function
 *    for <code>std::exception</code>.
 *
 * \ingroup group__concept__shim__string_access
 *
 * \return Length (in bytes) of the string <code>s</code>.
 */
inline ss_size_t c_str_len_a(stlsoft_ns_qual_std(exception) const& x)
{
    return c_str_len_a(x.what());
}

/** \brief \ref group__concept__shim__string_access__c_str_len function
 *    for <code>std::exception</code>.
 *
 * \ingroup group__concept__shim__string_access
 *
 * \return Length of the string <code>s</code>.
 */
inline ss_size_t c_str_len(stlsoft_ns_qual_std(exception) const& x)
{
    return c_str_len(x);
}

/* /////////////////////////////////////////////////////////////////////////
 * c_str_ptr
 *
 * This can be applied to an expression, and the return value is either a
 * pointer to the character string or to an empty string.
 */

/** \brief \ref group__concept__shim__string_access__c_str_ptr function
 *    for <code>std::exception</code>.
 *
 * \ingroup group__concept__shim__string_access
 *
 * \return None-NULL, non-mutating pointer to a nul-terminated C-style
 *   string of <code>char</code>.
 */
inline ss_char_a_t const* c_str_ptr_a(stlsoft_ns_qual_std(exception) const& x)
{
    return c_str_ptr_a(x.what());
}

/** \brief \ref group__concept__shim__string_access__c_str_ptr function
 *    for <code>std::exception</code>.
 *
 * \ingroup group__concept__shim__string_access
 *
 * \return None-NULL, non-mutating pointer to a nul-terminated C-style
 *   string.
 */
inline ss_char_a_t const* c_str_ptr(stlsoft_ns_qual_std(exception) const& x)
{
    return c_str_ptr(x);
}

/* /////////////////////////////////////////////////////////////////////////
 * c_str_ptr_null
 *
 * This can be applied to an expression, and the return value is either a
 * pointer to the character string or NULL.
 */

/** \brief \ref group__concept__shim__string_access__c_str_ptr_null function
 *    for <code>std::exception</code>.
 *
 * \ingroup group__concept__shim__string_access
 *
 * \return Possibly NULL, non-mutating pointer to a nul-terminated C-style
 *   string of <code>char</code>.
 */
inline ss_char_a_t const* c_str_ptr_null_a(stlsoft_ns_qual_std(exception) const& x)
{
    return c_str_ptr_null_a(x.what());
}

/** \brief \ref group__concept__shim__string_access__c_str_ptr_null function
 *    for <code>std::exception</code>.
 *
 * \ingroup group__concept__shim__string_access
 *
 * \return Possibly NULL, non-mutating pointer to a nul-terminated C-style
 *   string.
 */
inline ss_char_a_t const* c_str_ptr_null(stlsoft_ns_qual_std(exception) const& x)
{
    return c_str_ptr_null(x);
}

/* ////////////////////////////////////////////////////////////////////// */

#ifndef _STLSOFT_NO_NAMESPACE
} // namespace stlsoft
#endif /* _STLSOFT_NO_NAMESPACE */

/* ////////////////////////////////////////////////////////////////////// */

#endif /* !STLSOFT_INCL_STLSOFT_SHIMS_ACCESS_STRING_STD_HPP_EXCEPTION */

/* ///////////////////////////// end of file //////////////////////////// */
