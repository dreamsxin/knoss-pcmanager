/* /////////////////////////////////////////////////////////////////////////
 * File:        winstl/filesystem/memory_mapped_file.hpp (based on MMFile.h, ::SynesisWin)
 *
 * Purpose:     Memory mapped file class.
 *
 * Created:     15th December 1996
 * Updated:     7th June 2010
 *
 * Thanks:      To Pablo Aguilar for requesting multibyte / wide string
 *              ambivalence. To Joe Mariadassou for requesting swap().
 *
 * Home:        http://stlsoft.org/
 *
 * Copyright (c) 1996-2010, Matthew Wilson and Synesis Software
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * - Neither the name(s) of Matthew Wilson and Synesis Software nor the
 *   names of any contributors may be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ////////////////////////////////////////////////////////////////////// */


/** \file winstl/filesystem/memory_mapped_file.hpp
 *
 * \brief [C++ only] Definition of the winstl::memory_mapped_file class
 *   (\ref group__library__filesystem "File System" Library).
 */

#ifndef WINSTL_INCL_WINSTL_FILESYSTEM_HPP_MEMORY_MAPPED_FILE
#define WINSTL_INCL_WINSTL_FILESYSTEM_HPP_MEMORY_MAPPED_FILE

#ifndef STLSOFT_DOCUMENTATION_SKIP_SECTION
# define WINSTL_VER_WINSTL_FILESYSTEM_HPP_MEMORY_MAPPED_FILE_MAJOR     4
# define WINSTL_VER_WINSTL_FILESYSTEM_HPP_MEMORY_MAPPED_FILE_MINOR     7
# define WINSTL_VER_WINSTL_FILESYSTEM_HPP_MEMORY_MAPPED_FILE_REVISION  2
# define WINSTL_VER_WINSTL_FILESYSTEM_HPP_MEMORY_MAPPED_FILE_EDIT      91
#endif /* !STLSOFT_DOCUMENTATION_SKIP_SECTION */

/* /////////////////////////////////////////////////////////////////////////
 * Includes
 */

#ifndef WINSTL_INCL_WINSTL_H_WINSTL
# include <winstl/winstl.h>
#endif /* !WINSTL_INCL_WINSTL_H_WINSTL */
#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
# ifndef WINSTL_INCL_WINSTL_ERROR_HPP_WINDOWS_EXCEPTIONS
#  include <winstl/error/exceptions.hpp>
# endif /* !WINSTL_INCL_WINSTL_ERROR_HPP_WINDOWS_EXCEPTIONS */
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */
#ifndef STLSOFT_INCL_STLSOFT_SMARTPTR_HPP_SCOPED_HANDLE
# include <stlsoft/smartptr/scoped_handle.hpp>
#endif /* !STLSOFT_INCL_STLSOFT_SMARTPTR_HPP_SCOPED_HANDLE */
#ifndef STLSOFT_INCL_STLSOFT_SHIMS_ACCESS_STRING_H_FWD
# include <stlsoft/shims/access/string/fwd.h>
#endif /* !STLSOFT_INCL_STLSOFT_SHIMS_ACCESS_STRING_H_FWD */
#ifndef STLSOFT_INCL_STLSOFT_UTIL_HPP_STD_SWAP
# include <stlsoft/util/std_swap.hpp>
#endif /* !STLSOFT_INCL_STLSOFT_UTIL_HPP_STD_SWAP */

#ifdef STLSOFT_UNITTEST
# include <winstl/filesystem/file_path_buffer.hpp>
#endif /* STLSOFT_UNITTEST */

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#ifndef _WINSTL_NO_NAMESPACE
# if defined(_STLSOFT_NO_NAMESPACE) || \
     defined(STLSOFT_DOCUMENTATION_SKIP_SECTION)
/* There is no stlsoft namespace, so must define ::winstl */
namespace winstl
{
# else
/* Define stlsoft::winstl_project */

namespace stlsoft
{

namespace winstl_project
{

# endif /* _STLSOFT_NO_NAMESPACE */
#endif /* !_WINSTL_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * Classes
 */

/** Facade over the Win32 memory mapped file API.
 *
 * \ingroup group__library__filesystem
 */
class memory_mapped_file
{
/// \name Member Types
/// @{
public:
    /// This type
    typedef memory_mapped_file              class_type;
    /// The size type
#ifdef STLSOFT_CF_64BIT_INT_SUPPORT
    typedef ws_uint64_t                     size_type;
#else /* ? STLSOFT_CF_64BIT_INT_SUPPORT */
    typedef ws_uint32_t                     size_type;
#endif /* STLSOFT_CF_64BIT_INT_SUPPORT */
    /// The error type
    typedef ws_dword_t                      error_type;
#ifdef STLSOFT_CF_64BIT_INT_SUPPORT
    /// The offset type
    typedef ws_uint64_t                     offset_type;
#endif /* STLSOFT_CF_64BIT_INT_SUPPORT */
    /// The boolean type
    typedef ws_bool_t                       bool_type;
/// @}

/// \name Implementation
/// @{
private:
#if (   defined(STLSOFT_COMPILER_IS_MSVC) && \
        _MSC_VER < 1200) || \
    defined(STLSOFT_COMPILER_IS_WATCOM)
    static void CloseHandle(HANDLE h)
    {
        ::CloseHandle(h);
    }
#endif /* compiler */

    void open_(
        ws_char_a_t const*  fileName
#ifdef STLSOFT_CF_64BIT_INT_SUPPORT
    ,   offset_type         offset
    ,   ws_uint32_t         requestSize
#endif /* STLSOFT_CF_64BIT_INT_SUPPORT */
    )
    {
        scoped_handle<HANDLE>   hfile(  ::CreateFileA(  fileName
                                                    ,   GENERIC_READ
                                                    ,   FILE_SHARE_READ
                                                    ,   NULL
                                                    ,   OPEN_EXISTING
                                                    ,   FILE_FLAG_RANDOM_ACCESS
                                                    ,   NULL)
                            ,   CloseHandle
                            ,   INVALID_HANDLE_VALUE);


#ifdef STLSOFT_CF_64BIT_INT_SUPPORT
        open_helper_(hfile.get(), offset, requestSize);
#else /* ? STLSOFT_CF_64BIT_INT_SUPPORT */
        open_helper_(hfile.get());
#endif /* STLSOFT_CF_64BIT_INT_SUPPORT */
    }

    void open_(
        ws_char_w_t const*  fileName
#ifdef STLSOFT_CF_64BIT_INT_SUPPORT
    ,   offset_type         offset
    ,   ws_uint32_t         requestSize
#endif /* STLSOFT_CF_64BIT_INT_SUPPORT */
    )
    {
        scoped_handle<HANDLE>   hfile(  ::CreateFileW(  fileName
                                                    ,   GENERIC_READ
                                                    ,   FILE_SHARE_READ
                                                    ,   NULL
                                                    ,   OPEN_EXISTING
                                                    ,   FILE_FLAG_RANDOM_ACCESS
                                                    ,   NULL)
                            ,   CloseHandle
                            ,   INVALID_HANDLE_VALUE);


#ifdef STLSOFT_CF_64BIT_INT_SUPPORT
        open_helper_(hfile.get(), offset, requestSize);
#else /* ? STLSOFT_CF_64BIT_INT_SUPPORT */
        open_helper_(hfile.get());
#endif /* STLSOFT_CF_64BIT_INT_SUPPORT */
    }

    void open_helper_(
        HANDLE      hFile
#ifdef STLSOFT_CF_64BIT_INT_SUPPORT
    ,   offset_type offset
    ,   ws_uint32_t requestSize
#endif /* STLSOFT_CF_64BIT_INT_SUPPORT */
    )
    {
        if(INVALID_HANDLE_VALUE == hFile)
        {
            on_error_("Failed to open file for mapping");
        }
        else
        {
            DWORD   fileSizeHigh;
            DWORD   fileSizeLow =   ::GetFileSize(hFile, &fileSizeHigh);
            DWORD   error       =   ::GetLastError();

            if( INVALID_FILE_SIZE == fileSizeLow &&
                ERROR_SUCCESS != error)
            {
                on_error_("Failed to determine mapped file size", error);
            }
#ifndef STLSOFT_CF_64BIT_INT_SUPPORT
            else if(0 != fileSizeHigh)
            {
                on_error_("Cannot map files with sizes larger than 4GB with compilers that do not support 64-bit integers", ERROR_SUCCESS);
            }
#endif /* !STLSOFT_CF_64BIT_INT_SUPPORT */
            else if(0 == fileSizeHigh &&
                    0 == fileSizeLow)
            {
                // Windows CreateFileMapping() does not support mapping
                // zero-length files, so we catch this condition here
                m_memory    =   NULL;
                m_cb        =   0;
            }
            else
            {
                DWORD   maxSizeHi   =   0;
                DWORD   maxSizeLo   =   0;

                if(0 == requestSize)
                {
#ifdef STLSOFT_CF_64BIT_INT_SUPPORT
                    requestSize =   ws_uint32_t((size_type(fileSizeHigh) << 32) | fileSizeLow);
#else /* ? STLSOFT_CF_64BIT_INT_SUPPORT */
                    requestSize =   fileSizeLow;
#endif /* STLSOFT_CF_64BIT_INT_SUPPORT */
                }                 

#ifdef STLSOFT_CF_64BIT_INT_SUPPORT
                if( /* 0 != offset && \
                     */0 != requestSize)
                {
                    offset_type maxSize     =   offset + requestSize;
                    offset_type actualSize  =   (offset_type(fileSizeHigh) << 32) + fileSizeLow;

                    if(maxSize > actualSize)
                    {
                        WINSTL_ASSERT(actualSize >= offset);

                        requestSize = static_cast<ws_uint32_t>(actualSize - offset);
                        maxSize = 0;
                    }

                    maxSizeHi = static_cast<DWORD>(maxSize >> 32);
                    maxSizeLo = static_cast<DWORD>(maxSize);
                }
#endif /* !STLSOFT_CF_64BIT_INT_SUPPORT */

                if(0 == requestSize)
                {
                    m_memory    =   NULL;
                    m_cb        =   0;
                }
                else
                {
                    scoped_handle<HANDLE>   hmap(
                                                ::CreateFileMappingA(
                                                    hFile
                                                ,   NULL
                                                ,   PAGE_READONLY
                                                ,   maxSizeHi
                                                ,   maxSizeLo
                                                ,   NULL
                                                )
                                            ,   CloseHandle
                                            ,   NULL
                                            );

                    if(hmap.empty())
                    {
                        on_error_("Failed to open file mapping");
                    }
                    else
                    {
                        void* memory = ::MapViewOfFile(
                                            hmap.get()
                                        ,   FILE_MAP_READ
                                        ,   static_cast<ws_uint32_t>(offset >> 32)
                                        ,   static_cast<ws_uint32_t>(offset)
                                        ,   requestSize
                                        );

                        if(NULL == memory)
                        {
                            on_error_("Failed to map view of file");
                        }
                        else
                        {
                            m_memory    =   memory;
                            m_cb        =   requestSize;
                        }
                    }
                }
            }
        }
    }
/// @}

/// \name Construction
/// @{
public:
    /// Maps an entire file into memory
    ///
    /// \param fileName The name of the file to map into memory
    ///
    /// \exception winstl::windows_exception Thrown if the map cannot be
    ///   created. May be any value returned by the Windows API; known
    ///   values include ERROR_NOT_ENOUGH_MEMORY (when the map size is
    ///   too large to fit into memory) and ERROR_INVALID_PARAMETER (when
    ///   the allocated size is too large to be valid 
    ss_explicit_k memory_mapped_file(ws_char_a_t const* fileName)
        : m_cb(0)
        , m_memory(NULL)
    {
        open_(fileName, 0, 0);
    }
    /// Maps an entire file into memory
    ss_explicit_k memory_mapped_file(ws_char_w_t const* fileName)
        : m_cb(0)
        , m_memory(NULL)
    {
        open_(fileName, 0, 0);
    }
    /// Maps an entire file into memory
    template <ss_typename_param_k S>
    ss_explicit_k memory_mapped_file(S const& fileName)
        : m_cb(0)
        , m_memory(NULL)
    {
        open_(stlsoft_ns_qual(c_str_ptr)(fileName), 0, 0);
    }

#ifdef STLSOFT_CF_64BIT_INT_SUPPORT
    /// Maps a portion of a file into memory
    ///
    /// \param fileName The name of the file to map into memory
    /// \param offset The offset into the file where the mapping
    ///   begins. Must be a multiple of the system allocation
    ///   granularity
    /// \param requestSize The size of the portion of the file
    ///   to map into memory. If 0, all (of the remaining portion)
    ///   of the file is loaded
    memory_mapped_file(
        ws_char_a_t const*  fileName
    ,   offset_type         offset
    ,   ws_uint32_t         requestSize
    )
        : m_cb(0)
        , m_memory(NULL)
    {
        open_(fileName, offset, requestSize);
    }
    /// Maps a portion of a file into memory
    ///
    /// \param fileName The name of the file to map into memory
    /// \param offset The offset into the file where the mapping
    ///   begins. Must be a multiple of the system allocation
    ///   granularity
    /// \param requestSize The size of the portion of the file
    ///   to map into memory. If 0, all (of the remaining portion)
    ///   of the file is loaded
    memory_mapped_file(
        ws_char_w_t const*  fileName
    ,   offset_type         offset
    ,   ws_uint32_t         requestSize
    )
        : m_cb(0)
        , m_memory(NULL)
    {
        open_(fileName, offset, requestSize);
    }
    /// Maps a portion of a file into memory
    ///
    /// \param fileName The name of the file to map into memory
    /// \param offset The offset into the file where the mapping
    ///   begins. Must be a multiple of the system allocation
    ///   granularity
    /// \param requestSize The size of the portion of the file
    ///   to map into memory. If 0, all (of the remaining portion)
    ///   of the file is loaded
    template <ss_typename_param_k S>
    memory_mapped_file(
        S const&    fileName
    ,   offset_type offset
    ,   ws_uint32_t requestSize
    )
        : m_cb(0)
        , m_memory(NULL)
    {
        open_(stlsoft_ns_qual(c_str_ptr)(fileName), offset, requestSize);
    }
#endif /* STLSOFT_CF_64BIT_INT_SUPPORT */

    /// Closes the view on the mapped file
    ~memory_mapped_file() stlsoft_throw_0()
    {
        WINSTL_ASSERT(is_valid());

        if(NULL != m_memory)
        {
            ::UnmapViewOfFile(m_memory);
        }
    }

    /// Swaps the state of this instance with another
    void swap(class_type& rhs) stlsoft_throw_0()
    {
        WINSTL_ASSERT(is_valid());

        std_swap(m_cb, rhs.m_cb);
        std_swap(m_memory, rhs.m_memory);
#ifndef STLSOFT_CF_EXCEPTION_SUPPORT
        std_swap(m_lastError, rhs.m_lastError);
#endif /* !STLSOFT_CF_EXCEPTION_SUPPORT */

        WINSTL_ASSERT(is_valid());
    }
/// @}

/// \name Accessors
/// @{
public:
    /// Non-mutating (const) pointer to the start of the mapped region
    void const* memory() const
    {
        return m_memory;
    }
    /// The number of bytes in the mapped region
    size_type size() const
    {
        return m_cb;
    }

#ifndef STLSOFT_CF_EXCEPTION_SUPPORT
    /// The error associated with the attempted file operation
    error_type lastError() const
    {
        return m_lastError;
    }
#endif /* !STLSOFT_CF_EXCEPTION_SUPPORT */
/// @}

/// \name Implementation
/// @{
private:
    void on_error_(
        char const* message
    ,   error_type  error = ::GetLastError()
    )
    {
#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
        // The exception policy is used because VC++ 5 has a cow when it is
        // asked to throw a windows_exception, and this special case is
        // handled by windows_exception_policy
        windows_exception_policy    xp;

        xp(message, error);
#else /* ? STLSOFT_CF_EXCEPTION_SUPPORT */

        STLSOFT_SUPPRESS_UNUSED(message);

        m_lastError = error;
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */
    }

    bool_type is_valid() const
    {
#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
        if((NULL != m_memory) != (0 != m_cb))
        {
            return false;
        }
#endif /* !STLSOFT_CF_EXCEPTION_SUPPORT */

        return true;
    }
/// @}

/// \name Members
/// @{
private:
    size_type   m_cb;
    void*       m_memory;
#ifndef STLSOFT_CF_EXCEPTION_SUPPORT
    error_type  m_lastError;
#endif /* !STLSOFT_CF_EXCEPTION_SUPPORT */
/// @}

/// \name Not to be implemented
/// @{
private:
    memory_mapped_file(class_type const&);
    class_type& operator =(class_type const&);
/// @}
};

/* /////////////////////////////////////////////////////////////////////////
 * Unit-testing
 */

#ifdef STLSOFT_UNITTEST
# include "./unittest/memory_mapped_file_unittest_.h"
#endif /* STLSOFT_UNITTEST */

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#ifndef _WINSTL_NO_NAMESPACE
# if defined(_STLSOFT_NO_NAMESPACE) || \
     defined(STLSOFT_DOCUMENTATION_SKIP_SECTION)
} // namespace winstl
# else
} // namespace winstl_project
} // namespace stlsoft
# endif /* _STLSOFT_NO_NAMESPACE */
#endif /* !_WINSTL_NO_NAMESPACE */

namespace std
{

    inline void swap(
        winstl_ns_qual(memory_mapped_file)& lhs
    ,   winstl_ns_qual(memory_mapped_file)& rhs
    )
    {
        lhs.swap(rhs);
    }

} /* namespace std */

/* ////////////////////////////////////////////////////////////////////// */

#endif /* !WINSTL_INCL_WINSTL_FILESYSTEM_HPP_MEMORY_MAPPED_FILE */

/* ///////////////////////////// end of file //////////////////////////// */
