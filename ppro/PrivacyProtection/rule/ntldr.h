
#ifndef _NTLDRAPI_
#define _NTLDRAPI_

typedef USHORT WORD;
typedef UCHAR  BYTE;
typedef ULONG  DWORD;

#define IMAGE_SIZEOF_ROM_OPTIONAL_HEADER      56
#define IMAGE_SIZEOF_STD_OPTIONAL_HEADER      28
#define IMAGE_SIZEOF_NT_OPTIONAL32_HEADER    224
#define IMAGE_SIZEOF_NT_OPTIONAL64_HEADER    240

#define IMAGE_NT_OPTIONAL_HDR32_MAGIC      0x10b
#define IMAGE_NT_OPTIONAL_HDR64_MAGIC      0x20b
#define IMAGE_ROM_OPTIONAL_HDR_MAGIC       0x107


// Directory Entries

#define IMAGE_DIRECTORY_ENTRY_EXPORT          0   // Export Directory
#define IMAGE_DIRECTORY_ENTRY_IMPORT          1   // Import Directory
#define IMAGE_DIRECTORY_ENTRY_RESOURCE        2   // Resource Directory
#define IMAGE_DIRECTORY_ENTRY_EXCEPTION       3   // Exception Directory
#define IMAGE_DIRECTORY_ENTRY_SECURITY        4   // Security Directory
#define IMAGE_DIRECTORY_ENTRY_BASERELOC       5   // Base Relocation Table
#define IMAGE_DIRECTORY_ENTRY_DEBUG           6   // Debug Directory
//      IMAGE_DIRECTORY_ENTRY_COPYRIGHT       7   // (X86 usage)
#define IMAGE_DIRECTORY_ENTRY_ARCHITECTURE    7   // Architecture Specific Data
#define IMAGE_DIRECTORY_ENTRY_GLOBALPTR       8   // RVA of GP
#define IMAGE_DIRECTORY_ENTRY_TLS             9   // TLS Directory
#define IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG    10   // Load Configuration Directory
#define IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT   11   // Bound Import Directory in headers
#define IMAGE_DIRECTORY_ENTRY_IAT            12   // Import Address Table
#define IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT   13   // Delay Load Import Descriptors
#define IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR 14   // COM Runtime descriptor

// typedef struct _IMAGE_OPTIONAL_HEADER64 {
// 	WORD        Magic;
// 	BYTE        MajorLinkerVersion;
// 	BYTE        MinorLinkerVersion;
// 	DWORD       SizeOfCode;
// 	DWORD       SizeOfInitializedData;
// 	DWORD       SizeOfUninitializedData;
// 	DWORD       AddressOfEntryPoint;
// 	DWORD       BaseOfCode;
// 	ULONGLONG   ImageBase;
// 	DWORD       SectionAlignment;
// 	DWORD       FileAlignment;
// 	WORD        MajorOperatingSystemVersion;
// 	WORD        MinorOperatingSystemVersion;
// 	WORD        MajorImageVersion;
// 	WORD        MinorImageVersion;
// 	WORD        MajorSubsystemVersion;
// 	WORD        MinorSubsystemVersion;
// 	DWORD       Win32VersionValue;
// 	DWORD       SizeOfImage;
// 	DWORD       SizeOfHeaders;
// 	DWORD       CheckSum;
// 	WORD        Subsystem;
// 	WORD        DllCharacteristics;
// 	ULONGLONG   SizeOfStackReserve;
// 	ULONGLONG   SizeOfStackCommit;
// 	ULONGLONG   SizeOfHeapReserve;
// 	ULONGLONG   SizeOfHeapCommit;
// 	DWORD       LoaderFlags;
// 	DWORD       NumberOfRvaAndSizes;
// 	IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
// } IMAGE_OPTIONAL_HEADER64, *PIMAGE_OPTIONAL_HEADER64;
// 
// typedef struct _IMAGE_NT_HEADERS64 {
// 	DWORD Signature;
// 	IMAGE_FILE_HEADER FileHeader;
// 	IMAGE_OPTIONAL_HEADER64 OptionalHeader;
// } IMAGE_NT_HEADERS64, *PIMAGE_NT_HEADERS64;

#define IMAGE_DOS_SIGNATURE                 0x5A4D      // MZ
#define IMAGE_OS2_SIGNATURE                 0x454E      // NE
#define IMAGE_OS2_SIGNATURE_LE              0x454C      // LE
#define IMAGE_VXD_SIGNATURE                 0x454C      // LE
#define IMAGE_NT_SIGNATURE                  0x00004550  // PE00


typedef IMAGE_NT_HEADERS32                  IMAGE_NT_HEADERS;
typedef PIMAGE_NT_HEADERS32                 PIMAGE_NT_HEADERS;


#define IMAGE_FIRST_SECTION( ntheader ) ((PIMAGE_SECTION_HEADER)        \
	((ULONG_PTR)ntheader +                                              \
	FIELD_OFFSET( IMAGE_NT_HEADERS, OptionalHeader ) +                 \
	((PIMAGE_NT_HEADERS)(ntheader))->FileHeader.SizeOfOptionalHeader   \
	))

typedef PIMAGE_NT_HEADERS32 PIMAGE_NT_HEADERS;
#if _MSC_VER > 1000
#pragma once
#endif

#ifdef __cplusplus
extern "C" {
#endif

//#include "ntddk.h"

#ifndef NTSTATUS
	typedef long NTSTATUS;
#endif

#ifndef NTAPI
#define NTAPI __stdcall
#endif

#define RTL_PAGED_CODE()
#define PAGED_CODE()

#define MM_HIGHEST_USER_ADDRESS ((PVOID)0x7FFEFFFF)
#define MAXULONG                0xFFFFFFFF  // winnt



#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)
#define STATUS_INVALID_PARAMETER         ((NTSTATUS)0xC000000DL)
#define STATUS_INVALID_IMAGE_FORMAT      ((NTSTATUS)0xC000007BL)
#define STATUS_SUCCESS                   ((NTSTATUS)0x00000000L)


	//
	// Private flags for loader data table entries
	//

#define LDRP_STATIC_LINK                0x00000002
#define LDRP_IMAGE_DLL                  0x00000004
#define LDRP_LOAD_IN_PROGRESS           0x00001000
#define LDRP_UNLOAD_IN_PROGRESS         0x00002000
#define LDRP_ENTRY_PROCESSED            0x00004000
#define LDRP_ENTRY_INSERTED             0x00008000
#define LDRP_CURRENT_LOAD               0x00010000
#define LDRP_FAILED_BUILTIN_LOAD        0x00020000
#define LDRP_DONT_CALL_FOR_THREADS      0x00040000
#define LDRP_PROCESS_ATTACH_CALLED      0x00080000
#define LDRP_DEBUG_SYMBOLS_LOADED       0x00100000
#define LDRP_IMAGE_NOT_AT_BASE          0x00200000
#define LDRP_COR_IMAGE                  0x00400000
#define LDRP_COR_OWNS_UNMAP             0x00800000
#define LDRP_SYSTEM_MAPPED              0x01000000
#define LDRP_IMAGE_VERIFYING            0x02000000
#define LDRP_DRIVER_DEPENDENT_DLL       0x04000000
#define LDRP_ENTRY_NATIVE               0x08000000
#define LDRP_REDIRECTED                 0x10000000
#define LDRP_NON_PAGED_DEBUG_INFO       0x20000000
#define LDRP_MM_LOADED                  0x40000000
#define LDRP_COMPAT_DATABASE_PROCESSED  0x80000000

#define DLL_PROCESS_ATTACH   1    // winnt
#define DLL_THREAD_ATTACH    2    // winnt
#define DLL_THREAD_DETACH    3    // winnt
#define DLL_PROCESS_DETACH   0    // winnt
#define DLL_PROCESS_VERIFIER 4    // winnt


#define LDR_RESOURCE_ID_NAME_MASK   ((~(ULONG_PTR)0) << 16) /* lower 16bits clear */
#define LDR_RESOURCE_ID_NAME_MINVAL (( (ULONG_PTR)1) << 16) /* 17th bit set */

	//
	// These are how you currently pass the flag to FindResource.
	//
	// VIEW_TO_DATAFILE and DATAFILE_TO_VIEW are idempotent,
	// so you can covert a datafile to a datafile with VIEW_TO_DATAFILE.
	// Think of better names therefore..
	//
#define LDR_VIEW_TO_DATAFILE(x) ((PVOID)(((ULONG_PTR)(x)) |  (ULONG_PTR)1))
#define LDR_IS_DATAFILE(x)              (((ULONG_PTR)(x)) &  (ULONG_PTR)1)
#define LDR_IS_VIEW(x)                  (!LDR_IS_DATAFILE(x))
#define LDR_DATAFILE_TO_VIEW(x) ((PVOID)(((ULONG_PTR)(x)) & ~(ULONG_PTR)1))

	//
	// These functions work on ULONG, ULONG_PTR, ULONG64, etc.
	// They do not work on pointers.
	//
#define LDR_VIEW_TO_DATAFILE_INTEGER(x) ((x) | 1)
#define LDR_IS_DATAFILE_INTEGER(x)      (((x) & 1) == 1)
#define LDR_IS_VIEW_INTEGER(x)          (((x) & 1) == 0)
#define LDR_DATAFILE_TO_VIEW_INTEGER(x) ((x) - ((x) & 1))

	//
	// Flags to LdrCreateOutOfProcessImage.
	//
	// These first two values must not share any bits, even though this is an enum,
	// because LDR_DLL_MAPPED_AS_UNREFORMATED_IMAGE is actually changed to one of them
	// and then it is treated as bits.
#define LDR_DLL_MAPPED_AS_IMAGE            (0x00000001)
#define LDR_DLL_MAPPED_AS_DATA             (0x00000002)
#define LDR_DLL_MAPPED_AS_UNFORMATED_IMAGE (0x00000003)
#define LDR_DLL_MAPPED_AS_MASK             (0x00000003)

	//
	// These are flags to a function that doesn't yet exist:
	//    LdrpSearchResourceSectionEx and/or LdrpSearchOutOfProcessResourceSection
	//
#define LDRP_FIND_RESOURCE_DATA                 (0x00000000)
#define LDRP_FIND_RESOURCE_DIRECTORY            (0x00000002)

	//
	// Flags to LdrFindResourceEx/LdrpSearchResourceSection/LdrFindOutOfProcessResource.
	//
#define LDR_FIND_RESOURCE_LANGUAGE_CAN_FALLBACK            (0x00000000)
#define LDR_FIND_RESOURCE_LANGUAGE_EXACT                   (0x00000004)
#define LDR_FIND_RESOURCE_LANGUAGE_REDIRECT_VERSION        (0x00000008)

	
	// type, id/name, langid
#define LDR_MAXIMUM_RESOURCE_PATH_DEPTH (3)


#define LDR_IMAGE_FILE_OPTIONS_KEY_LARGE_PAGE_DLLS L"LargePageDLLs"


#define LDR_DLL_LOADED_FLAG_RELOCATED (0x00000001)

#define LDR_DLL_UNLOADED_FLAG_PROCESS_TERMINATION (0x00000001)


#define LDR_DLL_NOTIFICATION_REASON_LOADED (1)
#define LDR_DLL_NOTIFICATION_REASON_UNLOADED (2)

#define LDR_LOCK_LOADER_LOCK_FLAG_RAISE_ON_ERRORS (0x00000001)
#define LDR_LOCK_LOADER_LOCK_FLAG_TRY_ONLY (0x00000002)

#define LDR_LOCK_LOADER_LOCK_DISPOSITION_INVALID (0)
#define LDR_LOCK_LOADER_LOCK_DISPOSITION_LOCK_ACQUIRED (1)
#define LDR_LOCK_LOADER_LOCK_DISPOSITION_LOCK_NOT_ACQUIRED (2)


#define LDR_UNLOCK_LOADER_LOCK_FLAG_RAISE_ON_ERRORS (0x00000001)


#define LDR_GET_DLL_HANDLE_EX_UNCHANGED_REFCOUNT (0x00000001)
#define LDR_GET_DLL_HANDLE_EX_PIN                (0x00000002)



#define LDR_ADDREF_DLL_PIN (0x00000001)


 //-----------------------------------------------------------------

PIMAGE_NT_HEADERS32
NTAPI
RtlImageNtHeader(
				 PVOID Base
				 );

NTSTATUS
NTAPI
RtlImageNtHeaderEx(
				   ULONG Flags,
				   PVOID Base,
				   ULONG64 Size,
				   OUT PIMAGE_NT_HEADERS * OutHeaders
				   ) ;

#ifdef __cplusplus
}
#endif

#endif // _NTLDRAPI_

