/*++ BUILD Version: 0097    // Increment this if a change has global effects

Copyright (c) 1990-1997  Microsoft Corporation

Module Name:

    ntifs.h

Abstract:

    This module defines the NT types, constants, and functions that are
    exposed to file system drivers.

Revision History:

--*/

#ifndef _NTIFS_
#define _NTIFS_

#define NT_INCLUDED
#define _NTMSV1_0_
#define _CTYPE_DISABLE_MACROS

#include <excpt.h>
#include <ntdef.h>
#include <ntnls.h>
#include <ntstatus.h>
#include <bugcodes.h>
#include <ntiologc.h>
//
// Kernel Mutex Level Numbers (must be globallly assigned within executive)
// The third token in the name is the sub-component name that defines and
// uses the level number.
//

//
// Used by Vdm for protecting io simulation structures
//

#define MUTEX_LEVEL_VDM_IO                  (ULONG)0x00000001

#define MUTEX_LEVEL_EX_PROFILE              (ULONG)0x00000040

//
// The LANMAN Redirector uses the file system major function, but defines
// it's own mutex levels.  We can do this safely because we know that the
// local filesystem will never call the remote filesystem and vice versa.
//

#define MUTEX_LEVEL_RDR_FILESYS_DATABASE    (ULONG)0x10100000
#define MUTEX_LEVEL_RDR_FILESYS_SECURITY    (ULONG)0x10100001

//
// File System levels.
//

#define MUTEX_LEVEL_FILESYSTEM_RAW_VCB      (ULONG)0x11000006

//
// In the NT STREAMS environment, a mutex is used to serialize open, close
// and Scheduler threads executing in a subsystem-parallelized stack.
//

#define MUTEX_LEVEL_STREAMS_SUBSYS          (ULONG)0x11001001

//
// Mutex level used by LDT support on x86
//

#define MUTEX_LEVEL_PS_LDT                  (ULONG)0x1F000000

//
//  This macro returns TRUE if a flag in a set of flags is on and FALSE
//  otherwise
//

#define FlagOn(Flags,SingleFlag)        ((Flags) & (SingleFlag))

//
// Define types that are not exported.
//

typedef struct _KTHREAD *PKTHREAD, *PRKTHREAD, *PETHREAD;
typedef struct _KPROCESS *PKPROCESS ,*PRKPROCESS, *PEPROCESS;
typedef struct _PEB *PPEB;
typedef struct _KINTERRUPT *PKINTERRUPT;
typedef struct _IO_TIMER *PIO_TIMER;
typedef struct _OBJECT_TYPE *POBJECT_TYPE;
typedef struct _CALLBACK_OBJECT *PCALLBACK_OBJECT;
typedef struct _DEVICE_HANDLER_OBJECT *PDEVICE_HANDLER_OBJECT;
typedef struct _BUS_HANDLER *PBUS_HANDLER;

#if defined(_M_ALPHA)
void *__rdpcr(void);
#pragma intrinsic(__rdpcr)
#define PCR ((PKPCR)__rdpcr())

unsigned char __swpirql(unsigned char);
#pragma intrinsic(__swpirql)

void *__rdthread(void);
#pragma intrinsic(__rdthread)
#define KeGetCurrentThread() ((struct _KTHREAD *) __rdthread())
#endif // defined(_M_ALPHA)

#if defined(_M_IX86)
PKTHREAD NTAPI KeGetCurrentThread();
#endif // defined(_M_IX86)

#if defined(_M_IA64)

//
// Define Address of Processor Control Registers.
//

#define KIPCR ((ULONG_PTR)(KADDRESS_BASE + 0xffff0000))            // kernel address of first PCR

//
// Define Pointer to Processor Control Registers.
//

#define PCR ((volatile KPCR * const)KIPCR)

PKTHREAD NTAPI KeGetCurrentThread();

#endif // defined(_M_IA64)

#define PsGetCurrentProcess() IoGetCurrentProcess()
#define PsGetCurrentThread() ((PETHREAD) (KeGetCurrentThread()))
extern PCCHAR KeNumberProcessors;
//
//  Define an access token from a programmer's viewpoint.  The structure is
//  completely opaque and the programer is only allowed to have pointers
//  to tokens.
//

typedef PVOID PACCESS_TOKEN;            // winnt

//
// Pointer to a SECURITY_DESCRIPTOR  opaque data type.
//

typedef PVOID PSECURITY_DESCRIPTOR;     // winnt

//
// Define a pointer to the Security ID data type (an opaque data type)
//

typedef PVOID PSID;     // winnt

typedef ULONG ACCESS_MASK;
typedef ACCESS_MASK *PACCESS_MASK;

// end_winnt
//
//  The following are masks for the predefined standard access types
//

#define DELETE                           (0x00010000L)
#define READ_CONTROL                     (0x00020000L)
#define WRITE_DAC                        (0x00040000L)
#define WRITE_OWNER                      (0x00080000L)
#define SYNCHRONIZE                      (0x00100000L)

#define STANDARD_RIGHTS_REQUIRED         (0x000F0000L)

#define STANDARD_RIGHTS_READ             (READ_CONTROL)
#define STANDARD_RIGHTS_WRITE            (READ_CONTROL)
#define STANDARD_RIGHTS_EXECUTE          (READ_CONTROL)

#define STANDARD_RIGHTS_ALL              (0x001F0000L)

#define SPECIFIC_RIGHTS_ALL              (0x0000FFFFL)

//
// AccessSystemAcl access type
//

#define ACCESS_SYSTEM_SECURITY           (0x01000000L)

//
// MaximumAllowed access type
//

#define MAXIMUM_ALLOWED                  (0x02000000L)

//
//  These are the generic rights.
//

#define GENERIC_READ                     (0x80000000L)
#define GENERIC_WRITE                    (0x40000000L)
#define GENERIC_EXECUTE                  (0x20000000L)
#define GENERIC_ALL                      (0x10000000L)


//
//  Define the generic mapping array.  This is used to denote the
//  mapping of each generic access right to a specific access mask.
//

typedef struct _GENERIC_MAPPING {
    ACCESS_MASK GenericRead;
    ACCESS_MASK GenericWrite;
    ACCESS_MASK GenericExecute;
    ACCESS_MASK GenericAll;
} GENERIC_MAPPING;
typedef GENERIC_MAPPING *PGENERIC_MAPPING;



////////////////////////////////////////////////////////////////////////
//                                                                    //
//                        LUID_AND_ATTRIBUTES                         //
//                                                                    //
////////////////////////////////////////////////////////////////////////
//
//


#include <pshpack4.h>

typedef struct _LUID_AND_ATTRIBUTES {
    LUID Luid;
    ULONG Attributes;
    } LUID_AND_ATTRIBUTES, * PLUID_AND_ATTRIBUTES;
typedef LUID_AND_ATTRIBUTES LUID_AND_ATTRIBUTES_ARRAY[ANYSIZE_ARRAY];
typedef LUID_AND_ATTRIBUTES_ARRAY *PLUID_AND_ATTRIBUTES_ARRAY;

#include <poppack.h>


#ifndef SID_IDENTIFIER_AUTHORITY_DEFINED
#define SID_IDENTIFIER_AUTHORITY_DEFINED
typedef struct _SID_IDENTIFIER_AUTHORITY {
    UCHAR Value[6];
} SID_IDENTIFIER_AUTHORITY, *PSID_IDENTIFIER_AUTHORITY;
#endif


#ifndef SID_DEFINED
#define SID_DEFINED
typedef struct _SID {
   UCHAR Revision;
   UCHAR SubAuthorityCount;
   SID_IDENTIFIER_AUTHORITY IdentifierAuthority;
#ifdef MIDL_PASS
   [size_is(SubAuthorityCount)] ULONG SubAuthority[*];
#else // MIDL_PASS
   ULONG SubAuthority[ANYSIZE_ARRAY];
#endif // MIDL_PASS
} SID, *PISID;
#endif

#define SID_REVISION                     (1)    // Current revision level
#define SID_MAX_SUB_AUTHORITIES          (15)
#define SID_RECOMMENDED_SUB_AUTHORITIES  (1)    // Will change to around 6
                                                // in a future release.

#ifndef MIDL_PASS
#define SECURITY_MAX_SID_SIZE  \
      (sizeof(SID) - sizeof(ULONG) + (SID_MAX_SUB_AUTHORITIES * sizeof(ULONG)))
#endif // MIDL_PASS

typedef enum _SID_NAME_USE {
    SidTypeUser = 1,
    SidTypeGroup,
    SidTypeDomain,
    SidTypeAlias,
    SidTypeWellKnownGroup,
    SidTypeDeletedAccount,
    SidTypeInvalid,
    SidTypeUnknown,
    SidTypeComputer
} SID_NAME_USE, *PSID_NAME_USE;

typedef struct _SID_AND_ATTRIBUTES {
    PSID Sid;
    ULONG Attributes;
    } SID_AND_ATTRIBUTES, * PSID_AND_ATTRIBUTES;

typedef SID_AND_ATTRIBUTES SID_AND_ATTRIBUTES_ARRAY[ANYSIZE_ARRAY];
typedef SID_AND_ATTRIBUTES_ARRAY *PSID_AND_ATTRIBUTES_ARRAY;



/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// Universal well-known SIDs                                               //
//                                                                         //
//     Null SID                     S-1-0-0                                //
//     World                        S-1-1-0                                //
//     Local                        S-1-2-0                                //
//     Creator Owner ID             S-1-3-0                                //
//     Creator Group ID             S-1-3-1                                //
//     Creator Owner Server ID      S-1-3-2                                //
//     Creator Group Server ID      S-1-3-3                                //
//                                                                         //
//     (Non-unique IDs)             S-1-4                                  //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#define SECURITY_NULL_SID_AUTHORITY       {0,0,0,0,0,0}
#define SECURITY_WORLD_SID_AUTHORITY      {0,0,0,0,0,1}
#define SECURITY_LOCAL_SID_AUTHORITY      {0,0,0,0,0,2}
#define SECURITY_CREATOR_SID_AUTHORITY    {0,0,0,0,0,3}
#define SECURITY_NON_UNIQUE_AUTHORITY     {0,0,0,0,0,4}

#define SECURITY_NULL_RID                 (0x00000000L)
#define SECURITY_WORLD_RID                (0x00000000L)
#define SECURITY_LOCAL_RID                (0x00000000L)

#define SECURITY_CREATOR_OWNER_RID        (0x00000000L)
#define SECURITY_CREATOR_GROUP_RID        (0x00000001L)

#define SECURITY_CREATOR_OWNER_SERVER_RID (0x00000002L)
#define SECURITY_CREATOR_GROUP_SERVER_RID (0x00000003L)


/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// NT well-known SIDs                                                      //
//                                                                         //
//     NT Authority          S-1-5                                         //
//     Dialup                S-1-5-1                                       //
//                                                                         //
//     Network               S-1-5-2                                       //
//     Batch                 S-1-5-3                                       //
//     Interactive           S-1-5-4                                       //
//     Service               S-1-5-6                                       //
//     AnonymousLogon        S-1-5-7       (aka null logon session)        //
//     Proxy                 S-1-5-8                                       //
//     ServerLogon           S-1-5-9       (aka domain controller account) //
//     Self                  S-1-5-10      (self RID)                      //
//     Authenticated User    S-1-5-11      (Authenticated user somewhere)  //
//     Restricted Code       S-1-5-12      (Running restricted code)       //
//     Terminal Server       S-1-5-13      (Running on Terminal Server)    //
//                                                                         //
//     (Logon IDs)           S-1-5-5-X-Y                                   //
//                                                                         //
//     (NT non-unique IDs)   S-1-5-0x15-...                                //
//                                                                         //
//     (Built-in domain)     s-1-5-0x20                                    //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////


#define SECURITY_NT_AUTHORITY           {0,0,0,0,0,5}   // ntifs

#define SECURITY_DIALUP_RID             (0x00000001L)
#define SECURITY_NETWORK_RID            (0x00000002L)
#define SECURITY_BATCH_RID              (0x00000003L)
#define SECURITY_INTERACTIVE_RID        (0x00000004L)
#define SECURITY_SERVICE_RID            (0x00000006L)
#define SECURITY_ANONYMOUS_LOGON_RID    (0x00000007L)
#define SECURITY_PROXY_RID              (0x00000008L)
#define SECURITY_ENTERPRISE_CONTROLLERS_RID (0x00000009L)
#define SECURITY_SERVER_LOGON_RID       SECURITY_ENTERPRISE_CONTROLLERS_RID
#define SECURITY_PRINCIPAL_SELF_RID     (0x0000000AL)
#define SECURITY_AUTHENTICATED_USER_RID (0x0000000BL)
#define SECURITY_RESTRICTED_CODE_RID    (0x0000000CL)
#define SECURITY_TERMINAL_SERVER_RID    (0x0000000DL)
#define SECURITY_REMOTE_LOGON_RID       (0x0000000EL)


#define SECURITY_LOGON_IDS_RID          (0x00000005L)
#define SECURITY_LOGON_IDS_RID_COUNT    (3L)

#define SECURITY_LOCAL_SYSTEM_RID       (0x00000012L)
#define SECURITY_LOCAL_SERVICE_RID      (0x00000013L)
#define SECURITY_NETWORK_SERVICE_RID    (0x00000014L)

#define SECURITY_NT_NON_UNIQUE          (0x00000015L)
#define SECURITY_NT_NON_UNIQUE_SUB_AUTH_COUNT  (3L)

#define SECURITY_BUILTIN_DOMAIN_RID     (0x00000020L)





/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// well-known domain relative sub-authority values (RIDs)...               //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

// Well-known users ...

#define DOMAIN_USER_RID_ADMIN          (0x000001F4L)
#define DOMAIN_USER_RID_GUEST          (0x000001F5L)
#define DOMAIN_USER_RID_KRBTGT         (0x000001F6L)



// well-known groups ...

#define DOMAIN_GROUP_RID_ADMINS        (0x00000200L)
#define DOMAIN_GROUP_RID_USERS         (0x00000201L)
#define DOMAIN_GROUP_RID_GUESTS        (0x00000202L)
#define DOMAIN_GROUP_RID_COMPUTERS     (0x00000203L)
#define DOMAIN_GROUP_RID_CONTROLLERS   (0x00000204L)
#define DOMAIN_GROUP_RID_CERT_ADMINS   (0x00000205L)
#define DOMAIN_GROUP_RID_SCHEMA_ADMINS (0x00000206L)
#define DOMAIN_GROUP_RID_ENTERPRISE_ADMINS (0x00000207L)
#define DOMAIN_GROUP_RID_POLICY_ADMINS (0x00000208L)




// well-known aliases ...

#define DOMAIN_ALIAS_RID_ADMINS        (0x00000220L)
#define DOMAIN_ALIAS_RID_USERS         (0x00000221L)
#define DOMAIN_ALIAS_RID_GUESTS        (0x00000222L)
#define DOMAIN_ALIAS_RID_POWER_USERS   (0x00000223L)

#define DOMAIN_ALIAS_RID_ACCOUNT_OPS   (0x00000224L)
#define DOMAIN_ALIAS_RID_SYSTEM_OPS    (0x00000225L)
#define DOMAIN_ALIAS_RID_PRINT_OPS     (0x00000226L)
#define DOMAIN_ALIAS_RID_BACKUP_OPS    (0x00000227L)

#define DOMAIN_ALIAS_RID_REPLICATOR    (0x00000228L)
#define DOMAIN_ALIAS_RID_RAS_SERVERS   (0x00000229L)
#define DOMAIN_ALIAS_RID_PREW2KCOMPACCESS (0x0000022AL)
#define DOMAIN_ALIAS_RID_REMOTE_DESKTOP_USERS (0x0000022BL)
#define DOMAIN_ALIAS_RID_NETWORK_CONFIGURATION_OPS (0x0000022CL)


//
// Allocate the System Luid.  The first 1000 LUIDs are reserved.
// Use #999 here (0x3E7 = 999)
//

#define SYSTEM_LUID                     { 0x3E7, 0x0 }
#define ANONYMOUS_LOGON_LUID            { 0x3e6, 0x0 }

// This is the *current* ACL revision

#define ACL_REVISION     (2)
#define ACL_REVISION_DS  (4)

// This is the history of ACL revisions.  Add a new one whenever
// ACL_REVISION is updated

#define ACL_REVISION1   (1)
#define MIN_ACL_REVISION ACL_REVISION2
#define ACL_REVISION2   (2)
#define ACL_REVISION3   (3)
#define ACL_REVISION4   (4)
#define MAX_ACL_REVISION ACL_REVISION4

typedef struct _ACL {
    UCHAR AclRevision;
    UCHAR Sbz1;
    USHORT AclSize;
    USHORT AceCount;
    USHORT Sbz2;
} ACL;
typedef ACL *PACL;

// end_ntddk

//
//  The structure of an ACE is a common ace header followed by ace type
//  specific data.  Pictorally the structure of the common ace header is
//  as follows:
//
//       3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//       1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//      +---------------+-------+-------+---------------+---------------+
//      |            AceSize            |    AceFlags   |     AceType   |
//      +---------------+-------+-------+---------------+---------------+
//
//  AceType denotes the type of the ace, there are some predefined ace
//  types
//
//  AceSize is the size, in bytes, of ace.
//
//  AceFlags are the Ace flags for audit and inheritance, defined shortly.

typedef struct _ACE_HEADER {
    UCHAR AceType;
    UCHAR AceFlags;
    USHORT AceSize;
} ACE_HEADER;
typedef ACE_HEADER *PACE_HEADER;

//
//  The following are the predefined ace types that go into the AceType
//  field of an Ace header.
//

#define ACCESS_MIN_MS_ACE_TYPE                  (0x0)
#define ACCESS_ALLOWED_ACE_TYPE                 (0x0)
#define ACCESS_DENIED_ACE_TYPE                  (0x1)
#define SYSTEM_AUDIT_ACE_TYPE                   (0x2)
#define SYSTEM_ALARM_ACE_TYPE                   (0x3)
#define ACCESS_MAX_MS_V2_ACE_TYPE               (0x3)

#define ACCESS_ALLOWED_COMPOUND_ACE_TYPE        (0x4)
#define ACCESS_MAX_MS_V3_ACE_TYPE               (0x4)

#define ACCESS_MIN_MS_OBJECT_ACE_TYPE           (0x5)
#define ACCESS_ALLOWED_OBJECT_ACE_TYPE          (0x5)
#define ACCESS_DENIED_OBJECT_ACE_TYPE           (0x6)
#define SYSTEM_AUDIT_OBJECT_ACE_TYPE            (0x7)
#define SYSTEM_ALARM_OBJECT_ACE_TYPE            (0x8)
#define ACCESS_MAX_MS_OBJECT_ACE_TYPE           (0x8)

#define ACCESS_MAX_MS_V4_ACE_TYPE               (0x8)
#define ACCESS_MAX_MS_ACE_TYPE                  (0x8)
// end_winnt


// begin_winnt

//
//  The following are the inherit flags that go into the AceFlags field
//  of an Ace header.
//

#define OBJECT_INHERIT_ACE                (0x1)
#define CONTAINER_INHERIT_ACE             (0x2)
#define NO_PROPAGATE_INHERIT_ACE          (0x4)
#define INHERIT_ONLY_ACE                  (0x8)
#define INHERITED_ACE                     (0x10)
#define VALID_INHERIT_FLAGS               (0x1F)


//  The following are the currently defined ACE flags that go into the
//  AceFlags field of an ACE header.  Each ACE type has its own set of
//  AceFlags.
//
//  SUCCESSFUL_ACCESS_ACE_FLAG - used only with system audit and alarm ACE
//  types to indicate that a message is generated for successful accesses.
//
//  FAILED_ACCESS_ACE_FLAG - used only with system audit and alarm ACE types
//  to indicate that a message is generated for failed accesses.
//

//
//  SYSTEM_AUDIT and SYSTEM_ALARM AceFlags
//
//  These control the signaling of audit and alarms for success or failure.
//

#define SUCCESSFUL_ACCESS_ACE_FLAG       (0x40)
#define FAILED_ACCESS_ACE_FLAG           (0x80)


//
//  We'll define the structure of the predefined ACE types.  Pictorally
//  the structure of the predefined ACE's is as follows:
//
//       3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//       1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//      +---------------+-------+-------+---------------+---------------+
//      |    AceFlags   | Resd  |Inherit|    AceSize    |     AceType   |
//      +---------------+-------+-------+---------------+---------------+
//      |                              Mask                             |
//      +---------------------------------------------------------------+
//      |                                                               |
//      +                                                               +
//      |                                                               |
//      +                              Sid                              +
//      |                                                               |
//      +                                                               +
//      |                                                               |
//      +---------------------------------------------------------------+
//
//  Mask is the access mask associated with the ACE.  This is either the
//  access allowed, access denied, audit, or alarm mask.
//
//  Sid is the Sid associated with the ACE.
//

//  The following are the four predefined ACE types.

//  Examine the AceType field in the Header to determine
//  which structure is appropriate to use for casting.


typedef struct _ACCESS_ALLOWED_ACE {
    ACE_HEADER Header;
    ACCESS_MASK Mask;
    ULONG SidStart;
} ACCESS_ALLOWED_ACE;

typedef ACCESS_ALLOWED_ACE *PACCESS_ALLOWED_ACE;

typedef struct _ACCESS_DENIED_ACE {
    ACE_HEADER Header;
    ACCESS_MASK Mask;
    ULONG SidStart;
} ACCESS_DENIED_ACE;
typedef ACCESS_DENIED_ACE *PACCESS_DENIED_ACE;

typedef struct _SYSTEM_AUDIT_ACE {
    ACE_HEADER Header;
    ACCESS_MASK Mask;
    ULONG SidStart;
} SYSTEM_AUDIT_ACE;
typedef SYSTEM_AUDIT_ACE *PSYSTEM_AUDIT_ACE;

typedef struct _SYSTEM_ALARM_ACE {
    ACE_HEADER Header;
    ACCESS_MASK Mask;
    ULONG SidStart;
} SYSTEM_ALARM_ACE;
typedef SYSTEM_ALARM_ACE *PSYSTEM_ALARM_ACE;

//
// Current security descriptor revision value
//

#define SECURITY_DESCRIPTOR_REVISION     (1)
#define SECURITY_DESCRIPTOR_REVISION1    (1)

// end_ntddk


#define SECURITY_DESCRIPTOR_MIN_LENGTH   (sizeof(SECURITY_DESCRIPTOR))


typedef USHORT SECURITY_DESCRIPTOR_CONTROL, *PSECURITY_DESCRIPTOR_CONTROL;

#define SE_OWNER_DEFAULTED               (0x0001)
#define SE_GROUP_DEFAULTED               (0x0002)
#define SE_DACL_PRESENT                  (0x0004)
#define SE_DACL_DEFAULTED                (0x0008)
#define SE_SACL_PRESENT                  (0x0010)
#define SE_SACL_DEFAULTED                (0x0020)
// end_winnt
#define SE_DACL_UNTRUSTED                (0x0040)
#define SE_SERVER_SECURITY               (0x0080)
// begin_winnt
#define SE_DACL_AUTO_INHERIT_REQ         (0x0100)
#define SE_SACL_AUTO_INHERIT_REQ         (0x0200)
#define SE_DACL_AUTO_INHERITED           (0x0400)
#define SE_SACL_AUTO_INHERITED           (0x0800)
#define SE_DACL_PROTECTED                (0x1000)
#define SE_SACL_PROTECTED                (0x2000)
#define SE_RM_CONTROL_VALID              (0x4000)
#define SE_SELF_RELATIVE                 (0x8000)

//
//  Where:
//
//      SE_OWNER_DEFAULTED - This boolean flag, when set, indicates that the
//          SID pointed to by the Owner field was provided by a
//          defaulting mechanism rather than explicitly provided by the
//          original provider of the security descriptor.  This may
//          affect the treatment of the SID with respect to inheritence
//          of an owner.
//
//      SE_GROUP_DEFAULTED - This boolean flag, when set, indicates that the
//          SID in the Group field was provided by a defaulting mechanism
//          rather than explicitly provided by the original provider of
//          the security descriptor.  This may affect the treatment of
//          the SID with respect to inheritence of a primary group.
//
//      SE_DACL_PRESENT - This boolean flag, when set, indicates that the
//          security descriptor contains a discretionary ACL.  If this
//          flag is set and the Dacl field of the SECURITY_DESCRIPTOR is
//          null, then a null ACL is explicitly being specified.
//
//      SE_DACL_DEFAULTED - This boolean flag, when set, indicates that the
//          ACL pointed to by the Dacl field was provided by a defaulting
//          mechanism rather than explicitly provided by the original
//          provider of the security descriptor.  This may affect the
//          treatment of the ACL with respect to inheritence of an ACL.
//          This flag is ignored if the DaclPresent flag is not set.
//
//      SE_SACL_PRESENT - This boolean flag, when set,  indicates that the
//          security descriptor contains a system ACL pointed to by the
//          Sacl field.  If this flag is set and the Sacl field of the
//          SECURITY_DESCRIPTOR is null, then an empty (but present)
//          ACL is being specified.
//
//      SE_SACL_DEFAULTED - This boolean flag, when set, indicates that the
//          ACL pointed to by the Sacl field was provided by a defaulting
//          mechanism rather than explicitly provided by the original
//          provider of the security descriptor.  This may affect the
//          treatment of the ACL with respect to inheritence of an ACL.
//          This flag is ignored if the SaclPresent flag is not set.
//
// end_winnt
//      SE_DACL_TRUSTED - This boolean flag, when set, indicates that the
//          ACL pointed to by the Dacl field was provided by a trusted source
//          and does not require any editing of compound ACEs.  If this flag
//          is not set and a compound ACE is encountered, the system will
//          substitute known valid SIDs for the server SIDs in the ACEs.
//
//      SE_SERVER_SECURITY - This boolean flag, when set, indicates that the
//         caller wishes the system to create a Server ACL based on the
//         input ACL, regardess of its source (explicit or defaulting.
//         This is done by replacing all of the GRANT ACEs with compound
//         ACEs granting the current server.  This flag is only
//         meaningful if the subject is impersonating.
//
// begin_winnt
//      SE_SELF_RELATIVE - This boolean flag, when set, indicates that the
//          security descriptor is in self-relative form.  In this form,
//          all fields of the security descriptor are contiguous in memory
//          and all pointer fields are expressed as offsets from the
//          beginning of the security descriptor.  This form is useful
//          for treating security descriptors as opaque data structures
//          for transmission in communication protocol or for storage on
//          secondary media.
//
//
//
// Pictorially the structure of a security descriptor is as follows:
//
//       3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//       1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//      +---------------------------------------------------------------+
//      |            Control            |Reserved1 (SBZ)|   Revision    |
//      +---------------------------------------------------------------+
//      |                            Owner                              |
//      +---------------------------------------------------------------+
//      |                            Group                              |
//      +---------------------------------------------------------------+
//      |                            Sacl                               |
//      +---------------------------------------------------------------+
//      |                            Dacl                               |
//      +---------------------------------------------------------------+
//
// In general, this data structure should be treated opaquely to ensure future
// compatibility.
//
//

typedef struct _SECURITY_DESCRIPTOR_RELATIVE {
    UCHAR Revision;
    UCHAR Sbz1;
    SECURITY_DESCRIPTOR_CONTROL Control;
    ULONG Owner;
    ULONG Group;
    ULONG Sacl;
    ULONG Dacl;
    } SECURITY_DESCRIPTOR_RELATIVE, *PISECURITY_DESCRIPTOR_RELATIVE;

typedef struct _SECURITY_DESCRIPTOR {
   UCHAR Revision;
   UCHAR Sbz1;
   SECURITY_DESCRIPTOR_CONTROL Control;
   PSID Owner;
   PSID Group;
   PACL Sacl;
   PACL Dacl;

   } SECURITY_DESCRIPTOR, *PISECURITY_DESCRIPTOR;

//
// Privilege attributes
//

#define SE_PRIVILEGE_ENABLED_BY_DEFAULT (0x00000001L)
#define SE_PRIVILEGE_ENABLED            (0x00000002L)
#define SE_PRIVILEGE_USED_FOR_ACCESS    (0x80000000L)

//
// Privilege Set Control flags
//

#define PRIVILEGE_SET_ALL_NECESSARY    (1)

//
//  Privilege Set - This is defined for a privilege set of one.
//                  If more than one privilege is needed, then this structure
//                  will need to be allocated with more space.
//
//  Note: don't change this structure without fixing the INITIAL_PRIVILEGE_SET
//  structure (defined in se.h)
//

typedef struct _PRIVILEGE_SET {
    ULONG PrivilegeCount;
    ULONG Control;
    LUID_AND_ATTRIBUTES Privilege[ANYSIZE_ARRAY];
    } PRIVILEGE_SET, * PPRIVILEGE_SET;

//
// These must be converted to LUIDs before use.
//

#define SE_MIN_WELL_KNOWN_PRIVILEGE       (2L)
#define SE_CREATE_TOKEN_PRIVILEGE         (2L)
#define SE_ASSIGNPRIMARYTOKEN_PRIVILEGE   (3L)
#define SE_LOCK_MEMORY_PRIVILEGE          (4L)
#define SE_INCREASE_QUOTA_PRIVILEGE       (5L)

//
// Unsolicited Input is obsolete and unused.
//

#define SE_UNSOLICITED_INPUT_PRIVILEGE    (6L)

#define SE_MACHINE_ACCOUNT_PRIVILEGE      (6L)
#define SE_TCB_PRIVILEGE                  (7L)
#define SE_SECURITY_PRIVILEGE             (8L)
#define SE_TAKE_OWNERSHIP_PRIVILEGE       (9L)
#define SE_LOAD_DRIVER_PRIVILEGE          (10L)
#define SE_SYSTEM_PROFILE_PRIVILEGE       (11L)
#define SE_SYSTEMTIME_PRIVILEGE           (12L)
#define SE_PROF_SINGLE_PROCESS_PRIVILEGE  (13L)
#define SE_INC_BASE_PRIORITY_PRIVILEGE    (14L)
#define SE_CREATE_PAGEFILE_PRIVILEGE      (15L)
#define SE_CREATE_PERMANENT_PRIVILEGE     (16L)
#define SE_BACKUP_PRIVILEGE               (17L)
#define SE_RESTORE_PRIVILEGE              (18L)
#define SE_SHUTDOWN_PRIVILEGE             (19L)
#define SE_DEBUG_PRIVILEGE                (20L)
#define SE_AUDIT_PRIVILEGE                (21L)
#define SE_SYSTEM_ENVIRONMENT_PRIVILEGE   (22L)
#define SE_CHANGE_NOTIFY_PRIVILEGE        (23L)
#define SE_REMOTE_SHUTDOWN_PRIVILEGE      (24L)
#define SE_UNDOCK_PRIVILEGE               (25L)
#define SE_SYNC_AGENT_PRIVILEGE           (26L)
#define SE_ENABLE_DELEGATION_PRIVILEGE    (27L)
#define SE_MAX_WELL_KNOWN_PRIVILEGE       (SE_ENABLE_DELEGATION_PRIVILEGE)

//
// Impersonation Level
//
// Impersonation level is represented by a pair of bits in Windows.
// If a new impersonation level is added or lowest value is changed from
// 0 to something else, fix the Windows CreateFile call.
//

typedef enum _SECURITY_IMPERSONATION_LEVEL {
    SecurityAnonymous,
    SecurityIdentification,
    SecurityImpersonation,
    SecurityDelegation
    } SECURITY_IMPERSONATION_LEVEL, * PSECURITY_IMPERSONATION_LEVEL;

#define SECURITY_MAX_IMPERSONATION_LEVEL SecurityDelegation

#define DEFAULT_IMPERSONATION_LEVEL SecurityImpersonation


////////////////////////////////////////////////////////////////////
//                                                                //
//           Token Object Definitions                             //
//                                                                //
//                                                                //
////////////////////////////////////////////////////////////////////


//
// Token Specific Access Rights.
//

#define TOKEN_ASSIGN_PRIMARY    (0x0001)
#define TOKEN_DUPLICATE         (0x0002)
#define TOKEN_IMPERSONATE       (0x0004)
#define TOKEN_QUERY             (0x0008)
#define TOKEN_QUERY_SOURCE      (0x0010)
#define TOKEN_ADJUST_PRIVILEGES (0x0020)
#define TOKEN_ADJUST_GROUPS     (0x0040)
#define TOKEN_ADJUST_DEFAULT    (0x0080)
#define TOKEN_ADJUST_SESSIONID  (0x0100)

#define TOKEN_ALL_ACCESS_P (STANDARD_RIGHTS_REQUIRED  |\
                          TOKEN_ASSIGN_PRIMARY      |\
                          TOKEN_DUPLICATE           |\
                          TOKEN_IMPERSONATE         |\
                          TOKEN_QUERY               |\
                          TOKEN_QUERY_SOURCE        |\
                          TOKEN_ADJUST_PRIVILEGES   |\
                          TOKEN_ADJUST_GROUPS       |\
                          TOKEN_ADJUST_DEFAULT )

#if ((defined(_WIN32_WINNT) && (_WIN32_WINNT > 0x0400)) || (!defined(_WIN32_WINNT))) 
#define TOKEN_ALL_ACCESS  (TOKEN_ALL_ACCESS_P |\
                          TOKEN_ADJUST_SESSIONID )
#else 
#define TOKEN_ALL_ACCESS  (TOKEN_ALL_ACCESS_P)
#endif 

#define TOKEN_READ       (STANDARD_RIGHTS_READ      |\
                          TOKEN_QUERY)


#define TOKEN_WRITE      (STANDARD_RIGHTS_WRITE     |\
                          TOKEN_ADJUST_PRIVILEGES   |\
                          TOKEN_ADJUST_GROUPS       |\
                          TOKEN_ADJUST_DEFAULT)

#define TOKEN_EXECUTE    (STANDARD_RIGHTS_EXECUTE)


//
//
// Token Types
//

typedef enum _TOKEN_TYPE {
    TokenPrimary = 1,
    TokenImpersonation
    } TOKEN_TYPE;
typedef TOKEN_TYPE *PTOKEN_TYPE;


//
// Token Information Classes.
//


typedef enum _TOKEN_INFORMATION_CLASS {
    TokenUser = 1,
    TokenGroups,
    TokenPrivileges,
    TokenOwner,
    TokenPrimaryGroup,
    TokenDefaultDacl,
    TokenSource,
    TokenType,
    TokenImpersonationLevel,
    TokenStatistics,
    TokenRestrictedSids,
    TokenSessionId
} TOKEN_INFORMATION_CLASS, *PTOKEN_INFORMATION_CLASS;

//
// Token information class structures
//


typedef struct _TOKEN_USER {
    SID_AND_ATTRIBUTES User;
} TOKEN_USER, *PTOKEN_USER;

typedef struct _TOKEN_GROUPS {
    ULONG GroupCount;
    SID_AND_ATTRIBUTES Groups[ANYSIZE_ARRAY];
} TOKEN_GROUPS, *PTOKEN_GROUPS;


typedef struct _TOKEN_PRIVILEGES {
    ULONG PrivilegeCount;
    LUID_AND_ATTRIBUTES Privileges[ANYSIZE_ARRAY];
} TOKEN_PRIVILEGES, *PTOKEN_PRIVILEGES;


typedef struct _TOKEN_OWNER {
    PSID Owner;
} TOKEN_OWNER, *PTOKEN_OWNER;


typedef struct _TOKEN_PRIMARY_GROUP {
    PSID PrimaryGroup;
} TOKEN_PRIMARY_GROUP, *PTOKEN_PRIMARY_GROUP;


typedef struct _TOKEN_DEFAULT_DACL {
    PACL DefaultDacl;
} TOKEN_DEFAULT_DACL, *PTOKEN_DEFAULT_DACL;



#define TOKEN_SOURCE_LENGTH 8

typedef struct _TOKEN_SOURCE {
    CHAR SourceName[TOKEN_SOURCE_LENGTH];
    LUID SourceIdentifier;
} TOKEN_SOURCE, *PTOKEN_SOURCE;


typedef struct _TOKEN_STATISTICS {
    LUID TokenId;
    LUID AuthenticationId;
    LARGE_INTEGER ExpirationTime;
    TOKEN_TYPE TokenType;
    SECURITY_IMPERSONATION_LEVEL ImpersonationLevel;
    ULONG DynamicCharged;
    ULONG DynamicAvailable;
    ULONG GroupCount;
    ULONG PrivilegeCount;
    LUID ModifiedId;
} TOKEN_STATISTICS, *PTOKEN_STATISTICS;



typedef struct _TOKEN_CONTROL {
    LUID TokenId;
    LUID AuthenticationId;
    LUID ModifiedId;
    TOKEN_SOURCE TokenSource;
    } TOKEN_CONTROL, *PTOKEN_CONTROL;

// end_winnt
//
// Security Tracking Mode
//

#define SECURITY_DYNAMIC_TRACKING      (TRUE)
#define SECURITY_STATIC_TRACKING       (FALSE)

typedef BOOLEAN SECURITY_CONTEXT_TRACKING_MODE,
                    * PSECURITY_CONTEXT_TRACKING_MODE;



//
// Quality Of Service
//

typedef struct _SECURITY_QUALITY_OF_SERVICE {
    ULONG Length;
    SECURITY_IMPERSONATION_LEVEL ImpersonationLevel;
    SECURITY_CONTEXT_TRACKING_MODE ContextTrackingMode;
    BOOLEAN EffectiveOnly;
    } SECURITY_QUALITY_OF_SERVICE, * PSECURITY_QUALITY_OF_SERVICE;


//
// Used to represent information related to a thread impersonation
//

typedef struct _SE_IMPERSONATION_STATE {
    PACCESS_TOKEN Token;
    BOOLEAN CopyOnOpen;
    BOOLEAN EffectiveOnly;
    SECURITY_IMPERSONATION_LEVEL Level;
} SE_IMPERSONATION_STATE, *PSE_IMPERSONATION_STATE;


typedef ULONG SECURITY_INFORMATION, *PSECURITY_INFORMATION;

#define OWNER_SECURITY_INFORMATION       (0x00000001L)
#define GROUP_SECURITY_INFORMATION       (0x00000002L)
#define DACL_SECURITY_INFORMATION        (0x00000004L)
#define SACL_SECURITY_INFORMATION        (0x00000008L)

#define PROTECTED_DACL_SECURITY_INFORMATION     (0x80000000L)
#define PROTECTED_SACL_SECURITY_INFORMATION     (0x40000000L)
#define UNPROTECTED_DACL_SECURITY_INFORMATION   (0x20000000L)
#define UNPROTECTED_SACL_SECURITY_INFORMATION   (0x10000000L)


#if defined(_WIN64)

typedef union _SLIST_HEADER {
    ULONGLONG Alignment;
    struct {
        ULONGLONG Depth : 16;
        ULONGLONG Sequence : 8;
        ULONGLONG Next : 40;
    };
} SLIST_HEADER, *PSLIST_HEADER;

#else

typedef union _SLIST_HEADER {
    ULONGLONG Alignment;
    struct {
        SINGLE_LIST_ENTRY Next;
        USHORT Depth;
        USHORT Sequence;
    };
} SLIST_HEADER, *PSLIST_HEADER;

#endif


//
// Define alignment macros to align structure sizes and pointers up and down.
//

#define ALIGN_DOWN(length, type) \
    ((ULONG)(length) & ~(sizeof(type) - 1))

#define ALIGN_UP(length, type) \
    (ALIGN_DOWN(((ULONG)(length) + sizeof(type) - 1), type))

#define ALIGN_DOWN_POINTER(address, type) \
    ((PVOID)((ULONG_PTR)(address) & ~((ULONG_PTR)sizeof(type) - 1)))

#define ALIGN_UP_POINTER(address, type) \
    (ALIGN_DOWN_POINTER(((ULONG_PTR)(address) + sizeof(type) - 1), type))

#define POOL_TAGGING 1

#ifndef DBG
#define DBG 0
#endif

#if DBG
#define IF_DEBUG if (TRUE)
#else
#define IF_DEBUG if (FALSE)
#endif

#if DEVL


extern ULONG NtGlobalFlag;

#define IF_NTOS_DEBUG( FlagName ) \
    if (NtGlobalFlag & (FLG_ ## FlagName))

#else
#define IF_NTOS_DEBUG( FlagName ) if (FALSE)
#endif

//
// Kernel definitions that need to be here for forward reference purposes
//

// begin_ntndis
//
// Processor modes.
//

typedef CCHAR KPROCESSOR_MODE;

typedef enum _MODE {
    KernelMode,
    UserMode,
    MaximumMode
} MODE;

// end_ntndis
//
// APC function types
//

//
// Put in an empty definition for the KAPC so that the
// routines can reference it before it is declared.
//

struct _KAPC;

typedef
VOID
(*PKNORMAL_ROUTINE) (
    IN PVOID NormalContext,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2
    );

typedef
VOID
(*PKKERNEL_ROUTINE) (
    IN struct _KAPC *Apc,
    IN OUT PKNORMAL_ROUTINE *NormalRoutine,
    IN OUT PVOID *NormalContext,
    IN OUT PVOID *SystemArgument1,
    IN OUT PVOID *SystemArgument2
    );

typedef
VOID
(*PKRUNDOWN_ROUTINE) (
    IN struct _KAPC *Apc
    );

typedef
BOOLEAN
(*PKSYNCHRONIZE_ROUTINE) (
    IN PVOID SynchronizeContext
    );

typedef
BOOLEAN
(*PKTRANSFER_ROUTINE) (
    VOID
    );

//
//
// Asynchronous Procedure Call (APC) object
//

typedef struct _KAPC {
    CSHORT Type;
    CSHORT Size;
    ULONG Spare0;
    struct _KTHREAD *Thread;
    LIST_ENTRY ApcListEntry;
    PKKERNEL_ROUTINE KernelRoutine;
    PKRUNDOWN_ROUTINE RundownRoutine;
    PKNORMAL_ROUTINE NormalRoutine;
    PVOID NormalContext;

    //
    // N.B. The following two members MUST be together.
    //

    PVOID SystemArgument1;
    PVOID SystemArgument2;
    CCHAR ApcStateIndex;
    KPROCESSOR_MODE ApcMode;
    BOOLEAN Inserted;
} KAPC, *PKAPC, *RESTRICTED_POINTER PRKAPC;

// begin_ntndis
//
// DPC routine
//

struct _KDPC;

typedef
VOID
(*PKDEFERRED_ROUTINE) (
    IN struct _KDPC *Dpc,
    IN PVOID DeferredContext,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2
    );

//
// Define DPC importance.
//
// LowImportance - Queue DPC at end of target DPC queue.
// MediumImportance - Queue DPC at end of target DPC queue.
// HighImportance - Queue DPC at front of target DPC DPC queue.
//
// If there is currently a DPC active on the target processor, or a DPC
// interrupt has already been requested on the target processor when a
// DPC is queued, then no further action is necessary. The DPC will be
// executed on the target processor when its queue entry is processed.
//
// If there is not a DPC active on the target processor and a DPC interrupt
// has not been requested on the target processor, then the exact treatment
// of the DPC is dependent on whether the host system is a UP system or an
// MP system.
//
// UP system.
//
// If the DPC is of medium or high importance, the current DPC queue depth
// is greater than the maximum target depth, or current DPC request rate is
// less the minimum target rate, then a DPC interrupt is requested on the
// host processor and the DPC will be processed when the interrupt occurs.
// Otherwise, no DPC interupt is requested and the DPC execution will be
// delayed until the DPC queue depth is greater that the target depth or the
// minimum DPC rate is less than the target rate.
//
// MP system.
//
// If the DPC is being queued to another processor and the depth of the DPC
// queue on the target processor is greater than the maximum target depth or
// the DPC is of high importance, then a DPC interrupt is requested on the
// target processor and the DPC will be processed when the interrupt occurs.
// Otherwise, the DPC execution will be delayed on the target processor until
// the DPC queue depth on the target processor is greater that the maximum
// target depth or the minimum DPC rate on the target processor is less than
// the target mimimum rate.
//
// If the DPC is being queued to the current processor and the DPC is not of
// low importance, the current DPC queue depth is greater than the maximum
// target depth, or the minimum DPC rate is less than the minimum target rate,
// then a DPC interrupt is request on the current processor and the DPV will
// be processed whne the interrupt occurs. Otherwise, no DPC interupt is
// requested and the DPC execution will be delayed until the DPC queue depth
// is greater that the target depth or the minimum DPC rate is less than the
// target rate.
//

typedef enum _KDPC_IMPORTANCE {
    LowImportance,
    MediumImportance,
    HighImportance
} KDPC_IMPORTANCE;

//
// Deferred Procedure Call (DPC) object
//

typedef struct _KDPC {
    CSHORT Type;
    UCHAR Number;
    UCHAR Importance;
    LIST_ENTRY DpcListEntry;
    PKDEFERRED_ROUTINE DeferredRoutine;
    PVOID DeferredContext;
    PVOID SystemArgument1;
    PVOID SystemArgument2;
    PULONG_PTR Lock;
} KDPC, *PKDPC, *RESTRICTED_POINTER PRKDPC;

//
// Interprocessor interrupt worker routine function prototype.
//

typedef PVOID PKIPI_CONTEXT;

typedef
VOID
(*PKIPI_WORKER)(
    IN PKIPI_CONTEXT PacketContext,
    IN PVOID Parameter1,
    IN PVOID Parameter2,
    IN PVOID Parameter3
    );

//
// Define interprocessor interrupt performance counters.
//

typedef struct _KIPI_COUNTS {
    ULONG Freeze;
    ULONG Packet;
    ULONG DPC;
    ULONG APC;
    ULONG FlushSingleTb;
    ULONG FlushMultipleTb;
    ULONG FlushEntireTb;
    ULONG GenericCall;
    ULONG ChangeColor;
    ULONG SweepDcache;
    ULONG SweepIcache;
    ULONG SweepIcacheRange;
    ULONG FlushIoBuffers;
    ULONG GratuitousDPC;
} KIPI_COUNTS, *PKIPI_COUNTS;

#if defined(NT_UP)

#define HOT_STATISTIC(a) a

#else

#define HOT_STATISTIC(a) (KeGetCurrentPrcb()->a)

#endif

//
// I/O system definitions.
//
// Define a Memory Descriptor List (MDL)
//
// An MDL describes pages in a virtual buffer in terms of physical pages.  The
// pages associated with the buffer are described in an array that is allocated
// just after the MDL header structure itself.  In a future compiler this will
// be placed at:
//
//      ULONG Pages[];
//
// Until this declaration is permitted, however, one simply calculates the
// base of the array by adding one to the base MDL pointer:
//
//      Pages = (PULONG) (Mdl + 1);
//
// Notice that while in the context of the subject thread, the base virtual
// address of a buffer mapped by an MDL may be referenced using the following:
//
//      Mdl->StartVa | Mdl->ByteOffset
//


typedef struct _MDL {
    struct _MDL *Next;
    CSHORT Size;
    CSHORT MdlFlags;
    struct _EPROCESS *Process;
    PVOID MappedSystemVa;
    PVOID StartVa;
    ULONG ByteCount;
    ULONG ByteOffset;
} MDL, *PMDL;

#define MDL_MAPPED_TO_SYSTEM_VA     0x0001
#define MDL_PAGES_LOCKED            0x0002
#define MDL_SOURCE_IS_NONPAGED_POOL 0x0004
#define MDL_ALLOCATED_FIXED_SIZE    0x0008
#define MDL_PARTIAL                 0x0010
#define MDL_PARTIAL_HAS_BEEN_MAPPED 0x0020
#define MDL_IO_PAGE_READ            0x0040
#define MDL_WRITE_OPERATION         0x0080
#define MDL_PARENT_MAPPED_SYSTEM_VA 0x0100
#define MDL_LOCK_HELD               0x0200
#define MDL_PHYSICAL_VIEW           0x0400
#define MDL_IO_SPACE                0x0800
#define MDL_NETWORK_HEADER          0x1000
#define MDL_MAPPING_CAN_FAIL        0x2000
#define MDL_ALLOCATED_MUST_SUCCEED  0x4000


#define MDL_MAPPING_FLAGS (MDL_MAPPED_TO_SYSTEM_VA     | \
                           MDL_PAGES_LOCKED            | \
                           MDL_SOURCE_IS_NONPAGED_POOL | \
                           MDL_PARTIAL_HAS_BEEN_MAPPED | \
                           MDL_PARENT_MAPPED_SYSTEM_VA | \
                           MDL_LOCK_HELD               | \
                           MDL_SYSTEM_VA               | \
                           MDL_IO_SPACE )

// end_ntndis
//
// switch to DBG when appropriate
//

#if DBG
#define PAGED_CODE() \
    if (KeGetCurrentIrql() > APC_LEVEL) { \
    KdPrint(( "EX: Pageable code called at IRQL %d\n", KeGetCurrentIrql() )); \
        ASSERT(FALSE); \
        }
#else
#define PAGED_CODE()
#endif

//
// Data structure used to represent client security context for a thread.
// This data structure is used to support impersonation.
//
//  THE FIELDS OF THIS DATA STRUCTURE SHOULD BE CONSIDERED OPAQUE
//  BY ALL EXCEPT THE SECURITY ROUTINES.
//

typedef struct _SECURITY_CLIENT_CONTEXT {
    SECURITY_QUALITY_OF_SERVICE SecurityQos;
    PACCESS_TOKEN ClientToken;
    BOOLEAN DirectlyAccessClientToken;
    BOOLEAN DirectAccessEffectiveOnly;
    BOOLEAN ServerIsRemote;
    TOKEN_CONTROL ClientTokenControl;
    } SECURITY_CLIENT_CONTEXT, *PSECURITY_CLIENT_CONTEXT;

//
// Define function decoration depending on whether a driver, a file system,
// or a kernel component is being built.
//
// end_wdm

#if (defined(_NTDRIVER_) || defined(_NTDDK_) || defined(_NTIFS_) || defined(_NTHAL_)) && !defined(_BLDR_)

#define NTKERNELAPI DECLSPEC_IMPORT         // wdm

#else

#define NTKERNELAPI

#endif

//
// Define function decoration depending on whether the HAL or other kernel
// component is being build.
//

#if !defined(_NTHAL_) && !defined(_BLDR_)

#define NTHALAPI DECLSPEC_IMPORT         // wdm

#else

#define NTHALAPI

#endif


#define LOW_PRIORITY 0              // Lowest thread priority level
#define LOW_REALTIME_PRIORITY 16    // Lowest realtime priority level
#define HIGH_PRIORITY 31            // Highest thread priority level
#define MAXIMUM_PRIORITY 32         // Number of thread priority levels
// begin_winnt
#define MAXIMUM_WAIT_OBJECTS 64     // Maximum number of wait objects

#define MAXIMUM_SUSPEND_COUNT MAXCHAR // Maximum times thread can be suspended
// end_winnt

//
// Thread affinity
//

typedef ULONG KAFFINITY;
typedef KAFFINITY *PKAFFINITY;

//
// Thread priority
//

typedef LONG KPRIORITY;

//
// Spin Lock
//

// begin_ntndis begin_winnt

typedef ULONG_PTR KSPIN_LOCK;
typedef KSPIN_LOCK *PKSPIN_LOCK;

// end_ntndis end_winnt

//
// Interrupt routine (first level dispatch)
//

typedef
VOID
(*PKINTERRUPT_ROUTINE) (
    VOID
    );

//
// Profile source types
//
typedef enum _KPROFILE_SOURCE {
    ProfileTime,
    ProfileAlignmentFixup,
    ProfileTotalIssues,
    ProfilePipelineDry,
    ProfileLoadInstructions,
    ProfilePipelineFrozen,
    ProfileBranchInstructions,
    ProfileTotalNonissues,
    ProfileDcacheMisses,
    ProfileIcacheMisses,
    ProfileCacheMisses,
    ProfileBranchMispredictions,
    ProfileStoreInstructions,
    ProfileFpInstructions,
    ProfileIntegerInstructions,
    Profile2Issue,
    Profile3Issue,
    Profile4Issue,
    ProfileSpecialInstructions,
    ProfileTotalCycles,
    ProfileIcacheIssues,
    ProfileDcacheAccesses,
    ProfileMemoryBarrierCycles,
    ProfileLoadLinkedIssues,
    ProfileMaximum
} KPROFILE_SOURCE;


#ifndef _PO_DDK_
#define _PO_DDK_

typedef enum _SYSTEM_POWER_STATE {
    PowerSystemUnspecified = 0,
    PowerSystemWorking,
    PowerSystemSleeping1,
    PowerSystemSleeping2,
    PowerSystemSleeping3,
    PowerSystemHibernate,
    PowerSystemShutdown,
    PowerSystemMaximum
} SYSTEM_POWER_STATE, *PSYSTEM_POWER_STATE;

typedef enum {
    PowerActionNone = 0,
    PowerActionReserved,
    PowerActionSleep,
    PowerActionHibernate,
    PowerActionShutdown,
    PowerActionShutdownReset,
    PowerActionShutdownOff,
    PowerActionWarmEject
} POWER_ACTION, *PPOWER_ACTION;

typedef enum _DEVICE_POWER_STATE {
    PowerDeviceUnspecified = 0,
    PowerDeviceD0,
    PowerDeviceD1,
    PowerDeviceD2,
    PowerDeviceD3,
    PowerDeviceMaximum
} DEVICE_POWER_STATE, *PDEVICE_POWER_STATE;

typedef union _POWER_STATE {
    SYSTEM_POWER_STATE SystemState;
    DEVICE_POWER_STATE DeviceState;
} POWER_STATE, *PPOWER_STATE;

typedef enum _POWER_STATE_TYPE {
    SystemPowerState = 0,
    DevicePowerState
} POWER_STATE_TYPE, *PPOWER_STATE_TYPE;


//
// Generic power related IOCTLs
//

#define IOCTL_QUERY_DEVICE_POWER_STATE  \
        CTL_CODE(FILE_DEVICE_BATTERY, 0x0, METHOD_BUFFERED, FILE_READ_ACCESS)

#define IOCTL_SET_DEVICE_WAKE           \
        CTL_CODE(FILE_DEVICE_BATTERY, 0x1, METHOD_BUFFERED, FILE_WRITE_ACCESS)

#define IOCTL_CANCEL_DEVICE_WAKE        \
        CTL_CODE(FILE_DEVICE_BATTERY, 0x2, METHOD_BUFFERED, FILE_WRITE_ACCESS)


//
// Defines for W32 interfaces
//

// begin_winnt

#define ES_SYSTEM_REQUIRED  ((ULONG)0x00000001)
#define ES_DISPLAY_REQUIRED ((ULONG)0x00000002)
#define ES_USER_PRESENT     ((ULONG)0x00000004)
#define ES_CONTINUOUS       ((ULONG)0x80000000)

typedef ULONG EXECUTION_STATE;

typedef enum {
    LT_DONT_CARE,
    LT_LOWEST_LATENCY
} LATENCY_TIME;


#endif // !_PO_DDK_

//
// Define the various device type values.  Note that values used by Microsoft
// Corporation are in the range 0-32767, and 32768-65535 are reserved for use
// by customers.
//

#define DEVICE_TYPE ULONG

#define FILE_DEVICE_BEEP                0x00000001
#define FILE_DEVICE_CD_ROM              0x00000002
#define FILE_DEVICE_CD_ROM_FILE_SYSTEM  0x00000003
#define FILE_DEVICE_CONTROLLER          0x00000004
#define FILE_DEVICE_DATALINK            0x00000005
#define FILE_DEVICE_DFS                 0x00000006
#define FILE_DEVICE_DISK                0x00000007
#define FILE_DEVICE_DISK_FILE_SYSTEM    0x00000008
#define FILE_DEVICE_FILE_SYSTEM         0x00000009
#define FILE_DEVICE_INPORT_PORT         0x0000000a
#define FILE_DEVICE_KEYBOARD            0x0000000b
#define FILE_DEVICE_MAILSLOT            0x0000000c
#define FILE_DEVICE_MIDI_IN             0x0000000d
#define FILE_DEVICE_MIDI_OUT            0x0000000e
#define FILE_DEVICE_MOUSE               0x0000000f
#define FILE_DEVICE_MULTI_UNC_PROVIDER  0x00000010
#define FILE_DEVICE_NAMED_PIPE          0x00000011
#define FILE_DEVICE_NETWORK             0x00000012
#define FILE_DEVICE_NETWORK_BROWSER     0x00000013
#define FILE_DEVICE_NETWORK_FILE_SYSTEM 0x00000014
#define FILE_DEVICE_NULL                0x00000015
#define FILE_DEVICE_PARALLEL_PORT       0x00000016
#define FILE_DEVICE_PHYSICAL_NETCARD    0x00000017
#define FILE_DEVICE_PRINTER             0x00000018
#define FILE_DEVICE_SCANNER             0x00000019
#define FILE_DEVICE_SERIAL_MOUSE_PORT   0x0000001a
#define FILE_DEVICE_SERIAL_PORT         0x0000001b
#define FILE_DEVICE_SCREEN              0x0000001c
#define FILE_DEVICE_SOUND               0x0000001d
#define FILE_DEVICE_STREAMS             0x0000001e
#define FILE_DEVICE_TAPE                0x0000001f
#define FILE_DEVICE_TAPE_FILE_SYSTEM    0x00000020
#define FILE_DEVICE_TRANSPORT           0x00000021
#define FILE_DEVICE_UNKNOWN             0x00000022
#define FILE_DEVICE_VIDEO               0x00000023
#define FILE_DEVICE_VIRTUAL_DISK        0x00000024
#define FILE_DEVICE_WAVE_IN             0x00000025
#define FILE_DEVICE_WAVE_OUT            0x00000026
#define FILE_DEVICE_8042_PORT           0x00000027
#define FILE_DEVICE_NETWORK_REDIRECTOR  0x00000028
#define FILE_DEVICE_BATTERY             0x00000029
#define FILE_DEVICE_BUS_EXTENDER        0x0000002a
#define FILE_DEVICE_MODEM               0x0000002b
#define FILE_DEVICE_VDM                 0x0000002c
#define FILE_DEVICE_MASS_STORAGE        0x0000002d
#define FILE_DEVICE_SMB                 0x0000002e
#define FILE_DEVICE_KS                  0x0000002f
#define FILE_DEVICE_CHANGER             0x00000030
#define FILE_DEVICE_SMARTCARD           0x00000031
#define FILE_DEVICE_ACPI                0x00000032
#define FILE_DEVICE_DVD                 0x00000033
#define FILE_DEVICE_FULLSCREEN_VIDEO    0x00000034
#define FILE_DEVICE_DFS_FILE_SYSTEM     0x00000035
#define FILE_DEVICE_DFS_VOLUME          0x00000036
#define FILE_DEVICE_SERENUM             0x00000037
#define FILE_DEVICE_TERMSRV             0x00000038
#define FILE_DEVICE_KSEC                0x00000039

//
// Macro definition for defining IOCTL and FSCTL function control codes.  Note
// that function codes 0-2047 are reserved for Microsoft Corporation, and
// 2048-4095 are reserved for customers.
//

#define CTL_CODE( DeviceType, Function, Method, Access ) (                 \
    ((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method) \
)

//
// Macro to extract device type out of the device io control code
//
#define DEVICE_TYPE_FROM_CTL_CODE(ctrlCode)     (((ULONG)(ctrlCode & 0xffff0000)) >> 16)

//
// Define the method codes for how buffers are passed for I/O and FS controls
//

#define METHOD_BUFFERED                 0
#define METHOD_IN_DIRECT                1
#define METHOD_OUT_DIRECT               2
#define METHOD_NEITHER                  3

//
// Define the access check value for any access
//
//
// The FILE_READ_ACCESS and FILE_WRITE_ACCESS constants are also defined in
// ntioapi.h as FILE_READ_DATA and FILE_WRITE_DATA. The values for these
// constants *MUST* always be in sync.
//
//
// FILE_SPECIAL_ACCESS is checked by the NT I/O system the same as FILE_ANY_ACCESS.
// The file systems, however, may add additional access checks for I/O and FS controls
// that use this value.
//


#define FILE_ANY_ACCESS                 0
#define FILE_SPECIAL_ACCESS    (FILE_ANY_ACCESS)
#define FILE_READ_ACCESS          ( 0x0001 )    // file & pipe
#define FILE_WRITE_ACCESS         ( 0x0002 )    // file & pipe



//
// Security operation mode of the system is held in a control
// longword.
//

typedef ULONG  LSA_OPERATIONAL_MODE, *PLSA_OPERATIONAL_MODE;

//
// Used by a logon process to indicate what type of logon is being
// requested.
//

typedef enum _SECURITY_LOGON_TYPE {
    Interactive = 2,    // Interactively logged on (locally or remotely)
    Network,            // Accessing system via network
    Batch,              // Started via a batch queue
    Service,            // Service started by service controller
    Proxy,              // Proxy logon
    Unlock,             // Unlock workstation
    NetworkCleartext,   // Network logon with cleartext credentials
    NewCredentials      // Clone caller, new default credentials
} SECURITY_LOGON_TYPE, *PSECURITY_LOGON_TYPE;

typedef UNICODE_STRING LSA_UNICODE_STRING, *PLSA_UNICODE_STRING;
typedef STRING LSA_STRING, *PLSA_STRING;
typedef OBJECT_ATTRIBUTES LSA_OBJECT_ATTRIBUTES, *PLSA_OBJECT_ATTRIBUTES;

NTSTATUS
NTAPI
LsaRegisterLogonProcess (
    IN PLSA_STRING LogonProcessName,
    OUT PHANDLE LsaHandle,
    OUT PLSA_OPERATIONAL_MODE SecurityMode
    );


NTSTATUS
NTAPI
LsaFreeReturnBuffer (
    IN PVOID Buffer
    );


#ifndef _NTLSA_IFS_
#define _NTLSA_IFS_
#endif

/////////////////////////////////////////////////////////////////////////
//                                                                     //
// Name of the MSV1_0 authentication package                           //
//                                                                     //
/////////////////////////////////////////////////////////////////////////

#define MSV1_0_PACKAGE_NAME     "MICROSOFT_AUTHENTICATION_PACKAGE_V1_0"
#define MSV1_0_PACKAGE_NAMEW    L"MICROSOFT_AUTHENTICATION_PACKAGE_V1_0"
#define MSV1_0_PACKAGE_NAMEW_LENGTH sizeof(MSV1_0_PACKAGE_NAMEW) - sizeof(WCHAR)

//
// Location of MSV authentication package data
//
#define MSV1_0_SUBAUTHENTICATION_KEY "SYSTEM\\CurrentControlSet\\Control\\Lsa\\MSV1_0"
#define MSV1_0_SUBAUTHENTICATION_VALUE "Auth"


/////////////////////////////////////////////////////////////////////////
//                                                                     //
// Widely used MSV1_0 data types                                       //
//                                                                     //
/////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//       LOGON      Related Data Structures
//
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

//
// When a LsaLogonUser() call is dispatched to the MsV1_0 authentication
// package, the beginning of the AuthenticationInformation buffer is
// cast to a MSV1_0_LOGON_SUBMIT_TYPE to determine the type of logon
// being requested.  Similarly, upon return, the type of profile buffer
// can be determined by typecasting it to a MSV_1_0_PROFILE_BUFFER_TYPE.
//

//
//  MSV1.0 LsaLogonUser() submission message types.
//

typedef enum _MSV1_0_LOGON_SUBMIT_TYPE {
    MsV1_0InteractiveLogon = 2,
    MsV1_0Lm20Logon,
    MsV1_0NetworkLogon,
    MsV1_0SubAuthLogon,
    MsV1_0WorkstationUnlockLogon = 7
} MSV1_0_LOGON_SUBMIT_TYPE, *PMSV1_0_LOGON_SUBMIT_TYPE;


//
//  MSV1.0 LsaLogonUser() profile buffer types.
//

typedef enum _MSV1_0_PROFILE_BUFFER_TYPE {
    MsV1_0InteractiveProfile = 2,
    MsV1_0Lm20LogonProfile,
    MsV1_0SmartCardProfile
} MSV1_0_PROFILE_BUFFER_TYPE, *PMSV1_0_PROFILE_BUFFER_TYPE;






//
// MsV1_0InteractiveLogon
//
// The AuthenticationInformation buffer of an LsaLogonUser() call to
// perform an interactive logon contains the following data structure:
//

typedef struct _MSV1_0_INTERACTIVE_LOGON {
    MSV1_0_LOGON_SUBMIT_TYPE MessageType;
    UNICODE_STRING LogonDomainName;
    UNICODE_STRING UserName;
    UNICODE_STRING Password;
} MSV1_0_INTERACTIVE_LOGON, *PMSV1_0_INTERACTIVE_LOGON;

//
// Where:
//
//     MessageType - Contains the type of logon being requested.  This
//         field must be set to MsV1_0InteractiveLogon.
//
//     UserName - Is a string representing the user's account name.  The
//         name may be up to 255 characters long.  The name is treated case
//         insensitive.
//
//     Password - Is a string containing the user's cleartext password.
//         The password may be up to 255 characters long and contain any
//         UNICODE value.
//
//


//
// The ProfileBuffer returned upon a successful logon of this type
// contains the following data structure:
//

typedef struct _MSV1_0_INTERACTIVE_PROFILE {
    MSV1_0_PROFILE_BUFFER_TYPE MessageType;
    USHORT LogonCount;
    USHORT BadPasswordCount;
    LARGE_INTEGER LogonTime;
    LARGE_INTEGER LogoffTime;
    LARGE_INTEGER KickOffTime;
    LARGE_INTEGER PasswordLastSet;
    LARGE_INTEGER PasswordCanChange;
    LARGE_INTEGER PasswordMustChange;
    UNICODE_STRING LogonScript;
    UNICODE_STRING HomeDirectory;
    UNICODE_STRING FullName;
    UNICODE_STRING ProfilePath;
    UNICODE_STRING HomeDirectoryDrive;
    UNICODE_STRING LogonServer;
    ULONG UserFlags;
} MSV1_0_INTERACTIVE_PROFILE, *PMSV1_0_INTERACTIVE_PROFILE;

//
// where:
//
//     MessageType - Identifies the type of profile data being returned.
//         Contains the type of logon being requested.  This field must
//         be set to MsV1_0InteractiveProfile.
//
//     LogonCount - Number of times the user is currently logged on.
//
//     BadPasswordCount - Number of times a bad password was applied to
//         the account since last successful logon.
//
//     LogonTime - Time when user last logged on.  This is an absolute
//         format NT standard time value.
//
//     LogoffTime - Time when user should log off.  This is an absolute
//         format NT standard time value.
//
//     KickOffTime - Time when system should force user logoff.  This is
//         an absolute format NT standard time value.
//
//     PasswordLastChanged - Time and date the password was last
//         changed.  This is an absolute format NT standard time
//         value.
//
//     PasswordCanChange - Time and date when the user can change the
//         password.  This is an absolute format NT time value.  To
//         prevent a password from ever changing, set this field to a
//         date very far into the future.
//
//     PasswordMustChange - Time and date when the user must change the
//         password.  If the user can never change the password, this
//         field is undefined.  This is an absolute format NT time
//         value.
//
//     LogonScript - The (relative) path to the account's logon
//         script.
//
//     HomeDirectory - The home directory for the user.
//


//
// MsV1_0Lm20Logon and MsV1_0NetworkLogon
//
// The AuthenticationInformation buffer of an LsaLogonUser() call to
// perform an network logon contains the following data structure:
//
// MsV1_0NetworkLogon logon differs from MsV1_0Lm20Logon in that the
// ParameterControl field exists.
//

#define MSV1_0_CHALLENGE_LENGTH 8
#define MSV1_0_USER_SESSION_KEY_LENGTH 16
#define MSV1_0_LANMAN_SESSION_KEY_LENGTH 8



//
// Values for ParameterControl.
//

#define MSV1_0_CLEARTEXT_PASSWORD_ALLOWED    0x02
#define MSV1_0_UPDATE_LOGON_STATISTICS       0x04
#define MSV1_0_RETURN_USER_PARAMETERS        0x08
#define MSV1_0_DONT_TRY_GUEST_ACCOUNT        0x10
#define MSV1_0_ALLOW_SERVER_TRUST_ACCOUNT    0x20
#define MSV1_0_RETURN_PASSWORD_EXPIRY        0x40
// this next flag says that CaseInsensitiveChallengeResponse
//  (aka LmResponse) contains a client challenge in the first 8 bytes
#define MSV1_0_USE_CLIENT_CHALLENGE          0x80
#define MSV1_0_TRY_GUEST_ACCOUNT_ONLY        0x100
#define MSV1_0_RETURN_PROFILE_PATH           0x200
#define MSV1_0_TRY_SPECIFIED_DOMAIN_ONLY     0x400
#define MSV1_0_ALLOW_WORKSTATION_TRUST_ACCOUNT 0x800
#define MSV1_0_SUBAUTHENTICATION_DLL_EX      0x00100000

//
// The high order byte is a value indicating the SubAuthentication DLL.
//  Zero indicates no SubAuthentication DLL.
//
#define MSV1_0_SUBAUTHENTICATION_DLL         0xFF000000
#define MSV1_0_SUBAUTHENTICATION_DLL_SHIFT   24
#define MSV1_0_MNS_LOGON                     0x01000000

//
// This is the list of subauthentication dlls used in MS
//

#define MSV1_0_SUBAUTHENTICATION_DLL_RAS     2
#define MSV1_0_SUBAUTHENTICATION_DLL_IIS     132

typedef struct _MSV1_0_LM20_LOGON {
    MSV1_0_LOGON_SUBMIT_TYPE MessageType;
    UNICODE_STRING LogonDomainName;
    UNICODE_STRING UserName;
    UNICODE_STRING Workstation;
    UCHAR ChallengeToClient[MSV1_0_CHALLENGE_LENGTH];
    STRING CaseSensitiveChallengeResponse;
    STRING CaseInsensitiveChallengeResponse;
    ULONG ParameterControl;
} MSV1_0_LM20_LOGON, * PMSV1_0_LM20_LOGON;



//
// NT 5.0 SubAuth dlls can use this struct
//

typedef struct _MSV1_0_SUBAUTH_LOGON{
    MSV1_0_LOGON_SUBMIT_TYPE MessageType;
    UNICODE_STRING LogonDomainName;
    UNICODE_STRING UserName;
    UNICODE_STRING Workstation;
    UCHAR ChallengeToClient[MSV1_0_CHALLENGE_LENGTH];
    STRING AuthenticationInfo1;
    STRING AuthenticationInfo2;
    ULONG ParameterControl;
    ULONG SubAuthPackageId;
} MSV1_0_SUBAUTH_LOGON, * PMSV1_0_SUBAUTH_LOGON;


//
// Values for UserFlags.
//

#define LOGON_GUEST                 0x01
#define LOGON_NOENCRYPTION          0x02
#define LOGON_CACHED_ACCOUNT        0x04
#define LOGON_USED_LM_PASSWORD      0x08
#define LOGON_EXTRA_SIDS            0x20
#define LOGON_SUBAUTH_SESSION_KEY   0x40
#define LOGON_SERVER_TRUST_ACCOUNT  0x80
#define LOGON_NTLMV2_ENABLED        0x100       // says DC understands NTLMv2
#define LOGON_RESOURCE_GROUPS       0x200
#define LOGON_PROFILE_PATH_RETURNED 0x400

//
// The high order byte is reserved for return by SubAuthentication DLLs.
//

#define MSV1_0_SUBAUTHENTICATION_FLAGS 0xFF000000

// Values returned by the MSV1_0_MNS_LOGON SubAuthentication DLL
#define LOGON_GRACE_LOGON              0x01000000

typedef struct _MSV1_0_LM20_LOGON_PROFILE {
    MSV1_0_PROFILE_BUFFER_TYPE MessageType;
    LARGE_INTEGER KickOffTime;
    LARGE_INTEGER LogoffTime;
    ULONG UserFlags;
    UCHAR UserSessionKey[MSV1_0_USER_SESSION_KEY_LENGTH];
    UNICODE_STRING LogonDomainName;
    UCHAR LanmanSessionKey[MSV1_0_LANMAN_SESSION_KEY_LENGTH];
    UNICODE_STRING LogonServer;
    UNICODE_STRING UserParameters;
} MSV1_0_LM20_LOGON_PROFILE, * PMSV1_0_LM20_LOGON_PROFILE;


//
// Supplemental credentials structure used for passing credentials into
// MSV1_0 from other packages
//

#define MSV1_0_OWF_PASSWORD_LENGTH 16
#define MSV1_0_CRED_LM_PRESENT 0x1
#define MSV1_0_CRED_NT_PRESENT 0x2
#define MSV1_0_CRED_VERSION 0

typedef struct _MSV1_0_SUPPLEMENTAL_CREDENTIAL {
    ULONG Version;
    ULONG Flags;
    UCHAR LmPassword[MSV1_0_OWF_PASSWORD_LENGTH];
    UCHAR NtPassword[MSV1_0_OWF_PASSWORD_LENGTH];
} MSV1_0_SUPPLEMENTAL_CREDENTIAL, *PMSV1_0_SUPPLEMENTAL_CREDENTIAL;


//
// NTLM3 definitions.
//

#define MSV1_0_NTLM3_RESPONSE_LENGTH 16
#define MSV1_0_NTLM3_OWF_LENGTH 16

//
// this is the longest amount of time we'll allow challenge response
// pairs to be used. Note that this also has to allow for worst case clock skew
//
#define MSV1_0_MAX_NTLM3_LIFE 1800     // 30 minutes (in seconds)
#define MSV1_0_MAX_AVL_SIZE 64000

// this is an MSV1_0 private data structure, defining the layout of an NTLM3 response, as sent by a
//  client in the NtChallengeResponse field of the NETLOGON_NETWORK_INFO structure. If can be differentiated
//  from an old style NT response by its length. This is crude, but it needs to pass through servers and
//  the servers' DCs that do not understand NTLM3 but that are willing to pass longer responses.
typedef struct _MSV1_0_NTLM3_RESPONSE {
    UCHAR Response[MSV1_0_NTLM3_RESPONSE_LENGTH]; // hash of OWF of password with all the following fields
    UCHAR RespType;     // id number of response; current is 1
    UCHAR HiRespType;   // highest id number understood by client
    USHORT Flags;       // reserved; must be sent as zero at this version
    ULONG MsgWord;      // 32 bit message from client to server (for use by auth protocol)
    ULONGLONG TimeStamp;    // time stamp when client generated response -- NT system time, quad part
    UCHAR ChallengeFromClient[MSV1_0_CHALLENGE_LENGTH];
    ULONG AvPairsOff;   // offset to start of AvPairs (to allow future expansion)
    UCHAR Buffer[1];    // start of buffer with AV pairs (or future stuff -- so use the offset)
} MSV1_0_NTLM3_RESPONSE, *PMSV1_0_NTLM3_RESPONSE;

#define MSV1_0_NTLM3_INPUT_LENGTH (sizeof(MSV1_0_NTLM3_RESPONSE) - MSV1_0_NTLM3_RESPONSE_LENGTH)

typedef enum {
    MsvAvEOL,                 // end of list
    MsvAvNbComputerName,      // server's computer name -- NetBIOS
    MsvAvNbDomainName,        // server's domain name -- NetBIOS
    MsvAvDnsComputerName,     // server's computer name -- DNS
    MsvAvDnsDomainName        // server's domain name -- DNS
} MSV1_0_AVID;

typedef struct  _MSV1_0_AV_PAIR {
    USHORT AvId;
    USHORT AvLen;
    // Data is treated as byte array following structure
} MSV1_0_AV_PAIR, *PMSV1_0_AV_PAIR;



///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//       CALL PACKAGE Related Data Structures                                //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


//
//  MSV1.0 LsaCallAuthenticationPackage() submission and response
//  message types.
//

typedef enum _MSV1_0_PROTOCOL_MESSAGE_TYPE {
    MsV1_0Lm20ChallengeRequest = 0,          // Both submission and response
    MsV1_0Lm20GetChallengeResponse,          // Both submission and response
    MsV1_0EnumerateUsers,                    // Both submission and response
    MsV1_0GetUserInfo,                       // Both submission and response
    MsV1_0ReLogonUsers,                      // Submission only
    MsV1_0ChangePassword,                    // Both submission and response
    MsV1_0ChangeCachedPassword,              // Both submission and response
    MsV1_0GenericPassthrough,                // Both submission and response
    MsV1_0CacheLogon,                        // Submission only, no response
    MsV1_0SubAuth,                           // Both submission and response
    MsV1_0DeriveCredential,                  // Both submission and response
    MsV1_0CacheLookup                        // Both submission and response
} MSV1_0_PROTOCOL_MESSAGE_TYPE, *PMSV1_0_PROTOCOL_MESSAGE_TYPE;

// end_ntsecapi

//
// MsV1_0Lm20ChallengeRequest submit buffer and response
//

typedef struct _MSV1_0_LM20_CHALLENGE_REQUEST {
    MSV1_0_PROTOCOL_MESSAGE_TYPE MessageType;
} MSV1_0_LM20_CHALLENGE_REQUEST, *PMSV1_0_LM20_CHALLENGE_REQUEST;

typedef struct _MSV1_0_LM20_CHALLENGE_RESPONSE {
    MSV1_0_PROTOCOL_MESSAGE_TYPE MessageType;
    UCHAR ChallengeToClient[MSV1_0_CHALLENGE_LENGTH];
} MSV1_0_LM20_CHALLENGE_RESPONSE, *PMSV1_0_LM20_CHALLENGE_RESPONSE;

//
// MsV1_0Lm20GetChallengeResponse submit buffer and response
//

#define USE_PRIMARY_PASSWORD            0x01
#define RETURN_PRIMARY_USERNAME         0x02
#define RETURN_PRIMARY_LOGON_DOMAINNAME 0x04
#define RETURN_NON_NT_USER_SESSION_KEY  0x08
#define GENERATE_CLIENT_CHALLENGE       0x10
#define GCR_NTLM3_PARMS                 0x20
#define GCR_TARGET_INFO                 0x40    // ServerName field contains target info AV pairs
#define RETURN_RESERVED_PARAMETER       0x80    // was 0x10

//
// version 1 of the GETCHALLENRESP structure, which was used by RAS and others.
// compiled before the additional fields added to GETCHALLENRESP_REQUEST.
// here to allow sizing operations for backwards compatibility.
//

typedef struct _MSV1_0_GETCHALLENRESP_REQUEST_V1 {
    MSV1_0_PROTOCOL_MESSAGE_TYPE MessageType;
    ULONG ParameterControl;
    LUID LogonId;
    UNICODE_STRING Password;
    UCHAR ChallengeToClient[MSV1_0_CHALLENGE_LENGTH];
} MSV1_0_GETCHALLENRESP_REQUEST_V1, *PMSV1_0_GETCHALLENRESP_REQUEST_V1;

typedef struct _MSV1_0_GETCHALLENRESP_REQUEST {
    MSV1_0_PROTOCOL_MESSAGE_TYPE MessageType;
    ULONG ParameterControl;
    LUID LogonId;
    UNICODE_STRING Password;
    UCHAR ChallengeToClient[MSV1_0_CHALLENGE_LENGTH];

    //
    // the following 3 fields are only present if GCR_NTLM3_PARMS is set in ParameterControl
    //

    UNICODE_STRING UserName;
    UNICODE_STRING LogonDomainName;
    UNICODE_STRING ServerName;      // server domain or target info AV pairs
} MSV1_0_GETCHALLENRESP_REQUEST, *PMSV1_0_GETCHALLENRESP_REQUEST;

typedef struct _MSV1_0_GETCHALLENRESP_RESPONSE {
    MSV1_0_PROTOCOL_MESSAGE_TYPE MessageType;
    STRING CaseSensitiveChallengeResponse;
    STRING CaseInsensitiveChallengeResponse;
    UNICODE_STRING UserName;
    UNICODE_STRING LogonDomainName;
    UCHAR UserSessionKey[MSV1_0_USER_SESSION_KEY_LENGTH];
    UCHAR LanmanSessionKey[MSV1_0_LANMAN_SESSION_KEY_LENGTH];
} MSV1_0_GETCHALLENRESP_RESPONSE, *PMSV1_0_GETCHALLENRESP_RESPONSE;

//
// MsV1_0EnumerateUsers submit buffer and response
//

typedef struct _MSV1_0_ENUMUSERS_REQUEST {
    MSV1_0_PROTOCOL_MESSAGE_TYPE MessageType;
} MSV1_0_ENUMUSERS_REQUEST, *PMSV1_0_ENUMUSERS_REQUEST;

typedef struct _MSV1_0_ENUMUSERS_RESPONSE {
    MSV1_0_PROTOCOL_MESSAGE_TYPE MessageType;
    ULONG NumberOfLoggedOnUsers;
    PLUID LogonIds;
    PULONG EnumHandles;
} MSV1_0_ENUMUSERS_RESPONSE, *PMSV1_0_ENUMUSERS_RESPONSE;

//
// MsV1_0GetUserInfo submit buffer and response
//

typedef struct _MSV1_0_GETUSERINFO_REQUEST {
    MSV1_0_PROTOCOL_MESSAGE_TYPE MessageType;
    LUID LogonId;
} MSV1_0_GETUSERINFO_REQUEST, *PMSV1_0_GETUSERINFO_REQUEST;

typedef struct _MSV1_0_GETUSERINFO_RESPONSE {
    MSV1_0_PROTOCOL_MESSAGE_TYPE MessageType;
    PSID UserSid;
    UNICODE_STRING UserName;
    UNICODE_STRING LogonDomainName;
    UNICODE_STRING LogonServer;
    SECURITY_LOGON_TYPE LogonType;
} MSV1_0_GETUSERINFO_RESPONSE, *PMSV1_0_GETUSERINFO_RESPONSE;

// begin_winnt

//
// Define access rights to files and directories
//

//
// The FILE_READ_DATA and FILE_WRITE_DATA constants are also defined in
// devioctl.h as FILE_READ_ACCESS and FILE_WRITE_ACCESS. The values for these
// constants *MUST* always be in sync.
// The values are redefined in devioctl.h because they must be available to
// both DOS and NT.
//

#define FILE_READ_DATA            ( 0x0001 )    // file & pipe
#define FILE_LIST_DIRECTORY       ( 0x0001 )    // directory

#define FILE_WRITE_DATA           ( 0x0002 )    // file & pipe
#define FILE_ADD_FILE             ( 0x0002 )    // directory

#define FILE_APPEND_DATA          ( 0x0004 )    // file
#define FILE_ADD_SUBDIRECTORY     ( 0x0004 )    // directory
#define FILE_CREATE_PIPE_INSTANCE ( 0x0004 )    // named pipe


#define FILE_READ_EA              ( 0x0008 )    // file & directory

#define FILE_WRITE_EA             ( 0x0010 )    // file & directory

#define FILE_EXECUTE              ( 0x0020 )    // file
#define FILE_TRAVERSE             ( 0x0020 )    // directory

#define FILE_DELETE_CHILD         ( 0x0040 )    // directory

#define FILE_READ_ATTRIBUTES      ( 0x0080 )    // all

#define FILE_WRITE_ATTRIBUTES     ( 0x0100 )    // all

#define FILE_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE | 0x1FF)

#define FILE_GENERIC_READ         (STANDARD_RIGHTS_READ     |\
                                   FILE_READ_DATA           |\
                                   FILE_READ_ATTRIBUTES     |\
                                   FILE_READ_EA             |\
                                   SYNCHRONIZE)


#define FILE_GENERIC_WRITE        (STANDARD_RIGHTS_WRITE    |\
                                   FILE_WRITE_DATA          |\
                                   FILE_WRITE_ATTRIBUTES    |\
                                   FILE_WRITE_EA            |\
                                   FILE_APPEND_DATA         |\
                                   SYNCHRONIZE)


#define FILE_GENERIC_EXECUTE      (STANDARD_RIGHTS_EXECUTE  |\
                                   FILE_READ_ATTRIBUTES     |\
                                   FILE_EXECUTE             |\
                                   SYNCHRONIZE)

// end_winnt


//
// Define share access rights to files and directories
//

#define FILE_SHARE_READ                 0x00000001  // winnt
#define FILE_SHARE_WRITE                0x00000002  // winnt
#define FILE_SHARE_DELETE               0x00000004  // winnt
#define FILE_SHARE_VALID_FLAGS          0x00000007

//
// Define the file attributes values
//
// Note:  0x00000008 is reserved for use for the old DOS VOLID (volume ID)
//        and is therefore not considered valid in NT.
//
// Note:  0x00000010 is reserved for use for the old DOS SUBDIRECTORY flag
//        and is therefore not considered valid in NT.  This flag has
//        been disassociated with file attributes since the other flags are
//        protected with READ_ and WRITE_ATTRIBUTES access to the file.
//
// Note:  Note also that the order of these flags is set to allow both the
//        FAT and the Pinball File Systems to directly set the attributes
//        flags in attributes words without having to pick each flag out
//        individually.  The order of these flags should not be changed!
//

#define FILE_ATTRIBUTE_READONLY             0x00000001  // winnt
#define FILE_ATTRIBUTE_HIDDEN               0x00000002  // winnt
#define FILE_ATTRIBUTE_SYSTEM               0x00000004  // winnt
//OLD DOS VOLID                             0x00000008

#define FILE_ATTRIBUTE_DIRECTORY            0x00000010  // winnt
#define FILE_ATTRIBUTE_ARCHIVE              0x00000020  // winnt
#define FILE_ATTRIBUTE_DEVICE               0x00000040  // winnt
#define FILE_ATTRIBUTE_NORMAL               0x00000080  // winnt

#define FILE_ATTRIBUTE_TEMPORARY            0x00000100  // winnt
#define FILE_ATTRIBUTE_SPARSE_FILE          0x00000200  // winnt
#define FILE_ATTRIBUTE_REPARSE_POINT        0x00000400  // winnt
#define FILE_ATTRIBUTE_COMPRESSED           0x00000800  // winnt

#define FILE_ATTRIBUTE_OFFLINE              0x00001000  // winnt
#define FILE_ATTRIBUTE_NOT_CONTENT_INDEXED  0x00002000  // winnt
#define FILE_ATTRIBUTE_ENCRYPTED            0x00004000  // winnt

//
//  This definition is old and will disappear shortly
//

#define FILE_ATTRIBUTE_CONTENT_INDEXED      FILE_ATTRIBUTE_NOT_CONTENT_INDEXED

#define FILE_ATTRIBUTE_VALID_FLAGS      0x00007fb7
#define FILE_ATTRIBUTE_VALID_SET_FLAGS  0x000031a7

//
// Define the create disposition values
//

#define FILE_SUPERSEDE                  0x00000000
#define FILE_OPEN                       0x00000001
#define FILE_CREATE                     0x00000002
#define FILE_OPEN_IF                    0x00000003
#define FILE_OVERWRITE                  0x00000004
#define FILE_OVERWRITE_IF               0x00000005
#define FILE_MAXIMUM_DISPOSITION        0x00000005

//
// Define the create/open option flags
//

#define FILE_DIRECTORY_FILE                     0x00000001
#define FILE_WRITE_THROUGH                      0x00000002
#define FILE_SEQUENTIAL_ONLY                    0x00000004
#define FILE_NO_INTERMEDIATE_BUFFERING          0x00000008

#define FILE_SYNCHRONOUS_IO_ALERT               0x00000010
#define FILE_SYNCHRONOUS_IO_NONALERT            0x00000020
#define FILE_NON_DIRECTORY_FILE                 0x00000040
#define FILE_CREATE_TREE_CONNECTION             0x00000080

#define FILE_COMPLETE_IF_OPLOCKED               0x00000100
#define FILE_NO_EA_KNOWLEDGE                    0x00000200
#define FILE_OPEN_FOR_RECOVERY                  0x00000400
#define FILE_RANDOM_ACCESS                      0x00000800

#define FILE_DELETE_ON_CLOSE                    0x00001000
#define FILE_OPEN_BY_FILE_ID                    0x00002000
#define FILE_OPEN_FOR_BACKUP_INTENT             0x00004000
#define FILE_NO_COMPRESSION                     0x00008000

#define FILE_RESERVE_OPFILTER                   0x00100000
#define FILE_OPEN_REPARSE_POINT                 0x00200000
#define FILE_OPEN_NO_RECALL                     0x00400000
#define FILE_OPEN_FOR_FREE_SPACE_QUERY          0x00800000

#define FILE_COPY_STRUCTURED_STORAGE            0x00000041
#define FILE_STRUCTURED_STORAGE                 0x00000441

#define FILE_VALID_OPTION_FLAGS                 0x00ffffff
#define FILE_VALID_PIPE_OPTION_FLAGS            0x00000032
#define FILE_VALID_MAILSLOT_OPTION_FLAGS        0x00000032
#define FILE_VALID_SET_FLAGS                    0x00000036

//
// Define the I/O status information return values for NtCreateFile/NtOpenFile
//

#define FILE_SUPERSEDED                 0x00000000
#define FILE_OPENED                     0x00000001
#define FILE_CREATED                    0x00000002
#define FILE_OVERWRITTEN                0x00000003
#define FILE_EXISTS                     0x00000004
#define FILE_DOES_NOT_EXIST             0x00000005

// end_ntddk end_wdm end_nthal

//
// Define the I/O status information return values for requests for oplocks
// via NtFsControlFile
//

#define FILE_OPLOCK_BROKEN_TO_LEVEL_2   0x00000007
#define FILE_OPLOCK_BROKEN_TO_NONE      0x00000008

//
// Define the I/O status information return values for NtCreateFile/NtOpenFile
// when the sharing access fails but a batch oplock break is in progress
//

#define FILE_OPBATCH_BREAK_UNDERWAY     0x00000009

//
// Define the filter flags for NtNotifyChangeDirectoryFile
//

#define FILE_NOTIFY_CHANGE_FILE_NAME    0x00000001   // winnt
#define FILE_NOTIFY_CHANGE_DIR_NAME     0x00000002   // winnt
#define FILE_NOTIFY_CHANGE_NAME         0x00000003
#define FILE_NOTIFY_CHANGE_ATTRIBUTES   0x00000004   // winnt
#define FILE_NOTIFY_CHANGE_SIZE         0x00000008   // winnt
#define FILE_NOTIFY_CHANGE_LAST_WRITE   0x00000010   // winnt
#define FILE_NOTIFY_CHANGE_LAST_ACCESS  0x00000020   // winnt
#define FILE_NOTIFY_CHANGE_CREATION     0x00000040   // winnt
#define FILE_NOTIFY_CHANGE_EA           0x00000080
#define FILE_NOTIFY_CHANGE_SECURITY     0x00000100   // winnt
#define FILE_NOTIFY_CHANGE_STREAM_NAME  0x00000200
#define FILE_NOTIFY_CHANGE_STREAM_SIZE  0x00000400
#define FILE_NOTIFY_CHANGE_STREAM_WRITE 0x00000800
#define FILE_NOTIFY_VALID_MASK          0x00000fff

//
// Define the file action type codes for NtNotifyChangeDirectoryFile
//

#define FILE_ACTION_ADDED                   0x00000001   // winnt
#define FILE_ACTION_REMOVED                 0x00000002   // winnt
#define FILE_ACTION_MODIFIED                0x00000003   // winnt
#define FILE_ACTION_RENAMED_OLD_NAME        0x00000004   // winnt
#define FILE_ACTION_RENAMED_NEW_NAME        0x00000005   // winnt
#define FILE_ACTION_ADDED_STREAM            0x00000006
#define FILE_ACTION_REMOVED_STREAM          0x00000007
#define FILE_ACTION_MODIFIED_STREAM         0x00000008
#define FILE_ACTION_REMOVED_BY_DELETE       0x00000009
#define FILE_ACTION_ID_NOT_TUNNELLED        0x0000000A
#define FILE_ACTION_TUNNELLED_ID_COLLISION  0x0000000B

//
// Define the NamedPipeType flags for NtCreateNamedPipeFile
//

#define FILE_PIPE_BYTE_STREAM_TYPE      0x00000000
#define FILE_PIPE_MESSAGE_TYPE          0x00000001

//
// Define the CompletionMode flags for NtCreateNamedPipeFile
//

#define FILE_PIPE_QUEUE_OPERATION       0x00000000
#define FILE_PIPE_COMPLETE_OPERATION    0x00000001

//
// Define the ReadMode flags for NtCreateNamedPipeFile
//

#define FILE_PIPE_BYTE_STREAM_MODE      0x00000000
#define FILE_PIPE_CONNECTED_STATE       0x00000003  
//
// Define special ByteOffset parameters for read and write operations
//

#define FILE_WRITE_TO_END_OF_FILE       0xffffffff
#define FILE_USE_FILE_POINTER_POSITION  0xfffffffe

//
// Define alignment requirement values
//

#define FILE_BYTE_ALIGNMENT             0x00000000
#define FILE_WORD_ALIGNMENT             0x00000001
#define FILE_LONG_ALIGNMENT             0x00000003
#define FILE_QUAD_ALIGNMENT             0x00000007
#define FILE_OCTA_ALIGNMENT             0x0000000f
#define FILE_32_BYTE_ALIGNMENT          0x0000001f
#define FILE_64_BYTE_ALIGNMENT          0x0000003f
#define FILE_128_BYTE_ALIGNMENT         0x0000007f
#define FILE_256_BYTE_ALIGNMENT         0x000000ff
#define FILE_512_BYTE_ALIGNMENT         0x000001ff

//
// Define the maximum length of a filename string
//

#define MAXIMUM_FILENAME_LENGTH         256

// end_ntddk end_wdm end_nthal

//
// Define the file system attributes flags
//

#define FILE_CASE_SENSITIVE_SEARCH      0x00000001  // winnt
#define FILE_CASE_PRESERVED_NAMES       0x00000002  // winnt
#define FILE_UNICODE_ON_DISK            0x00000004  // winnt
#define FILE_PERSISTENT_ACLS            0x00000008  // winnt
#define FILE_FILE_COMPRESSION           0x00000010  // winnt
#define FILE_VOLUME_QUOTAS              0x00000020  // winnt
#define FILE_SUPPORTS_SPARSE_FILES      0x00000040  // winnt
#define FILE_SUPPORTS_REPARSE_POINTS    0x00000080  // winnt
#define FILE_SUPPORTS_REMOTE_STORAGE    0x00000100  // winnt
#define FILE_VOLUME_IS_COMPRESSED       0x00008000  // winnt
#define FILE_SUPPORTS_OBJECT_IDS        0x00010000  // winnt
#define FILE_SUPPORTS_ENCRYPTION        0x00020000  // winnt
#define FILE_NAMED_STREAMS              0x00040000  // winnt

//
// Define the flags for NtSet(Query)EaFile service structure entries
//

#define FILE_NEED_EA                    0x00000080

//
// Define EA type values
//

#define FILE_EA_TYPE_BINARY             0xfffe
#define FILE_EA_TYPE_ASCII              0xfffd
#define FILE_EA_TYPE_BITMAP             0xfffb
#define FILE_EA_TYPE_METAFILE           0xfffa
#define FILE_EA_TYPE_ICON               0xfff9
#define FILE_EA_TYPE_EA                 0xffee
#define FILE_EA_TYPE_MVMT               0xffdf
#define FILE_EA_TYPE_MVST               0xffde
#define FILE_EA_TYPE_ASN1               0xffdd
#define FILE_EA_TYPE_FAMILY_IDS         0xff01

// begin_ntddk begin_wdm begin_nthal
//
// Define the various device characteristics flags
//

#define FILE_REMOVABLE_MEDIA            0x00000001
#define FILE_READ_ONLY_DEVICE           0x00000002
#define FILE_FLOPPY_DISKETTE            0x00000004
#define FILE_WRITE_ONCE_MEDIA           0x00000008
#define FILE_REMOTE_DEVICE              0x00000010
#define FILE_DEVICE_IS_MOUNTED          0x00000020
#define FILE_VIRTUAL_VOLUME             0x00000040
#define FILE_AUTOGENERATED_DEVICE_NAME  0x00000080
#define FILE_DEVICE_SECURE_OPEN         0x00000100

// end_wdm

//
// flags specified here will be propagated up and down a device stack
// after FDO and all filter devices are added, but before the device
// stack is started
//

#define FILE_CHARACTERISTICS_PROPAGATED (   FILE_REMOVABLE_MEDIA   | \
                                            FILE_READ_ONLY_DEVICE  | \
                                            FILE_FLOPPY_DISKETTE   | \
                                            FILE_WRITE_ONCE_MEDIA  | \
                                            FILE_DEVICE_SECURE_OPEN  )

// end_ntddk end_nthal

// begin_ntddk begin_wdm begin_nthal
//
// Define the base asynchronous I/O argument types
//

typedef struct _IO_STATUS_BLOCK {
    union {
        NTSTATUS Status;
        PVOID Pointer;
    };

    ULONG_PTR Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;

#if defined(_WIN64)
typedef struct _IO_STATUS_BLOCK32 {
    NTSTATUS Status;
    ULONG Information;
} IO_STATUS_BLOCK32, *PIO_STATUS_BLOCK32;
#endif


//
// Define an Asynchronous Procedure Call from I/O viewpoint
//

typedef
VOID
(NTAPI *PIO_APC_ROUTINE) (
    IN PVOID ApcContext,
    IN PIO_STATUS_BLOCK IoStatusBlock,
    IN ULONG Reserved
    );
#define PIO_APC_ROUTINE_DEFINED

// end_ntddk end_wdm end_nthal

// begin_winnt

//
// Define the file notification information structure
//

typedef struct _FILE_NOTIFY_INFORMATION {
    ULONG NextEntryOffset;
    ULONG Action;
    ULONG FileNameLength;
    WCHAR FileName[1];
} FILE_NOTIFY_INFORMATION, *PFILE_NOTIFY_INFORMATION;

// end_winnt

// begin_ntddk begin_wdm begin_nthal
//
// Define the file information class values
//
// WARNING:  The order of the following values are assumed by the I/O system.
//           Any changes made here should be reflected there as well.
//

typedef enum _FILE_INFORMATION_CLASS {
// end_wdm
    FileDirectoryInformation       = 1,
    FileFullDirectoryInformation,   // 2
    FileBothDirectoryInformation,   // 3
    FileBasicInformation,           // 4  wdm
    FileStandardInformation,        // 5  wdm
    FileInternalInformation,        // 6
    FileEaInformation,              // 7
    FileAccessInformation,          // 8
    FileNameInformation,            // 9
    FileRenameInformation,          // 10
    FileLinkInformation,            // 11
    FileNamesInformation,           // 12
    FileDispositionInformation,     // 13
    FilePositionInformation,        // 14 wdm
    FileFullEaInformation,          // 15
    FileModeInformation,            // 16
    FileAlignmentInformation,       // 17
    FileAllInformation,             // 18
    FileAllocationInformation,      // 19
    FileEndOfFileInformation,       // 20 wdm
    FileAlternateNameInformation,   // 21
    FileStreamInformation,          // 22
    FilePipeInformation,            // 23
    FilePipeLocalInformation,       // 24
    FilePipeRemoteInformation,      // 25
    FileMailslotQueryInformation,   // 26
    FileMailslotSetInformation,     // 27
    FileCompressionInformation,     // 28
    FileObjectIdInformation,        // 29
    FileCompletionInformation,      // 30
    FileMoveClusterInformation,     // 31
    FileQuotaInformation,           // 32
    FileReparsePointInformation,    // 33
    FileNetworkOpenInformation,     // 34
    FileAttributeTagInformation,    // 35
    FileTrackingInformation,        // 36
    FileIdBothDirectoryInformation, // 37
    FileIdFullDirectoryInformation, // 38
    FileMaximumInformation
// begin_wdm
} FILE_INFORMATION_CLASS, *PFILE_INFORMATION_CLASS;

//
// Define the various structures which are returned on query operations
//

// end_ntddk end_wdm end_nthal

//
// NtQueryDirectoryFile return types:
//
//      FILE_DIRECTORY_INFORMATION
//      FILE_FULL_DIR_INFORMATION
//      FILE_ID_FULL_DIR_INFORMATION
//      FILE_BOTH_DIR_INFORMATION
//      FILE_ID_BOTH_DIR_INFORMATION
//      FILE_NAMES_INFORMATION
//      FILE_OBJECTID_INFORMATION
//

typedef struct _FILE_DIRECTORY_INFORMATION {
    ULONG NextEntryOffset;
    ULONG FileIndex;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER EndOfFile;
    LARGE_INTEGER AllocationSize;
    ULONG FileAttributes;
    ULONG FileNameLength;
    WCHAR FileName[1];
} FILE_DIRECTORY_INFORMATION, *PFILE_DIRECTORY_INFORMATION;

typedef struct _FILE_FULL_DIR_INFORMATION {
    ULONG NextEntryOffset;
    ULONG FileIndex;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER EndOfFile;
    LARGE_INTEGER AllocationSize;
    ULONG FileAttributes;
    ULONG FileNameLength;
    ULONG EaSize;
    WCHAR FileName[1];
} FILE_FULL_DIR_INFORMATION, *PFILE_FULL_DIR_INFORMATION;

typedef struct _FILE_ID_FULL_DIR_INFORMATION {
    ULONG NextEntryOffset;
    ULONG FileIndex;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER EndOfFile;
    LARGE_INTEGER AllocationSize;
    ULONG FileAttributes;
    ULONG FileNameLength;
    ULONG EaSize;
    LARGE_INTEGER FileId;
    WCHAR FileName[1];
} FILE_ID_FULL_DIR_INFORMATION, *PFILE_ID_FULL_DIR_INFORMATION;

typedef struct _FILE_BOTH_DIR_INFORMATION {
    ULONG NextEntryOffset;
    ULONG FileIndex;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER EndOfFile;
    LARGE_INTEGER AllocationSize;
    ULONG FileAttributes;
    ULONG FileNameLength;
    ULONG EaSize;
    CCHAR ShortNameLength;
    WCHAR ShortName[12];
    WCHAR FileName[1];
} FILE_BOTH_DIR_INFORMATION, *PFILE_BOTH_DIR_INFORMATION;

typedef struct _FILE_ID_BOTH_DIR_INFORMATION {
    ULONG NextEntryOffset;
    ULONG FileIndex;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER EndOfFile;
    LARGE_INTEGER AllocationSize;
    ULONG FileAttributes;
    ULONG FileNameLength;
    ULONG EaSize;
    CCHAR ShortNameLength;
    WCHAR ShortName[12];
    LARGE_INTEGER FileId;
    WCHAR FileName[1];
} FILE_ID_BOTH_DIR_INFORMATION, *PFILE_ID_BOTH_DIR_INFORMATION;

typedef struct _FILE_NAMES_INFORMATION {
    ULONG NextEntryOffset;
    ULONG FileIndex;
    ULONG FileNameLength;
    WCHAR FileName[1];
} FILE_NAMES_INFORMATION, *PFILE_NAMES_INFORMATION;

typedef struct _FILE_OBJECTID_INFORMATION {
    LONGLONG FileReference;
    UCHAR ObjectId[16];
    union {
        struct {
            UCHAR BirthVolumeId[16];
            UCHAR BirthObjectId[16];
            UCHAR DomainId[16];
        } ;
        UCHAR ExtendedInfo[48];
    };
} FILE_OBJECTID_INFORMATION, *PFILE_OBJECTID_INFORMATION;

//
//  The following constants provide addition meta characters to fully
//  support the more obscure aspects of DOS wild card processing.
//

#define ANSI_DOS_STAR   ('<')
#define ANSI_DOS_QM     ('>')
#define ANSI_DOS_DOT    ('"')

#define DOS_STAR        (L'<')
#define DOS_QM          (L'>')
#define DOS_DOT         (L'"')

//
// NtQuery(Set)InformationFile return types:
//
//      FILE_BASIC_INFORMATION
//      FILE_STANDARD_INFORMATION
//      FILE_INTERNAL_INFORMATION
//      FILE_EA_INFORMATION
//      FILE_ACCESS_INFORMATION
//      FILE_POSITION_INFORMATION
//      FILE_MODE_INFORMATION
//      FILE_ALIGNMENT_INFORMATION
//      FILE_NAME_INFORMATION
//      FILE_ALL_INFORMATION
//
//      FILE_NETWORK_OPEN_INFORMATION
//
//      FILE_ALLOCATION_INFORMATION
//      FILE_COMPRESSION_INFORMATION
//      FILE_DISPOSITION_INFORMATION
//      FILE_END_OF_FILE_INFORMATION
//      FILE_LINK_INFORMATION
//      FILE_MOVE_CLUSTER_INFORMATION
//      FILE_RENAME_INFORMATION
//      FILE_STREAM_INFORMATION
//      FILE_COMPLETION_INFORMATION
//
//      FILE_PIPE_INFORMATION
//      FILE_PIPE_LOCAL_INFORMATION
//      FILE_PIPE_REMOTE_INFORMATION
//
//      FILE_MAILSLOT_QUERY_INFORMATION
//      FILE_MAILSLOT_SET_INFORMATION
//      FILE_REPARSE_POINT_INFORMATION
//

typedef struct _FILE_BASIC_INFORMATION {                    // ntddk wdm nthal
    LARGE_INTEGER CreationTime;                             // ntddk wdm nthal
    LARGE_INTEGER LastAccessTime;                           // ntddk wdm nthal
    LARGE_INTEGER LastWriteTime;                            // ntddk wdm nthal
    LARGE_INTEGER ChangeTime;                               // ntddk wdm nthal
    ULONG FileAttributes;                                   // ntddk wdm nthal
} FILE_BASIC_INFORMATION, *PFILE_BASIC_INFORMATION;         // ntddk wdm nthal
                                                            // ntddk wdm nthal
typedef struct _FILE_STANDARD_INFORMATION {                 // ntddk wdm nthal
    LARGE_INTEGER AllocationSize;                           // ntddk wdm nthal
    LARGE_INTEGER EndOfFile;                                // ntddk wdm nthal
    ULONG NumberOfLinks;                                    // ntddk wdm nthal
    BOOLEAN DeletePending;                                  // ntddk wdm nthal
    BOOLEAN Directory;                                      // ntddk wdm nthal
} FILE_STANDARD_INFORMATION, *PFILE_STANDARD_INFORMATION;   // ntddk wdm nthal
                                                            // ntddk wdm nthal
typedef struct _FILE_INTERNAL_INFORMATION {
    LARGE_INTEGER IndexNumber;
} FILE_INTERNAL_INFORMATION, *PFILE_INTERNAL_INFORMATION;

typedef struct _FILE_EA_INFORMATION {
    ULONG EaSize;
} FILE_EA_INFORMATION, *PFILE_EA_INFORMATION;

typedef struct _FILE_ACCESS_INFORMATION {
    ACCESS_MASK AccessFlags;
} FILE_ACCESS_INFORMATION, *PFILE_ACCESS_INFORMATION;

typedef struct _FILE_POSITION_INFORMATION {                 // ntddk wdm nthal
    LARGE_INTEGER CurrentByteOffset;                        // ntddk wdm nthal
} FILE_POSITION_INFORMATION, *PFILE_POSITION_INFORMATION;   // ntddk wdm nthal
                                                            // ntddk wdm nthal
typedef struct _FILE_MODE_INFORMATION {
    ULONG Mode;
} FILE_MODE_INFORMATION, *PFILE_MODE_INFORMATION;

typedef struct _FILE_ALIGNMENT_INFORMATION {                // ntddk nthal
    ULONG AlignmentRequirement;                             // ntddk nthal
} FILE_ALIGNMENT_INFORMATION, *PFILE_ALIGNMENT_INFORMATION; // ntddk nthal
                                                            // ntddk nthal
typedef struct _FILE_NAME_INFORMATION {                     // ntddk
    ULONG FileNameLength;                                   // ntddk
    WCHAR FileName[1];                                      // ntddk
} FILE_NAME_INFORMATION, *PFILE_NAME_INFORMATION;           // ntddk
                                                            // ntddk
typedef struct _FILE_ALL_INFORMATION {
    FILE_BASIC_INFORMATION BasicInformation;
    FILE_STANDARD_INFORMATION StandardInformation;
    FILE_INTERNAL_INFORMATION InternalInformation;
    FILE_EA_INFORMATION EaInformation;
    FILE_ACCESS_INFORMATION AccessInformation;
    FILE_POSITION_INFORMATION PositionInformation;
    FILE_MODE_INFORMATION ModeInformation;
    FILE_ALIGNMENT_INFORMATION AlignmentInformation;
    FILE_NAME_INFORMATION NameInformation;
} FILE_ALL_INFORMATION, *PFILE_ALL_INFORMATION;

typedef struct _FILE_NETWORK_OPEN_INFORMATION {                 // ntddk wdm nthal
    LARGE_INTEGER CreationTime;                                 // ntddk wdm nthal
    LARGE_INTEGER LastAccessTime;                               // ntddk wdm nthal
    LARGE_INTEGER LastWriteTime;                                // ntddk wdm nthal
    LARGE_INTEGER ChangeTime;                                   // ntddk wdm nthal
    LARGE_INTEGER AllocationSize;                               // ntddk wdm nthal
    LARGE_INTEGER EndOfFile;                                    // ntddk wdm nthal
    ULONG FileAttributes;                                       // ntddk wdm nthal
} FILE_NETWORK_OPEN_INFORMATION, *PFILE_NETWORK_OPEN_INFORMATION;   // ntddk wdm nthal
                                                                // ntddk wdm nthal
typedef struct _FILE_ATTRIBUTE_TAG_INFORMATION {               // ntddk nthal
    ULONG FileAttributes;                                       // ntddk nthal
    ULONG ReparseTag;                                           // ntddk nthal
} FILE_ATTRIBUTE_TAG_INFORMATION, *PFILE_ATTRIBUTE_TAG_INFORMATION;  // ntddk nthal
                                                                // ntddk nthal
typedef struct _FILE_ALLOCATION_INFORMATION {
    LARGE_INTEGER AllocationSize;
} FILE_ALLOCATION_INFORMATION, *PFILE_ALLOCATION_INFORMATION;


typedef struct _FILE_DISPOSITION_INFORMATION {                  // ntddk nthal
    BOOLEAN DeleteFile;                                         // ntddk nthal
} FILE_DISPOSITION_INFORMATION, *PFILE_DISPOSITION_INFORMATION; // ntddk nthal
                                                                // ntddk nthal
typedef struct _FILE_END_OF_FILE_INFORMATION {                  // ntddk nthal
    LARGE_INTEGER EndOfFile;                                    // ntddk nthal
} FILE_END_OF_FILE_INFORMATION, *PFILE_END_OF_FILE_INFORMATION; // ntddk nthal
                                                                // ntddk nthal
#ifdef _MAC
#pragma warning( disable : 4121)
#endif

typedef struct _FILE_LINK_INFORMATION {
    BOOLEAN ReplaceIfExists;
    HANDLE RootDirectory;
    ULONG FileNameLength;
    WCHAR FileName[1];
} FILE_LINK_INFORMATION, *PFILE_LINK_INFORMATION;


typedef struct _FILE_RENAME_INFORMATION {
    BOOLEAN ReplaceIfExists;
    HANDLE RootDirectory;
    ULONG FileNameLength;
    WCHAR FileName[1];
} FILE_RENAME_INFORMATION, *PFILE_RENAME_INFORMATION;

#ifdef _MAC
#pragma warning( default : 4121 )
#endif

typedef struct _FILE_STREAM_INFORMATION {
    ULONG NextEntryOffset;
    ULONG StreamNameLength;
    LARGE_INTEGER StreamSize;
    LARGE_INTEGER StreamAllocationSize;
    WCHAR StreamName[1];
} FILE_STREAM_INFORMATION, *PFILE_STREAM_INFORMATION;

typedef struct _FILE_TRACKING_INFORMATION {
    HANDLE DestinationFile;
    ULONG ObjectInformationLength;
    CHAR ObjectInformation[1];
} FILE_TRACKING_INFORMATION, *PFILE_TRACKING_INFORMATION;

typedef struct _FILE_COMPLETION_INFORMATION {
    HANDLE Port;
    PVOID Key;
} FILE_COMPLETION_INFORMATION, *PFILE_COMPLETION_INFORMATION;

typedef struct _FILE_PIPE_INFORMATION {
     ULONG ReadMode;
     ULONG CompletionMode;
} FILE_PIPE_INFORMATION, *PFILE_PIPE_INFORMATION;

typedef struct _FILE_PIPE_LOCAL_INFORMATION {
     ULONG NamedPipeType;
     ULONG NamedPipeConfiguration;
     ULONG MaximumInstances;
     ULONG CurrentInstances;
     ULONG InboundQuota;
     ULONG ReadDataAvailable;
     ULONG OutboundQuota;
     ULONG WriteQuotaAvailable;
     ULONG NamedPipeState;
     ULONG NamedPipeEnd;
} FILE_PIPE_LOCAL_INFORMATION, *PFILE_PIPE_LOCAL_INFORMATION;

typedef struct _FILE_PIPE_REMOTE_INFORMATION {
     LARGE_INTEGER CollectDataTime;
     ULONG MaximumCollectionCount;
} FILE_PIPE_REMOTE_INFORMATION, *PFILE_PIPE_REMOTE_INFORMATION;

//
// NtQuery(Set)EaFile
//
// The offset for the start of EaValue is EaName[EaNameLength + 1]
//

// begin_ntddk begin_wdm

typedef struct _FILE_FULL_EA_INFORMATION {
    ULONG NextEntryOffset;
    UCHAR Flags;
    UCHAR EaNameLength;
    USHORT EaValueLength;
    CHAR EaName[1];
} FILE_FULL_EA_INFORMATION, *PFILE_FULL_EA_INFORMATION;

// end_ntddk end_wdm

typedef struct _FILE_GET_EA_INFORMATION {
    ULONG NextEntryOffset;
    UCHAR EaNameLength;
    CHAR EaName[1];
} FILE_GET_EA_INFORMATION, *PFILE_GET_EA_INFORMATION;

//
// NtQuery(Set)QuotaInformationFile
//

typedef struct _FILE_GET_QUOTA_INFORMATION {
    ULONG NextEntryOffset;
    ULONG SidLength;
    SID Sid;
} FILE_GET_QUOTA_INFORMATION, *PFILE_GET_QUOTA_INFORMATION;

typedef struct _FILE_QUOTA_INFORMATION {
    ULONG NextEntryOffset;
    ULONG SidLength;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER QuotaUsed;
    LARGE_INTEGER QuotaThreshold;
    LARGE_INTEGER QuotaLimit;
    SID Sid;
} FILE_QUOTA_INFORMATION, *PFILE_QUOTA_INFORMATION;

// begin_ntddk begin_wdm begin_nthal
//
// Define the file system information class values
//
// WARNING:  The order of the following values are assumed by the I/O system.
//           Any changes made here should be reflected there as well.

typedef enum _FSINFOCLASS {
    FileFsVolumeInformation       = 1,
    FileFsLabelInformation,      // 2
    FileFsSizeInformation,       // 3
    FileFsDeviceInformation,     // 4
    FileFsAttributeInformation,  // 5
    FileFsControlInformation,    // 6
    FileFsFullSizeInformation,   // 7
    FileFsObjectIdInformation,   // 8
    FileFsMaximumInformation
} FS_INFORMATION_CLASS, *PFS_INFORMATION_CLASS;

// end_ntddk end_wdm end_nthal
//
// NtQuery[Set]VolumeInformationFile types:
//
//  FILE_FS_LABEL_INFORMATION
//  FILE_FS_VOLUME_INFORMATION
//  FILE_FS_SIZE_INFORMATION
//  FILE_FS_DEVICE_INFORMATION
//  FILE_FS_ATTRIBUTE_INFORMATION
//  FILE_FS_CONTROL_INFORMATION
//  FILE_FS_OBJECTID_INFORMATION
//

typedef struct _FILE_FS_LABEL_INFORMATION {
    ULONG VolumeLabelLength;
    WCHAR VolumeLabel[1];
} FILE_FS_LABEL_INFORMATION, *PFILE_FS_LABEL_INFORMATION;

typedef struct _FILE_FS_VOLUME_INFORMATION {
    LARGE_INTEGER VolumeCreationTime;
    ULONG VolumeSerialNumber;
    ULONG VolumeLabelLength;
    BOOLEAN SupportsObjects;
    WCHAR VolumeLabel[1];
} FILE_FS_VOLUME_INFORMATION, *PFILE_FS_VOLUME_INFORMATION;

typedef struct _FILE_FS_SIZE_INFORMATION {
    LARGE_INTEGER TotalAllocationUnits;
    LARGE_INTEGER AvailableAllocationUnits;
    ULONG SectorsPerAllocationUnit;
    ULONG BytesPerSector;
} FILE_FS_SIZE_INFORMATION, *PFILE_FS_SIZE_INFORMATION;

typedef struct _FILE_FS_FULL_SIZE_INFORMATION {
    LARGE_INTEGER TotalAllocationUnits;
    LARGE_INTEGER CallerAvailableAllocationUnits;
    LARGE_INTEGER ActualAvailableAllocationUnits;
    ULONG SectorsPerAllocationUnit;
    ULONG BytesPerSector;
} FILE_FS_FULL_SIZE_INFORMATION, *PFILE_FS_FULL_SIZE_INFORMATION;

typedef struct _FILE_FS_OBJECTID_INFORMATION {
    UCHAR ObjectId[16];
    UCHAR ExtendedInfo[48];
} FILE_FS_OBJECTID_INFORMATION, *PFILE_FS_OBJECTID_INFORMATION;

typedef struct _FILE_FS_DEVICE_INFORMATION {                    // ntddk wdm nthal
    DEVICE_TYPE DeviceType;                                     // ntddk wdm nthal
    ULONG Characteristics;                                      // ntddk wdm nthal
} FILE_FS_DEVICE_INFORMATION, *PFILE_FS_DEVICE_INFORMATION;     // ntddk wdm nthal
                                                                // ntddk wdm nthal
typedef struct _FILE_FS_ATTRIBUTE_INFORMATION {
    ULONG FileSystemAttributes;
    LONG MaximumComponentNameLength;
    ULONG FileSystemNameLength;
    WCHAR FileSystemName[1];
} FILE_FS_ATTRIBUTE_INFORMATION, *PFILE_FS_ATTRIBUTE_INFORMATION;

//
// Macro definition for defining IOCTL and FSCTL function control codes.  Note
// that function codes 0-2047 are reserved for Microsoft Corporation, and
// 2048-4095 are reserved for customers.
//
// These macros are defined in devioctl.h which contains the portable IO
// definitions (for use by both DOS and NT)
//

//
// The IoGetFunctionCodeFromCtlCode( ControlCode ) Macro is defined in io.h
// This macro is used to extract the function code from an IOCTL (or FSCTL).
// The macro can only be used in kernel mode code.
//

//
// General File System control codes - Note that these values are valid
// regardless of the actual file system type
//

//
//  IMPORTANT:  These values have been arranged in order of increasing
//              control codes.  Do NOT breaks this!!  Add all new codes
//              at end of list regardless of functionality type.
//
//  Note: FSCTL_QUERY_RETRIEVAL_POINTER and FSCTL_MARK_AS_SYSTEM_HIVE only
//        work from Kernel mode on local paging files or the system hives.
//

// begin_winioctl
#ifndef _FILESYSTEMFSCTL_
#define _FILESYSTEMFSCTL_

//
// The following is a list of the native file system fsctls followed by
// additional network file system fsctls.  Some values have been
// decommissioned.
//

#define FSCTL_REQUEST_OPLOCK_LEVEL_1    CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  0, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_REQUEST_OPLOCK_LEVEL_2    CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  1, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_REQUEST_BATCH_OPLOCK      CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  2, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_OPLOCK_BREAK_ACKNOWLEDGE  CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  3, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_OPBATCH_ACK_CLOSE_PENDING CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  4, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_OPLOCK_BREAK_NOTIFY       CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  5, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_LOCK_VOLUME               CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  6, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_UNLOCK_VOLUME             CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  7, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_DISMOUNT_VOLUME           CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  8, METHOD_BUFFERED, FILE_ANY_ACCESS)
// decommissioned fsctl value                                              9
#define FSCTL_IS_VOLUME_MOUNTED         CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 10, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_IS_PATHNAME_VALID         CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 11, METHOD_BUFFERED, FILE_ANY_ACCESS) // PATHNAME_BUFFER,
#define FSCTL_MARK_VOLUME_DIRTY         CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 12, METHOD_BUFFERED, FILE_ANY_ACCESS)
// decommissioned fsctl value                                             13
#define FSCTL_QUERY_RETRIEVAL_POINTERS  CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 14,  METHOD_NEITHER, FILE_ANY_ACCESS)
#define FSCTL_GET_COMPRESSION           CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 15, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_SET_COMPRESSION           CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 16, METHOD_BUFFERED, FILE_READ_DATA | FILE_WRITE_DATA)
// decommissioned fsctl value                                             17
// decommissioned fsctl value                                             18
#define FSCTL_MARK_AS_SYSTEM_HIVE       CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 19,  METHOD_NEITHER, FILE_ANY_ACCESS)
#define FSCTL_OPLOCK_BREAK_ACK_NO_2     CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 20, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_INVALIDATE_VOLUMES        CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 21, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_QUERY_FAT_BPB             CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 22, METHOD_BUFFERED, FILE_ANY_ACCESS) // FSCTL_QUERY_FAT_BPB_BUFFER
#define FSCTL_REQUEST_FILTER_OPLOCK     CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 23, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_FILESYSTEM_GET_STATISTICS CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 24, METHOD_BUFFERED, FILE_ANY_ACCESS) // FILESYSTEM_STATISTICS
#if(_WIN32_WINNT >= 0x0400)
#define FSCTL_GET_NTFS_VOLUME_DATA      CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 25, METHOD_BUFFERED, FILE_ANY_ACCESS) // NTFS_VOLUME_DATA_BUFFER
#define FSCTL_GET_NTFS_FILE_RECORD      CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 26, METHOD_BUFFERED, FILE_ANY_ACCESS) // NTFS_FILE_RECORD_INPUT_BUFFER, NTFS_FILE_RECORD_OUTPUT_BUFFER
#define FSCTL_GET_VOLUME_BITMAP         CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 27,  METHOD_NEITHER, FILE_ANY_ACCESS) // STARTING_LCN_INPUT_BUFFER, VOLUME_BITMAP_BUFFER
#define FSCTL_GET_RETRIEVAL_POINTERS    CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 28,  METHOD_NEITHER, FILE_ANY_ACCESS) // STARTING_VCN_INPUT_BUFFER, RETRIEVAL_POINTERS_BUFFER
#define FSCTL_MOVE_FILE                 CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 29, METHOD_BUFFERED, FILE_SPECIAL_ACCESS) // MOVE_FILE_DATA,
#define FSCTL_IS_VOLUME_DIRTY           CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 30, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_GET_HFS_INFORMATION       CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 31, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_ALLOW_EXTENDED_DASD_IO    CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 32, METHOD_NEITHER,  FILE_ANY_ACCESS)
#endif /* _WIN32_WINNT >= 0x0400 */

#if(_WIN32_WINNT >= 0x0500)
#define FSCTL_READ_PROPERTY_DATA        CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 33, METHOD_NEITHER, FILE_ANY_ACCESS)
#define FSCTL_WRITE_PROPERTY_DATA       CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 34, METHOD_NEITHER, FILE_ANY_ACCESS)
#define FSCTL_FIND_FILES_BY_SID         CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 35, METHOD_NEITHER, FILE_ANY_ACCESS)
// decommissioned fsctl value                                             36
#define FSCTL_DUMP_PROPERTY_DATA        CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 37,  METHOD_NEITHER, FILE_ANY_ACCESS)
#define FSCTL_SET_OBJECT_ID             CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 38, METHOD_BUFFERED, FILE_SPECIAL_ACCESS) // FILE_OBJECTID_BUFFER
#define FSCTL_GET_OBJECT_ID             CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 39, METHOD_BUFFERED, FILE_ANY_ACCESS) // FILE_OBJECTID_BUFFER
#define FSCTL_DELETE_OBJECT_ID          CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 40, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define FSCTL_SET_REPARSE_POINT         CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 41, METHOD_BUFFERED, FILE_SPECIAL_ACCESS) // REPARSE_DATA_BUFFER,
#define FSCTL_GET_REPARSE_POINT         CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 42, METHOD_BUFFERED, FILE_ANY_ACCESS) // REPARSE_DATA_BUFFER
#define FSCTL_DELETE_REPARSE_POINT      CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 43, METHOD_BUFFERED, FILE_SPECIAL_ACCESS) // REPARSE_DATA_BUFFER,
#define FSCTL_ENUM_USN_DATA             CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 44,  METHOD_NEITHER, FILE_ANY_ACCESS) // MFT_ENUM_DATA,
#define FSCTL_SECURITY_ID_CHECK         CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 45,  METHOD_NEITHER, FILE_READ_DATA)  // BULK_SECURITY_TEST_DATA,
#define FSCTL_READ_USN_JOURNAL          CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 46,  METHOD_NEITHER, FILE_ANY_ACCESS) // READ_USN_JOURNAL_DATA, USN
#define FSCTL_SET_OBJECT_ID_EXTENDED    CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 47, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define FSCTL_CREATE_OR_GET_OBJECT_ID   CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 48, METHOD_BUFFERED, FILE_ANY_ACCESS) // FILE_OBJECTID_BUFFER
#define FSCTL_SET_SPARSE                CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 49, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define FSCTL_SET_ZERO_DATA             CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 50, METHOD_BUFFERED, FILE_WRITE_DATA) // FILE_ZERO_DATA_INFORMATION,
#define FSCTL_QUERY_ALLOCATED_RANGES    CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 51,  METHOD_NEITHER, FILE_READ_DATA)  // FILE_ALLOCATED_RANGE_BUFFER, FILE_ALLOCATED_RANGE_BUFFER
#define FSCTL_ENABLE_UPGRADE            CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 52, METHOD_BUFFERED, FILE_WRITE_DATA)
#define FSCTL_SET_ENCRYPTION            CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 53,  METHOD_NEITHER, FILE_ANY_ACCESS) // ENCRYPTION_BUFFER, DECRYPTION_STATUS_BUFFER
#define FSCTL_ENCRYPTION_FSCTL_IO       CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 54,  METHOD_NEITHER, FILE_ANY_ACCESS)
#define FSCTL_WRITE_RAW_ENCRYPTED       CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 55,  METHOD_NEITHER, FILE_SPECIAL_ACCESS) // ENCRYPTED_DATA_INFO,
#define FSCTL_READ_RAW_ENCRYPTED        CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 56,  METHOD_NEITHER, FILE_SPECIAL_ACCESS) // REQUEST_RAW_ENCRYPTED_DATA, ENCRYPTED_DATA_INFO
#define FSCTL_CREATE_USN_JOURNAL        CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 57,  METHOD_NEITHER, FILE_ANY_ACCESS) // CREATE_USN_JOURNAL_DATA,
#define FSCTL_READ_FILE_USN_DATA        CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 58,  METHOD_NEITHER, FILE_ANY_ACCESS) // Read the Usn Record for a file
#define FSCTL_WRITE_USN_CLOSE_RECORD    CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 59,  METHOD_NEITHER, FILE_ANY_ACCESS) // Generate Close Usn Record
#define FSCTL_EXTEND_VOLUME             CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 60, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_QUERY_USN_JOURNAL         CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 61, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_DELETE_USN_JOURNAL        CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 62, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_MARK_HANDLE               CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 63, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_SIS_COPYFILE              CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 64, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_SIS_LINK_FILES            CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 65, METHOD_BUFFERED, FILE_READ_DATA | FILE_WRITE_DATA)
#define FSCTL_HSM_MSG                   CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 66, METHOD_BUFFERED, FILE_READ_DATA | FILE_WRITE_DATA)
#define FSCTL_NSS_CONTROL               CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 67, METHOD_BUFFERED, FILE_WRITE_DATA)
#define FSCTL_HSM_DATA                  CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 68, METHOD_NEITHER, FILE_READ_DATA | FILE_WRITE_DATA)
#define FSCTL_RECALL_FILE               CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 69, METHOD_NEITHER, FILE_ANY_ACCESS)
#define FSCTL_NSS_RCONTROL              CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 70, METHOD_BUFFERED, FILE_READ_DATA)
#endif /* _WIN32_WINNT >= 0x0500 */

//
// The following long list of structs are associated with the preceeding
// file system fsctls.
//
// Note that the structs for the property sets fsctls are not included
// in this file.  They can be found in ntfsprop.h where they should stay
// because of their dependency on objidl.h.
//

//
// Structure for FSCTL_IS_PATHNAME_VALID
//

typedef struct _PATHNAME_BUFFER {

    ULONG PathNameLength;
    WCHAR Name[1];

} PATHNAME_BUFFER, *PPATHNAME_BUFFER;

//
// Structure for FSCTL_QUERY_BPB_INFO
//

typedef struct _FSCTL_QUERY_FAT_BPB_BUFFER {

    UCHAR First0x24BytesOfBootSector[0x24];

} FSCTL_QUERY_FAT_BPB_BUFFER, *PFSCTL_QUERY_FAT_BPB_BUFFER;

#if(_WIN32_WINNT >= 0x0400)
//
// Structures for FSCTL_GET_NTFS_VOLUME_DATA.
// The user must pass the basic buffer below.  Ntfs
// will return as many fields as available in the extended
// buffer which follows immediately after the VOLUME_DATA_BUFFER.
//

typedef struct {

    LARGE_INTEGER VolumeSerialNumber;
    LARGE_INTEGER NumberSectors;
    LARGE_INTEGER TotalClusters;
    LARGE_INTEGER FreeClusters;
    LARGE_INTEGER TotalReserved;
    ULONG BytesPerSector;
    ULONG BytesPerCluster;
    ULONG BytesPerFileRecordSegment;
    ULONG ClustersPerFileRecordSegment;
    LARGE_INTEGER MftValidDataLength;
    LARGE_INTEGER MftStartLcn;
    LARGE_INTEGER Mft2StartLcn;
    LARGE_INTEGER MftZoneStart;
    LARGE_INTEGER MftZoneEnd;

} NTFS_VOLUME_DATA_BUFFER, *PNTFS_VOLUME_DATA_BUFFER;

typedef struct {

    ULONG ByteCount;

    USHORT MajorVersion;
    USHORT MinorVersion;

} NTFS_EXTENDED_VOLUME_DATA, *PNTFS_EXTENDED_VOLUME_DATA;
#endif /* _WIN32_WINNT >= 0x0400 */

#if(_WIN32_WINNT >= 0x0400)
//
// Structure for FSCTL_GET_VOLUME_BITMAP
//

typedef struct {

    LARGE_INTEGER StartingLcn;

} STARTING_LCN_INPUT_BUFFER, *PSTARTING_LCN_INPUT_BUFFER;

typedef struct {

    LARGE_INTEGER StartingLcn;
    LARGE_INTEGER BitmapSize;
    UCHAR Buffer[1];

} VOLUME_BITMAP_BUFFER, *PVOLUME_BITMAP_BUFFER;
#endif /* _WIN32_WINNT >= 0x0400 */

#if(_WIN32_WINNT >= 0x0400)
//
// Structure for FSCTL_GET_RETRIEVAL_POINTERS
//

typedef struct {

    LARGE_INTEGER StartingVcn;

} STARTING_VCN_INPUT_BUFFER, *PSTARTING_VCN_INPUT_BUFFER;

typedef struct RETRIEVAL_POINTERS_BUFFER {

    ULONG ExtentCount;
    LARGE_INTEGER StartingVcn;
    struct {
        LARGE_INTEGER NextVcn;
        LARGE_INTEGER Lcn;
    } Extents[1];

} RETRIEVAL_POINTERS_BUFFER, *PRETRIEVAL_POINTERS_BUFFER;
#endif /* _WIN32_WINNT >= 0x0400 */

#if(_WIN32_WINNT >= 0x0400)
//
// Structures for FSCTL_GET_NTFS_FILE_RECORD
//

typedef struct {

    LARGE_INTEGER FileReferenceNumber;

} NTFS_FILE_RECORD_INPUT_BUFFER, *PNTFS_FILE_RECORD_INPUT_BUFFER;

typedef struct {

    LARGE_INTEGER FileReferenceNumber;
    ULONG FileRecordLength;
    UCHAR FileRecordBuffer[1];

} NTFS_FILE_RECORD_OUTPUT_BUFFER, *PNTFS_FILE_RECORD_OUTPUT_BUFFER;
#endif /* _WIN32_WINNT >= 0x0400 */

#if(_WIN32_WINNT >= 0x0400)
//
// Structure for FSCTL_MOVE_FILE
//

typedef struct {

    HANDLE FileHandle;
    LARGE_INTEGER StartingVcn;
    LARGE_INTEGER StartingLcn;
    ULONG ClusterCount;

} MOVE_FILE_DATA, *PMOVE_FILE_DATA;
#endif /* _WIN32_WINNT >= 0x0400 */

#if(_WIN32_WINNT >= 0x0400)

//
//
// Structure for FSCTL_GET_HFS_INFORMATION
//

typedef struct {

    UCHAR FinderInfo[32];

} HFS_INFORMATION_BUFFER, *PHFS_INFORMATION_BUFFER;
#endif /* _WIN32_WINNT >= 0x0400 */

#if(_WIN32_WINNT >= 0x0500)
//
// Structure for FSCTL_FIND_FILES_BY_SID
//

typedef struct {
    ULONG Restart;
    SID Sid;
} FIND_BY_SID_DATA, *PFIND_BY_SID_DATA;
#endif /* _WIN32_WINNT >= 0x0500 */

#if(_WIN32_WINNT >= 0x0500)
//
//  The following structures apply to Usn operations.
//

//
// Structure for FSCTL_ENUM_USN_DATA
//

typedef struct {

    ULONGLONG StartFileReferenceNumber;
    USN LowUsn;
    USN HighUsn;

} MFT_ENUM_DATA, *PMFT_ENUM_DATA;

//
// Structure for FSCTL_CREATE_USN_JOURNAL
//

typedef struct {

    ULONGLONG MaximumSize;
    ULONGLONG AllocationDelta;

} CREATE_USN_JOURNAL_DATA, *PCREATE_USN_JOURNAL_DATA;

//
// Structure for FSCTL_READ_USN_JOURNAL
//

typedef struct {

    USN StartUsn;
    ULONG ReasonMask;
    ULONG ReturnOnlyOnClose;
    ULONGLONG Timeout;
    ULONGLONG BytesToWaitFor;
    ULONGLONG UsnJournalID;

} READ_USN_JOURNAL_DATA, *PREAD_USN_JOURNAL_DATA;

//
//  The initial Major.Minor version of the Usn record will be 2.0.
//  In general, the MinorVersion may be changed if fields are added
//  to this structure in such a way that the previous version of the
//  software can still correctly the fields it knows about.  The
//  MajorVersion should only be changed if the previous version of
//  any software using this structure would incorrectly handle new
//  records due to structure changes.
//
//  The first update to this will force the structure to version 2.0.
//  This will add the extended information about the source as
//  well as indicate the file name offset within the structure.
//
//  The following structure is returned with these fsctls.
//
//      FSCTL_READ_USN_JOURNAL
//      FSCTL_READ_FILE_USN_DATA
//      FSCTL_ENUM_USN_DATA
//

typedef struct {

    ULONG RecordLength;
    USHORT MajorVersion;
    USHORT MinorVersion;
    ULONGLONG FileReferenceNumber;
    ULONGLONG ParentFileReferenceNumber;
    USN Usn;
    LARGE_INTEGER TimeStamp;
    ULONG Reason;
    ULONG SourceInfo;
    ULONG SecurityId;
    ULONG FileAttributes;
    USHORT FileNameLength;
    USHORT FileNameOffset;
    WCHAR FileName[1];

} USN_RECORD, *PUSN_RECORD;

#define USN_PAGE_SIZE                    (0x1000)

#define USN_REASON_DATA_OVERWRITE        (0x00000001)
#define USN_REASON_DATA_EXTEND           (0x00000002)
#define USN_REASON_DATA_TRUNCATION       (0x00000004)
#define USN_REASON_NAMED_DATA_OVERWRITE  (0x00000010)
#define USN_REASON_NAMED_DATA_EXTEND     (0x00000020)
#define USN_REASON_NAMED_DATA_TRUNCATION (0x00000040)
#define USN_REASON_FILE_CREATE           (0x00000100)
#define USN_REASON_FILE_DELETE           (0x00000200)
#define USN_REASON_EA_CHANGE             (0x00000400)
#define USN_REASON_SECURITY_CHANGE       (0x00000800)
#define USN_REASON_RENAME_OLD_NAME       (0x00001000)
#define USN_REASON_RENAME_NEW_NAME       (0x00002000)
#define USN_REASON_INDEXABLE_CHANGE      (0x00004000)
#define USN_REASON_BASIC_INFO_CHANGE     (0x00008000)
#define USN_REASON_HARD_LINK_CHANGE      (0x00010000)
#define USN_REASON_COMPRESSION_CHANGE    (0x00020000)
#define USN_REASON_ENCRYPTION_CHANGE     (0x00040000)
#define USN_REASON_OBJECT_ID_CHANGE      (0x00080000)
#define USN_REASON_REPARSE_POINT_CHANGE  (0x00100000)
#define USN_REASON_STREAM_CHANGE         (0x00200000)

#define USN_REASON_CLOSE                 (0x80000000)

//
//  Structure for FSCTL_QUERY_USN_JOUNAL
//

typedef struct {

    ULONGLONG UsnJournalID;
    USN FirstUsn;
    USN NextUsn;
    USN LowestValidUsn;
    USN MaxUsn;
    ULONGLONG MaximumSize;
    ULONGLONG AllocationDelta;

} USN_JOURNAL_DATA, *PUSN_JOURNAL_DATA;

//
//  Structure for FSCTL_DELETE_USN_JOURNAL
//

typedef struct {

    ULONGLONG UsnJournalID;
    ULONG DeleteFlags;

} DELETE_USN_JOURNAL_DATA, *PDELETE_USN_JOURNAL_DATA;

#define USN_DELETE_FLAG_DELETE              (0x00000001)
#define USN_DELETE_FLAG_NOTIFY              (0x00000002)

#define USN_DELETE_VALID_FLAGS              (0x00000003)

//
//  Structure for FSCTL_MARK_HANDLE
//

typedef struct {

    ULONG UsnSourceInfo;
    HANDLE VolumeHandle;
    ULONG HandleInfo;

} MARK_HANDLE_INFO, *PMARK_HANDLE_INFO;

//
//  Flags for the additional source information above.
//
//      USN_SOURCE_DATA_MANAGEMENT - Service is not modifying the external view
//          of any part of the file.  Typical case is HSM moving data to
//          and from external storage.
//
//      USN_SOURCE_AUXILIARY_DATA - Service is not modifying the external view
//          of the file with regard to the application that created this file.
//          Can be used to add private data streams to a file.
//
//      USN_SOURCE_REPLICATION_MANAGEMENT - Service is modifying a file to match
//          the contents of the same file which exists in another member of the
//          replica set.
//

#define USN_SOURCE_DATA_MANAGEMENT          (0x00000001)
#define USN_SOURCE_AUXILIARY_DATA           (0x00000002)
#define USN_SOURCE_REPLICATION_MANAGEMENT   (0x00000004)

#endif /* _WIN32_WINNT >= 0x0500 */

#if(_WIN32_WINNT >= 0x0500)
//
// Structure for FSCTL_SECURITY_ID_CHECK
//

typedef struct {

    ACCESS_MASK DesiredAccess;
    ULONG SecurityIds[1];

} BULK_SECURITY_TEST_DATA, *PBULK_SECURITY_TEST_DATA;
#endif /* _WIN32_WINNT >= 0x0500 */

#if(_WIN32_WINNT >= 0x0500)
//
//  Output flags for the FSCTL_IS_VOLUME_DIRTY
//

#define VOLUME_IS_DIRTY                  (0x00000001)
#define VOLUME_UPGRADE_SCHEDULED         (0x00000002)
#endif /* _WIN32_WINNT >= 0x0500 */

//
// Structures for FSCTL_FILESYSTEM_GET_STATISTICS
//
// Filesystem performance counters
//

typedef struct _FILESYSTEM_STATISTICS {

    USHORT FileSystemType;
    USHORT Version;                     // currently version 1

    ULONG SizeOfCompleteStructure;      // must by a mutiple of 64 bytes

    ULONG UserFileReads;
    ULONG UserFileReadBytes;
    ULONG UserDiskReads;
    ULONG UserFileWrites;
    ULONG UserFileWriteBytes;
    ULONG UserDiskWrites;

    ULONG MetaDataReads;
    ULONG MetaDataReadBytes;
    ULONG MetaDataDiskReads;
    ULONG MetaDataWrites;
    ULONG MetaDataWriteBytes;
    ULONG MetaDataDiskWrites;

    //
    //  The file system's private structure is appended here.
    //

} FILESYSTEM_STATISTICS, *PFILESYSTEM_STATISTICS;

// values for FS_STATISTICS.FileSystemType

#define FILESYSTEM_STATISTICS_TYPE_NTFS     1
#define FILESYSTEM_STATISTICS_TYPE_FAT      2

//
//  File System Specific Statistics Data
//

typedef struct _FAT_STATISTICS {
    ULONG CreateHits;
    ULONG SuccessfulCreates;
    ULONG FailedCreates;

    ULONG NonCachedReads;
    ULONG NonCachedReadBytes;
    ULONG NonCachedWrites;
    ULONG NonCachedWriteBytes;

    ULONG NonCachedDiskReads;
    ULONG NonCachedDiskWrites;
} FAT_STATISTICS, *PFAT_STATISTICS;

typedef struct _NTFS_STATISTICS {

    ULONG LogFileFullExceptions;
    ULONG OtherExceptions;

    //
    // Other meta data io's
    //

    ULONG MftReads;
    ULONG MftReadBytes;
    ULONG MftWrites;
    ULONG MftWriteBytes;
    struct {
        USHORT Write;
        USHORT Create;
        USHORT SetInfo;
        USHORT Flush;
    } MftWritesUserLevel;

    USHORT MftWritesFlushForLogFileFull;
    USHORT MftWritesLazyWriter;
    USHORT MftWritesUserRequest;

    ULONG Mft2Writes;
    ULONG Mft2WriteBytes;
    struct {
        USHORT Write;
        USHORT Create;
        USHORT SetInfo;
        USHORT Flush;
    } Mft2WritesUserLevel;

    USHORT Mft2WritesFlushForLogFileFull;
    USHORT Mft2WritesLazyWriter;
    USHORT Mft2WritesUserRequest;

    ULONG RootIndexReads;
    ULONG RootIndexReadBytes;
    ULONG RootIndexWrites;
    ULONG RootIndexWriteBytes;

    ULONG BitmapReads;
    ULONG BitmapReadBytes;
    ULONG BitmapWrites;
    ULONG BitmapWriteBytes;

    USHORT BitmapWritesFlushForLogFileFull;
    USHORT BitmapWritesLazyWriter;
    USHORT BitmapWritesUserRequest;

    struct {
        USHORT Write;
        USHORT Create;
        USHORT SetInfo;
    } BitmapWritesUserLevel;

    ULONG MftBitmapReads;
    ULONG MftBitmapReadBytes;
    ULONG MftBitmapWrites;
    ULONG MftBitmapWriteBytes;

    USHORT MftBitmapWritesFlushForLogFileFull;
    USHORT MftBitmapWritesLazyWriter;
    USHORT MftBitmapWritesUserRequest;

    struct {
        USHORT Write;
        USHORT Create;
        USHORT SetInfo;
        USHORT Flush;
    } MftBitmapWritesUserLevel;

    ULONG UserIndexReads;
    ULONG UserIndexReadBytes;
    ULONG UserIndexWrites;
    ULONG UserIndexWriteBytes;

    //
    // Additions for NT 5.0
    //

    ULONG LogFileReads;
    ULONG LogFileReadBytes;
    ULONG LogFileWrites;
    ULONG LogFileWriteBytes;

    struct {
        ULONG Calls;                // number of individual calls to allocate clusters
        ULONG Clusters;             // number of clusters allocated
        ULONG Hints;                // number of times a hint was specified

        ULONG RunsReturned;         // number of runs used to satisify all the requests

        ULONG HintsHonored;         // number of times the hint was useful
        ULONG HintsClusters;        // number of clusters allocated via the hint
        ULONG Cache;                // number of times the cache was useful other than the hint
        ULONG CacheClusters;        // number of clusters allocated via the cache other than the hint
        ULONG CacheMiss;            // number of times the cache wasn't useful
        ULONG CacheMissClusters;    // number of clusters allocated without the cache
    } Allocate;

} NTFS_STATISTICS, *PNTFS_STATISTICS;

#if(_WIN32_WINNT >= 0x0500)
//
// Structure for FSCTL_SET_OBJECT_ID, FSCTL_GET_OBJECT_ID, and FSCTL_CREATE_OR_GET_OBJECT_ID
//

typedef struct _FILE_OBJECTID_BUFFER {

    //
    //  This is the portion of the object id that is indexed.
    //

    UCHAR ObjectId[16];

    //
    //  This portion of the object id is not indexed, it's just
    //  some metadata for the user's benefit.
    //

    union {
        struct {
            UCHAR BirthVolumeId[16];
            UCHAR BirthObjectId[16];
            UCHAR DomainId[16];
        } ;
        UCHAR ExtendedInfo[48];
    };

} FILE_OBJECTID_BUFFER, *PFILE_OBJECTID_BUFFER;
#endif /* _WIN32_WINNT >= 0x0500 */


#if(_WIN32_WINNT >= 0x0500)
//
// Structure for FSCTL_SET_SPARSE
//

typedef struct _FILE_SET_SPARSE_BUFFER {
    BOOLEAN SetSparse;
} FILE_SET_SPARSE_BUFFER, *PFILE_SET_SPARSE_BUFFER;


#endif /* _WIN32_WINNT >= 0x0500 */


#if(_WIN32_WINNT >= 0x0500)
//
// Structure for FSCTL_SET_ZERO_DATA
//

typedef struct _FILE_ZERO_DATA_INFORMATION {

    LARGE_INTEGER FileOffset;
    LARGE_INTEGER BeyondFinalZero;

} FILE_ZERO_DATA_INFORMATION, *PFILE_ZERO_DATA_INFORMATION;
#endif /* _WIN32_WINNT >= 0x0500 */

#if(_WIN32_WINNT >= 0x0500)
//
// Structure for FSCTL_QUERY_ALLOCATED_RANGES
//

//
// Querying the allocated ranges requires an output buffer to store the
// allocated ranges and an input buffer to specify the range to query.
// The input buffer contains a single entry, the output buffer is an
// array of the following structure.
//

typedef struct _FILE_ALLOCATED_RANGE_BUFFER {

    LARGE_INTEGER FileOffset;
    LARGE_INTEGER Length;

} FILE_ALLOCATED_RANGE_BUFFER, *PFILE_ALLOCATED_RANGE_BUFFER;
#endif /* _WIN32_WINNT >= 0x0500 */

#if(_WIN32_WINNT >= 0x0500)
//
// Structures for FSCTL_SET_ENCRYPTION, FSCTL_WRITE_RAW_ENCRYPTED, and FSCTL_READ_RAW_ENCRYPTED
//

//
//  The input buffer to set encryption indicates whether we are to encrypt/decrypt a file
//  or an individual stream.
//

typedef struct _ENCRYPTION_BUFFER {

    ULONG EncryptionOperation;
    UCHAR Private[1];

} ENCRYPTION_BUFFER, *PENCRYPTION_BUFFER;

#define FILE_SET_ENCRYPTION         0x00000001
#define FILE_CLEAR_ENCRYPTION       0x00000002
#define STREAM_SET_ENCRYPTION       0x00000003
#define STREAM_CLEAR_ENCRYPTION     0x00000004

#define MAXIMUM_ENCRYPTION_VALUE    0x00000004

//
//  The optional output buffer to set encryption indicates that the last encrypted
//  stream in a file has been marked as decrypted.
//

typedef struct _DECRYPTION_STATUS_BUFFER {

    BOOLEAN NoEncryptedStreams;

} DECRYPTION_STATUS_BUFFER, *PDECRYPTION_STATUS_BUFFER;

#define ENCRYPTION_FORMAT_DEFAULT        (0x01)

#define COMPRESSION_FORMAT_SPARSE        (0x4000)

//
//  Request Encrypted Data structure.  This is used to indicate
//  the range of the file to read.  It also describes the
//  output buffer used to return the data.
//

typedef struct _REQUEST_RAW_ENCRYPTED_DATA {

    //
    //  Requested file offset and requested length to read.
    //  The fsctl will round the starting offset down
    //  to a file system boundary.  It will also
    //  round the length up to a file system boundary.
    //

    LONGLONG FileOffset;
    ULONG Length;

} REQUEST_RAW_ENCRYPTED_DATA, *PREQUEST_RAW_ENCRYPTED_DATA;

//
//  Encrypted Data Information structure.  This structure
//  is used to return raw encrypted data from a file in
//  order to perform off-line recovery.  The data will be
//  encrypted or encrypted and compressed.  The off-line
//  service will need to use the encryption and compression
//  format information to recover the file data.  In the
//  event that the data is both encrypted and compressed then
//  the decryption must occur before decompression.  All
//  the data units below must be encrypted and compressed
//  with the same format.
//
//  The data will be returned in units.  The data unit size
//  will be fixed per request.  If the data is compressed
//  then the data unit size will be the compression unit size.
//
//  This structure is at the beginning of the buffer used to
//  return the encrypted data.  The actual raw bytes from
//  the file will follow this buffer.  The offset of the
//  raw bytes from the beginning of this structure is
//  specified in the REQUEST_RAW_ENCRYPTED_DATA structure
//  described above.
//

typedef struct _ENCRYPTED_DATA_INFO {

    //
    //  This is the file offset for the first entry in the
    //  data block array.  The file system will round
    //  the requested start offset down to a boundary
    //  that is consistent with the format of the file.
    //

    ULONGLONG StartingFileOffset;

    //
    //  Data offset in output buffer.  The output buffer
    //  begins with an ENCRYPTED_DATA_INFO structure.
    //  The file system will then store the raw bytes from
    //  disk beginning at the following offset within the
    //  output buffer.
    //

    ULONG OutputBufferOffset;

    //
    //  The number of bytes being returned that are within
    //  the size of the file.  If this value is less than
    //  (NumberOfDataBlocks << DataUnitShift), it means the
    //  end of the file occurs within this transfer.  Any
    //  data beyond file size is invalid and was never
    //  passed to the encryption driver.
    //

    ULONG BytesWithinFileSize;

    //
    //  The number of bytes being returned that are below
    //  valid data length.  If this value is less than
    //  (NumberOfDataBlocks << DataUnitShift), it means the
    //  end of the valid data occurs within this transfer.
    //  After decrypting the data from this transfer, any
    //  byte(s) beyond valid data length must be zeroed.
    //

    ULONG BytesWithinValidDataLength;

    //
    //  Code for the compression format as defined in
    //  ntrtl.h.  Note that COMPRESSION_FORMAT_NONE
    //  and COMPRESSION_FORMAT_DEFAULT are invalid if
    //  any of the described chunks are compressed.
    //

    USHORT CompressionFormat;

    //
    //  The DataUnit is the granularity used to access the
    //  disk.  It will be the same as the compression unit
    //  size for a compressed file.  For an uncompressed
    //  file, it will be some cluster-aligned power of 2 that
    //  the file system deems convenient.  A caller should
    //  not expect that successive calls will have the
    //  same data unit shift value as the previous call.
    //
    //  Since chunks and compression units are expected to be
    //  powers of 2 in size, we express them log2.  So, for
    //  example (1 << ChunkShift) == ChunkSizeInBytes.  The
    //  ClusterShift indicates how much space must be saved
    //  to successfully compress a compression unit - each
    //  successfully compressed data unit must occupy
    //  at least one cluster less in bytes than an uncompressed
    //  data block unit.
    //

    UCHAR DataUnitShift;
    UCHAR ChunkShift;
    UCHAR ClusterShift;

    //
    //  The format for the encryption.
    //

    UCHAR EncryptionFormat;

    //
    //  This is the number of entries in the data block size
    //  array.
    //

    USHORT NumberOfDataBlocks;

    //
    //  This is an array of sizes in the data block array.  There
    //  must be one entry in this array for each data block
    //  read from disk.  The size has a different meaning
    //  depending on whether the file is compressed.
    //
    //  A size of zero always indicates that the final data consists entirely
    //  of zeroes.  There is no decryption or decompression to
    //  perform.
    //
    //  If the file is compressed then the data block size indicates
    //  whether this block is compressed.  A size equal to
    //  the block size indicates that the corresponding block did
    //  not compress.  Any other non-zero size indicates the
    //  size of the compressed data which needs to be
    //  decrypted/decompressed.
    //
    //  If the file is not compressed then the data block size
    //  indicates the amount of data within the block that
    //  needs to be decrypted.  Any other non-zero size indicates
    //  that the remaining bytes in the data unit within the file
    //  consists of zeros.  An example of this is when the
    //  the read spans the valid data length of the file.  There
    //  is no data to decrypt past the valid data length.
    //

    ULONG DataBlockSize[ANYSIZE_ARRAY];

} ENCRYPTED_DATA_INFO;
typedef ENCRYPTED_DATA_INFO *PENCRYPTED_DATA_INFO;
#endif /* _WIN32_WINNT >= 0x0500 */

#if(_WIN32_WINNT >= 0x0500)
//
// FSCTL_SIS_COPYFILE support
// Source and destination file names are passed in the FileNameBuffer.
// Both strings are null terminated, with the source name starting at
// the beginning of FileNameBuffer, and the destination name immediately
// following.  Length fields include terminating nulls.
//

typedef struct _SI_COPYFILE {
    ULONG SourceFileNameLength;
    ULONG DestinationFileNameLength;
    ULONG Flags;
    WCHAR FileNameBuffer[1];
} SI_COPYFILE, *PSI_COPYFILE;

#define COPYFILE_SIS_LINK       0x0001              // Copy only if source is SIS
#define COPYFILE_SIS_REPLACE    0x0002              // Replace destination if it exists, otherwise don't.
#define COPYFILE_SIS_FLAGS      0x0003
#endif /* _WIN32_WINNT >= 0x0500 */

#if(_WIN32_WINNT >= 0x0500)
//
// FSCTL_NSS_CONTROL support
// Used to mark/unmark files as NSS
//
typedef struct _NSS_CONTROL {
    ULONG   code;
    ULONG   param;
} NSS_CONTROL, *PNSS_CONTROL;

#define NSS_CONTROL_MARK           1    // FSCTL_NSS_CONTROL
#define NSS_CONTROL_UNMARK         2    // FSCTL_NSS_CONTROL
#define NSS_CONTROL_ISNSSFILE      4    // FSCTL_NSS_RCONTROL
#endif /* _WIN32_WINNT >= 0x0500 */

#endif // _FILESYSTEMFSCTL_

// end_winioctl

//
// Structures for FSCTL_SET_REPARSE_POINT, FSCTL_GET_REPARSE_POINT, and FSCTL_DELETE_REPARSE_POINT
//

//
// The reparse structure is used by layered drivers to store data in a
// reparse point. The constraints on reparse tags are defined below.
// This version of the reparse data buffer is only for Microsoft tags.
//

typedef struct _REPARSE_DATA_BUFFER {
    ULONG  ReparseTag;
    USHORT ReparseDataLength;
    USHORT Reserved;
    union {
        struct {
            USHORT SubstituteNameOffset;
            USHORT SubstituteNameLength;
            USHORT PrintNameOffset;
            USHORT PrintNameLength;
            WCHAR PathBuffer[1];
        } SymbolicLinkReparseBuffer;
        struct {
            USHORT SubstituteNameOffset;
            USHORT SubstituteNameLength;
            USHORT PrintNameOffset;
            USHORT PrintNameLength;
            WCHAR PathBuffer[1];
        } MountPointReparseBuffer;
        struct {
            UCHAR  DataBuffer[1];
        } GenericReparseBuffer;
    };
} REPARSE_DATA_BUFFER, *PREPARSE_DATA_BUFFER;

#define REPARSE_DATA_BUFFER_HEADER_SIZE   FIELD_OFFSET(REPARSE_DATA_BUFFER, GenericReparseBuffer)


// begin_winnt
//
// The reparse GUID structure is used by all 3rd party layered drivers to
// store data in a reparse point. For non-Microsoft tags, The GUID field
// cannot be GUID_NULL.
// The constraints on reparse tags are defined below.
// Microsoft tags can also be used with this format of the reparse point buffer.
//

typedef struct _REPARSE_GUID_DATA_BUFFER {
    ULONG  ReparseTag;
    USHORT ReparseDataLength;
    USHORT Reserved;
    GUID   ReparseGuid;
    struct {
        UCHAR  DataBuffer[1];
    } GenericReparseBuffer;
} REPARSE_GUID_DATA_BUFFER, *PREPARSE_GUID_DATA_BUFFER;

#define REPARSE_GUID_DATA_BUFFER_HEADER_SIZE   FIELD_OFFSET(REPARSE_GUID_DATA_BUFFER, GenericReparseBuffer)



//
// Maximum allowed size of the reparse data.
//

#define MAXIMUM_REPARSE_DATA_BUFFER_SIZE      ( 16 * 1024 )

//
// Predefined reparse tags.
// These tags need to avoid conflicting with IO_REMOUNT defined in ntos\inc\io.h
//

#define IO_REPARSE_TAG_RESERVED_ZERO             (0)
#define IO_REPARSE_TAG_RESERVED_ONE              (1)

//
// The value of the following constant needs to satisfy the following conditions:
//  (1) Be at least as large as the largest of the reserved tags.
//  (2) Be strictly smaller than all the tags in use.
//

#define IO_REPARSE_TAG_RESERVED_RANGE            IO_REPARSE_TAG_RESERVED_ONE

//
// The reparse tags are a ULONG. The 32 bits are laid out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +-+-+-+-------------------------+-------------------------------+
//  |M|L|N|   Reserved bits         |       Reparse Tag Value       |
//  +-+-+-+-------------------------+-------------------------------+
//
// M is the Microsoft bit. When set to 1, it denotes a tag owned by Microsoft.
//   All ISVs must use a tag with a 0 in this position.
//   Note: If a Microsoft tag is used by non-Microsoft software, the
//   behavior is not defined.
//
// L is the high-latency bit. When set to 1, a file with this tag is
//   expected to have a long latency to retrieve the first byte of data.
//
// N is name surrogate. When set to 1, the file represents another named
//   entity in the system.
//
// The M, L, and N bits are OR-able.
// The following macros check for the M, L and N bit values:
//

//
// Macro to determine whether a reparse point tag corresponds to a tag
// owned by Microsoft.
//

#define IsReparseTagMicrosoft(_tag) (              \
                           ((_tag) & 0x80000000)   \
                           )

//
// Macro to determine whether a reparse point tag corresponds to a file
// that is to be displayed with the slow icon overlay.
//

#define IsReparseTagHighLatency(_tag) (            \
                           ((_tag) & 0x40000000)   \
                           )

//
// Macro to determine whether a reparse point tag corresponds to a file
// that is to be displayed with the slow icon overlay.
//

#define IsReparseTagNameSurrogate(_tag) (          \
                           ((_tag) & 0x20000000)   \
                           )


#define IO_REPARSE_TAG_MOUNT_POINT               (0xA0000003)        
#define IO_REPARSE_TAG_HSM                       (0xC0000004)        
#define IO_REPARSE_TAG_SIS                       (0x80000007)        
#define FSCTL_PIPE_PEEK                 CTL_CODE(FILE_DEVICE_NAMED_PIPE, 3, METHOD_BUFFERED, FILE_READ_DATA) 
// Control structure for FSCTL_PIPE_PEEK

typedef struct _FILE_PIPE_PEEK_BUFFER {
     ULONG NamedPipeState;
     ULONG ReadDataAvailable;
     ULONG NumberOfMessages;
     ULONG MessageLength;
     CHAR Data[1];
} FILE_PIPE_PEEK_BUFFER, *PFILE_PIPE_PEEK_BUFFER;

//
// Control structure for FSCTL_LMR_GET_LINK_TRACKING_INFORMATION
//

//
// For links on DFS volumes the volume id and machine id are returned for
// link tracking
//

typedef enum _LINK_TRACKING_INFORMATION_TYPE {
    NtfsLinkTrackingInformation,
    DfsLinkTrackingInformation
} LINK_TRACKING_INFORMATION_TYPE, *PLINK_TRACKING_INFORMATION_TYPE;

typedef struct _LINK_TRACKING_INFORMATION {
    LINK_TRACKING_INFORMATION_TYPE Type;
    UCHAR   VolumeId[16];
} LINK_TRACKING_INFORMATION, *PLINK_TRACKING_INFORMATION;

//
// Control structure for FSCTL_LMR_SET_LINK_TRACKING_INFORMATION
//

typedef struct _REMOTE_LINK_TRACKING_INFORMATION_ {
    PVOID       TargetFileObject;
    ULONG   TargetLinkTrackingInformationLength;
    UCHAR   TargetLinkTrackingInformationBuffer[1];
} REMOTE_LINK_TRACKING_INFORMATION,
 *PREMOTE_LINK_TRACKING_INFORMATION;


//
// Define the I/O bus interface types.
//

typedef enum _INTERFACE_TYPE {
    InterfaceTypeUndefined = -1,
    Internal,
    Isa,
    Eisa,
    MicroChannel,
    TurboChannel,
    PCIBus,
    VMEBus,
    NuBus,
    PCMCIABus,
    CBus,
    MPIBus,
    MPSABus,
    ProcessorInternal,
    InternalPowerBus,
    PNPISABus,
    PNPBus,
    MaximumInterfaceType
}INTERFACE_TYPE, *PINTERFACE_TYPE;

//
// Define the DMA transfer widths.
//

typedef enum _DMA_WIDTH {
    Width8Bits,
    Width16Bits,
    Width32Bits,
    MaximumDmaWidth
}DMA_WIDTH, *PDMA_WIDTH;

//
// Define DMA transfer speeds.
//

typedef enum _DMA_SPEED {
    Compatible,
    TypeA,
    TypeB,
    TypeC,
    TypeF,
    MaximumDmaSpeed
}DMA_SPEED, *PDMA_SPEED;

//
// Define Interface reference/dereference routines for
//  Interfaces exported by IRP_MN_QUERY_INTERFACE
//

typedef VOID (*PINTERFACE_REFERENCE)(PVOID Context);
typedef VOID (*PINTERFACE_DEREFERENCE)(PVOID Context);

// end_wdm

//
// Define types of bus information.
//

typedef enum _BUS_DATA_TYPE {
    ConfigurationSpaceUndefined = -1,
    Cmos,
    EisaConfiguration,
    Pos,
    CbusConfiguration,
    PCIConfiguration,
    VMEConfiguration,
    NuBusConfiguration,
    PCMCIAConfiguration,
    MPIConfiguration,
    MPSAConfiguration,
    PNPISAConfiguration,
    SgiInternalConfiguration,
    MaximumBusDataType
} BUS_DATA_TYPE, *PBUS_DATA_TYPE;

#define PROCESS_ALL_ACCESS        (STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE | \
                                   0xFFF)


#define THREAD_ALL_ACCESS         (STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE | \
                                   0x3FF)

//
// ClientId
//

typedef struct _CLIENT_ID {
    HANDLE UniqueProcess;
    HANDLE UniqueThread;
} CLIENT_ID;
typedef CLIENT_ID *PCLIENT_ID;

//
// Thread Environment Block (and portable part of Thread Information Block)
//

//
//  NT_TIB - Thread Information Block - Portable part.
//
//      This is the subsystem portable part of the Thread Information Block.
//      It appears as the first part of the TEB for all threads which have
//      a user mode component.
//
//

// begin_winnt

typedef struct _NT_TIB {
    struct _EXCEPTION_REGISTRATION_RECORD *ExceptionList;
    PVOID StackBase;
    PVOID StackLimit;
    PVOID SubSystemTib;
    union {
        PVOID FiberData;
        ULONG Version;
    };
    PVOID ArbitraryUserPointer;
    struct _NT_TIB *Self;
} NT_TIB;
typedef NT_TIB *PNT_TIB;
#define NtCurrentProcess() ( (HANDLE) -1 )  
#define NtCurrentThread() ( (HANDLE) -2 )   
//
// Priority increment definitions.  The comment for each definition gives
// the names of the system services that use the definition when satisfying
// a wait.
//

//
// Priority increment used when satisfying a wait on an executive event
// (NtPulseEvent and NtSetEvent)
//

#define EVENT_INCREMENT                 1

//
// Priority increment when no I/O has been done.  This is used by device
// and file system drivers when completing an IRP (IoCompleteRequest).
//

#define IO_NO_INCREMENT                 0

//
// Priority increment for completing CD-ROM I/O.  This is used by CD-ROM device
// and file system drivers when completing an IRP (IoCompleteRequest)
//

#define IO_CD_ROM_INCREMENT             1

//
// Priority increment for completing disk I/O.  This is used by disk device
// and file system drivers when completing an IRP (IoCompleteRequest)
//

#define IO_DISK_INCREMENT               1

//
// Priority increment for completing mailslot I/O.  This is used by the mail-
// slot file system driver when completing an IRP (IoCompleteRequest).
//

#define IO_MAILSLOT_INCREMENT           2

//
// Priority increment for completing named pipe I/O.  This is used by the
// named pipe file system driver when completing an IRP (IoCompleteRequest).
//

#define IO_NAMED_PIPE_INCREMENT         2

//
// Priority increment for completing network I/O.  This is used by network
// device and network file system drivers when completing an IRP
// (IoCompleteRequest).
//

#define IO_NETWORK_INCREMENT            2

//
// Priority increment used when satisfying a wait on an executive semaphore
// (NtReleaseSemaphore)
//

#define SEMAPHORE_INCREMENT             1


#if defined(_X86_)

//
// Types to use to contain PFNs and their counts.
//

typedef ULONG PFN_COUNT;

typedef LONG SPFN_NUMBER, *PSPFN_NUMBER;
typedef ULONG PFN_NUMBER, *PPFN_NUMBER;

//
// Define maximum size of flush multiple TB request.
//

#define FLUSH_MULTIPLE_MAXIMUM 16

//
// Indicate that the i386 compiler supports the pragma textout construct.
//

#define ALLOC_PRAGMA 1
//
// Indicate that the i386 compiler supports the DATA_SEG("INIT") and
// DATA_SEG("PAGE") pragmas
//

#define ALLOC_DATA_PRAGMA 1

#define NORMAL_DISPATCH_LENGTH 106                  
#define DISPATCH_LENGTH NORMAL_DISPATCH_LENGTH      
//
// STATUS register for each MCA bank.
//

typedef union _MCI_STATS {
    struct {
        USHORT  McaCod;
        USHORT  MsCod;
        ULONG   OtherInfo : 25;
        ULONG   Damage : 1;
        ULONG   AddressValid : 1;
        ULONG   MiscValid : 1;
        ULONG   Enabled : 1;
        ULONG   UnCorrected : 1;
        ULONG   OverFlow : 1;
        ULONG   Valid : 1;
    } MciStats;

    ULONGLONG QuadPart;

} MCI_STATS, *PMCI_STATS;

//
// Interrupt Request Level definitions
//

#define PASSIVE_LEVEL 0             // Passive release level
#define LOW_LEVEL 0                 // Lowest interrupt level
#define APC_LEVEL 1                 // APC interrupt level
#define DISPATCH_LEVEL 2            // Dispatcher level

#define PROFILE_LEVEL 27            // timer used for profiling.
#define CLOCK1_LEVEL 28             // Interval clock 1 level - Not used on x86
#define CLOCK2_LEVEL 28             // Interval clock 2 level
#define IPI_LEVEL 29                // Interprocessor interrupt level
#define POWER_LEVEL 30              // Power failure level
#define HIGH_LEVEL 31               // Highest interrupt level
#define SYNCH_LEVEL (IPI_LEVEL-1)   // synchronization level

//
// I/O space read and write macros.
//
//  These have to be actual functions on the 386, because we need
//  to use assembler, but cannot return a value if we inline it.
//
//  The READ/WRITE_REGISTER_* calls manipulate I/O registers in MEMORY space.
//  (Use x86 move instructions, with LOCK prefix to force correct behavior
//   w.r.t. caches and write buffers.)
//
//  The READ/WRITE_PORT_* calls manipulate I/O registers in PORT space.
//  (Use x86 in/out instructions.)
//

NTHALAPI
UCHAR
READ_REGISTER_UCHAR(
    PUCHAR  Register
    );

NTHALAPI
USHORT
READ_REGISTER_USHORT(
    PUSHORT Register
    );

NTHALAPI
ULONG
READ_REGISTER_ULONG(
    PULONG  Register
    );

NTHALAPI
VOID
READ_REGISTER_BUFFER_UCHAR(
    PUCHAR  Register,
    PUCHAR  Buffer,
    ULONG   Count
    );

NTHALAPI
VOID
READ_REGISTER_BUFFER_USHORT(
    PUSHORT Register,
    PUSHORT Buffer,
    ULONG   Count
    );

NTHALAPI
VOID
READ_REGISTER_BUFFER_ULONG(
    PULONG  Register,
    PULONG  Buffer,
    ULONG   Count
    );


NTHALAPI
VOID
WRITE_REGISTER_UCHAR(
    PUCHAR  Register,
    UCHAR   Value
    );

NTHALAPI
VOID
WRITE_REGISTER_USHORT(
    PUSHORT Register,
    USHORT  Value
    );

NTHALAPI
VOID
WRITE_REGISTER_ULONG(
    PULONG  Register,
    ULONG   Value
    );

NTHALAPI
VOID
WRITE_REGISTER_BUFFER_UCHAR(
    PUCHAR  Register,
    PUCHAR  Buffer,
    ULONG   Count
    );

NTHALAPI
VOID
WRITE_REGISTER_BUFFER_USHORT(
    PUSHORT Register,
    PUSHORT Buffer,
    ULONG   Count
    );

NTHALAPI
VOID
WRITE_REGISTER_BUFFER_ULONG(
    PULONG  Register,
    PULONG  Buffer,
    ULONG   Count
    );

NTHALAPI
UCHAR
READ_PORT_UCHAR(
    PUCHAR  Port
    );

NTHALAPI
USHORT
READ_PORT_USHORT(
    PUSHORT Port
    );

NTHALAPI
ULONG
READ_PORT_ULONG(
    PULONG  Port
    );

NTHALAPI
VOID
READ_PORT_BUFFER_UCHAR(
    PUCHAR  Port,
    PUCHAR  Buffer,
    ULONG   Count
    );

NTHALAPI
VOID
READ_PORT_BUFFER_USHORT(
    PUSHORT Port,
    PUSHORT Buffer,
    ULONG   Count
    );

NTHALAPI
VOID
READ_PORT_BUFFER_ULONG(
    PULONG  Port,
    PULONG  Buffer,
    ULONG   Count
    );

NTHALAPI
VOID
WRITE_PORT_UCHAR(
    PUCHAR  Port,
    UCHAR   Value
    );

NTHALAPI
VOID
WRITE_PORT_USHORT(
    PUSHORT Port,
    USHORT  Value
    );

NTHALAPI
VOID
WRITE_PORT_ULONG(
    PULONG  Port,
    ULONG   Value
    );

NTHALAPI
VOID
WRITE_PORT_BUFFER_UCHAR(
    PUCHAR  Port,
    PUCHAR  Buffer,
    ULONG   Count
    );

NTHALAPI
VOID
WRITE_PORT_BUFFER_USHORT(
    PUSHORT Port,
    PUSHORT Buffer,
    ULONG   Count
    );

NTHALAPI
VOID
WRITE_PORT_BUFFER_ULONG(
    PULONG  Port,
    PULONG  Buffer,
    ULONG   Count
    );

// end_ntndis
//
// Get data cache fill size.
//

#define KeGetDcacheFillSize() 1L


#define KeFlushIoBuffers(Mdl, ReadOperation, DmaOperation)


#define ExAcquireSpinLock(Lock, OldIrql) KeAcquireSpinLock((Lock), (OldIrql))
#define ExReleaseSpinLock(Lock, OldIrql) KeReleaseSpinLock((Lock), (OldIrql))
#define ExAcquireSpinLockAtDpcLevel(Lock) KeAcquireSpinLockAtDpcLevel(Lock)
#define ExReleaseSpinLockFromDpcLevel(Lock) KeReleaseSpinLockFromDpcLevel(Lock)


#define KeQueryTickCount(CurrentCount ) { \
    volatile PKSYSTEM_TIME _TickCount = *((PKSYSTEM_TIME *)(&KeTickCount)); \
    while (TRUE) {                                                          \
        (CurrentCount)->HighPart = _TickCount->High1Time;                   \
        (CurrentCount)->LowPart = _TickCount->LowPart;                      \
        if ((CurrentCount)->HighPart == _TickCount->High2Time) break;       \
        _asm { rep nop }                                                    \
    }                                                                       \
}


//
// Processor Control Region Structure Definition
//

#define PCR_MINOR_VERSION 1
#define PCR_MAJOR_VERSION 1

typedef struct _KPCR {

//
// Start of the architecturally defined section of the PCR. This section
// may be directly addressed by vendor/platform specific HAL code and will
// not change from version to version of NT.
//

    NT_TIB  NtTib;
    struct _KPCR *SelfPcr;              // flat address of this PCR
    struct _KPRCB *Prcb;                // pointer to Prcb
    KIRQL   Irql;
    ULONG   IRR;
    ULONG   IrrActive;
    ULONG   IDR;
    ULONG   Reserved2;

    struct _KIDTENTRY *IDT;
    struct _KGDTENTRY *GDT;
    struct _KTSS      *TSS;
    USHORT  MajorVersion;
    USHORT  MinorVersion;
    KAFFINITY SetMember;
    ULONG   StallScaleFactor;
    UCHAR   DebugActive;
    UCHAR   Number;

} KPCR;
typedef KPCR *PKPCR;

//
// The non-volatile 387 state
//

typedef struct _KFLOATING_SAVE {
    ULONG   ControlWord;
    ULONG   StatusWord;
    ULONG   ErrorOffset;
    ULONG   ErrorSelector;
    ULONG   DataOffset;                 // Not used in wdm
    ULONG   DataSelector;
    ULONG   Cr0NpxState;
    ULONG   Spare1;                     // Not used in wdm
} KFLOATING_SAVE, *PKFLOATING_SAVE;

//
// i386 Specific portions of mm component
//

//
// Define the page size for the Intel 386 as 4096 (0x1000).
//

#define PAGE_SIZE 0x1000

//
// Define the number of trailing zeroes in a page aligned virtual address.
// This is used as the shift count when shifting virtual addresses to
// virtual page numbers.
//

#define PAGE_SHIFT 12L

// end_ntndis end_wdm
//
// Define the number of bits to shift to right justify the Page Directory Index
// field of a PTE.
//

#define PDI_SHIFT_X86    22
#define PDI_SHIFT_X86PAE 21

#if !defined (_X86PAE_)
#define PDI_SHIFT PDI_SHIFT_X86
#else
#define PDI_SHIFT PDI_SHIFT_X86PAE
#define PPI_SHIFT 30
#endif

//
// Define the number of bits to shift to right justify the Page Table Index
// field of a PTE.
//

#define PTI_SHIFT 12

//
// Define the highest user address and user probe address.
//


extern PVOID *MmHighestUserAddress;
extern PVOID *MmSystemRangeStart;
extern ULONG *MmUserProbeAddress;

#define MM_HIGHEST_USER_ADDRESS *MmHighestUserAddress
#define MM_SYSTEM_RANGE_START *MmSystemRangeStart
#define MM_USER_PROBE_ADDRESS *MmUserProbeAddress

//
// The lowest user address reserves the low 64k.
//

#define MM_LOWEST_USER_ADDRESS (PVOID)0x10000

//
// The lowest address for system space.
//

#if !defined (_X86PAE_)
#define MM_LOWEST_SYSTEM_ADDRESS (PVOID)0xC0800000
#else
#define MM_LOWEST_SYSTEM_ADDRESS (PVOID)0xC0C00000
#endif

// begin_wdm

#define MmGetProcedureAddress(Address) (Address)
#define MmLockPagableCodeSection(Address) MmLockPagableDataSection(Address)


#define KI_USER_SHARED_DATA         0xffdf0000
#define SharedUserData  ((KUSER_SHARED_DATA * const) KI_USER_SHARED_DATA)

//
// Result type definition for i386.  (Machine specific enumerate type
// which is return type for portable exinterlockedincrement/decrement
// procedures.)  In general, you should use the enumerated type defined
// in ex.h instead of directly referencing these constants.
//

// Flags loaded into AH by LAHF instruction

#define EFLAG_SIGN      0x8000
#define EFLAG_ZERO      0x4000
#define EFLAG_SELECT    (EFLAG_SIGN | EFLAG_ZERO)

#define RESULT_NEGATIVE ((EFLAG_SIGN & ~EFLAG_ZERO) & EFLAG_SELECT)
#define RESULT_ZERO     ((~EFLAG_SIGN & EFLAG_ZERO) & EFLAG_SELECT)
#define RESULT_POSITIVE ((~EFLAG_SIGN & ~EFLAG_ZERO) & EFLAG_SELECT)

//
// Convert various portable ExInterlock APIs into their architectural
// equivalents.
//

#define ExInterlockedIncrementLong(Addend,Lock) \
        Exfi386InterlockedIncrementLong(Addend)

#define ExInterlockedDecrementLong(Addend,Lock) \
        Exfi386InterlockedDecrementLong(Addend)

#define ExInterlockedExchangeUlong(Target,Value,Lock) \
        Exfi386InterlockedExchangeUlong(Target,Value)

//  begin_wdm

#define ExInterlockedAddUlong           ExfInterlockedAddUlong
#define ExInterlockedInsertHeadList     ExfInterlockedInsertHeadList
#define ExInterlockedInsertTailList     ExfInterlockedInsertTailList
#define ExInterlockedRemoveHeadList     ExfInterlockedRemoveHeadList
#define ExInterlockedPopEntryList       ExfInterlockedPopEntryList
#define ExInterlockedPushEntryList      ExfInterlockedPushEntryList

//  end_wdm

//
// Prototypes for architectural specific versions of Exi386 Api
//

//
// Interlocked result type is portable, but its values are machine specific.
// Constants for value are in i386.h, mips.h, etc.
//

typedef enum _INTERLOCKED_RESULT {
    ResultNegative = RESULT_NEGATIVE,
    ResultZero     = RESULT_ZERO,
    ResultPositive = RESULT_POSITIVE
} INTERLOCKED_RESULT;

NTKERNELAPI
INTERLOCKED_RESULT
FASTCALL
Exfi386InterlockedIncrementLong (
    IN PLONG Addend
    );

NTKERNELAPI
INTERLOCKED_RESULT
FASTCALL
Exfi386InterlockedDecrementLong (
    IN PLONG Addend
    );

NTKERNELAPI
LARGE_INTEGER
ExInterlockedExchangeAddLargeInteger (
    IN PLARGE_INTEGER Addend,
    IN LARGE_INTEGER Increment,
    IN PKSPIN_LOCK Lock
    );

NTKERNELAPI
ULONG
FASTCALL
Exfi386InterlockedExchangeUlong (
    IN PULONG Target,
    IN ULONG Value
    );

//
// Intrinsic interlocked functions
//

#if (defined(_NTDDK_) || defined(_NTIFS_) || defined(_NTHAL_) || defined(NO_INTERLOCKED_INTRINSICS)) && !defined(_WINBASE_)

//  begin_wdm

NTKERNELAPI
LONG
FASTCALL
InterlockedIncrement(
    IN PLONG Addend
    );

NTKERNELAPI
LONG
FASTCALL
InterlockedDecrement(
    IN PLONG Addend
    );

NTKERNELAPI
LONG
FASTCALL
InterlockedExchange(
    IN OUT PLONG Target,
    IN LONG Value
    );

#define InterlockedExchangePointer(Target, Value) \
   (PVOID)InterlockedExchange((PLONG)(Target), (LONG)(Value))

LONG
FASTCALL
InterlockedExchangeAdd(
    IN OUT PLONG Addend,
    IN LONG Increment
    );

NTKERNELAPI
LONG
FASTCALL
InterlockedCompareExchange(
    IN OUT PLONG Destination,
    IN LONG ExChange,
    IN LONG Comperand
    );

#define InterlockedCompareExchangePointer(Destination, ExChange, Comperand) \
    (PVOID)InterlockedCompareExchange((PLONG)Destination, (LONG)ExChange, (LONG)Comperand)

//  end_wdm

#endif


#if !defined(MIDL_PASS) && defined(_M_IX86)

//
// i386 function definitions
//

#pragma warning(disable:4035)               // re-enable below

    #define _PCR   fs:[0]                   

//
// Get current IRQL.
//
// On x86 this function resides in the HAL
//

NTHALAPI
KIRQL
KeGetCurrentIrql();

// end_wdm
//
// Get the current processor number
//

__inline ULONG KeGetCurrentProcessorNumber(VOID)
{
    __asm {  movzx eax, _PCR KPCR.Number  }
}


#endif // !defined(MIDL_PASS) && defined(_M_IX86)


NTKERNELAPI
NTSTATUS
NTAPI
KeSaveFloatingPointState (
    OUT PKFLOATING_SAVE     FloatSave
    );

NTKERNELAPI
NTSTATUS
NTAPI
KeRestoreFloatingPointState (
    IN PKFLOATING_SAVE      FloatSave
    );


#endif // defined(_X86_)


// Use the following for kernel mode runtime checks of X86 system architecture

#ifdef _X86_

#ifdef IsNEC_98
#undef IsNEC_98
#endif

#ifdef IsNotNEC_98
#undef IsNotNEC_98
#endif

#ifdef SetNEC_98
#undef SetNEC_98
#endif

#ifdef SetNotNEC_98
#undef SetNotNEC_98
#endif

#define IsNEC_98     (SharedUserData->AlternativeArchitecture == NEC98x86)
#define IsNotNEC_98  (SharedUserData->AlternativeArchitecture != NEC98x86)
#define SetNEC_98    SharedUserData->AlternativeArchitecture = NEC98x86
#define SetNotNEC_98 SharedUserData->AlternativeArchitecture = StandardDesign

#endif


#if defined(_ALPHA_)
#ifdef __cplusplus
extern "C" {
#endif

//
// Types to use to contain PFNs and their counts.
//

typedef ULONG PFN_COUNT;

typedef LONG_PTR SPFN_NUMBER, *PSPFN_NUMBER;
typedef ULONG_PTR PFN_NUMBER, *PPFN_NUMBER;

//
// Define maximum size of flush multiple TB request.
//

#define FLUSH_MULTIPLE_MAXIMUM 16

//
// Indicate that the Alpha compiler supports the pragma textout construct.
//

#define ALLOC_PRAGMA 1

// end_ntndis
//
// Include the Alpha instruction definitions
//

#include "alphaops.h"

//
// Include reference machine definitions.
//

#include "alpharef.h"

//
// Define macro to generate import names.
//

#define IMPORT_NAME(name) __imp_##name

//
// Define length of interrupt vector table.
//

#define MAXIMUM_VECTOR 256

//
// Define bus error routine type.
//

struct _EXCEPTION_RECORD;
struct _KEXCEPTION_FRAME;
struct _KTRAP_FRAME;

typedef
BOOLEAN
(*PKBUS_ERROR_ROUTINE) (
    IN struct _EXCEPTION_RECORD *ExceptionRecord,
    IN struct _KEXCEPTION_FRAME *ExceptionFrame,
    IN struct _KTRAP_FRAME *TrapFrame
    );


#define PCR_MINOR_VERSION 1
#define PCR_MAJOR_VERSION 1

typedef struct _KPCR {

//
// Major and minor version numbers of the PCR.
//

    ULONG MinorVersion;
    ULONG MajorVersion;

//
// Start of the architecturally defined section of the PCR. This section
// may be directly addressed by vendor/platform specific PAL/HAL code and will
// not change from version to version of NT.

//
// PALcode information.
//

    ULONGLONG PalBaseAddress;
    ULONG PalMajorVersion;
    ULONG PalMinorVersion;
    ULONG PalSequenceVersion;
    ULONG PalMajorSpecification;
    ULONG PalMinorSpecification;

//
// Firmware restart information.
//

    ULONGLONG FirmwareRestartAddress;
    PVOID RestartBlock;

//
// Reserved per-processor region for the PAL (3K-8 bytes).
//

    ULONGLONG PalReserved[383];

//
// Alignment fixup count updated by PAL and read by kernel.
//

    ULONGLONG PalAlignmentFixupCount;

//
// Panic Stack Address.
//

    PVOID PanicStack;

//
// Processor parameters.
//

    ULONG ProcessorType;
    ULONG ProcessorRevision;
    ULONG PhysicalAddressBits;
    ULONG MaximumAddressSpaceNumber;
    ULONG PageSize;
    ULONG FirstLevelDcacheSize;
    ULONG FirstLevelDcacheFillSize;
    ULONG FirstLevelIcacheSize;
    ULONG FirstLevelIcacheFillSize;

//
// System Parameters.
//

    ULONG FirmwareRevisionId;
    UCHAR SystemType[8];
    ULONG SystemVariant;
    ULONG SystemRevision;
    UCHAR SystemSerialNumber[16];
    ULONG CycleClockPeriod;
    ULONG SecondLevelCacheSize;
    ULONG SecondLevelCacheFillSize;
    ULONG ThirdLevelCacheSize;
    ULONG ThirdLevelCacheFillSize;
    ULONG FourthLevelCacheSize;
    ULONG FourthLevelCacheFillSize;

//
// Pointer to processor control block.
//

    struct _KPRCB *Prcb;

//
// Processor identification.
//

    CCHAR Number;
    KAFFINITY SetMember;

//
// Reserved per-processor region for the HAL (.5K bytes).
//

    ULONGLONG HalReserved[64];

//
// IRQL mapping tables.
//

    ULONG IrqlTable[8];

#define SFW_IMT_ENTRIES 4
#define HDW_IMT_ENTRIES 128

    struct _IRQLMASK {
        USHORT IrqlTableIndex;   // synchronization irql level
        USHORT IDTIndex;         // vector in IDT
    } IrqlMask[SFW_IMT_ENTRIES + HDW_IMT_ENTRIES];

//
// Interrupt Dispatch Table (IDT).
//

    PKINTERRUPT_ROUTINE InterruptRoutine[MAXIMUM_VECTOR];

//
// Reserved vectors mask, these vectors cannot be attached to via
// standard interrupt objects.
//

    ULONG ReservedVectors;

//
// Complement of processor affinity mask.
//

    KAFFINITY NotMember;

    ULONG InterruptInProgress;
    ULONG DpcRequested;

//
// Pointer to machine check handler
//

    PKBUS_ERROR_ROUTINE MachineCheckError;

//
// DPC Stack.
//

    PVOID DpcStack;

//
// End of the architecturally defined section of the PCR. This section
// may be directly addressed by vendor/platform specific HAL code and will
// not change from version to version of NT.  Some of these values are
// reserved for chip-specific palcode.
} KPCR, *PKPCR; 

#if defined(_AXP64_)

#define KI_USER_SHARED_DATA 0xffffffffff000000UI64

#else

#define KI_USER_SHARED_DATA 0xff000000UL

#endif

#define SharedUserData ((KUSER_SHARED_DATA * const) KI_USER_SHARED_DATA)

// begin_wdm
//
// length of dispatch code in interrupt template
//
#define DISPATCH_LENGTH 4

//
// Define IRQL levels across the architecture.
//

#define PASSIVE_LEVEL   0
#define LOW_LEVEL       0
#define APC_LEVEL       1
#define DISPATCH_LEVEL  2
#define HIGH_LEVEL      7
#define SYNCH_LEVEL (IPI_LEVEL-1)

//
// Non-volatile floating point state
//

typedef struct _KFLOATING_SAVE {
    ULONGLONG   Fpcr;
    ULONGLONG   SoftFpcr;
    ULONG       Reserved1;              // These reserved words are here to make it
    ULONG       Reserved2;              // the same size as i386/WDM.
    ULONG       Reserved3;
    ULONG       Reserved4;
} KFLOATING_SAVE, *PKFLOATING_SAVE;

//
// Processor Control Block (PRCB)
//

#define PRCB_MINOR_VERSION 1
#define PRCB_MAJOR_VERSION 2
#define PRCB_BUILD_DEBUG        0x0001
#define PRCB_BUILD_UNIPROCESSOR 0x0002

typedef struct _KPRCB {

//
// Major and minor version numbers of the PCR.
//

    USHORT MinorVersion;
    USHORT MajorVersion;

//
// Start of the architecturally defined section of the PRCB. This section
// may be directly addressed by vendor/platform specific HAL code and will
// not change from version to version of NT.
//

    struct _KTHREAD *CurrentThread;
    struct _KTHREAD *NextThread;
    struct _KTHREAD *IdleThread;
    CCHAR Number;
    CCHAR Reserved;
    USHORT BuildType;
    KAFFINITY SetMember;
    struct _RESTART_BLOCK *RestartBlock;

//
// End of the architecturally defined section of the PRCB. This section
// may be directly addressed by vendor/platform specific HAL code and will
// not change from version to version of NT.
//
} KPRCB, *PKPRCB, *RESTRICTED_POINTER PRKPRCB;      
//
// I/O space read and write macros.
//
//  These have to be actual functions on Alpha, because we need
//  to shift the VA and OR in the BYTE ENABLES.
//
//  These can become INLINEs if we require that ALL Alpha systems shift
//  the same number of bits and have the SAME byte enables.
//
//  The READ/WRITE_REGISTER_* calls manipulate I/O registers in MEMORY space?
//
//  The READ/WRITE_PORT_* calls manipulate I/O registers in PORT space?
//

NTHALAPI
UCHAR
READ_REGISTER_UCHAR(
    PUCHAR Register
    );

NTHALAPI
USHORT
READ_REGISTER_USHORT(
    PUSHORT Register
    );

NTHALAPI
ULONG
READ_REGISTER_ULONG(
    PULONG Register
    );

NTHALAPI
VOID
READ_REGISTER_BUFFER_UCHAR(
    PUCHAR  Register,
    PUCHAR  Buffer,
    ULONG   Count
    );

NTHALAPI
VOID
READ_REGISTER_BUFFER_USHORT(
    PUSHORT Register,
    PUSHORT Buffer,
    ULONG   Count
    );

NTHALAPI
VOID
READ_REGISTER_BUFFER_ULONG(
    PULONG  Register,
    PULONG  Buffer,
    ULONG   Count
    );


NTHALAPI
VOID
WRITE_REGISTER_UCHAR(
    PUCHAR Register,
    UCHAR   Value
    );

NTHALAPI
VOID
WRITE_REGISTER_USHORT(
    PUSHORT Register,
    USHORT  Value
    );

NTHALAPI
VOID
WRITE_REGISTER_ULONG(
    PULONG Register,
    ULONG   Value
    );

NTHALAPI
VOID
WRITE_REGISTER_BUFFER_UCHAR(
    PUCHAR  Register,
    PUCHAR  Buffer,
    ULONG   Count
    );

NTHALAPI
VOID
WRITE_REGISTER_BUFFER_USHORT(
    PUSHORT Register,
    PUSHORT Buffer,
    ULONG   Count
    );

NTHALAPI
VOID
WRITE_REGISTER_BUFFER_ULONG(
    PULONG  Register,
    PULONG  Buffer,
    ULONG   Count
    );

NTHALAPI
UCHAR
READ_PORT_UCHAR(
    PUCHAR Port
    );

NTHALAPI
USHORT
READ_PORT_USHORT(
    PUSHORT Port
    );

NTHALAPI
ULONG
READ_PORT_ULONG(
    PULONG  Port
    );

NTHALAPI
VOID
READ_PORT_BUFFER_UCHAR(
    PUCHAR  Port,
    PUCHAR  Buffer,
    ULONG   Count
    );

NTHALAPI
VOID
READ_PORT_BUFFER_USHORT(
    PUSHORT Port,
    PUSHORT Buffer,
    ULONG   Count
    );

NTHALAPI
VOID
READ_PORT_BUFFER_ULONG(
    PULONG  Port,
    PULONG  Buffer,
    ULONG   Count
    );

NTHALAPI
VOID
WRITE_PORT_UCHAR(
    PUCHAR  Port,
    UCHAR   Value
    );

NTHALAPI
VOID
WRITE_PORT_USHORT(
    PUSHORT Port,
    USHORT  Value
    );

NTHALAPI
VOID
WRITE_PORT_ULONG(
    PULONG  Port,
    ULONG   Value
    );

NTHALAPI
VOID
WRITE_PORT_BUFFER_UCHAR(
    PUCHAR  Port,
    PUCHAR  Buffer,
    ULONG   Count
    );

NTHALAPI
VOID
WRITE_PORT_BUFFER_USHORT(
    PUSHORT Port,
    PUSHORT Buffer,
    ULONG   Count
    );

NTHALAPI
VOID
WRITE_PORT_BUFFER_ULONG(
    PULONG  Port,
    PULONG  Buffer,
    ULONG   Count
    );

// end_ntndis end_wdm
//
// Define Interlocked operation result values.
//

#define RESULT_ZERO 0
#define RESULT_NEGATIVE 1
#define RESULT_POSITIVE 2

//
// Interlocked result type is portable, but its values are machine specific.
// Constants for value are in i386.h, mips.h, etc.
//

typedef enum _INTERLOCKED_RESULT {
    ResultNegative = RESULT_NEGATIVE,
    ResultZero     = RESULT_ZERO,
    ResultPositive = RESULT_POSITIVE
} INTERLOCKED_RESULT;

//
// Convert portable interlock interfaces to architecture specific interfaces.
//

#define ExInterlockedIncrementLong(Addend, Lock) \
    ExAlphaInterlockedIncrementLong(Addend)

#define ExInterlockedDecrementLong(Addend, Lock) \
    ExAlphaInterlockedDecrementLong(Addend)

#define ExInterlockedExchangeAddLargeInteger(Target, Value, Lock) \
    ExpInterlockedExchangeAddLargeInteger(Target, Value)

#define ExInterlockedExchangeUlong(Target, Value, Lock) \
    ExAlphaInterlockedExchangeUlong(Target, Value)

NTKERNELAPI
INTERLOCKED_RESULT
ExAlphaInterlockedIncrementLong (
    IN PLONG Addend
    );

NTKERNELAPI
INTERLOCKED_RESULT
ExAlphaInterlockedDecrementLong (
    IN PLONG Addend
    );

NTKERNELAPI
LARGE_INTEGER
ExpInterlockedExchangeAddLargeInteger (
    IN PLARGE_INTEGER Addend,
    IN LARGE_INTEGER Increment
    );

NTKERNELAPI
ULONG
ExAlphaInterlockedExchangeUlong (
    IN PULONG Target,
    IN ULONG Value
    );

//  begin_wdm

#if defined(_M_ALPHA) && !defined(RC_INVOKED)

#define InterlockedIncrement _InterlockedIncrement
#define InterlockedDecrement _InterlockedDecrement
#define InterlockedExchange _InterlockedExchange
#define InterlockedExchangeAdd _InterlockedExchangeAdd

LONG
InterlockedIncrement (
    IN OUT PLONG Addend
    );

LONG
InterlockedDecrement (
    IN OUT PLONG Addend
    );

LONG
InterlockedExchange (
    IN OUT PLONG Target,
    LONG Value
    );

#if defined(_M_AXP64)

#define InterlockedCompareExchange _InterlockedCompareExchange
#define InterlockedCompareExchange64 _InterlockedCompareExchange64
#define InterlockedExchangePointer _InterlockedExchangePointer
#define InterlockedCompareExchangePointer _InterlockedCompareExchangePointer
#define InterlockedExchange64 _InterlockedExchange64

LONG
InterlockedCompareExchange (
    IN OUT PLONG Destination,
    IN LONG ExChange,
    IN LONG Comperand
    );

LONGLONG
InterlockedCompareExchange64 (
    IN OUT PLONGLONG Destination,
    IN LONGLONG ExChange,
    IN LONGLONG Comperand
    );

PVOID
InterlockedExchangePointer (
    IN OUT PVOID *Target,
    IN PVOID Value
    );

PVOID
InterlockedCompareExchangePointer (
    IN OUT PVOID *Destination,
    IN PVOID ExChange,
    IN PVOID Comperand
    );

LONGLONG
InterlockedExchange64(
    IN OUT PLONGLONG Target,
    IN LONGLONG Value
    );

#pragma intrinsic(_InterlockedCompareExchange64)
#pragma intrinsic(_InterlockedExchangePointer)
#pragma intrinsic(_InterlockedCompareExchangePointer)
#pragma intrinsic(_InterlockedExchange64)

#else

#define InterlockedExchangePointer(Target, Value) \
    (PVOID)InterlockedExchange((PLONG)(Target), (LONG)(Value))

#define InterlockedCompareExchange(Destination, ExChange, Comperand) \
    (LONG)_InterlockedCompareExchange((PVOID *)(Destination), (PVOID)(ExChange), (PVOID)(Comperand))

#define InterlockedCompareExchangePointer(Destination, ExChange, Comperand) \
    _InterlockedCompareExchange(Destination, ExChange, Comperand)

PVOID
_InterlockedCompareExchange (
    IN OUT PVOID *Destination,
    IN PVOID ExChange,
    IN PVOID Comperand
    );

NTKERNELAPI
LONGLONG
ExpInterlockedCompareExchange64 (
    IN OUT PLONGLONG Destination,
    IN PLONGLONG Exchange,
    IN PLONGLONG Comperand
    );

#endif

LONG
InterlockedExchangeAdd(
    IN OUT PLONG Addend,
    IN LONG Value
    );

#pragma intrinsic(_InterlockedIncrement)
#pragma intrinsic(_InterlockedDecrement)
#pragma intrinsic(_InterlockedExchange)
#pragma intrinsic(_InterlockedExchangeAdd)
#pragma intrinsic(_InterlockedCompareExchange)

#endif

// there is a lot of other stuff that could go in here
//   probe macros
//   others

//
// Define the page size for the Alpha ev4 and lca as 8k.
//

#define PAGE_SIZE 0x2000

//
// Define the number of trailing zeroes in a page aligned virtual address.
// This is used as the shift count when shifting virtual addresses to
// virtual page numbers.
//

#define PAGE_SHIFT 13L


//
// The highest user address reserves 64K bytes for a guard page. This is so
// the probing of addresses from kernel mode only have to check the
// starting address for structures of 64K bytes or less.
//

#if defined(_AXP64_)

#define MM_HIGHEST_USER_ADDRESS (PVOID)0x3FFFFFEFFFF // highest user address
#define MM_USER_PROBE_ADDRESS          0x3FFFFFF0000UI64 // guard page address
#define MM_SYSTEM_RANGE_START   (PVOID)0xFFFFFC0000000000 // start of system space

#else

#define MM_HIGHEST_USER_ADDRESS (PVOID)0x7FFEFFFF // highest user address
#define MM_USER_PROBE_ADDRESS 0x7FFF0000 // starting address of guard page
#define MM_SYSTEM_RANGE_START (PVOID)KSEG0_BASE // start of system space

#endif


//
// The following definitions are required for the debugger data block.
//

extern PVOID MmHighestUserAddress;
extern PVOID MmSystemRangeStart;
extern ULONG_PTR MmUserProbeAddress;

//
// The lowest user address reserves the low 64k.
//

#define MM_LOWEST_USER_ADDRESS  (PVOID)0x00010000

// begin_wdm

#define MmGetProcedureAddress(Address) (Address)
#define MmLockPagableCodeSection(Address) MmLockPagableDataSection(Address)

//
// The lowest address for system space.
//

#if defined(_AXP64_)

#define MM_LOWEST_SYSTEM_ADDRESS (PVOID)0xFFFFFE0200000000

#else

#define MM_LOWEST_SYSTEM_ADDRESS (PVOID)0xC0800000

#endif


//
// Define prototypes to access PCR values
//

NTKERNELAPI
KIRQL
KeGetCurrentIrql();


NTSTATUS
KeSaveFloatingPointState (
    OUT PKFLOATING_SAVE     FloatSave
    );

NTSTATUS
KeRestoreFloatingPointState (
    IN PKFLOATING_SAVE      FloatSave
    );

//
// Get address of current PRCB.
//

#define KeGetCurrentPrcb() (PCR->Prcb)

//
// Get current processor number.
//

#define KeGetCurrentProcessorNumber() KeGetCurrentPrcb()->Number

//
// Cache and write buffer flush functions.
//

VOID
KeFlushIoBuffers (
    IN PMDL Mdl,
    IN BOOLEAN ReadOperation,
    IN BOOLEAN DmaOperation
    );


#define KeQueryTickCount(CurrentCount ) \
    *(PULONGLONG)(CurrentCount) = **((volatile ULONGLONG **)(&KeTickCount));


#define ExAcquireSpinLock(Lock, OldIrql) KeAcquireSpinLock((Lock), (OldIrql))
#define ExReleaseSpinLock(Lock, OldIrql) KeReleaseSpinLock((Lock), (OldIrql))
#define ExAcquireSpinLockAtDpcLevel(Lock) KeAcquireSpinLockAtDpcLevel(Lock)
#define ExReleaseSpinLockFromDpcLevel(Lock) KeReleaseSpinLockFromDpcLevel(Lock)

#ifdef __cplusplus
}   // extern "C"
#endif
#endif // _ALPHA_

#if defined(_IA64_)

//
// Types to use to contain PFNs and their counts.
//

typedef ULONG PFN_COUNT;

typedef LONG_PTR SPFN_NUMBER, *PSPFN_NUMBER;
typedef ULONG_PTR PFN_NUMBER, *PPFN_NUMBER;    

//
// Define maximum size of flush multiple TB request.
//

#define FLUSH_MULTIPLE_MAXIMUM 100

//
// Indicate that the IA64 compiler supports the pragma textout construct.
//

#define ALLOC_PRAGMA 1

//
// Define intrinsic calls and their prototypes
//

#include "ia64reg.h"

// Please contact INTEL to get IA64-specific information

// end_wdm end_ntndis

//
// Define macro to generate import names.
//

#define IMPORT_NAME(name) __imp_##name

// begin_wdm

//
// Define length of interrupt vector table.
//

// Please contact INTEL to get IA64-specific information
#define MAXIMUM_VECTOR 256

// end_wdm


//
// IA64 specific interlocked operation result values.
//

#define RESULT_ZERO 0
#define RESULT_NEGATIVE 1
#define RESULT_POSITIVE 2

//
// Interlocked result type is portable, but its values are machine specific.
// Constants for values are in i386.h, mips.h, etc.
//

typedef enum _INTERLOCKED_RESULT {
    ResultNegative = RESULT_NEGATIVE,
    ResultZero     = RESULT_ZERO,
    ResultPositive = RESULT_POSITIVE
} INTERLOCKED_RESULT;

//
// Convert portable interlock interfaces to architecture specific interfaces.
//

#define ExInterlockedIncrementLong(Addend, Lock) \
    ExIa64InterlockedIncrementLong(Addend)

#define ExInterlockedDecrementLong(Addend, Lock) \
    ExIa64InterlockedDecrementLong(Addend)

#define ExInterlockedExchangeAddLargeInteger(Target, Value, Lock) \
    ExpInterlockedExchangeAddLargeInteger(Target, Value)

#define ExInterlockedExchangeUlong(Target, Value, Lock) \
    ExIa64InterlockedExchangeUlong(Target, Value)

NTKERNELAPI
INTERLOCKED_RESULT
ExIa64InterlockedIncrementLong (
    IN PLONG Addend
    );

NTKERNELAPI
INTERLOCKED_RESULT
ExIa64InterlockedDecrementLong (
    IN PLONG Addend
    );

NTKERNELAPI
LARGE_INTEGER
ExpInterlockedExchangeAddLargeInteger (
    IN PLARGE_INTEGER Addend,
    IN LARGE_INTEGER Increment
    );

NTKERNELAPI
ULONG
ExIa64InterlockedExchangeUlong (
    IN PULONG Target,
    IN ULONG Value
    );

// begin_wdm

//
// IA64 Interrupt Definitions.
//
// Define length of interrupt object dispatch code in longwords.
//

// Please contact INTEL to get IA64-specific information

//
// Begin of a block of definitions that must be synchronized with kxia64.h.
//

//
// Define Interrupt Request Levels.
//

#define PASSIVE_LEVEL            0      // Passive release level
#define LOW_LEVEL                0      // Lowest interrupt level
#define APC_LEVEL                1      // APC interrupt level
#define DISPATCH_LEVEL           2      // Dispatcher level
#define CMC_LEVEL                3      // Correctable machine check level
#define DEVICE_LEVEL_BASE        4      // 4 - 11 - Device IRQLs
#define PROFILE_LEVEL           12      // Profiling level
#define PC_LEVEL                12      // Performance Counter IRQL
#define SYNCH_LEVEL             (IPI_LEVEL-1)      // Synchronization level
#define IPI_LEVEL               14      // IPI IRQL
#define CLOCK_LEVEL             13      // Clock Timer IRQL
#define POWER_LEVEL             15      // Power failure level
#define HIGH_LEVEL              15      // Highest interrupt level

// Please contact INTEL to get IA64-specific information

//
// End of a block of definitions that must be synchronized with kxia64.h.
//

//
// Define profile intervals.
//

#define DEFAULT_PROFILE_COUNT 0x40000000 // ~= 20 seconds @50mhz
#define DEFAULT_PROFILE_INTERVAL (10 * 500) // 500 microseconds
#define MAXIMUM_PROFILE_INTERVAL (10 * 1000 * 1000) // 1 second
#define MINIMUM_PROFILE_INTERVAL (10 * 40) // 40 microseconds

#if defined(_M_IA64) && !defined(RC_INVOKED)

#define InterlockedAdd _InterlockedAdd
#define InterlockedIncrement _InterlockedIncrement
#define InterlockedDecrement _InterlockedDecrement
#define InterlockedExchange _InterlockedExchange
#define InterlockedExchangeAdd _InterlockedExchangeAdd

#define InterlockedAdd64 _InterlockedAdd64
#define InterlockedIncrement64 _InterlockedIncrement64
#define InterlockedDecrement64 _InterlockedDecrement64
#define InterlockedExchange64 _InterlockedExchange64
#define InterlockedExchangeAdd64 _InterlockedExchangeAdd64
#define InterlockedCompareExchange64 _InterlockedCompareExchange64

#define InterlockedCompareExchange _InterlockedCompareExchange
#define InterlockedExchangePointer _InterlockedExchangePointer
#define InterlockedCompareExchangePointer _InterlockedCompareExchangePointer

LONG
__cdecl
InterlockedAdd (
    LONG *Addend,
    LONG Value
    );

LONGLONG
__cdecl
InterlockedAdd64 (
    LONGLONG *Addend,
    LONGLONG Value
    );

LONG
__cdecl
InterlockedIncrement(
    IN OUT PLONG Addend
    );

LONG
__cdecl
InterlockedDecrement(
    IN OUT PLONG Addend
    );

LONG
__cdecl
InterlockedExchange(
    IN OUT PLONG Target,
    IN LONG Value
    );

LONG
__cdecl
InterlockedExchangeAdd(
    IN OUT PLONG Addend,
    IN LONG Value
    );

LONG
__cdecl
InterlockedCompareExchange (
    IN OUT PLONG Destination,
    IN LONG ExChange,
    IN LONG Comperand
    );

LONGLONG
__cdecl
InterlockedIncrement64(
    IN OUT PLONGLONG Addend
    );

LONGLONG
__cdecl
InterlockedDecrement64(
    IN OUT PLONGLONG Addend
    );

LONGLONG
__cdecl
InterlockedExchange64(
    IN OUT PLONGLONG Target,
    IN LONGLONG Value
    );

LONGLONG
__cdecl
InterlockedExchangeAdd64(
    IN OUT PLONGLONG Addend,
    IN LONGLONG Value
    );

LONGLONG
__cdecl
InterlockedCompareExchange64 (
    IN OUT PLONGLONG Destination,
    IN LONGLONG ExChange,
    IN LONGLONG Comperand
    );

PVOID
__cdecl
InterlockedCompareExchangePointer (
    IN OUT PVOID *Destination,
    IN PVOID Exchange,
    IN PVOID Comperand
    );

PVOID
__cdecl
InterlockedExchangePointer(
    IN OUT PVOID *Target,
    IN PVOID Value
    );

#pragma intrinsic(_InterlockedAdd)
#pragma intrinsic(_InterlockedIncrement)
#pragma intrinsic(_InterlockedDecrement)
#pragma intrinsic(_InterlockedExchange)
#pragma intrinsic(_InterlockedCompareExchange)
#pragma intrinsic(_InterlockedExchangeAdd)
#pragma intrinsic(_InterlockedAdd64)
#pragma intrinsic(_InterlockedIncrement64)
#pragma intrinsic(_InterlockedDecrement64)
#pragma intrinsic(_InterlockedExchange64)
#pragma intrinsic(_InterlockedCompareExchange64)
#pragma intrinsic(_InterlockedExchangeAdd64)
#pragma intrinsic(_InterlockedExchangePointer)
#pragma intrinsic(_InterlockedCompareExchangePointer)

#endif // defined(_M_IA64) && !defined(RC_INVOKED)


#define KI_USER_SHARED_DATA ((ULONG_PTR)(KADDRESS_BASE + 0xFFFE0000))
#define SharedUserData ((KUSER_SHARED_DATA * const)KI_USER_SHARED_DATA)

//
// Prototype for get current IRQL. **** TBD (read TPR)
//

NTKERNELAPI
KIRQL
KeGetCurrentIrql();

// end_wdm

//
// Get address of current processor block.
//

#define KeGetCurrentPrcb() PCR->Prcb

//
// Get address of processor control region.
//

#define KeGetPcr() PCR

//
// Get address of current kernel thread object.
//

#define KeGetCurrentThread() PCR->CurrentThread

//
// Get current processor number.
//

#define KeGetCurrentProcessorNumber() PCR->Number

//
// Get data cache fill size.
//

#define KeGetDcacheFillSize() PCR->DcacheFillSize


#define KeSaveFloatingPointState(a)         STATUS_SUCCESS
#define KeRestoreFloatingPointState(a)      STATUS_SUCCESS


//
// Define the page size
//

#define PAGE_SIZE 0x2000

//
// Define the number of trailing zeroes in a page aligned virtual address.
// This is used as the shift count when shifting virtual addresses to
// virtual page numbers.
//

#define PAGE_SHIFT 13L

//
// Cache and write buffer flush functions.
//

NTKERNELAPI
VOID
KeFlushIoBuffers (
    IN PMDL Mdl,
    IN BOOLEAN ReadOperation,
    IN BOOLEAN DmaOperation
    );


//
// Kernel breakin breakpoint
//

VOID
KeBreakinBreakpoint (
    VOID
    );


#define ExAcquireSpinLock(Lock, OldIrql) KeAcquireSpinLock((Lock), (OldIrql))
#define ExReleaseSpinLock(Lock, OldIrql) KeReleaseSpinLock((Lock), (OldIrql))
#define ExAcquireSpinLockAtDpcLevel(Lock) KeAcquireSpinLockAtDpcLevel(Lock)
#define ExReleaseSpinLockFromDpcLevel(Lock) KeReleaseSpinLockFromDpcLevel(Lock)


#if defined(_NTDRIVER_) || defined(_NTDDK_) || defined(_NTIFS_)

// begin_wdm

#define KeQueryTickCount(CurrentCount ) \
    *(PULONGLONG)(CurrentCount) = **((volatile ULONGLONG **)(&KeTickCount));

// end_wdm

#else

#define KiQueryTickCount(CurrentCount) \
    *(PULONGLONG)(CurrentCount) = KeTickCount;

NTKERNELAPI
VOID
KeQueryTickCount (
    OUT PLARGE_INTEGER CurrentCount
    );

#endif // defined(_NTDRIVER_) || defined(_NTDDK_) || defined(_NTIFS_)

//
// I/O space read and write macros.
//

NTHALAPI
UCHAR
READ_PORT_UCHAR (
    PUCHAR RegisterAddress
    );

NTHALAPI
USHORT
READ_PORT_USHORT (
    PUSHORT RegisterAddress
    );

NTHALAPI
ULONG
READ_PORT_ULONG (
    PULONG RegisterAddress
    );

NTHALAPI
VOID
READ_PORT_BUFFER_UCHAR (
    PUCHAR portAddress,
    PUCHAR readBuffer,
    ULONG  readCount
    );

NTHALAPI
VOID
READ_PORT_BUFFER_USHORT (
    PUSHORT portAddress,
    PUSHORT readBuffer,
    ULONG  readCount
    );

NTHALAPI
VOID
READ_PORT_BUFFER_ULONG (
    PULONG portAddress,
    PULONG readBuffer,
    ULONG  readCount
    );

NTHALAPI
VOID
WRITE_PORT_UCHAR (
    PUCHAR portAddress,
    UCHAR  Data
    );

NTHALAPI
VOID
WRITE_PORT_USHORT (
    PUSHORT portAddress,
    USHORT  Data
    );

NTHALAPI
VOID
WRITE_PORT_ULONG (
    PULONG portAddress,
    ULONG  Data
    );

NTHALAPI
VOID
WRITE_PORT_BUFFER_UCHAR (
    PUCHAR portAddress,
    PUCHAR writeBuffer,
    ULONG  writeCount
    );

NTHALAPI
VOID
WRITE_PORT_BUFFER_USHORT (
    PUSHORT portAddress,
    PUSHORT writeBuffer,
    ULONG  writeCount
    );

NTHALAPI
VOID
WRITE_PORT_BUFFER_ULONG (
    PULONG portAddress,
    PULONG writeBuffer,
    ULONG  writeCount
    );


#define READ_REGISTER_UCHAR(x) \
    (__mf(), *(volatile UCHAR * const)(x))

#define READ_REGISTER_USHORT(x) \
    (__mf(), *(volatile USHORT * const)(x))

#define READ_REGISTER_ULONG(x) \
    (__mf(), *(volatile ULONG * const)(x))

#define READ_REGISTER_BUFFER_UCHAR(x, y, z) {                           \
    PUCHAR registerBuffer = x;                                          \
    PUCHAR readBuffer = y;                                              \
    ULONG readCount;                                                    \
    __mf();                                                             \
    for (readCount = z; readCount--; readBuffer++, registerBuffer++) {  \
        *readBuffer = *(volatile UCHAR * const)(registerBuffer);        \
    }                                                                   \
}

#define READ_REGISTER_BUFFER_USHORT(x, y, z) {                          \
    PUSHORT registerBuffer = x;                                         \
    PUSHORT readBuffer = y;                                             \
    ULONG readCount;                                                    \
    __mf();                                                             \
    for (readCount = z; readCount--; readBuffer++, registerBuffer++) {  \
        *readBuffer = *(volatile USHORT * const)(registerBuffer);       \
    }                                                                   \
}

#define READ_REGISTER_BUFFER_ULONG(x, y, z) {                           \
    PULONG registerBuffer = x;                                          \
    PULONG readBuffer = y;                                              \
    ULONG readCount;                                                    \
    __mf();                                                             \
    for (readCount = z; readCount--; readBuffer++, registerBuffer++) {  \
        *readBuffer = *(volatile ULONG * const)(registerBuffer);        \
    }                                                                   \
}

#define WRITE_REGISTER_UCHAR(x, y) {    \
    *(volatile UCHAR * const)(x) = y;   \
    KeFlushWriteBuffer();               \
}

#define WRITE_REGISTER_USHORT(x, y) {   \
    *(volatile USHORT * const)(x) = y;  \
    KeFlushWriteBuffer();               \
}

#define WRITE_REGISTER_ULONG(x, y) {    \
    *(volatile ULONG * const)(x) = y;   \
    KeFlushWriteBuffer();               \
}

#define WRITE_REGISTER_BUFFER_UCHAR(x, y, z) {                            \
    PUCHAR registerBuffer = x;                                            \
    PUCHAR writeBuffer = y;                                               \
    ULONG writeCount;                                                     \
    for (writeCount = z; writeCount--; writeBuffer++, registerBuffer++) { \
        *(volatile UCHAR * const)(registerBuffer) = *writeBuffer;         \
    }                                                                     \
    KeFlushWriteBuffer();                                                 \
}

#define WRITE_REGISTER_BUFFER_USHORT(x, y, z) {                           \
    PUSHORT registerBuffer = x;                                           \
    PUSHORT writeBuffer = y;                                              \
    ULONG writeCount;                                                     \
    for (writeCount = z; writeCount--; writeBuffer++, registerBuffer++) { \
        *(volatile USHORT * const)(registerBuffer) = *writeBuffer;        \
    }                                                                     \
    KeFlushWriteBuffer();                                                 \
}

#define WRITE_REGISTER_BUFFER_ULONG(x, y, z) {                            \
    PULONG registerBuffer = x;                                            \
    PULONG writeBuffer = y;                                               \
    ULONG writeCount;                                                     \
    for (writeCount = z; writeCount--; writeBuffer++, registerBuffer++) { \
        *(volatile ULONG * const)(registerBuffer) = *writeBuffer;         \
    }                                                                     \
    KeFlushWriteBuffer();                                                 \
}

//
// Non-volatile floating point state
//

typedef struct _KFLOATING_SAVE {
    ULONG   Reserved;
} KFLOATING_SAVE, *PKFLOATING_SAVE;

//
// STATUS register for each MCA bank.
//

typedef union _MCI_STATS {
    struct {
        USHORT  McaCod;
        USHORT  MsCod;
        ULONG   OtherInfo : 25;
        ULONG   Damage : 1;
        ULONG   AddressValid : 1;
        ULONG   MiscValid : 1;
        ULONG   Enabled : 1;
        ULONG   UnCorrected : 1;
        ULONG   OverFlow : 1;
        ULONG   Valid : 1;
    } MciStats;

    ULONGLONG QuadPart;

} MCI_STATS, *PMCI_STATS;


//
// Processor Control Block (PRCB)
//

#define PRCB_MINOR_VERSION 1
#define PRCB_MAJOR_VERSION 1
#define PRCB_BUILD_DEBUG        0x0001
#define PRCB_BUILD_UNIPROCESSOR 0x0002

struct _RESTART_BLOCK;

typedef struct _KPRCB {

//
// Major and minor version numbers of the PCR.
//

    USHORT MinorVersion;
    USHORT MajorVersion;

//
// Start of the architecturally defined section of the PRCB. This section
// may be directly addressed by vendor/platform specific HAL code and will
// not change from version to version of NT.
//
//

    struct _KTHREAD *CurrentThread;
    struct _KTHREAD *RESTRICTED_POINTER NextThread;
    struct _KTHREAD *IdleThread;
    CCHAR Number;
    CCHAR Reserved;
    USHORT BuildType;
    KAFFINITY SetMember;
    struct _RESTART_BLOCK *RestartBlock;
    ULONG_PTR PcrPage;
    ULONG Spares1[4];

//
// Space reserved for the system.
//

    ULONGLONG SystemReserved[8];

//
// Space reserved for the HAL.
//

    ULONGLONG HalReserved[16];

//
// End of the architecturally defined section of the PRCB.
} KPRCB, *PKPRCB, *RESTRICTED_POINTER PRKPRCB;

// begin_ntndis

//
// Define Processor Control Region Structure.
//

#define PCR_MINOR_VERSION 1
#define PCR_MAJOR_VERSION 1

typedef struct _KPCR {

//
// Major and minor version numbers of the PCR.
//
    ULONG MinorVersion;
    ULONG MajorVersion;

//
// Start of the architecturally defined section of the PCR. This section
// may be directly addressed by vendor/platform specific HAL code and will
// not change from version to version of NT.
//

//
// First and second level cache parameters.
//

    ULONG FirstLevelDcacheSize;
    ULONG FirstLevelDcacheFillSize;
    ULONG FirstLevelIcacheSize;
    ULONG FirstLevelIcacheFillSize;
    ULONG SecondLevelDcacheSize;
    ULONG SecondLevelDcacheFillSize;
    ULONG SecondLevelIcacheSize;
    ULONG SecondLevelIcacheFillSize;

//
// Data cache alignment and fill size used for cache flushing and alignment.
// These fields are set to the larger of the first and second level data
// cache fill sizes.
//

    ULONG DcacheAlignment;
    ULONG DcacheFillSize;

//
// Instruction cache alignment and fill size used for cache flushing and
// alignment. These fields are set to the larger of the first and second
// level data cache fill sizes.
//

    ULONG IcacheAlignment;
    ULONG IcacheFillSize;

//
// Processor identification from PrId register.
//

    ULONG ProcessorId;

//
// Profiling data.
//

    ULONG ProfileInterval;
    ULONG ProfileCount;

//
// Stall execution count and scale factor.
//

    ULONG StallExecutionCount;
    ULONG StallScaleFactor;

    ULONG InterruptionCount;

//
// Space reserved for the system.
//

    ULONGLONG   SystemReserved[6];

//
// Space reserved for the HAL
//

    ULONGLONG   HalReserved[64];

//
// IRQL mapping tables.
//

    UCHAR IrqlMask[64];
    UCHAR IrqlTable[64];

//
// External Interrupt vectors.
//

    PKINTERRUPT_ROUTINE InterruptRoutine[MAXIMUM_VECTOR];

//
// Reserved interrupt vector mask.
//

    ULONG ReservedVectors;

//
// Processor affinity mask.
//

    KAFFINITY SetMember;

//
// Complement of the processor affinity mask.
//

    KAFFINITY NotMember;

//
// Pointer to processor control block.
//

    struct _KPRCB *Prcb;

//
//  Shadow copy of Prcb->CurrentThread for fast access
//

    struct _KTHREAD *CurrentThread;

//
// Processor number.
//

    CCHAR Number;                        // Processor Number
    UCHAR DebugActive;                   // debug register active in user flag
    UCHAR KernelDebugActive;             // debug register active in kernel flag
    UCHAR CurrentIrql;                   // Current IRQL
    union {
        USHORT SoftwareInterruptPending; // Software Interrupt Pending Flag
        struct {
            UCHAR ApcInterrupt;          // 0x01 if APC int pending
            UCHAR DispatchInterrupt;     // 0x01 if dispatch int pending
        };
    };

//
// End of the architecturally defined section of the PCR. This section
// may be directly addressed by vendor/platform specific HAL code and will
// not change from version to version of NT.
//


} KPCR, *PKPCR;

//
// The highest user address reserves 64K bytes for a guard page. This
// the probing of address from kernel mode to only have to check the
// starting address for structures of 64k bytes or less.
//

#define MM_HIGHEST_USER_ADDRESS (PVOID) (ULONG_PTR)((UADDRESS_BASE + 0x3FFFFFEFFFF)) // highest user address
#define MM_USER_PROBE_ADDRESS ((ULONG_PTR)(UADDRESS_BASE + 0x3FFFFFF0000UI64)) // starting address of guard page
#define MM_SYSTEM_RANGE_START (PVOID) (KSEG0_BASE) // start of system space

//
// The following definitions are required for the debugger data block.
//

extern PVOID MmHighestUserAddress;
extern PVOID MmSystemRangeStart;
extern ULONG_PTR MmUserProbeAddress;

//
// The lowest user address reserves the low 64k.
//

#define MM_LOWEST_USER_ADDRESS  (PVOID)((ULONG_PTR)(UADDRESS_BASE+0x00010000))

// begin_wdm

#define MmGetProcedureAddress(Address) (Address)
#define MmLockPagableCodeSection(PLabelAddress) \
    MmLockPagableDataSection((PVOID)(*((PULONGLONG)PLabelAddress)))

//
// The lowest address for system space.
//

#define MM_LOWEST_SYSTEM_ADDRESS ((PVOID)((ULONG_PTR)(KADDRESS_BASE + 0xC0C00000)))
#endif // defined(_IA64_)

#define OBJ_NAME_PATH_SEPARATOR ((WCHAR)L'\\')

//
// Object Manager Object Type Specific Access Rights.
//

#define OBJECT_TYPE_CREATE (0x0001)

#define OBJECT_TYPE_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED | 0x1)

//
// Object Manager Directory Specific Access Rights.
//

#define DIRECTORY_QUERY                 (0x0001)
#define DIRECTORY_TRAVERSE              (0x0002)
#define DIRECTORY_CREATE_OBJECT         (0x0004)
#define DIRECTORY_CREATE_SUBDIRECTORY   (0x0008)

#define DIRECTORY_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED | 0xF)

//
// Object Manager Symbolic Link Specific Access Rights.
//

#define SYMBOLIC_LINK_QUERY (0x0001)

#define SYMBOLIC_LINK_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED | 0x1)

typedef struct _OBJECT_NAME_INFORMATION {               
    UNICODE_STRING Name;                                
} OBJECT_NAME_INFORMATION, *POBJECT_NAME_INFORMATION;   
#define DUPLICATE_CLOSE_SOURCE      0x00000001  // winnt
#define DUPLICATE_SAME_ACCESS       0x00000002  // winnt
#define DUPLICATE_SAME_ATTRIBUTES   0x00000004
// begin_winnt
//
// Predefined Value Types.
//

#define REG_NONE                    ( 0 )   // No value type
#define REG_SZ                      ( 1 )   // Unicode nul terminated string
#define REG_EXPAND_SZ               ( 2 )   // Unicode nul terminated string
                                            // (with environment variable references)
#define REG_BINARY                  ( 3 )   // Free form binary
#define REG_DWORD                   ( 4 )   // 32-bit number
#define REG_DWORD_LITTLE_ENDIAN     ( 4 )   // 32-bit number (same as REG_DWORD)
#define REG_DWORD_BIG_ENDIAN        ( 5 )   // 32-bit number
#define REG_LINK                    ( 6 )   // Symbolic Link (unicode)
#define REG_MULTI_SZ                ( 7 )   // Multiple Unicode strings
#define REG_RESOURCE_LIST           ( 8 )   // Resource list in the resource map
#define REG_FULL_RESOURCE_DESCRIPTOR ( 9 )  // Resource list in the hardware description
#define REG_RESOURCE_REQUIREMENTS_LIST ( 10 )
#define REG_QWORD                   ( 11 )  // 64-bit number
#define REG_QWORD_LITTLE_ENDIAN     ( 11 )  // 64-bit number (same as REG_QWORD)

//
// Service Types (Bit Mask)
//
#define SERVICE_KERNEL_DRIVER          0x00000001
#define SERVICE_FILE_SYSTEM_DRIVER     0x00000002
#define SERVICE_ADAPTER                0x00000004
#define SERVICE_RECOGNIZER_DRIVER      0x00000008

#define SERVICE_DRIVER                 (SERVICE_KERNEL_DRIVER | \
                                        SERVICE_FILE_SYSTEM_DRIVER | \
                                        SERVICE_RECOGNIZER_DRIVER)

#define SERVICE_WIN32_OWN_PROCESS      0x00000010
#define SERVICE_WIN32_SHARE_PROCESS    0x00000020
#define SERVICE_WIN32                  (SERVICE_WIN32_OWN_PROCESS | \
                                        SERVICE_WIN32_SHARE_PROCESS)

#define SERVICE_INTERACTIVE_PROCESS    0x00000100

#define SERVICE_TYPE_ALL               (SERVICE_WIN32  | \
                                        SERVICE_ADAPTER | \
                                        SERVICE_DRIVER  | \
                                        SERVICE_INTERACTIVE_PROCESS)

//
// Start Type
//

#define SERVICE_BOOT_START             0x00000000
#define SERVICE_SYSTEM_START           0x00000001
#define SERVICE_AUTO_START             0x00000002
#define SERVICE_DEMAND_START           0x00000003
#define SERVICE_DISABLED               0x00000004

//
// Error control type
//
#define SERVICE_ERROR_IGNORE           0x00000000
#define SERVICE_ERROR_NORMAL           0x00000001
#define SERVICE_ERROR_SEVERE           0x00000002
#define SERVICE_ERROR_CRITICAL         0x00000003

//
//
// Define the registry driver node enumerations
//

typedef enum _CM_SERVICE_NODE_TYPE {
    DriverType               = SERVICE_KERNEL_DRIVER,
    FileSystemType           = SERVICE_FILE_SYSTEM_DRIVER,
    Win32ServiceOwnProcess   = SERVICE_WIN32_OWN_PROCESS,
    Win32ServiceShareProcess = SERVICE_WIN32_SHARE_PROCESS,
    AdapterType              = SERVICE_ADAPTER,
    RecognizerType           = SERVICE_RECOGNIZER_DRIVER
} SERVICE_NODE_TYPE;

typedef enum _CM_SERVICE_LOAD_TYPE {
    BootLoad    = SERVICE_BOOT_START,
    SystemLoad  = SERVICE_SYSTEM_START,
    AutoLoad    = SERVICE_AUTO_START,
    DemandLoad  = SERVICE_DEMAND_START,
    DisableLoad = SERVICE_DISABLED
} SERVICE_LOAD_TYPE;

typedef enum _CM_ERROR_CONTROL_TYPE {
    IgnoreError   = SERVICE_ERROR_IGNORE,
    NormalError   = SERVICE_ERROR_NORMAL,
    SevereError   = SERVICE_ERROR_SEVERE,
    CriticalError = SERVICE_ERROR_CRITICAL
} SERVICE_ERROR_TYPE;

// end_winnt

//
// Resource List definitions
//

// begin_ntminiport begin_ntndis

//
// Defines the Type in the RESOURCE_DESCRIPTOR
//
// NOTE:  For all CM_RESOURCE_TYPE values, there must be a
// corresponding ResType value in the 32-bit ConfigMgr headerfile
// (cfgmgr32.h).  Values in the range [0x6,0x80) use the same values
// as their ConfigMgr counterparts.  CM_RESOURCE_TYPE values with
// the high bit set (i.e., in the range [0x80,0xFF]), are
// non-arbitrated resources.  These correspond to the same values
// in cfgmgr32.h that have their high bit set (however, since
// cfgmgr32.h uses 16 bits for ResType values, these values are in
// the range [0x8000,0x807F).  Note that ConfigMgr ResType values
// cannot be in the range [0x8080,0xFFFF), because they would not
// be able to map into CM_RESOURCE_TYPE values.  (0xFFFF itself is
// a special value, because it maps to CmResourceTypeDeviceSpecific.)
//

typedef int CM_RESOURCE_TYPE;

// CmResourceTypeNull is reserved

#define CmResourceTypeNull                0   // ResType_All or ResType_None (0x0000)
#define CmResourceTypePort                1   // ResType_IO (0x0002)
#define CmResourceTypeInterrupt           2   // ResType_IRQ (0x0004)
#define CmResourceTypeMemory              3   // ResType_Mem (0x0001)
#define CmResourceTypeDma                 4   // ResType_DMA (0x0003)
#define CmResourceTypeDeviceSpecific      5   // ResType_ClassSpecific (0xFFFF)
#define CmResourceTypeBusNumber           6   // ResType_BusNumber (0x0006)
// end_wdm
#define CmResourceTypeMaximum             7
#define CmResourceTypeAssignedResource    8   // BUGBUG--remove
#define CmResourceTypeSubAllocateFrom     9   // BUGBUG--remove
// begin_wdm
#define CmResourceTypeNonArbitrated     128   // Not arbitrated if 0x80 bit set
#define CmResourceTypeConfigData        128   // ResType_Reserved (0x8000)
#define CmResourceTypeDevicePrivate     129   // ResType_DevicePrivate (0x8001)
#define CmResourceTypePcCardConfig      130   // ResType_PcCardConfig (0x8002)
#define CmResourceTypeMfCardConfig      131   // ResType_MfCardConfig (0x8003)

//
// Defines the ShareDisposition in the RESOURCE_DESCRIPTOR
//

typedef enum _CM_SHARE_DISPOSITION {
    CmResourceShareUndetermined = 0,    // Reserved
    CmResourceShareDeviceExclusive,
    CmResourceShareDriverExclusive,
    CmResourceShareShared
} CM_SHARE_DISPOSITION;

//
// Define the PASSIGNED_RESOURCE type
//

#ifndef PASSIGNED_RESOURCE_DEFINED
#define PASSIGNED_RESOURCE_DEFINED
typedef PVOID PASSIGNED_RESOURCE;
#endif // PASSIGNED_RESOURCE_DEFINED

// end_wdm

//
// Define the bit masks for Flags common for all CM_RESOURCE_TYPE
//
// BUGBUG--remove the following 3 flags...
//
#define CM_RESOURCE_COMMON_COMPUTE_LENGTH_FROM_DEPENDENTS   0x8000
#define CM_RESOURCE_COMMON_NOT_REASSIGNED                   0x4000
#define CM_RESOURCE_COMMON_SUBSTRACTIVE                     0x2000

// begin_wdm

//
// Define the bit masks for Flags when type is CmResourceTypeInterrupt
//

#define CM_RESOURCE_INTERRUPT_LEVEL_SENSITIVE 0
#define CM_RESOURCE_INTERRUPT_LATCHED         1

//
// Define the bit masks for Flags when type is CmResourceTypeMemory
//

#define CM_RESOURCE_MEMORY_READ_WRITE       0x0000
#define CM_RESOURCE_MEMORY_READ_ONLY        0x0001
#define CM_RESOURCE_MEMORY_WRITE_ONLY       0x0002
#define CM_RESOURCE_MEMORY_PREFETCHABLE     0x0004

#define CM_RESOURCE_MEMORY_COMBINEDWRITE    0x0008
#define CM_RESOURCE_MEMORY_24               0x0010
#define CM_RESOURCE_MEMORY_CACHEABLE        0x0020

//
// Define the bit masks for Flags when type is CmResourceTypePort
//

#define CM_RESOURCE_PORT_MEMORY                             0x0000
#define CM_RESOURCE_PORT_IO                                 0x0001
// end_wdm
#define CM_RESOURCE_PORT_FORWARD_FIRST_256_OF_EACH_1024     0x0002  // BUGBUG--remove
// begin_wdm
#define CM_RESOURCE_PORT_10_BIT_DECODE                      0x0004
#define CM_RESOURCE_PORT_12_BIT_DECODE                      0x0008
#define CM_RESOURCE_PORT_16_BIT_DECODE                      0x0010
#define CM_RESOURCE_PORT_POSITIVE_DECODE                    0x0020
#define CM_RESOURCE_PORT_PASSIVE_DECODE                     0x0040
#define CM_RESOURCE_PORT_WINDOW_DECODE                      0x0080

//
// Define the bit masks for Flags when type is CmResourceTypeDma
//

#define CM_RESOURCE_DMA_8                   0x0000
#define CM_RESOURCE_DMA_16                  0x0001
#define CM_RESOURCE_DMA_32                  0x0002
#define CM_RESOURCE_DMA_8_AND_16            0x0004
#define CM_RESOURCE_DMA_BUS_MASTER          0x0008
#define CM_RESOURCE_DMA_TYPE_A              0x0010
#define CM_RESOURCE_DMA_TYPE_B              0x0020
#define CM_RESOURCE_DMA_TYPE_F              0x0040
// end_wdm

//
// Define the bit masks for Flags when type is CmResourceTypeBusNumber
//

#define CM_RESOURCE_BUSNUMBER_SUBALLOCATE_FIRST_VALUE   0x0001  // BUGBUG--remove

//
// Define the bit masks for Flags when type is CmResourceTypeSubAllocateFrom
//

#define CM_RESOURCE_SUBALLOCATEFROM_FIXED_TRANSLATION   0x0001  // BUGBUG--remove
#define CM_RESOURCE_SUBALLOCATEFROM_WIRED_TRANSLATION   0x0002  // BUGBUG--remove

// end_ntminiport end_ntndis

// begin_wdm

//
// This structure defines one type of resource used by a driver.
//
// There can only be *1* DeviceSpecificData block. It must be located at
// the end of all resource descriptors in a full descriptor block.
//

//
// Make sure alignment is made properly by compiler; otherwise move
// flags back to the top of the structure (common to all members of the
// union).
//
// begin_ntndis

#include "pshpack4.h"
typedef struct _CM_PARTIAL_RESOURCE_DESCRIPTOR {
    UCHAR Type;
    UCHAR ShareDisposition;
    USHORT Flags;
    union {

        //
        // Range of resources, inclusive.  These are physical, bus relative.
        // It is known that Port and Memory below have the exact same layout
        // as Generic.
        //

        struct {
            PHYSICAL_ADDRESS Start;
            ULONG Length;
        } Generic;

        //
        // end_wdm
        // Range of port numbers, inclusive. These are physical, bus
        // relative. The value should be the same as the one passed to
        // HalTranslateBusAddress().
        // begin_wdm
        //

        struct {
            PHYSICAL_ADDRESS Start;
            ULONG Length;
        } Port;

        //
        // end_wdm
        // IRQL and vector. Should be same values as were passed to
        // HalGetInterruptVector().
        // begin_wdm
        //

        struct {
            ULONG Level;
            ULONG Vector;
            ULONG Affinity;
        } Interrupt;

        //
        // Range of memory addresses, inclusive. These are physical, bus
        // relative. The value should be the same as the one passed to
        // HalTranslateBusAddress().
        //

        struct {
            PHYSICAL_ADDRESS Start;    // 64 bit physical addresses.
            ULONG Length;
        } Memory;

        //
        // Physical DMA channel.
        //

        struct {
            ULONG Channel;
            ULONG Port;
            ULONG Reserved1;
        } Dma;

        //
        // Device driver private data, usually used to help it figure
        // what the resource assignments decisions that were made.
        //

        struct {
            ULONG Data[3];
        } DevicePrivate;

        //
        // Bus Number information.
        //

        struct {
            ULONG Start;
            ULONG Length;
            ULONG Reserved;
        } BusNumber;

        //
        // Device Specific information defined by the driver.
        // The DataSize field indicates the size of the data in bytes. The
        // data is located immediately after the DeviceSpecificData field in
        // the structure.
        //

        struct {
            ULONG DataSize;
            ULONG Reserved1;
            ULONG Reserved2;
        } DeviceSpecificData;
    } u;
} CM_PARTIAL_RESOURCE_DESCRIPTOR, *PCM_PARTIAL_RESOURCE_DESCRIPTOR;
#include "poppack.h"

//
// A Partial Resource List is what can be found in the ARC firmware
// or will be generated by ntdetect.com.
// The configuration manager will transform this structure into a Full
// resource descriptor when it is about to store it in the regsitry.
//
// Note: There must a be a convention to the order of fields of same type,
// (defined on a device by device basis) so that the fields can make sense
// to a driver (i.e. when multiple memory ranges are necessary).
//

typedef struct _CM_PARTIAL_RESOURCE_LIST {
    USHORT Version;
    USHORT Revision;
    ULONG Count;
    CM_PARTIAL_RESOURCE_DESCRIPTOR PartialDescriptors[1];
} CM_PARTIAL_RESOURCE_LIST, *PCM_PARTIAL_RESOURCE_LIST;

//
// A Full Resource Descriptor is what can be found in the registry.
// This is what will be returned to a driver when it queries the registry
// to get device information; it will be stored under a key in the hardware
// description tree.
//
// end_wdm
// Note: The BusNumber and Type are redundant information, but we will keep
// it since it allows the driver _not_ to append it when it is creating
// a resource list which could possibly span multiple buses.
//
// begin_wdm
// Note: There must a be a convention to the order of fields of same type,
// (defined on a device by device basis) so that the fields can make sense
// to a driver (i.e. when multiple memory ranges are necessary).
//

typedef struct _CM_FULL_RESOURCE_DESCRIPTOR {
    INTERFACE_TYPE InterfaceType; // unused for WDM
    ULONG BusNumber; // unused for WDM
    CM_PARTIAL_RESOURCE_LIST PartialResourceList;
} CM_FULL_RESOURCE_DESCRIPTOR, *PCM_FULL_RESOURCE_DESCRIPTOR;

//
// The Resource list is what will be stored by the drivers into the
// resource map via the IO API.
//

typedef struct _CM_RESOURCE_LIST {
    ULONG Count;
    CM_FULL_RESOURCE_DESCRIPTOR List[1];
} CM_RESOURCE_LIST, *PCM_RESOURCE_LIST;

// end_ntndis
//
// Define the structures used to interpret configuration data of
// \\Registry\machine\hardware\description tree.
// Basically, these structures are used to interpret component
// sepcific data.
//

//
// Define DEVICE_FLAGS
//

typedef struct _DEVICE_FLAGS {
    ULONG Failed : 1;
    ULONG ReadOnly : 1;
    ULONG Removable : 1;
    ULONG ConsoleIn : 1;
    ULONG ConsoleOut : 1;
    ULONG Input : 1;
    ULONG Output : 1;
} DEVICE_FLAGS, *PDEVICE_FLAGS;

//
// Define Component Information structure
//

typedef struct _CM_COMPONENT_INFORMATION {
    DEVICE_FLAGS Flags;
    ULONG Version;
    ULONG Key;
    ULONG AffinityMask;
} CM_COMPONENT_INFORMATION, *PCM_COMPONENT_INFORMATION;

//
// The following structures are used to interpret x86
// DeviceSpecificData of CM_PARTIAL_RESOURCE_DESCRIPTOR.
// (Most of the structures are defined by BIOS.  They are
// not aligned on word (or dword) boundary.
//

//
// Define the Rom Block structure
//

typedef struct _CM_ROM_BLOCK {
    ULONG Address;
    ULONG Size;
} CM_ROM_BLOCK, *PCM_ROM_BLOCK;

// begin_ntminiport begin_ntndis

#include "pshpack1.h"

// end_ntminiport end_ntndis

//
// Define INT13 driver parameter block
//

typedef struct _CM_INT13_DRIVE_PARAMETER {
    USHORT DriveSelect;
    ULONG MaxCylinders;
    USHORT SectorsPerTrack;
    USHORT MaxHeads;
    USHORT NumberDrives;
} CM_INT13_DRIVE_PARAMETER, *PCM_INT13_DRIVE_PARAMETER;

// begin_ntminiport begin_ntndis

//
// Define Mca POS data block for slot
//

typedef struct _CM_MCA_POS_DATA {
    USHORT AdapterId;
    UCHAR PosData1;
    UCHAR PosData2;
    UCHAR PosData3;
    UCHAR PosData4;
} CM_MCA_POS_DATA, *PCM_MCA_POS_DATA;

//
// Memory configuration of eisa data block structure
//

typedef struct _EISA_MEMORY_TYPE {
    UCHAR ReadWrite: 1;
    UCHAR Cached : 1;
    UCHAR Reserved0 :1;
    UCHAR Type:2;
    UCHAR Shared:1;
    UCHAR Reserved1 :1;
    UCHAR MoreEntries : 1;
} EISA_MEMORY_TYPE, *PEISA_MEMORY_TYPE;

typedef struct _EISA_MEMORY_CONFIGURATION {
    EISA_MEMORY_TYPE ConfigurationByte;
    UCHAR DataSize;
    USHORT AddressLowWord;
    UCHAR AddressHighByte;
    USHORT MemorySize;
} EISA_MEMORY_CONFIGURATION, *PEISA_MEMORY_CONFIGURATION;


//
// Interrupt configurationn of eisa data block structure
//

typedef struct _EISA_IRQ_DESCRIPTOR {
    UCHAR Interrupt : 4;
    UCHAR Reserved :1;
    UCHAR LevelTriggered :1;
    UCHAR Shared : 1;
    UCHAR MoreEntries : 1;
} EISA_IRQ_DESCRIPTOR, *PEISA_IRQ_DESCRIPTOR;

typedef struct _EISA_IRQ_CONFIGURATION {
    EISA_IRQ_DESCRIPTOR ConfigurationByte;
    UCHAR Reserved;
} EISA_IRQ_CONFIGURATION, *PEISA_IRQ_CONFIGURATION;


//
// DMA description of eisa data block structure
//

typedef struct _DMA_CONFIGURATION_BYTE0 {
    UCHAR Channel : 3;
    UCHAR Reserved : 3;
    UCHAR Shared :1;
    UCHAR MoreEntries :1;
} DMA_CONFIGURATION_BYTE0;

typedef struct _DMA_CONFIGURATION_BYTE1 {
    UCHAR Reserved0 : 2;
    UCHAR TransferSize : 2;
    UCHAR Timing : 2;
    UCHAR Reserved1 : 2;
} DMA_CONFIGURATION_BYTE1;

typedef struct _EISA_DMA_CONFIGURATION {
    DMA_CONFIGURATION_BYTE0 ConfigurationByte0;
    DMA_CONFIGURATION_BYTE1 ConfigurationByte1;
} EISA_DMA_CONFIGURATION, *PEISA_DMA_CONFIGURATION;


//
// Port description of eisa data block structure
//

typedef struct _EISA_PORT_DESCRIPTOR {
    UCHAR NumberPorts : 5;
    UCHAR Reserved :1;
    UCHAR Shared :1;
    UCHAR MoreEntries : 1;
} EISA_PORT_DESCRIPTOR, *PEISA_PORT_DESCRIPTOR;

typedef struct _EISA_PORT_CONFIGURATION {
    EISA_PORT_DESCRIPTOR Configuration;
    USHORT PortAddress;
} EISA_PORT_CONFIGURATION, *PEISA_PORT_CONFIGURATION;


//
// Eisa slot information definition
// N.B. This structure is different from the one defined
//      in ARC eisa addendum.
//

typedef struct _CM_EISA_SLOT_INFORMATION {
    UCHAR ReturnCode;
    UCHAR ReturnFlags;
    UCHAR MajorRevision;
    UCHAR MinorRevision;
    USHORT Checksum;
    UCHAR NumberFunctions;
    UCHAR FunctionInformation;
    ULONG CompressedId;
} CM_EISA_SLOT_INFORMATION, *PCM_EISA_SLOT_INFORMATION;


//
// Eisa function information definition
//

typedef struct _CM_EISA_FUNCTION_INFORMATION {
    ULONG CompressedId;
    UCHAR IdSlotFlags1;
    UCHAR IdSlotFlags2;
    UCHAR MinorRevision;
    UCHAR MajorRevision;
    UCHAR Selections[26];
    UCHAR FunctionFlags;
    UCHAR TypeString[80];
    EISA_MEMORY_CONFIGURATION EisaMemory[9];
    EISA_IRQ_CONFIGURATION EisaIrq[7];
    EISA_DMA_CONFIGURATION EisaDma[4];
    EISA_PORT_CONFIGURATION EisaPort[20];
    UCHAR InitializationData[60];
} CM_EISA_FUNCTION_INFORMATION, *PCM_EISA_FUNCTION_INFORMATION;

//
// The following defines the way pnp bios information is stored in
// the registry \\HKEY_LOCAL_MACHINE\HARDWARE\Description\System\MultifunctionAdapter\x
// key, where x is an integer number indicating adapter instance. The
// "Identifier" of the key must equal to "PNP BIOS" and the
// "ConfigurationData" is organized as follow:
//
//      CM_PNP_BIOS_INSTALLATION_CHECK        +
//      CM_PNP_BIOS_DEVICE_NODE for device 1  +
//      CM_PNP_BIOS_DEVICE_NODE for device 2  +
//                ...
//      CM_PNP_BIOS_DEVICE_NODE for device n
//

//
// Pnp BIOS device node structure
//

typedef struct _CM_PNP_BIOS_DEVICE_NODE {
    USHORT Size;
    UCHAR Node;
    ULONG ProductId;
    UCHAR DeviceType[3];
    USHORT DeviceAttributes;
    // followed by AllocatedResourceBlock, PossibleResourceBlock
    // and CompatibleDeviceId
} CM_PNP_BIOS_DEVICE_NODE,*PCM_PNP_BIOS_DEVICE_NODE;

//
// Pnp BIOS Installation check
//

typedef struct _CM_PNP_BIOS_INSTALLATION_CHECK {
    UCHAR Signature[4];             // $PnP (ascii)
    UCHAR Revision;
    UCHAR Length;
    USHORT ControlField;
    UCHAR Checksum;
    ULONG EventFlagAddress;         // Physical address
    USHORT RealModeEntryOffset;
    USHORT RealModeEntrySegment;
    USHORT ProtectedModeEntryOffset;
    ULONG ProtectedModeCodeBaseAddress;
    ULONG OemDeviceId;
    USHORT RealModeDataBaseAddress;
    ULONG ProtectedModeDataBaseAddress;
} CM_PNP_BIOS_INSTALLATION_CHECK, *PCM_PNP_BIOS_INSTALLATION_CHECK;

#include "poppack.h"

//
// Masks for EISA function information
//

#define EISA_FUNCTION_ENABLED                   0x80
#define EISA_FREE_FORM_DATA                     0x40
#define EISA_HAS_PORT_INIT_ENTRY                0x20
#define EISA_HAS_PORT_RANGE                     0x10
#define EISA_HAS_DMA_ENTRY                      0x08
#define EISA_HAS_IRQ_ENTRY                      0x04
#define EISA_HAS_MEMORY_ENTRY                   0x02
#define EISA_HAS_TYPE_ENTRY                     0x01
#define EISA_HAS_INFORMATION                    EISA_HAS_PORT_RANGE + \
                                                EISA_HAS_DMA_ENTRY + \
                                                EISA_HAS_IRQ_ENTRY + \
                                                EISA_HAS_MEMORY_ENTRY + \
                                                EISA_HAS_TYPE_ENTRY

//
// Masks for EISA memory configuration
//

#define EISA_MORE_ENTRIES                       0x80
#define EISA_SYSTEM_MEMORY                      0x00
#define EISA_MEMORY_TYPE_RAM                    0x01

//
// Returned error code for EISA bios call
//

#define EISA_INVALID_SLOT                       0x80
#define EISA_INVALID_FUNCTION                   0x81
#define EISA_INVALID_CONFIGURATION              0x82
#define EISA_EMPTY_SLOT                         0x83
#define EISA_INVALID_BIOS_CALL                  0x86

// end_ntminiport end_ntndis

//
// The following structures are used to interpret mips
// DeviceSpecificData of CM_PARTIAL_RESOURCE_DESCRIPTOR.
//

//
// Device data records for adapters.
//

//
// The device data record for the Emulex SCSI controller.
//

typedef struct _CM_SCSI_DEVICE_DATA {
    USHORT Version;
    USHORT Revision;
    UCHAR HostIdentifier;
} CM_SCSI_DEVICE_DATA, *PCM_SCSI_DEVICE_DATA;

//
// Device data records for controllers.
//

//
// The device data record for the Video controller.
//

typedef struct _CM_VIDEO_DEVICE_DATA {
    USHORT Version;
    USHORT Revision;
    ULONG VideoClock;
} CM_VIDEO_DEVICE_DATA, *PCM_VIDEO_DEVICE_DATA;

//
// The device data record for the SONIC network controller.
//

typedef struct _CM_SONIC_DEVICE_DATA {
    USHORT Version;
    USHORT Revision;
    USHORT DataConfigurationRegister;
    UCHAR EthernetAddress[8];
} CM_SONIC_DEVICE_DATA, *PCM_SONIC_DEVICE_DATA;

//
// The device data record for the serial controller.
//

typedef struct _CM_SERIAL_DEVICE_DATA {
    USHORT Version;
    USHORT Revision;
    ULONG BaudClock;
} CM_SERIAL_DEVICE_DATA, *PCM_SERIAL_DEVICE_DATA;

//
// Device data records for peripherals.
//

//
// The device data record for the Monitor peripheral.
//

typedef struct _CM_MONITOR_DEVICE_DATA {
    USHORT Version;
    USHORT Revision;
    USHORT HorizontalScreenSize;
    USHORT VerticalScreenSize;
    USHORT HorizontalResolution;
    USHORT VerticalResolution;
    USHORT HorizontalDisplayTimeLow;
    USHORT HorizontalDisplayTime;
    USHORT HorizontalDisplayTimeHigh;
    USHORT HorizontalBackPorchLow;
    USHORT HorizontalBackPorch;
    USHORT HorizontalBackPorchHigh;
    USHORT HorizontalFrontPorchLow;
    USHORT HorizontalFrontPorch;
    USHORT HorizontalFrontPorchHigh;
    USHORT HorizontalSyncLow;
    USHORT HorizontalSync;
    USHORT HorizontalSyncHigh;
    USHORT VerticalBackPorchLow;
    USHORT VerticalBackPorch;
    USHORT VerticalBackPorchHigh;
    USHORT VerticalFrontPorchLow;
    USHORT VerticalFrontPorch;
    USHORT VerticalFrontPorchHigh;
    USHORT VerticalSyncLow;
    USHORT VerticalSync;
    USHORT VerticalSyncHigh;
} CM_MONITOR_DEVICE_DATA, *PCM_MONITOR_DEVICE_DATA;

//
// The device data record for the Floppy peripheral.
//

typedef struct _CM_FLOPPY_DEVICE_DATA {
    USHORT Version;
    USHORT Revision;
    CHAR Size[8];
    ULONG MaxDensity;
    ULONG MountDensity;
    //
    // New data fields for version >= 2.0
    //
    UCHAR StepRateHeadUnloadTime;
    UCHAR HeadLoadTime;
    UCHAR MotorOffTime;
    UCHAR SectorLengthCode;
    UCHAR SectorPerTrack;
    UCHAR ReadWriteGapLength;
    UCHAR DataTransferLength;
    UCHAR FormatGapLength;
    UCHAR FormatFillCharacter;
    UCHAR HeadSettleTime;
    UCHAR MotorSettleTime;
    UCHAR MaximumTrackValue;
    UCHAR DataTransferRate;
} CM_FLOPPY_DEVICE_DATA, *PCM_FLOPPY_DEVICE_DATA;

//
// The device data record for the Keyboard peripheral.
// The KeyboardFlags is defined (by x86 BIOS INT 16h, function 02) as:
//      bit 7 : Insert on
//      bit 6 : Caps Lock on
//      bit 5 : Num Lock on
//      bit 4 : Scroll Lock on
//      bit 3 : Alt Key is down
//      bit 2 : Ctrl Key is down
//      bit 1 : Left shift key is down
//      bit 0 : Right shift key is down
//

typedef struct _CM_KEYBOARD_DEVICE_DATA {
    USHORT Version;
    USHORT Revision;
    UCHAR Type;
    UCHAR Subtype;
    USHORT KeyboardFlags;
} CM_KEYBOARD_DEVICE_DATA, *PCM_KEYBOARD_DEVICE_DATA;

//
// Declaration of the structure for disk geometries
//

typedef struct _CM_DISK_GEOMETRY_DEVICE_DATA {
    ULONG BytesPerSector;
    ULONG NumberOfCylinders;
    ULONG SectorsPerTrack;
    ULONG NumberOfHeads;
} CM_DISK_GEOMETRY_DEVICE_DATA, *PCM_DISK_GEOMETRY_DEVICE_DATA;

// end_wdm
//
// Declaration of the structure for the PcCard ISA IRQ map
//

typedef struct _CM_PCCARD_DEVICE_DATA {
    UCHAR Flags;
    UCHAR ErrorCode;
    USHORT Reserved;
    ULONG BusData;
    ULONG DeviceId;
    ULONG LegacyBaseAddress;
    UCHAR IRQMap[16];
} CM_PCCARD_DEVICE_DATA, *PCM_PCCARD_DEVICE_DATA;

// Definitions for Flags

#define PCCARD_MAP_ERROR        0x01
#define PCCARD_DEVICE_PCI       0x10

#define PCCARD_SCAN_DISABLED    0x01
#define PCCARD_MAP_ZERO         0x02
#define PCCARD_NO_TIMER         0x03
#define PCCARD_NO_PIC           0x04
#define PCCARD_NO_LEGACY_BASE   0x05
#define PCCARD_DUP_LEGACY_BASE  0x06
#define PCCARD_NO_CONTROLLERS   0x07

// begin_wdm
// begin_ntminiport

//
// Defines Resource Options
//

#define IO_RESOURCE_PREFERRED       0x01
#define IO_RESOURCE_DEFAULT         0x02
#define IO_RESOURCE_ALTERNATIVE     0x08


//
// This structure defines one type of resource requested by the driver
//

typedef struct _IO_RESOURCE_DESCRIPTOR {
    UCHAR Option;
    UCHAR Type;                         // use CM_RESOURCE_TYPE
    UCHAR ShareDisposition;             // use CM_SHARE_DISPOSITION
    UCHAR Spare1;
    USHORT Flags;                       // use CM resource flag defines
    USHORT Spare2;                      // align

    union {
        struct {
            ULONG Length;
            ULONG Alignment;
            PHYSICAL_ADDRESS MinimumAddress;
            PHYSICAL_ADDRESS MaximumAddress;
        } Port;

        struct {
            ULONG Length;
            ULONG Alignment;
            PHYSICAL_ADDRESS MinimumAddress;
            PHYSICAL_ADDRESS MaximumAddress;
        } Memory;

        struct {
            ULONG MinimumVector;
            ULONG MaximumVector;
        } Interrupt;

        struct {
            ULONG MinimumChannel;
            ULONG MaximumChannel;
        } Dma;

        struct {
            ULONG Length;
            ULONG Alignment;
            PHYSICAL_ADDRESS MinimumAddress;
            PHYSICAL_ADDRESS MaximumAddress;
        } Generic;

        struct {
            ULONG Data[3];
        } DevicePrivate;

        //
        // Bus Number information.
        //

        struct {
            ULONG Length;
            ULONG MinBusNumber;
            ULONG MaxBusNumber;
            ULONG Reserved;
        } BusNumber;

// end_wdm

        struct {
            PASSIGNED_RESOURCE AssignedResource;
        } AssignedResource;     // will be obsoleted

        struct {
            UCHAR Type;                 // use CM_RESOURCE_TYPE
            UCHAR Reserved[3];
            PASSIGNED_RESOURCE AssignedResource;
            PHYSICAL_ADDRESS Transformation;
        } SubAllocateFrom;      // will be obsoleted

// begin_wdm

        struct {
            ULONG Priority;   // use LCPRI_Xxx values in cfg.h
            ULONG Reserved1;
            ULONG Reserved2;
        } ConfigData;

    } u;

} IO_RESOURCE_DESCRIPTOR, *PIO_RESOURCE_DESCRIPTOR;

// end_ntminiport


typedef struct _IO_RESOURCE_LIST {
    USHORT Version;
    USHORT Revision;

    ULONG Count;
    IO_RESOURCE_DESCRIPTOR Descriptors[1];
} IO_RESOURCE_LIST, *PIO_RESOURCE_LIST;


typedef struct _IO_RESOURCE_REQUIREMENTS_LIST {
    ULONG ListSize;
    INTERFACE_TYPE InterfaceType; // unused for WDM
    ULONG BusNumber; // unused for WDM
    ULONG SlotNumber;
    ULONG Reserved[3];
    ULONG AlternativeLists;
    IO_RESOURCE_LIST  List[1];
} IO_RESOURCE_REQUIREMENTS_LIST, *PIO_RESOURCE_REQUIREMENTS_LIST;

//
// Registry Specific Access Rights.
//

#define KEY_QUERY_VALUE         (0x0001)
#define KEY_SET_VALUE           (0x0002)
#define KEY_CREATE_SUB_KEY      (0x0004)
#define KEY_ENUMERATE_SUB_KEYS  (0x0008)
#define KEY_NOTIFY              (0x0010)
#define KEY_CREATE_LINK         (0x0020)

#define KEY_READ                ((STANDARD_RIGHTS_READ       |\
                                  KEY_QUERY_VALUE            |\
                                  KEY_ENUMERATE_SUB_KEYS     |\
                                  KEY_NOTIFY)                 \
                                  &                           \
                                 (~SYNCHRONIZE))


#define KEY_WRITE               ((STANDARD_RIGHTS_WRITE      |\
                                  KEY_SET_VALUE              |\
                                  KEY_CREATE_SUB_KEY)         \
                                  &                           \
                                 (~SYNCHRONIZE))

#define KEY_EXECUTE             ((KEY_READ)                   \
                                  &                           \
                                 (~SYNCHRONIZE))

#define KEY_ALL_ACCESS          ((STANDARD_RIGHTS_ALL        |\
                                  KEY_QUERY_VALUE            |\
                                  KEY_SET_VALUE              |\
                                  KEY_CREATE_SUB_KEY         |\
                                  KEY_ENUMERATE_SUB_KEYS     |\
                                  KEY_NOTIFY                 |\
                                  KEY_CREATE_LINK)            \
                                  &                           \
                                 (~SYNCHRONIZE))

//
// Open/Create Options
//

#define REG_OPTION_RESERVED         (0x00000000L)   // Parameter is reserved

#define REG_OPTION_NON_VOLATILE     (0x00000000L)   // Key is preserved
                                                    // when system is rebooted

#define REG_OPTION_VOLATILE         (0x00000001L)   // Key is not preserved
                                                    // when system is rebooted

#define REG_OPTION_CREATE_LINK      (0x00000002L)   // Created key is a
                                                    // symbolic link

#define REG_OPTION_BACKUP_RESTORE   (0x00000004L)   // open for backup or restore
                                                    // special access rules
                                                    // privilege required

#define REG_OPTION_OPEN_LINK        (0x00000008L)   // Open symbolic link

#define REG_LEGAL_OPTION            \
                (REG_OPTION_RESERVED            |\
                 REG_OPTION_NON_VOLATILE        |\
                 REG_OPTION_VOLATILE            |\
                 REG_OPTION_CREATE_LINK         |\
                 REG_OPTION_BACKUP_RESTORE      |\
                 REG_OPTION_OPEN_LINK)

//
// Key creation/open disposition
//

#define REG_CREATED_NEW_KEY         (0x00000001L)   // New Registry Key created
#define REG_OPENED_EXISTING_KEY     (0x00000002L)   // Existing Key opened

//
// Key restore flags
//

#define REG_WHOLE_HIVE_VOLATILE     (0x00000001L)   // Restore whole hive volatile
#define REG_REFRESH_HIVE            (0x00000002L)   // Unwind changes to last flush
#define REG_NO_LAZY_FLUSH           (0x00000004L)   // Never lazy flush this hive
#define REG_FORCE_RESTORE           (0x00000008L)   // Force the restore process even when we have open handles on subkeys

//
// Key query structures
//

typedef struct _KEY_BASIC_INFORMATION {
    LARGE_INTEGER LastWriteTime;
    ULONG   TitleIndex;
    ULONG   NameLength;
    WCHAR   Name[1];            // Variable length string
} KEY_BASIC_INFORMATION, *PKEY_BASIC_INFORMATION;

typedef struct _KEY_NODE_INFORMATION {
    LARGE_INTEGER LastWriteTime;
    ULONG   TitleIndex;
    ULONG   ClassOffset;
    ULONG   ClassLength;
    ULONG   NameLength;
    WCHAR   Name[1];            // Variable length string
//          Class[1];           // Variable length string not declared
} KEY_NODE_INFORMATION, *PKEY_NODE_INFORMATION;

typedef struct _KEY_FULL_INFORMATION {
    LARGE_INTEGER LastWriteTime;
    ULONG   TitleIndex;
    ULONG   ClassOffset;
    ULONG   ClassLength;
    ULONG   SubKeys;
    ULONG   MaxNameLen;
    ULONG   MaxClassLen;
    ULONG   Values;
    ULONG   MaxValueNameLen;
    ULONG   MaxValueDataLen;
    WCHAR   Class[1];           // Variable length
} KEY_FULL_INFORMATION, *PKEY_FULL_INFORMATION;

// end_wdm
typedef struct _KEY_NAME_INFORMATION {
    ULONG   NameLength;
    WCHAR   Name[1];            // Variable length string
} KEY_NAME_INFORMATION, *PKEY_NAME_INFORMATION;

// begin_wdm
typedef enum _KEY_INFORMATION_CLASS {
    KeyBasicInformation,
    KeyNodeInformation,
    KeyFullInformation
// end_wdm
    ,
    KeyNameInformation
// begin_wdm
} KEY_INFORMATION_CLASS;

typedef struct _KEY_WRITE_TIME_INFORMATION {
    LARGE_INTEGER LastWriteTime;
} KEY_WRITE_TIME_INFORMATION, *PKEY_WRITE_TIME_INFORMATION;

typedef enum _KEY_SET_INFORMATION_CLASS {
    KeyWriteTimeInformation
} KEY_SET_INFORMATION_CLASS;

//
// Value entry query structures
//

typedef struct _KEY_VALUE_BASIC_INFORMATION {
    ULONG   TitleIndex;
    ULONG   Type;
    ULONG   NameLength;
    WCHAR   Name[1];            // Variable size
} KEY_VALUE_BASIC_INFORMATION, *PKEY_VALUE_BASIC_INFORMATION;

typedef struct _KEY_VALUE_FULL_INFORMATION {
    ULONG   TitleIndex;
    ULONG   Type;
    ULONG   DataOffset;
    ULONG   DataLength;
    ULONG   NameLength;
    WCHAR   Name[1];            // Variable size
//          Data[1];            // Variable size data not declared
} KEY_VALUE_FULL_INFORMATION, *PKEY_VALUE_FULL_INFORMATION;

typedef struct _KEY_VALUE_PARTIAL_INFORMATION {
    ULONG   TitleIndex;
    ULONG   Type;
    ULONG   DataLength;
    UCHAR   Data[1];            // Variable size
} KEY_VALUE_PARTIAL_INFORMATION, *PKEY_VALUE_PARTIAL_INFORMATION;

typedef struct _KEY_VALUE_PARTIAL_INFORMATION_ALIGN64 {
    ULONG   Type;
    ULONG   DataLength;
    UCHAR   Data[1];            // Variable size
} KEY_VALUE_PARTIAL_INFORMATION_ALIGN64, *PKEY_VALUE_PARTIAL_INFORMATION_ALIGN64;

typedef struct _KEY_VALUE_ENTRY {
    PUNICODE_STRING ValueName;
    ULONG           DataLength;
    ULONG           DataOffset;
    ULONG           Type;
} KEY_VALUE_ENTRY, *PKEY_VALUE_ENTRY;

typedef enum _KEY_VALUE_INFORMATION_CLASS {
    KeyValueBasicInformation,
    KeyValueFullInformation,
    KeyValuePartialInformation,
    KeyValueFullInformationAlign64,
    KeyValuePartialInformationAlign64
} KEY_VALUE_INFORMATION_CLASS;



//
// Section Information Structures.
//

typedef enum _SECTION_INHERIT {
    ViewShare = 1,
    ViewUnmap = 2
} SECTION_INHERIT;

//
// Section Access Rights.
//

// begin_winnt
#define SECTION_QUERY       0x0001
#define SECTION_MAP_WRITE   0x0002
#define SECTION_MAP_READ    0x0004
#define SECTION_MAP_EXECUTE 0x0008
#define SECTION_EXTEND_SIZE 0x0010

#define SECTION_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED|SECTION_QUERY|\
                            SECTION_MAP_WRITE |      \
                            SECTION_MAP_READ |       \
                            SECTION_MAP_EXECUTE |    \
                            SECTION_EXTEND_SIZE)
// end_winnt

#define SEGMENT_ALL_ACCESS SECTION_ALL_ACCESS

#define PAGE_NOACCESS          0x01     // winnt
#define PAGE_READONLY          0x02     // winnt
#define PAGE_READWRITE         0x04     // winnt
#define PAGE_WRITECOPY         0x08     // winnt
#define PAGE_EXECUTE           0x10     // winnt
#define PAGE_EXECUTE_READ      0x20     // winnt
#define PAGE_EXECUTE_READWRITE 0x40     // winnt
#define PAGE_EXECUTE_WRITECOPY 0x80     // winnt
#define PAGE_GUARD            0x100     // winnt
#define PAGE_NOCACHE          0x200     // winnt
#define PAGE_WRITECOMBINE     0x400     // winnt

#define MEM_COMMIT           0x1000     
#define MEM_RESERVE          0x2000     
#define MEM_DECOMMIT         0x4000     
#define MEM_RELEASE          0x8000     
#define MEM_FREE            0x10000     
#define MEM_PRIVATE         0x20000     
#define MEM_MAPPED          0x40000     
#define MEM_RESET           0x80000     
#define MEM_TOP_DOWN       0x100000     
#define MEM_LARGE_PAGES  0x20000000     
#define MEM_4MB_PAGES    0x80000000     
#define SEC_RESERVE       0x4000000     

#if defined(_X86_)

//
// Define system time structure.
//

typedef struct _KSYSTEM_TIME {
    ULONG LowPart;
    LONG High1Time;
    LONG High2Time;
} KSYSTEM_TIME, *PKSYSTEM_TIME;

#endif


#ifdef _X86_

//
// Disable these two pramas that evaluate to "sti" "cli" on x86 so that driver
// writers to not leave them inadvertantly in their code.
//

#if !defined(MIDL_PASS)
#if !defined(RC_INVOKED)

#if _MSC_VER >= 1200
#pragma warning(push)
#endif
#pragma warning(disable:4164)   // disable C4164 warning so that apps that
                                // build with /Od don't get weird errors !
#ifdef _M_IX86
#pragma function(_enable)
#pragma function(_disable)
#endif

#if _MSC_VER >= 1200
#pragma warning(pop)
#else
#pragma warning(default:4164)   // reenable C4164 warning
#endif

#endif
#endif

//
// Size of kernel mode stack.
//

#define KERNEL_STACK_SIZE 12288

//
// Define size of large kernel mode stack for callbacks.
//

#define KERNEL_LARGE_STACK_SIZE 61440

//
// Define number of pages to initialize in a large kernel stack.
//

#define KERNEL_LARGE_STACK_COMMIT 12288

#ifdef _X86_

//
//  Define the size of the 80387 save area, which is in the context frame.
//

#define SIZE_OF_80387_REGISTERS      80

//
// The following flags control the contents of the CONTEXT structure.
//

#if !defined(RC_INVOKED)

#define CONTEXT_i386    0x00010000    // this assumes that i386 and
#define CONTEXT_i486    0x00010000    // i486 have identical context records

// end_wx86

#define CONTEXT_CONTROL         (CONTEXT_i386 | 0x00000001L) // SS:SP, CS:IP, FLAGS, BP
#define CONTEXT_INTEGER         (CONTEXT_i386 | 0x00000002L) // AX, BX, CX, DX, SI, DI
#define CONTEXT_SEGMENTS        (CONTEXT_i386 | 0x00000004L) // DS, ES, FS, GS
#define CONTEXT_FLOATING_POINT  (CONTEXT_i386 | 0x00000008L) // 387 state
#define CONTEXT_DEBUG_REGISTERS (CONTEXT_i386 | 0x00000010L) // DB 0-3,6,7
#define CONTEXT_EXTENDED_REGISTERS  (CONTEXT_i386 | 0x00000020L) // cpu specific extensions

#define CONTEXT_FULL (CONTEXT_CONTROL | CONTEXT_INTEGER |\
                      CONTEXT_SEGMENTS)

// begin_wx86

#endif

#define MAXIMUM_SUPPORTED_EXTENSION     512

typedef struct _FLOATING_SAVE_AREA {
    ULONG   ControlWord;
    ULONG   StatusWord;
    ULONG   TagWord;
    ULONG   ErrorOffset;
    ULONG   ErrorSelector;
    ULONG   DataOffset;
    ULONG   DataSelector;
    UCHAR   RegisterArea[SIZE_OF_80387_REGISTERS];
    ULONG   Cr0NpxState;
} FLOATING_SAVE_AREA;

typedef FLOATING_SAVE_AREA *PFLOATING_SAVE_AREA;

//
// Context Frame
//
//  This frame has a several purposes: 1) it is used as an argument to
//  NtContinue, 2) is is used to constuct a call frame for APC delivery,
//  and 3) it is used in the user level thread creation routines.
//
//  The layout of the record conforms to a standard call frame.
//

typedef struct _CONTEXT {

    //
    // The flags values within this flag control the contents of
    // a CONTEXT record.
    //
    // If the context record is used as an input parameter, then
    // for each portion of the context record controlled by a flag
    // whose value is set, it is assumed that that portion of the
    // context record contains valid context. If the context record
    // is being used to modify a threads context, then only that
    // portion of the threads context will be modified.
    //
    // If the context record is used as an IN OUT parameter to capture
    // the context of a thread, then only those portions of the thread's
    // context corresponding to set flags will be returned.
    //
    // The context record is never used as an OUT only parameter.
    //

    ULONG ContextFlags;

    //
    // This section is specified/returned if CONTEXT_DEBUG_REGISTERS is
    // set in ContextFlags.  Note that CONTEXT_DEBUG_REGISTERS is NOT
    // included in CONTEXT_FULL.
    //

    ULONG   Dr0;
    ULONG   Dr1;
    ULONG   Dr2;
    ULONG   Dr3;
    ULONG   Dr6;
    ULONG   Dr7;

    //
    // This section is specified/returned if the
    // ContextFlags word contians the flag CONTEXT_FLOATING_POINT.
    //

    FLOATING_SAVE_AREA FloatSave;

    //
    // This section is specified/returned if the
    // ContextFlags word contians the flag CONTEXT_SEGMENTS.
    //

    ULONG   SegGs;
    ULONG   SegFs;
    ULONG   SegEs;
    ULONG   SegDs;

    //
    // This section is specified/returned if the
    // ContextFlags word contians the flag CONTEXT_INTEGER.
    //

    ULONG   Edi;
    ULONG   Esi;
    ULONG   Ebx;
    ULONG   Edx;
    ULONG   Ecx;
    ULONG   Eax;

    //
    // This section is specified/returned if the
    // ContextFlags word contians the flag CONTEXT_CONTROL.
    //

    ULONG   Ebp;
    ULONG   Eip;
    ULONG   SegCs;              // MUST BE SANITIZED
    ULONG   EFlags;             // MUST BE SANITIZED
    ULONG   Esp;
    ULONG   SegSs;

    //
    // This section is specified/returned if the ContextFlags word
    // contains the flag CONTEXT_EXTENDED_REGISTERS.
    // The format and contexts are processor specific
    //

    UCHAR   ExtendedRegisters[MAXIMUM_SUPPORTED_EXTENSION];

} CONTEXT;



typedef CONTEXT *PCONTEXT;

// begin_ntminiport

#endif //_X86_

#endif // _X86_

#if defined(_ALPHA_)

//
// Define system time structure.
//

typedef ULONGLONG KSYSTEM_TIME;
typedef KSYSTEM_TIME *PKSYSTEM_TIME;

#endif

#ifdef _ALPHA_                  
//
// Define size of kernel mode stack.
//

#if defined(_AXP64_)

#define KERNEL_STACK_SIZE 0x6000

#else

#define KERNEL_STACK_SIZE 0x4000

#endif

//
// Define size of large kernel mode stack for callbacks.
//

#define KERNEL_LARGE_STACK_SIZE 65536

//
// Define number of pages to initialize in a large kernel stack.
//

#define KERNEL_LARGE_STACK_COMMIT KERNEL_STACK_SIZE

//
// The following flags control the contents of the CONTEXT structure.
//

#if !defined(RC_INVOKED)

#define CONTEXT_ALPHA              0x00020000

#define CONTEXT_CONTROL         (CONTEXT_ALPHA | 0x00000001L)
#define CONTEXT_FLOATING_POINT  (CONTEXT_ALPHA | 0x00000002L)
#define CONTEXT_INTEGER         (CONTEXT_ALPHA | 0x00000004L)

#define CONTEXT_FULL (CONTEXT_CONTROL | CONTEXT_FLOATING_POINT | CONTEXT_INTEGER)

#endif

#ifndef _PORTABLE_32BIT_CONTEXT

//
// Context Frame
//
//  This frame has a several purposes: 1) it is used as an argument to
//  NtContinue, 2) it is used to construct a call frame for APC delivery,
//  3) it is used to construct a call frame for exception dispatching
//  in user mode, 4) it is used in the user level thread creation
//  routines, and 5) it is used to to pass thread state to debuggers.
//
//  N.B. Because this record is used as a call frame, it must be EXACTLY
//  a multiple of 16 bytes in length.
//
//  There are two variations of the context structure. This is the real one.
//

typedef struct _CONTEXT {

    //
    // This section is specified/returned if the ContextFlags word contains
    // the flag CONTEXT_FLOATING_POINT.
    //

    ULONGLONG FltF0;
    ULONGLONG FltF1;
    ULONGLONG FltF2;
    ULONGLONG FltF3;
    ULONGLONG FltF4;
    ULONGLONG FltF5;
    ULONGLONG FltF6;
    ULONGLONG FltF7;
    ULONGLONG FltF8;
    ULONGLONG FltF9;
    ULONGLONG FltF10;
    ULONGLONG FltF11;
    ULONGLONG FltF12;
    ULONGLONG FltF13;
    ULONGLONG FltF14;
    ULONGLONG FltF15;
    ULONGLONG FltF16;
    ULONGLONG FltF17;
    ULONGLONG FltF18;
    ULONGLONG FltF19;
    ULONGLONG FltF20;
    ULONGLONG FltF21;
    ULONGLONG FltF22;
    ULONGLONG FltF23;
    ULONGLONG FltF24;
    ULONGLONG FltF25;
    ULONGLONG FltF26;
    ULONGLONG FltF27;
    ULONGLONG FltF28;
    ULONGLONG FltF29;
    ULONGLONG FltF30;
    ULONGLONG FltF31;

    //
    // This section is specified/returned if the ContextFlags word contains
    // the flag CONTEXT_INTEGER.
    //
    // N.B. The registers gp, sp, and ra are defined in this section, but are
    //  considered part of the control context rather than part of the integer
    //  context.
    //

    ULONGLONG IntV0;    //  $0: return value register, v0
    ULONGLONG IntT0;    //  $1: temporary registers, t0 - t7
    ULONGLONG IntT1;    //  $2:
    ULONGLONG IntT2;    //  $3:
    ULONGLONG IntT3;    //  $4:
    ULONGLONG IntT4;    //  $5:
    ULONGLONG IntT5;    //  $6:
    ULONGLONG IntT6;    //  $7:
    ULONGLONG IntT7;    //  $8:
    ULONGLONG IntS0;    //  $9: nonvolatile registers, s0 - s5
    ULONGLONG IntS1;    // $10:
    ULONGLONG IntS2;    // $11:
    ULONGLONG IntS3;    // $12:
    ULONGLONG IntS4;    // $13:
    ULONGLONG IntS5;    // $14:
    ULONGLONG IntFp;    // $15: frame pointer register, fp/s6
    ULONGLONG IntA0;    // $16: argument registers, a0 - a5
    ULONGLONG IntA1;    // $17:
    ULONGLONG IntA2;    // $18:
    ULONGLONG IntA3;    // $19:
    ULONGLONG IntA4;    // $20:
    ULONGLONG IntA5;    // $21:
    ULONGLONG IntT8;    // $22: temporary registers, t8 - t11
    ULONGLONG IntT9;    // $23:
    ULONGLONG IntT10;   // $24:
    ULONGLONG IntT11;   // $25:
    ULONGLONG IntRa;    // $26: return address register, ra
    ULONGLONG IntT12;   // $27: temporary register, t12
    ULONGLONG IntAt;    // $28: assembler temp register, at
    ULONGLONG IntGp;    // $29: global pointer register, gp
    ULONGLONG IntSp;    // $30: stack pointer register, sp
    ULONGLONG IntZero;  // $31: zero register, zero

    //
    // This section is specified/returned if the ContextFlags word contains
    // the flag CONTEXT_FLOATING_POINT.
    //

    ULONGLONG Fpcr;     // floating point control register
    ULONGLONG SoftFpcr; // software extension to FPCR

    //
    // This section is specified/returned if the ContextFlags word contains
    // the flag CONTEXT_CONTROL.
    //
    // N.B. The registers gp, sp, and ra are defined in the integer section,
    //   but are considered part of the control context rather than part of
    //   the integer context.
    //

    ULONGLONG Fir;      // (fault instruction) continuation address
    ULONG Psr;          // processor status

    //
    // The flags values within this flag control the contents of
    // a CONTEXT record.
    //
    // If the context record is used as an input parameter, then
    // for each portion of the context record controlled by a flag
    // whose value is set, it is assumed that that portion of the
    // context record contains valid context. If the context record
    // is being used to modify a thread's context, then only that
    // portion of the threads context will be modified.
    //
    // If the context record is used as an IN OUT parameter to capture
    // the context of a thread, then only those portions of the thread's
    // context corresponding to set flags will be returned.
    //
    // The context record is never used as an OUT only parameter.
    //

    ULONG ContextFlags;
    ULONG Fill[4];      // padding for 16-byte stack frame alignment

} CONTEXT, *PCONTEXT;

#else

#error _PORTABLE_32BIT_CONTEXT no longer supported on Alpha.

#endif // _PORTABLE_32BIT_CONTEXT

#endif 
#ifdef _IA64_

//
// Define system time structure (use alpha approach).
//

typedef ULONGLONG KSYSTEM_TIME;
typedef KSYSTEM_TIME *PKSYSTEM_TIME;

#endif // _IA64_

#ifdef _IA64_

// Please contact INTEL to get IA64-specific information
//
// Define size of kernel mode stack.
//

#define KERNEL_STACK_SIZE 0x8000

//
// Define size of large kernel mode stack for callbacks.
//

#define KERNEL_LARGE_STACK_SIZE 0xC000

//
// Define number of pages to initialize in a large kernel stack.
//

#define KERNEL_LARGE_STACK_COMMIT 0x8000

//
// Intel-IA64-Filler ; Define size of kernel mode backing store stack.
//

#define KERNEL_BSTORE_SIZE 0x4000

//
// Intel-IA64-Filler ; Define size of large kernel mode backing store for callbacks.
//

#define KERNEL_LARGE_BSTORE_SIZE 0x8000

//
// Intel-IA64-Filler ; Define number of pages to initialize in a large kernel backing store
//

#define KERNEL_LARGE_BSTORE_COMMIT 0x4000

//
// Define base address for kernel and user space
//

#define UREGION_INDEX 0

#define KREGION_INDEX 7

#define UADDRESS_BASE ((ULONGLONG)UREGION_INDEX << 61)


#define KADDRESS_BASE ((ULONGLONG)KREGION_INDEX << 61)


//
// The following flags control the contents of the CONTEXT structure.
//

#if !defined(RC_INVOKED)

// Please contact INTEL to get IA64-specific information

#endif // !defined(RC_INVOKED)

//
// Context Frame
//
//  This frame has a several purposes: 1) it is used as an argument to
//  NtContinue, 2) it is used to construct a call frame for APC delivery,
//  3) it is used to construct a call frame for exception dispatching
//  in user mode, 4) it is used in the user level thread creation
//  routines, and 5) it is used to to pass thread state to debuggers.
//
//  N.B. Because this record is used as a call frame, it must be EXACTLY
//  a multiple of 16 bytes in length and aligned on a 16-byte boundary.
//

typedef struct _CONTEXT {

    //
    // The flags values within this flag control the contents of
    // a CONTEXT record.
    //
    // If the context record is used as an input parameter, then
    // for each portion of the context record controlled by a flag
    // whose value is set, it is assumed that that portion of the
    // context record contains valid context. If the context record
    // is being used to modify a thread's context, then only that
    // portion of the threads context will be modified.
    //
    // If the context record is used as an IN OUT parameter to capture
    // the context of a thread, then only those portions of the thread's
    // context corresponding to set flags will be returned.
    //
    // The context record is never used as an OUT only parameter.
    //

    ULONG ContextFlags;
    ULONG Fill1[3];         // for alignment of following on 16-byte boundary

    //
    // This section is specified/returned if the ContextFlags word contains
    // the flag CONTEXT_DEBUG.
    //
    // N.B. CONTEXT_DEBUG is *not* part of CONTEXT_FULL.
    //

// Please contact INTEL to get IA64-specific information

} CONTEXT, *PCONTEXT;

//
// Followings are the data structures that required to support Legacy x86
//


//
//  Define the size of the 80387 save area, which is in the context frame.
//

#define SIZE_OF_80387_REGISTERS      80

//
// The following flags control the contents of the CONTEXT structure.
//

#if !defined(RC_INVOKED)

#define CONTEXT_i386    0x00010000    // this assumes that i386 and
#define CONTEXT_i486    0x00010000    // i486 have identical context records
#define CONTEXT_X86     0x00010000    // X86 have identical context records

#define CONTEXT86_CONTROL         (CONTEXT_i386 | 0x00000001L) // SS:SP, CS:IP, FLAGS, BP
#define CONTEXT86_INTEGER         (CONTEXT_i386 | 0x00000002L) // AX, BX, CX, DX, SI, DI
#define CONTEXT86_SEGMENTS        (CONTEXT_i386 | 0x00000004L) // DS, ES, FS, GS
#define CONTEXT86_FLOATING_POINT  (CONTEXT_i386 | 0x00000008L) // 387 state
#define CONTEXT86_DEBUG_REGISTERS (CONTEXT_i386 | 0x00000010L) // DB 0-3,6,7

#define CONTEXT86_FULL (CONTEXT86_CONTROL | CONTEXT86_INTEGER |\
                      CONTEXT86_SEGMENTS)    // context corresponding to set flags will be returned.

#endif // !defined(RC_INVOKED)

typedef struct _FLOATING_SAVE_AREA {
    ULONG   ControlWord;
    ULONG   StatusWord;
    ULONG   TagWord;
    ULONG   ErrorOffset;
    ULONG   ErrorSelector;
    ULONG   DataOffset;
    ULONG   DataSelector;
    UCHAR   RegisterArea[SIZE_OF_80387_REGISTERS];
    ULONG   Cr0NpxState;
} FLOATING_SAVE_AREA;

typedef FLOATING_SAVE_AREA *PFLOATING_SAVE_AREA;

//
// Context Frame
//
//  This frame has a several purposes: 1) it is used as an argument to
//  NtContinue, 2) is is used to constuct a call frame for APC delivery,
//  and 3) it is used in the user level thread creation routines.
//
//  The layout of the record conforms to a standard call frame.
//

typedef struct _CONTEXT86 {

    //
    // The flags values within this flag control the contents of
    // a CONTEXT record.
    //
    // If the context record is used as an input parameter, then
    // for each portion of the context record controlled by a flag
    // whose value is set, it is assumed that that portion of the
    // context record contains valid context. If the context record
    // is being used to modify a threads context, then only that
    // portion of the threads context will be modified.
    //
    // If the context record is used as an IN OUT parameter to capture
    // the context of a thread, then only those portions of the thread's
    // context corresponding to set flags will be returned.
    //
    // The context record is never used as an OUT only parameter.
    //

    ULONG ContextFlags;

    //
    // This section is specified/returned if CONTEXT_DEBUG_REGISTERS is
    // set in ContextFlags.  Note that CONTEXT_DEBUG_REGISTERS is NOT
    // included in CONTEXT_FULL.
    //

    ULONG   Dr0;
    ULONG   Dr1;
    ULONG   Dr2;
    ULONG   Dr3;
    ULONG   Dr6;
    ULONG   Dr7;

    //
    // This section is specified/returned if the
    // ContextFlags word contians the flag CONTEXT_FLOATING_POINT.
    //

    FLOATING_SAVE_AREA FloatSave;

    //
    // This section is specified/returned if the
    // ContextFlags word contians the flag CONTEXT_SEGMENTS.
    //

    ULONG   SegGs;
    ULONG   SegFs;
    ULONG   SegEs;
    ULONG   SegDs;

    //
    // This section is specified/returned if the
    // ContextFlags word contians the flag CONTEXT_INTEGER.
    //
    //

    ULONG   Edi;
    ULONG   Esi;
    ULONG   Ebx;
    ULONG   Edx;
    ULONG   Ecx;
    ULONG   Eax;

    //
    // This section is specified/returned if the
    // ContextFlags word contians the flag CONTEXT_CONTROL.
    //

    ULONG   Ebp;
    ULONG   Eip;
    ULONG   SegCs;              // MUST BE SANITIZED
    ULONG   EFlags;             // MUST BE SANITIZED
    ULONG   Esp;
    ULONG   SegSs;

} CONTEXT86;

// end_winnt

typedef CONTEXT86 *PCONTEXT86;

#define CONTEXT86_TO_PROGRAM_COUNTER(Context) (PVOID)((Context)->Eip)

#define CONTEXT86_LENGTH  (sizeof(CONTEXT86))
#define CONTEXT86_ALIGN   (sizeof(ULONG))
#define CONTEXT86_ROUND   (CONTEXT86_ALIGN - 1)

//
//  GDT selectors - These defines are R0 selector numbers, which means
//                  they happen to match the byte offset relative to
//                  the base of the GDT.
//

#define KGDT_NULL       0
#define KGDT_R0_CODE    8
#define KGDT_R0_DATA    16
#define KGDT_R3_CODE    24
#define KGDT_R3_DATA    32
#define KGDT_TSS        40
#define KGDT_R0_PCR     48
#define KGDT_R3_TEB     56
#define KGDT_VDM_TILE   64
#define KGDT_LDT        72
#define KGDT_DF_TSS     80
#define KGDT_NMI_TSS    88

//
//  GDT selectors - These defines are R0 selector numbers, which means
//                  they happen to match the byte offset relative to
//                  the base of the GDT.
//

#define KGDT_NULL       0
#define KGDT_R3_CODE    24
#define KGDT_R3_DATA    32
#define KGDT_R3_TEB     56
#define KGDT_VDM_TILE   64

//
//  GDT table is now reside in TEB structure, and the entries can only
//  accomerdate 24 entries.  We purposely take away KGDT_NUMBER and use
//  GDT_TABLE_SIZE
//

#define GDT_ENTRIES 24
#define GDT_TABLE_SIZE  (GDT_ENTRIES<<3)


// begin_winnt

#ifndef _LDT_ENTRY_DEFINED
#define _LDT_ENTRY_DEFINED

//
//  LDT descriptor entry
//

typedef struct _LDT_ENTRY {
    USHORT  LimitLow;
    USHORT  BaseLow;
    union {
        struct {
            UCHAR   BaseMid;
            UCHAR   Flags1;     // Declare as bytes to avoid alignment
            UCHAR   Flags2;     // Problems.
            UCHAR   BaseHi;
        } Bytes;
        struct {
            ULONG   BaseMid : 8;
            ULONG   Type : 5;
            ULONG   Dpl : 2;
            ULONG   Pres : 1;
            ULONG   LimitHi : 4;
            ULONG   Sys : 1;
            ULONG   Reserved_0 : 1;
            ULONG   Default_Big : 1;
            ULONG   Granularity : 1;
            ULONG   BaseHi : 8;
        } Bits;
    } HighWord;
} LDT_ENTRY, *PLDT_ENTRY;

#endif // _LDT_ENTRY_DEFINED

// end_winnt

#define LDT_TABLE_SIZE  ( 8 * 1024 * sizeof(LDT_ENTRY) )

//
// Process Ldt Information
//  NtQueryInformationProcess using ProcessLdtInformation
//

typedef struct _LDT_INFORMATION {
    ULONG Start;
    ULONG Length;
    LDT_ENTRY LdtEntries[1];
} PROCESS_LDT_INFORMATION, *PPROCESS_LDT_INFORMATION;

//
// Process Ldt Size
//  NtSetInformationProcess using ProcessLdtSize
//

typedef struct _LDT_SIZE {
    ULONG Length;
} PROCESS_LDT_SIZE, *PPROCESS_LDT_SIZE;

//
// Thread Descriptor Table Entry
//  NtQueryInformationThread using ThreadDescriptorTableEntry
//

// begin_windbgkd

#ifndef _DESCRIPTOR_TABLE_ENTRY_DEFINED
#define _DESCRIPTOR_TABLE_ENTRY_DEFINED

typedef struct _DESCRIPTOR_TABLE_ENTRY {
    ULONG Selector;
    LDT_ENTRY Descriptor;
} DESCRIPTOR_TABLE_ENTRY, *PDESCRIPTOR_TABLE_ENTRY;

#endif // _DESCRIPTOR_TABLE_ENTRY_DEFINED

// end_windbgkd

// begin_winnt

//
// Plabel descriptor structure definition
//

// Please contact INTEL to get IA64-specific information

// end_winnt

#endif // _IA64_
//
// Exception flag definitions.
//

// begin_winnt
#define EXCEPTION_NONCONTINUABLE 0x1    // Noncontinuable exception
// end_winnt

//
// Define maximum number of exception parameters.
//

// begin_winnt
#define EXCEPTION_MAXIMUM_PARAMETERS 15 // maximum number of exception parameters

//
// Exception record definition.
//

typedef struct _EXCEPTION_RECORD {
    NTSTATUS ExceptionCode;
    ULONG ExceptionFlags;
    struct _EXCEPTION_RECORD *ExceptionRecord;
    PVOID ExceptionAddress;
    ULONG NumberParameters;
    ULONG_PTR ExceptionInformation[EXCEPTION_MAXIMUM_PARAMETERS];
    } EXCEPTION_RECORD;

typedef EXCEPTION_RECORD *PEXCEPTION_RECORD;

typedef struct _EXCEPTION_RECORD32 {
    NTSTATUS ExceptionCode;
    ULONG ExceptionFlags;
    ULONG ExceptionRecord;
    ULONG ExceptionAddress;
    ULONG NumberParameters;
    ULONG ExceptionInformation[EXCEPTION_MAXIMUM_PARAMETERS];
} EXCEPTION_RECORD32, *PEXCEPTION_RECORD32;

typedef struct _EXCEPTION_RECORD64 {
    NTSTATUS ExceptionCode;
    ULONG ExceptionFlags;
    ULONG64 ExceptionRecord;
    ULONG64 ExceptionAddress;
    ULONG NumberParameters;
    ULONG __unusedAlignment;
    ULONG64 ExceptionInformation[EXCEPTION_MAXIMUM_PARAMETERS];
} EXCEPTION_RECORD64, *PEXCEPTION_RECORD64;

//
// Typedef for pointer returned by exception_info()
//

typedef struct _EXCEPTION_POINTERS {
    PEXCEPTION_RECORD ExceptionRecord;
    PCONTEXT ContextRecord;
} EXCEPTION_POINTERS, *PEXCEPTION_POINTERS;
// end_winnt

#ifdef _WIN64
#define PORT_MAXIMUM_MESSAGE_LENGTH 512
#else
#define PORT_MAXIMUM_MESSAGE_LENGTH 256
#endif
//
// Define I/O Driver error log packet structure.  This structure is filled in
// by the driver.
//

typedef struct _IO_ERROR_LOG_PACKET {
    UCHAR MajorFunctionCode;
    UCHAR RetryCount;
    USHORT DumpDataSize;
    USHORT NumberOfStrings;
    USHORT StringOffset;
    USHORT EventCategory;
    NTSTATUS ErrorCode;
    ULONG UniqueErrorValue;
    NTSTATUS FinalStatus;
    ULONG SequenceNumber;
    ULONG IoControlCode;
    LARGE_INTEGER DeviceOffset;
    ULONG DumpData[1];
}IO_ERROR_LOG_PACKET, *PIO_ERROR_LOG_PACKET;

//
// Define the I/O error log message.  This message is sent by the error log
// thread over the lpc port.
//

typedef struct _IO_ERROR_LOG_MESSAGE {
    USHORT Type;
    USHORT Size;
    USHORT DriverNameLength;
    LARGE_INTEGER TimeStamp;
    ULONG DriverNameOffset;
    IO_ERROR_LOG_PACKET EntryData;
}IO_ERROR_LOG_MESSAGE, *PIO_ERROR_LOG_MESSAGE;

//
// Define the maximum message size that will be sent over the LPC to the
// application reading the error log entries.
//

//
// Regardless of LPC size restrictions, ERROR_LOG_MAXIMUM_SIZE must remain
// a value that can fit in a UCHAR.
//

#define ERROR_LOG_LIMIT_SIZE (256-16)

//
// This limit, exclusive of IO_ERROR_LOG_MESSAGE_HEADER_LENGTH, also applies
// to IO_ERROR_LOG_MESSAGE_LENGTH
// 

#define IO_ERROR_LOG_MESSAGE_HEADER_LENGTH (sizeof(IO_ERROR_LOG_MESSAGE) -    \
                                            sizeof(IO_ERROR_LOG_PACKET) +     \
                                            (sizeof(WCHAR) * 40))

#define ERROR_LOG_MESSAGE_LIMIT_SIZE                                          \
    (ERROR_LOG_LIMIT_SIZE + IO_ERROR_LOG_MESSAGE_HEADER_LENGTH)

//
// IO_ERROR_LOG_MESSAGE_LENGTH is
// min(PORT_MAXIMUM_MESSAGE_LENGTH, ERROR_LOG_MESSAGE_LIMIT_SIZE)
// 

#define IO_ERROR_LOG_MESSAGE_LENGTH                                           \
    ((PORT_MAXIMUM_MESSAGE_LENGTH > ERROR_LOG_MESSAGE_LIMIT_SIZE) ?           \
        ERROR_LOG_MESSAGE_LIMIT_SIZE :                                        \
        PORT_MAXIMUM_MESSAGE_LENGTH)

//
// Define the maximum packet size a driver can allocate.
//

#define ERROR_LOG_MAXIMUM_SIZE (IO_ERROR_LOG_MESSAGE_LENGTH -                 \
                                IO_ERROR_LOG_MESSAGE_HEADER_LENGTH)

//
// Event Specific Access Rights.
//

#define EVENT_QUERY_STATE       0x0001
#define EVENT_MODIFY_STATE      0x0002  // winnt
#define EVENT_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED|SYNCHRONIZE|0x3) // winnt

//
// Semaphore Specific Access Rights.
//

#define SEMAPHORE_QUERY_STATE       0x0001
#define SEMAPHORE_MODIFY_STATE      0x0002  // winnt

#define SEMAPHORE_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED|SYNCHRONIZE|0x3) // winnt


//
// Defined processor features
//

#define PF_FLOATING_POINT_PRECISION_ERRATA  0   // winnt
#define PF_FLOATING_POINT_EMULATED          1   // winnt
#define PF_COMPARE_EXCHANGE_DOUBLE          2   // winnt
#define PF_MMX_INSTRUCTIONS_AVAILABLE       3   // winnt
#define PF_PPC_MOVEMEM_64BIT_OK             4   // winnt
#define PF_ALPHA_BYTE_INSTRUCTIONS          5   // winnt
#define PF_XMMI_INSTRUCTIONS_AVAILABLE      6   // winnt
#define PF_3DNOW_INSTRUCTIONS_AVAILABLE     7   // winnt
#define PF_RDTSC_INSTRUCTION_AVAILABLE      8   // winnt
#define PF_PAE_ENABLED                      9   // winnt

typedef enum _ALTERNATIVE_ARCHITECTURE_TYPE {
    StandardDesign,                 // None == 0 == standard design
    NEC98x86,                       // NEC PC98xx series on X86
    EndAlternatives                 // past end of known alternatives
} ALTERNATIVE_ARCHITECTURE_TYPE;

// correctly define these run-time definitions for non X86 machines

#ifndef _X86_

#ifndef IsNEC_98
#define IsNEC_98 (FALSE)
#endif

#ifndef IsNotNEC_98
#define IsNotNEC_98 (TRUE)
#endif

#ifndef SetNEC_98
#define SetNEC_98
#endif

#ifndef SetNotNEC_98
#define SetNotNEC_98
#endif

#endif

#define PROCESSOR_FEATURE_MAX 64

#if defined(REMOTE_BOOT)
//
// Defined system flags.
//

/* the following two lines should be tagged with "winnt" when REMOTE_BOOT is on. */
#define SYSTEM_FLAG_REMOTE_BOOT_CLIENT 0x00000001
#define SYSTEM_FLAG_DISKLESS_CLIENT    0x00000002
#endif // defined(REMOTE_BOOT)

//
// Define data shared between kernel and user mode.
//
// N.B. User mode has read only access to this data
//
#ifdef _MAC
#pragma warning( disable : 4121)
#endif

typedef struct _KUSER_SHARED_DATA {

    //
    // Current low 32-bit of tick count and tick count multiplier.
    //
    // N.B. The tick count is updated each time the clock ticks.
    //

    volatile ULONG TickCountLow;
    ULONG TickCountMultiplier;

    //
    // Current 64-bit interrupt time in 100ns units.
    //

    volatile KSYSTEM_TIME InterruptTime;
#if defined(_WIN64)
    volatile LONG InterruptHigh2Time;
#endif

    //
    // Current 64-bit system time in 100ns units.
    //

#if defined(_WIN64)
    volatile ULONG SystemLowTime;
    volatile LONG SystemHigh1Time;
    volatile LONG SystemHigh2Time;
#else
    volatile KSYSTEM_TIME SystemTime;
#endif

    //
    // Current 64-bit time zone bias.
    //

    volatile KSYSTEM_TIME TimeZoneBias;
#if defined(_WIN64)
    volatile LONG TimeZoneBiasHigh2Time;
#endif

    //
    // Support image magic number range for the host system.
    //
    // N.B. This is an inclusive range.
    //

    USHORT ImageNumberLow;
    USHORT ImageNumberHigh;

    //
    // Copy of system root in Unicode
    //

    WCHAR NtSystemRoot[ 260 ];

    //
    // Maximum stack trace depth if tracing enabled.
    //

    ULONG MaxStackTraceDepth;

    //
    // Crypto Exponent
    //

    ULONG CryptoExponent;

    //
    // TimeZoneId
    //

    ULONG TimeZoneId;

    ULONG Reserved2[ 8 ];

    //
    // product type
    //

    NT_PRODUCT_TYPE NtProductType;
    BOOLEAN ProductTypeIsValid;

    //
    // NT Version. Note that each process sees a version from its PEB, but
    // if the process is running with an altered view of the system version,
    // the following two fields are used to correctly identify the version
    //

    ULONG NtMajorVersion;
    ULONG NtMinorVersion;

    //
    // Processor Feature Bits
    //

    BOOLEAN ProcessorFeatures[PROCESSOR_FEATURE_MAX];

    //
    // Reserved fields - do not use
    //
    ULONG Reserved1;
    ULONG Reserved3;

    //
    // Time slippage while in debugger
    //

    volatile ULONG TimeSlip;

    //
    // Alternative system architecture.  Example: NEC PC98xx on x86
    //

    ALTERNATIVE_ARCHITECTURE_TYPE AlternativeArchitecture;

    //
    // If the system is an evaluation unit, the following field contains the
    // date and time that the evaluation unit expires. A value of 0 indicates
    // that there is no expiration. A non-zero value is the UTC absolute time
    // that the system expires.
    //

    LARGE_INTEGER SystemExpirationDate;

    //
    // Suite Support
    //
    ULONG SuiteMask;

#if defined(REMOTE_BOOT)
    //
    // System flags.
    //

    ULONG SystemFlags;

    //
    // Server path to remote boot root. Only valid for remote boot clients.
    //

    WCHAR RemoteBootServerPath[ 260 ];
#endif // defined(REMOTE_BOOT)

    //
    // TRUE if a kernel debugger is connected/enabled
    //
    BOOLEAN KdDebuggerEnabled;

} KUSER_SHARED_DATA, *PKUSER_SHARED_DATA;


#ifdef _MAC
#pragma warning( default : 4121 )
#endif


//
// Interrupt modes.
//

typedef enum _KINTERRUPT_MODE {
    LevelSensitive,
    Latched
    } KINTERRUPT_MODE;

//
// Wait reasons
//

typedef enum _KWAIT_REASON {
    Executive,
    FreePage,
    PageIn,
    PoolAllocation,
    DelayExecution,
    Suspended,
    UserRequest,
    WrExecutive,
    WrFreePage,
    WrPageIn,
    WrPoolAllocation,
    WrDelayExecution,
    WrSuspended,
    WrUserRequest,
    WrEventPair,
    WrQueue,
    WrLpcReceive,
    WrLpcReply,
    WrVirtualMemory,
    WrPageOut,
    WrRendezvous,
    Spare2,
    Spare3,
    Spare4,
    Spare5,
    Spare6,
    WrKernel,
    MaximumWaitReason
    } KWAIT_REASON;

// end_ntddk end_wdm end_nthal

//
// Miscellaneous type definitions
//
// APC state
//

typedef struct _KAPC_STATE {
    LIST_ENTRY ApcListHead[MaximumMode];
    struct _KPROCESS *Process;
    BOOLEAN KernelApcInProgress;
    BOOLEAN KernelApcPending;
    BOOLEAN UserApcPending;
} KAPC_STATE, *PKAPC_STATE, *RESTRICTED_POINTER PRKAPC_STATE;

// begin_ntddk begin_wdm begin_nthal begin_ntndis
//
// Common dispatcher object header
//
// N.B. The size field contains the number of dwords in the structure.
//

typedef struct _DISPATCHER_HEADER {
    UCHAR Type;
    UCHAR Absolute;
    UCHAR Size;
    UCHAR Inserted;
    LONG SignalState;
    LIST_ENTRY WaitListHead;
} DISPATCHER_HEADER;


typedef struct _KWAIT_BLOCK {
    LIST_ENTRY WaitListEntry;
    struct _KTHREAD *RESTRICTED_POINTER Thread;
    PVOID Object;
    struct _KWAIT_BLOCK *RESTRICTED_POINTER NextWaitBlock;
    USHORT WaitKey;
    USHORT WaitType;
} KWAIT_BLOCK, *PKWAIT_BLOCK, *RESTRICTED_POINTER PRKWAIT_BLOCK;

//
// Thread start function
//

typedef
VOID
(*PKSTART_ROUTINE) (
    IN PVOID StartContext
    );

//
// Kernel object structure definitions
//

//
// Device Queue object and entry
//

typedef struct _KDEVICE_QUEUE {
    CSHORT Type;
    CSHORT Size;
    LIST_ENTRY DeviceListHead;
    KSPIN_LOCK Lock;
    BOOLEAN Busy;
} KDEVICE_QUEUE, *PKDEVICE_QUEUE, *RESTRICTED_POINTER PRKDEVICE_QUEUE;

typedef struct _KDEVICE_QUEUE_ENTRY {
    LIST_ENTRY DeviceListEntry;
    ULONG SortKey;
    BOOLEAN Inserted;
} KDEVICE_QUEUE_ENTRY, *PKDEVICE_QUEUE_ENTRY, *RESTRICTED_POINTER PRKDEVICE_QUEUE_ENTRY;

// begin_ntndis
//
// Event object
//

typedef struct _KEVENT {
    DISPATCHER_HEADER Header;
} KEVENT, *PKEVENT, *RESTRICTED_POINTER PRKEVENT;

//
// Define the interrupt service function type and the empty struct
// type.
//
typedef
BOOLEAN
(*PKSERVICE_ROUTINE) (
    IN struct _KINTERRUPT *Interrupt,
    IN PVOID ServiceContext
    );
//
// Mutant object
//

typedef struct _KMUTANT {
    DISPATCHER_HEADER Header;
    LIST_ENTRY MutantListEntry;
    struct _KTHREAD *RESTRICTED_POINTER OwnerThread;
    BOOLEAN Abandoned;
    UCHAR ApcDisable;
} KMUTANT, *PKMUTANT, *RESTRICTED_POINTER PRKMUTANT, KMUTEX, *PKMUTEX, *RESTRICTED_POINTER PRKMUTEX;

// end_ntddk end_wdm
//
// Queue object
//

typedef struct _KQUEUE {
    DISPATCHER_HEADER Header;
    LIST_ENTRY EntryListHead;
    ULONG CurrentCount;
    ULONG MaximumCount;
    LIST_ENTRY ThreadListHead;
} KQUEUE, *PKQUEUE, *RESTRICTED_POINTER PRKQUEUE;

// begin_ntddk begin_wdm
//
//
// Semaphore object
//

typedef struct _KSEMAPHORE {
    DISPATCHER_HEADER Header;
    LONG Limit;
} KSEMAPHORE, *PKSEMAPHORE, *RESTRICTED_POINTER PRKSEMAPHORE;

// begin_ntndis
//
// Timer object
//

typedef struct _KTIMER {
    DISPATCHER_HEADER Header;
    ULARGE_INTEGER DueTime;
    LIST_ENTRY TimerListEntry;
    struct _KDPC *Dpc;
    LONG Period;
} KTIMER, *PKTIMER, *RESTRICTED_POINTER PRKTIMER;

//
// DPC object
//

NTKERNELAPI
VOID
KeInitializeDpc (
    IN PRKDPC Dpc,
    IN PKDEFERRED_ROUTINE DeferredRoutine,
    IN PVOID DeferredContext
    );

NTKERNELAPI
BOOLEAN
KeInsertQueueDpc (
    IN PRKDPC Dpc,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2
    );

NTKERNELAPI
BOOLEAN
KeRemoveQueueDpc (
    IN PRKDPC Dpc
    );

// end_wdm

NTKERNELAPI
VOID
KeSetImportanceDpc (
    IN PRKDPC Dpc,
    IN KDPC_IMPORTANCE Importance
    );

NTKERNELAPI
VOID
KeSetTargetProcessorDpc (
    IN PRKDPC Dpc,
    IN CCHAR Number
    );

// begin_wdm
//
// Device queue object
//

NTKERNELAPI
VOID
KeInitializeDeviceQueue (
    IN PKDEVICE_QUEUE DeviceQueue
    );

NTKERNELAPI
BOOLEAN
KeInsertDeviceQueue (
    IN PKDEVICE_QUEUE DeviceQueue,
    IN PKDEVICE_QUEUE_ENTRY DeviceQueueEntry
    );

NTKERNELAPI
BOOLEAN
KeInsertByKeyDeviceQueue (
    IN PKDEVICE_QUEUE DeviceQueue,
    IN PKDEVICE_QUEUE_ENTRY DeviceQueueEntry,
    IN ULONG SortKey
    );

NTKERNELAPI
PKDEVICE_QUEUE_ENTRY
KeRemoveDeviceQueue (
    IN PKDEVICE_QUEUE DeviceQueue
    );

NTKERNELAPI
PKDEVICE_QUEUE_ENTRY
KeRemoveByKeyDeviceQueue (
    IN PKDEVICE_QUEUE DeviceQueue,
    IN ULONG SortKey
    );

NTKERNELAPI
BOOLEAN
KeRemoveEntryDeviceQueue (
    IN PKDEVICE_QUEUE DeviceQueue,
    IN PKDEVICE_QUEUE_ENTRY DeviceQueueEntry
    );

//
// Kernel dispatcher object functions
//
// Event Object
//


NTKERNELAPI
VOID
KeInitializeEvent (
    IN PRKEVENT Event,
    IN EVENT_TYPE Type,
    IN BOOLEAN State
    );

NTKERNELAPI
VOID
KeClearEvent (
    IN PRKEVENT Event
    );

NTKERNELAPI
LONG
KePulseEvent (
    IN PRKEVENT Event,
    IN KPRIORITY Increment,
    IN BOOLEAN Wait
    );

NTKERNELAPI
LONG
KeReadStateEvent (
    IN PRKEVENT Event
    );

//  begin_wdm

NTKERNELAPI
LONG
KeResetEvent (
    IN PRKEVENT Event
    );

NTKERNELAPI
LONG
KeSetEvent (
    IN PRKEVENT Event,
    IN KPRIORITY Increment,
    IN BOOLEAN Wait
    );

//
// Mutex object
//

NTKERNELAPI
VOID
KeInitializeMutex (
    IN PRKMUTEX Mutex,
    IN ULONG Level
    );

#define KeReadStateMutex(Mutex) KeReadStateMutant(Mutex)

NTKERNELAPI
LONG
KeReleaseMutex (
    IN PRKMUTEX Mutex,
    IN BOOLEAN Wait
    );

// end_ntddk end_wdm
//
// Queue Object.
//

NTKERNELAPI
VOID
KeInitializeQueue (
    IN PRKQUEUE Queue,
    IN ULONG Count OPTIONAL
    );

NTKERNELAPI
LONG
KeReadStateQueue (
    IN PRKQUEUE Queue
    );

NTKERNELAPI
LONG
KeInsertQueue (
    IN PRKQUEUE Queue,
    IN PLIST_ENTRY Entry
    );

NTKERNELAPI
LONG
KeInsertHeadQueue (
    IN PRKQUEUE Queue,
    IN PLIST_ENTRY Entry
    );

NTKERNELAPI
PLIST_ENTRY
KeRemoveQueue (
    IN PRKQUEUE Queue,
    IN KPROCESSOR_MODE WaitMode,
    IN PLARGE_INTEGER Timeout OPTIONAL
    );

PLIST_ENTRY
KeRundownQueue (
    IN PRKQUEUE Queue
    );

// begin_ntddk begin_wdm
//
// Semaphore object
//

NTKERNELAPI
VOID
KeInitializeSemaphore (
    IN PRKSEMAPHORE Semaphore,
    IN LONG Count,
    IN LONG Limit
    );

NTKERNELAPI
LONG
KeReadStateSemaphore (
    IN PRKSEMAPHORE Semaphore
    );

NTKERNELAPI
LONG
KeReleaseSemaphore (
    IN PRKSEMAPHORE Semaphore,
    IN KPRIORITY Increment,
    IN LONG Adjustment,
    IN BOOLEAN Wait
    );


NTKERNELAPI
VOID
KeAttachProcess (
    IN PRKPROCESS Process
    );

NTKERNELAPI
VOID
KeDetachProcess (
    VOID
    );

NTKERNELAPI
VOID
KeStackAttachProcess (
    IN PRKPROCESS PROCESS,
    OUT PRKAPC_STATE ApcState
    );

NTKERNELAPI
VOID
KeUnstackDetachProcess (
    IN PRKAPC_STATE ApcState
    );

NTKERNELAPI                                         
NTSTATUS                                            
KeDelayExecutionThread (                            
    IN KPROCESSOR_MODE WaitMode,                    
    IN BOOLEAN Alertable,                           
    IN PLARGE_INTEGER Interval                      
    );                                              
                                                    
NTKERNELAPI                                         
KPRIORITY                                           
KeQueryPriorityThread (                             
    IN PKTHREAD Thread                              
    );                                              
                                                    
NTKERNELAPI                                         
LONG                                                
KeSetBasePriorityThread (                           
    IN PKTHREAD Thread,                             
    IN LONG Increment                               
    );                                              
                                                    
NTKERNELAPI                                         
KPRIORITY                                           
KeSetPriorityThread (                               
    IN PKTHREAD Thread,                             
    IN KPRIORITY Priority                           
    );                                              
                                                    

#if (defined(_NTDRIVER_) || defined(_NTDDK_) || defined(_NTIFS_) || defined(_NTHAL_)) && !defined(_NTSYSTEM_DRIVER_)

// begin_wdm

NTKERNELAPI
VOID
KeEnterCriticalRegion (
    VOID
    );

NTKERNELAPI
VOID
KeLeaveCriticalRegion (
    VOID
    );

// end_wdm

#else

//++
//
// VOID
// KeEnterCriticalRegion (
//    VOID
//    )
//
//
// Routine Description:
//
//    This function disables kernel APC's.
//
//    N.B. The following code does not require any interlocks. There are
//         two cases of interest: 1) On an MP system, the thread cannot
//         be running on two processors as once, and 2) if the thread is
//         is interrupted to deliver a kernel mode APC which also calls
//         this routine, the values read and stored will stack and unstack
//         properly.
//
// Arguments:
//
//    None.
//
// Return Value:
//
//    None.
//--

#define KeEnterCriticalRegion() KeGetCurrentThread()->KernelApcDisable -= 1;

//++
//
// VOID
// KeLeaveCriticalRegion (
//    VOID
//    )
//
//
// Routine Description:
//
//    This function enables kernel APC's.
//
//    N.B. The following code does not require any interlocks. There are
//         two cases of interest: 1) On an MP system, the thread cannot
//         be running on two processors as once, and 2) if the thread is
//         is interrupted to deliver a kernel mode APC which also calls
//         this routine, the values read and stored will stack and unstack
//         properly.
//
// Arguments:
//
//    None.
//
// Return Value:
//
//    None.
//--

#define KeLeaveCriticalRegion() KiLeaveCriticalRegion()

#endif

//  begin_wdm

//
// Timer object
//

NTKERNELAPI
VOID
KeInitializeTimer (
    IN PKTIMER Timer
    );

NTKERNELAPI
VOID
KeInitializeTimerEx (
    IN PKTIMER Timer,
    IN TIMER_TYPE Type
    );

NTKERNELAPI
BOOLEAN
KeCancelTimer (
    IN PKTIMER
    );

NTKERNELAPI
BOOLEAN
KeReadStateTimer (
    PKTIMER Timer
    );

NTKERNELAPI
BOOLEAN
KeSetTimer (
    IN PKTIMER Timer,
    IN LARGE_INTEGER DueTime,
    IN PKDPC Dpc OPTIONAL
    );

NTKERNELAPI
BOOLEAN
KeSetTimerEx (
    IN PKTIMER Timer,
    IN LARGE_INTEGER DueTime,
    IN LONG Period OPTIONAL,
    IN PKDPC Dpc OPTIONAL
    );


#define KeWaitForMutexObject KeWaitForSingleObject

NTKERNELAPI
NTSTATUS
KeWaitForMultipleObjects (
    IN ULONG Count,
    IN PVOID Object[],
    IN WAIT_TYPE WaitType,
    IN KWAIT_REASON WaitReason,
    IN KPROCESSOR_MODE WaitMode,
    IN BOOLEAN Alertable,
    IN PLARGE_INTEGER Timeout OPTIONAL,
    IN PKWAIT_BLOCK WaitBlockArray OPTIONAL
    );

NTKERNELAPI
NTSTATUS
KeWaitForSingleObject (
    IN PVOID Object,
    IN KWAIT_REASON WaitReason,
    IN KPROCESSOR_MODE WaitMode,
    IN BOOLEAN Alertable,
    IN PLARGE_INTEGER Timeout OPTIONAL
    );


//
// On X86 the following routines are defined in the HAL and imported by
// all other modules.
//

#if defined(_X86_) && !defined(_NTHAL_)

#define _DECL_HAL_KE_IMPORT  __declspec(dllimport)

#else

#define _DECL_HAL_KE_IMPORT

#endif

//
// spin lock functions
//

NTKERNELAPI
VOID
NTAPI
KeInitializeSpinLock (
    IN PKSPIN_LOCK SpinLock
    );

#if defined(_X86_)

NTKERNELAPI
VOID
FASTCALL
KefAcquireSpinLockAtDpcLevel (
    IN PKSPIN_LOCK SpinLock
    );

NTKERNELAPI
VOID
FASTCALL
KefReleaseSpinLockFromDpcLevel (
    IN PKSPIN_LOCK SpinLock
    );

#define KeAcquireSpinLockAtDpcLevel(a)      KefAcquireSpinLockAtDpcLevel(a)
#define KeReleaseSpinLockFromDpcLevel(a)    KefReleaseSpinLockFromDpcLevel(a)

_DECL_HAL_KE_IMPORT
KIRQL
FASTCALL
KfAcquireSpinLock (
    IN PKSPIN_LOCK SpinLock
    );

_DECL_HAL_KE_IMPORT
VOID
FASTCALL
KfReleaseSpinLock (
    IN PKSPIN_LOCK SpinLock,
    IN KIRQL NewIrql
    );

// end_wdm

_DECL_HAL_KE_IMPORT
KIRQL
FASTCALL
KeAcquireSpinLockRaiseToSynch (
    IN PKSPIN_LOCK SpinLock
    );

// begin_wdm

#define KeAcquireSpinLock(a,b)  *(b) = KfAcquireSpinLock(a)
#define KeReleaseSpinLock(a,b)  KfReleaseSpinLock(a,b)

#else

NTKERNELAPI
KIRQL
FASTCALL
KeAcquireSpinLockRaiseToSynch (
    IN PKSPIN_LOCK SpinLock
    );

NTKERNELAPI
VOID
KeAcquireSpinLockAtDpcLevel (
    IN PKSPIN_LOCK SpinLock
    );

NTKERNELAPI
VOID
KeReleaseSpinLockFromDpcLevel (
    IN PKSPIN_LOCK SpinLock
    );

NTKERNELAPI
KIRQL
KeAcquireSpinLockRaiseToDpc (
    IN PKSPIN_LOCK SpinLock
    );

#define KeAcquireSpinLock(SpinLock, OldIrql) \
    *(OldIrql) = KeAcquireSpinLockRaiseToDpc(SpinLock)

NTKERNELAPI
VOID
KeReleaseSpinLock (
    IN PKSPIN_LOCK SpinLock,
    IN KIRQL NewIrql
    );

#endif


#if defined(_X86_)

_DECL_HAL_KE_IMPORT
VOID
FASTCALL
KfLowerIrql (
    IN KIRQL NewIrql
    );

_DECL_HAL_KE_IMPORT
KIRQL
FASTCALL
KfRaiseIrql (
    IN KIRQL NewIrql
    );

// end_wdm

_DECL_HAL_KE_IMPORT
KIRQL
KeRaiseIrqlToDpcLevel(
    VOID
    );

_DECL_HAL_KE_IMPORT
KIRQL
KeRaiseIrqlToSynchLevel(
    VOID
    );

// begin_wdm

#define KeLowerIrql(a)      KfLowerIrql(a)
#define KeRaiseIrql(a,b)    *(b) = KfRaiseIrql(a)

// end_wdm

// begin_wdm

#elif defined(_ALPHA_)

#define KeLowerIrql(a)      __swpirql(a)
#define KeRaiseIrql(a,b)    *(b) = __swpirql(a)

// end_wdm

#define KfRaiseIrql(a)      __swpirql(a)
#define KeRaiseIrqlToDpcLevel() __swpirql(DISPATCH_LEVEL)
#define KeRaiseIrqlToSynchLevel() __swpirql((UCHAR)KiSynchIrql)

// begin_wdm

#elif defined(_IA64_)

VOID
KeLowerIrql (
    IN KIRQL NewIrql
    );

VOID
KeRaiseIrql (
    IN KIRQL NewIrql,
    OUT PKIRQL OldIrql
    );

// end_wdm

KIRQL
KeRaiseIrqlToDpcLevel (
    VOID
    );

KIRQL
KeRaiseIrqlToSynchLevel (
    VOID
    );

// begin_wdm

#endif

//
// Miscellaneous kernel functions
//

// end_wdm

BOOLEAN
KeGetBugMessageText(
    IN ULONG MessageId,
    IN PANSI_STRING ReturnedString OPTIONAL
    );

typedef enum _KBUGCHECK_BUFFER_DUMP_STATE {
    BufferEmpty,
    BufferInserted,
    BufferStarted,
    BufferFinished,
    BufferIncomplete
} KBUGCHECK_BUFFER_DUMP_STATE;

typedef
VOID
(*PKBUGCHECK_CALLBACK_ROUTINE) (
    IN PVOID Buffer,
    IN ULONG Length
    );

typedef struct _KBUGCHECK_CALLBACK_RECORD {
    LIST_ENTRY Entry;
    PKBUGCHECK_CALLBACK_ROUTINE CallbackRoutine;
    PVOID Buffer;
    ULONG Length;
    PUCHAR Component;
    ULONG_PTR Checksum;
    UCHAR State;
} KBUGCHECK_CALLBACK_RECORD, *PKBUGCHECK_CALLBACK_RECORD;

NTKERNELAPI
DECLSPEC_NORETURN
VOID
NTAPI
KeBugCheck (
    IN ULONG BugCheckCode
    );

// begin_wdm

NTKERNELAPI
DECLSPEC_NORETURN
VOID
KeBugCheckEx(
    IN ULONG BugCheckCode,
    IN ULONG_PTR BugCheckParameter1,
    IN ULONG_PTR BugCheckParameter2,
    IN ULONG_PTR BugCheckParameter3,
    IN ULONG_PTR BugCheckParameter4
    );

// end_wdm

#define KeInitializeCallbackRecord(CallbackRecord) \
    (CallbackRecord)->State = BufferEmpty

NTKERNELAPI
BOOLEAN
KeDeregisterBugCheckCallback (
    IN PKBUGCHECK_CALLBACK_RECORD CallbackRecord
    );

NTKERNELAPI
BOOLEAN
KeRegisterBugCheckCallback (
    IN PKBUGCHECK_CALLBACK_RECORD CallbackRecord,
    IN PKBUGCHECK_CALLBACK_ROUTINE CallbackRoutine,
    IN PVOID Buffer,
    IN ULONG Length,
    IN PUCHAR Component
    );

NTKERNELAPI
VOID
KeEnterKernelDebugger (
    VOID
    );


NTKERNELAPI
ULONGLONG
KeQueryInterruptTime (
    VOID
    );

NTKERNELAPI
VOID
KeQuerySystemTime (
    OUT PLARGE_INTEGER CurrentTime
    );

NTKERNELAPI
ULONG
KeQueryTimeIncrement (
    VOID
    );

// end_wdm
NTKERNELAPI
KAFFINITY
KeQueryActiveProcessors (
    VOID
    );

//
// Context swap notify routine.
//

typedef
VOID
(FASTCALL *PSWAP_CONTEXT_NOTIFY_ROUTINE)(
    IN HANDLE OldThreadId,
    IN HANDLE NewThreadId
    );

NTKERNELAPI
VOID
FASTCALL
KeSetSwapContextNotifyRoutine(
    IN PSWAP_CONTEXT_NOTIFY_ROUTINE NotifyRoutine
    );

//
// Thread select notify routine.
//

typedef
LOGICAL
(FASTCALL *PTHREAD_SELECT_NOTIFY_ROUTINE)(
    IN HANDLE ThreadId
    );

NTKERNELAPI
VOID
FASTCALL
KeSetThreadSelectNotifyRoutine(
    IN PTHREAD_SELECT_NOTIFY_ROUTINE NotifyRoutine
    );

//
// Time update notify routine.
//

typedef
VOID
(FASTCALL *PTIME_UPDATE_NOTIFY_ROUTINE)(
    IN HANDLE ThreadId,
    IN KPROCESSOR_MODE Mode
    );

NTKERNELAPI
VOID
FASTCALL
KeSetTimeUpdateNotifyRoutine(
    IN PTIME_UPDATE_NOTIFY_ROUTINE NotifyRoutine
    );

extern volatile KSYSTEM_TIME KeTickCount;           

typedef enum _MEMORY_CACHING_TYPE_ORIG {
    MmFrameBufferCached = 2
} MEMORY_CACHING_TYPE_ORIG;

typedef enum _MEMORY_CACHING_TYPE {
    MmNonCached = FALSE,
    MmCached = TRUE,
    MmWriteCombined = MmFrameBufferCached,
    MmHardwareCoherentCached,
    MmNonCachedUnordered,       // IA64
    MmUSWCCached,
    MmMaximumCacheType
} MEMORY_CACHING_TYPE;

//
// Pool Allocation routines (in pool.c)
//

typedef enum _POOL_TYPE {
    NonPagedPool,
    PagedPool,
    NonPagedPoolMustSucceed,
    DontUseThisType,
    NonPagedPoolCacheAligned,
    PagedPoolCacheAligned,
    NonPagedPoolCacheAlignedMustS,
    MaxPoolType

    // end_wdm
    ,
    //
    // Note these per session types are carefully chosen so that the appropriate
    // masking still applies as well as MaxPoolType above.
    //

    NonPagedPoolSession = 32,
    PagedPoolSession = NonPagedPoolSession + 1,
    NonPagedPoolMustSucceedSession = PagedPoolSession + 1,
    DontUseThisTypeSession = NonPagedPoolMustSucceedSession + 1,
    NonPagedPoolCacheAlignedSession = DontUseThisTypeSession + 1,
    PagedPoolCacheAlignedSession = NonPagedPoolCacheAlignedSession + 1,
    NonPagedPoolCacheAlignedMustSSession = PagedPoolCacheAlignedSession + 1,

    // begin_wdm

    } POOL_TYPE;

#define POOL_RAISE_IF_ALLOCATION_FAILURE 16               

NTKERNELAPI
PVOID
ExAllocatePool(
    IN POOL_TYPE PoolType,
    IN SIZE_T NumberOfBytes
    );

NTKERNELAPI
PVOID
ExAllocatePoolWithQuota(
    IN POOL_TYPE PoolType,
    IN SIZE_T NumberOfBytes
    );

NTKERNELAPI
PVOID
NTAPI
ExAllocatePoolWithTag(
    IN POOL_TYPE PoolType,
    IN SIZE_T NumberOfBytes,
    IN ULONG Tag
    );

// end_wdm

//
// _EX_POOL_PRIORITY_ provides a method for the system to handle requests
// intelligently in low resource conditions.
//
// LowPoolPriority should be used when it is acceptable to the driver for the
// mapping request to fail if the system is low on resources.  An example of
// this could be for a non-critical network connection where the driver can
// handle the failure case when system resources are close to being depleted.
//
// NormalPoolPriority should be used when it is acceptable to the driver for the
// mapping request to fail if the system is very low on resources.  An example
// of this could be for a non-critical local filesystem request.
//
// HighPoolPriority should be used when it is unacceptable to the driver for the
// mapping request to fail unless the system is completely out of resources.
// An example of this would be the paging file path in a driver.
//
// SpecialPool can be specified to bound the allocation at a page end (or
// beginning).  This should only be done on systems being debugged as the
// memory cost is expensive.
//
// N.B.  These values are very carefully chosen so that the pool allocation
//       code can quickly crack the priority request.
//

typedef enum _EX_POOL_PRIORITY {
    LowPoolPriority,
    LowPoolPrioritySpecialPoolOverrun = 8,
    LowPoolPrioritySpecialPoolUnderrun = 9,
    NormalPoolPriority = 16,
    NormalPoolPrioritySpecialPoolOverrun = 24,
    NormalPoolPrioritySpecialPoolUnderrun = 25,
    HighPoolPriority = 32,
    HighPoolPrioritySpecialPoolOverrun = 40,
    HighPoolPrioritySpecialPoolUnderrun = 41

    } EX_POOL_PRIORITY;

NTKERNELAPI
PVOID
NTAPI
ExAllocatePoolWithTagPriority(
    IN POOL_TYPE PoolType,
    IN SIZE_T NumberOfBytes,
    IN ULONG Tag,
    IN EX_POOL_PRIORITY Priority
    );

// begin_wdm

#ifndef POOL_TAGGING
#define ExAllocatePoolWithTag(a,b,c) ExAllocatePool(a,b)
#endif //POOL_TAGGING

NTKERNELAPI
PVOID
ExAllocatePoolWithQuotaTag(
    IN POOL_TYPE PoolType,
    IN SIZE_T NumberOfBytes,
    IN ULONG Tag
    );

#ifndef POOL_TAGGING
#define ExAllocatePoolWithQuotaTag(a,b,c) ExAllocatePoolWithQuota(a,b)
#endif //POOL_TAGGING

NTKERNELAPI
VOID
NTAPI
ExFreePool(
    IN PVOID P
    );

NTKERNELAPI                                     
ULONG                                           
ExQueryPoolBlockSize (                          
    IN PVOID PoolBlock,                         
    OUT PBOOLEAN QuotaCharged                   
    );                                          
//
// Routines to support fast mutexes.
//

typedef struct _FAST_MUTEX {
    LONG Count;
    PKTHREAD Owner;
    ULONG Contention;
    KEVENT Event;
    ULONG OldIrql;
} FAST_MUTEX, *PFAST_MUTEX;

#if DBG
#define ExInitializeFastMutex(_FastMutex)                            \
    (_FastMutex)->Count = 1;                                         \
    (_FastMutex)->Owner = NULL;                                      \
    (_FastMutex)->Contention = 0;                                    \
    KeInitializeEvent(&(_FastMutex)->Event,                          \
                      SynchronizationEvent,                          \
                      FALSE);
#else
#define ExInitializeFastMutex(_FastMutex)                            \
    (_FastMutex)->Count = 1;                                         \
    (_FastMutex)->Contention = 0;                                    \
    KeInitializeEvent(&(_FastMutex)->Event,                          \
                      SynchronizationEvent,                          \
                      FALSE);
#endif // DBG

NTKERNELAPI
VOID
FASTCALL
ExAcquireFastMutexUnsafe (
    IN PFAST_MUTEX FastMutex
    );

NTKERNELAPI
VOID
FASTCALL
ExReleaseFastMutexUnsafe (
    IN PFAST_MUTEX FastMutex
    );

#if defined(_ALPHA_) || defined(_IA64_)

NTKERNELAPI
VOID
FASTCALL
ExAcquireFastMutex (
    IN PFAST_MUTEX FastMutex
    );

NTKERNELAPI
VOID
FASTCALL
ExReleaseFastMutex (
    IN PFAST_MUTEX FastMutex
    );

// end_wdm

NTKERNELAPI
BOOLEAN
FASTCALL
ExTryToAcquireFastMutex (
    IN PFAST_MUTEX FastMutex
    );

// begin_wdm

#elif defined(_X86_)

NTHALAPI
VOID
FASTCALL
ExAcquireFastMutex (
    IN PFAST_MUTEX FastMutex
    );

NTHALAPI
VOID
FASTCALL
ExReleaseFastMutex (
    IN PFAST_MUTEX FastMutex
    );

// end_wdm

NTHALAPI
BOOLEAN
FASTCALL
ExTryToAcquireFastMutex (
    IN PFAST_MUTEX FastMutex
    );

// begin_wdm

#else

#error "Target architecture not defined"

#endif

//

NTKERNELAPI
VOID
FASTCALL
ExInterlockedAddLargeStatistic (
    IN PLARGE_INTEGER Addend,
    IN ULONG Increment
    );

// end_ntndis

NTKERNELAPI
LARGE_INTEGER
ExInterlockedAddLargeInteger (
    IN PLARGE_INTEGER Addend,
    IN LARGE_INTEGER Increment,
    IN PKSPIN_LOCK Lock
    );


NTKERNELAPI
ULONG
FASTCALL
ExInterlockedAddUlong (
    IN PULONG Addend,
    IN ULONG Increment,
    IN PKSPIN_LOCK Lock
    );


#if defined(_AXP64_)

#define ExInterlockedCompareExchange64(Destination, Exchange, Comperand, Lock) \
    InterlockedCompareExchange64(Destination, *(Exchange), *(Comperand))

#elif defined(_ALPHA_)

#define ExInterlockedCompareExchange64(Destination, Exchange, Comperand, Lock) \
    ExpInterlockedCompareExchange64(Destination, Exchange, Comperand)

#elif defined(_IA64_)

#define ExInterlockedCompareExchange64(Destination, Exchange, Comperand, Lock) \
    InterlockedCompareExchange64(Destination, *(Exchange), *(Comperand))

#else

NTKERNELAPI
LONGLONG
FASTCALL
ExInterlockedCompareExchange64 (
    IN PLONGLONG Destination,
    IN PLONGLONG Exchange,
    IN PLONGLONG Comperand,
    IN PKSPIN_LOCK Lock
    );

#endif

NTKERNELAPI
PLIST_ENTRY
FASTCALL
ExInterlockedInsertHeadList (
    IN PLIST_ENTRY ListHead,
    IN PLIST_ENTRY ListEntry,
    IN PKSPIN_LOCK Lock
    );

NTKERNELAPI
PLIST_ENTRY
FASTCALL
ExInterlockedInsertTailList (
    IN PLIST_ENTRY ListHead,
    IN PLIST_ENTRY ListEntry,
    IN PKSPIN_LOCK Lock
    );

NTKERNELAPI
PLIST_ENTRY
FASTCALL
ExInterlockedRemoveHeadList (
    IN PLIST_ENTRY ListHead,
    IN PKSPIN_LOCK Lock
    );

NTKERNELAPI
PSINGLE_LIST_ENTRY
FASTCALL
ExInterlockedPopEntryList (
    IN PSINGLE_LIST_ENTRY ListHead,
    IN PKSPIN_LOCK Lock
    );

NTKERNELAPI
PSINGLE_LIST_ENTRY
FASTCALL
ExInterlockedPushEntryList (
    IN PSINGLE_LIST_ENTRY ListHead,
    IN PSINGLE_LIST_ENTRY ListEntry,
    IN PKSPIN_LOCK Lock
    );

// begin_ntndis

//
// Define interlocked sequenced listhead functions.
//
// A sequenced interlocked list is a singly linked list with a header that
// contains the current depth and a sequence number. Each time an entry is
// inserted or removed from the list the depth is updated and the sequence
// number is incremented. This enables MIPS, Alpha, and Pentium and later
// machines to insert and remove from the list without the use of spinlocks.
// The PowerPc, however, must use a spinlock to synchronize access to the
// list.
//
// N.B. A spinlock must be specified with SLIST operations. However, it may
//      not actually be used.
//

/*++

VOID
ExInitializeSListHead (
    IN PSLIST_HEADER SListHead
    )

Routine Description:

    This function initializes a sequenced singly linked listhead.

Arguments:

    SListHead - Supplies a pointer to a sequenced singly linked listhead.

Return Value:

    None.

--*/

#define ExInitializeSListHead(_listhead_) (_listhead_)->Alignment = 0

/*++

USHORT
ExQueryDepthSList (
    IN PSLIST_HEADERT SListHead
    )

Routine Description:

    This function queries the current number of entries contained in a
    sequenced single linked list.

Arguments:

    SListHead - Supplies a pointer to the sequenced listhead which is
        be queried.

Return Value:

    The current number of entries in the sequenced singly linked list is
    returned as the function value.

--*/

#define ExQueryDepthSList(_listhead_) (USHORT)(_listhead_)->Depth

#if defined(_MIPS_) || defined(_ALPHA_) || defined(_IA64_)

#define ExInterlockedPopEntrySList(Head, Lock) \
    ExpInterlockedPopEntrySList(Head)

#define ExInterlockedPushEntrySList(Head, Entry, Lock) \
    ExpInterlockedPushEntrySList(Head, Entry)

#define ExInterlockedFlushSList(Head) \
    ExpInterlockedFlushSList(Head)

NTKERNELAPI
PSINGLE_LIST_ENTRY
ExpInterlockedPopEntrySList (
    IN PSLIST_HEADER ListHead
    );

NTKERNELAPI
PSINGLE_LIST_ENTRY
ExpInterlockedPushEntrySList (
    IN PSLIST_HEADER ListHead,
    IN PSINGLE_LIST_ENTRY ListEntry
    );

NTKERNELAPI
PSINGLE_LIST_ENTRY
ExpInterlockedFlushSList (
    IN PSLIST_HEADER ListHead
    );

#else

NTKERNELAPI
PSINGLE_LIST_ENTRY
FASTCALL
ExInterlockedPopEntrySList (
    IN PSLIST_HEADER ListHead,
    IN PKSPIN_LOCK Lock
    );

NTKERNELAPI
PSINGLE_LIST_ENTRY
FASTCALL
ExInterlockedPushEntrySList (
    IN PSLIST_HEADER ListHead,
    IN PSINGLE_LIST_ENTRY ListEntry,
    IN PKSPIN_LOCK Lock
    );

NTKERNELAPI
PSINGLE_LIST_ENTRY
FASTCALL
ExInterlockedFlushSList (
    IN PSLIST_HEADER ListHead
    );

#endif

// end_ntddk end_wdm
//
// Define interlocked lookaside list structure and allocation functions.
//

VOID
ExAdjustLookasideDepth (
    VOID
    );

// begin_ntddk begin_wdm

typedef
PVOID
(*PALLOCATE_FUNCTION) (
    IN POOL_TYPE PoolType,
    IN SIZE_T NumberOfBytes,
    IN ULONG Tag
    );

typedef
VOID
(*PFREE_FUNCTION) (
    IN PVOID Buffer
    );

typedef struct _GENERAL_LOOKASIDE {
    SLIST_HEADER ListHead;
    USHORT Depth;
    USHORT MaximumDepth;
    ULONG TotalAllocates;
    union {
        ULONG AllocateMisses;
        ULONG AllocateHits;
    };

    ULONG TotalFrees;
    union {
        ULONG FreeMisses;
        ULONG FreeHits;
    };

    POOL_TYPE Type;
    ULONG Tag;
    ULONG Size;
    PALLOCATE_FUNCTION Allocate;
    PFREE_FUNCTION Free;
    LIST_ENTRY ListEntry;
    ULONG LastTotalAllocates;
    union {
        ULONG LastAllocateMisses;
        ULONG LastAllocateHits;
    };

    ULONG Future[2];
} GENERAL_LOOKASIDE, *PGENERAL_LOOKASIDE;

typedef struct _NPAGED_LOOKASIDE_LIST {
    GENERAL_LOOKASIDE L;
    KSPIN_LOCK Lock;
} NPAGED_LOOKASIDE_LIST, *PNPAGED_LOOKASIDE_LIST;


NTKERNELAPI
VOID
ExInitializeNPagedLookasideList (
    IN PNPAGED_LOOKASIDE_LIST Lookaside,
    IN PALLOCATE_FUNCTION Allocate,
    IN PFREE_FUNCTION Free,
    IN ULONG Flags,
    IN SIZE_T Size,
    IN ULONG Tag,
    IN USHORT Depth
    );

NTKERNELAPI
VOID
ExDeleteNPagedLookasideList (
    IN PNPAGED_LOOKASIDE_LIST Lookaside
    );

__inline
PVOID
ExAllocateFromNPagedLookasideList(
    IN PNPAGED_LOOKASIDE_LIST Lookaside
    )

/*++

Routine Description:

    This function removes (pops) the first entry from the specified
    nonpaged lookaside list.

Arguments:

    Lookaside - Supplies a pointer to a nonpaged lookaside list structure.

Return Value:

    If an entry is removed from the specified lookaside list, then the
    address of the entry is returned as the function value. Otherwise,
    NULL is returned.

--*/

{

    PVOID Entry;

    Lookaside->L.TotalAllocates += 1;
    Entry = ExInterlockedPopEntrySList(&Lookaside->L.ListHead, &Lookaside->Lock);
    if (Entry == NULL) {
        Lookaside->L.AllocateMisses += 1;
        Entry = (Lookaside->L.Allocate)(Lookaside->L.Type,
                                        Lookaside->L.Size,
                                        Lookaside->L.Tag);
    }

    return Entry;
}

__inline
VOID
ExFreeToNPagedLookasideList(
    IN PNPAGED_LOOKASIDE_LIST Lookaside,
    IN PVOID Entry
    )

/*++

Routine Description:

    This function inserts (pushes) the specified entry into the specified
    nonpaged lookaside list.

Arguments:

    Lookaside - Supplies a pointer to a nonpaged lookaside list structure.

    Entry - Supples a pointer to the entry that is inserted in the
        lookaside list.

Return Value:

    None.

--*/

{

    Lookaside->L.TotalFrees += 1;
    if (ExQueryDepthSList(&Lookaside->L.ListHead) >= Lookaside->L.Depth) {
        Lookaside->L.FreeMisses += 1;
        (Lookaside->L.Free)(Entry);

    } else {
        ExInterlockedPushEntrySList(&Lookaside->L.ListHead,
                                    (PSINGLE_LIST_ENTRY)Entry,
                                    &Lookaside->Lock);
    }

    return;
}

// end_ntndis

typedef struct _PAGED_LOOKASIDE_LIST {
    GENERAL_LOOKASIDE L;
    FAST_MUTEX Lock;
} PAGED_LOOKASIDE_LIST, *PPAGED_LOOKASIDE_LIST;

NTKERNELAPI
VOID
ExInitializePagedLookasideList (
    IN PPAGED_LOOKASIDE_LIST Lookaside,
    IN PALLOCATE_FUNCTION Allocate,
    IN PFREE_FUNCTION Free,
    IN ULONG Flags,
    IN SIZE_T Size,
    IN ULONG Tag,
    IN USHORT Depth
    );

NTKERNELAPI
VOID
ExDeletePagedLookasideList (
    IN PPAGED_LOOKASIDE_LIST Lookaside
    );

#if defined(_X86_)

NTKERNELAPI
PVOID
ExAllocateFromPagedLookasideList(
    IN PPAGED_LOOKASIDE_LIST Lookaside
    );

NTKERNELAPI
VOID
ExFreeToPagedLookasideList(
    IN PPAGED_LOOKASIDE_LIST Lookaside,
    IN PVOID Entry
    );

#else

__inline
PVOID
ExAllocateFromPagedLookasideList(
    IN PPAGED_LOOKASIDE_LIST Lookaside
    )

/*++

Routine Description:

    This function removes (pops) the first entry from the specified
    paged lookaside list.

Arguments:

    Lookaside - Supplies a pointer to a paged lookaside list structure.

Return Value:

    If an entry is removed from the specified lookaside list, then the
    address of the entry is returned as the function value. Otherwise,
    NULL is returned.

--*/

{

    PVOID Entry;

    Lookaside->L.TotalAllocates += 1;
    Entry = ExInterlockedPopEntrySList(&Lookaside->L.ListHead, NULL);
    if (Entry == NULL) {
        Lookaside->L.AllocateMisses += 1;
        Entry = (Lookaside->L.Allocate)(Lookaside->L.Type,
                                        Lookaside->L.Size,
                                        Lookaside->L.Tag);
    }

    return Entry;
}

__inline
VOID
ExFreeToPagedLookasideList(
    IN PPAGED_LOOKASIDE_LIST Lookaside,
    IN PVOID Entry
    )

/*++

Routine Description:

    This function inserts (pushes) the specified entry into the specified
    paged lookaside list.

Arguments:

    Lookaside - Supplies a pointer to a nonpaged lookaside list structure.

    Entry - Supples a pointer to the entry that is inserted in the
        lookaside list.

Return Value:

    None.

--*/

{

    Lookaside->L.TotalFrees += 1;
    if (ExQueryDepthSList(&Lookaside->L.ListHead) >= Lookaside->L.Depth) {
        Lookaside->L.FreeMisses += 1;
        (Lookaside->L.Free)(Entry);

    } else {
        ExInterlockedPushEntrySList(&Lookaside->L.ListHead,
                                    (PSINGLE_LIST_ENTRY)Entry,
                                    NULL);
    }

    return;
}

#endif


NTKERNELAPI
VOID
NTAPI
ProbeForRead(
    IN CONST VOID *Address,
    IN ULONG Length,
    IN ULONG Alignment
    );

//
// Common probe for write functions.
//

NTKERNELAPI
VOID
NTAPI
ProbeForWrite (
    IN PVOID Address,
    IN ULONG Length,
    IN ULONG Alignment
    );

//
// Worker Thread
//

typedef enum _WORK_QUEUE_TYPE {
    CriticalWorkQueue,
    DelayedWorkQueue,
    HyperCriticalWorkQueue,
    MaximumWorkQueue
} WORK_QUEUE_TYPE;

typedef
VOID
(*PWORKER_THREAD_ROUTINE)(
    IN PVOID Parameter
    );

typedef struct _WORK_QUEUE_ITEM {
    LIST_ENTRY List;
    PWORKER_THREAD_ROUTINE WorkerRoutine;
    PVOID Parameter;
} WORK_QUEUE_ITEM, *PWORK_QUEUE_ITEM;


#define ExInitializeWorkItem(Item, Routine, Context) \
    (Item)->WorkerRoutine = (Routine);               \
    (Item)->Parameter = (Context);                   \
    (Item)->List.Flink = NULL;

NTKERNELAPI
VOID
ExQueueWorkItem(
    IN PWORK_QUEUE_ITEM WorkItem,
    IN WORK_QUEUE_TYPE QueueType
    );

//  end_wdm

NTKERNELAPI
BOOLEAN
ExIsProcessorFeaturePresent(
    ULONG ProcessorFeature
    );

//
// Zone Allocation
//

typedef struct _ZONE_SEGMENT_HEADER {
    SINGLE_LIST_ENTRY SegmentList;
    PVOID Reserved;
} ZONE_SEGMENT_HEADER, *PZONE_SEGMENT_HEADER;

typedef struct _ZONE_HEADER {
    SINGLE_LIST_ENTRY FreeList;
    SINGLE_LIST_ENTRY SegmentList;
    ULONG BlockSize;
    ULONG TotalSegmentSize;
} ZONE_HEADER, *PZONE_HEADER;


NTKERNELAPI
NTSTATUS
ExInitializeZone(
    IN PZONE_HEADER Zone,
    IN ULONG BlockSize,
    IN PVOID InitialSegment,
    IN ULONG InitialSegmentSize
    );

NTKERNELAPI
NTSTATUS
ExExtendZone(
    IN PZONE_HEADER Zone,
    IN PVOID Segment,
    IN ULONG SegmentSize
    );

NTKERNELAPI
NTSTATUS
ExInterlockedExtendZone(
    IN PZONE_HEADER Zone,
    IN PVOID Segment,
    IN ULONG SegmentSize,
    IN PKSPIN_LOCK Lock
    );

//++
//
// PVOID
// ExAllocateFromZone(
//     IN PZONE_HEADER Zone
//     )
//
// Routine Description:
//
//     This routine removes an entry from the zone and returns a pointer to it.
//
// Arguments:
//
//     Zone - Pointer to the zone header controlling the storage from which the
//         entry is to be allocated.
//
// Return Value:
//
//     The function value is a pointer to the storage allocated from the zone.
//
//--

#define ExAllocateFromZone(Zone) \
    (PVOID)((Zone)->FreeList.Next); \
    if ( (Zone)->FreeList.Next ) (Zone)->FreeList.Next = (Zone)->FreeList.Next->Next


//++
//
// PVOID
// ExFreeToZone(
//     IN PZONE_HEADER Zone,
//     IN PVOID Block
//     )
//
// Routine Description:
//
//     This routine places the specified block of storage back onto the free
//     list in the specified zone.
//
// Arguments:
//
//     Zone - Pointer to the zone header controlling the storage to which the
//         entry is to be inserted.
//
//     Block - Pointer to the block of storage to be freed back to the zone.
//
// Return Value:
//
//     Pointer to previous block of storage that was at the head of the free
//         list.  NULL implies the zone went from no available free blocks to
//         at least one free block.
//
//--

#define ExFreeToZone(Zone,Block)                                    \
    ( ((PSINGLE_LIST_ENTRY)(Block))->Next = (Zone)->FreeList.Next,  \
      (Zone)->FreeList.Next = ((PSINGLE_LIST_ENTRY)(Block)),        \
      ((PSINGLE_LIST_ENTRY)(Block))->Next                           \
    )

//++
//
// BOOLEAN
// ExIsFullZone(
//     IN PZONE_HEADER Zone
//     )
//
// Routine Description:
//
//     This routine determines if the specified zone is full or not.  A zone
//     is considered full if the free list is empty.
//
// Arguments:
//
//     Zone - Pointer to the zone header to be tested.
//
// Return Value:
//
//     TRUE if the zone is full and FALSE otherwise.
//
//--

#define ExIsFullZone(Zone) \
    ( (Zone)->FreeList.Next == (PSINGLE_LIST_ENTRY)NULL )

//++
//
// PVOID
// ExInterlockedAllocateFromZone(
//     IN PZONE_HEADER Zone,
//     IN PKSPIN_LOCK Lock
//     )
//
// Routine Description:
//
//     This routine removes an entry from the zone and returns a pointer to it.
//     The removal is performed with the specified lock owned for the sequence
//     to make it MP-safe.
//
// Arguments:
//
//     Zone - Pointer to the zone header controlling the storage from which the
//         entry is to be allocated.
//
//     Lock - Pointer to the spin lock which should be obtained before removing
//         the entry from the allocation list.  The lock is released before
//         returning to the caller.
//
// Return Value:
//
//     The function value is a pointer to the storage allocated from the zone.
//
//--

#define ExInterlockedAllocateFromZone(Zone,Lock) \
    (PVOID) ExInterlockedPopEntryList( &(Zone)->FreeList, Lock )

//++
//
// PVOID
// ExInterlockedFreeToZone(
//     IN PZONE_HEADER Zone,
//     IN PVOID Block,
//     IN PKSPIN_LOCK Lock
//     )
//
// Routine Description:
//
//     This routine places the specified block of storage back onto the free
//     list in the specified zone.  The insertion is performed with the lock
//     owned for the sequence to make it MP-safe.
//
// Arguments:
//
//     Zone - Pointer to the zone header controlling the storage to which the
//         entry is to be inserted.
//
//     Block - Pointer to the block of storage to be freed back to the zone.
//
//     Lock - Pointer to the spin lock which should be obtained before inserting
//         the entry onto the free list.  The lock is released before returning
//         to the caller.
//
// Return Value:
//
//     Pointer to previous block of storage that was at the head of the free
//         list.  NULL implies the zone went from no available free blocks to
//         at least one free block.
//
//--

#define ExInterlockedFreeToZone(Zone,Block,Lock) \
    ExInterlockedPushEntryList( &(Zone)->FreeList, ((PSINGLE_LIST_ENTRY) (Block)), Lock )


//++
//
// BOOLEAN
// ExIsObjectInFirstZoneSegment(
//     IN PZONE_HEADER Zone,
//     IN PVOID Object
//     )
//
// Routine Description:
//
//     This routine determines if the specified pointer lives in the zone.
//
// Arguments:
//
//     Zone - Pointer to the zone header controlling the storage to which the
//         object may belong.
//
//     Object - Pointer to the object in question.
//
// Return Value:
//
//     TRUE if the Object came from the first segment of zone.
//
//--

#define ExIsObjectInFirstZoneSegment(Zone,Object) ((BOOLEAN)     \
    (((PUCHAR)(Object) >= (PUCHAR)(Zone)->SegmentList.Next) &&   \
     ((PUCHAR)(Object) < (PUCHAR)(Zone)->SegmentList.Next +      \
                         (Zone)->TotalSegmentSize))              \
)

//
//  Define executive resource data structures.
//

typedef ULONG_PTR ERESOURCE_THREAD;
typedef ERESOURCE_THREAD *PERESOURCE_THREAD;

typedef struct _OWNER_ENTRY {
    ERESOURCE_THREAD OwnerThread;
    union {
        LONG OwnerCount;
        ULONG TableSize;
    };

} OWNER_ENTRY, *POWNER_ENTRY;

typedef struct _ERESOURCE {
    LIST_ENTRY SystemResourcesList;
    POWNER_ENTRY OwnerTable;
    SHORT ActiveCount;
    USHORT Flag;
    PKSEMAPHORE SharedWaiters;
    PKEVENT ExclusiveWaiters;
    OWNER_ENTRY OwnerThreads[2];
    ULONG ContentionCount;
    USHORT NumberOfSharedWaiters;
    USHORT NumberOfExclusiveWaiters;
    union {
        PVOID Address;
        ULONG_PTR CreatorBackTraceIndex;
    };

    KSPIN_LOCK SpinLock;
} ERESOURCE, *PERESOURCE;

//
//  Values for ERESOURCE.Flag
//

#define ResourceNeverExclusive       0x10
#define ResourceReleaseByOtherThread 0x20
#define ResourceOwnedExclusive       0x80

#define RESOURCE_HASH_TABLE_SIZE 64

typedef struct _RESOURCE_HASH_ENTRY {
    LIST_ENTRY ListEntry;
    PVOID Address;
    ULONG ContentionCount;
    ULONG Number;
} RESOURCE_HASH_ENTRY, *PRESOURCE_HASH_ENTRY;

typedef struct _RESOURCE_PERFORMANCE_DATA {
    ULONG ActiveResourceCount;
    ULONG TotalResourceCount;
    ULONG ExclusiveAcquire;
    ULONG SharedFirstLevel;
    ULONG SharedSecondLevel;
    ULONG StarveFirstLevel;
    ULONG StarveSecondLevel;
    ULONG WaitForExclusive;
    ULONG OwnerTableExpands;
    ULONG MaximumTableExpand;
    LIST_ENTRY HashTable[RESOURCE_HASH_TABLE_SIZE];
} RESOURCE_PERFORMANCE_DATA, *PRESOURCE_PERFORMANCE_DATA;

//
// Define executive resource function prototypes.
//

NTKERNELAPI
NTSTATUS
ExInitializeResourceLite(
    IN PERESOURCE Resource
    );

NTKERNELAPI
NTSTATUS
ExReinitializeResourceLite(
    IN PERESOURCE Resource
    );

NTKERNELAPI
BOOLEAN
ExAcquireResourceSharedLite(
    IN PERESOURCE Resource,
    IN BOOLEAN Wait
    );

NTKERNELAPI
BOOLEAN
ExAcquireResourceExclusiveLite(
    IN PERESOURCE Resource,
    IN BOOLEAN Wait
    );

NTKERNELAPI
BOOLEAN
ExAcquireSharedStarveExclusive(
    IN PERESOURCE Resource,
    IN BOOLEAN Wait
    );

NTKERNELAPI
BOOLEAN
ExAcquireSharedWaitForExclusive(
    IN PERESOURCE Resource,
    IN BOOLEAN Wait
    );

NTKERNELAPI
BOOLEAN
ExTryToAcquireResourceExclusiveLite(
    IN PERESOURCE Resource
    );

//
//  VOID
//  ExReleaseResource(
//      IN PERESOURCE Resource
//      );
//

#define ExReleaseResource(R) (ExReleaseResourceLite(R))

NTKERNELAPI
VOID
FASTCALL
ExReleaseResourceLite(
    IN PERESOURCE Resource
    );

NTKERNELAPI
VOID
ExReleaseResourceForThreadLite(
    IN PERESOURCE Resource,
    IN ERESOURCE_THREAD ResourceThreadId
    );

NTKERNELAPI
VOID
ExSetResourceOwnerPointer(
    IN PERESOURCE Resource,
    IN PVOID OwnerPointer
    );

NTKERNELAPI
VOID
ExConvertExclusiveToSharedLite(
    IN PERESOURCE Resource
    );

NTKERNELAPI
NTSTATUS
ExDeleteResourceLite (
    IN PERESOURCE Resource
    );

NTKERNELAPI
ULONG
ExGetExclusiveWaiterCount (
    IN PERESOURCE Resource
    );

NTKERNELAPI
ULONG
ExGetSharedWaiterCount (
    IN PERESOURCE Resource
    );

// end_ntddk

NTKERNELAPI
VOID
ExDisableResourceBoostLite (
    IN PERESOURCE Resource
    );

// begin_ntddk
//
//  ERESOURCE_THREAD
//  ExGetCurrentResourceThread(
//      );
//

#define ExGetCurrentResourceThread() ((ULONG_PTR)PsGetCurrentThread())

NTKERNELAPI
BOOLEAN
ExIsResourceAcquiredExclusiveLite (
    IN PERESOURCE Resource
    );

NTKERNELAPI
ULONG
ExIsResourceAcquiredSharedLite (
    IN PERESOURCE Resource
    );

//
//  ntddk.h stole the entrypoints we wanted so fix them up here.
//

#define ExInitializeResource ExInitializeResourceLite
#define ExAcquireResourceShared ExAcquireResourceSharedLite
#define ExAcquireResourceExclusive ExAcquireResourceExclusiveLite
#define ExReleaseResourceForThread ExReleaseResourceForThreadLite
#define ExConvertExclusiveToShared ExConvertExclusiveToSharedLite
#define ExDeleteResource ExDeleteResourceLite
#define ExIsResourceAcquiredExclusive ExIsResourceAcquiredExclusiveLite
#define ExIsResourceAcquiredShared ExIsResourceAcquiredSharedLite
// end_ntddk
#define ExDisableResourceBoost ExDisableResourceBoostLite
//
// Get previous mode
//

NTKERNELAPI
KPROCESSOR_MODE
ExGetPreviousMode(
    VOID
    );
//
// Raise status from kernel mode.
//

NTKERNELAPI
VOID
NTAPI
ExRaiseStatus (
    IN NTSTATUS Status
    );

// end_wdm

NTKERNELAPI
VOID
ExRaiseDatatypeMisalignment (
    VOID
    );

NTKERNELAPI
VOID
ExRaiseAccessViolation (
    VOID
    );

//
// Set timer resolution.
//

NTKERNELAPI
ULONG
ExSetTimerResolution (
    IN ULONG DesiredTime,
    IN BOOLEAN SetResolution
    );

// end_wdm
//
// Subtract time zone bias from system time to get local time.
//

NTKERNELAPI
VOID
ExSystemTimeToLocalTime (
    IN PLARGE_INTEGER SystemTime,
    OUT PLARGE_INTEGER LocalTime
    );

//
// Add time zone bias to local time to get system time.
//

NTKERNELAPI
VOID
ExLocalTimeToSystemTime (
    IN PLARGE_INTEGER LocalTime,
    OUT PLARGE_INTEGER SystemTime
    );


//
// Define the type for Callback function.
//

typedef struct _CALLBACK_OBJECT *PCALLBACK_OBJECT;

typedef VOID (*PCALLBACK_FUNCTION ) (
    IN PVOID CallbackContext,
    IN PVOID Argument1,
    IN PVOID Argument2
    );


NTKERNELAPI
NTSTATUS
ExCreateCallback (
    OUT PCALLBACK_OBJECT *CallbackObject,
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    IN BOOLEAN Create,
    IN BOOLEAN AllowMultipleCallbacks
    );

NTKERNELAPI
PVOID
ExRegisterCallback (
    IN PCALLBACK_OBJECT CallbackObject,
    IN PCALLBACK_FUNCTION CallbackFunction,
    IN PVOID CallbackContext
    );

NTKERNELAPI
VOID
ExUnregisterCallback (
    IN PVOID CallbackRegistration
    );

NTKERNELAPI
VOID
ExNotifyCallback (
    IN PVOID CallbackObject,
    IN PVOID Argument1,
    IN PVOID Argument2
    );

//
//  Security operation codes
//

typedef enum _SECURITY_OPERATION_CODE {
    SetSecurityDescriptor,
    QuerySecurityDescriptor,
    DeleteSecurityDescriptor,
    AssignSecurityDescriptor
    } SECURITY_OPERATION_CODE, *PSECURITY_OPERATION_CODE;

//
// Token Flags
//
// Flags that may be defined in the TokenFlags field of the token object,
// or in an ACCESS_STATE structure
//

#define TOKEN_HAS_TRAVERSE_PRIVILEGE    0x01
#define TOKEN_HAS_BACKUP_PRIVILEGE      0x02
#define TOKEN_HAS_RESTORE_PRIVILEGE     0x04
#define TOKEN_HAS_ADMIN_GROUP           0x08
#define TOKEN_IS_RESTRICTED             0x10

//
//  Data structure used to capture subject security context
//  for access validations and auditing.
//
//  THE FIELDS OF THIS DATA STRUCTURE SHOULD BE CONSIDERED OPAQUE
//  BY ALL EXCEPT THE SECURITY ROUTINES.
//

typedef struct _SECURITY_SUBJECT_CONTEXT {
    PACCESS_TOKEN ClientToken;
    SECURITY_IMPERSONATION_LEVEL ImpersonationLevel;
    PACCESS_TOKEN PrimaryToken;
    PVOID ProcessAuditId;
    } SECURITY_SUBJECT_CONTEXT, *PSECURITY_SUBJECT_CONTEXT;

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                  ACCESS_STATE and related structures                      //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

//
//  Initial Privilege Set - Room for three privileges, which should
//  be enough for most applications.  This structure exists so that
//  it can be imbedded in an ACCESS_STATE structure.  Use PRIVILEGE_SET
//  for all other references to Privilege sets.
//

#define INITIAL_PRIVILEGE_COUNT         3

typedef struct _INITIAL_PRIVILEGE_SET {
    ULONG PrivilegeCount;
    ULONG Control;
    LUID_AND_ATTRIBUTES Privilege[INITIAL_PRIVILEGE_COUNT];
    } INITIAL_PRIVILEGE_SET, * PINITIAL_PRIVILEGE_SET;



//
// Combine the information that describes the state
// of an access-in-progress into a single structure
//


typedef struct _ACCESS_STATE {
   LUID OperationID;
   BOOLEAN SecurityEvaluated;
   BOOLEAN GenerateAudit;
   BOOLEAN GenerateOnClose;
   BOOLEAN PrivilegesAllocated;
   ULONG Flags;
   ACCESS_MASK RemainingDesiredAccess;
   ACCESS_MASK PreviouslyGrantedAccess;
   ACCESS_MASK OriginalDesiredAccess;
   SECURITY_SUBJECT_CONTEXT SubjectSecurityContext;
   PSECURITY_DESCRIPTOR SecurityDescriptor;
   PVOID AuxData;
   union {
      INITIAL_PRIVILEGE_SET InitialPrivilegeSet;
      PRIVILEGE_SET PrivilegeSet;
      } Privileges;

   BOOLEAN AuditPrivileges;
   UNICODE_STRING ObjectName;
   UNICODE_STRING ObjectTypeName;

   } ACCESS_STATE, *PACCESS_STATE;


typedef struct _SE_EXPORTS {

    //
    // Privilege values
    //

    LUID    SeCreateTokenPrivilege;
    LUID    SeAssignPrimaryTokenPrivilege;
    LUID    SeLockMemoryPrivilege;
    LUID    SeIncreaseQuotaPrivilege;
    LUID    SeUnsolicitedInputPrivilege;
    LUID    SeTcbPrivilege;
    LUID    SeSecurityPrivilege;
    LUID    SeTakeOwnershipPrivilege;
    LUID    SeLoadDriverPrivilege;
    LUID    SeCreatePagefilePrivilege;
    LUID    SeIncreaseBasePriorityPrivilege;
    LUID    SeSystemProfilePrivilege;
    LUID    SeSystemtimePrivilege;
    LUID    SeProfileSingleProcessPrivilege;
    LUID    SeCreatePermanentPrivilege;
    LUID    SeBackupPrivilege;
    LUID    SeRestorePrivilege;
    LUID    SeShutdownPrivilege;
    LUID    SeDebugPrivilege;
    LUID    SeAuditPrivilege;
    LUID    SeSystemEnvironmentPrivilege;
    LUID    SeChangeNotifyPrivilege;
    LUID    SeRemoteShutdownPrivilege;


    //
    // Universally defined Sids
    //


    PSID  SeNullSid;
    PSID  SeWorldSid;
    PSID  SeLocalSid;
    PSID  SeCreatorOwnerSid;
    PSID  SeCreatorGroupSid;


    //
    // Nt defined Sids
    //


    PSID  SeNtAuthoritySid;
    PSID  SeDialupSid;
    PSID  SeNetworkSid;
    PSID  SeBatchSid;
    PSID  SeInteractiveSid;
    PSID  SeLocalSystemSid;
    PSID  SeAliasAdminsSid;
    PSID  SeAliasUsersSid;
    PSID  SeAliasGuestsSid;
    PSID  SeAliasPowerUsersSid;
    PSID  SeAliasAccountOpsSid;
    PSID  SeAliasSystemOpsSid;
    PSID  SeAliasPrintOpsSid;
    PSID  SeAliasBackupOpsSid;

    //
    // New Sids defined for NT5
    //

    PSID  SeAuthenticatedUsersSid;

    PSID  SeRestrictedSid;
    PSID  SeAnonymousLogonSid;

    //
    // New Privileges defined for NT5
    //

    LUID  SeUndockPrivilege;
    LUID  SeSyncAgentPrivilege;
    LUID  SeEnableDelegationPrivilege;

} SE_EXPORTS, *PSE_EXPORTS;

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//              Logon session notification callback routines                 //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

//
//  These callback routines are used to notify file systems that have
//  registered of logon sessions being terminated, so they can cleanup state
//  associated with this logon session
//

typedef NTSTATUS
(*PSE_LOGON_SESSION_TERMINATED_ROUTINE)(
    IN PLUID LogonId);

//
//VOID
//SeDeleteClientSecurity(
//    IN PSECURITY_CLIENT_CONTEXT ClientContext
//    )
//
///*++
//
//Routine Description:
//
//    This service deletes a client security context block,
//    performing whatever cleanup might be necessary to do so.  In
//    particular, reference to any client token is removed.
//
//Arguments:
//
//    ClientContext - Points to the client security context block to be
//        deleted.
//
//
//Return Value:
//
//
//
//--*/
//--

#define SeDeleteClientSecurity(C)  {                                           \
            if (SeTokenType((C)->ClientToken) == TokenPrimary) {               \
                PsDereferencePrimaryToken( (C)->ClientToken );                 \
            } else {                                                           \
                PsDereferenceImpersonationToken( (C)->ClientToken );           \
            }                                                                  \
        }


//++
//
//  PACCESS_TOKEN
//  SeQuerySubjectContextToken(
//      IN PSECURITY_SUBJECT_CONTEXT SubjectContext
//      );
//
//  Routine Description:
//
//      This routine returns the effective token from the subject context,
//      either the client token, if present, or the process token.
//
//  Arguments:
//
//      SubjectContext - Context to query
//
//  Return Value:
//
//      This routine returns the PACCESS_TOKEN for the effective token.
//      The pointer may be passed to SeQueryInformationToken.  This routine
//      does not affect the lock status of the token, i.e. the token is not
//      locked.  If the SubjectContext has been locked, the token remains locked,
//      if not, the token remains unlocked.
//
//--

#define SeQuerySubjectContextToken( SubjectContext ) \
        ( ARGUMENT_PRESENT( ((PSECURITY_SUBJECT_CONTEXT) SubjectContext)->ClientToken) ? \
            ((PSECURITY_SUBJECT_CONTEXT) SubjectContext)->ClientToken : \
            ((PSECURITY_SUBJECT_CONTEXT) SubjectContext)->PrimaryToken )


NTKERNELAPI
VOID
SeCaptureSubjectContext (
    OUT PSECURITY_SUBJECT_CONTEXT SubjectContext
    );

NTKERNELAPI
VOID
SeLockSubjectContext(
    IN PSECURITY_SUBJECT_CONTEXT SubjectContext
    );

NTKERNELAPI
VOID
SeUnlockSubjectContext(
    IN PSECURITY_SUBJECT_CONTEXT SubjectContext
    );

NTKERNELAPI
VOID
SeReleaseSubjectContext (
    IN PSECURITY_SUBJECT_CONTEXT SubjectContext
    );


NTKERNELAPI
NTSTATUS
SeAssignSecurity (
    IN PSECURITY_DESCRIPTOR ParentDescriptor OPTIONAL,
    IN PSECURITY_DESCRIPTOR ExplicitDescriptor,
    OUT PSECURITY_DESCRIPTOR *NewDescriptor,
    IN BOOLEAN IsDirectoryObject,
    IN PSECURITY_SUBJECT_CONTEXT SubjectContext,
    IN PGENERIC_MAPPING GenericMapping,
    IN POOL_TYPE PoolType
    );

NTKERNELAPI
NTSTATUS
SeAssignSecurityEx (
    IN PSECURITY_DESCRIPTOR ParentDescriptor OPTIONAL,
    IN PSECURITY_DESCRIPTOR ExplicitDescriptor OPTIONAL,
    OUT PSECURITY_DESCRIPTOR *NewDescriptor,
    IN GUID *ObjectType OPTIONAL,
    IN BOOLEAN IsDirectoryObject,
    IN ULONG AutoInheritFlags,
    IN PSECURITY_SUBJECT_CONTEXT SubjectContext,
    IN PGENERIC_MAPPING GenericMapping,
    IN POOL_TYPE PoolType
    );

NTKERNELAPI
NTSTATUS
SeDeassignSecurity (
    IN OUT PSECURITY_DESCRIPTOR *SecurityDescriptor
    );

NTKERNELAPI
BOOLEAN
SeAccessCheck (
    IN PSECURITY_DESCRIPTOR SecurityDescriptor,
    IN PSECURITY_SUBJECT_CONTEXT SubjectSecurityContext,
    IN BOOLEAN SubjectContextLocked,
    IN ACCESS_MASK DesiredAccess,
    IN ACCESS_MASK PreviouslyGrantedAccess,
    OUT PPRIVILEGE_SET *Privileges OPTIONAL,
    IN PGENERIC_MAPPING GenericMapping,
    IN KPROCESSOR_MODE AccessMode,
    OUT PACCESS_MASK GrantedAccess,
    OUT PNTSTATUS AccessStatus
    );


NTKERNELAPI
BOOLEAN
SePrivilegeCheck(
    IN OUT PPRIVILEGE_SET RequiredPrivileges,
    IN PSECURITY_SUBJECT_CONTEXT SubjectSecurityContext,
    IN KPROCESSOR_MODE AccessMode
    );

NTKERNELAPI
VOID
SeFreePrivileges(
    IN PPRIVILEGE_SET Privileges
    );


NTKERNELAPI
VOID
SeOpenObjectAuditAlarm (
    IN PUNICODE_STRING ObjectTypeName,
    IN PVOID Object OPTIONAL,
    IN PUNICODE_STRING AbsoluteObjectName OPTIONAL,
    IN PSECURITY_DESCRIPTOR SecurityDescriptor,
    IN PACCESS_STATE AccessState,
    IN BOOLEAN ObjectCreated,
    IN BOOLEAN AccessGranted,
    IN KPROCESSOR_MODE AccessMode,
    OUT PBOOLEAN GenerateOnClose
    );

NTKERNELAPI
VOID
SeOpenObjectForDeleteAuditAlarm (
    IN PUNICODE_STRING ObjectTypeName,
    IN PVOID Object OPTIONAL,
    IN PUNICODE_STRING AbsoluteObjectName OPTIONAL,
    IN PSECURITY_DESCRIPTOR SecurityDescriptor,
    IN PACCESS_STATE AccessState,
    IN BOOLEAN ObjectCreated,
    IN BOOLEAN AccessGranted,
    IN KPROCESSOR_MODE AccessMode,
    OUT PBOOLEAN GenerateOnClose
    );

VOID
SeDeleteObjectAuditAlarm(
    IN PVOID Object,
    IN HANDLE Handle
    );



NTKERNELAPI
BOOLEAN
SeValidSecurityDescriptor(
    IN ULONG Length,
    IN PSECURITY_DESCRIPTOR SecurityDescriptor
    );

NTKERNELAPI                                     
TOKEN_TYPE                                      
SeTokenType(                                    
    IN PACCESS_TOKEN Token                      
    );                                          
NTKERNELAPI                                     
BOOLEAN                                         
SeTokenIsAdmin(                                 
    IN PACCESS_TOKEN Token                      
    );                                          
NTKERNELAPI                                     
BOOLEAN                                         
SeTokenIsRestricted(                            
    IN PACCESS_TOKEN Token                      
    );                                          

NTKERNELAPI
NTSTATUS
SeQueryAuthenticationIdToken(
    IN PACCESS_TOKEN Token,
    OUT PLUID AuthenticationId
    );

NTKERNELAPI
NTSTATUS
SeQuerySessionIdToken(
    IN PACCESS_TOKEN,
    IN PULONG pSessionId
    );

NTKERNELAPI
NTSTATUS
SeSetSessionIdToken(
    IN PACCESS_TOKEN,
    IN ULONG SessionId
    );

NTKERNELAPI
NTSTATUS
SeCreateClientSecurity (
    IN PETHREAD ClientThread,
    IN PSECURITY_QUALITY_OF_SERVICE ClientSecurityQos,
    IN BOOLEAN RemoteSession,
    OUT PSECURITY_CLIENT_CONTEXT ClientContext
    );

NTKERNELAPI
VOID
SeImpersonateClient(
    IN PSECURITY_CLIENT_CONTEXT ClientContext,
    IN PETHREAD ServerThread OPTIONAL
    );

NTKERNELAPI
NTSTATUS
SeImpersonateClientEx(
    IN PSECURITY_CLIENT_CONTEXT ClientContext,
    IN PETHREAD ServerThread OPTIONAL
    );

NTKERNELAPI
NTSTATUS
SeCreateClientSecurityFromSubjectContext (
    IN PSECURITY_SUBJECT_CONTEXT SubjectContext,
    IN PSECURITY_QUALITY_OF_SERVICE ClientSecurityQos,
    IN BOOLEAN ServerIsRemote,
    OUT PSECURITY_CLIENT_CONTEXT ClientContext
    );


NTKERNELAPI
NTSTATUS
SeQuerySecurityDescriptorInfo (
    IN PSECURITY_INFORMATION SecurityInformation,
    OUT PSECURITY_DESCRIPTOR SecurityDescriptor,
    IN OUT PULONG Length,
    IN PSECURITY_DESCRIPTOR *ObjectsSecurityDescriptor
    );

NTKERNELAPI
NTSTATUS
SeSetSecurityDescriptorInfo (
    IN PVOID Object OPTIONAL,
    IN PSECURITY_INFORMATION SecurityInformation,
    IN PSECURITY_DESCRIPTOR SecurityDescriptor,
    IN OUT PSECURITY_DESCRIPTOR *ObjectsSecurityDescriptor,
    IN POOL_TYPE PoolType,
    IN PGENERIC_MAPPING GenericMapping
    );

NTKERNELAPI
NTSTATUS
SeSetSecurityDescriptorInfoEx (
    IN PVOID Object OPTIONAL,
    IN PSECURITY_INFORMATION SecurityInformation,
    IN PSECURITY_DESCRIPTOR ModificationDescriptor,
    IN OUT PSECURITY_DESCRIPTOR *ObjectsSecurityDescriptor,
    IN ULONG AutoInheritFlags,
    IN POOL_TYPE PoolType,
    IN PGENERIC_MAPPING GenericMapping
    );

NTKERNELAPI
NTSTATUS
SeAppendPrivileges(
    PACCESS_STATE AccessState,
    PPRIVILEGE_SET Privileges
    );

NTKERNELAPI                                                     
BOOLEAN                                                         
SeSinglePrivilegeCheck(                                         
    LUID PrivilegeValue,                                        
    KPROCESSOR_MODE PreviousMode                                
    );                                                          
NTKERNELAPI                                                     
BOOLEAN                                                         
SeAuditingFileEvents(                                           
    IN BOOLEAN AccessGranted,                                   
    IN PSECURITY_DESCRIPTOR SecurityDescriptor                  
    );                                                          
NTKERNELAPI                                                     
BOOLEAN                                                         
SeAuditingFileOrGlobalEvents(                                   
    IN BOOLEAN AccessGranted,                                   
    IN PSECURITY_DESCRIPTOR SecurityDescriptor,                 
    IN PSECURITY_SUBJECT_CONTEXT SubjectSecurityContext         
    );                                                          

VOID
SeSetAccessStateGenericMapping (
    PACCESS_STATE AccessState,
    PGENERIC_MAPPING GenericMapping
    );


NTKERNELAPI
NTSTATUS
SeRegisterLogonSessionTerminatedRoutine(
    IN PSE_LOGON_SESSION_TERMINATED_ROUTINE CallbackRoutine
    );

NTKERNELAPI
NTSTATUS
SeUnregisterLogonSessionTerminatedRoutine(
    IN PSE_LOGON_SESSION_TERMINATED_ROUTINE CallbackRoutine
    );

NTKERNELAPI
NTSTATUS
SeMarkLogonSessionForTerminationNotification(
    IN PLUID LogonId
    );

NTKERNELAPI
NTSTATUS
SeQueryInformationToken (
    IN PACCESS_TOKEN Token,
    IN TOKEN_INFORMATION_CLASS TokenInformationClass,
    OUT PVOID *TokenInformation
    );


// **************************************************************
//
//              C A V E A T      P R O G R A M M E R
//
//
//  If you wish to include this file in an NT driver and use SeExports structure
//  defined above, you need to call:
//
//
//      SeEnableAccessToExports()
//
//  exactly once during initialization.
//
//              C A V E A T      P R O G R A M M E R
//
// **************************************************************
#if 0
#define SeEnableAccessToExports() SeExports = *(PSE_EXPORTS *)SeExports;
extern PSE_EXPORTS SeExports;                       // ntifs
#else
#define SeEnableAccessToExports()
extern NTKERNELAPI PSE_EXPORTS SeExports;
#endif

//
// System Thread and Process Creation and Termination
//

NTKERNELAPI
NTSTATUS
PsCreateSystemThread(
    OUT PHANDLE ThreadHandle,
    IN ULONG DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
    IN HANDLE ProcessHandle OPTIONAL,
    OUT PCLIENT_ID ClientId OPTIONAL,
    IN PKSTART_ROUTINE StartRoutine,
    IN PVOID StartContext
    );

NTKERNELAPI
NTSTATUS
PsTerminateSystemThread(
    IN NTSTATUS ExitStatus
    );


typedef
VOID
(*PCREATE_PROCESS_NOTIFY_ROUTINE)(
    IN HANDLE ParentId,
    IN HANDLE ProcessId,
    IN BOOLEAN Create
    );

NTSTATUS
PsSetCreateProcessNotifyRoutine(
    IN PCREATE_PROCESS_NOTIFY_ROUTINE NotifyRoutine,
    IN BOOLEAN Remove
    );

typedef
VOID
(*PCREATE_THREAD_NOTIFY_ROUTINE)(
    IN HANDLE ProcessId,
    IN HANDLE ThreadId,
    IN BOOLEAN Create
    );

NTSTATUS
PsSetCreateThreadNotifyRoutine(
    IN PCREATE_THREAD_NOTIFY_ROUTINE NotifyRoutine
    );

//
// Structures for Load Image Notify
//

typedef struct _IMAGE_INFO {
    union {
        ULONG Properties;
        struct {
            ULONG ImageAddressingMode  : 8;  // code addressing mode
            ULONG SystemModeImage      : 1;  // system mode image
            ULONG ImageMappedToAllPids : 1;  // image mapped into all processes
            ULONG Reserved             : 22;
        };
    };
    PVOID       ImageBase;
    ULONG       ImageSelector;
    SIZE_T      ImageSize;
    ULONG       ImageSectionNumber;
} IMAGE_INFO, *PIMAGE_INFO;

#define IMAGE_ADDRESSING_MODE_32BIT     3

typedef
VOID
(*PLOAD_IMAGE_NOTIFY_ROUTINE)(
    IN PUNICODE_STRING FullImageName,
    IN HANDLE ProcessId,                // pid into which image is being mapped
    IN PIMAGE_INFO ImageInfo
    );

NTSTATUS
PsSetLoadImageNotifyRoutine(
    IN PLOAD_IMAGE_NOTIFY_ROUTINE NotifyRoutine
    );
//
// VOID
// PsDereferencePrimaryToken(
//    IN PACCESS_TOKEN PrimaryToken
//    );
//
#define PsDereferencePrimaryToken(T) (ObDereferenceObject((T)))

//
// VOID
// PsDereferenceImpersonationToken(
//    In PACCESS_TOKEN ImpersonationToken
//    );
//
#define PsDereferenceImpersonationToken(T)                                          \
            {if (ARGUMENT_PRESENT(T)) {                                       \
                (ObDereferenceObject((T)));                                   \
             } else {                                                         \
                ;                                                             \
             }                                                                \
            }

LARGE_INTEGER
PsGetProcessExitTime(
    VOID
    );

BOOLEAN
PsIsThreadTerminating(
    IN PETHREAD Thread
    );

//
// Quota Operations
//

VOID
PsChargePoolQuota(
    IN PEPROCESS Process,
    IN POOL_TYPE PoolType,
    IN ULONG_PTR Amount
    );

VOID
PsReturnPoolQuota(
    IN PEPROCESS Process,
    IN POOL_TYPE PoolType,
    IN ULONG_PTR Amount
    );

HANDLE
PsGetCurrentProcessId( VOID );

HANDLE
PsGetCurrentThreadId( VOID );

BOOLEAN
PsGetVersion(
    PULONG MajorVersion OPTIONAL,
    PULONG MinorVersion OPTIONAL,
    PULONG BuildNumber OPTIONAL,
    PUNICODE_STRING CSDVersion OPTIONAL
    );

//
// for move macros
//
#ifdef _MAC
#ifndef _INC_STRING
#include <string.h>
#endif /* _INC_STRING */
#else
#include <string.h>
#endif // _MAC

#ifndef _SLIST_HEADER_
#define _SLIST_HEADER_

#define SLIST_ENTRY SINGLE_LIST_ENTRY
#define _SLIST_ENTRY _SINGLE_LIST_ENTRY
#define PSLIST_ENTRY PSINGLE_LIST_ENTRY

#endif


//
// If debugging support enabled, define an ASSERT macro that works.  Otherwise
// define the ASSERT macro to expand to an empty expression.
//

#if DBG
NTSYSAPI
VOID
NTAPI
RtlAssert(
    PVOID FailedAssertion,
    PVOID FileName,
    ULONG LineNumber,
    PCHAR Message
    );

#define ASSERT( exp ) \
    if (!(exp)) \
        RtlAssert( #exp, __FILE__, __LINE__, NULL )

#define ASSERTMSG( msg, exp ) \
    if (!(exp)) \
        RtlAssert( #exp, __FILE__, __LINE__, msg )

#else
#define ASSERT( exp )
#define ASSERTMSG( msg, exp )
#endif // DBG

//
//  Doubly-linked list manipulation routines.  Implemented as macros
//  but logically these are procedures.
//

//
//  VOID
//  InitializeListHead(
//      PLIST_ENTRY ListHead
//      );
//

#define InitializeListHead(ListHead) (\
    (ListHead)->Flink = (ListHead)->Blink = (ListHead))

//
//  BOOLEAN
//  IsListEmpty(
//      PLIST_ENTRY ListHead
//      );
//

#define IsListEmpty(ListHead) \
    ((ListHead)->Flink == (ListHead))

//
//  PLIST_ENTRY
//  RemoveHeadList(
//      PLIST_ENTRY ListHead
//      );
//

#define RemoveHeadList(ListHead) \
    (ListHead)->Flink;\
    {RemoveEntryList((ListHead)->Flink)}

//
//  PLIST_ENTRY
//  RemoveTailList(
//      PLIST_ENTRY ListHead
//      );
//

#define RemoveTailList(ListHead) \
    (ListHead)->Blink;\
    {RemoveEntryList((ListHead)->Blink)}

//
//  VOID
//  RemoveEntryList(
//      PLIST_ENTRY Entry
//      );
//

#define RemoveEntryList(Entry) {\
    PLIST_ENTRY _EX_Blink;\
    PLIST_ENTRY _EX_Flink;\
    _EX_Flink = (Entry)->Flink;\
    _EX_Blink = (Entry)->Blink;\
    _EX_Blink->Flink = _EX_Flink;\
    _EX_Flink->Blink = _EX_Blink;\
    }

//
//  VOID
//  InsertTailList(
//      PLIST_ENTRY ListHead,
//      PLIST_ENTRY Entry
//      );
//

#define InsertTailList(ListHead,Entry) {\
    PLIST_ENTRY _EX_Blink;\
    PLIST_ENTRY _EX_ListHead;\
    _EX_ListHead = (ListHead);\
    _EX_Blink = _EX_ListHead->Blink;\
    (Entry)->Flink = _EX_ListHead;\
    (Entry)->Blink = _EX_Blink;\
    _EX_Blink->Flink = (Entry);\
    _EX_ListHead->Blink = (Entry);\
    }

//
//  VOID
//  InsertHeadList(
//      PLIST_ENTRY ListHead,
//      PLIST_ENTRY Entry
//      );
//

#define InsertHeadList(ListHead,Entry) {\
    PLIST_ENTRY _EX_Flink;\
    PLIST_ENTRY _EX_ListHead;\
    _EX_ListHead = (ListHead);\
    _EX_Flink = _EX_ListHead->Flink;\
    (Entry)->Flink = _EX_Flink;\
    (Entry)->Blink = _EX_ListHead;\
    _EX_Flink->Blink = (Entry);\
    _EX_ListHead->Flink = (Entry);\
    }

//
//
//  PSINGLE_LIST_ENTRY
//  PopEntryList(
//      PSINGLE_LIST_ENTRY ListHead
//      );
//

#define PopEntryList(ListHead) \
    (ListHead)->Next;\
    {\
        PSINGLE_LIST_ENTRY FirstEntry;\
        FirstEntry = (ListHead)->Next;\
        if (FirstEntry != NULL) {     \
            (ListHead)->Next = FirstEntry->Next;\
        }                             \
    }


//
//  VOID
//  PushEntryList(
//      PSINGLE_LIST_ENTRY ListHead,
//      PSINGLE_LIST_ENTRY Entry
//      );
//

#define PushEntryList(ListHead,Entry) \
    (Entry)->Next = (ListHead)->Next; \
    (ListHead)->Next = (Entry)

//
//  Define the splay links and the associated manipuliation macros and
//  routines.  Note that the splay_links should be an opaque type.
//  Routine are provided to traverse and manipulate the structure.
//

typedef struct _RTL_SPLAY_LINKS {
    struct _RTL_SPLAY_LINKS *Parent;
    struct _RTL_SPLAY_LINKS *LeftChild;
    struct _RTL_SPLAY_LINKS *RightChild;
} RTL_SPLAY_LINKS;
typedef RTL_SPLAY_LINKS *PRTL_SPLAY_LINKS;

//
//  The macro procedure InitializeSplayLinks takes as input a pointer to
//  splay link and initializes its substructure.  All splay link nodes must
//  be initialized before they are used in the different splay routines and
//  macros.
//
//  VOID
//  RtlInitializeSplayLinks (
//      PRTL_SPLAY_LINKS Links
//      );
//

#define RtlInitializeSplayLinks(Links) {    \
    PRTL_SPLAY_LINKS _SplayLinks;            \
    _SplayLinks = (PRTL_SPLAY_LINKS)(Links); \
    _SplayLinks->Parent = _SplayLinks;   \
    _SplayLinks->LeftChild = NULL;       \
    _SplayLinks->RightChild = NULL;      \
    }

//
//  The macro function Parent takes as input a pointer to a splay link in a
//  tree and returns a pointer to the splay link of the parent of the input
//  node.  If the input node is the root of the tree the return value is
//  equal to the input value.
//
//  PRTL_SPLAY_LINKS
//  RtlParent (
//      PRTL_SPLAY_LINKS Links
//      );
//

#define RtlParent(Links) (           \
    (PRTL_SPLAY_LINKS)(Links)->Parent \
    )

//
//  The macro function LeftChild takes as input a pointer to a splay link in
//  a tree and returns a pointer to the splay link of the left child of the
//  input node.  If the left child does not exist, the return value is NULL.
//
//  PRTL_SPLAY_LINKS
//  RtlLeftChild (
//      PRTL_SPLAY_LINKS Links
//      );
//

#define RtlLeftChild(Links) (           \
    (PRTL_SPLAY_LINKS)(Links)->LeftChild \
    )

//
//  The macro function RightChild takes as input a pointer to a splay link
//  in a tree and returns a pointer to the splay link of the right child of
//  the input node.  If the right child does not exist, the return value is
//  NULL.
//
//  PRTL_SPLAY_LINKS
//  RtlRightChild (
//      PRTL_SPLAY_LINKS Links
//      );
//

#define RtlRightChild(Links) (           \
    (PRTL_SPLAY_LINKS)(Links)->RightChild \
    )

//
//  The macro function IsRoot takes as input a pointer to a splay link
//  in a tree and returns TRUE if the input node is the root of the tree,
//  otherwise it returns FALSE.
//
//  BOOLEAN
//  RtlIsRoot (
//      PRTL_SPLAY_LINKS Links
//      );
//

#define RtlIsRoot(Links) (                          \
    (RtlParent(Links) == (PRTL_SPLAY_LINKS)(Links)) \
    )

//
//  The macro function IsLeftChild takes as input a pointer to a splay link
//  in a tree and returns TRUE if the input node is the left child of its
//  parent, otherwise it returns FALSE.
//
//  BOOLEAN
//  RtlIsLeftChild (
//      PRTL_SPLAY_LINKS Links
//      );
//

#define RtlIsLeftChild(Links) (                                   \
    (RtlLeftChild(RtlParent(Links)) == (PRTL_SPLAY_LINKS)(Links)) \
    )

//
//  The macro function IsRightChild takes as input a pointer to a splay link
//  in a tree and returns TRUE if the input node is the right child of its
//  parent, otherwise it returns FALSE.
//
//  BOOLEAN
//  RtlIsRightChild (
//      PRTL_SPLAY_LINKS Links
//      );
//

#define RtlIsRightChild(Links) (                                   \
    (RtlRightChild(RtlParent(Links)) == (PRTL_SPLAY_LINKS)(Links)) \
    )

//
//  The macro procedure InsertAsLeftChild takes as input a pointer to a splay
//  link in a tree and a pointer to a node not in a tree.  It inserts the
//  second node as the left child of the first node.  The first node must not
//  already have a left child, and the second node must not already have a
//  parent.
//
//  VOID
//  RtlInsertAsLeftChild (
//      PRTL_SPLAY_LINKS ParentLinks,
//      PRTL_SPLAY_LINKS ChildLinks
//      );
//

#define RtlInsertAsLeftChild(ParentLinks,ChildLinks) { \
    PRTL_SPLAY_LINKS _SplayParent;                      \
    PRTL_SPLAY_LINKS _SplayChild;                       \
    _SplayParent = (PRTL_SPLAY_LINKS)(ParentLinks);     \
    _SplayChild = (PRTL_SPLAY_LINKS)(ChildLinks);       \
    _SplayParent->LeftChild = _SplayChild;          \
    _SplayChild->Parent = _SplayParent;             \
    }

//
//  The macro procedure InsertAsRightChild takes as input a pointer to a splay
//  link in a tree and a pointer to a node not in a tree.  It inserts the
//  second node as the right child of the first node.  The first node must not
//  already have a right child, and the second node must not already have a
//  parent.
//
//  VOID
//  RtlInsertAsRightChild (
//      PRTL_SPLAY_LINKS ParentLinks,
//      PRTL_SPLAY_LINKS ChildLinks
//      );
//

#define RtlInsertAsRightChild(ParentLinks,ChildLinks) { \
    PRTL_SPLAY_LINKS _SplayParent;                       \
    PRTL_SPLAY_LINKS _SplayChild;                        \
    _SplayParent = (PRTL_SPLAY_LINKS)(ParentLinks);      \
    _SplayChild = (PRTL_SPLAY_LINKS)(ChildLinks);        \
    _SplayParent->RightChild = _SplayChild;          \
    _SplayChild->Parent = _SplayParent;              \
    }

//
//  The Splay function takes as input a pointer to a splay link in a tree
//  and splays the tree.  Its function return value is a pointer to the
//  root of the splayed tree.
//

NTSYSAPI
PRTL_SPLAY_LINKS
NTAPI
RtlSplay (
    PRTL_SPLAY_LINKS Links
    );

//
//  The Delete function takes as input a pointer to a splay link in a tree
//  and deletes that node from the tree.  Its function return value is a
//  pointer to the root of the tree.  If the tree is now empty, the return
//  value is NULL.
//

NTSYSAPI
PRTL_SPLAY_LINKS
NTAPI
RtlDelete (
    PRTL_SPLAY_LINKS Links
    );

//
//  The DeleteNoSplay function takes as input a pointer to a splay link in a tree,
//  the caller's pointer to the root of the tree and deletes that node from the
//  tree.  Upon return the caller's pointer to the root node will correctly point
//  at the root of the tree.
//
//  It operationally differs from RtlDelete only in that it will not splay the tree.
//

NTSYSAPI
VOID
NTAPI
RtlDeleteNoSplay (
    PRTL_SPLAY_LINKS Links,
    PRTL_SPLAY_LINKS *Root
    );

//
//  The SubtreeSuccessor function takes as input a pointer to a splay link
//  in a tree and returns a pointer to the successor of the input node of
//  the substree rooted at the input node.  If there is not a successor, the
//  return value is NULL.
//

NTSYSAPI
PRTL_SPLAY_LINKS
NTAPI
RtlSubtreeSuccessor (
    PRTL_SPLAY_LINKS Links
    );

//
//  The SubtreePredecessor function takes as input a pointer to a splay link
//  in a tree and returns a pointer to the predecessor of the input node of
//  the substree rooted at the input node.  If there is not a predecessor,
//  the return value is NULL.
//

NTSYSAPI
PRTL_SPLAY_LINKS
NTAPI
RtlSubtreePredecessor (
    PRTL_SPLAY_LINKS Links
    );

//
//  The RealSuccessor function takes as input a pointer to a splay link
//  in a tree and returns a pointer to the successor of the input node within
//  the entire tree.  If there is not a successor, the return value is NULL.
//

NTSYSAPI
PRTL_SPLAY_LINKS
NTAPI
RtlRealSuccessor (
    PRTL_SPLAY_LINKS Links
    );

//
//  The RealPredecessor function takes as input a pointer to a splay link
//  in a tree and returns a pointer to the predecessor of the input node
//  within the entire tree.  If there is not a predecessor, the return value
//  is NULL.
//

NTSYSAPI
PRTL_SPLAY_LINKS
NTAPI
RtlRealPredecessor (
    PRTL_SPLAY_LINKS Links
    );


//
//  Define the generic table package.  Note a generic table should really
//  be an opaque type.  We provide routines to manipulate the structure.
//
//  A generic table is package for inserting, deleting, and looking up elements
//  in a table (e.g., in a symbol table).  To use this package the user
//  defines the structure of the elements stored in the table, provides a
//  comparison function, a memory allocation function, and a memory
//  deallocation function.
//
//  Note: the user compare function must impose a complete ordering among
//  all of the elements, and the table does not allow for duplicate entries.
//

//
// Add an empty typedef so that functions can reference the
// a pointer to the generic table struct before it is declared.
//

struct _RTL_GENERIC_TABLE;

//
//  The results of a compare can be less than, equal, or greater than.
//

typedef enum _RTL_GENERIC_COMPARE_RESULTS {
    GenericLessThan,
    GenericGreaterThan,
    GenericEqual
} RTL_GENERIC_COMPARE_RESULTS;

//
//  The comparison function takes as input pointers to elements containing
//  user defined structures and returns the results of comparing the two
//  elements.
//

typedef
RTL_GENERIC_COMPARE_RESULTS
(NTAPI *PRTL_GENERIC_COMPARE_ROUTINE) (
    struct _RTL_GENERIC_TABLE *Table,
    PVOID FirstStruct,
    PVOID SecondStruct
    );

//
//  The allocation function is called by the generic table package whenever
//  it needs to allocate memory for the table.
//

typedef
PVOID
(NTAPI *PRTL_GENERIC_ALLOCATE_ROUTINE) (
    struct _RTL_GENERIC_TABLE *Table,
    CLONG ByteSize
    );

//
//  The deallocation function is called by the generic table package whenever
//  it needs to deallocate memory from the table that was allocated by calling
//  the user supplied allocation function.
//

typedef
VOID
(NTAPI *PRTL_GENERIC_FREE_ROUTINE) (
    struct _RTL_GENERIC_TABLE *Table,
    PVOID Buffer
    );

//
//  To use the generic table package the user declares a variable of type
//  GENERIC_TABLE and then uses the routines described below to initialize
//  the table and to manipulate the table.  Note that the generic table
//  should really be an opaque type.
//

typedef struct _RTL_GENERIC_TABLE {
    PRTL_SPLAY_LINKS TableRoot;
    LIST_ENTRY InsertOrderList;
    PLIST_ENTRY OrderedPointer;
    ULONG WhichOrderedElement;
    ULONG NumberGenericTableElements;
    PRTL_GENERIC_COMPARE_ROUTINE CompareRoutine;
    PRTL_GENERIC_ALLOCATE_ROUTINE AllocateRoutine;
    PRTL_GENERIC_FREE_ROUTINE FreeRoutine;
    PVOID TableContext;
} RTL_GENERIC_TABLE;
typedef RTL_GENERIC_TABLE *PRTL_GENERIC_TABLE;

//
// This enumerated type is used as the function return value of the function
// that is used to search the tree for a key. FoundNode indicates that the
// function found the key. Insert as left indicates that the key was not found
// and the node should be inserted as the left child of the parent. Insert as
// right indicates that the key was not found and the node should be inserted
//  as the right child of the parent.
//
typedef enum _TABLE_SEARCH_RESULT{
    TableEmptyTree,
    TableFoundNode,
    TableInsertAsLeft,
    TableInsertAsRight
} TABLE_SEARCH_RESULT;

//
//  The procedure InitializeGenericTable takes as input an uninitialized
//  generic table variable and pointers to the three user supplied routines.
//  This must be called for every individual generic table variable before
//  it can be used.
//

NTSYSAPI
VOID
NTAPI
RtlInitializeGenericTable (
    PRTL_GENERIC_TABLE Table,
    PRTL_GENERIC_COMPARE_ROUTINE CompareRoutine,
    PRTL_GENERIC_ALLOCATE_ROUTINE AllocateRoutine,
    PRTL_GENERIC_FREE_ROUTINE FreeRoutine,
    PVOID TableContext
    );

//
//  The function InsertElementGenericTable will insert a new element
//  in a table.  It does this by allocating space for the new element
//  (this includes splay links), inserting the element in the table, and
//  then returning to the user a pointer to the new element.  If an element
//  with the same key already exists in the table the return value is a pointer
//  to the old element.  The optional output parameter NewElement is used
//  to indicate if the element previously existed in the table.  Note: the user
//  supplied Buffer is only used for searching the table, upon insertion its
//  contents are copied to the newly created element.  This means that
//  pointer to the input buffer will not point to the new element.
//

NTSYSAPI
PVOID
NTAPI
RtlInsertElementGenericTable (
    PRTL_GENERIC_TABLE Table,
    PVOID Buffer,
    CLONG BufferSize,
    PBOOLEAN NewElement OPTIONAL
    );

//
//  The function InsertElementGenericTableFull will insert a new element
//  in a table.  It does this by allocating space for the new element
//  (this includes splay links), inserting the element in the table, and
//  then returning to the user a pointer to the new element.  If an element
//  with the same key already exists in the table the return value is a pointer
//  to the old element.  The optional output parameter NewElement is used
//  to indicate if the element previously existed in the table.  Note: the user
//  supplied Buffer is only used for searching the table, upon insertion its
//  contents are copied to the newly created element.  This means that
//  pointer to the input buffer will not point to the new element.
//  This routine is passed the NodeOrParent and SearchResult from a
//  previous RtlLookupElementGenericTableFull.
//

NTSYSAPI
PVOID
NTAPI
RtlInsertElementGenericTableFull (
    PRTL_GENERIC_TABLE Table,
    PVOID Buffer,
    CLONG BufferSize,
    PBOOLEAN NewElement OPTIONAL,
    PVOID NodeOrParent,
    TABLE_SEARCH_RESULT SearchResult
    );

//
//  The function DeleteElementGenericTable will find and delete an element
//  from a generic table.  If the element is located and deleted the return
//  value is TRUE, otherwise if the element is not located the return value
//  is FALSE.  The user supplied input buffer is only used as a key in
//  locating the element in the table.
//

NTSYSAPI
BOOLEAN
NTAPI
RtlDeleteElementGenericTable (
    PRTL_GENERIC_TABLE Table,
    PVOID Buffer
    );

//
//  The function LookupElementGenericTable will find an element in a generic
//  table.  If the element is located the return value is a pointer to
//  the user defined structure associated with the element, otherwise if
//  the element is not located the return value is NULL.  The user supplied
//  input buffer is only used as a key in locating the element in the table.
//

NTSYSAPI
PVOID
NTAPI
RtlLookupElementGenericTable (
    PRTL_GENERIC_TABLE Table,
    PVOID Buffer
    );

//
//  The function LookupElementGenericTableFull will find an element in a generic
//  table.  If the element is located the return value is a pointer to
//  the user defined structure associated with the element.  If the element is not
//  located then a pointer to the parent for the insert location is returned.  The
//  user must look at the SearchResult value to determine which is being returned.
//  The user can use the SearchResult and parent for a subsequent FullInsertElement
//  call to optimize the insert.
//

NTSYSAPI
PVOID
NTAPI
RtlLookupElementGenericTableFull (
    PRTL_GENERIC_TABLE Table,
    PVOID Buffer,
    OUT PVOID *NodeOrParent,
    OUT TABLE_SEARCH_RESULT *SearchResult
    );

//
//  The function EnumerateGenericTable will return to the caller one-by-one
//  the elements of of a table.  The return value is a pointer to the user
//  defined structure associated with the element.  The input parameter
//  Restart indicates if the enumeration should start from the beginning
//  or should return the next element.  If the are no more new elements to
//  return the return value is NULL.  As an example of its use, to enumerate
//  all of the elements in a table the user would write:
//
//      for (ptr = EnumerateGenericTable(Table, TRUE);
//           ptr != NULL;
//           ptr = EnumerateGenericTable(Table, FALSE)) {
//              :
//      }
//
//
//  PLEASE NOTE:
//
//      If you enumerate a GenericTable using RtlEnumerateGenericTable, you
//      will flatten the table, turning it into a sorted linked list.
//      To enumerate the table without perturbing the splay links, use
//      RtlEnumerateGenericTableWithoutSplaying

NTSYSAPI
PVOID
NTAPI
RtlEnumerateGenericTable (
    PRTL_GENERIC_TABLE Table,
    BOOLEAN Restart
    );

//
//  The function EnumerateGenericTableWithoutSplaying will return to the
//  caller one-by-one the elements of of a table.  The return value is a
//  pointer to the user defined structure associated with the element.
//  The input parameter RestartKey indicates if the enumeration should
//  start from the beginning or should return the next element.  If the
//  are no more new elements to return the return value is NULL.  As an
//  example of its use, to enumerate all of the elements in a table the
//  user would write:
//
//      RestartKey = NULL;
//      for (ptr = EnumerateGenericTableWithoutSplaying(Table, &RestartKey);
//           ptr != NULL;
//           ptr = EnumerateGenericTableWithoutSplaying(Table, &RestartKey)) {
//              :
//      }
//
//  If RestartKey is NULL, the package will start from the least entry in the
//  table, otherwise it will start from the last entry returned.
//
//
//  Note that unlike RtlEnumerateGenericTable, this routine will NOT perturb
//  the splay order of the tree.
//

NTSYSAPI
PVOID
NTAPI
RtlEnumerateGenericTableWithoutSplaying (
    PRTL_GENERIC_TABLE Table,
    PVOID *RestartKey
    );

//
// The function GetElementGenericTable will return the i'th element
// inserted in the generic table.  I = 0 implies the first element,
// I = (RtlNumberGenericTableElements(Table)-1) will return the last element
// inserted into the generic table.  The type of I is ULONG.  Values
// of I > than (NumberGenericTableElements(Table)-1) will return NULL.  If
// an arbitrary element is deleted from the generic table it will cause
// all elements inserted after the deleted element to "move up".

NTSYSAPI
PVOID
NTAPI
RtlGetElementGenericTable(
    PRTL_GENERIC_TABLE Table,
    ULONG I
    );

//
// The function NumberGenericTableElements returns a ULONG value
// which is the number of generic table elements currently inserted
// in the generic table.

NTSYSAPI
ULONG
NTAPI
RtlNumberGenericTableElements(
    PRTL_GENERIC_TABLE Table
    );

//
//  The function IsGenericTableEmpty will return to the caller TRUE if
//  the input table is empty (i.e., does not contain any elements) and
//  FALSE otherwise.
//

NTSYSAPI
BOOLEAN
NTAPI
RtlIsGenericTableEmpty (
    PRTL_GENERIC_TABLE Table
    );


#if defined(_M_ALPHA) || defined(_M_AXP64) || defined(_M_IA64)
PVOID
_ReturnAddress (
    VOID
    );

#pragma intrinsic(_ReturnAddress)

#define RtlGetCallersAddress(CallersAddress, CallersCaller) \
    *CallersAddress = (PVOID)_ReturnAddress(); \
    *CallersCaller = NULL;
#else
NTSYSAPI
VOID
NTAPI
RtlGetCallersAddress(
    OUT PVOID *CallersAddress,
    OUT PVOID *CallersCaller
    );
#endif

NTSYSAPI
ULONG
NTAPI
RtlWalkFrameChain (
    OUT PVOID *Callers,
    IN ULONG Count,
    IN ULONG Flags);


typedef NTSTATUS (NTAPI * PRTL_QUERY_REGISTRY_ROUTINE)(
    IN PWSTR ValueName,
    IN ULONG ValueType,
    IN PVOID ValueData,
    IN ULONG ValueLength,
    IN PVOID Context,
    IN PVOID EntryContext
    );

typedef struct _RTL_QUERY_REGISTRY_TABLE {
    PRTL_QUERY_REGISTRY_ROUTINE QueryRoutine;
    ULONG Flags;
    PWSTR Name;
    PVOID EntryContext;
    ULONG DefaultType;
    PVOID DefaultData;
    ULONG DefaultLength;

} RTL_QUERY_REGISTRY_TABLE, *PRTL_QUERY_REGISTRY_TABLE;


//
// The following flags specify how the Name field of a RTL_QUERY_REGISTRY_TABLE
// entry is interpreted.  A NULL name indicates the end of the table.
//

#define RTL_QUERY_REGISTRY_SUBKEY   0x00000001  // Name is a subkey and remainder of
                                                // table or until next subkey are value
                                                // names for that subkey to look at.

#define RTL_QUERY_REGISTRY_TOPKEY   0x00000002  // Reset current key to original key for
                                                // this and all following table entries.

#define RTL_QUERY_REGISTRY_REQUIRED 0x00000004  // Fail if no match found for this table
                                                // entry.

#define RTL_QUERY_REGISTRY_NOVALUE  0x00000008  // Used to mark a table entry that has no
                                                // value name, just wants a call out, not
                                                // an enumeration of all values.

#define RTL_QUERY_REGISTRY_NOEXPAND 0x00000010  // Used to suppress the expansion of
                                                // REG_MULTI_SZ into multiple callouts or
                                                // to prevent the expansion of environment
                                                // variable values in REG_EXPAND_SZ

#define RTL_QUERY_REGISTRY_DIRECT   0x00000020  // QueryRoutine field ignored.  EntryContext
                                                // field points to location to store value.
                                                // For null terminated strings, EntryContext
                                                // points to UNICODE_STRING structure that
                                                // that describes maximum size of buffer.
                                                // If .Buffer field is NULL then a buffer is
                                                // allocated.
                                                //

#define RTL_QUERY_REGISTRY_DELETE   0x00000040  // Used to delete value keys after they
                                                // are queried.

NTSYSAPI
NTSTATUS
NTAPI
RtlQueryRegistryValues(
    IN ULONG RelativeTo,
    IN PCWSTR Path,
    IN PRTL_QUERY_REGISTRY_TABLE QueryTable,
    IN PVOID Context,
    IN PVOID Environment OPTIONAL
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlWriteRegistryValue(
    IN ULONG RelativeTo,
    IN PCWSTR Path,
    IN PCWSTR ValueName,
    IN ULONG ValueType,
    IN PVOID ValueData,
    IN ULONG ValueLength
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlDeleteRegistryValue(
    IN ULONG RelativeTo,
    IN PCWSTR Path,
    IN PCWSTR ValueName
    );

// end_wdm

NTSYSAPI
NTSTATUS
NTAPI
RtlCreateRegistryKey(
    IN ULONG RelativeTo,
    IN PWSTR Path
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlCheckRegistryKey(
    IN ULONG RelativeTo,
    IN PWSTR Path
    );

// begin_wdm
//
// The following values for the RelativeTo parameter determine what the
// Path parameter to RtlQueryRegistryValues is relative to.
//

#define RTL_REGISTRY_ABSOLUTE     0   // Path is a full path
#define RTL_REGISTRY_SERVICES     1   // \Registry\Machine\System\CurrentControlSet\Services
#define RTL_REGISTRY_CONTROL      2   // \Registry\Machine\System\CurrentControlSet\Control
#define RTL_REGISTRY_WINDOWS_NT   3   // \Registry\Machine\Software\Microsoft\Windows NT\CurrentVersion
#define RTL_REGISTRY_DEVICEMAP    4   // \Registry\Machine\Hardware\DeviceMap
#define RTL_REGISTRY_USER         5   // \Registry\User\CurrentUser
#define RTL_REGISTRY_MAXIMUM      6
#define RTL_REGISTRY_HANDLE       0x40000000    // Low order bits are registry handle
#define RTL_REGISTRY_OPTIONAL     0x80000000    // Indicates the key node is optional

NTSYSAPI                                            
ULONG                                               
NTAPI                                               
RtlRandom (                                         
    PULONG Seed                                     
    );                                              
NTSYSAPI                                            
NTSTATUS                                            
NTAPI                                               
RtlCharToInteger (                                  
    PCSZ String,                                    
    ULONG Base,                                     
    PULONG Value                                    
    );                                              

NTSYSAPI
NTSTATUS
NTAPI
RtlIntegerToUnicodeString (
    ULONG Value,
    ULONG Base,
    PUNICODE_STRING String
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlInt64ToUnicodeString (
    IN ULONGLONG Value,
    IN ULONG Base OPTIONAL,
    IN OUT PUNICODE_STRING String
    );

#ifdef _WIN64
#define RtlIntPtrToUnicodeString(Value, Base, String) RtlInt64ToUnicodeString(Value, Base, String)
#else
#define RtlIntPtrToUnicodeString(Value, Base, String) RtlIntegerToUnicodeString(Value, Base, String)
#endif

NTSYSAPI
NTSTATUS
NTAPI
RtlUnicodeStringToInteger (
    PUNICODE_STRING String,
    ULONG Base,
    PULONG Value
    );


//
//  String manipulation routines
//

#ifdef _NTSYSTEM_

#define NLS_MB_CODE_PAGE_TAG NlsMbCodePageTag
#define NLS_MB_OEM_CODE_PAGE_TAG NlsMbOemCodePageTag

#else

#define NLS_MB_CODE_PAGE_TAG (*NlsMbCodePageTag)
#define NLS_MB_OEM_CODE_PAGE_TAG (*NlsMbOemCodePageTag)

#endif // _NTSYSTEM_

extern BOOLEAN NLS_MB_CODE_PAGE_TAG;     // TRUE -> Multibyte CP, FALSE -> Singlebyte
extern BOOLEAN NLS_MB_OEM_CODE_PAGE_TAG; // TRUE -> Multibyte CP, FALSE -> Singlebyte

NTSYSAPI
VOID
NTAPI
RtlInitString(
    PSTRING DestinationString,
    PCSZ SourceString
    );

NTSYSAPI
VOID
NTAPI
RtlInitAnsiString(
    PANSI_STRING DestinationString,
    PCSZ SourceString
    );

NTSYSAPI
VOID
NTAPI
RtlInitUnicodeString(
    PUNICODE_STRING DestinationString,
    PCWSTR SourceString
    );

// end_ntddk end_wdm

NTSYSAPI
BOOLEAN
NTAPI
RtlCreateUnicodeString(
    OUT PUNICODE_STRING DestinationString,
    IN PCWSTR SourceString
    );


NTSYSAPI
VOID
NTAPI
RtlCopyString(
    PSTRING DestinationString,
    PSTRING SourceString
    );

NTSYSAPI
CHAR
NTAPI
RtlUpperChar (
    CHAR Character
    );

NTSYSAPI
LONG
NTAPI
RtlCompareString(
    PSTRING String1,
    PSTRING String2,
    BOOLEAN CaseInSensitive
    );

NTSYSAPI
BOOLEAN
NTAPI
RtlEqualString(
    PSTRING String1,
    PSTRING String2,
    BOOLEAN CaseInSensitive
    );


NTSYSAPI
VOID
NTAPI
RtlUpperString(
    PSTRING DestinationString,
    PSTRING SourceString
    );


NTSYSAPI
NTSTATUS
NTAPI
RtlAppendStringToString (
    PSTRING Destination,
    PSTRING Source
    );

// begin_ntddk begin_wdm
//
// NLS String functions
//

NTSYSAPI
NTSTATUS
NTAPI
RtlAnsiStringToUnicodeString(
    PUNICODE_STRING DestinationString,
    PANSI_STRING SourceString,
    BOOLEAN AllocateDestinationString
    );


NTSYSAPI
NTSTATUS
NTAPI
RtlUnicodeStringToAnsiString(
    PANSI_STRING DestinationString,
    PUNICODE_STRING SourceString,
    BOOLEAN AllocateDestinationString
    );


NTSYSAPI
NTSTATUS
NTAPI
RtlOemStringToUnicodeString(
    PUNICODE_STRING DestinationString,
    POEM_STRING SourceString,
    BOOLEAN AllocateDestinationString
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlUnicodeStringToOemString(
    POEM_STRING DestinationString,
    PUNICODE_STRING SourceString,
    BOOLEAN AllocateDestinationString
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlUpcaseUnicodeStringToOemString(
    POEM_STRING DestinationString,
    PUNICODE_STRING SourceString,
    BOOLEAN AllocateDestinationString
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlOemStringToCountedUnicodeString(
    PUNICODE_STRING DestinationString,
    POEM_STRING SourceString,
    BOOLEAN AllocateDestinationString
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlUnicodeStringToCountedOemString(
    POEM_STRING DestinationString,
    PUNICODE_STRING SourceString,
    BOOLEAN AllocateDestinationString
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlUpcaseUnicodeStringToCountedOemString(
    POEM_STRING DestinationString,
    PUNICODE_STRING SourceString,
    BOOLEAN AllocateDestinationString
    );

// begin_ntddk begin_wdm begin_ntndis

NTSYSAPI
LONG
NTAPI
RtlCompareUnicodeString(
    PUNICODE_STRING String1,
    PUNICODE_STRING String2,
    BOOLEAN CaseInSensitive
    );

NTSYSAPI
BOOLEAN
NTAPI
RtlEqualUnicodeString(
    const UNICODE_STRING *String1,
    const UNICODE_STRING *String2,
    BOOLEAN CaseInSensitive
    );

// end_wdm

NTSYSAPI
BOOLEAN
NTAPI
RtlPrefixUnicodeString(
    IN PUNICODE_STRING String1,
    IN PUNICODE_STRING String2,
    IN BOOLEAN CaseInSensitive
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlUpcaseUnicodeString(
    PUNICODE_STRING DestinationString,
    PCUNICODE_STRING SourceString,
    BOOLEAN AllocateDestinationString
    );


NTSTATUS
RtlDowncaseUnicodeString(
    OUT PUNICODE_STRING DestinationString,
    IN PUNICODE_STRING SourceString,
    IN BOOLEAN AllocateDestinationString
    );


NTSYSAPI
VOID
NTAPI
RtlCopyUnicodeString(
    PUNICODE_STRING DestinationString,
    PUNICODE_STRING SourceString
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlAppendUnicodeStringToString (
    PUNICODE_STRING Destination,
    PUNICODE_STRING Source
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlAppendUnicodeToString (
    PUNICODE_STRING Destination,
    PCWSTR Source
    );

// end_ntndis end_wdm

NTSYSAPI
WCHAR
NTAPI
RtlUpcaseUnicodeChar(
    WCHAR SourceCharacter
    );

// begin_wdm

NTSYSAPI
VOID
NTAPI
RtlFreeUnicodeString(
    PUNICODE_STRING UnicodeString
    );

NTSYSAPI
VOID
NTAPI
RtlFreeAnsiString(
    PANSI_STRING AnsiString
    );

// end_ntddk end_wdm end_nthal

NTSYSAPI
VOID
NTAPI
RtlFreeOemString(
    POEM_STRING OemString
    );

// begin_wdm
NTSYSAPI
ULONG
NTAPI
RtlxUnicodeStringToAnsiSize(
    PUNICODE_STRING UnicodeString
    );

//
//  NTSYSAPI
//  ULONG
//  NTAPI
//  RtlUnicodeStringToAnsiSize(
//      PUNICODE_STRING UnicodeString
//      );
//

#define RtlUnicodeStringToAnsiSize(STRING) (                  \
    NLS_MB_CODE_PAGE_TAG ?                                    \
    RtlxUnicodeStringToAnsiSize(STRING) :                     \
    ((STRING)->Length + sizeof(UNICODE_NULL)) / sizeof(WCHAR) \
)

// end_wdm

NTSYSAPI
ULONG
NTAPI
RtlxUnicodeStringToOemSize(
    PUNICODE_STRING UnicodeString
    );

//
//  NTSYSAPI
//  ULONG
//  NTAPI
//  RtlUnicodeStringToOemSize(
//      PUNICODE_STRING UnicodeString
//      );
//

#define RtlUnicodeStringToOemSize(STRING) (                   \
    NLS_MB_OEM_CODE_PAGE_TAG ?                                \
    RtlxUnicodeStringToOemSize(STRING) :                      \
    ((STRING)->Length + sizeof(UNICODE_NULL)) / sizeof(WCHAR) \
)


NTSYSAPI
ULONG
NTAPI
RtlxAnsiStringToUnicodeSize(
    PANSI_STRING AnsiString
    );

//
//  NTSYSAPI
//  ULONG
//  NTAPI
//  RtlAnsiStringToUnicodeSize(
//      PANSI_STRING AnsiString
//      );
//

#define RtlAnsiStringToUnicodeSize(STRING) (                 \
    NLS_MB_CODE_PAGE_TAG ?                                   \
    RtlxAnsiStringToUnicodeSize(STRING) :                    \
    ((STRING)->Length + sizeof(ANSI_NULL)) * sizeof(WCHAR) \
)

// end_ntddk end_wdm

NTSYSAPI
ULONG
NTAPI
RtlxOemStringToUnicodeSize(
    POEM_STRING OemString
    );
//
//  NTSYSAPI
//  ULONG
//  NTAPI
//  RtlOemStringToUnicodeSize(
//      POEM_STRING OemString
//      );
//

#define RtlOemStringToUnicodeSize(STRING) (                  \
    NLS_MB_OEM_CODE_PAGE_TAG ?                               \
    RtlxOemStringToUnicodeSize(STRING) :                     \
    ((STRING)->Length + sizeof(ANSI_NULL)) * sizeof(WCHAR) \
)

//
//  ULONG
//  RtlOemStringToCountedUnicodeSize(
//      POEM_STRING OemString
//      );
//

#define RtlOemStringToCountedUnicodeSize(STRING) (                    \
    (ULONG)(RtlOemStringToUnicodeSize(STRING) - sizeof(UNICODE_NULL)) \
    )

NTSYSAPI
NTSTATUS
NTAPI
RtlMultiByteToUnicodeN(
    PWSTR UnicodeString,
    ULONG MaxBytesInUnicodeString,
    PULONG BytesInUnicodeString,
    PCHAR MultiByteString,
    ULONG BytesInMultiByteString
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlMultiByteToUnicodeSize(
    PULONG BytesInUnicodeString,
    PCHAR MultiByteString,
    ULONG BytesInMultiByteString
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlUnicodeToMultiByteSize(
    PULONG BytesInMultiByteString,
    PWSTR UnicodeString,
    ULONG BytesInUnicodeString
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlUnicodeToMultiByteN(
    PCHAR MultiByteString,
    ULONG MaxBytesInMultiByteString,
    PULONG BytesInMultiByteString,
    PWSTR UnicodeString,
    ULONG BytesInUnicodeString
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlUpcaseUnicodeToMultiByteN(
    PCHAR MultiByteString,
    ULONG MaxBytesInMultiByteString,
    PULONG BytesInMultiByteString,
    PWSTR UnicodeString,
    ULONG BytesInUnicodeString
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlOemToUnicodeN(
    PWSTR UnicodeString,
    ULONG MaxBytesInUnicodeString,
    PULONG BytesInUnicodeString,
    PCHAR OemString,
    ULONG BytesInOemString
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlUnicodeToOemN(
    PCHAR OemString,
    ULONG MaxBytesInOemString,
    PULONG BytesInOemString,
    PWSTR UnicodeString,
    ULONG BytesInUnicodeString
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlUpcaseUnicodeToOemN(
    PCHAR OemString,
    ULONG MaxBytesInOemString,
    PULONG BytesInOemString,
    PWSTR UnicodeString,
    ULONG BytesInUnicodeString
    );


typedef
PVOID
(NTAPI *PRTL_ALLOCATE_STRING_ROUTINE) (
    SIZE_T NumberOfBytes
    );

typedef
VOID
(NTAPI *PRTL_FREE_STRING_ROUTINE) (
    PVOID Buffer
    );

extern PRTL_ALLOCATE_STRING_ROUTINE RtlAllocateStringRoutine;
extern PRTL_FREE_STRING_ROUTINE RtlFreeStringRoutine;


//
//  Defines and Routines for handling GUID's.
//

// begin_ntddk begin_wdm begin_nthal

// begin_ntminiport

#include <guiddef.h>

// end_ntminiport

#ifndef DEFINE_GUIDEX
    #define DEFINE_GUIDEX(name) EXTERN_C const CDECL GUID name
#endif // !defined(DEFINE_GUIDEX)

#ifndef STATICGUIDOF
    #define STATICGUIDOF(guid) STATIC_##guid
#endif // !defined(STATICGUIDOF)

#ifndef __IID_ALIGNED__
    #define __IID_ALIGNED__
    #ifdef __cplusplus
        inline int IsEqualGUIDAligned(REFGUID guid1, REFGUID guid2)
        {
            return ((*(PLONGLONG)(&guid1) == *(PLONGLONG)(&guid2)) && (*((PLONGLONG)(&guid1) + 1) == *((PLONGLONG)(&guid2) + 1)));
        }
    #else // !__cplusplus
        #define IsEqualGUIDAligned(guid1, guid2) \
            ((*(PLONGLONG)(guid1) == *(PLONGLONG)(guid2)) && (*((PLONGLONG)(guid1) + 1) == *((PLONGLONG)(guid2) + 1)))
    #endif // !__cplusplus
#endif // !__IID_ALIGNED__

NTSYSAPI
NTSTATUS
NTAPI
RtlStringFromGUID(
    IN REFGUID Guid,
    OUT PUNICODE_STRING GuidString
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlGUIDFromString(
    IN PUNICODE_STRING GuidString,
    OUT GUID* Guid
    );

// end_ntddk end_wdm end_nthal

//
//  Routine for generating 8.3 names from long names.
//

//
//  The context structure is used when generating 8.3 names.  The caller must
//  always zero out the structure before starting a new generation sequence
//

typedef struct _GENERATE_NAME_CONTEXT {

    //
    //  The structure is divided into two strings.  The Name, and extension.
    //  Each part contains the value that was last inserted in the name.
    //  The length values are in terms of wchars and not bytes.  We also
    //  store the last index value used in the generation collision algorithm.
    //

    USHORT Checksum;
    BOOLEAN ChecksumInserted;

    UCHAR NameLength;         // not including extension
    WCHAR NameBuffer[8];      // e.g., "ntoskrnl"

    ULONG ExtensionLength;    // including dot
    WCHAR ExtensionBuffer[4]; // e.g., ".exe"

    ULONG LastIndexValue;

} GENERATE_NAME_CONTEXT;
typedef GENERATE_NAME_CONTEXT *PGENERATE_NAME_CONTEXT;

NTSYSAPI
VOID
NTAPI
RtlGenerate8dot3Name (
    IN PUNICODE_STRING Name,
    IN BOOLEAN AllowExtendedCharacters,
    IN OUT PGENERATE_NAME_CONTEXT Context,
    OUT PUNICODE_STRING Name8dot3
    );

NTSYSAPI
BOOLEAN
NTAPI
RtlIsNameLegalDOS8Dot3 (
    IN PUNICODE_STRING Name,
    IN OUT POEM_STRING OemName OPTIONAL,
    IN OUT PBOOLEAN NameContainsSpaces OPTIONAL
    );

BOOLEAN
RtlIsValidOemCharacter (
    IN PWCHAR Char
    );

//
//  Prefix package types and procedures.
//
//  Note that the following two record structures should really be opaque
//  to the user of this package.  The only information about the two
//  structures available for the user should be the size and alignment
//  of the structures.
//

typedef struct _PREFIX_TABLE_ENTRY {
    CSHORT NodeTypeCode;
    CSHORT NameLength;
    struct _PREFIX_TABLE_ENTRY *NextPrefixTree;
    RTL_SPLAY_LINKS Links;
    PSTRING Prefix;
} PREFIX_TABLE_ENTRY;
typedef PREFIX_TABLE_ENTRY *PPREFIX_TABLE_ENTRY;

typedef struct _PREFIX_TABLE {
    CSHORT NodeTypeCode;
    CSHORT NameLength;
    PPREFIX_TABLE_ENTRY NextPrefixTree;
} PREFIX_TABLE;
typedef PREFIX_TABLE *PPREFIX_TABLE;

//
//  The procedure prototypes for the prefix package
//

NTSYSAPI
VOID
NTAPI
PfxInitialize (
    PPREFIX_TABLE PrefixTable
    );

NTSYSAPI
BOOLEAN
NTAPI
PfxInsertPrefix (
    PPREFIX_TABLE PrefixTable,
    PSTRING Prefix,
    PPREFIX_TABLE_ENTRY PrefixTableEntry
    );

NTSYSAPI
VOID
NTAPI
PfxRemovePrefix (
    PPREFIX_TABLE PrefixTable,
    PPREFIX_TABLE_ENTRY PrefixTableEntry
    );

NTSYSAPI
PPREFIX_TABLE_ENTRY
NTAPI
PfxFindPrefix (
    PPREFIX_TABLE PrefixTable,
    PSTRING FullName
    );

//
//  The following definitions are for the unicode version of the prefix
//  package.
//

typedef struct _UNICODE_PREFIX_TABLE_ENTRY {
    CSHORT NodeTypeCode;
    CSHORT NameLength;
    struct _UNICODE_PREFIX_TABLE_ENTRY *NextPrefixTree;
    struct _UNICODE_PREFIX_TABLE_ENTRY *CaseMatch;
    RTL_SPLAY_LINKS Links;
    PUNICODE_STRING Prefix;
} UNICODE_PREFIX_TABLE_ENTRY;
typedef UNICODE_PREFIX_TABLE_ENTRY *PUNICODE_PREFIX_TABLE_ENTRY;

typedef struct _UNICODE_PREFIX_TABLE {
    CSHORT NodeTypeCode;
    CSHORT NameLength;
    PUNICODE_PREFIX_TABLE_ENTRY NextPrefixTree;
    PUNICODE_PREFIX_TABLE_ENTRY LastNextEntry;
} UNICODE_PREFIX_TABLE;
typedef UNICODE_PREFIX_TABLE *PUNICODE_PREFIX_TABLE;

NTSYSAPI
VOID
NTAPI
RtlInitializeUnicodePrefix (
    PUNICODE_PREFIX_TABLE PrefixTable
    );

NTSYSAPI
BOOLEAN
NTAPI
RtlInsertUnicodePrefix (
    PUNICODE_PREFIX_TABLE PrefixTable,
    PUNICODE_STRING Prefix,
    PUNICODE_PREFIX_TABLE_ENTRY PrefixTableEntry
    );

NTSYSAPI
VOID
NTAPI
RtlRemoveUnicodePrefix (
    PUNICODE_PREFIX_TABLE PrefixTable,
    PUNICODE_PREFIX_TABLE_ENTRY PrefixTableEntry
    );

NTSYSAPI
PUNICODE_PREFIX_TABLE_ENTRY
NTAPI
RtlFindUnicodePrefix (
    PUNICODE_PREFIX_TABLE PrefixTable,
    PUNICODE_STRING FullName,
    ULONG CaseInsensitiveIndex
    );

NTSYSAPI
PUNICODE_PREFIX_TABLE_ENTRY
NTAPI
RtlNextUnicodePrefix (
    PUNICODE_PREFIX_TABLE PrefixTable,
    BOOLEAN Restart
    );
//
// Fast primitives to compare, move, and zero memory
//

// begin_winnt begin_ntndis

NTSYSAPI
SIZE_T
NTAPI
RtlCompareMemory (
    const VOID *Source1,
    const VOID *Source2,
    SIZE_T Length
    );

#if defined(_M_AXP64) || defined(_M_IA64)

#define RtlEqualMemory(Source1, Source2, Length) \
    ((Length) == RtlCompareMemory(Source1, Source2, Length))

NTSYSAPI
VOID
NTAPI
RtlCopyMemory (
   VOID UNALIGNED *Destination,
   CONST VOID UNALIGNED *Source,
   SIZE_T Length
   );

NTSYSAPI
VOID
NTAPI
RtlCopyMemory32 (
   VOID UNALIGNED *Destination,
   CONST VOID UNALIGNED *Source,
   ULONG Length
   );

NTSYSAPI
VOID
NTAPI
RtlMoveMemory (
   VOID UNALIGNED *Destination,
   CONST VOID UNALIGNED *Source,
   SIZE_T Length
   );

NTSYSAPI
VOID
NTAPI
RtlFillMemory (
   VOID UNALIGNED *Destination,
   SIZE_T Length,
   UCHAR Fill
   );

NTSYSAPI
VOID
NTAPI
RtlZeroMemory (
   VOID UNALIGNED *Destination,
   SIZE_T Length
   );

#else

#define RtlEqualMemory(Destination,Source,Length) (!memcmp((Destination),(Source),(Length)))
#define RtlMoveMemory(Destination,Source,Length) memmove((Destination),(Source),(Length))
#define RtlCopyMemory(Destination,Source,Length) memcpy((Destination),(Source),(Length))
#define RtlFillMemory(Destination,Length,Fill) memset((Destination),(Fill),(Length))
#define RtlZeroMemory(Destination,Length) memset((Destination),0,(Length))

#endif

// end_ntndis end_winnt

#if defined(_M_ALPHA)

//
// Guaranteed byte granularity memory copy function.
//

NTSYSAPI
VOID
NTAPI
RtlCopyBytes (
   PVOID Destination,
   CONST VOID *Source,
   SIZE_T Length
   );

//
// Guaranteed byte granularity memory zero function.
//

NTSYSAPI
VOID
NTAPI
RtlZeroBytes (
   PVOID Destination,
   SIZE_T Length
   );

//
// Guaranteed byte granularity memory fill function.
//

NTSYSAPI
VOID
NTAPI
RtlFillBytes (
    PVOID Destination,
    SIZE_T Length,
    UCHAR Fill
    );

#else

#define RtlCopyBytes RtlCopyMemory
#define RtlZeroBytes RtlZeroMemory
#define RtlFillBytes RtlFillMemory

#endif

NTSYSAPI
VOID
FASTCALL
RtlPrefetchMemoryNonTemporal(
    IN PVOID Source,
    IN SIZE_T Length
    );

// end_ntddk end_wdm end_nthal

NTSYSAPI
SIZE_T
NTAPI
RtlCompareMemoryUlong (
    PVOID Source,
    SIZE_T Length,
    ULONG Pattern
    );

NTSYSAPI
VOID
NTAPI
RtlFillMemoryUlong (
   PVOID Destination,
   SIZE_T Length,
   ULONG Pattern
   );

NTSYSAPI
VOID
NTAPI
RtlFillMemoryUlonglong (
   PVOID Destination,
   SIZE_T Length,
   ULONGLONG Pattern
   );

//
// Define kernel debugger print prototypes and macros.
//
// N.B. The following function cannot be directly imported because there are
//      a few places in the source tree where this function is redefined.
//

VOID
NTAPI
DbgBreakPoint(
    VOID
    );

// end_wdm

NTSYSAPI
VOID
NTAPI
DbgBreakPointWithStatus(
    IN ULONG Status
    );

// begin_wdm

#define DBG_STATUS_CONTROL_C        1
#define DBG_STATUS_SYSRQ            2
#define DBG_STATUS_BUGCHECK_FIRST   3
#define DBG_STATUS_BUGCHECK_SECOND  4
#define DBG_STATUS_FATAL            5
#define DBG_STATUS_DEBUG_CONTROL    6

#if DBG

#define KdPrint(_x_) DbgPrint _x_
#define KdBreakPoint() DbgBreakPoint()

// end_wdm

#define KdBreakPointWithStatus(s) DbgBreakPointWithStatus(s)

// begin_wdm

#else

#define KdPrint(_x_)
#define KdBreakPoint()

// end_wdm

#define KdBreakPointWithStatus(s)

// begin_wdm
#endif

#ifndef _DBGNT_

ULONG
_cdecl
DbgPrint(
    PCH Format,
    ...
    );

// end_wdm

ULONG
_cdecl
DbgPrintReturnControlC(
    PCH Format,
    ...
    );

// begin_wdm
#endif // _DBGNT_
//
// Large integer arithmetic routines.
//

//
// Large integer add - 64-bits + 64-bits -> 64-bits
//

#if !defined(MIDL_PASS)

__inline
LARGE_INTEGER
NTAPI
RtlLargeIntegerAdd (
    LARGE_INTEGER Addend1,
    LARGE_INTEGER Addend2
    )
{
    LARGE_INTEGER Sum;

    Sum.QuadPart = Addend1.QuadPart + Addend2.QuadPart;
    return Sum;
}

//
// Enlarged integer multiply - 32-bits * 32-bits -> 64-bits
//

__inline
LARGE_INTEGER
NTAPI
RtlEnlargedIntegerMultiply (
    LONG Multiplicand,
    LONG Multiplier
    )
{
    LARGE_INTEGER Product;

    Product.QuadPart = (LONGLONG)Multiplicand * (ULONGLONG)Multiplier;
    return Product;
}

//
// Unsigned enlarged integer multiply - 32-bits * 32-bits -> 64-bits
//

__inline
LARGE_INTEGER
NTAPI
RtlEnlargedUnsignedMultiply (
    ULONG Multiplicand,
    ULONG Multiplier
    )
{
    LARGE_INTEGER Product;

    Product.QuadPart = (ULONGLONG)Multiplicand * (ULONGLONG)Multiplier;
    return Product;
}

//
// Enlarged integer divide - 64-bits / 32-bits > 32-bits
//

__inline
ULONG
NTAPI
RtlEnlargedUnsignedDivide (
    IN ULARGE_INTEGER Dividend,
    IN ULONG Divisor,
    IN PULONG Remainder
    )
{
    ULONG Quotient;

    Quotient = (ULONG)(Dividend.QuadPart / Divisor);
    if (ARGUMENT_PRESENT( Remainder )) {

        *Remainder = (ULONG)(Dividend.QuadPart % Divisor);
    }

    return Quotient;
}

//
// Large integer negation - -(64-bits)
//

__inline
LARGE_INTEGER
NTAPI
RtlLargeIntegerNegate (
    LARGE_INTEGER Subtrahend
    )
{
    LARGE_INTEGER Difference;

    Difference.QuadPart = -Subtrahend.QuadPart;
    return Difference;
}

//
// Large integer subtract - 64-bits - 64-bits -> 64-bits.
//

__inline
LARGE_INTEGER
NTAPI
RtlLargeIntegerSubtract (
    LARGE_INTEGER Minuend,
    LARGE_INTEGER Subtrahend
    )
{
    LARGE_INTEGER Difference;

    Difference.QuadPart = Minuend.QuadPart - Subtrahend.QuadPart;
    return Difference;
}

#endif

//
// Extended large integer magic divide - 64-bits / 32-bits -> 64-bits
//

NTSYSAPI
LARGE_INTEGER
NTAPI
RtlExtendedMagicDivide (
    LARGE_INTEGER Dividend,
    LARGE_INTEGER MagicDivisor,
    CCHAR ShiftCount
    );

//
// Large Integer divide - 64-bits / 32-bits -> 64-bits
//

NTSYSAPI
LARGE_INTEGER
NTAPI
RtlExtendedLargeIntegerDivide (
    LARGE_INTEGER Dividend,
    ULONG Divisor,
    PULONG Remainder
    );

// end_wdm
//
// Large Integer divide - 64-bits / 32-bits -> 64-bits
//

NTSYSAPI
LARGE_INTEGER
NTAPI
RtlLargeIntegerDivide (
    LARGE_INTEGER Dividend,
    LARGE_INTEGER Divisor,
    PLARGE_INTEGER Remainder
    );

// begin_wdm
//
// Extended integer multiply - 32-bits * 64-bits -> 64-bits
//

NTSYSAPI
LARGE_INTEGER
NTAPI
RtlExtendedIntegerMultiply (
    LARGE_INTEGER Multiplicand,
    LONG Multiplier
    );

//
// Large integer and - 64-bite & 64-bits -> 64-bits.
//

#define RtlLargeIntegerAnd(Result, Source, Mask)   \
        {                                           \
            Result.HighPart = Source.HighPart & Mask.HighPart; \
            Result.LowPart = Source.LowPart & Mask.LowPart; \
        }

//
// Large integer conversion routines.
//

#if defined(MIDL_PASS) || defined(__cplusplus) || !defined(_M_IX86)

//
// Convert signed integer to large integer.
//

NTSYSAPI
LARGE_INTEGER
NTAPI
RtlConvertLongToLargeInteger (
    LONG SignedInteger
    );

//
// Convert unsigned integer to large integer.
//

NTSYSAPI
LARGE_INTEGER
NTAPI
RtlConvertUlongToLargeInteger (
    ULONG UnsignedInteger
    );


//
// Large integer shift routines.
//

NTSYSAPI
LARGE_INTEGER
NTAPI
RtlLargeIntegerShiftLeft (
    LARGE_INTEGER LargeInteger,
    CCHAR ShiftCount
    );

NTSYSAPI
LARGE_INTEGER
NTAPI
RtlLargeIntegerShiftRight (
    LARGE_INTEGER LargeInteger,
    CCHAR ShiftCount
    );

NTSYSAPI
LARGE_INTEGER
NTAPI
RtlLargeIntegerArithmeticShift (
    LARGE_INTEGER LargeInteger,
    CCHAR ShiftCount
    );

#else

#if _MSC_VER >= 1200
#pragma warning(push)
#endif
#pragma warning(disable:4035)               // re-enable below

//
// Convert signed integer to large integer.
//

__inline LARGE_INTEGER
NTAPI
RtlConvertLongToLargeInteger (
    LONG SignedInteger
    )
{
    __asm {
        mov     eax, SignedInteger
        cdq                 ; (edx:eax) = signed LargeInt
    }
}

//
// Convert unsigned integer to large integer.
//

__inline LARGE_INTEGER
NTAPI
RtlConvertUlongToLargeInteger (
    ULONG UnsignedInteger
    )
{
    __asm {
        sub     edx, edx    ; zero highpart
        mov     eax, UnsignedInteger
    }
}

//
// Large integer shift routines.
//

__inline LARGE_INTEGER
NTAPI
RtlLargeIntegerShiftLeft (
    LARGE_INTEGER LargeInteger,
    CCHAR ShiftCount
    )
{
    __asm    {
        mov     cl, ShiftCount
        and     cl, 0x3f                    ; mod 64

        cmp     cl, 32
        jc      short sl10

        mov     edx, LargeInteger.LowPart   ; ShiftCount >= 32
        xor     eax, eax                    ; lowpart is zero
        shl     edx, cl                     ; store highpart
        jmp     short done

sl10:
        mov     eax, LargeInteger.LowPart   ; ShiftCount < 32
        mov     edx, LargeInteger.HighPart
        shld    edx, eax, cl
        shl     eax, cl
done:
    }
}


__inline LARGE_INTEGER
NTAPI
RtlLargeIntegerShiftRight (
    LARGE_INTEGER LargeInteger,
    CCHAR ShiftCount
    )
{
    __asm    {
        mov     cl, ShiftCount
        and     cl, 0x3f               ; mod 64

        cmp     cl, 32
        jc      short sr10

        mov     eax, LargeInteger.HighPart  ; ShiftCount >= 32
        xor     edx, edx                    ; lowpart is zero
        shr     eax, cl                     ; store highpart
        jmp     short done

sr10:
        mov     eax, LargeInteger.LowPart   ; ShiftCount < 32
        mov     edx, LargeInteger.HighPart
        shrd    eax, edx, cl
        shr     edx, cl
done:
    }
}


__inline LARGE_INTEGER
NTAPI
RtlLargeIntegerArithmeticShift (
    LARGE_INTEGER LargeInteger,
    CCHAR ShiftCount
    )
{
    __asm {
        mov     cl, ShiftCount
        and     cl, 3fh                 ; mod 64

        cmp     cl, 32
        jc      short sar10

        mov     eax, LargeInteger.HighPart
        sar     eax, cl
        bt      eax, 31                     ; sign bit set?
        sbb     edx, edx                    ; duplicate sign bit into highpart
        jmp     short done
sar10:
        mov     eax, LargeInteger.LowPart   ; (eax) = LargeInteger.LowPart
        mov     edx, LargeInteger.HighPart  ; (edx) = LargeInteger.HighPart
        shrd    eax, edx, cl
        sar     edx, cl
done:
    }
}

#if _MSC_VER >= 1200
#pragma warning(pop)
#else
#pragma warning(default:4035)
#endif

#endif

//
// Large integer comparison routines.
//
// BOOLEAN
// RtlLargeIntegerGreaterThan (
//     LARGE_INTEGER Operand1,
//     LARGE_INTEGER Operand2
//     );
//
// BOOLEAN
// RtlLargeIntegerGreaterThanOrEqualTo (
//     LARGE_INTEGER Operand1,
//     LARGE_INTEGER Operand2
//     );
//
// BOOLEAN
// RtlLargeIntegerEqualTo (
//     LARGE_INTEGER Operand1,
//     LARGE_INTEGER Operand2
//     );
//
// BOOLEAN
// RtlLargeIntegerNotEqualTo (
//     LARGE_INTEGER Operand1,
//     LARGE_INTEGER Operand2
//     );
//
// BOOLEAN
// RtlLargeIntegerLessThan (
//     LARGE_INTEGER Operand1,
//     LARGE_INTEGER Operand2
//     );
//
// BOOLEAN
// RtlLargeIntegerLessThanOrEqualTo (
//     LARGE_INTEGER Operand1,
//     LARGE_INTEGER Operand2
//     );
//
// BOOLEAN
// RtlLargeIntegerGreaterThanZero (
//     LARGE_INTEGER Operand
//     );
//
// BOOLEAN
// RtlLargeIntegerGreaterOrEqualToZero (
//     LARGE_INTEGER Operand
//     );
//
// BOOLEAN
// RtlLargeIntegerEqualToZero (
//     LARGE_INTEGER Operand
//     );
//
// BOOLEAN
// RtlLargeIntegerNotEqualToZero (
//     LARGE_INTEGER Operand
//     );
//
// BOOLEAN
// RtlLargeIntegerLessThanZero (
//     LARGE_INTEGER Operand
//     );
//
// BOOLEAN
// RtlLargeIntegerLessOrEqualToZero (
//     LARGE_INTEGER Operand
//     );
//

#define RtlLargeIntegerGreaterThan(X,Y) (                              \
    (((X).HighPart == (Y).HighPart) && ((X).LowPart > (Y).LowPart)) || \
    ((X).HighPart > (Y).HighPart)                                      \
)

#define RtlLargeIntegerGreaterThanOrEqualTo(X,Y) (                      \
    (((X).HighPart == (Y).HighPart) && ((X).LowPart >= (Y).LowPart)) || \
    ((X).HighPart > (Y).HighPart)                                       \
)

#define RtlLargeIntegerEqualTo(X,Y) (                              \
    !(((X).LowPart ^ (Y).LowPart) | ((X).HighPart ^ (Y).HighPart)) \
)

#define RtlLargeIntegerNotEqualTo(X,Y) (                          \
    (((X).LowPart ^ (Y).LowPart) | ((X).HighPart ^ (Y).HighPart)) \
)

#define RtlLargeIntegerLessThan(X,Y) (                                 \
    (((X).HighPart == (Y).HighPart) && ((X).LowPart < (Y).LowPart)) || \
    ((X).HighPart < (Y).HighPart)                                      \
)

#define RtlLargeIntegerLessThanOrEqualTo(X,Y) (                         \
    (((X).HighPart == (Y).HighPart) && ((X).LowPart <= (Y).LowPart)) || \
    ((X).HighPart < (Y).HighPart)                                       \
)

#define RtlLargeIntegerGreaterThanZero(X) (       \
    (((X).HighPart == 0) && ((X).LowPart > 0)) || \
    ((X).HighPart > 0 )                           \
)

#define RtlLargeIntegerGreaterOrEqualToZero(X) ( \
    (X).HighPart >= 0                            \
)

#define RtlLargeIntegerEqualToZero(X) ( \
    !((X).LowPart | (X).HighPart)       \
)

#define RtlLargeIntegerNotEqualToZero(X) ( \
    ((X).LowPart | (X).HighPart)           \
)

#define RtlLargeIntegerLessThanZero(X) ( \
    ((X).HighPart < 0)                   \
)

#define RtlLargeIntegerLessOrEqualToZero(X) (           \
    ((X).HighPart < 0) || !((X).LowPart | (X).HighPart) \
)


//
//  Time conversion routines
//

typedef struct _TIME_FIELDS {
    CSHORT Year;        // range [1601...]
    CSHORT Month;       // range [1..12]
    CSHORT Day;         // range [1..31]
    CSHORT Hour;        // range [0..23]
    CSHORT Minute;      // range [0..59]
    CSHORT Second;      // range [0..59]
    CSHORT Milliseconds;// range [0..999]
    CSHORT Weekday;     // range [0..6] == [Sunday..Saturday]
} TIME_FIELDS;
typedef TIME_FIELDS *PTIME_FIELDS;


NTSYSAPI
VOID
NTAPI
RtlTimeToTimeFields (
    PLARGE_INTEGER Time,
    PTIME_FIELDS TimeFields
    );

//
//  A time field record (Weekday ignored) -> 64 bit Time value
//

NTSYSAPI
BOOLEAN
NTAPI
RtlTimeFieldsToTime (
    PTIME_FIELDS TimeFields,
    PLARGE_INTEGER Time
    );

// end_ntddk end_wdm

//
//  A 64 bit Time value -> Seconds since the start of 1980
//

NTSYSAPI
BOOLEAN
NTAPI
RtlTimeToSecondsSince1980 (
    PLARGE_INTEGER Time,
    PULONG ElapsedSeconds
    );

//
//  Seconds since the start of 1980 -> 64 bit Time value
//

NTSYSAPI
VOID
NTAPI
RtlSecondsSince1980ToTime (
    ULONG ElapsedSeconds,
    PLARGE_INTEGER Time
    );

//
//  A 64 bit Time value -> Seconds since the start of 1970
//

NTSYSAPI
BOOLEAN
NTAPI
RtlTimeToSecondsSince1970 (
    PLARGE_INTEGER Time,
    PULONG ElapsedSeconds
    );

//
//  Seconds since the start of 1970 -> 64 bit Time value
//

NTSYSAPI
VOID
NTAPI
RtlSecondsSince1970ToTime (
    ULONG ElapsedSeconds,
    PLARGE_INTEGER Time
    );

//
// The following macros store and retrieve USHORTS and ULONGS from potentially
// unaligned addresses, avoiding alignment faults.  they should probably be
// rewritten in assembler
//

#define SHORT_SIZE  (sizeof(USHORT))
#define SHORT_MASK  (SHORT_SIZE - 1)
#define LONG_SIZE       (sizeof(LONG))
#define LONGLONG_SIZE   (sizeof(LONGLONG))
#define LONG_MASK       (LONG_SIZE - 1)
#define LONGLONG_MASK   (LONGLONG_SIZE - 1)
#define LOWBYTE_MASK 0x00FF

#define FIRSTBYTE(VALUE)  ((VALUE) & LOWBYTE_MASK)
#define SECONDBYTE(VALUE) (((VALUE) >> 8) & LOWBYTE_MASK)
#define THIRDBYTE(VALUE)  (((VALUE) >> 16) & LOWBYTE_MASK)
#define FOURTHBYTE(VALUE) (((VALUE) >> 24) & LOWBYTE_MASK)

//
// if MIPS Big Endian, order of bytes is reversed.
//

#define SHORT_LEAST_SIGNIFICANT_BIT  0
#define SHORT_MOST_SIGNIFICANT_BIT   1

#define LONG_LEAST_SIGNIFICANT_BIT       0
#define LONG_3RD_MOST_SIGNIFICANT_BIT    1
#define LONG_2ND_MOST_SIGNIFICANT_BIT    2
#define LONG_MOST_SIGNIFICANT_BIT        3

//++
//
// VOID
// RtlStoreUshort (
//     PUSHORT ADDRESS
//     USHORT VALUE
//     )
//
// Routine Description:
//
// This macro stores a USHORT value in at a particular address, avoiding
// alignment faults.
//
// Arguments:
//
//     ADDRESS - where to store USHORT value
//     VALUE - USHORT to store
//
// Return Value:
//
//     none.
//
//--

#define RtlStoreUshort(ADDRESS,VALUE)                     \
         if ((ULONG_PTR)(ADDRESS) & SHORT_MASK) {         \
             ((PUCHAR) (ADDRESS))[SHORT_LEAST_SIGNIFICANT_BIT] = (UCHAR)(FIRSTBYTE(VALUE));    \
             ((PUCHAR) (ADDRESS))[SHORT_MOST_SIGNIFICANT_BIT ] = (UCHAR)(SECONDBYTE(VALUE));   \
         }                                                \
         else {                                           \
             *((PUSHORT) (ADDRESS)) = (USHORT) VALUE;     \
         }


//++
//
// VOID
// RtlStoreUlong (
//     PULONG ADDRESS
//     ULONG VALUE
//     )
//
// Routine Description:
//
// This macro stores a ULONG value in at a particular address, avoiding
// alignment faults.
//
// Arguments:
//
//     ADDRESS - where to store ULONG value
//     VALUE - ULONG to store
//
// Return Value:
//
//     none.
//
// Note:
//     Depending on the machine, we might want to call storeushort in the
//     unaligned case.
//
//--

#define RtlStoreUlong(ADDRESS,VALUE)                      \
         if ((ULONG_PTR)(ADDRESS) & LONG_MASK) {          \
             ((PUCHAR) (ADDRESS))[LONG_LEAST_SIGNIFICANT_BIT      ] = (UCHAR)(FIRSTBYTE(VALUE));    \
             ((PUCHAR) (ADDRESS))[LONG_3RD_MOST_SIGNIFICANT_BIT   ] = (UCHAR)(SECONDBYTE(VALUE));   \
             ((PUCHAR) (ADDRESS))[LONG_2ND_MOST_SIGNIFICANT_BIT   ] = (UCHAR)(THIRDBYTE(VALUE));    \
             ((PUCHAR) (ADDRESS))[LONG_MOST_SIGNIFICANT_BIT       ] = (UCHAR)(FOURTHBYTE(VALUE));   \
         }                                                \
         else {                                           \
             *((PULONG) (ADDRESS)) = (ULONG) (VALUE);     \
         }

//++
//
// VOID
// RtlStoreUlonglong (
//     PULONGLONG ADDRESS
//     ULONG VALUE
//     )
//
// Routine Description:
//
// This macro stores a ULONGLONG value in at a particular address, avoiding
// alignment faults.
//
// Arguments:
//
//     ADDRESS - where to store ULONGLONG value
//     VALUE - ULONGLONG to store
//
// Return Value:
//
//     none.
//
//--

#define RtlStoreUlonglong(ADDRESS,VALUE)                        \
         if ((ULONG_PTR)(ADDRESS) & LONGLONG_MASK) {            \
             RtlStoreUlong((ULONG_PTR)(ADDRESS),                \
                           (ULONGLONG)(VALUE) & 0xFFFFFFFF);    \
             RtlStoreUlong((ULONG_PTR)(ADDRESS)+sizeof(ULONG),  \
                           (ULONGLONG)(VALUE) >> 32);           \
         } else {                                               \
             *((PULONGLONG)(ADDRESS)) = (ULONGLONG)(VALUE);     \
         }

//++
//
// VOID
// RtlStoreUlongPtr (
//     PULONG_PTR ADDRESS
//     ULONG_PTR VALUE
//     )
//
// Routine Description:
//
// This macro stores a ULONG_PTR value in at a particular address, avoiding
// alignment faults.
//
// Arguments:
//
//     ADDRESS - where to store ULONG_PTR value
//     VALUE - ULONG_PTR to store
//
// Return Value:
//
//     none.
//
//--

#ifdef _WIN64

#define RtlStoreUlongPtr(ADDRESS,VALUE)                         \
         RtlStoreUlonglong(ADDRESS,VALUE)

#else

#define RtlStoreUlongPtr(ADDRESS,VALUE)                         \
         RtlStoreUlong(ADDRESS,VALUE)

#endif

//++
//
// VOID
// RtlRetrieveUshort (
//     PUSHORT DESTINATION_ADDRESS
//     PUSHORT SOURCE_ADDRESS
//     )
//
// Routine Description:
//
// This macro retrieves a USHORT value from the SOURCE address, avoiding
// alignment faults.  The DESTINATION address is assumed to be aligned.
//
// Arguments:
//
//     DESTINATION_ADDRESS - where to store USHORT value
//     SOURCE_ADDRESS - where to retrieve USHORT value from
//
// Return Value:
//
//     none.
//
//--

#define RtlRetrieveUshort(DEST_ADDRESS,SRC_ADDRESS)                   \
         if ((ULONG_PTR)SRC_ADDRESS & SHORT_MASK) {                       \
             ((PUCHAR) DEST_ADDRESS)[0] = ((PUCHAR) SRC_ADDRESS)[0];  \
             ((PUCHAR) DEST_ADDRESS)[1] = ((PUCHAR) SRC_ADDRESS)[1];  \
         }                                                            \
         else {                                                       \
             *((PUSHORT) DEST_ADDRESS) = *((PUSHORT) SRC_ADDRESS);    \
         }                                                            \

//++
//
// VOID
// RtlRetrieveUlong (
//     PULONG DESTINATION_ADDRESS
//     PULONG SOURCE_ADDRESS
//     )
//
// Routine Description:
//
// This macro retrieves a ULONG value from the SOURCE address, avoiding
// alignment faults.  The DESTINATION address is assumed to be aligned.
//
// Arguments:
//
//     DESTINATION_ADDRESS - where to store ULONG value
//     SOURCE_ADDRESS - where to retrieve ULONG value from
//
// Return Value:
//
//     none.
//
// Note:
//     Depending on the machine, we might want to call retrieveushort in the
//     unaligned case.
//
//--

#define RtlRetrieveUlong(DEST_ADDRESS,SRC_ADDRESS)                    \
         if ((ULONG_PTR)SRC_ADDRESS & LONG_MASK) {                        \
             ((PUCHAR) DEST_ADDRESS)[0] = ((PUCHAR) SRC_ADDRESS)[0];  \
             ((PUCHAR) DEST_ADDRESS)[1] = ((PUCHAR) SRC_ADDRESS)[1];  \
             ((PUCHAR) DEST_ADDRESS)[2] = ((PUCHAR) SRC_ADDRESS)[2];  \
             ((PUCHAR) DEST_ADDRESS)[3] = ((PUCHAR) SRC_ADDRESS)[3];  \
         }                                                            \
         else {                                                       \
             *((PULONG) DEST_ADDRESS) = *((PULONG) SRC_ADDRESS);      \
         }
// end_ntddk end_wdm

//++
//
// PCHAR
// RtlOffsetToPointer (
//     PVOID Base,
//     ULONG Offset
//     )
//
// Routine Description:
//
// This macro generates a pointer which points to the byte that is 'Offset'
// bytes beyond 'Base'. This is useful for referencing fields within
// self-relative data structures.
//
// Arguments:
//
//     Base - The address of the base of the structure.
//
//     Offset - An unsigned integer offset of the byte whose address is to
//         be generated.
//
// Return Value:
//
//     A PCHAR pointer to the byte that is 'Offset' bytes beyond 'Base'.
//
//
//--

#define RtlOffsetToPointer(B,O)  ((PCHAR)( ((PCHAR)(B)) + ((ULONG_PTR)(O))  ))


//++
//
// ULONG
// RtlPointerToOffset (
//     PVOID Base,
//     PVOID Pointer
//     )
//
// Routine Description:
//
// This macro calculates the offset from Base to Pointer.  This is useful
// for producing self-relative offsets for structures.
//
// Arguments:
//
//     Base - The address of the base of the structure.
//
//     Pointer - A pointer to a field, presumably within the structure
//         pointed to by Base.  This value must be larger than that specified
//         for Base.
//
// Return Value:
//
//     A ULONG offset from Base to Pointer.
//
//
//--

#define RtlPointerToOffset(B,P)  ((ULONG)( ((PCHAR)(P)) - ((PCHAR)(B))  ))

//
//  BitMap routines.  The following structure, routines, and macros are
//  for manipulating bitmaps.  The user is responsible for allocating a bitmap
//  structure (which is really a header) and a buffer (which must be longword
//  aligned and multiple longwords in size).
//

typedef struct _RTL_BITMAP {
    ULONG SizeOfBitMap;                     // Number of bits in bit map
    PULONG Buffer;                          // Pointer to the bit map itself
} RTL_BITMAP;
typedef RTL_BITMAP *PRTL_BITMAP;

//
//  The following routine initializes a new bitmap.  It does not alter the
//  data currently in the bitmap.  This routine must be called before
//  any other bitmap routine/macro.
//

NTSYSAPI
VOID
NTAPI
RtlInitializeBitMap (
    PRTL_BITMAP BitMapHeader,
    PULONG BitMapBuffer,
    ULONG SizeOfBitMap
    );

//
//  The following two routines either clear or set all of the bits
//  in a bitmap.
//

NTSYSAPI
VOID
NTAPI
RtlClearAllBits (
    PRTL_BITMAP BitMapHeader
    );

NTSYSAPI
VOID
NTAPI
RtlSetAllBits (
    PRTL_BITMAP BitMapHeader
    );

//
//  The following two routines locate a contiguous region of either
//  clear or set bits within the bitmap.  The region will be at least
//  as large as the number specified, and the search of the bitmap will
//  begin at the specified hint index (which is a bit index within the
//  bitmap, zero based).  The return value is the bit index of the located
//  region (zero based) or -1 (i.e., 0xffffffff) if such a region cannot
//  be located
//

NTSYSAPI
ULONG
NTAPI
RtlFindClearBits (
    PRTL_BITMAP BitMapHeader,
    ULONG NumberToFind,
    ULONG HintIndex
    );

NTSYSAPI
ULONG
NTAPI
RtlFindSetBits (
    PRTL_BITMAP BitMapHeader,
    ULONG NumberToFind,
    ULONG HintIndex
    );

//
//  The following two routines locate a contiguous region of either
//  clear or set bits within the bitmap and either set or clear the bits
//  within the located region.  The region will be as large as the number
//  specified, and the search for the region will begin at the specified
//  hint index (which is a bit index within the bitmap, zero based).  The
//  return value is the bit index of the located region (zero based) or
//  -1 (i.e., 0xffffffff) if such a region cannot be located.  If a region
//  cannot be located then the setting/clearing of the bitmap is not performed.
//

NTSYSAPI
ULONG
NTAPI
RtlFindClearBitsAndSet (
    PRTL_BITMAP BitMapHeader,
    ULONG NumberToFind,
    ULONG HintIndex
    );

NTSYSAPI
ULONG
NTAPI
RtlFindSetBitsAndClear (
    PRTL_BITMAP BitMapHeader,
    ULONG NumberToFind,
    ULONG HintIndex
    );

//
//  The following two routines clear or set bits within a specified region
//  of the bitmap.  The starting index is zero based.
//

NTSYSAPI
VOID
NTAPI
RtlClearBits (
    PRTL_BITMAP BitMapHeader,
    ULONG StartingIndex,
    ULONG NumberToClear
    );

NTSYSAPI
VOID
NTAPI
RtlSetBits (
    PRTL_BITMAP BitMapHeader,
    ULONG StartingIndex,
    ULONG NumberToSet
    );

//
//  The following routine locates a set of contiguous regions of clear
//  bits within the bitmap.  The caller specifies whether to return the
//  longest runs or just the first found lcoated.  The following structure is
//  used to denote a contiguous run of bits.  The two routines return an array
//  of this structure, one for each run located.
//

typedef struct _RTL_BITMAP_RUN {

    ULONG StartingIndex;
    ULONG NumberOfBits;

} RTL_BITMAP_RUN;
typedef RTL_BITMAP_RUN *PRTL_BITMAP_RUN;

NTSYSAPI
ULONG
NTAPI
RtlFindClearRuns (
    PRTL_BITMAP BitMapHeader,
    PRTL_BITMAP_RUN RunArray,
    ULONG SizeOfRunArray,
    BOOLEAN LocateLongestRuns
    );

//
//  The following routine locates the longest contiguous region of
//  clear bits within the bitmap.  The returned starting index value
//  denotes the first contiguous region located satisfying our requirements
//  The return value is the length (in bits) of the longest region found.
//

NTSYSAPI
ULONG
NTAPI
RtlFindLongestRunClear (
    PRTL_BITMAP BitMapHeader,
    PULONG StartingIndex
    );

//
//  The following routine locates the first contiguous region of
//  clear bits within the bitmap.  The returned starting index value
//  denotes the first contiguous region located satisfying our requirements
//  The return value is the length (in bits) of the region found.
//

NTSYSAPI
ULONG
NTAPI
RtlFindFirstRunClear (
    PRTL_BITMAP BitMapHeader,
    PULONG StartingIndex
    );

//
//  The following macro returns the value of the bit stored within the
//  bitmap at the specified location.  If the bit is set a value of 1 is
//  returned otherwise a value of 0 is returned.
//
//      ULONG
//      RtlCheckBit (
//          PRTL_BITMAP BitMapHeader,
//          ULONG BitPosition
//          );
//
//
//  To implement CheckBit the macro retrieves the longword containing the
//  bit in question, shifts the longword to get the bit in question into the
//  low order bit position and masks out all other bits.
//

#define RtlCheckBit(BMH,BP) ((((BMH)->Buffer[(BP) / 32]) >> ((BP) % 32)) & 0x1)

//
//  The following two procedures return to the caller the total number of
//  clear or set bits within the specified bitmap.
//

NTSYSAPI
ULONG
NTAPI
RtlNumberOfClearBits (
    PRTL_BITMAP BitMapHeader
    );

NTSYSAPI
ULONG
NTAPI
RtlNumberOfSetBits (
    PRTL_BITMAP BitMapHeader
    );

//
//  The following two procedures return to the caller a boolean value
//  indicating if the specified range of bits are all clear or set.
//

NTSYSAPI
BOOLEAN
NTAPI
RtlAreBitsClear (
    PRTL_BITMAP BitMapHeader,
    ULONG StartingIndex,
    ULONG Length
    );

NTSYSAPI
BOOLEAN
NTAPI
RtlAreBitsSet (
    PRTL_BITMAP BitMapHeader,
    ULONG StartingIndex,
    ULONG Length
    );

NTSYSAPI
ULONG
NTAPI
RtlFindNextForwardRunClear (
    IN PRTL_BITMAP BitMapHeader,
    IN ULONG FromIndex,
    IN PULONG StartingRunIndex
    );

NTSYSAPI
ULONG
NTAPI
RtlFindLastBackwardRunClear (
    IN PRTL_BITMAP BitMapHeader,
    IN ULONG FromIndex,
    IN PULONG StartingRunIndex
    );

//
//  The following two procedures return to the caller a value indicating
//  the position within a ULONGLONG of the most or least significant non-zero
//  bit.  A value of zero results in a return value of -1.
//

NTSYSAPI
CCHAR
NTAPI
RtlFindLeastSignificantBit (
    IN ULONGLONG Set
    );

NTSYSAPI
CCHAR
NTAPI
RtlFindMostSignificantBit (
    IN ULONGLONG Set
    );

//
//  Security ID RTL routine definitions
//


NTSYSAPI
BOOLEAN
NTAPI
RtlValidSid (
    PSID Sid
    );


NTSYSAPI
BOOLEAN
NTAPI
RtlEqualSid (
    PSID Sid1,
    PSID Sid2
    );


NTSYSAPI
BOOLEAN
NTAPI
RtlEqualPrefixSid (
    PSID Sid1,
    PSID Sid2
    );


NTSYSAPI
ULONG
NTAPI
RtlLengthRequiredSid (
    ULONG SubAuthorityCount
    );

NTSYSAPI                                            
NTSTATUS                                            
NTAPI                                               
RtlInitializeSid (                                  
    PSID Sid,                                       
    PSID_IDENTIFIER_AUTHORITY IdentifierAuthority,  
    UCHAR SubAuthorityCount                         
    );                                              
NTSYSAPI                                            
PULONG                                              
NTAPI                                               
RtlSubAuthoritySid (                                
    PSID Sid,                                       
    ULONG SubAuthority                              
    );                                              
NTSYSAPI
ULONG
NTAPI
RtlLengthSid (
    PSID Sid
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlCopySid (
    ULONG DestinationSidLength,
    PSID DestinationSid,
    PSID SourceSid
    );

//
// LUID RTL routine definitions
//

// begin_ntddk

//
// BOOLEAN
// RtlEqualLuid(
//      PLUID L1,
//      PLUID L2
//      );

#define RtlEqualLuid(L1, L2) (((L1)->LowPart == (L2)->LowPart) && \
                              ((L1)->HighPart  == (L2)->HighPart))

//
// BOOLEAN
// RtlIsZeroLuid(
//      PLUID L1
//      );
//
#define RtlIsZeroLuid(L1) ((BOOLEAN) (((L1)->LowPart | (L1)->HighPart) == 0))


#if !defined(MIDL_PASS)

__inline LUID
NTAPI
RtlConvertLongToLuid(
    LONG Long
    )
{
    LUID TempLuid;
    LARGE_INTEGER TempLi;

    TempLi = RtlConvertLongToLargeInteger(Long);
    TempLuid.LowPart = TempLi.LowPart;
    TempLuid.HighPart = TempLi.HighPart;
    return(TempLuid);
}

__inline LUID
NTAPI
RtlConvertUlongToLuid(
    ULONG Ulong
    )
{
    LUID TempLuid;

    TempLuid.LowPart = Ulong;
    TempLuid.HighPart = 0;
    return(TempLuid);
}
#endif

// end_ntddk

NTSYSAPI
VOID
NTAPI
RtlCopyLuid (
    PLUID DestinationLuid,
    PLUID SourceLuid
    );


NTSYSAPI
VOID
NTAPI
RtlMapGenericMask(
    PACCESS_MASK AccessMask,
    PGENERIC_MAPPING GenericMapping
    );
NTSYSAPI                                        
NTSTATUS                                        
NTAPI                                           
RtlCreateAcl (                                  
    PACL Acl,                                   
    ULONG AclLength,                            
    ULONG AclRevision                           
    );                                          
NTSYSAPI                                        
NTSTATUS                                        
NTAPI                                           
RtlAddAccessAllowedAce (                        
    PACL Acl,                                   
    ULONG AceRevision,                          
    ACCESS_MASK AccessMask,                     
    PSID Sid                                    
    );                                          
//
//  SecurityDescriptor RTL routine definitions
//

NTSYSAPI
NTSTATUS
NTAPI
RtlCreateSecurityDescriptor (
    PSECURITY_DESCRIPTOR SecurityDescriptor,
    ULONG Revision
    );

// end_ntddk

NTSYSAPI
NTSTATUS
NTAPI
RtlCreateSecurityDescriptorRelative (
    PISECURITY_DESCRIPTOR_RELATIVE SecurityDescriptor,
    ULONG Revision
    );

// begin_ntddk

NTSYSAPI
BOOLEAN
NTAPI
RtlValidSecurityDescriptor (
    PSECURITY_DESCRIPTOR SecurityDescriptor
    );


NTSYSAPI
ULONG
NTAPI
RtlLengthSecurityDescriptor (
    PSECURITY_DESCRIPTOR SecurityDescriptor
    );

NTSYSAPI
BOOLEAN
NTAPI
RtlValidRelativeSecurityDescriptor (
    IN PSECURITY_DESCRIPTOR SecurityDescriptorInput,
    IN ULONG SecurityDescriptorLength,
    IN SECURITY_INFORMATION RequiredInformation
    );


NTSYSAPI
NTSTATUS
NTAPI
RtlSetDaclSecurityDescriptor (
    PSECURITY_DESCRIPTOR SecurityDescriptor,
    BOOLEAN DaclPresent,
    PACL Dacl,
    BOOLEAN DaclDefaulted
    );

NTSYSAPI                                        
NTSTATUS                                        
NTAPI                                           
RtlSetOwnerSecurityDescriptor (                 
    PSECURITY_DESCRIPTOR SecurityDescriptor,    
    PSID Owner,                                 
    BOOLEAN OwnerDefaulted                      
    );                                          
NTSYSAPI                                        
NTSTATUS                                        
NTAPI                                           
RtlGetOwnerSecurityDescriptor (                 
    PSECURITY_DESCRIPTOR SecurityDescriptor,    
    PSID *Owner,                                
    PBOOLEAN OwnerDefaulted                     
    );                                          

NTSYSAPI
NTSTATUS
NTAPI
RtlCustomCPToUnicodeN(
    IN PCPTABLEINFO CustomCP,
    OUT PWCH UnicodeString,
    IN ULONG MaxBytesInUnicodeString,
    OUT PULONG BytesInUnicodeString OPTIONAL,
    IN PCH CustomCPString,
    IN ULONG BytesInCustomCPString
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlUnicodeToCustomCPN(
    IN PCPTABLEINFO CustomCP,
    OUT PCH CustomCPString,
    IN ULONG MaxBytesInCustomCPString,
    OUT PULONG BytesInCustomCPString OPTIONAL,
    IN PWCH UnicodeString,
    IN ULONG BytesInUnicodeString
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlUpcaseUnicodeToCustomCPN(
    IN PCPTABLEINFO CustomCP,
    OUT PCH CustomCPString,
    IN ULONG MaxBytesInCustomCPString,
    OUT PULONG BytesInCustomCPString OPTIONAL,
    IN PWCH UnicodeString,
    IN ULONG BytesInUnicodeString
    );

NTSYSAPI
VOID
NTAPI
RtlInitCodePageTable(
    IN PUSHORT TableBase,
    OUT PCPTABLEINFO CodePageTable
    );


//
// Routine for converting from a volume device object to a DOS name.
//

NTSYSAPI
NTSTATUS
NTAPI
RtlVolumeDeviceToDosName(
    IN  PVOID           VolumeDeviceObject,
    OUT PUNICODE_STRING DosName
    );

//
// Define I/O system data structure type codes.  Each major data structure in
// the I/O system has a type code  The type field in each structure is at the
// same offset.  The following values can be used to determine which type of
// data structure a pointer refers to.
//

#define IO_TYPE_ADAPTER                 0x00000001
#define IO_TYPE_CONTROLLER              0x00000002
#define IO_TYPE_DEVICE                  0x00000003
#define IO_TYPE_DRIVER                  0x00000004
#define IO_TYPE_FILE                    0x00000005
#define IO_TYPE_IRP                     0x00000006
#define IO_TYPE_MASTER_ADAPTER          0x00000007
#define IO_TYPE_OPEN_PACKET             0x00000008
#define IO_TYPE_TIMER                   0x00000009
#define IO_TYPE_VPB                     0x0000000a
#define IO_TYPE_ERROR_LOG               0x0000000b
#define IO_TYPE_ERROR_MESSAGE           0x0000000c
#define IO_TYPE_DEVICE_OBJECT_EXTENSION 0x0000000d


//
// Define the major function codes for IRPs.
//


#define IRP_MJ_CREATE                   0x00
#define IRP_MJ_CREATE_NAMED_PIPE        0x01
#define IRP_MJ_CLOSE                    0x02
#define IRP_MJ_READ                     0x03
#define IRP_MJ_WRITE                    0x04
#define IRP_MJ_QUERY_INFORMATION        0x05
#define IRP_MJ_SET_INFORMATION          0x06
#define IRP_MJ_QUERY_EA                 0x07
#define IRP_MJ_SET_EA                   0x08
#define IRP_MJ_FLUSH_BUFFERS            0x09
#define IRP_MJ_QUERY_VOLUME_INFORMATION 0x0a
#define IRP_MJ_SET_VOLUME_INFORMATION   0x0b
#define IRP_MJ_DIRECTORY_CONTROL        0x0c
#define IRP_MJ_FILE_SYSTEM_CONTROL      0x0d
#define IRP_MJ_DEVICE_CONTROL           0x0e
#define IRP_MJ_INTERNAL_DEVICE_CONTROL  0x0f
#define IRP_MJ_SHUTDOWN                 0x10
#define IRP_MJ_LOCK_CONTROL             0x11
#define IRP_MJ_CLEANUP                  0x12
#define IRP_MJ_CREATE_MAILSLOT          0x13
#define IRP_MJ_QUERY_SECURITY           0x14
#define IRP_MJ_SET_SECURITY             0x15
#define IRP_MJ_POWER                    0x16
#define IRP_MJ_SYSTEM_CONTROL           0x17
#define IRP_MJ_DEVICE_CHANGE            0x18
#define IRP_MJ_QUERY_QUOTA              0x19
#define IRP_MJ_SET_QUOTA                0x1a
#define IRP_MJ_PNP                      0x1b
#define IRP_MJ_PNP_POWER                IRP_MJ_PNP      // Obsolete....
#define IRP_MJ_MAXIMUM_FUNCTION         0x1b

//
// Make the Scsi major code the same as internal device control.
//

#define IRP_MJ_SCSI                     IRP_MJ_INTERNAL_DEVICE_CONTROL

//
// Define the minor function codes for IRPs.  The lower 128 codes, from 0x00 to
// 0x7f are reserved to Microsoft.  The upper 128 codes, from 0x80 to 0xff, are
// reserved to customers of Microsoft.
//

// end_wdm end_ntndis
//
// Directory control minor function codes
//

#define IRP_MN_QUERY_DIRECTORY          0x01
#define IRP_MN_NOTIFY_CHANGE_DIRECTORY  0x02

//
// File system control minor function codes.  Note that "user request" is
// assumed to be zero by both the I/O system and file systems.  Do not change
// this value.
//

#define IRP_MN_USER_FS_REQUEST          0x00
#define IRP_MN_MOUNT_VOLUME             0x01
#define IRP_MN_VERIFY_VOLUME            0x02
#define IRP_MN_LOAD_FILE_SYSTEM         0x03
#define IRP_MN_TRACK_LINK               0x04    // To be obsoleted soon
#define IRP_MN_KERNEL_CALL              0x04

//
// Lock control minor function codes
//

#define IRP_MN_LOCK                     0x01
#define IRP_MN_UNLOCK_SINGLE            0x02
#define IRP_MN_UNLOCK_ALL               0x03
#define IRP_MN_UNLOCK_ALL_BY_KEY        0x04

//
// Read and Write minor function codes for file systems supporting Lan Manager
// software.  All of these subfunction codes are invalid if the file has been
// opened with FO_NO_INTERMEDIATE_BUFFERING.  They are also invalid in combi-
// nation with synchronous calls (Irp Flag or file open option).
//
// Note that "normal" is assumed to be zero by both the I/O system and file
// systems.  Do not change this value.
//

#define IRP_MN_NORMAL                   0x00
#define IRP_MN_DPC                      0x01
#define IRP_MN_MDL                      0x02
#define IRP_MN_COMPLETE                 0x04
#define IRP_MN_COMPRESSED               0x08

#define IRP_MN_MDL_DPC                  (IRP_MN_MDL | IRP_MN_DPC)
#define IRP_MN_COMPLETE_MDL             (IRP_MN_COMPLETE | IRP_MN_MDL)
#define IRP_MN_COMPLETE_MDL_DPC         (IRP_MN_COMPLETE_MDL | IRP_MN_DPC)

// begin_wdm
//
// Device Control Request minor function codes for SCSI support. Note that
// user requests are assumed to be zero.
//

#define IRP_MN_SCSI_CLASS               0x01

//
// PNP minor function codes.
//

#define IRP_MN_START_DEVICE                 0x00
#define IRP_MN_QUERY_REMOVE_DEVICE          0x01
#define IRP_MN_REMOVE_DEVICE                0x02
#define IRP_MN_CANCEL_REMOVE_DEVICE         0x03
#define IRP_MN_STOP_DEVICE                  0x04
#define IRP_MN_QUERY_STOP_DEVICE            0x05
#define IRP_MN_CANCEL_STOP_DEVICE           0x06

#define IRP_MN_QUERY_DEVICE_RELATIONS       0x07
#define IRP_MN_QUERY_INTERFACE              0x08
#define IRP_MN_QUERY_CAPABILITIES           0x09
#define IRP_MN_QUERY_RESOURCES              0x0A
#define IRP_MN_QUERY_RESOURCE_REQUIREMENTS  0x0B
#define IRP_MN_QUERY_DEVICE_TEXT            0x0C
#define IRP_MN_FILTER_RESOURCE_REQUIREMENTS 0x0D

#define IRP_MN_READ_CONFIG                  0x0F
#define IRP_MN_WRITE_CONFIG                 0x10
#define IRP_MN_EJECT                        0x11
#define IRP_MN_SET_LOCK                     0x12
#define IRP_MN_QUERY_ID                     0x13
#define IRP_MN_QUERY_PNP_DEVICE_STATE       0x14
#define IRP_MN_QUERY_BUS_INFORMATION        0x15
#define IRP_MN_DEVICE_USAGE_NOTIFICATION    0x16
#define IRP_MN_SURPRISE_REMOVAL             0x17
// end_wdm
#define IRP_MN_QUERY_LEGACY_BUS_INFORMATION 0x18
// begin_wdm

//
// POWER minor function codes
//
#define IRP_MN_WAIT_WAKE                    0x00
#define IRP_MN_POWER_SEQUENCE               0x01
#define IRP_MN_SET_POWER                    0x02
#define IRP_MN_QUERY_POWER                  0x03

// begin_ntminiport
//
// WMI minor function codes under IRP_MJ_SYSTEM_CONTROL
//

#define IRP_MN_QUERY_ALL_DATA               0x00
#define IRP_MN_QUERY_SINGLE_INSTANCE        0x01
#define IRP_MN_CHANGE_SINGLE_INSTANCE       0x02
#define IRP_MN_CHANGE_SINGLE_ITEM           0x03
#define IRP_MN_ENABLE_EVENTS                0x04
#define IRP_MN_DISABLE_EVENTS               0x05
#define IRP_MN_ENABLE_COLLECTION            0x06
#define IRP_MN_DISABLE_COLLECTION           0x07
#define IRP_MN_REGINFO                      0x08
#define IRP_MN_EXECUTE_METHOD               0x09

// end_ntminiport

//
// Define option flags for IoCreateFile.  Note that these values must be
// exactly the same as the SL_... flags for a create function.  Note also
// that there are flags that may be passed to IoCreateFile that are not
// placed in the stack location for the create IRP.  These flags start in
// the next byte.
//

#define IO_FORCE_ACCESS_CHECK           0x0001
#define IO_OPEN_PAGING_FILE             0x0002
#define IO_OPEN_TARGET_DIRECTORY        0x0004
// end_ntddk end_wdm end_nthal

//
// Flags not passed to driver
//

// begin_ntddk begin_wdm
#define IO_NO_PARAMETER_CHECKING        0x0100

//
// Define Information fields for whether or not a REPARSE or a REMOUNT has
// occurred in the file system.
//

#define IO_REPARSE                      0x0
#define IO_REMOUNT                      0x1

// end_ntddk end_wdm

#define IO_CHECK_CREATE_PARAMETERS      0x0200
#define IO_ATTACH_DEVICE                0x0400

//
// Define kernel-only, internal option flags
//

#define IO_ATTACH_DEVICE_API            0x80000000


//
// Define the objects that can be created by IoCreateFile.
//

typedef enum _CREATE_FILE_TYPE {
    CreateFileTypeNone,
    CreateFileTypeNamedPipe,
    CreateFileTypeMailslot
} CREATE_FILE_TYPE;

//
// Define the structures used by the I/O system
//

//
// Define empty typedefs for the _IRP, _DEVICE_OBJECT, and _DRIVER_OBJECT
// structures so they may be referenced by function types before they are
// actually defined.
//
struct _DEVICE_DESCRIPTION;
struct _DEVICE_OBJECT;
struct _DMA_ADAPTER;
struct _DRIVER_OBJECT;
struct _DRIVE_LAYOUT_INFORMATION;
struct _DISK_PARTITION;
struct _FILE_OBJECT;
struct _IRP;
struct _SCSI_REQUEST_BLOCK;

//
// Define the I/O version of a DPC routine.
//

typedef
VOID
(*PIO_DPC_ROUTINE) (
    IN PKDPC Dpc,
    IN struct _DEVICE_OBJECT *DeviceObject,
    IN struct _IRP *Irp,
    IN PVOID Context
    );

//
// Define driver timer routine type.
//

typedef
VOID
(*PIO_TIMER_ROUTINE) (
    IN struct _DEVICE_OBJECT *DeviceObject,
    IN PVOID Context
    );

//
// Define driver initialization routine type.
//

typedef
NTSTATUS
(*PDRIVER_INITIALIZE) (
    IN struct _DRIVER_OBJECT *DriverObject,
    IN PUNICODE_STRING RegistryPath
    );

// end_wdm
//
// Define driver reinitialization routine type.
//

typedef
VOID
(*PDRIVER_REINITIALIZE) (
    IN struct _DRIVER_OBJECT *DriverObject,
    IN PVOID Context,
    IN ULONG Count
    );

// begin_wdm begin_ntndis
//
// Define driver cancel routine type.
//

typedef
VOID
(*PDRIVER_CANCEL) (
    IN struct _DEVICE_OBJECT *DeviceObject,
    IN struct _IRP *Irp
    );

//
// Define driver dispatch routine type.
//

typedef
NTSTATUS
(*PDRIVER_DISPATCH) (
    IN struct _DEVICE_OBJECT *DeviceObject,
    IN struct _IRP *Irp
    );

//
// Define driver start I/O routine type.
//

typedef
VOID
(*PDRIVER_STARTIO) (
    IN struct _DEVICE_OBJECT *DeviceObject,
    IN struct _IRP *Irp
    );

//
// Define driver unload routine type.
//

typedef
VOID
(*PDRIVER_UNLOAD) (
    IN struct _DRIVER_OBJECT *DriverObject
    );

//
// Define driver AddDevice routine type.
//

typedef
NTSTATUS
(*PDRIVER_ADD_DEVICE) (
    IN struct _DRIVER_OBJECT *DriverObject,
    IN struct _DEVICE_OBJECT *PhysicalDeviceObject
    );

// end_ntddk end_wdm end_nthal end_ntndis

//
// Define driver FS notification change routine type.
//

typedef
VOID
(*PDRIVER_FS_NOTIFICATION) (
    IN struct _DEVICE_OBJECT *DeviceObject,
    IN BOOLEAN FsActive
    );

// begin_ntddk begin_wdm

//
// Define fast I/O procedure prototypes.
//
// Fast I/O read and write procedures.
//

typedef
BOOLEAN
(*PFAST_IO_CHECK_IF_POSSIBLE) (
    IN struct _FILE_OBJECT *FileObject,
    IN PLARGE_INTEGER FileOffset,
    IN ULONG Length,
    IN BOOLEAN Wait,
    IN ULONG LockKey,
    IN BOOLEAN CheckForReadOperation,
    OUT PIO_STATUS_BLOCK IoStatus,
    IN struct _DEVICE_OBJECT *DeviceObject
    );

typedef
BOOLEAN
(*PFAST_IO_READ) (
    IN struct _FILE_OBJECT *FileObject,
    IN PLARGE_INTEGER FileOffset,
    IN ULONG Length,
    IN BOOLEAN Wait,
    IN ULONG LockKey,
    OUT PVOID Buffer,
    OUT PIO_STATUS_BLOCK IoStatus,
    IN struct _DEVICE_OBJECT *DeviceObject
    );

typedef
BOOLEAN
(*PFAST_IO_WRITE) (
    IN struct _FILE_OBJECT *FileObject,
    IN PLARGE_INTEGER FileOffset,
    IN ULONG Length,
    IN BOOLEAN Wait,
    IN ULONG LockKey,
    IN PVOID Buffer,
    OUT PIO_STATUS_BLOCK IoStatus,
    IN struct _DEVICE_OBJECT *DeviceObject
    );

//
// Fast I/O query basic and standard information procedures.
//

typedef
BOOLEAN
(*PFAST_IO_QUERY_BASIC_INFO) (
    IN struct _FILE_OBJECT *FileObject,
    IN BOOLEAN Wait,
    OUT PFILE_BASIC_INFORMATION Buffer,
    OUT PIO_STATUS_BLOCK IoStatus,
    IN struct _DEVICE_OBJECT *DeviceObject
    );

typedef
BOOLEAN
(*PFAST_IO_QUERY_STANDARD_INFO) (
    IN struct _FILE_OBJECT *FileObject,
    IN BOOLEAN Wait,
    OUT PFILE_STANDARD_INFORMATION Buffer,
    OUT PIO_STATUS_BLOCK IoStatus,
    IN struct _DEVICE_OBJECT *DeviceObject
    );

//
// Fast I/O lock and unlock procedures.
//

typedef
BOOLEAN
(*PFAST_IO_LOCK) (
    IN struct _FILE_OBJECT *FileObject,
    IN PLARGE_INTEGER FileOffset,
    IN PLARGE_INTEGER Length,
    PEPROCESS ProcessId,
    ULONG Key,
    BOOLEAN FailImmediately,
    BOOLEAN ExclusiveLock,
    OUT PIO_STATUS_BLOCK IoStatus,
    IN struct _DEVICE_OBJECT *DeviceObject
    );

typedef
BOOLEAN
(*PFAST_IO_UNLOCK_SINGLE) (
    IN struct _FILE_OBJECT *FileObject,
    IN PLARGE_INTEGER FileOffset,
    IN PLARGE_INTEGER Length,
    PEPROCESS ProcessId,
    ULONG Key,
    OUT PIO_STATUS_BLOCK IoStatus,
    IN struct _DEVICE_OBJECT *DeviceObject
    );

typedef
BOOLEAN
(*PFAST_IO_UNLOCK_ALL) (
    IN struct _FILE_OBJECT *FileObject,
    PEPROCESS ProcessId,
    OUT PIO_STATUS_BLOCK IoStatus,
    IN struct _DEVICE_OBJECT *DeviceObject
    );

typedef
BOOLEAN
(*PFAST_IO_UNLOCK_ALL_BY_KEY) (
    IN struct _FILE_OBJECT *FileObject,
    PVOID ProcessId,
    ULONG Key,
    OUT PIO_STATUS_BLOCK IoStatus,
    IN struct _DEVICE_OBJECT *DeviceObject
    );

//
// Fast I/O device control procedure.
//

typedef
BOOLEAN
(*PFAST_IO_DEVICE_CONTROL) (
    IN struct _FILE_OBJECT *FileObject,
    IN BOOLEAN Wait,
    IN PVOID InputBuffer OPTIONAL,
    IN ULONG InputBufferLength,
    OUT PVOID OutputBuffer OPTIONAL,
    IN ULONG OutputBufferLength,
    IN ULONG IoControlCode,
    OUT PIO_STATUS_BLOCK IoStatus,
    IN struct _DEVICE_OBJECT *DeviceObject
    );

//
// Define callbacks for NtCreateSection to synchronize correctly with
// the file system.  It pre-acquires the resources that will be needed
// when calling to query and set file/allocation size in the file system.
//

typedef
VOID
(*PFAST_IO_ACQUIRE_FILE) (
    IN struct _FILE_OBJECT *FileObject
    );

typedef
VOID
(*PFAST_IO_RELEASE_FILE) (
    IN struct _FILE_OBJECT *FileObject
    );

//
// Define callback for drivers that have device objects attached to lower-
// level drivers' device objects.  This callback is made when the lower-level
// driver is deleting its device object.
//

typedef
VOID
(*PFAST_IO_DETACH_DEVICE) (
    IN struct _DEVICE_OBJECT *SourceDevice,
    IN struct _DEVICE_OBJECT *TargetDevice
    );

//
// This structure is used by the server to quickly get the information needed
// to service a server open call.  It is takes what would be two fast io calls
// one for basic information and the other for standard information and makes
// it into one call.
//

typedef
BOOLEAN
(*PFAST_IO_QUERY_NETWORK_OPEN_INFO) (
    IN struct _FILE_OBJECT *FileObject,
    IN BOOLEAN Wait,
    OUT struct _FILE_NETWORK_OPEN_INFORMATION *Buffer,
    OUT struct _IO_STATUS_BLOCK *IoStatus,
    IN struct _DEVICE_OBJECT *DeviceObject
    );

//
//  Define Mdl-based routines for the server to call
//

typedef
BOOLEAN
(*PFAST_IO_MDL_READ) (
    IN struct _FILE_OBJECT *FileObject,
    IN PLARGE_INTEGER FileOffset,
    IN ULONG Length,
    IN ULONG LockKey,
    OUT PMDL *MdlChain,
    OUT PIO_STATUS_BLOCK IoStatus,
    IN struct _DEVICE_OBJECT *DeviceObject
    );

typedef
BOOLEAN
(*PFAST_IO_MDL_READ_COMPLETE) (
    IN struct _FILE_OBJECT *FileObject,
    IN PMDL MdlChain,
    IN struct _DEVICE_OBJECT *DeviceObject
    );

typedef
BOOLEAN
(*PFAST_IO_PREPARE_MDL_WRITE) (
    IN struct _FILE_OBJECT *FileObject,
    IN PLARGE_INTEGER FileOffset,
    IN ULONG Length,
    IN ULONG LockKey,
    OUT PMDL *MdlChain,
    OUT PIO_STATUS_BLOCK IoStatus,
    IN struct _DEVICE_OBJECT *DeviceObject
    );

typedef
BOOLEAN
(*PFAST_IO_MDL_WRITE_COMPLETE) (
    IN struct _FILE_OBJECT *FileObject,
    IN PLARGE_INTEGER FileOffset,
    IN PMDL MdlChain,
    IN struct _DEVICE_OBJECT *DeviceObject
    );

//
//  If this routine is present, it will be called by FsRtl
//  to acquire the file for the mapped page writer.
//

typedef
NTSTATUS
(*PFAST_IO_ACQUIRE_FOR_MOD_WRITE) (
    IN struct _FILE_OBJECT *FileObject,
    IN PLARGE_INTEGER EndingOffset,
    OUT struct _ERESOURCE **ResourceToRelease,
    IN struct _DEVICE_OBJECT *DeviceObject
             );

typedef
NTSTATUS
(*PFAST_IO_RELEASE_FOR_MOD_WRITE) (
    IN struct _FILE_OBJECT *FileObject,
    IN struct _ERESOURCE *ResourceToRelease,
    IN struct _DEVICE_OBJECT *DeviceObject
             );

//
//  If this routine is present, it will be called by FsRtl
//  to acquire the file for the mapped page writer.
//

typedef
NTSTATUS
(*PFAST_IO_ACQUIRE_FOR_CCFLUSH) (
    IN struct _FILE_OBJECT *FileObject,
    IN struct _DEVICE_OBJECT *DeviceObject
             );

typedef
NTSTATUS
(*PFAST_IO_RELEASE_FOR_CCFLUSH) (
    IN struct _FILE_OBJECT *FileObject,
    IN struct _DEVICE_OBJECT *DeviceObject
             );

typedef
BOOLEAN
(*PFAST_IO_READ_COMPRESSED) (
    IN struct _FILE_OBJECT *FileObject,
    IN PLARGE_INTEGER FileOffset,
    IN ULONG Length,
    IN ULONG LockKey,
    OUT PVOID Buffer,
    OUT PMDL *MdlChain,
    OUT PIO_STATUS_BLOCK IoStatus,
    OUT struct _COMPRESSED_DATA_INFO *CompressedDataInfo,
    IN ULONG CompressedDataInfoLength,
    IN struct _DEVICE_OBJECT *DeviceObject
    );

typedef
BOOLEAN
(*PFAST_IO_WRITE_COMPRESSED) (
    IN struct _FILE_OBJECT *FileObject,
    IN PLARGE_INTEGER FileOffset,
    IN ULONG Length,
    IN ULONG LockKey,
    IN PVOID Buffer,
    OUT PMDL *MdlChain,
    OUT PIO_STATUS_BLOCK IoStatus,
    IN struct _COMPRESSED_DATA_INFO *CompressedDataInfo,
    IN ULONG CompressedDataInfoLength,
    IN struct _DEVICE_OBJECT *DeviceObject
    );

typedef
BOOLEAN
(*PFAST_IO_MDL_READ_COMPLETE_COMPRESSED) (
    IN struct _FILE_OBJECT *FileObject,
    IN PMDL MdlChain,
    IN struct _DEVICE_OBJECT *DeviceObject
    );

typedef
BOOLEAN
(*PFAST_IO_MDL_WRITE_COMPLETE_COMPRESSED) (
    IN struct _FILE_OBJECT *FileObject,
    IN PLARGE_INTEGER FileOffset,
    IN PMDL MdlChain,
    IN struct _DEVICE_OBJECT *DeviceObject
    );

typedef
BOOLEAN
(*PFAST_IO_QUERY_OPEN) (
    IN struct _IRP *Irp,
    OUT PFILE_NETWORK_OPEN_INFORMATION NetworkInformation,
    IN struct _DEVICE_OBJECT *DeviceObject
    );

//
// Define the structure to describe the Fast I/O dispatch routines.  Any
// additions made to this structure MUST be added monotonically to the end
// of the structure, and fields CANNOT be removed from the middle.
//

typedef struct _FAST_IO_DISPATCH {
    ULONG SizeOfFastIoDispatch;
    PFAST_IO_CHECK_IF_POSSIBLE FastIoCheckIfPossible;
    PFAST_IO_READ FastIoRead;
    PFAST_IO_WRITE FastIoWrite;
    PFAST_IO_QUERY_BASIC_INFO FastIoQueryBasicInfo;
    PFAST_IO_QUERY_STANDARD_INFO FastIoQueryStandardInfo;
    PFAST_IO_LOCK FastIoLock;
    PFAST_IO_UNLOCK_SINGLE FastIoUnlockSingle;
    PFAST_IO_UNLOCK_ALL FastIoUnlockAll;
    PFAST_IO_UNLOCK_ALL_BY_KEY FastIoUnlockAllByKey;
    PFAST_IO_DEVICE_CONTROL FastIoDeviceControl;
    PFAST_IO_ACQUIRE_FILE AcquireFileForNtCreateSection;
    PFAST_IO_RELEASE_FILE ReleaseFileForNtCreateSection;
    PFAST_IO_DETACH_DEVICE FastIoDetachDevice;
    PFAST_IO_QUERY_NETWORK_OPEN_INFO FastIoQueryNetworkOpenInfo;
    PFAST_IO_ACQUIRE_FOR_MOD_WRITE AcquireForModWrite;
    PFAST_IO_MDL_READ MdlRead;
    PFAST_IO_MDL_READ_COMPLETE MdlReadComplete;
    PFAST_IO_PREPARE_MDL_WRITE PrepareMdlWrite;
    PFAST_IO_MDL_WRITE_COMPLETE MdlWriteComplete;
    PFAST_IO_READ_COMPRESSED FastIoReadCompressed;
    PFAST_IO_WRITE_COMPRESSED FastIoWriteCompressed;
    PFAST_IO_MDL_READ_COMPLETE_COMPRESSED MdlReadCompleteCompressed;
    PFAST_IO_MDL_WRITE_COMPLETE_COMPRESSED MdlWriteCompleteCompressed;
    PFAST_IO_QUERY_OPEN FastIoQueryOpen;
    PFAST_IO_RELEASE_FOR_MOD_WRITE ReleaseForModWrite;
    PFAST_IO_ACQUIRE_FOR_CCFLUSH AcquireForCcFlush;
    PFAST_IO_RELEASE_FOR_CCFLUSH ReleaseForCcFlush;
} FAST_IO_DISPATCH, *PFAST_IO_DISPATCH;

// end_ntddk end_wdm

// begin_ntddk begin_wdm begin_nthal
//
// Define the actions that a driver execution routine may request of the
// adapter/controller allocation routines upon return.
//

typedef enum _IO_ALLOCATION_ACTION {
    KeepObject = 1,
    DeallocateObject,
    DeallocateObjectKeepRegisters
} IO_ALLOCATION_ACTION, *PIO_ALLOCATION_ACTION;

//
// Define device driver adapter/controller execution routine.
//

typedef
IO_ALLOCATION_ACTION
(*PDRIVER_CONTROL) (
    IN struct _DEVICE_OBJECT *DeviceObject,
    IN struct _IRP *Irp,
    IN PVOID MapRegisterBase,
    IN PVOID Context
    );

//
// Define the I/O system's security context type for use by file system's
// when checking access to volumes, files, and directories.
//

typedef struct _IO_SECURITY_CONTEXT {
    PSECURITY_QUALITY_OF_SERVICE SecurityQos;
    PACCESS_STATE AccessState;
    ACCESS_MASK DesiredAccess;
    ULONG FullCreateOptions;
} IO_SECURITY_CONTEXT, *PIO_SECURITY_CONTEXT;

//
// Define Volume Parameter Block (VPB) flags.
//

#define VPB_MOUNTED                     0x00000001
#define VPB_LOCKED                      0x00000002
#define VPB_PERSISTENT                  0x00000004
#define VPB_REMOVE_PENDING              0x00000008
#define VPB_RAW_MOUNT                   0x00000010  


//
// Volume Parameter Block (VPB)
//

#define MAXIMUM_VOLUME_LABEL_LENGTH  (32 * sizeof(WCHAR)) // 32 characters

typedef struct _VPB {
    CSHORT Type;
    CSHORT Size;
    USHORT Flags;
    USHORT VolumeLabelLength; // in bytes
    struct _DEVICE_OBJECT *DeviceObject;
    struct _DEVICE_OBJECT *RealDevice;
    ULONG SerialNumber;
    ULONG ReferenceCount;
    WCHAR VolumeLabel[MAXIMUM_VOLUME_LABEL_LENGTH / sizeof(WCHAR)];
} VPB, *PVPB;


#if defined(_AXP64_)

//
// Use __inline DMA macros (hal.h)
//
#ifndef USE_DMA_MACROS
#define USE_DMA_MACROS
#endif

//
// Only PnP drivers!
//
#ifndef NO_LEGACY_DRIVERS
#define NO_LEGACY_DRIVERS
#endif

#endif // _AXP64_


#if defined(USE_DMA_MACROS) && (defined(_NTDDK_) || defined(_NTDRIVER_))

//  begin_wdm
//
// Define object type specific fields of various objects used by the I/O system
//

typedef struct _DMA_ADAPTER *PADAPTER_OBJECT;

// end_wdm
#else

//
// Define object type specific fields of various objects used by the I/O system
//

typedef struct _ADAPTER_OBJECT *PADAPTER_OBJECT; // ntndis

#endif // USE_DMA_MACROS && (_NTDDK_ || _NTDRIVER_)

//  begin_wdm
//
// Define Wait Context Block (WCB)
//

typedef struct _WAIT_CONTEXT_BLOCK {
    KDEVICE_QUEUE_ENTRY WaitQueueEntry;
    PDRIVER_CONTROL DeviceRoutine;
    PVOID DeviceContext;
    ULONG NumberOfMapRegisters;
    PVOID DeviceObject;
    PVOID CurrentIrp;
    PKDPC BufferChainingDpc;
} WAIT_CONTEXT_BLOCK, *PWAIT_CONTEXT_BLOCK;

// end_wdm

typedef struct _CONTROLLER_OBJECT {
    CSHORT Type;
    CSHORT Size;
    PVOID ControllerExtension;
    KDEVICE_QUEUE DeviceWaitQueue;

    ULONG Spare1;
    LARGE_INTEGER Spare2;

} CONTROLLER_OBJECT, *PCONTROLLER_OBJECT;

// begin_wdm
//
// Define Device Object (DO) flags
//
#define DO_VERIFY_VOLUME                0x00000002      
#define DO_BUFFERED_IO                  0x00000004      
#define DO_EXCLUSIVE                    0x00000008      
#define DO_DIRECT_IO                    0x00000010      
#define DO_MAP_IO_BUFFER                0x00000020      
#define DO_DEVICE_HAS_NAME              0x00000040      
#define DO_DEVICE_INITIALIZING          0x00000080      
#define DO_SYSTEM_BOOT_PARTITION        0x00000100      
#define DO_LONG_TERM_REQUESTS           0x00000200      
#define DO_NEVER_LAST_DEVICE            0x00000400      
#define DO_SHUTDOWN_REGISTERED          0x00000800      
#define DO_BUS_ENUMERATED_DEVICE        0x00001000      
#define DO_POWER_PAGABLE                0x00002000      
#define DO_POWER_INRUSH                 0x00004000      
#define DO_LOW_PRIORITY_FILESYSTEM      0x00010000      
//
// Device Object structure definition
//

typedef struct _DEVICE_OBJECT {
    CSHORT Type;
    USHORT Size;
    LONG ReferenceCount;
    struct _DRIVER_OBJECT *DriverObject;
    struct _DEVICE_OBJECT *NextDevice;
    struct _DEVICE_OBJECT *AttachedDevice;
    struct _IRP *CurrentIrp;
    PIO_TIMER Timer;
    ULONG Flags;                                // See above:  DO_...
    ULONG Characteristics;                      // See ntioapi:  FILE_...
    PVPB Vpb;
    PVOID DeviceExtension;
    DEVICE_TYPE DeviceType;
    CCHAR StackSize;
    union {
        LIST_ENTRY ListEntry;
        WAIT_CONTEXT_BLOCK Wcb;
    } Queue;
    ULONG AlignmentRequirement;
    KDEVICE_QUEUE DeviceQueue;
    KDPC Dpc;

    //
    //  The following field is for exclusive use by the filesystem to keep
    //  track of the number of Fsp threads currently using the device
    //

    ULONG ActiveThreadCount;
    PSECURITY_DESCRIPTOR SecurityDescriptor;
    KEVENT DeviceLock;

    USHORT SectorSize;
    USHORT Spare1;

    struct _DEVOBJ_EXTENSION  *DeviceObjectExtension;
    PVOID  Reserved;
} DEVICE_OBJECT;
typedef struct _DEVICE_OBJECT *PDEVICE_OBJECT; // ntndis


struct  _DEVICE_OBJECT_POWER_EXTENSION;

typedef struct _DEVOBJ_EXTENSION {

    CSHORT          Type;
    USHORT          Size;

    //
    // Public part of the DeviceObjectExtension structure
    //

    PDEVICE_OBJECT  DeviceObject;               // owning device object


} DEVOBJ_EXTENSION, *PDEVOBJ_EXTENSION;

//
// Define Driver Object (DRVO) flags
//

#define DRVO_UNLOAD_INVOKED             0x00000001
#define DRVO_LEGACY_DRIVER              0x00000002
#define DRVO_BUILTIN_DRIVER             0x00000004    // Driver objects for Hal, PnP Mgr
// end_wdm
#define DRVO_REINIT_REGISTERED          0x00000008
#define DRVO_INITIALIZED                0x00000010
#define DRVO_BOOTREINIT_REGISTERED      0x00000020
#define DRVO_LEGACY_RESOURCES           0x00000040
// begin_wdm

typedef struct _DRIVER_EXTENSION {

    //
    // Back pointer to Driver Object
    //

    struct _DRIVER_OBJECT *DriverObject;

    //
    // The AddDevice entry point is called by the Plug & Play manager
    // to inform the driver when a new device instance arrives that this
    // driver must control.
    //

    PDRIVER_ADD_DEVICE AddDevice;

    //
    // The count field is used to count the number of times the driver has
    // had its registered reinitialization routine invoked.
    //

    ULONG Count;

    //
    // The service name field is used by the pnp manager to determine
    // where the driver related info is stored in the registry.
    //

    UNICODE_STRING ServiceKeyName;

    //
    // Note: any new shared fields get added here.
    //


} DRIVER_EXTENSION, *PDRIVER_EXTENSION;

typedef struct _DRIVER_OBJECT {
    CSHORT Type;
    CSHORT Size;

    //
    // The following links all of the devices created by a single driver
    // together on a list, and the Flags word provides an extensible flag
    // location for driver objects.
    //

    PDEVICE_OBJECT DeviceObject;
    ULONG Flags;

    //
    // The following section describes where the driver is loaded.  The count
    // field is used to count the number of times the driver has had its
    // registered reinitialization routine invoked.
    //

    PVOID DriverStart;
    ULONG DriverSize;
    PVOID DriverSection;
    PDRIVER_EXTENSION DriverExtension;

    //
    // The driver name field is used by the error log thread
    // determine the name of the driver that an I/O request is/was bound.
    //

    UNICODE_STRING DriverName;

    //
    // The following section is for registry support.  Thise is a pointer
    // to the path to the hardware information in the registry
    //

    PUNICODE_STRING HardwareDatabase;

    //
    // The following section contains the optional pointer to an array of
    // alternate entry points to a driver for "fast I/O" support.  Fast I/O
    // is performed by invoking the driver routine directly with separate
    // parameters, rather than using the standard IRP call mechanism.  Note
    // that these functions may only be used for synchronous I/O, and when
    // the file is cached.
    //

    PFAST_IO_DISPATCH FastIoDispatch;

    //
    // The following section describes the entry points to this particular
    // driver.  Note that the major function dispatch table must be the last
    // field in the object so that it remains extensible.
    //

    PDRIVER_INITIALIZE DriverInit;
    PDRIVER_STARTIO DriverStartIo;
    PDRIVER_UNLOAD DriverUnload;
    PDRIVER_DISPATCH MajorFunction[IRP_MJ_MAXIMUM_FUNCTION + 1];

} DRIVER_OBJECT;
typedef struct _DRIVER_OBJECT *PDRIVER_OBJECT; // ntndis



//
// The following structure is pointed to by the SectionObject pointer field
// of a file object, and is allocated by the various NT file systems.
//

typedef struct _SECTION_OBJECT_POINTERS {
    PVOID DataSectionObject;
    PVOID SharedCacheMap;
    PVOID ImageSectionObject;
} SECTION_OBJECT_POINTERS;
typedef SECTION_OBJECT_POINTERS *PSECTION_OBJECT_POINTERS;

//
// Define the format of a completion message.
//

typedef struct _IO_COMPLETION_CONTEXT {
    PVOID Port;
    PVOID Key;
} IO_COMPLETION_CONTEXT, *PIO_COMPLETION_CONTEXT;

//
// Define File Object (FO) flags
//

#define FO_FILE_OPEN                    0x00000001
#define FO_SYNCHRONOUS_IO               0x00000002
#define FO_ALERTABLE_IO                 0x00000004
#define FO_NO_INTERMEDIATE_BUFFERING    0x00000008
#define FO_WRITE_THROUGH                0x00000010
#define FO_SEQUENTIAL_ONLY              0x00000020
#define FO_CACHE_SUPPORTED              0x00000040
#define FO_NAMED_PIPE                   0x00000080
#define FO_STREAM_FILE                  0x00000100
#define FO_MAILSLOT                     0x00000200
#define FO_GENERATE_AUDIT_ON_CLOSE      0x00000400
#define FO_DIRECT_DEVICE_OPEN           0x00000800
#define FO_FILE_MODIFIED                0x00001000
#define FO_FILE_SIZE_CHANGED            0x00002000
#define FO_CLEANUP_COMPLETE             0x00004000
#define FO_TEMPORARY_FILE               0x00008000
#define FO_DELETE_ON_CLOSE              0x00010000
#define FO_OPENED_CASE_SENSITIVE        0x00020000
#define FO_HANDLE_CREATED               0x00040000
#define FO_FILE_FAST_IO_READ            0x00080000
#define FO_RANDOM_ACCESS                0x00100000
#define FO_FILE_OPEN_CANCELLED          0x00200000
#define FO_VOLUME_OPEN                  0x00400000
#define FO_REMOTE_ORIGIN                0x01000000

typedef struct _FILE_OBJECT {
    CSHORT Type;
    CSHORT Size;
    PDEVICE_OBJECT DeviceObject;
    PVPB Vpb;
    PVOID FsContext;
    PVOID FsContext2;
    PSECTION_OBJECT_POINTERS SectionObjectPointer;
    PVOID PrivateCacheMap;
    NTSTATUS FinalStatus;
    struct _FILE_OBJECT *RelatedFileObject;
    BOOLEAN LockOperation;
    BOOLEAN DeletePending;
    BOOLEAN ReadAccess;
    BOOLEAN WriteAccess;
    BOOLEAN DeleteAccess;
    BOOLEAN SharedRead;
    BOOLEAN SharedWrite;
    BOOLEAN SharedDelete;
    ULONG Flags;
    UNICODE_STRING FileName;
    LARGE_INTEGER CurrentByteOffset;
    ULONG Waiters;
    ULONG Busy;
    PVOID LastLock;
    KEVENT Lock;
    KEVENT Event;
    PIO_COMPLETION_CONTEXT CompletionContext;
} FILE_OBJECT;
typedef struct _FILE_OBJECT *PFILE_OBJECT; // ntndis

//
// Define I/O Request Packet (IRP) flags
//

#define IRP_NOCACHE                     0x00000001
#define IRP_PAGING_IO                   0x00000002
#define IRP_MOUNT_COMPLETION            0x00000002
#define IRP_SYNCHRONOUS_API             0x00000004
#define IRP_ASSOCIATED_IRP              0x00000008
#define IRP_BUFFERED_IO                 0x00000010
#define IRP_DEALLOCATE_BUFFER           0x00000020
#define IRP_INPUT_OPERATION             0x00000040
#define IRP_SYNCHRONOUS_PAGING_IO       0x00000040
#define IRP_CREATE_OPERATION            0x00000080
#define IRP_READ_OPERATION              0x00000100
#define IRP_WRITE_OPERATION             0x00000200
#define IRP_CLOSE_OPERATION             0x00000400
// end_wdm

#define IRP_DEFER_IO_COMPLETION         0x00000800
#define IRP_OB_QUERY_NAME               0x00001000
#define IRP_HOLD_DEVICE_QUEUE           0x00002000
#define IRP_RETRY_IO_COMPLETION         0x00004000

// begin_wdm
//
// Define I/O request packet (IRP) alternate flags for allocation control.
//

#define IRP_QUOTA_CHARGED               0x01
#define IRP_ALLOCATED_MUST_SUCCEED      0x02
#define IRP_ALLOCATED_FIXED_SIZE        0x04
#define IRP_LOOKASIDE_ALLOCATION        0x08

//
// I/O Request Packet (IRP) definition
//

typedef struct _IRP {
    CSHORT Type;
    USHORT Size;

    //
    // Define the common fields used to control the IRP.
    //

    //
    // Define a pointer to the Memory Descriptor List (MDL) for this I/O
    // request.  This field is only used if the I/O is "direct I/O".
    //

    PMDL MdlAddress;

    //
    // Flags word - used to remember various flags.
    //

    ULONG Flags;

    //
    // The following union is used for one of three purposes:
    //
    //    1. This IRP is an associated IRP.  The field is a pointer to a master
    //       IRP.
    //
    //    2. This is the master IRP.  The field is the count of the number of
    //       IRPs which must complete (associated IRPs) before the master can
    //       complete.
    //
    //    3. This operation is being buffered and the field is the address of
    //       the system space buffer.
    //

    union {
        struct _IRP *MasterIrp;
        LONG IrpCount;
        PVOID SystemBuffer;
    } AssociatedIrp;

    //
    // Thread list entry - allows queueing the IRP to the thread pending I/O
    // request packet list.
    //

    LIST_ENTRY ThreadListEntry;

    //
    // I/O status - final status of operation.
    //

    IO_STATUS_BLOCK IoStatus;

    //
    // Requestor mode - mode of the original requestor of this operation.
    //

    KPROCESSOR_MODE RequestorMode;

    //
    // Pending returned - TRUE if pending was initially returned as the
    // status for this packet.
    //

    BOOLEAN PendingReturned;

    //
    // Stack state information.
    //

    CHAR StackCount;
    CHAR CurrentLocation;

    //
    // Cancel - packet has been canceled.
    //

    BOOLEAN Cancel;

    //
    // Cancel Irql - Irql at which the cancel spinlock was acquired.
    //

    KIRQL CancelIrql;

    //
    // ApcEnvironment - Used to save the APC environment at the time that the
    // packet was initialized.
    //

    CCHAR ApcEnvironment;

    //
    // Allocation control flags.
    //

    UCHAR AllocationFlags;

    //
    // User parameters.
    //

    PIO_STATUS_BLOCK UserIosb;
    PKEVENT UserEvent;
    union {
        struct {
            PIO_APC_ROUTINE UserApcRoutine;
            PVOID UserApcContext;
        } AsynchronousParameters;
        LARGE_INTEGER AllocationSize;
    } Overlay;

    //
    // CancelRoutine - Used to contain the address of a cancel routine supplied
    // by a device driver when the IRP is in a cancelable state.
    //

    PDRIVER_CANCEL CancelRoutine;

    //
    // Note that the UserBuffer parameter is outside of the stack so that I/O
    // completion can copy data back into the user's address space without
    // having to know exactly which service was being invoked.  The length
    // of the copy is stored in the second half of the I/O status block. If
    // the UserBuffer field is NULL, then no copy is performed.
    //

    PVOID UserBuffer;

    //
    // Kernel structures
    //
    // The following section contains kernel structures which the IRP needs
    // in order to place various work information in kernel controller system
    // queues.  Because the size and alignment cannot be controlled, they are
    // placed here at the end so they just hang off and do not affect the
    // alignment of other fields in the IRP.
    //

    union {

        struct {

            union {

                //
                // DeviceQueueEntry - The device queue entry field is used to
                // queue the IRP to the device driver device queue.
                //

                KDEVICE_QUEUE_ENTRY DeviceQueueEntry;

                struct {

                    //
                    // The following are available to the driver to use in
                    // whatever manner is desired, while the driver owns the
                    // packet.
                    //

                    PVOID DriverContext[4];

                } ;

            } ;

            //
            // Thread - pointer to caller's Thread Control Block.
            //

            PETHREAD Thread;

            //
            // Auxiliary buffer - pointer to any auxiliary buffer that is
            // required to pass information to a driver that is not contained
            // in a normal buffer.
            //

            PCHAR AuxiliaryBuffer;

            //
            // The following unnamed structure must be exactly identical
            // to the unnamed structure used in the minipacket header used
            // for completion queue entries.
            //

            struct {

                //
                // List entry - used to queue the packet to completion queue, among
                // others.
                //

                LIST_ENTRY ListEntry;

                union {

                    //
                    // Current stack location - contains a pointer to the current
                    // IO_STACK_LOCATION structure in the IRP stack.  This field
                    // should never be directly accessed by drivers.  They should
                    // use the standard functions.
                    //

                    struct _IO_STACK_LOCATION *CurrentStackLocation;

                    //
                    // Minipacket type.
                    //

                    ULONG PacketType;
                };
            };

            //
            // Original file object - pointer to the original file object
            // that was used to open the file.  This field is owned by the
            // I/O system and should not be used by any other drivers.
            //

            PFILE_OBJECT OriginalFileObject;

        } Overlay;

        //
        // APC - This APC control block is used for the special kernel APC as
        // well as for the caller's APC, if one was specified in the original
        // argument list.  If so, then the APC is reused for the normal APC for
        // whatever mode the caller was in and the "special" routine that is
        // invoked before the APC gets control simply deallocates the IRP.
        //

        KAPC Apc;

        //
        // CompletionKey - This is the key that is used to distinguish
        // individual I/O operations initiated on a single file handle.
        //

        PVOID CompletionKey;

    } Tail;

} IRP, *PIRP;

//
// Define completion routine types for use in stack locations in an IRP
//

typedef
NTSTATUS
(*PIO_COMPLETION_ROUTINE) (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Context
    );

//
// Define stack location control flags
//

#define SL_PENDING_RETURNED             0x01
#define SL_INVOKE_ON_CANCEL             0x20
#define SL_INVOKE_ON_SUCCESS            0x40
#define SL_INVOKE_ON_ERROR              0x80

//
// Define flags for various functions
//

//
// Create / Create Named Pipe
//
// The following flags must exactly match those in the IoCreateFile call's
// options.  The case sensitive flag is added in later, by the parse routine,
// and is not an actual option to open.  Rather, it is part of the object
// manager's attributes structure.
//

#define SL_FORCE_ACCESS_CHECK           0x01
#define SL_OPEN_PAGING_FILE             0x02
#define SL_OPEN_TARGET_DIRECTORY        0x04

#define SL_CASE_SENSITIVE               0x80

//
// Read / Write
//

#define SL_KEY_SPECIFIED                0x01
#define SL_OVERRIDE_VERIFY_VOLUME       0x02
#define SL_WRITE_THROUGH                0x04
#define SL_FT_SEQUENTIAL_WRITE          0x08

//
// Device I/O Control
//
//
// Same SL_OVERRIDE_VERIFY_VOLUME as for read/write above.
//

//
// Lock
//

#define SL_FAIL_IMMEDIATELY             0x01
#define SL_EXCLUSIVE_LOCK               0x02

//
// QueryDirectory / QueryEa / QueryQuota
//

#define SL_RESTART_SCAN                 0x01
#define SL_RETURN_SINGLE_ENTRY          0x02
#define SL_INDEX_SPECIFIED              0x04

//
// NotifyDirectory
//

#define SL_WATCH_TREE                   0x01

//
// FileSystemControl
//
//    minor: mount/verify volume
//

#define SL_ALLOW_RAW_MOUNT              0x01

//
// Define PNP/POWER types required by IRP_MJ_PNP/IRP_MJ_POWER.
//

typedef enum _DEVICE_RELATION_TYPE {
    BusRelations,
    EjectionRelations,
    PowerRelations,
    RemovalRelations,
    TargetDeviceRelation
} DEVICE_RELATION_TYPE, *PDEVICE_RELATION_TYPE;

typedef struct _DEVICE_RELATIONS {
    ULONG Count;
    PDEVICE_OBJECT Objects[1];  // variable length
} DEVICE_RELATIONS, *PDEVICE_RELATIONS;

typedef enum _DEVICE_USAGE_NOTIFICATION_TYPE {
    DeviceUsageTypeUndefined,
    DeviceUsageTypePaging,
    DeviceUsageTypeHibernation,
    DeviceUsageTypeDumpFile
} DEVICE_USAGE_NOTIFICATION_TYPE;

// begin_ntminiport

typedef struct _INTERFACE {
    USHORT Size;
    USHORT Version;
    PVOID Context;
    PINTERFACE_REFERENCE InterfaceReference;
    PINTERFACE_DEREFERENCE InterfaceDereference;
    // interface specific entries go here
} INTERFACE, *PINTERFACE;

// end_ntminiport

typedef struct _DEVICE_CAPABILITIES {
    USHORT Size;
    USHORT Version;  // the version documented here is version 1
    ULONG DeviceD1:1;
    ULONG DeviceD2:1;
    ULONG LockSupported:1;
    ULONG EjectSupported:1; // Ejectable in S0
    ULONG Removable:1;
    ULONG DockDevice:1;
    ULONG UniqueID:1;
    ULONG SilentInstall:1;
    ULONG RawDeviceOK:1;
    ULONG SurpriseRemovalOK:1;
    ULONG WakeFromD0:1;
    ULONG WakeFromD1:1;
    ULONG WakeFromD2:1;
    ULONG WakeFromD3:1;
    ULONG HardwareDisabled:1;
    ULONG NonDynamic:1;
    ULONG WarmEjectSupported:1;
    ULONG NoDisplayInUI:1;
    ULONG Reserved:14;

    ULONG Address;
    ULONG UINumber;

    DEVICE_POWER_STATE DeviceState[PowerSystemMaximum];
    SYSTEM_POWER_STATE SystemWake;
    DEVICE_POWER_STATE DeviceWake;
    ULONG D1Latency;
    ULONG D2Latency;
    ULONG D3Latency;
} DEVICE_CAPABILITIES, *PDEVICE_CAPABILITIES;

typedef struct _POWER_SEQUENCE {
    ULONG SequenceD1;
    ULONG SequenceD2;
    ULONG SequenceD3;
} POWER_SEQUENCE, *PPOWER_SEQUENCE;

typedef enum {
    BusQueryDeviceID = 0,       // <Enumerator>\<Enumerator-specific device id>
    BusQueryHardwareIDs = 1,    // Hardware ids
    BusQueryCompatibleIDs = 2,  // compatible device ids
    BusQueryInstanceID = 3,     // persistent id for this instance of the device
    BusQueryDeviceSerialNumber = 4    // serial number for this device
} BUS_QUERY_ID_TYPE, *PBUS_QUERY_ID_TYPE;

typedef ULONG PNP_DEVICE_STATE, *PPNP_DEVICE_STATE;

#define PNP_DEVICE_DISABLED                      0x00000001
#define PNP_DEVICE_DONT_DISPLAY_IN_UI            0x00000002
#define PNP_DEVICE_FAILED                        0x00000004
#define PNP_DEVICE_REMOVED                       0x00000008
#define PNP_DEVICE_RESOURCE_REQUIREMENTS_CHANGED 0x00000010
#define PNP_DEVICE_NOT_DISABLEABLE               0x00000020

typedef enum {
    DeviceTextDescription = 0,            // DeviceDesc property
    DeviceTextLocationInformation = 1     // DeviceLocation property
} DEVICE_TEXT_TYPE, *PDEVICE_TEXT_TYPE;

//
// Define I/O Request Packet (IRP) stack locations
//

#if !defined(_ALPHA_) && !defined(_IA64_)
#include "pshpack4.h"
#endif

#if defined(_WIN64)
#define POINTER_ALIGNMENT DECLSPEC_ALIGN(8)
#else
#define POINTER_ALIGNMENT
#endif

typedef struct _IO_STACK_LOCATION {
    UCHAR MajorFunction;
    UCHAR MinorFunction;
    UCHAR Flags;
    UCHAR Control;

    //
    // The following user parameters are based on the service that is being
    // invoked.  Drivers and file systems can determine which set to use based
    // on the above major and minor function codes.
    //

    union {

        //
        // System service parameters for:  NtCreateFile
        //

        struct {
            PIO_SECURITY_CONTEXT SecurityContext;
            ULONG Options;
            USHORT POINTER_ALIGNMENT FileAttributes;
            USHORT ShareAccess;
            ULONG POINTER_ALIGNMENT EaLength;
        } Create;


        //
        // System service parameters for:  NtReadFile
        //

        struct {
            ULONG Length;
            ULONG POINTER_ALIGNMENT Key;
            LARGE_INTEGER ByteOffset;
        } Read;

        //
        // System service parameters for:  NtWriteFile
        //

        struct {
            ULONG Length;
            ULONG POINTER_ALIGNMENT Key;
            LARGE_INTEGER ByteOffset;
        } Write;

// end_ntddk end_wdm end_nthal

        //
        // System service parameters for:  NtQueryDirectoryFile
        //

        struct {
            ULONG Length;
            PSTRING FileName;
            FILE_INFORMATION_CLASS FileInformationClass;
            ULONG POINTER_ALIGNMENT FileIndex;
        } QueryDirectory;

        //
        // System service parameters for:  NtNotifyChangeDirectoryFile
        //

        struct {
            ULONG Length;
            ULONG POINTER_ALIGNMENT CompletionFilter;
        } NotifyDirectory;

// begin_ntddk begin_wdm begin_nthal

        //
        // System service parameters for:  NtQueryInformationFile
        //

        struct {
            ULONG Length;
            FILE_INFORMATION_CLASS POINTER_ALIGNMENT FileInformationClass;
        } QueryFile;

        //
        // System service parameters for:  NtSetInformationFile
        //

        struct {
            ULONG Length;
            FILE_INFORMATION_CLASS POINTER_ALIGNMENT FileInformationClass;
            PFILE_OBJECT FileObject;
            union {
                struct {
                    BOOLEAN ReplaceIfExists;
                    BOOLEAN AdvanceOnly;
                };
                ULONG ClusterCount;
                HANDLE DeleteHandle;
            };
        } SetFile;

// end_ntddk end_wdm end_nthal

        //
        // System service parameters for:  NtQueryEaFile
        //

        struct {
            ULONG Length;
            PVOID EaList;
            ULONG EaListLength;
            ULONG POINTER_ALIGNMENT EaIndex;
        } QueryEa;

        //
        // System service parameters for:  NtSetEaFile
        //

        struct {
            ULONG Length;
        } SetEa;

// begin_ntddk begin_wdm begin_nthal

        //
        // System service parameters for:  NtQueryVolumeInformationFile
        //

        struct {
            ULONG Length;
            FS_INFORMATION_CLASS POINTER_ALIGNMENT FsInformationClass;
        } QueryVolume;

// end_ntddk end_wdm end_nthal

        //
        // System service parameters for:  NtSetVolumeInformationFile
        //

        struct {
            ULONG Length;
            FS_INFORMATION_CLASS POINTER_ALIGNMENT FsInformationClass;
        } SetVolume;

        //
        // System service parameters for:  NtFsControlFile
        //
        // Note that the user's output buffer is stored in the UserBuffer field
        // and the user's input buffer is stored in the SystemBuffer field.
        //

        struct {
            ULONG OutputBufferLength;
            ULONG POINTER_ALIGNMENT InputBufferLength;
            ULONG POINTER_ALIGNMENT FsControlCode;
            PVOID Type3InputBuffer;
        } FileSystemControl;

        //
        // System service parameters for:  NtLockFile/NtUnlockFile
        //

        struct {
            PLARGE_INTEGER Length;
            ULONG POINTER_ALIGNMENT Key;
            LARGE_INTEGER ByteOffset;
        } LockControl;

// begin_ntddk begin_wdm begin_nthal

        //
        // System service parameters for:  NtFlushBuffersFile
        //
        // No extra user-supplied parameters.
        //

// end_ntddk end_wdm end_nthal

        //
        // System service parameters for:  NtCancelIoFile
        //
        // No extra user-supplied parameters.
        //

// begin_ntddk begin_wdm begin_nthal

        //
        // System service parameters for:  NtDeviceIoControlFile
        //
        // Note that the user's output buffer is stored in the UserBuffer field
        // and the user's input buffer is stored in the SystemBuffer field.
        //

        struct {
            ULONG OutputBufferLength;
            ULONG POINTER_ALIGNMENT InputBufferLength;
            ULONG POINTER_ALIGNMENT IoControlCode;
            PVOID Type3InputBuffer;
        } DeviceIoControl;

// end_wdm
        //
        // System service parameters for:  NtQuerySecurityObject
        //

        struct {
            SECURITY_INFORMATION SecurityInformation;
            ULONG POINTER_ALIGNMENT Length;
        } QuerySecurity;

        //
        // System service parameters for:  NtSetSecurityObject
        //

        struct {
            SECURITY_INFORMATION SecurityInformation;
            PSECURITY_DESCRIPTOR SecurityDescriptor;
        } SetSecurity;

// begin_wdm
        //
        // Non-system service parameters.
        //
        // Parameters for MountVolume
        //

        struct {
            PVPB Vpb;
            PDEVICE_OBJECT DeviceObject;
        } MountVolume;

        //
        // Parameters for VerifyVolume
        //

        struct {
            PVPB Vpb;
            PDEVICE_OBJECT DeviceObject;
        } VerifyVolume;

        //
        // Parameters for Scsi with internal device contorl.
        //

        struct {
            struct _SCSI_REQUEST_BLOCK *Srb;
        } Scsi;

// end_ntddk end_wdm end_nthal

        //
        // System service parameters for:  NtQueryQuotaInformationFile
        //

        struct {
            ULONG Length;
            PSID StartSid;
            PFILE_GET_QUOTA_INFORMATION SidList;
            ULONG SidListLength;
        } QueryQuota;

        //
        // System service parameters for:  NtSetQuotaInformationFile
        //

        struct {
            ULONG Length;
        } SetQuota;

// begin_ntddk begin_wdm begin_nthal

        //
        // Parameters for IRP_MN_QUERY_DEVICE_RELATIONS
        //

        struct {
            DEVICE_RELATION_TYPE Type;
        } QueryDeviceRelations;

        //
        // Parameters for IRP_MN_QUERY_INTERFACE
        //

        struct {
            CONST GUID *InterfaceType;
            USHORT Size;
            USHORT Version;
            PINTERFACE Interface;
            PVOID InterfaceSpecificData;
        } QueryInterface;

        //
        // Parameters for Cleanup
        //
        // No extra parameters supplied
        //

        //
        // WMI Irps
        //

        struct {
            ULONG_PTR ProviderId;
            PVOID DataPath;
            ULONG BufferSize;
            PVOID Buffer;
        } WMI;

        //
        // Others - driver-specific
        //

        struct {
            PVOID Argument1;
            PVOID Argument2;
            PVOID Argument3;
            PVOID Argument4;
        } Others;

    } Parameters;

    //
    // Save a pointer to this device driver's device object for this request
    // so it can be passed to the completion routine if needed.
    //

    PDEVICE_OBJECT DeviceObject;

    //
    // The following location contains a pointer to the file object for this
    //

    PFILE_OBJECT FileObject;

    //
    // The following routine is invoked depending on the flags in the above
    // flags field.
    //

    PIO_COMPLETION_ROUTINE CompletionRoutine;

    //
    // The following is used to store the address of the context parameter
    // that should be passed to the CompletionRoutine.
    //

    PVOID Context;

} IO_STACK_LOCATION, *PIO_STACK_LOCATION;
#if !defined(_ALPHA_) && !defined(_IA64_)
#include "poppack.h"
#endif

//
// Define the share access structure used by file systems to determine
// whether or not another accessor may open the file.
//

typedef struct _SHARE_ACCESS {
    ULONG OpenCount;
    ULONG Readers;
    ULONG Writers;
    ULONG Deleters;
    ULONG SharedRead;
    ULONG SharedWrite;
    ULONG SharedDelete;
} SHARE_ACCESS, *PSHARE_ACCESS;

// end_wdm

//
// The following structure is used by drivers that are initializing to
// determine the number of devices of a particular type that have already
// been initialized.  It is also used to track whether or not the AtDisk
// address range has already been claimed.  Finally, it is used by the
// NtQuerySystemInformation system service to return device type counts.
//

typedef struct _CONFIGURATION_INFORMATION {

    //
    // This field indicates the total number of disks in the system.  This
    // number should be used by the driver to determine the name of new
    // disks.  This field should be updated by the driver as it finds new
    // disks.
    //

    ULONG DiskCount;                // Count of hard disks thus far
    ULONG FloppyCount;              // Count of floppy disks thus far
    ULONG CdRomCount;               // Count of CD-ROM drives thus far
    ULONG TapeCount;                // Count of tape drives thus far
    ULONG ScsiPortCount;            // Count of SCSI port adapters thus far
    ULONG SerialCount;              // Count of serial devices thus far
    ULONG ParallelCount;            // Count of parallel devices thus far

    //
    // These next two fields indicate ownership of one of the two IO address
    // spaces that are used by WD1003-compatable disk controllers.
    //

    BOOLEAN AtDiskPrimaryAddressClaimed;    // 0x1F0 - 0x1FF
    BOOLEAN AtDiskSecondaryAddressClaimed;  // 0x170 - 0x17F

    //
    // Indicates the structure version, as anything value belong this will have been added.
    // Use the structure size as the version.
    //

    ULONG Version;

    //
    // Indicates the total number of medium changer devices in the system.
    // This field will be updated by the drivers as it determines that
    // new devices have been found and will be supported.
    //

    ULONG MediumChangerCount;

} CONFIGURATION_INFORMATION, *PCONFIGURATION_INFORMATION;

// end_ntddk end_nthal

//
// The following are global counters used by the I/O system to indicate the
// amount of I/O being performed in the system.  The first three counters
// are just that, counts of operations that have been requested, while the
// last three counters track the amount of data transferred for each type
// of I/O request.
//

extern KSPIN_LOCK IoStatisticsLock;
extern ULONG IoReadOperationCount;
extern ULONG IoWriteOperationCount;
extern ULONG IoOtherOperationCount;
extern LARGE_INTEGER IoReadTransferCount;
extern LARGE_INTEGER IoWriteTransferCount;
extern LARGE_INTEGER IoOtherTransferCount;

//
// It is difficult for cached file systems to properly charge quota
// for the storage that they allocate on behalf of user file handles,
// so the following amount of additional quota is charged against each
// handle as a "best guess" as to the amount of quota the file system
// will allocate on behalf of this handle.
//

//
// These numbers are totally arbitrary, and can be changed if it turns out
// that the file systems actually allocate more (or less) on behalf of
// their file objects.  The non-paged pool charge constant is added to the
// size of a FILE_OBJECT to get the actual charge amount.
//

#define IO_FILE_OBJECT_NON_PAGED_POOL_CHARGE    64
#define IO_FILE_OBJECT_PAGED_POOL_CHARGE        1024


// begin_ntddk begin_wdm begin_nthal
//
// Public I/O routine definitions
//

NTKERNELAPI
VOID
IoAcquireCancelSpinLock(
    OUT PKIRQL Irql
    );

// end_ntddk end_wdm end_nthal

NTKERNELAPI
VOID
IoAcquireVpbSpinLock(
    OUT PKIRQL Irql
    );


NTKERNELAPI
PVOID
IoAllocateErrorLogEntry(
    IN PVOID IoObject,
    IN UCHAR EntrySize
    );

NTKERNELAPI
PIRP
IoAllocateIrp(
    IN CCHAR StackSize,
    IN BOOLEAN ChargeQuota
    );

NTKERNELAPI
PMDL
IoAllocateMdl(
    IN PVOID VirtualAddress,
    IN ULONG Length,
    IN BOOLEAN SecondaryBuffer,
    IN BOOLEAN ChargeQuota,
    IN OUT PIRP Irp OPTIONAL
    );


NTKERNELAPI
NTSTATUS
IoAttachDevice(
    IN PDEVICE_OBJECT SourceDevice,
    IN PUNICODE_STRING TargetDevice,
    OUT PDEVICE_OBJECT *AttachedDevice
    );

// end_wdm

NTKERNELAPI
NTSTATUS
IoAttachDeviceByPointer(
    IN PDEVICE_OBJECT SourceDevice,
    IN PDEVICE_OBJECT TargetDevice
    );

// begin_wdm

NTKERNELAPI
PDEVICE_OBJECT
IoAttachDeviceToDeviceStack(
    IN PDEVICE_OBJECT SourceDevice,
    IN PDEVICE_OBJECT TargetDevice
    );

NTKERNELAPI
PIRP
IoBuildAsynchronousFsdRequest(
    IN ULONG MajorFunction,
    IN PDEVICE_OBJECT DeviceObject,
    IN OUT PVOID Buffer OPTIONAL,
    IN ULONG Length OPTIONAL,
    IN PLARGE_INTEGER StartingOffset OPTIONAL,
    IN PIO_STATUS_BLOCK IoStatusBlock OPTIONAL
    );

NTKERNELAPI
PIRP
IoBuildDeviceIoControlRequest(
    IN ULONG IoControlCode,
    IN PDEVICE_OBJECT DeviceObject,
    IN PVOID InputBuffer OPTIONAL,
    IN ULONG InputBufferLength,
    OUT PVOID OutputBuffer OPTIONAL,
    IN ULONG OutputBufferLength,
    IN BOOLEAN InternalDeviceIoControl,
    IN PKEVENT Event,
    OUT PIO_STATUS_BLOCK IoStatusBlock
    );

NTKERNELAPI
VOID
IoBuildPartialMdl(
    IN PMDL SourceMdl,
    IN OUT PMDL TargetMdl,
    IN PVOID VirtualAddress,
    IN ULONG Length
    );

typedef struct _BOOTDISK_INFORMATION {
    LONGLONG BootPartitionOffset;
    LONGLONG SystemPartitionOffset;
    ULONG BootDeviceSignature;
    ULONG SystemDeviceSignature;
} BOOTDISK_INFORMATION, *PBOOTDISK_INFORMATION;

NTKERNELAPI
NTSTATUS
IoGetBootDiskInformation(
    IN OUT PBOOTDISK_INFORMATION BootDiskInformation,
    IN ULONG Size
    );


NTKERNELAPI
PIRP
IoBuildSynchronousFsdRequest(
    IN ULONG MajorFunction,
    IN PDEVICE_OBJECT DeviceObject,
    IN OUT PVOID Buffer OPTIONAL,
    IN ULONG Length OPTIONAL,
    IN PLARGE_INTEGER StartingOffset OPTIONAL,
    IN PKEVENT Event,
    OUT PIO_STATUS_BLOCK IoStatusBlock
    );

NTKERNELAPI
NTSTATUS
FASTCALL
IofCallDriver(
    IN PDEVICE_OBJECT DeviceObject,
    IN OUT PIRP Irp
    );

#define IoCallDriver(a,b)   \
        IofCallDriver(a,b)

NTKERNELAPI
BOOLEAN
IoCancelIrp(
    IN PIRP Irp
    );


NTKERNELAPI
NTSTATUS
IoCheckDesiredAccess(
    IN OUT PACCESS_MASK DesiredAccess,
    IN ACCESS_MASK GrantedAccess
    );

NTKERNELAPI
NTSTATUS
IoCheckEaBufferValidity(
    IN PFILE_FULL_EA_INFORMATION EaBuffer,
    IN ULONG EaLength,
    OUT PULONG ErrorOffset
    );

NTKERNELAPI
NTSTATUS
IoCheckFunctionAccess(
    IN ACCESS_MASK GrantedAccess,
    IN UCHAR MajorFunction,
    IN UCHAR MinorFunction,
    IN ULONG IoControlCode,
    IN PVOID Arg1 OPTIONAL,
    IN PVOID Arg2 OPTIONAL
    );


NTKERNELAPI
NTSTATUS
IoCheckQuotaBufferValidity(
    IN PFILE_QUOTA_INFORMATION QuotaBuffer,
    IN ULONG QuotaLength,
    OUT PULONG ErrorOffset
    );

// end_ntsrv
// begin_ntddk begin_wdm begin_nthal

NTKERNELAPI
NTSTATUS
IoCheckShareAccess(
    IN ACCESS_MASK DesiredAccess,
    IN ULONG DesiredShareAccess,
    IN OUT PFILE_OBJECT FileObject,
    IN OUT PSHARE_ACCESS ShareAccess,
    IN BOOLEAN Update
    );

NTKERNELAPI
VOID
FASTCALL
IofCompleteRequest(
    IN PIRP Irp,
    IN CCHAR PriorityBoost
    );

#define IoCompleteRequest(a,b)  \
        IofCompleteRequest(a,b)


NTKERNELAPI
NTSTATUS
IoCreateDevice(
    IN PDRIVER_OBJECT DriverObject,
    IN ULONG DeviceExtensionSize,
    IN PUNICODE_STRING DeviceName OPTIONAL,
    IN DEVICE_TYPE DeviceType,
    IN ULONG DeviceCharacteristics,
    IN BOOLEAN Exclusive,
    OUT PDEVICE_OBJECT *DeviceObject
    );

#define WDM_MAJORVERSION        0x01
#define WDM_MINORVERSION        0x10

NTKERNELAPI
BOOLEAN
IoIsWdmVersionAvailable(
    IN UCHAR MajorVersion,
    IN UCHAR MinorVersion
    );

// end_nthal

NTKERNELAPI
NTSTATUS
IoCreateFile(
    OUT PHANDLE FileHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    IN PLARGE_INTEGER AllocationSize OPTIONAL,
    IN ULONG FileAttributes,
    IN ULONG ShareAccess,
    IN ULONG Disposition,
    IN ULONG CreateOptions,
    IN PVOID EaBuffer OPTIONAL,
    IN ULONG EaLength,
    IN CREATE_FILE_TYPE CreateFileType,
    IN PVOID ExtraCreateParameters OPTIONAL,
    IN ULONG Options
    );

// end_ntddk end_wdm

NTKERNELAPI
PFILE_OBJECT
IoCreateStreamFileObject(
    IN PFILE_OBJECT FileObject OPTIONAL,
    IN PDEVICE_OBJECT DeviceObject OPTIONAL
    );

NTKERNELAPI
PFILE_OBJECT
IoCreateStreamFileObjectLite(
    IN PFILE_OBJECT FileObject OPTIONAL,
    IN PDEVICE_OBJECT DeviceObject OPTIONAL
    );

// begin_nthal begin_ntddk begin_wdm

NTKERNELAPI
PKEVENT
IoCreateNotificationEvent(
    IN PUNICODE_STRING EventName,
    OUT PHANDLE EventHandle
    );

NTKERNELAPI
NTSTATUS
IoCreateSymbolicLink(
    IN PUNICODE_STRING SymbolicLinkName,
    IN PUNICODE_STRING DeviceName
    );

//  end_wdm

NTKERNELAPI
PKEVENT
IoCreateSynchronizationEvent(
    IN PUNICODE_STRING EventName,
    OUT PHANDLE EventHandle
    );

//  begin_wdm

NTKERNELAPI
NTSTATUS
IoCreateUnprotectedSymbolicLink(
    IN PUNICODE_STRING SymbolicLinkName,
    IN PUNICODE_STRING DeviceName
    );

//  end_wdm

//++
//
// VOID
// IoDeassignArcName(
//     IN PUNICODE_STRING ArcName
//     )
//
// Routine Description:
//
//     This routine is invoked by drivers to deassign an ARC name that they
//     created to a device.  This is generally only called if the driver is
//     deleting the device object, which means that the driver is probably
//     unloading.
//
// Arguments:
//
//     ArcName - Supplies the ARC name to be removed.
//
// Return Value:
//
//     None.
//
//--

#define IoDeassignArcName( ArcName ) (  \
    IoDeleteSymbolicLink( (ArcName) ) )


NTKERNELAPI
VOID
IoDeleteDevice(
    IN PDEVICE_OBJECT DeviceObject
    );

NTKERNELAPI
NTSTATUS
IoDeleteSymbolicLink(
    IN PUNICODE_STRING SymbolicLinkName
    );

NTKERNELAPI
VOID
IoDetachDevice(
    IN OUT PDEVICE_OBJECT TargetDevice
    );

NTKERNELAPI                                             
BOOLEAN                                                 
IoFastQueryNetworkAttributes(                           
    IN POBJECT_ATTRIBUTES ObjectAttributes,             
    IN ACCESS_MASK DesiredAccess,                       
    IN ULONG OpenOptions,                               
    OUT PIO_STATUS_BLOCK IoStatus,                      
    OUT PFILE_NETWORK_OPEN_INFORMATION Buffer           
    );                                                  

NTKERNELAPI
VOID
IoFreeIrp(
    IN PIRP Irp
    );

NTKERNELAPI
VOID
IoFreeMdl(
    IN PMDL Mdl
    );


NTKERNELAPI
PDEVICE_OBJECT
IoGetAttachedDevice(
    IN PDEVICE_OBJECT DeviceObject
    );

NTKERNELAPI                                 // ntddk wdm nthal
PDEVICE_OBJECT                              // ntddk wdm nthal
IoGetAttachedDeviceReference(               // ntddk wdm nthal
    IN PDEVICE_OBJECT DeviceObject          // ntddk wdm nthal
    );                                      // ntddk wdm nthal
                                            // ntddk wdm nthal
NTKERNELAPI
PDEVICE_OBJECT
IoGetBaseFileSystemDeviceObject(
    IN PFILE_OBJECT FileObject
    );

NTKERNELAPI                                 // ntddk nthal
PCONFIGURATION_INFORMATION                  // ntddk nthal
IoGetConfigurationInformation( VOID );      // ntddk nthal

// begin_ntddk begin_wdm begin_nthal

//++
//
// PIO_STACK_LOCATION
// IoGetCurrentIrpStackLocation(
//     IN PIRP Irp
//     )
//
// Routine Description:
//
//     This routine is invoked to return a pointer to the current stack location
//     in an I/O Request Packet (IRP).
//
// Arguments:
//
//     Irp - Pointer to the I/O Request Packet.
//
// Return Value:
//
//     The function value is a pointer to the current stack location in the
//     packet.
//
//--

#define IoGetCurrentIrpStackLocation( Irp ) ( (Irp)->Tail.Overlay.CurrentStackLocation )

// end_nthal end_wdm

NTKERNELAPI
PDEVICE_OBJECT
IoGetDeviceToVerify(
    IN PETHREAD Thread
    );

//  begin_wdm

NTKERNELAPI
PVOID
IoGetDriverObjectExtension(
    IN PDRIVER_OBJECT DriverObject,
    IN PVOID ClientIdentificationAddress
    );

NTKERNELAPI
PEPROCESS
IoGetCurrentProcess(
    VOID
    );

// begin_nthal

NTKERNELAPI
NTSTATUS
IoGetDeviceObjectPointer(
    IN PUNICODE_STRING ObjectName,
    IN ACCESS_MASK DesiredAccess,
    OUT PFILE_OBJECT *FileObject,
    OUT PDEVICE_OBJECT *DeviceObject
    );

NTKERNELAPI
struct _DMA_ADAPTER *
IoGetDmaAdapter(
    IN PDEVICE_OBJECT PhysicalDeviceObject,           OPTIONAL // required for PnP drivers
    IN struct _DEVICE_DESCRIPTION *DeviceDescription,
    IN OUT PULONG NumberOfMapRegisters
    );

//  end_wdm

NTKERNELAPI
PGENERIC_MAPPING
IoGetFileObjectGenericMapping(
    VOID
    );

// end_nthal


// begin_wdm

//++
//
// ULONG
// IoGetFunctionCodeFromCtlCode(
//     IN ULONG ControlCode
//     )
//
// Routine Description:
//
//     This routine extracts the function code from IOCTL and FSCTL function
//     control codes.
//     This routine should only be used by kernel mode code.
//
// Arguments:
//
//     ControlCode - A function control code (IOCTL or FSCTL) from which the
//         function code must be extracted.
//
// Return Value:
//
//     The extracted function code.
//
// Note:
//
//     The CTL_CODE macro, used to create IOCTL and FSCTL function control
//     codes, is defined in ntioapi.h
//
//--

#define IoGetFunctionCodeFromCtlCode( ControlCode ) (\
    ( ControlCode >> 2) & 0x00000FFF )

// begin_nthal end_wdm

NTKERNELAPI
PVOID
IoGetInitialStack(
    VOID
    );

NTKERNELAPI
VOID
IoGetStackLimits (
    OUT PULONG_PTR LowLimit,
    OUT PULONG_PTR HighLimit
    );


//
//  The following function is used to tell the caller how much stack is available
//

__inline
ULONG_PTR
IoGetRemainingStackSize (
    VOID
    )
{
    ULONG_PTR Top;
    ULONG_PTR Bottom;

    IoGetStackLimits( &Bottom, &Top );
    return((ULONG_PTR)(&Top) - Bottom );
}

//  begin_wdm

//++
//
// PIO_STACK_LOCATION
// IoGetNextIrpStackLocation(
//     IN PIRP Irp
//     )
//
// Routine Description:
//
//     This routine is invoked to return a pointer to the next stack location
//     in an I/O Request Packet (IRP).
//
// Arguments:
//
//     Irp - Pointer to the I/O Request Packet.
//
// Return Value:
//
//     The function value is a pointer to the next stack location in the packet.
//
//--

#define IoGetNextIrpStackLocation( Irp ) (\
    (Irp)->Tail.Overlay.CurrentStackLocation - 1 )

NTKERNELAPI
PDEVICE_OBJECT
IoGetRelatedDeviceObject(
    IN PFILE_OBJECT FileObject
    );

// end_ntddk end_wdm end_nthal

NTKERNELAPI
ULONG
IoGetRequestorProcessId(
    IN PIRP Irp
    );

NTKERNELAPI
PEPROCESS
IoGetRequestorProcess(
    IN PIRP Irp
    );

NTKERNELAPI
PIRP
IoGetTopLevelIrp(
    VOID
    );

// begin_ntddk begin_wdm begin_nthal

//++
//
// VOID
// IoInitializeDpcRequest(
//     IN PDEVICE_OBJECT DeviceObject,
//     IN PIO_DPC_ROUTINE DpcRoutine
//     )
//
// Routine Description:
//
//     This routine is invoked to initialize the DPC in a device object for a
//     device driver during its initialization routine.  The DPC is used later
//     when the driver interrupt service routine requests that a DPC routine
//     be queued for later execution.
//
// Arguments:
//
//     DeviceObject - Pointer to the device object that the request is for.
//
//     DpcRoutine - Address of the driver's DPC routine to be executed when
//         the DPC is dequeued for processing.
//
// Return Value:
//
//     None.
//
//--

#define IoInitializeDpcRequest( DeviceObject, DpcRoutine ) (\
    KeInitializeDpc( &(DeviceObject)->Dpc,                  \
                     (PKDEFERRED_ROUTINE) (DpcRoutine),     \
                     (DeviceObject) ) )

NTKERNELAPI
VOID
IoInitializeIrp(
    IN OUT PIRP Irp,
    IN USHORT PacketSize,
    IN CCHAR StackSize
    );

NTKERNELAPI
NTSTATUS
IoInitializeTimer(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIO_TIMER_ROUTINE TimerRoutine,
    IN PVOID Context
    );


NTKERNELAPI
VOID
IoReuseIrp(
    IN OUT PIRP Irp,
    IN NTSTATUS Iostatus
    );


NTKERNELAPI
VOID
IoCancelFileOpen(
    IN PDEVICE_OBJECT  DeviceObject,
    IN PFILE_OBJECT    FileObject
    );

//++
//
// BOOLEAN
// IoIsErrorUserInduced(
//     IN NTSTATUS Status
//     )
//
// Routine Description:
//
//     This routine is invoked to determine if an error was as a
//     result of user actions.  Typically these error are related
//     to removable media and will result in a pop-up.
//
// Arguments:
//
//     Status - The status value to check.
//
// Return Value:
//     The function value is TRUE if the user induced the error,
//     otherwise FALSE is returned.
//
//--
#define IoIsErrorUserInduced( Status ) ((BOOLEAN)  \
    (((Status) == STATUS_DEVICE_NOT_READY) ||      \
     ((Status) == STATUS_IO_TIMEOUT) ||            \
     ((Status) == STATUS_MEDIA_WRITE_PROTECTED) || \
     ((Status) == STATUS_NO_MEDIA_IN_DEVICE) ||    \
     ((Status) == STATUS_VERIFY_REQUIRED) ||       \
     ((Status) == STATUS_UNRECOGNIZED_MEDIA) ||    \
     ((Status) == STATUS_WRONG_VOLUME)))

// end_ntddk end_wdm end_nthal

//++
//
// BOOLEAN
// IoIsFileOpenedExclusively(
//     IN PFILE_OBJECT FileObject
//     )
//
// Routine Description:
//
//     This routine is invoked to determine whether the file open represented
//     by the specified file object is opened exclusively.
//
// Arguments:
//
//     FileObject - Pointer to the file object that represents the open instance
//         of the target file to be tested for exclusive access.
//
// Return Value:
//
//     The function value is TRUE if the open instance of the file is exclusive;
//     otherwise FALSE is returned.
//
//--

#define IoIsFileOpenedExclusively( FileObject ) (\
    (BOOLEAN) !((FileObject)->SharedRead || (FileObject)->SharedWrite || (FileObject)->SharedDelete))

NTKERNELAPI
BOOLEAN
IoIsOperationSynchronous(
    IN PIRP Irp
    );

NTKERNELAPI
BOOLEAN
IoIsSystemThread(
    IN PETHREAD Thread
    );

NTKERNELAPI
BOOLEAN
IoIsValidNameGraftingBuffer(
    IN PIRP Irp,
    IN PREPARSE_DATA_BUFFER ReparseBuffer
    );

// begin_ntddk begin_nthal

NTKERNELAPI
PIRP
IoMakeAssociatedIrp(
    IN PIRP Irp,
    IN CCHAR StackSize
    );

//  begin_wdm

//++
//
// VOID
// IoMarkIrpPending(
//     IN OUT PIRP Irp
//     )
//
// Routine Description:
//
//     This routine marks the specified I/O Request Packet (IRP) to indicate
//     that an initial status of STATUS_PENDING was returned to the caller.
//     This is used so that I/O completion can determine whether or not to
//     fully complete the I/O operation requested by the packet.
//
// Arguments:
//
//     Irp - Pointer to the I/O Request Packet to be marked pending.
//
// Return Value:
//
//     None.
//
//--

#define IoMarkIrpPending( Irp ) ( \
    IoGetCurrentIrpStackLocation( (Irp) )->Control |= SL_PENDING_RETURNED )

NTKERNELAPI                                             
NTSTATUS                                                
IoPageRead(                                             
    IN PFILE_OBJECT FileObject,                         
    IN PMDL MemoryDescriptorList,                       
    IN PLARGE_INTEGER StartingOffset,                   
    IN PKEVENT Event,                                   
    OUT PIO_STATUS_BLOCK IoStatusBlock                  
    );                                                  

NTKERNELAPI
NTSTATUS
IoQueryFileInformation(
    IN PFILE_OBJECT FileObject,
    IN FILE_INFORMATION_CLASS FileInformationClass,
    IN ULONG Length,
    OUT PVOID FileInformation,
    OUT PULONG ReturnedLength
    );

NTKERNELAPI
NTSTATUS
IoQueryVolumeInformation(
    IN PFILE_OBJECT FileObject,
    IN FS_INFORMATION_CLASS FsInformationClass,
    IN ULONG Length,
    OUT PVOID FsInformation,
    OUT PULONG ReturnedLength
    );


NTKERNELAPI
VOID
IoRaiseHardError(
    IN PIRP Irp,
    IN PVPB Vpb OPTIONAL,
    IN PDEVICE_OBJECT RealDeviceObject
    );

NTKERNELAPI
BOOLEAN
IoRaiseInformationalHardError(
    IN NTSTATUS ErrorStatus,
    IN PUNICODE_STRING String OPTIONAL,
    IN PKTHREAD Thread OPTIONAL
    );

NTKERNELAPI
BOOLEAN
IoSetThreadHardErrorMode(
    IN BOOLEAN EnableHardErrors
    );

NTKERNELAPI
VOID
IoRegisterBootDriverReinitialization(
    IN PDRIVER_OBJECT DriverObject,
    IN PDRIVER_REINITIALIZE DriverReinitializationRoutine,
    IN PVOID Context
    );

NTKERNELAPI
VOID
IoRegisterDriverReinitialization(
    IN PDRIVER_OBJECT DriverObject,
    IN PDRIVER_REINITIALIZE DriverReinitializationRoutine,
    IN PVOID Context
    );

// end_ntddk end_nthal

NTKERNELAPI
VOID
IoRegisterFileSystem(
    IN OUT PDEVICE_OBJECT DeviceObject
    );

NTKERNELAPI
NTSTATUS
IoRegisterFsRegistrationChange(
    IN PDRIVER_OBJECT DriverObject,
    IN PDRIVER_FS_NOTIFICATION DriverNotificationRoutine
    );

// begin_ntddk begin_nthal

NTKERNELAPI
NTSTATUS
IoRegisterShutdownNotification(
    IN PDEVICE_OBJECT DeviceObject
    );

NTKERNELAPI
NTSTATUS
IoRegisterLastChanceShutdownNotification(
    IN PDEVICE_OBJECT DeviceObject
    );

// begin_wdm

NTKERNELAPI
VOID
IoReleaseCancelSpinLock(
    IN KIRQL Irql
    );

// end_ntddk end_nthal end_wdm

NTKERNELAPI
VOID
IoReleaseVpbSpinLock(
    IN KIRQL Irql
    );

// begin_ntddk begin_nthal

NTKERNELAPI
VOID
IoRemoveShareAccess(
    IN PFILE_OBJECT FileObject,
    IN OUT PSHARE_ACCESS ShareAccess
    );


NTKERNELAPI
NTSTATUS
IoReportResourceUsage(
    IN PUNICODE_STRING DriverClassName OPTIONAL,
    IN PDRIVER_OBJECT DriverObject,
    IN PCM_RESOURCE_LIST DriverList OPTIONAL,
    IN ULONG DriverListSize OPTIONAL,
    IN PDEVICE_OBJECT DeviceObject,
    IN PCM_RESOURCE_LIST DeviceList OPTIONAL,
    IN ULONG DeviceListSize OPTIONAL,
    IN BOOLEAN OverrideConflict,
    OUT PBOOLEAN ConflictDetected
    );

//  begin_wdm

//++
//
// VOID
// IoRequestDpc(
//     IN PDEVICE_OBJECT DeviceObject,
//     IN PIRP Irp,
//     IN PVOID Context
//     )
//
// Routine Description:
//
//     This routine is invoked by the device driver's interrupt service routine
//     to request that a DPC routine be queued for later execution at a lower
//     IRQL.
//
// Arguments:
//
//     DeviceObject - Device object for which the request is being processed.
//
//     Irp - Pointer to the current I/O Request Packet (IRP) for the specified
//         device.
//
//     Context - Provides a general context parameter to be passed to the
//         DPC routine.
//
// Return Value:
//
//     None.
//
//--

#define IoRequestDpc( DeviceObject, Irp, Context ) ( \
    KeInsertQueueDpc( &(DeviceObject)->Dpc, (Irp), (Context) ) )

//++
//
// PDRIVER_CANCEL
// IoSetCancelRoutine(
//     IN PIRP Irp,
//     IN PDRIVER_CANCEL CancelRoutine
//     )
//
// Routine Description:
//
//     This routine is invoked to set the address of a cancel routine which
//     is to be invoked when an I/O packet has been canceled.
//
// Arguments:
//
//     Irp - Pointer to the I/O Request Packet itself.
//
//     CancelRoutine - Address of the cancel routine that is to be invoked
//         if the IRP is cancelled.
//
// Return Value:
//
//     Previous value of CancelRoutine field in the IRP.
//
//--

#define IoSetCancelRoutine( Irp, NewCancelRoutine ) (  \
    (PDRIVER_CANCEL) InterlockedExchangePointer( (PVOID *) &(Irp)->CancelRoutine, (PVOID) (NewCancelRoutine) ) )

//++
//
// VOID
// IoSetCompletionRoutine(
//     IN PIRP Irp,
//     IN PIO_COMPLETION_ROUTINE CompletionRoutine,
//     IN PVOID Context,
//     IN BOOLEAN InvokeOnSuccess,
//     IN BOOLEAN InvokeOnError,
//     IN BOOLEAN InvokeOnCancel
//     )
//
// Routine Description:
//
//     This routine is invoked to set the address of a completion routine which
//     is to be invoked when an I/O packet has been completed by a lower-level
//     driver.
//
// Arguments:
//
//     Irp - Pointer to the I/O Request Packet itself.
//
//     CompletionRoutine - Address of the completion routine that is to be
//         invoked once the next level driver completes the packet.
//
//     Context - Specifies a context parameter to be passed to the completion
//         routine.
//
//     InvokeOnSuccess - Specifies that the completion routine is invoked when the
//         operation is successfully completed.
//
//     InvokeOnError - Specifies that the completion routine is invoked when the
//         operation completes with an error status.
//
//     InvokeOnCancel - Specifies that the completion routine is invoked when the
//         operation is being canceled.
//
// Return Value:
//
//     None.
//
//--

#define IoSetCompletionRoutine( Irp, Routine, CompletionContext, Success, Error, Cancel ) { \
    PIO_STACK_LOCATION irpSp;                                               \
    ASSERT( (Success) | (Error) | (Cancel) ? (Routine) != NULL : TRUE );    \
    irpSp = IoGetNextIrpStackLocation( (Irp) );                             \
    irpSp->CompletionRoutine = (Routine);                                   \
    irpSp->Context = (CompletionContext);                                   \
    irpSp->Control = 0;                                                     \
    if ((Success)) { irpSp->Control = SL_INVOKE_ON_SUCCESS; }               \
    if ((Error)) { irpSp->Control |= SL_INVOKE_ON_ERROR; }                  \
    if ((Cancel)) { irpSp->Control |= SL_INVOKE_ON_CANCEL; } }

// end_ntddk end_wdm end_nthal

NTKERNELAPI
VOID
IoSetDeviceToVerify(
    IN PETHREAD Thread,
    IN PDEVICE_OBJECT DeviceObject
    );

// begin_ntddk begin_nthal

NTKERNELAPI
VOID
IoSetHardErrorOrVerifyDevice(
    IN PIRP Irp,
    IN PDEVICE_OBJECT DeviceObject
    );

// end_ntddk end_nthal

NTKERNELAPI
NTSTATUS
IoSetInformation(
    IN PFILE_OBJECT FileObject,
    IN FILE_INFORMATION_CLASS FileInformationClass,
    IN ULONG Length,
    IN PVOID FileInformation
    );

NTKERNELAPI
BOOLEAN
IoIsFileOriginRemote(
    IN PFILE_OBJECT FileObject
    );

NTKERNELAPI
NTSTATUS
IoSetFileOrigin(
    IN PFILE_OBJECT FileObject,
    IN BOOLEAN Remote
    );

// begin_ntddk begin_wdm begin_nthal

//++
//
// VOID
// IoSetNextIrpStackLocation (
//     IN OUT PIRP Irp
//     )
//
// Routine Description:
//
//     This routine is invoked to set the current IRP stack location to
//     the next stack location, i.e. it "pushes" the stack.
//
// Arguments:
//
//     Irp - Pointer to the I/O Request Packet (IRP).
//
// Return Value:
//
//     None.
//
//--

#define IoSetNextIrpStackLocation( Irp ) {      \
    (Irp)->CurrentLocation--;                   \
    (Irp)->Tail.Overlay.CurrentStackLocation--; }

//++
//
// VOID
// IoCopyCurrentIrpStackLocationToNext(
//     IN PIRP Irp
//     )
//
// Routine Description:
//
//     This routine is invoked to copy the IRP stack arguments and file
//     pointer from the current IrpStackLocation to the next
//     in an I/O Request Packet (IRP).
//
//     If the caller wants to call IoCallDriver with a completion routine
//     but does not wish to change the arguments otherwise,
//     the caller first calls IoCopyCurrentIrpStackLocationToNext,
//     then IoSetCompletionRoutine, then IoCallDriver.
//
// Arguments:
//
//     Irp - Pointer to the I/O Request Packet.
//
// Return Value:
//
//     None.
//
//--

#define IoCopyCurrentIrpStackLocationToNext( Irp ) { \
    PIO_STACK_LOCATION irpSp; \
    PIO_STACK_LOCATION nextIrpSp; \
    irpSp = IoGetCurrentIrpStackLocation( (Irp) ); \
    nextIrpSp = IoGetNextIrpStackLocation( (Irp) ); \
    RtlCopyMemory( nextIrpSp, irpSp, FIELD_OFFSET(IO_STACK_LOCATION, CompletionRoutine)); \
    nextIrpSp->Control = 0; }

//++
//
// VOID
// IoSkipCurrentIrpStackLocation (
//     IN PIRP Irp
//     )
//
// Routine Description:
//
//     This routine is invoked to increment the current stack location of
//     a given IRP.
//
//     If the caller wishes to call the next driver in a stack, and does not
//     wish to change the arguments, nor does he wish to set a completion
//     routine, then the caller first calls IoSkipCurrentIrpStackLocation
//     and the calls IoCallDriver.
//
// Arguments:
//
//     Irp - Pointer to the I/O Request Packet.
//
// Return Value:
//
//     None
//
//--

#define IoSkipCurrentIrpStackLocation( Irp ) \
    (Irp)->CurrentLocation++; \
    (Irp)->Tail.Overlay.CurrentStackLocation++;


NTKERNELAPI
VOID
IoSetShareAccess(
    IN ACCESS_MASK DesiredAccess,
    IN ULONG DesiredShareAccess,
    IN OUT PFILE_OBJECT FileObject,
    OUT PSHARE_ACCESS ShareAccess
    );

// end_ntddk end_wdm end_nthal

NTKERNELAPI
VOID
IoSetTopLevelIrp(
    IN PIRP Irp
    );


//++
//
// USHORT
// IoSizeOfIrp(
//     IN CCHAR StackSize
//     )
//
// Routine Description:
//
//     Determines the size of an IRP given the number of stack locations
//     the IRP will have.
//
// Arguments:
//
//     StackSize - Number of stack locations for the IRP.
//
// Return Value:
//
//     Size in bytes of the IRP.
//
//--

#define IoSizeOfIrp( StackSize ) \
    ((USHORT) (sizeof( IRP ) + ((StackSize) * (sizeof( IO_STACK_LOCATION )))))


NTKERNELAPI
VOID
IoStartTimer(
    IN PDEVICE_OBJECT DeviceObject
    );

NTKERNELAPI
VOID
IoStopTimer(
    IN PDEVICE_OBJECT DeviceObject
    );

// end_ntddk end_wdm end_nthal

NTKERNELAPI
NTSTATUS
IoSynchronousPageWrite(
    IN PFILE_OBJECT FileObject,
    IN PMDL MemoryDescriptorList,
    IN PLARGE_INTEGER StartingOffset,
    IN PKEVENT Event,
    OUT PIO_STATUS_BLOCK IoStatusBlock
    );

NTKERNELAPI
PEPROCESS
IoThreadToProcess(
    IN PETHREAD Thread
    );

NTKERNELAPI
VOID
IoUnregisterFileSystem(
    IN OUT PDEVICE_OBJECT DeviceObject
    );

NTKERNELAPI
VOID
IoUnregisterFsRegistrationChange(
    IN PDRIVER_OBJECT DriverObject,
    IN PDRIVER_FS_NOTIFICATION DriverNotificationRoutine
    );

// begin_ntddk begin_wdm begin_nthal

NTKERNELAPI
VOID
IoUnregisterShutdownNotification(
    IN PDEVICE_OBJECT DeviceObject
    );

//  end_wdm

NTKERNELAPI
VOID
IoUpdateShareAccess(
    IN PFILE_OBJECT FileObject,
    IN OUT PSHARE_ACCESS ShareAccess
    );

// end_ntddk end_nthal

NTKERNELAPI
NTSTATUS
IoVerifyVolume(
    IN PDEVICE_OBJECT DeviceObject,
    IN BOOLEAN AllowRawMount
    );


NTKERNELAPI                                     // ntddk wdm nthal
VOID                                            // ntddk wdm nthal
IoWriteErrorLogEntry(                           // ntddk wdm nthal
    IN PVOID ElEntry                            // ntddk wdm nthal
    );                                          // ntddk wdm nthal


NTKERNELAPI
NTSTATUS
IoWMIRegistrationControl(
    IN PDEVICE_OBJECT DeviceObject,
    IN ULONG Action
);

//
// Action code for IoWMIRetgistrationControl api
//

#define WMIREG_ACTION_REGISTER      1
#define WMIREG_ACTION_DEREGISTER    2
#define WMIREG_ACTION_REREGISTER    3
#define WMIREG_ACTION_UPDATE_GUIDS  4
#define WMIREG_ACTION_BLOCK_IRPS    5

//
// Code passed in IRP_MN_REGINFO WMI irp
//

#define WMIREGISTER                 0
#define WMIUPDATE                   1

NTKERNELAPI
NTSTATUS
IoWMIAllocateInstanceIds(
    IN GUID *Guid,
    IN ULONG InstanceCount,
    OUT ULONG *FirstInstanceId
    );

NTKERNELAPI
NTSTATUS
IoWMISuggestInstanceName(
    IN PDEVICE_OBJECT PhysicalDeviceObject OPTIONAL,
    IN PUNICODE_STRING SymbolicLinkName OPTIONAL,
    IN BOOLEAN CombineNames,
    OUT PUNICODE_STRING SuggestedInstanceName
    );

NTKERNELAPI
NTSTATUS
IoWMIWriteEvent(
    IN PVOID WnodeEventItem
    );

#if defined(_WIN64)
ULONG IoWMIDeviceObjectToProviderId(
    PDEVICE_OBJECT DeviceObject
    );
#else
#define IoWMIDeviceObjectToProviderId(DeviceObject) ((ULONG)(DeviceObject))
#endif

#if defined(_ALPHA_) || defined(_IA64_)    
                                                                                
NTHALAPI                                        
ULONG                                           
HalGetDmaAlignmentRequirement (                 
    VOID                                        
    );                                          
                                                
#endif                                          
                                                
#if defined(_M_IX86)                            
                                                
#define HalGetDmaAlignmentRequirement() 1L      
#endif                                          
                                                
NTHALAPI                                        
VOID                                            
KeFlushWriteBuffer (                            
    VOID                                        
    );                                          
                                                
//
// Performance counter function.
//

NTHALAPI
LARGE_INTEGER
KeQueryPerformanceCounter (
   IN PLARGE_INTEGER PerformanceFrequency OPTIONAL
   );

// begin_ntndis
//
// Stall processor execution function.
//

NTHALAPI
VOID
KeStallExecutionProcessor (
    IN ULONG MicroSeconds
    );

//
//  Indicates the system may do I/O to physical addresses above 4 GB.
//

extern PBOOLEAN Mm64BitPhysicalAddress;


//
// Define maximum disk transfer size to be used by MM and Cache Manager,
// so that packet-oriented disk drivers can optimize their packet allocation
// to this size.
//

#define MM_MAXIMUM_DISK_IO_SIZE          (0x10000)

//++
//
// ULONG_PTR
// ROUND_TO_PAGES (
//     IN ULONG_PTR Size
//     )
//
// Routine Description:
//
//     The ROUND_TO_PAGES macro takes a size in bytes and rounds it up to a
//     multiple of the page size.
//
//     NOTE: This macro fails for values 0xFFFFFFFF - (PAGE_SIZE - 1).
//
// Arguments:
//
//     Size - Size in bytes to round up to a page multiple.
//
// Return Value:
//
//     Returns the size rounded up to a multiple of the page size.
//
//--

#define ROUND_TO_PAGES(Size)  (((ULONG_PTR)(Size) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1))

//++
//
// ULONG
// BYTES_TO_PAGES (
//     IN ULONG Size
//     )
//
// Routine Description:
//
//     The BYTES_TO_PAGES macro takes the size in bytes and calculates the
//     number of pages required to contain the bytes.
//
// Arguments:
//
//     Size - Size in bytes.
//
// Return Value:
//
//     Returns the number of pages required to contain the specified size.
//
//--

#define BYTES_TO_PAGES(Size)  ((ULONG)((ULONG_PTR)(Size) >> PAGE_SHIFT) + \
                               (((ULONG)(Size) & (PAGE_SIZE - 1)) != 0))

//++
//
// ULONG
// BYTE_OFFSET (
//     IN PVOID Va
//     )
//
// Routine Description:
//
//     The BYTE_OFFSET macro takes a virtual address and returns the byte offset
//     of that address within the page.
//
// Arguments:
//
//     Va - Virtual address.
//
// Return Value:
//
//     Returns the byte offset portion of the virtual address.
//
//--

#define BYTE_OFFSET(Va) ((ULONG)((LONG_PTR)(Va) & (PAGE_SIZE - 1)))

//++
//
// PVOID
// PAGE_ALIGN (
//     IN PVOID Va
//     )
//
// Routine Description:
//
//     The PAGE_ALIGN macro takes a virtual address and returns a page-aligned
//     virtual address for that page.
//
// Arguments:
//
//     Va - Virtual address.
//
// Return Value:
//
//     Returns the page aligned virtual address.
//
//--

#define PAGE_ALIGN(Va) ((PVOID)((ULONG_PTR)(Va) & ~(PAGE_SIZE - 1)))

//++
//
// ULONG
// ADDRESS_AND_SIZE_TO_SPAN_PAGES (
//     IN PVOID Va,
//     IN ULONG Size
//     )
//
// Routine Description:
//
//     The ADDRESS_AND_SIZE_TO_SPAN_PAGES macro takes a virtual address and
//     size and returns the number of pages spanned by the size.
//
// Arguments:
//
//     Va - Virtual address.
//
//     Size - Size in bytes.
//
// Return Value:
//
//     Returns the number of pages spanned by the size.
//
//--

#define ADDRESS_AND_SIZE_TO_SPAN_PAGES(Va,Size) \
   (((((Size) - 1) >> PAGE_SHIFT) + \
   (((((ULONG)(Size-1)&(PAGE_SIZE-1)) + (PtrToUlong(Va) & (PAGE_SIZE -1)))) >> PAGE_SHIFT)) + 1L)

#define COMPUTE_PAGES_SPANNED(Va, Size) \
    ((ULONG)((((ULONG_PTR)(Va) & (PAGE_SIZE -1)) + (Size) + (PAGE_SIZE - 1)) >> PAGE_SHIFT))


//++
// PPFN_NUMBER
// MmGetMdlPfnArray (
//     IN PMDL Mdl
//     )
//
// Routine Description:
//
//     The MmGetMdlPfnArray routine returns the virtual address of the
//     first element of the array of physical page numbers associated with
//     the MDL.
//
// Arguments:
//
//     Mdl - Pointer to an MDL.
//
// Return Value:
//
//     Returns the virtual address of the first element of the array of
//     physical page numbers associated with the MDL.
//
//--

#define MmGetMdlPfnArray(Mdl) ((PPFN_NUMBER)(Mdl + 1))

//++
//
// PVOID
// MmGetMdlVirtualAddress (
//     IN PMDL Mdl
//     )
//
// Routine Description:
//
//     The MmGetMdlVirtualAddress returns the virtual address of the buffer
//     described by the Mdl.
//
// Arguments:
//
//     Mdl - Pointer to an MDL.
//
// Return Value:
//
//     Returns the virtual address of the buffer described by the Mdl
//
//--

#define MmGetMdlVirtualAddress(Mdl)                                     \
    ((PVOID) ((PCHAR) ((Mdl)->StartVa) + (Mdl)->ByteOffset))

//++
//
// ULONG
// MmGetMdlByteCount (
//     IN PMDL Mdl
//     )
//
// Routine Description:
//
//     The MmGetMdlByteCount returns the length in bytes of the buffer
//     described by the Mdl.
//
// Arguments:
//
//     Mdl - Pointer to an MDL.
//
// Return Value:
//
//     Returns the byte count of the buffer described by the Mdl
//
//--

#define MmGetMdlByteCount(Mdl)  ((Mdl)->ByteCount)

//++
//
// ULONG
// MmGetMdlByteOffset (
//     IN PMDL Mdl
//     )
//
// Routine Description:
//
//     The MmGetMdlByteOffset returns the byte offset within the page
//     of the buffer described by the Mdl.
//
// Arguments:
//
//     Mdl - Pointer to an MDL.
//
// Return Value:
//
//     Returns the byte offset within the page of the buffer described by the Mdl
//
//--

#define MmGetMdlByteOffset(Mdl)  ((Mdl)->ByteOffset)

//++
//
// PVOID
// MmGetMdlStartVa (
//     IN PMDL Mdl
//     )
//
// Routine Description:
//
//     The MmGetMdlBaseVa returns the virtual address of the buffer
//     described by the Mdl rounded down to the nearest page.
//
// Arguments:
//
//     Mdl - Pointer to an MDL.
//
// Return Value:
//
//     Returns the returns the starting virtual address of the MDL.
//
//
//--

#define MmGetMdlBaseVa(Mdl)  ((Mdl)->StartVa)

typedef enum _MM_SYSTEM_SIZE {
    MmSmallSystem,
    MmMediumSystem,
    MmLargeSystem
} MM_SYSTEMSIZE;

NTKERNELAPI
MM_SYSTEMSIZE
MmQuerySystemSize(
    VOID
    );

//  end_wdm

NTKERNELAPI
BOOLEAN
MmIsThisAnNtAsSystem(
    VOID
    );

//  begin_wdm

typedef enum _LOCK_OPERATION {
    IoReadAccess,
    IoWriteAccess,
    IoModifyAccess
} LOCK_OPERATION;


NTKERNELAPI
BOOLEAN
MmIsRecursiveIoFault(
    VOID
    );


BOOLEAN
MmForceSectionClosed (
    IN PSECTION_OBJECT_POINTERS SectionObjectPointer,
    IN BOOLEAN DelayClose
    );


typedef enum _MMFLUSH_TYPE {
    MmFlushForDelete,
    MmFlushForWrite
} MMFLUSH_TYPE;


BOOLEAN
MmFlushImageSection (
    IN PSECTION_OBJECT_POINTERS SectionObjectPointer,
    IN MMFLUSH_TYPE FlushType
    );

BOOLEAN
MmCanFileBeTruncated (
    IN PSECTION_OBJECT_POINTERS SectionPointer,
    IN PLARGE_INTEGER NewFileSize
    );


BOOLEAN                                     
MmSetAddressRangeModified (                 
    IN PVOID Address,                       
    IN SIZE_T Length                        
    );                                      

NTKERNELAPI
VOID
MmProbeAndLockProcessPages (
    IN OUT PMDL MemoryDescriptorList,
    IN PEPROCESS Process,
    IN KPROCESSOR_MODE AccessMode,
    IN LOCK_OPERATION Operation
    );


// begin_nthal
//
// I/O support routines.
//

NTKERNELAPI
VOID
MmProbeAndLockPages (
    IN OUT PMDL MemoryDescriptorList,
    IN KPROCESSOR_MODE AccessMode,
    IN LOCK_OPERATION Operation
    );


NTKERNELAPI
VOID
MmUnlockPages (
    IN PMDL MemoryDescriptorList
    );

NTKERNELAPI
VOID
MmBuildMdlForNonPagedPool (
    IN OUT PMDL MemoryDescriptorList
    );

NTKERNELAPI
PVOID
MmMapLockedPages (
    IN PMDL MemoryDescriptorList,
    IN KPROCESSOR_MODE AccessMode
    );

NTKERNELAPI
PVOID
MmGetSystemRoutineAddress (
    IN PUNICODE_STRING SystemRoutineName
    );

// end_wdm

NTKERNELAPI
NTSTATUS
MmMapUserAddressesToPage (
    IN PVOID BaseAddress,
    IN SIZE_T NumberOfBytes,
    IN PVOID PageAddress
    );

// begin_wdm

//
// _MM_PAGE_PRIORITY_ provides a method for the system to handle requests
// intelligently in low resource conditions.
//
// LowPagePriority should be used when it is acceptable to the driver for the
// mapping request to fail if the system is low on resources.  An example of
// this could be for a non-critical network connection where the driver can
// handle the failure case when system resources are close to being depleted.
//
// NormalPagePriority should be used when it is acceptable to the driver for the
// mapping request to fail if the system is very low on resources.  An example
// of this could be for a non-critical local filesystem request.
//
// HighPagePriority should be used when it is unacceptable to the driver for the
// mapping request to fail unless the system is completely out of resources.
// An example of this would be the paging file path in a driver.
//

typedef enum _MM_PAGE_PRIORITY {
    LowPagePriority,
    NormalPagePriority = 16,
    HighPagePriority = 32
} MM_PAGE_PRIORITY;

//
// Note: This function is not available in WDM 1.0
//
NTKERNELAPI
PVOID
MmMapLockedPagesSpecifyCache (
     IN PMDL MemoryDescriptorList,
     IN KPROCESSOR_MODE AccessMode,
     IN MEMORY_CACHING_TYPE CacheType,
     IN PVOID BaseAddress,
     IN ULONG BugCheckOnFailure,
     IN MM_PAGE_PRIORITY Priority
     );

NTKERNELAPI
VOID
MmUnmapLockedPages (
    IN PVOID BaseAddress,
    IN PMDL MemoryDescriptorList
    );

// end_wdm

typedef struct _PHYSICAL_MEMORY_RANGE {
    PHYSICAL_ADDRESS BaseAddress;
    LARGE_INTEGER NumberOfBytes;
} PHYSICAL_MEMORY_RANGE, *PPHYSICAL_MEMORY_RANGE;

NTKERNELAPI
NTSTATUS
MmAddPhysicalMemory (
    IN PPHYSICAL_ADDRESS StartAddress,
    IN OUT PLARGE_INTEGER NumberOfBytes
    );

NTKERNELAPI
NTSTATUS
MmRemovePhysicalMemory (
    IN PPHYSICAL_ADDRESS StartAddress,
    IN OUT PLARGE_INTEGER NumberOfBytes
    );

NTKERNELAPI
PPHYSICAL_MEMORY_RANGE
MmGetPhysicalMemoryRanges (
    VOID
    );

NTKERNELAPI
PMDL
MmAllocatePagesForMdl (
    IN PHYSICAL_ADDRESS LowAddress,
    IN PHYSICAL_ADDRESS HighAddress,
    IN PHYSICAL_ADDRESS SkipBytes,
    IN SIZE_T TotalBytes
    );

NTKERNELAPI
VOID
MmFreePagesFromMdl (
    IN PMDL MemoryDescriptorList
    );

// begin_wdm

NTKERNELAPI
PVOID
MmMapIoSpace (
    IN PHYSICAL_ADDRESS PhysicalAddress,
    IN SIZE_T NumberOfBytes,
    IN MEMORY_CACHING_TYPE CacheType
    );

NTKERNELAPI
VOID
MmUnmapIoSpace (
    IN PVOID BaseAddress,
    IN SIZE_T NumberOfBytes
    );


NTKERNELAPI
PVOID
MmMapVideoDisplay (
    IN PHYSICAL_ADDRESS PhysicalAddress,
    IN SIZE_T NumberOfBytes,
    IN MEMORY_CACHING_TYPE CacheType
     );

NTKERNELAPI
VOID
MmUnmapVideoDisplay (
     IN PVOID BaseAddress,
     IN SIZE_T NumberOfBytes
     );

NTKERNELAPI
PHYSICAL_ADDRESS
MmGetPhysicalAddress (
    IN PVOID BaseAddress
    );

NTKERNELAPI
PVOID
MmGetVirtualForPhysical (
    IN PHYSICAL_ADDRESS PhysicalAddress
    );

NTKERNELAPI
PVOID
MmAllocateContiguousMemory (
    IN SIZE_T NumberOfBytes,
    IN PHYSICAL_ADDRESS HighestAcceptableAddress
    );

NTKERNELAPI
PVOID
MmAllocateContiguousMemorySpecifyCache (
    IN SIZE_T NumberOfBytes,
    IN PHYSICAL_ADDRESS LowestAcceptableAddress,
    IN PHYSICAL_ADDRESS HighestAcceptableAddress,
    IN PHYSICAL_ADDRESS BoundaryAddressMultiple OPTIONAL,
    IN MEMORY_CACHING_TYPE CacheType
    );

NTKERNELAPI
VOID
MmFreeContiguousMemory (
    IN PVOID BaseAddress
    );

NTKERNELAPI
VOID
MmFreeContiguousMemorySpecifyCache (
    IN PVOID BaseAddress,
    IN SIZE_T NumberOfBytes,
    IN MEMORY_CACHING_TYPE CacheType
    );


NTKERNELAPI
PVOID
MmAllocateNonCachedMemory (
    IN SIZE_T NumberOfBytes
    );

NTKERNELAPI
VOID
MmFreeNonCachedMemory (
    IN PVOID BaseAddress,
    IN SIZE_T NumberOfBytes
    );

NTKERNELAPI
BOOLEAN
MmIsAddressValid (
    IN PVOID VirtualAddress
    );

NTKERNELAPI
BOOLEAN
MmIsNonPagedSystemAddressValid (
    IN PVOID VirtualAddress
    );

//  begin_wdm

NTKERNELAPI
SIZE_T
MmSizeOfMdl(
    IN PVOID Base,
    IN SIZE_T Length
    );

NTKERNELAPI
PMDL
MmCreateMdl(
    IN PMDL MemoryDescriptorList OPTIONAL,
    IN PVOID Base,
    IN SIZE_T Length
    );

NTKERNELAPI
PVOID
MmLockPagableDataSection(
    IN PVOID AddressWithinSection
    );

//  end_wdm

NTKERNELAPI
VOID
MmLockPagableSectionByHandle (
    IN PVOID ImageSectionHandle
    );

NTKERNELAPI
VOID
MmResetDriverPaging (
    IN PVOID AddressWithinSection
    );


NTKERNELAPI
PVOID
MmPageEntireDriver (
    IN PVOID AddressWithinSection
    );

NTKERNELAPI
VOID
MmUnlockPagableImageSection(
    IN PVOID ImageSectionHandle
    );

// end_wdm

NTKERNELAPI
HANDLE
MmSecureVirtualMemory (
    IN PVOID Address,
    IN SIZE_T Size,
    IN ULONG ProbeMode
    );

NTKERNELAPI
VOID
MmUnsecureVirtualMemory (
    IN HANDLE SecureHandle
    );

NTKERNELAPI
NTSTATUS
MmMapViewInSystemSpace (
    IN PVOID Section,
    OUT PVOID *MappedBase,
    IN PSIZE_T ViewSize
    );

NTKERNELAPI
NTSTATUS
MmUnmapViewInSystemSpace (
    IN PVOID MappedBase
    );

NTKERNELAPI
NTSTATUS
MmMapViewInSessionSpace (
    IN PVOID Section,
    OUT PVOID *MappedBase,
    IN OUT PSIZE_T ViewSize
    );

NTKERNELAPI
NTSTATUS
MmUnmapViewInSessionSpace (
    IN PVOID MappedBase
    );

// begin_wdm

//++
//
// VOID
// MmInitializeMdl (
//     IN PMDL MemoryDescriptorList,
//     IN PVOID BaseVa,
//     IN SIZE_T Length
//     )
//
// Routine Description:
//
//     This routine initializes the header of a Memory Descriptor List (MDL).
//
// Arguments:
//
//     MemoryDescriptorList - Pointer to the MDL to initialize.
//
//     BaseVa - Base virtual address mapped by the MDL.
//
//     Length - Length, in bytes, of the buffer mapped by the MDL.
//
// Return Value:
//
//     None.
//
//--

#define MmInitializeMdl(MemoryDescriptorList, BaseVa, Length) { \
    (MemoryDescriptorList)->Next = (PMDL) NULL; \
    (MemoryDescriptorList)->Size = (CSHORT)(sizeof(MDL) +  \
            (sizeof(PFN_NUMBER) * ADDRESS_AND_SIZE_TO_SPAN_PAGES((BaseVa), (Length)))); \
    (MemoryDescriptorList)->MdlFlags = 0; \
    (MemoryDescriptorList)->StartVa = (PVOID) PAGE_ALIGN((BaseVa)); \
    (MemoryDescriptorList)->ByteOffset = BYTE_OFFSET((BaseVa)); \
    (MemoryDescriptorList)->ByteCount = (ULONG)(Length); \
    }

//++
//
// PVOID
// MmGetSystemAddressForMdlSafe (
//     IN PMDL MDL,
//     IN MM_PAGE_PRIORITY PRIORITY
//     )
//
// Routine Description:
//
//     This routine returns the mapped address of an MDL. If the
//     Mdl is not already mapped or a system address, it is mapped.
//
// Arguments:
//
//     MemoryDescriptorList - Pointer to the MDL to map.
//
//     Priority - Supplies an indication as to how important it is that this
//                request succeed under low available PTE conditions.
//
// Return Value:
//
//     Returns the base address where the pages are mapped.  The base address
//     has the same offset as the virtual address in the MDL.
//
//     Unlike MmGetSystemAddressForMdl, Safe guarantees that it will always
//     return NULL on failure instead of bugchecking the system.
//
//     This macro is not usable by WDM 1.0 drivers as 1.0 did not include
//     MmMapLockedPagesSpecifyCache.  The solution for WDM 1.0 drivers is to
//     provide synchronization and set/reset the MDL_MAPPING_CAN_FAIL bit.
//
//--

#define MmGetSystemAddressForMdlSafe(MDL, PRIORITY)                    \
     (((MDL)->MdlFlags & (MDL_MAPPED_TO_SYSTEM_VA |                    \
                        MDL_SOURCE_IS_NONPAGED_POOL)) ?                \
                             ((MDL)->MappedSystemVa) :                 \
                             (MmMapLockedPagesSpecifyCache((MDL),      \
                                                           KernelMode, \
                                                           MmCached,   \
                                                           NULL,       \
                                                           FALSE,      \
                                                           (PRIORITY))))

//++
//
// PVOID
// MmGetSystemAddressForMdl (
//     IN PMDL MDL
//     )
//
// Routine Description:
//
//     This routine returns the mapped address of an MDL, if the
//     Mdl is not already mapped or a system address, it is mapped.
//
// Arguments:
//
//     MemoryDescriptorList - Pointer to the MDL to map.
//
// Return Value:
//
//     Returns the base address where the pages are mapped.  The base address
//     has the same offset as the virtual address in the MDL.
//
//--

//#define MmGetSystemAddressForMdl(MDL)
//     (((MDL)->MdlFlags & (MDL_MAPPED_TO_SYSTEM_VA)) ?
//                             ((MDL)->MappedSystemVa) :
//                ((((MDL)->MdlFlags & (MDL_SOURCE_IS_NONPAGED_POOL)) ?
//                      ((PVOID)((ULONG)(MDL)->StartVa | (MDL)->ByteOffset)) :
//                            (MmMapLockedPages((MDL),KernelMode)))))

#define MmGetSystemAddressForMdl(MDL)                                  \
     (((MDL)->MdlFlags & (MDL_MAPPED_TO_SYSTEM_VA |                    \
                        MDL_SOURCE_IS_NONPAGED_POOL)) ?                \
                             ((MDL)->MappedSystemVa) :                 \
                             (MmMapLockedPages((MDL),KernelMode)))

//++
//
// VOID
// MmPrepareMdlForReuse (
//     IN PMDL MDL
//     )
//
// Routine Description:
//
//     This routine will take all of the steps necessary to allow an MDL to be
//     re-used.
//
// Arguments:
//
//     MemoryDescriptorList - Pointer to the MDL that will be re-used.
//
// Return Value:
//
//     None.
//
//--

#define MmPrepareMdlForReuse(MDL)                                       \
    if (((MDL)->MdlFlags & MDL_PARTIAL_HAS_BEEN_MAPPED) != 0) {         \
        ASSERT(((MDL)->MdlFlags & MDL_PARTIAL) != 0);                   \
        MmUnmapLockedPages( (MDL)->MappedSystemVa, (MDL) );             \
    } else if (((MDL)->MdlFlags & MDL_PARTIAL) == 0) {                  \
        ASSERT(((MDL)->MdlFlags & MDL_MAPPED_TO_SYSTEM_VA) == 0);       \
    }

typedef NTSTATUS (*PMM_DLL_INITIALIZE)(
    IN PUNICODE_STRING RegistryPath
    );

typedef NTSTATUS (*PMM_DLL_UNLOAD)(
    VOID
    );


//
// Object Manager types
//

typedef struct _OBJECT_HANDLE_INFORMATION {
    ULONG HandleAttributes;
    ACCESS_MASK GrantedAccess;
} OBJECT_HANDLE_INFORMATION, *POBJECT_HANDLE_INFORMATION;

NTKERNELAPI                                                     
NTSTATUS                                                        
ObReferenceObjectByHandle(                                      
    IN HANDLE Handle,                                           
    IN ACCESS_MASK DesiredAccess,                               
    IN POBJECT_TYPE ObjectType OPTIONAL,                        
    IN KPROCESSOR_MODE AccessMode,                              
    OUT PVOID *Object,                                          
    OUT POBJECT_HANDLE_INFORMATION HandleInformation OPTIONAL   
    );                                                          
NTKERNELAPI                                                     
NTSTATUS                                                        
ObOpenObjectByPointer(                                          
    IN PVOID Object,                                            
    IN ULONG HandleAttributes,                                  
    IN PACCESS_STATE PassedAccessState OPTIONAL,                
    IN ACCESS_MASK DesiredAccess OPTIONAL,                      
    IN POBJECT_TYPE ObjectType OPTIONAL,                        
    IN KPROCESSOR_MODE AccessMode,                              
    OUT PHANDLE Handle                                          
    );                                                          
NTKERNELAPI                                                     
VOID                                                            
ObMakeTemporaryObject(                                          
    IN PVOID Object                                             
    );                                                          

#define ObDereferenceObject(a)                                     \
        ObfDereferenceObject(a)

#define ObReferenceObject(Object) ObfReferenceObject(Object)

NTKERNELAPI
VOID
FASTCALL
ObfReferenceObject(
    IN PVOID Object
    );


NTKERNELAPI
NTSTATUS
ObReferenceObjectByPointer(
    IN PVOID Object,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_TYPE ObjectType,
    IN KPROCESSOR_MODE AccessMode
    );

NTKERNELAPI
VOID
FASTCALL
ObfDereferenceObject(
    IN PVOID Object
    );

NTKERNELAPI                                                     
NTSTATUS                                                        
ObQueryNameString(                                              
    IN PVOID Object,                                            
    OUT POBJECT_NAME_INFORMATION ObjectNameInfo,                
    IN ULONG Length,                                            
    OUT PULONG ReturnLength                                     
    );                                                          
                                                                
NTKERNELAPI                                                     
ULONG                                                           
ObGetObjectPointerCount(                                        
    IN PVOID Object                                             
    );                                                          
NTSTATUS                                                        
ObQueryObjectAuditingByHandle(                                  
    IN HANDLE Handle,                                           
    OUT PBOOLEAN GenerateOnClose                                
    );                                                          
//
//  The following are globally used definitions for an LBN and a VBN
//

typedef ULONG LBN;
typedef LBN *PLBN;

typedef ULONG VBN;
typedef VBN *PVBN;


//
//  Every file system that uses the cache manager must have FsContext
//  of the file object point to a common fcb header structure.
//

typedef enum _FAST_IO_POSSIBLE {
    FastIoIsNotPossible = 0,
    FastIoIsPossible,
    FastIoIsQuestionable
} FAST_IO_POSSIBLE;


typedef struct _FSRTL_COMMON_FCB_HEADER {

    CSHORT NodeTypeCode;
    CSHORT NodeByteSize;

    //
    //  General flags available to FsRtl.
    //

    UCHAR Flags;

    //
    //  Indicates if fast I/O is possible or if we should be calling
    //  the check for fast I/O routine which is found via the driver
    //  object.
    //

    UCHAR IsFastIoPossible; // really type FAST_IO_POSSIBLE

    //
    //  Second Flags Field
    //

    UCHAR Flags2;

    //
    //  The following reserved field should always be 0
    //

    UCHAR Reserved;

    PERESOURCE Resource;

    PERESOURCE PagingIoResource;

    LARGE_INTEGER AllocationSize;
    LARGE_INTEGER FileSize;
    LARGE_INTEGER ValidDataLength;

    
    
} FSRTL_COMMON_FCB_HEADER;
typedef FSRTL_COMMON_FCB_HEADER *PFSRTL_COMMON_FCB_HEADER;

//
//  Define FsRtl common header flags
//

#define FSRTL_FLAG_FILE_MODIFIED        (0x01)
#define FSRTL_FLAG_FILE_LENGTH_CHANGED  (0x02)
#define FSRTL_FLAG_LIMIT_MODIFIED_PAGES (0x04)

//
//  Following flags determine how the modified page writer should
//  acquire the file.  These flags can't change while either resource
//  is acquired.  If neither of these flags is set then the
//  modified/mapped page writer will attempt to acquire the paging io
//  resource shared.
//

#define FSRTL_FLAG_ACQUIRE_MAIN_RSRC_EX (0x08)
#define FSRTL_FLAG_ACQUIRE_MAIN_RSRC_SH (0x10)

//
//  This flag will be set by the Cache Manager if a view is mapped
//  to a file.
//

#define FSRTL_FLAG_USER_MAPPED_FILE     (0x20)

//  This flag determines whether there currently is an Eof advance
//  in progress.  All such advances must be serialized.
//

#define FSRTL_FLAG_EOF_ADVANCE_ACTIVE   (0x80)

//
//  Flag values for Flags2
//
//  All unused bits are reserved and should NOT be modified.
//

//
//  If this flag is set, the Cache Manager will allow modified writing
//  in spite of the value of FsContext2.
//

#define FSRTL_FLAG2_DO_MODIFIED_WRITE        (0x01)

//
//  The following constants are used to block top level Irp processing when
//  (in either the fast io or cc case) file system resources have been
//  acquired above the file system, or we are in an Fsp thread.
//

#define FSRTL_FSP_TOP_LEVEL_IRP         0x01
#define FSRTL_CACHE_TOP_LEVEL_IRP       0x02
#define FSRTL_MOD_WRITE_TOP_LEVEL_IRP   0x03
#define FSRTL_FAST_IO_TOP_LEVEL_IRP     0x04
#define FSRTL_MAX_TOP_LEVEL_IRP_FLAG    0x04

//
//  Normal uncompressed Copy and Mdl Apis
//

NTKERNELAPI
BOOLEAN
FsRtlCopyRead (
    IN PFILE_OBJECT FileObject,
    IN PLARGE_INTEGER FileOffset,
    IN ULONG Length,
    IN BOOLEAN Wait,
    IN ULONG LockKey,
    OUT PVOID Buffer,
    OUT PIO_STATUS_BLOCK IoStatus,
    IN PDEVICE_OBJECT DeviceObject
    );

NTKERNELAPI
BOOLEAN
FsRtlCopyWrite (
    IN PFILE_OBJECT FileObject,
    IN PLARGE_INTEGER FileOffset,
    IN ULONG Length,
    IN BOOLEAN Wait,
    IN ULONG LockKey,
    IN PVOID Buffer,
    OUT PIO_STATUS_BLOCK IoStatus,
    IN PDEVICE_OBJECT DeviceObject
    );

//
//  The following two routines are called from NtCreateSection to avoid
//  deadlocks with the file systems.
//

NTKERNELAPI
VOID
FsRtlAcquireFileExclusive (
    IN PFILE_OBJECT FileObject
    );

NTKERNELAPI
VOID
FsRtlReleaseFile (
    IN PFILE_OBJECT FileObject
    );

//
//  These routines provide a simple interface for the common operations
//  of query/set file size.
//

NTSTATUS
FsRtlGetFileSize(
    IN PFILE_OBJECT FileObject,
    IN OUT PLARGE_INTEGER FileSize
    );

//
// Determine if there is a complete device failure on an error.
//

NTKERNELAPI
BOOLEAN
FsRtlIsTotalDeviceFailure(
    IN NTSTATUS Status
    );

// end_ntddk

//
//  Byte range file lock routines, implemented in FileLock.c
//
//  The file lock info record is used to return enumerated information
//  about a file lock
//

typedef struct _FILE_LOCK_INFO {

    //
    //  A description of the current locked range, and if the lock
    //  is exclusive or shared
    //

    LARGE_INTEGER StartingByte;
    LARGE_INTEGER Length;
    BOOLEAN ExclusiveLock;

    //
    //  The following fields describe the owner of the lock.
    //

    ULONG Key;
    PFILE_OBJECT FileObject;
    PVOID ProcessId;

    //
    //  The following field is used internally by FsRtl
    //

    LARGE_INTEGER EndingByte;

} FILE_LOCK_INFO;
typedef FILE_LOCK_INFO *PFILE_LOCK_INFO;

//
//  The following two procedure prototypes are used by the caller of the
//  file lock package to supply an alternate routine to call when
//  completing an IRP and when unlocking a byte range.  Note that the only
//  utility to us this interface is currently the redirector, all other file
//  system will probably let the IRP complete normally with IoCompleteRequest.
//  The user supplied routine returns any value other than success then the
//  lock package will remove any lock that we just inserted.
//

typedef NTSTATUS (*PCOMPLETE_LOCK_IRP_ROUTINE) (
    IN PVOID Context,
    IN PIRP Irp
    );

typedef VOID (*PUNLOCK_ROUTINE) (
    IN PVOID Context,
    IN PFILE_LOCK_INFO FileLockInfo
    );

//
//  A FILE_LOCK is an opaque structure but we need to declare the size of
//  it here so that users can allocate space for one.
//

typedef struct _FILE_LOCK {

    //
    //  The optional procedure to call to complete a request
    //

    PCOMPLETE_LOCK_IRP_ROUTINE CompleteLockIrpRoutine;

    //
    //  The optional procedure to call when unlocking a byte range
    //

    PUNLOCK_ROUTINE UnlockRoutine;

    //
    //  FastIoIsQuestionable is set to true when... bugbug
    //

    BOOLEAN FastIoIsQuestionable;
    BOOLEAN SpareC[3];

    //
    //  FsRtl lock information
    //

    PVOID   LockInformation;

    //
    //  Contains contination information for FsRtlGetNextFileLock
    //

    FILE_LOCK_INFO  LastReturnedLockInfo;
    PVOID           LastReturnedLock;

} FILE_LOCK;
typedef FILE_LOCK *PFILE_LOCK;

PFILE_LOCK
FsRtlAllocateFileLock (
    IN PCOMPLETE_LOCK_IRP_ROUTINE CompleteLockIrpRoutine OPTIONAL,
    IN PUNLOCK_ROUTINE UnlockRoutine OPTIONAL
    );

VOID
FsRtlFreeFileLock (
    IN PFILE_LOCK FileLock
    );

NTKERNELAPI
VOID
FsRtlInitializeFileLock (
    IN PFILE_LOCK FileLock,
    IN PCOMPLETE_LOCK_IRP_ROUTINE CompleteLockIrpRoutine OPTIONAL,
    IN PUNLOCK_ROUTINE UnlockRoutine OPTIONAL
    );

NTKERNELAPI
VOID
FsRtlUninitializeFileLock (
    IN PFILE_LOCK FileLock
    );

NTKERNELAPI
NTSTATUS
FsRtlProcessFileLock (
    IN PFILE_LOCK FileLock,
    IN PIRP Irp,
    IN PVOID Context OPTIONAL
    );

NTKERNELAPI
BOOLEAN
FsRtlCheckLockForReadAccess (
    IN PFILE_LOCK FileLock,
    IN PIRP Irp
    );

NTKERNELAPI
BOOLEAN
FsRtlCheckLockForWriteAccess (
    IN PFILE_LOCK FileLock,
    IN PIRP Irp
    );

NTKERNELAPI
BOOLEAN
FsRtlFastCheckLockForRead (
    IN PFILE_LOCK FileLock,
    IN PLARGE_INTEGER StartingByte,
    IN PLARGE_INTEGER Length,
    IN ULONG Key,
    IN PFILE_OBJECT FileObject,
    IN PVOID ProcessId
    );

NTKERNELAPI
BOOLEAN
FsRtlFastCheckLockForWrite (
    IN PFILE_LOCK FileLock,
    IN PLARGE_INTEGER StartingByte,
    IN PLARGE_INTEGER Length,
    IN ULONG Key,
    IN PVOID FileObject,
    IN PVOID ProcessId
    );

NTKERNELAPI
PFILE_LOCK_INFO
FsRtlGetNextFileLock (
    IN PFILE_LOCK FileLock,
    IN BOOLEAN Restart
    );

NTKERNELAPI
NTSTATUS
FsRtlFastUnlockSingle (
    IN PFILE_LOCK FileLock,
    IN PFILE_OBJECT FileObject,
    IN LARGE_INTEGER UNALIGNED *FileOffset,
    IN PLARGE_INTEGER Length,
    IN PEPROCESS ProcessId,
    IN ULONG Key,
    IN PVOID Context OPTIONAL,
    IN BOOLEAN AlreadySynchronized
    );

NTKERNELAPI
NTSTATUS
FsRtlFastUnlockAll (
    IN PFILE_LOCK FileLock,
    IN PFILE_OBJECT FileObject,
    IN PEPROCESS ProcessId,
    IN PVOID Context OPTIONAL
    );

NTKERNELAPI
NTSTATUS
FsRtlFastUnlockAllByKey (
    IN PFILE_LOCK FileLock,
    IN PFILE_OBJECT FileObject,
    IN PEPROCESS ProcessId,
    IN ULONG Key,
    IN PVOID Context OPTIONAL
    );

NTKERNELAPI
BOOLEAN
FsRtlPrivateLock (
    IN PFILE_LOCK FileLock,
    IN PFILE_OBJECT FileObject,
    IN PLARGE_INTEGER FileOffset,
    IN PLARGE_INTEGER Length,
    IN PEPROCESS ProcessId,
    IN ULONG Key,
    IN BOOLEAN FailImmediately,
    IN BOOLEAN ExclusiveLock,
    OUT PIO_STATUS_BLOCK Iosb,
    IN PIRP Irp,
    IN PVOID Context,
    IN BOOLEAN AlreadySynchronized
    );

//
//  BOOLEAN
//  FsRtlFastLock (
//      IN PFILE_LOCK FileLock,
//      IN PFILE_OBJECT FileObject,
//      IN PLARGE_INTEGER FileOffset,
//      IN PLARGE_INTEGER Length,
//      IN PEPROCESS ProcessId,
//      IN ULONG Key,
//      IN BOOLEAN FailImmediately,
//      IN BOOLEAN ExclusiveLock,
//      OUT PIO_STATUS_BLOCK Iosb,
//      IN PVOID Context OPTIONAL,
//      IN BOOLEAN AlreadySynchronized
//      );
//

#define FsRtlFastLock(A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11) ( \
    FsRtlPrivateLock( A1,   /* FileLock            */       \
                      A2,   /* FileObject          */       \
                      A3,   /* FileOffset          */       \
                      A4,   /* Length              */       \
                      A5,   /* ProcessId           */       \
                      A6,   /* Key                 */       \
                      A7,   /* FailImmediately     */       \
                      A8,   /* ExclusiveLock       */       \
                      A9,   /* Iosb                */       \
                      NULL, /* Irp                 */       \
                      A10,  /* Context             */       \
                      A11   /* AlreadySynchronized */ )     \
)

//
//  BOOLEAN
//  FsRtlAreThereCurrentFileLocks (
//      IN PFILE_LOCK FileLock
//      );
//

#define FsRtlAreThereCurrentFileLocks(FL) ( \
    ((FL)->FastIoIsQuestionable))



//
//  Filesystem property tunneling, implemented in tunnel.c
//

//
//  Tunnel cache structure
//

typedef struct {

    //
    //  Mutex for cache manipulation
    //

    FAST_MUTEX          Mutex;

    //
    //  Splay Tree of tunneled information keyed by
    //  DirKey ## Name
    //

    PRTL_SPLAY_LINKS    Cache;

    //
    //  Timer queue used to age entries out of the main cache
    //

    LIST_ENTRY          TimerQueue;

    //
    //  Keep track of the number of entries in the cache to prevent
    //  excessive use of memory
    //

    USHORT              NumEntries;

} TUNNEL, *PTUNNEL;

NTKERNELAPI
VOID
FsRtlInitializeTunnelCache (
    IN TUNNEL *Cache);

NTKERNELAPI
VOID
FsRtlAddToTunnelCache (
    IN TUNNEL *Cache,
    IN ULONGLONG DirectoryKey,
    IN UNICODE_STRING *ShortName,
    IN UNICODE_STRING *LongName,
    IN BOOLEAN KeyByShortName,
    IN ULONG DataLength,
    IN VOID *Data);

NTKERNELAPI
BOOLEAN
FsRtlFindInTunnelCache (
    IN TUNNEL *Cache,
    IN ULONGLONG DirectoryKey,
    IN UNICODE_STRING *Name,
    OUT UNICODE_STRING *ShortName,
    OUT UNICODE_STRING *LongName,
    IN OUT ULONG  *DataLength,
    OUT VOID *Data);


NTKERNELAPI
VOID
FsRtlDeleteKeyFromTunnelCache (
    IN TUNNEL *Cache,
    IN ULONGLONG DirectoryKey);


NTKERNELAPI
VOID
FsRtlDeleteTunnelCache (
    IN TUNNEL *Cache);


//
//  Dbcs name support routines, implemented in DbcsName.c
//

//
//  The following enumerated type is used to denote the result of name
//  comparisons
//

typedef enum _FSRTL_COMPARISON_RESULT {
    LessThan = -1,
    EqualTo = 0,
    GreaterThan = 1
} FSRTL_COMPARISON_RESULT;

#ifdef NLS_MB_CODE_PAGE_TAG
#undef NLS_MB_CODE_PAGE_TAG
#endif // NLS_MB_CODE_PAGE_TAG


#define LEGAL_ANSI_CHARACTER_ARRAY        (*FsRtlLegalAnsiCharacterArray)
#define NLS_MB_CODE_PAGE_TAG              (*NlsMbOemCodePageTag)
#define NLS_OEM_LEAD_BYTE_INFO            (*NlsOemLeadByteInfo)


extern PUCHAR LEGAL_ANSI_CHARACTER_ARRAY;
extern PUSHORT NLS_OEM_LEAD_BYTE_INFO;  // Lead byte info. for ACP

//
//  These following bit values are set in the FsRtlLegalDbcsCharacterArray
//

#define FSRTL_FAT_LEGAL         0x01
#define FSRTL_HPFS_LEGAL        0x02
#define FSRTL_NTFS_LEGAL        0x04
#define FSRTL_WILD_CHARACTER    0x08
#define FSRTL_OLE_LEGAL         0x10
#define FSRTL_NTFS_STREAM_LEGAL (FSRTL_NTFS_LEGAL | FSRTL_OLE_LEGAL)

//
//  The following macro is used to determine if an Ansi character is wild.
//

#define FsRtlIsAnsiCharacterWild(C) (                               \
    FsRtlTestAnsiCharacter((C), FALSE, FALSE, FSRTL_WILD_CHARACTER) \
)

//
//  The following macro is used to determine if an Ansi character is Fat legal.
//

#define FsRtlIsAnsiCharacterLegalFat(C,WILD_OK) (                 \
    FsRtlTestAnsiCharacter((C), TRUE, (WILD_OK), FSRTL_FAT_LEGAL) \
)

//
//  The following macro is used to determine if an Ansi character is Hpfs legal.
//

#define FsRtlIsAnsiCharacterLegalHpfs(C,WILD_OK) (                 \
    FsRtlTestAnsiCharacter((C), TRUE, (WILD_OK), FSRTL_HPFS_LEGAL) \
)

//
//  The following macro is used to determine if an Ansi character is Ntfs legal.
//

#define FsRtlIsAnsiCharacterLegalNtfs(C,WILD_OK) (                 \
    FsRtlTestAnsiCharacter((C), TRUE, (WILD_OK), FSRTL_NTFS_LEGAL) \
)

//
//  The following macro is used to determine if an Ansi character is
//  legal in an Ntfs stream name
//

#define FsRtlIsAnsiCharacterLegalNtfsStream(C,WILD_OK) (                    \
    FsRtlTestAnsiCharacter((C), TRUE, (WILD_OK), FSRTL_NTFS_STREAM_LEGAL)   \
)

//
//  The following macro is used to determine if an Ansi character is legal,
//  according to the caller's specification.
//

#define FsRtlIsAnsiCharacterLegal(C,FLAGS) (          \
    FsRtlTestAnsiCharacter((C), TRUE, FALSE, (FLAGS)) \
)

//
//  The following macro is used to test attributes of an Ansi character,
//  according to the caller's specified flags.
//

#define FsRtlTestAnsiCharacter(C, DEFAULT_RET, WILD_OK, FLAGS) (            \
        ((SCHAR)(C) < 0) ? DEFAULT_RET :                                    \
                           FlagOn( LEGAL_ANSI_CHARACTER_ARRAY[(C)],         \
                                   (FLAGS) |                                \
                                   ((WILD_OK) ? FSRTL_WILD_CHARACTER : 0) ) \
)


//
//  The following two macros use global data defined in ntos\rtl\nlsdata.c
//
//  BOOLEAN
//  FsRtlIsLeadDbcsCharacter (
//      IN UCHAR DbcsCharacter
//      );
//
//  /*++
//
//  Routine Description:
//
//      This routine takes the first bytes of a Dbcs character and
//      returns whether it is a lead byte in the system code page.
//
//  Arguments:
//
//      DbcsCharacter - Supplies the input character being examined
//
//  Return Value:
//
//      BOOLEAN - TRUE if the input character is a dbcs lead and
//              FALSE otherwise
//
//  --*/
//
//

#define FsRtlIsLeadDbcsCharacter(DBCS_CHAR) (                      \
    (BOOLEAN)((UCHAR)(DBCS_CHAR) < 0x80 ? FALSE :                  \
              (NLS_MB_CODE_PAGE_TAG &&                             \
               (NLS_OEM_LEAD_BYTE_INFO[(UCHAR)(DBCS_CHAR)] != 0))) \
)

NTKERNELAPI
VOID
FsRtlDissectDbcs (
    IN ANSI_STRING InputName,
    OUT PANSI_STRING FirstPart,
    OUT PANSI_STRING RemainingPart
    );

NTKERNELAPI
BOOLEAN
FsRtlDoesDbcsContainWildCards (
    IN PANSI_STRING Name
    );

NTKERNELAPI
BOOLEAN
FsRtlIsDbcsInExpression (
    IN PANSI_STRING Expression,
    IN PANSI_STRING Name
    );

NTKERNELAPI
BOOLEAN
FsRtlIsFatDbcsLegal (
    IN ANSI_STRING DbcsName,
    IN BOOLEAN WildCardsPermissible,
    IN BOOLEAN PathNamePermissible,
    IN BOOLEAN LeadingBackslashPermissible
    );

NTKERNELAPI
BOOLEAN
FsRtlIsHpfsDbcsLegal (
    IN ANSI_STRING DbcsName,
    IN BOOLEAN WildCardsPermissible,
    IN BOOLEAN PathNamePermissible,
    IN BOOLEAN LeadingBackslashPermissible
    );


//
//  Exception filter routines, implemented in Filter.c
//

NTKERNELAPI
NTSTATUS
FsRtlNormalizeNtstatus (
    IN NTSTATUS Exception,
    IN NTSTATUS GenericException
    );

NTKERNELAPI
BOOLEAN
FsRtlIsNtstatusExpected (
    IN NTSTATUS Exception
    );

//
//  The following procedures are used to allocate executive pool and raise
//  insufficient resource status if pool isn't currently available.
//

#define FsRtlAllocatePoolWithTag(PoolType, NumberOfBytes, Tag)                \
    ExAllocatePoolWithTag((POOL_TYPE)((PoolType) | POOL_RAISE_IF_ALLOCATION_FAILURE), \
                          NumberOfBytes,                                      \
                          Tag)


#define FsRtlAllocatePoolWithQuotaTag(PoolType, NumberOfBytes, Tag)           \
    ExAllocatePoolWithQuotaTag((POOL_TYPE)((PoolType) | POOL_RAISE_IF_ALLOCATION_FAILURE), \
                               NumberOfBytes,                                 \
                               Tag)

//
//  The following function allocates a resource from the FsRtl pool.
//

NTKERNELAPI
PERESOURCE
FsRtlAllocateResource (
    );


//
//  Large Integer Mapped Control Blocks routines, implemented in LargeMcb.c
//
//  Originally this structure was truely opaque and code outside largemcb was
//  never allowed to examine or alter the structures.  However, for performance
//  reasons we want to allow ntfs the ability to quickly truncate down the
//  mcb without the overhead of an actual call to largemcb.c.  So to do that we
//  need to export the struture.  This structure is not exact.  The Mapping field
//  is declared here as a pvoid but largemcb.c it is a pointer to mapping pairs.
//

typedef struct _LARGE_MCB {
    PFAST_MUTEX FastMutex;
    ULONG MaximumPairCount;
    ULONG PairCount;
    POOL_TYPE PoolType;
    PVOID Mapping;
} LARGE_MCB;
typedef LARGE_MCB *PLARGE_MCB;

NTKERNELAPI
VOID
FsRtlInitializeLargeMcb (
    IN PLARGE_MCB Mcb,
    IN POOL_TYPE PoolType
    );

NTKERNELAPI
VOID
FsRtlUninitializeLargeMcb (
    IN PLARGE_MCB Mcb
    );

NTKERNELAPI
VOID
FsRtlResetLargeMcb (
    IN PLARGE_MCB Mcb,
    IN BOOLEAN SelfSynchronized
    );

NTKERNELAPI
VOID
FsRtlTruncateLargeMcb (
    IN PLARGE_MCB Mcb,
    IN LONGLONG Vbn
    );

NTKERNELAPI
BOOLEAN
FsRtlAddLargeMcbEntry (
    IN PLARGE_MCB Mcb,
    IN LONGLONG Vbn,
    IN LONGLONG Lbn,
    IN LONGLONG SectorCount
    );

NTKERNELAPI
VOID
FsRtlRemoveLargeMcbEntry (
    IN PLARGE_MCB Mcb,
    IN LONGLONG Vbn,
    IN LONGLONG SectorCount
    );

NTKERNELAPI
BOOLEAN
FsRtlLookupLargeMcbEntry (
    IN PLARGE_MCB Mcb,
    IN LONGLONG Vbn,
    OUT PLONGLONG Lbn OPTIONAL,
    OUT PLONGLONG SectorCountFromLbn OPTIONAL,
    OUT PLONGLONG StartingLbn OPTIONAL,
    OUT PLONGLONG SectorCountFromStartingLbn OPTIONAL,
    OUT PULONG Index OPTIONAL
    );

NTKERNELAPI
BOOLEAN
FsRtlLookupLastLargeMcbEntry (
    IN PLARGE_MCB Mcb,
    OUT PLONGLONG Vbn,
    OUT PLONGLONG Lbn
    );

NTKERNELAPI
BOOLEAN
FsRtlLookupLastLargeMcbEntryAndIndex (
    IN PLARGE_MCB OpaqueMcb,
    OUT PLONGLONG LargeVbn,
    OUT PLONGLONG LargeLbn,
    OUT PULONG Index
    );

NTKERNELAPI
ULONG
FsRtlNumberOfRunsInLargeMcb (
    IN PLARGE_MCB Mcb
    );

NTKERNELAPI
BOOLEAN
FsRtlGetNextLargeMcbEntry (
    IN PLARGE_MCB Mcb,
    IN ULONG RunIndex,
    OUT PLONGLONG Vbn,
    OUT PLONGLONG Lbn,
    OUT PLONGLONG SectorCount
    );

NTKERNELAPI
BOOLEAN
FsRtlSplitLargeMcb (
    IN PLARGE_MCB Mcb,
    IN LONGLONG Vbn,
    IN LONGLONG Amount
    );


//
//  Mapped Control Blocks routines, implemented in Mcb.c
//
//  An MCB is an opaque structure but we need to declare the size of
//  it here so that users can allocate space for one.  Consequently the
//  size computation here must be updated by hand if the MCB changes.
//

typedef struct _MCB {
    LARGE_MCB DummyFieldThatSizesThisStructureCorrectly;
} MCB;
typedef MCB *PMCB;

NTKERNELAPI
VOID
FsRtlInitializeMcb (
    IN PMCB Mcb,
    IN POOL_TYPE PoolType
    );

NTKERNELAPI
VOID
FsRtlUninitializeMcb (
    IN PMCB Mcb
    );

NTKERNELAPI
VOID
FsRtlTruncateMcb (
    IN PMCB Mcb,
    IN VBN Vbn
    );

NTKERNELAPI
BOOLEAN
FsRtlAddMcbEntry (
    IN PMCB Mcb,
    IN VBN Vbn,
    IN LBN Lbn,
    IN ULONG SectorCount
    );

NTKERNELAPI
VOID
FsRtlRemoveMcbEntry (
    IN PMCB Mcb,
    IN VBN Vbn,
    IN ULONG SectorCount
    );

NTKERNELAPI
BOOLEAN
FsRtlLookupMcbEntry (
    IN PMCB Mcb,
    IN VBN Vbn,
    OUT PLBN Lbn,
    OUT PULONG SectorCount OPTIONAL,
    OUT PULONG Index
    );

NTKERNELAPI
BOOLEAN
FsRtlLookupLastMcbEntry (
    IN PMCB Mcb,
    OUT PVBN Vbn,
    OUT PLBN Lbn
    );

NTKERNELAPI
ULONG
FsRtlNumberOfRunsInMcb (
    IN PMCB Mcb
    );

NTKERNELAPI
BOOLEAN
FsRtlGetNextMcbEntry (
    IN PMCB Mcb,
    IN ULONG RunIndex,
    OUT PVBN Vbn,
    OUT PLBN Lbn,
    OUT PULONG SectorCount
    );

//
//  Fault Tollerance routines, implemented in FaultTol.c
//
//  The routines in this package implement rouines that help file
//  systems interact with the FT device drivers.
//

NTKERNELAPI
NTSTATUS
FsRtlBalanceReads (
    IN PDEVICE_OBJECT TargetDevice
    );


//
//  Oplock routines, implemented in Oplock.c
//
//  An OPLOCK is an opaque structure, we declare it as a PVOID and
//  allocate the actual memory only when needed.
//

typedef PVOID OPLOCK, *POPLOCK;

typedef
VOID
(*POPLOCK_WAIT_COMPLETE_ROUTINE) (
    IN PVOID Context,
    IN PIRP Irp
    );

typedef
VOID
(*POPLOCK_FS_PREPOST_IRP) (
    IN PVOID Context,
    IN PIRP Irp
    );

NTKERNELAPI
VOID
FsRtlInitializeOplock (
    IN OUT POPLOCK Oplock
    );

NTKERNELAPI
VOID
FsRtlUninitializeOplock (
    IN OUT POPLOCK Oplock
    );

NTKERNELAPI
NTSTATUS
FsRtlOplockFsctrl (
    IN POPLOCK Oplock,
    IN PIRP Irp,
    IN ULONG OpenCount
    );

NTKERNELAPI
NTSTATUS
FsRtlCheckOplock (
    IN POPLOCK Oplock,
    IN PIRP Irp,
    IN PVOID Context,
    IN POPLOCK_WAIT_COMPLETE_ROUTINE CompletionRoutine OPTIONAL,
    IN POPLOCK_FS_PREPOST_IRP PostIrpRoutine OPTIONAL
    );

NTKERNELAPI
BOOLEAN
FsRtlOplockIsFastIoPossible (
    IN POPLOCK Oplock
    );

NTKERNELAPI
BOOLEAN
FsRtlCurrentBatchOplock (
    IN POPLOCK Oplock
    );


//
//  Volume lock/unlock notification routines, implemented in PnP.c
//
//  These routines provide PnP volume lock notification support
//  for all filesystems.
//

#define FSRTL_VOLUME_DISMOUNT           1
#define FSRTL_VOLUME_DISMOUNT_FAILED    2
#define FSRTL_VOLUME_LOCK               3
#define FSRTL_VOLUME_LOCK_FAILED        4
#define FSRTL_VOLUME_UNLOCK             5
#define FSRTL_VOLUME_MOUNT              6

NTKERNELAPI
NTSTATUS
FsRtlNotifyVolumeEvent (
    IN PFILE_OBJECT FileObject,
    IN ULONG EventCode
    );

//
//  Notify Change routines, implemented in Notify.c
//
//  These routines provide Notify Change support for all filesystems.
//  Any of the 'Full' notify routines will support returning the
//  change information into the user's buffer.
//

typedef PVOID PNOTIFY_SYNC;

typedef
BOOLEAN (*PCHECK_FOR_TRAVERSE_ACCESS) (
            IN PVOID NotifyContext,
            IN PVOID TargetContext,
            IN PSECURITY_SUBJECT_CONTEXT SubjectContext
            );

NTKERNELAPI
VOID
FsRtlNotifyInitializeSync (
    IN PNOTIFY_SYNC *NotifySync
    );

NTKERNELAPI
VOID
FsRtlNotifyUninitializeSync (
    IN PNOTIFY_SYNC *NotifySync
    );

NTKERNELAPI
VOID
FsRtlNotifyFullChangeDirectory (
    IN PNOTIFY_SYNC NotifySync,
    IN PLIST_ENTRY NotifyList,
    IN PVOID FsContext,
    IN PSTRING FullDirectoryName,
    IN BOOLEAN WatchTree,
    IN BOOLEAN IgnoreBuffer,
    IN ULONG CompletionFilter,
    IN PIRP NotifyIrp,
    IN PCHECK_FOR_TRAVERSE_ACCESS TraverseCallback OPTIONAL,
    IN PSECURITY_SUBJECT_CONTEXT SubjectContext OPTIONAL
    );

NTKERNELAPI
VOID
FsRtlNotifyFullReportChange (
    IN PNOTIFY_SYNC NotifySync,
    IN PLIST_ENTRY NotifyList,
    IN PSTRING FullTargetName,
    IN USHORT TargetNameOffset,
    IN PSTRING StreamName OPTIONAL,
    IN PSTRING NormalizedParentName OPTIONAL,
    IN ULONG FilterMatch,
    IN ULONG Action,
    IN PVOID TargetContext
    );

NTKERNELAPI
VOID
FsRtlNotifyCleanup (
    IN PNOTIFY_SYNC NotifySync,
    IN PLIST_ENTRY NotifyList,
    IN PVOID FsContext
    );


//
//  Unicode Name support routines, implemented in Name.c
//
//  The routines here are used to manipulate unicode names
//

//
//  The following macro is used to determine if a character is wild.
//

#define FsRtlIsUnicodeCharacterWild(C) (                                \
      (((C) >= 0x40) ? FALSE : FlagOn( LEGAL_ANSI_CHARACTER_ARRAY[(C)], \
                                       FSRTL_WILD_CHARACTER ) )         \
)

NTKERNELAPI
VOID
FsRtlDissectName (
    IN UNICODE_STRING Path,
    OUT PUNICODE_STRING FirstName,
    OUT PUNICODE_STRING RemainingName
    );

NTKERNELAPI
BOOLEAN
FsRtlDoesNameContainWildCards (
    IN PUNICODE_STRING Name
    );

NTKERNELAPI
BOOLEAN
FsRtlAreNamesEqual (
    PCUNICODE_STRING ConstantNameA,
    PCUNICODE_STRING ConstantNameB,
    IN BOOLEAN IgnoreCase,
    IN PCWCH UpcaseTable OPTIONAL
    );

NTKERNELAPI
BOOLEAN
FsRtlIsNameInExpression (
    IN PUNICODE_STRING Expression,
    IN PUNICODE_STRING Name,
    IN BOOLEAN IgnoreCase,
    IN PWCH UpcaseTable OPTIONAL
    );


//
//  Stack Overflow support routine, implemented in StackOvf.c
//

typedef
VOID
(*PFSRTL_STACK_OVERFLOW_ROUTINE) (
    IN PVOID Context,
    IN PKEVENT Event
    );

NTKERNELAPI
VOID
FsRtlPostStackOverflow (
    IN PVOID Context,
    IN PKEVENT Event,
    IN PFSRTL_STACK_OVERFLOW_ROUTINE StackOverflowRoutine
    );

NTKERNELAPI
VOID
FsRtlPostPagingFileStackOverflow (
    IN PVOID Context,
    IN PKEVENT Event,
    IN PFSRTL_STACK_OVERFLOW_ROUTINE StackOverflowRoutine
    );

//
// UNC Provider support
//

NTKERNELAPI
NTSTATUS
FsRtlRegisterUncProvider(
    IN OUT PHANDLE MupHandle,
    IN PUNICODE_STRING RedirectorDeviceName,
    IN BOOLEAN MailslotsSupported
    );

NTKERNELAPI
VOID
FsRtlDeregisterUncProvider(
    IN HANDLE Handle
    );


//++
//
//  VOID
//  FsRtlCompleteRequest (
//      IN PIRP Irp,
//      IN NTSTATUS Status
//      );
//
//  Routine Description:
//
//      This routine is used to complete an IRP with the indicated
//      status.  It does the necessary raise and lower of IRQL.
//
//  Arguments:
//
//      Irp - Supplies a pointer to the Irp to complete
//
//      Status - Supplies the completion status for the Irp
//
//  Return Value:
//
//      None.
//
//--

#define FsRtlCompleteRequest(IRP,STATUS) {         \
    (IRP)->IoStatus.Status = (STATUS);             \
    IoCompleteRequest( (IRP), IO_DISK_INCREMENT ); \
}


//++
//
//  VOID
//  FsRtlEnterFileSystem (
//      );
//
//  Routine Description:
//
//      This routine is used when entering a file system (e.g., through its
//      Fsd entry point).  It ensures that the file system cannot be suspended
//      while running and thus block other file I/O requests.  Upon exit
//      the file system must call FsRtlExitFileSystem.
//
//  Arguments:
//
//  Return Value:
//
//      None.
//
//--

#define FsRtlEnterFileSystem() { \
    KeEnterCriticalRegion();     \
}

//++
//
//  VOID
//  FsRtlExitFileSystem (
//      );
//
//  Routine Description:
//
//      This routine is used when exiting a file system (e.g., through its
//      Fsd entry point).
//
//  Arguments:
//
//  Return Value:
//
//      None.
//
//--

#define FsRtlExitFileSystem() { \
    KeLeaveCriticalRegion();    \
}

//
//  Define two constants describing the view size (and alignment)
//  that the Cache Manager uses to map files.
//

#define VACB_MAPPING_GRANULARITY         (0x40000)
#define VACB_OFFSET_SHIFT                (18)

//
// Public portion of BCB
//

typedef struct _PUBLIC_BCB {

    //
    // Type and size of this record
    //
    // NOTE: The first four fields must be the same as the BCB in cc.h.
    //

    CSHORT NodeTypeCode;
    CSHORT NodeByteSize;

    //
    // Description of range of file which is currently mapped.
    //

    ULONG MappedLength;
    LARGE_INTEGER MappedFileOffset;
} PUBLIC_BCB, *PPUBLIC_BCB;

//
//  File Sizes structure.
//

typedef struct _CC_FILE_SIZES {

    LARGE_INTEGER AllocationSize;
    LARGE_INTEGER FileSize;
    LARGE_INTEGER ValidDataLength;

} CC_FILE_SIZES, *PCC_FILE_SIZES;

//
// Define a Cache Manager callback structure.  These routines are required
// by the Lazy Writer, so that it can acquire resources in the right order
// to avoid deadlocks.  Note that otherwise you would have most FS requests
// acquiring FS resources first and caching structures second, while the
// Lazy Writer needs to acquire its own resources first, and then FS
// structures later as it calls the file system.
//

//
// First define the procedure pointer typedefs
//

//
// This routine is called by the Lazy Writer prior to doing a write,
// since this will require some file system resources associated with
// this cached file. The context parameter supplied is whatever the FS
// passed as the LazyWriteContext parameter when is called
// CcInitializeCacheMap.
//

typedef
BOOLEAN (*PACQUIRE_FOR_LAZY_WRITE) (
             IN PVOID Context,
             IN BOOLEAN Wait
             );

//
// This routine releases the Context acquired above.
//

typedef
VOID (*PRELEASE_FROM_LAZY_WRITE) (
             IN PVOID Context
             );

//
// This routine is called by the Lazy Writer prior to doing a readahead.
//

typedef
BOOLEAN (*PACQUIRE_FOR_READ_AHEAD) (
             IN PVOID Context,
             IN BOOLEAN Wait
             );

//
// This routine releases the Context acquired above.
//

typedef
VOID (*PRELEASE_FROM_READ_AHEAD) (
             IN PVOID Context
             );

typedef struct _CACHE_MANAGER_CALLBACKS {

    PACQUIRE_FOR_LAZY_WRITE AcquireForLazyWrite;
    PRELEASE_FROM_LAZY_WRITE ReleaseFromLazyWrite;
    PACQUIRE_FOR_READ_AHEAD AcquireForReadAhead;
    PRELEASE_FROM_READ_AHEAD ReleaseFromReadAhead;

} CACHE_MANAGER_CALLBACKS, *PCACHE_MANAGER_CALLBACKS;

//
//  This structure is passed into CcUninitializeCacheMap
//  if the caller wants to know when the cache map is deleted.
//

typedef struct _CACHE_UNINITIALIZE_EVENT {
    struct _CACHE_UNINITIALIZE_EVENT *Next;
    KEVENT Event;
} CACHE_UNINITIALIZE_EVENT, *PCACHE_UNINITIALIZE_EVENT;

//
// Macro to test whether a file is cached or not.
//

#define CcIsFileCached(FO) (                                                         \
    ((FO)->SectionObjectPointer != NULL) &&                                          \
    (((PSECTION_OBJECT_POINTERS)(FO)->SectionObjectPointer)->SharedCacheMap != NULL) \
)

//
// The following routines are intended for use by File Systems Only.
//

NTKERNELAPI
VOID
CcInitializeCacheMap (
    IN PFILE_OBJECT FileObject,
    IN PCC_FILE_SIZES FileSizes,
    IN BOOLEAN PinAccess,
    IN PCACHE_MANAGER_CALLBACKS Callbacks,
    IN PVOID LazyWriteContext
    );

NTKERNELAPI
BOOLEAN
CcUninitializeCacheMap (
    IN PFILE_OBJECT FileObject,
    IN PLARGE_INTEGER TruncateSize OPTIONAL,
    IN PCACHE_UNINITIALIZE_EVENT UninitializeCompleteEvent OPTIONAL
    );

NTKERNELAPI
VOID
CcSetFileSizes (
    IN PFILE_OBJECT FileObject,
    IN PCC_FILE_SIZES FileSizes
    );

//
//  VOID
//  CcFastIoSetFileSizes (
//      IN PFILE_OBJECT FileObject,
//      IN PCC_FILE_SIZES FileSizes
//      );
//

#define CcGetFileSizePointer(FO) (                                     \
    ((PLARGE_INTEGER)((FO)->SectionObjectPointer->SharedCacheMap) + 1) \
)

NTKERNELAPI
BOOLEAN
CcPurgeCacheSection (
    IN PSECTION_OBJECT_POINTERS SectionObjectPointer,
    IN PLARGE_INTEGER FileOffset OPTIONAL,
    IN ULONG Length,
    IN BOOLEAN UninitializeCacheMaps
    );

NTKERNELAPI
VOID
CcSetDirtyPageThreshold (
    IN PFILE_OBJECT FileObject,
    IN ULONG DirtyPageThreshold
    );

NTKERNELAPI
VOID
CcFlushCache (
    IN PSECTION_OBJECT_POINTERS SectionObjectPointer,
    IN PLARGE_INTEGER FileOffset OPTIONAL,
    IN ULONG Length,
    OUT PIO_STATUS_BLOCK IoStatus OPTIONAL
    );

NTKERNELAPI
LARGE_INTEGER
CcGetFlushedValidData (
    IN PSECTION_OBJECT_POINTERS SectionObjectPointer,
    IN BOOLEAN BcbListHeld
    );

NTKERNELAPI
BOOLEAN
CcZeroData (
    IN PFILE_OBJECT FileObject,
    IN PLARGE_INTEGER StartOffset,
    IN PLARGE_INTEGER EndOffset,
    IN BOOLEAN Wait
    );

NTKERNELAPI
PVOID
CcRemapBcb (
    IN PVOID Bcb
    );

NTKERNELAPI
VOID
CcRepinBcb (
    IN PVOID Bcb
    );

NTKERNELAPI
VOID
CcUnpinRepinnedBcb (
    IN PVOID Bcb,
    IN BOOLEAN WriteThrough,
    OUT PIO_STATUS_BLOCK IoStatus
    );

NTKERNELAPI
PFILE_OBJECT
CcGetFileObjectFromSectionPtrs (
    IN PSECTION_OBJECT_POINTERS SectionObjectPointer
    );

NTKERNELAPI
PFILE_OBJECT
CcGetFileObjectFromBcb (
    IN PVOID Bcb
    );

//
// These routines are implemented to support write throttling.
//

//
//  BOOLEAN
//  CcCopyWriteWontFlush (
//      IN PFILE_OBJECT FileObject,
//      IN PLARGE_INTEGER FileOffset,
//      IN ULONG Length
//      );
//

#define CcCopyWriteWontFlush(FO,FOFF,LEN) ((LEN) <= 0X10000)

NTKERNELAPI
BOOLEAN
CcCanIWrite (
    IN PFILE_OBJECT FileObject,
    IN ULONG BytesToWrite,
    IN BOOLEAN Wait,
    IN BOOLEAN Retrying
    );

typedef
VOID (*PCC_POST_DEFERRED_WRITE) (
    IN PVOID Context1,
    IN PVOID Context2
    );

NTKERNELAPI
VOID
CcDeferWrite (
    IN PFILE_OBJECT FileObject,
    IN PCC_POST_DEFERRED_WRITE PostRoutine,
    IN PVOID Context1,
    IN PVOID Context2,
    IN ULONG BytesToWrite,
    IN BOOLEAN Retrying
    );

//
// The following routines provide a data copy interface to the cache, and
// are intended for use by File Servers and File Systems.
//

NTKERNELAPI
BOOLEAN
CcCopyRead (
    IN PFILE_OBJECT FileObject,
    IN PLARGE_INTEGER FileOffset,
    IN ULONG Length,
    IN BOOLEAN Wait,
    OUT PVOID Buffer,
    OUT PIO_STATUS_BLOCK IoStatus
    );

NTKERNELAPI
VOID
CcFastCopyRead (
    IN PFILE_OBJECT FileObject,
    IN ULONG FileOffset,
    IN ULONG Length,
    IN ULONG PageCount,
    OUT PVOID Buffer,
    OUT PIO_STATUS_BLOCK IoStatus
    );

NTKERNELAPI
BOOLEAN
CcCopyWrite (
    IN PFILE_OBJECT FileObject,
    IN PLARGE_INTEGER FileOffset,
    IN ULONG Length,
    IN BOOLEAN Wait,
    IN PVOID Buffer
    );

NTKERNELAPI
VOID
CcFastCopyWrite (
    IN PFILE_OBJECT FileObject,
    IN ULONG FileOffset,
    IN ULONG Length,
    IN PVOID Buffer
    );

//
//  The following routines provide an Mdl interface for transfers to and
//  from the cache, and are primarily intended for File Servers.
//
//  NOBODY SHOULD BE CALLING THESE MDL ROUTINES DIRECTLY, USE FSRTL AND
//  FASTIO INTERFACES.
//

NTKERNELAPI
VOID
CcMdlRead (
    IN PFILE_OBJECT FileObject,
    IN PLARGE_INTEGER FileOffset,
    IN ULONG Length,
    OUT PMDL *MdlChain,
    OUT PIO_STATUS_BLOCK IoStatus
    );

//
//  This routine is now a wrapper for FastIo if present or CcMdlReadComplete2
//

NTKERNELAPI
VOID
CcMdlReadComplete (
    IN PFILE_OBJECT FileObject,
    IN PMDL MdlChain
    );


NTKERNELAPI
VOID
CcPrepareMdlWrite (
    IN PFILE_OBJECT FileObject,
    IN PLARGE_INTEGER FileOffset,
    IN ULONG Length,
    OUT PMDL *MdlChain,
    OUT PIO_STATUS_BLOCK IoStatus
    );

//
//  This routine is now a wrapper for FastIo if present or CcMdlWriteComplete2
//

NTKERNELAPI
VOID
CcMdlWriteComplete (
    IN PFILE_OBJECT FileObject,
    IN PLARGE_INTEGER FileOffset,
    IN PMDL MdlChain
    );

//
// Common ReadAhead call for Copy Read and Mdl Read.
//
// ReadAhead should always be invoked by calling the CcReadAhead macro,
// which tests first to see if the read is large enough to warrant read
// ahead.  Measurements have shown that, calling the read ahead routine
// actually decreases performance for small reads, such as issued by
// many compilers and linkers.  Compilers simply want all of the include
// files to stay in memory after being read the first time.
//

#define CcReadAhead(FO,FOFF,LEN) {                       \
    if ((LEN) >= 256) {                                  \
        CcScheduleReadAhead((FO),(FOFF),(LEN));          \
    }                                                    \
}

NTKERNELAPI
VOID
CcScheduleReadAhead (
    IN PFILE_OBJECT FileObject,
    IN PLARGE_INTEGER FileOffset,
    IN ULONG Length
    );

//
//  The following routine allows a caller to wait for the next batch
//  of lazy writer work to complete.  In particular, this provides a
//  mechanism for a caller to be sure that all avaliable lazy closes
//  at the time of this call have issued.
//

NTSTATUS
CcWaitForCurrentLazyWriterActivity (
    );

//
// This routine changes the read ahead granularity for a file, which is
// PAGE_SIZE by default.
//

NTKERNELAPI
VOID
CcSetReadAheadGranularity (
    IN PFILE_OBJECT FileObject,
    IN ULONG Granularity
    );

//
// The following routines provide direct access data which is pinned in the
// cache, and is primarily intended for use by File Systems.  In particular,
// this mode of access is ideal for dealing with volume structures.
//

//
//  Flags for pinning
//

//
//  Synchronous Wait - normally specified.  This pattern may be specified as TRUE.
//

#define PIN_WAIT                         (1)

//
//  Acquire metadata Bcb exclusive (default is shared, Lazy Writer uses exclusive).
//
//  Must be set with PIN_WAIT.
//

#define PIN_EXCLUSIVE                    (2)

//
//  Acquire metadata Bcb but do not fault data in.  Default is to fault the data in.
//  This unusual flag is only used by Ntfs for cache coherency synchronization between
//  compressed and uncompressed streams for the same compressed file.
//
//  Must be set with PIN_WAIT.
//

#define PIN_NO_READ                      (4)

//
//  This option may be used to pin data only if the Bcb already exists.  If the Bcb
//  does not already exist - the pin is unsuccessful and no Bcb is returned.  This routine
//  provides a way to see if data is already pinned (and possibly dirty) in the cache,
//  without forcing a fault if the data is not there.
//

#define PIN_IF_BCB                       (8)

NTKERNELAPI
BOOLEAN
CcPinRead (
    IN PFILE_OBJECT FileObject,
    IN PLARGE_INTEGER FileOffset,
    IN ULONG Length,
    IN ULONG Flags,
    OUT PVOID *Bcb,
    OUT PVOID *Buffer
    );

NTKERNELAPI
BOOLEAN
CcMapData (
    IN PFILE_OBJECT FileObject,
    IN PLARGE_INTEGER FileOffset,
    IN ULONG Length,
    IN BOOLEAN Wait,
    OUT PVOID *Bcb,
    OUT PVOID *Buffer
    );

NTKERNELAPI
BOOLEAN
CcPinMappedData (
    IN PFILE_OBJECT FileObject,
    IN PLARGE_INTEGER FileOffset,
    IN ULONG Length,
    IN ULONG Flags,
    IN OUT PVOID *Bcb
    );

NTKERNELAPI
BOOLEAN
CcPreparePinWrite (
    IN PFILE_OBJECT FileObject,
    IN PLARGE_INTEGER FileOffset,
    IN ULONG Length,
    IN BOOLEAN Zero,
    IN ULONG Flags,
    OUT PVOID *Bcb,
    OUT PVOID *Buffer
    );

NTKERNELAPI
VOID
CcSetDirtyPinnedData (
    IN PVOID BcbVoid,
    IN PLARGE_INTEGER Lsn OPTIONAL
    );

NTKERNELAPI
VOID
CcUnpinData (
    IN PVOID Bcb
    );

NTKERNELAPI
VOID
CcSetBcbOwnerPointer (
    IN PVOID Bcb,
    IN PVOID OwnerPointer
    );

NTKERNELAPI
VOID
CcUnpinDataForThread (
    IN PVOID Bcb,
    IN ERESOURCE_THREAD ResourceThreadId
    );


NTKERNELAPI
VOID
CcSetAdditionalCacheAttributes (
    IN PFILE_OBJECT FileObject,
    IN BOOLEAN DisableReadAhead,
    IN BOOLEAN DisableWriteBehind
    );


NTKERNELAPI
BOOLEAN
CcIsThereDirtyData (
    IN PVPB Vpb
    );

#ifndef __SSPI_H__
#define __SSPI_H__
#define ISSP_LEVEL  32          
#define ISSP_MODE   0           

typedef WCHAR SEC_WCHAR;
typedef CHAR SEC_CHAR;


typedef LONG SECURITY_STATUS;

#define SEC_TEXT TEXT
#define SEC_FAR
#define SEC_ENTRY __stdcall


#ifndef __SECHANDLE_DEFINED__
typedef struct _SecHandle
{
    ULONG_PTR dwLower ;
    ULONG_PTR dwUpper ;
} SecHandle, * PSecHandle ;

#define __SECHANDLE_DEFINED__
#endif // __SECHANDLE_DEFINED__

#define SecInvalidateHandle( x )    \
            ((PSecHandle) x)->dwLower = (ULONG_PTR) -1 ; \
            ((PSecHandle) x)->dwUpper = (ULONG_PTR) -1 ;


typedef SecHandle CredHandle;
typedef PSecHandle PCredHandle;

typedef SecHandle CtxtHandle;
typedef PSecHandle PCtxtHandle;

typedef LARGE_INTEGER _SECURITY_INTEGER, SECURITY_INTEGER, *PSECURITY_INTEGER; 
typedef SECURITY_INTEGER TimeStamp;                 
typedef SECURITY_INTEGER SEC_FAR * PTimeStamp;      
typedef UNICODE_STRING SECURITY_STRING, *PSECURITY_STRING;  

//
// SecPkgInfo structure
//
//  Provides general information about a security provider
//

typedef struct _SecPkgInfoW
{
    unsigned long fCapabilities;        // Capability bitmask
    unsigned short wVersion;            // Version of driver
    unsigned short wRPCID;              // ID for RPC Runtime
    unsigned long cbMaxToken;           // Size of authentication token (max)
#ifdef MIDL_PASS
    [string]
#endif
    SEC_WCHAR SEC_FAR * Name;           // Text name

#ifdef MIDL_PASS
    [string]
#endif
    SEC_WCHAR SEC_FAR * Comment;        // Comment
} SecPkgInfoW, SEC_FAR * PSecPkgInfoW;

#  define SecPkgInfo SecPkgInfoW        
#  define PSecPkgInfo PSecPkgInfoW      

//
//  Security Package Capabilities
//
#define SECPKG_FLAG_INTEGRITY       0x00000001  // Supports integrity on messages
#define SECPKG_FLAG_PRIVACY         0x00000002  // Supports privacy (confidentiality)
#define SECPKG_FLAG_TOKEN_ONLY      0x00000004  // Only security token needed
#define SECPKG_FLAG_DATAGRAM        0x00000008  // Datagram RPC support
#define SECPKG_FLAG_CONNECTION      0x00000010  // Connection oriented RPC support
#define SECPKG_FLAG_MULTI_REQUIRED  0x00000020  // Full 3-leg required for re-auth.
#define SECPKG_FLAG_CLIENT_ONLY     0x00000040  // Server side functionality not available
#define SECPKG_FLAG_EXTENDED_ERROR  0x00000080  // Supports extended error msgs
#define SECPKG_FLAG_IMPERSONATION   0x00000100  // Supports impersonation
#define SECPKG_FLAG_ACCEPT_WIN32_NAME   0x00000200  // Accepts Win32 names
#define SECPKG_FLAG_STREAM          0x00000400  // Supports stream semantics
#define SECPKG_FLAG_NEGOTIABLE      0x00000800  // Can be used by the negotiate package
#define SECPKG_FLAG_GSS_COMPATIBLE  0x00001000  // GSS Compatibility Available
#define SECPKG_FLAG_LOGON           0x00002000  // Supports common LsaLogonUser
#define SECPKG_FLAG_ASCII_BUFFERS   0x00004000  // Token Buffers are in ASCII
#define SECPKG_FLAG_FRAGMENT        0x00008000  // Package can fragment to fit
#define SECPKG_FLAG_MUTUAL_AUTH     0x00010000  // Package can perform mutual authentication
#define SECPKG_FLAG_DELEGATION      0x00020000  // Package can delegate


#define SECPKG_ID_NONE      0xFFFF


//
// SecBuffer
//
//  Generic memory descriptors for buffers passed in to the security
//  API
//

typedef struct _SecBuffer {
    unsigned long cbBuffer;             // Size of the buffer, in bytes
    unsigned long BufferType;           // Type of the buffer (below)
    void SEC_FAR * pvBuffer;            // Pointer to the buffer
} SecBuffer, SEC_FAR * PSecBuffer;

typedef struct _SecBufferDesc {
    unsigned long ulVersion;            // Version number
    unsigned long cBuffers;             // Number of buffers
#ifdef MIDL_PASS
    [size_is(cBuffers)]
#endif
    PSecBuffer pBuffers;                // Pointer to array of buffers
} SecBufferDesc, SEC_FAR * PSecBufferDesc;

#define SECBUFFER_VERSION           0

#define SECBUFFER_EMPTY             0   // Undefined, replaced by provider
#define SECBUFFER_DATA              1   // Packet data
#define SECBUFFER_TOKEN             2   // Security token
#define SECBUFFER_PKG_PARAMS        3   // Package specific parameters
#define SECBUFFER_MISSING           4   // Missing Data indicator
#define SECBUFFER_EXTRA             5   // Extra data
#define SECBUFFER_STREAM_TRAILER    6   // Security Trailer
#define SECBUFFER_STREAM_HEADER     7   // Security Header
#define SECBUFFER_NEGOTIATION_INFO  8   // Hints from the negotiation pkg
#define SECBUFFER_PADDING           9   // non-data padding
#define SECBUFFER_STREAM            10  // whole encrypted message
#define SECBUFFER_MECHLIST          11  
#define SECBUFFER_MECHLIST_SIGNATURE 12 

#define SECBUFFER_ATTRMASK          0xF0000000
#define SECBUFFER_READONLY          0x80000000  // Buffer is read-only
#define SECBUFFER_RESERVED          0x40000000

typedef struct _SEC_NEGOTIATION_INFO {
    unsigned long       Size;           // Size of this structure
    unsigned long       NameLength;     // Length of name hint
    SEC_WCHAR SEC_FAR * Name;           // Name hint
    void SEC_FAR *      Reserved;       // Reserved
} SEC_NEGOTIATION_INFO, SEC_FAR * PSEC_NEGOTIATION_INFO ;

//
//  Data Representation Constant:
//
#define SECURITY_NATIVE_DREP        0x00000010
#define SECURITY_NETWORK_DREP       0x00000000

//
//  Credential Use Flags
//
#define SECPKG_CRED_INBOUND         0x00000001
#define SECPKG_CRED_OUTBOUND        0x00000002
#define SECPKG_CRED_BOTH            0x00000003
#define SECPKG_CRED_DEFAULT         0x00000004
#define SECPKG_CRED_RESERVED        0xF0000000

//
//  InitializeSecurityContext Requirement and return flags:
//

#define ISC_REQ_DELEGATE                0x00000001
#define ISC_REQ_MUTUAL_AUTH             0x00000002
#define ISC_REQ_REPLAY_DETECT           0x00000004
#define ISC_REQ_SEQUENCE_DETECT         0x00000008
#define ISC_REQ_CONFIDENTIALITY         0x00000010
#define ISC_REQ_USE_SESSION_KEY         0x00000020
#define ISC_REQ_PROMPT_FOR_CREDS        0x00000040
#define ISC_REQ_USE_SUPPLIED_CREDS      0x00000080
#define ISC_REQ_ALLOCATE_MEMORY         0x00000100
#define ISC_REQ_USE_DCE_STYLE           0x00000200
#define ISC_REQ_DATAGRAM                0x00000400
#define ISC_REQ_CONNECTION              0x00000800
#define ISC_REQ_CALL_LEVEL              0x00001000
#define ISC_REQ_FRAGMENT_SUPPLIED       0x00002000
#define ISC_REQ_EXTENDED_ERROR          0x00004000
#define ISC_REQ_STREAM                  0x00008000
#define ISC_REQ_INTEGRITY               0x00010000
#define ISC_REQ_IDENTIFY                0x00020000
#define ISC_REQ_NULL_SESSION            0x00040000
#define ISC_REQ_MANUAL_CRED_VALIDATION  0x00080000
#define ISC_REQ_RESERVED1               0x00100000
#define ISC_REQ_FRAGMENT_TO_FIT         0x00200000

#define ISC_RET_DELEGATE                0x00000001
#define ISC_RET_MUTUAL_AUTH             0x00000002
#define ISC_RET_REPLAY_DETECT           0x00000004
#define ISC_RET_SEQUENCE_DETECT         0x00000008
#define ISC_RET_CONFIDENTIALITY         0x00000010
#define ISC_RET_USE_SESSION_KEY         0x00000020
#define ISC_RET_USED_COLLECTED_CREDS    0x00000040
#define ISC_RET_USED_SUPPLIED_CREDS     0x00000080
#define ISC_RET_ALLOCATED_MEMORY        0x00000100
#define ISC_RET_USED_DCE_STYLE          0x00000200
#define ISC_RET_DATAGRAM                0x00000400
#define ISC_RET_CONNECTION              0x00000800
#define ISC_RET_INTERMEDIATE_RETURN     0x00001000
#define ISC_RET_CALL_LEVEL              0x00002000
#define ISC_RET_EXTENDED_ERROR          0x00004000
#define ISC_RET_STREAM                  0x00008000
#define ISC_RET_INTEGRITY               0x00010000
#define ISC_RET_IDENTIFY                0x00020000
#define ISC_RET_NULL_SESSION            0x00040000
#define ISC_RET_MANUAL_CRED_VALIDATION  0x00080000
#define ISC_RET_RESERVED1               0x00100000
#define ISC_RET_FRAGMENT_ONLY           0x00200000

#define ASC_REQ_DELEGATE                0x00000001
#define ASC_REQ_MUTUAL_AUTH             0x00000002
#define ASC_REQ_REPLAY_DETECT           0x00000004
#define ASC_REQ_SEQUENCE_DETECT         0x00000008
#define ASC_REQ_CONFIDENTIALITY         0x00000010
#define ASC_REQ_USE_SESSION_KEY         0x00000020
#define ASC_REQ_ALLOCATE_MEMORY         0x00000100
#define ASC_REQ_USE_DCE_STYLE           0x00000200
#define ASC_REQ_DATAGRAM                0x00000400
#define ASC_REQ_CONNECTION              0x00000800
#define ASC_REQ_CALL_LEVEL              0x00001000
#define ASC_REQ_EXTENDED_ERROR          0x00008000
#define ASC_REQ_STREAM                  0x00010000
#define ASC_REQ_INTEGRITY               0x00020000
#define ASC_REQ_LICENSING               0x00040000
#define ASC_REQ_IDENTIFY                0x00080000
#define ASC_REQ_ALLOW_NULL_SESSION      0x00100000
#define ASC_REQ_ALLOW_NON_USER_LOGONS   0x00200000
#define ASC_REQ_ALLOW_CONTEXT_REPLAY    0x00400000
#define ASC_REQ_FRAGMENT_TO_FIT         0x00800000
#define ASC_REQ_FRAGMENT_SUPPLIED       0x00002000

#define ASC_RET_DELEGATE                0x00000001
#define ASC_RET_MUTUAL_AUTH             0x00000002
#define ASC_RET_REPLAY_DETECT           0x00000004
#define ASC_RET_SEQUENCE_DETECT         0x00000008
#define ASC_RET_CONFIDENTIALITY         0x00000010
#define ASC_RET_USE_SESSION_KEY         0x00000020
#define ASC_RET_ALLOCATED_MEMORY        0x00000100
#define ASC_RET_USED_DCE_STYLE          0x00000200
#define ASC_RET_DATAGRAM                0x00000400
#define ASC_RET_CONNECTION              0x00000800
#define ASC_RET_CALL_LEVEL              0x00002000 // skipped 1000 to be like ISC_
#define ASC_RET_THIRD_LEG_FAILED        0x00004000
#define ASC_RET_EXTENDED_ERROR          0x00008000
#define ASC_RET_STREAM                  0x00010000
#define ASC_RET_INTEGRITY               0x00020000
#define ASC_RET_LICENSING               0x00040000
#define ASC_RET_IDENTIFY                0x00080000
#define ASC_RET_NULL_SESSION            0x00100000
#define ASC_RET_ALLOW_NON_USER_LOGONS   0x00200000
#define ASC_RET_ALLOW_CONTEXT_REPLAY    0x00400000
#define ASC_RET_FRAGMENT_ONLY           0x00800000

//
//  Security Credentials Attributes:
//

#define SECPKG_CRED_ATTR_NAMES 1

typedef struct _SecPkgCredentials_NamesW
{
    SEC_WCHAR SEC_FAR * sUserName;
} SecPkgCredentials_NamesW, SEC_FAR * PSecPkgCredentials_NamesW;

#  define SecPkgCredentials_Names SecPkgCredentials_NamesW      
#  define PSecPkgCredentials_Names PSecPkgCredentials_NamesW    

//
//  Security Context Attributes:
//

#define SECPKG_ATTR_SIZES           0
#define SECPKG_ATTR_NAMES           1
#define SECPKG_ATTR_LIFESPAN        2
#define SECPKG_ATTR_DCE_INFO        3
#define SECPKG_ATTR_STREAM_SIZES    4
#define SECPKG_ATTR_KEY_INFO        5
#define SECPKG_ATTR_AUTHORITY       6
#define SECPKG_ATTR_PROTO_INFO      7
#define SECPKG_ATTR_PASSWORD_EXPIRY 8
#define SECPKG_ATTR_SESSION_KEY     9
#define SECPKG_ATTR_PACKAGE_INFO    10
#define SECPKG_ATTR_USER_FLAGS      11
#define SECPKG_ATTR_NEGOTIATION_INFO 12
#define SECPKG_ATTR_NATIVE_NAMES    13
#define SECPKG_ATTR_FLAGS           14

typedef struct _SecPkgContext_Sizes
{
    unsigned long cbMaxToken;
    unsigned long cbMaxSignature;
    unsigned long cbBlockSize;
    unsigned long cbSecurityTrailer;
} SecPkgContext_Sizes, SEC_FAR * PSecPkgContext_Sizes;

typedef struct _SecPkgContext_StreamSizes
{
    unsigned long   cbHeader;
    unsigned long   cbTrailer;
    unsigned long   cbMaximumMessage;
    unsigned long   cBuffers;
    unsigned long   cbBlockSize;
} SecPkgContext_StreamSizes, * PSecPkgContext_StreamSizes;

typedef struct _SecPkgContext_NamesW
{
    SEC_WCHAR SEC_FAR * sUserName;
} SecPkgContext_NamesW, SEC_FAR * PSecPkgContext_NamesW;

#  define SecPkgContext_Names SecPkgContext_NamesW          
#  define PSecPkgContext_Names PSecPkgContext_NamesW        

typedef struct _SecPkgContext_Lifespan
{
    TimeStamp tsStart;
    TimeStamp tsExpiry;
} SecPkgContext_Lifespan, SEC_FAR * PSecPkgContext_Lifespan;

typedef struct _SecPkgContext_DceInfo
{
    unsigned long AuthzSvc;
    void SEC_FAR * pPac;
} SecPkgContext_DceInfo, SEC_FAR * PSecPkgContext_DceInfo;


typedef struct _SecPkgContext_KeyInfoW
{
    SEC_WCHAR SEC_FAR * sSignatureAlgorithmName;
    SEC_WCHAR SEC_FAR * sEncryptAlgorithmName;
    unsigned long       KeySize;
    unsigned long       SignatureAlgorithm;
    unsigned long       EncryptAlgorithm;
} SecPkgContext_KeyInfoW, SEC_FAR * PSecPkgContext_KeyInfoW;

#define SecPkgContext_KeyInfo   SecPkgContext_KeyInfoW      
#define PSecPkgContext_KeyInfo  PSecPkgContext_KeyInfoW     

typedef struct _SecPkgContext_AuthorityW
{
    SEC_WCHAR SEC_FAR * sAuthorityName;
} SecPkgContext_AuthorityW, * PSecPkgContext_AuthorityW;

#define SecPkgContext_Authority SecPkgContext_AuthorityW        
#define PSecPkgContext_Authority    PSecPkgContext_AuthorityW   

typedef struct _SecPkgContext_ProtoInfoW
{
    SEC_WCHAR SEC_FAR * sProtocolName;
    unsigned long       majorVersion;
    unsigned long       minorVersion;
} SecPkgContext_ProtoInfoW, SEC_FAR * PSecPkgContext_ProtoInfoW;

#define SecPkgContext_ProtoInfo   SecPkgContext_ProtoInfoW      
#define PSecPkgContext_ProtoInfo  PSecPkgContext_ProtoInfoW     

typedef struct _SecPkgContext_PasswordExpiry
{
    TimeStamp tsPasswordExpires;
} SecPkgContext_PasswordExpiry, SEC_FAR * PSecPkgContext_PasswordExpiry;

typedef struct _SecPkgContext_SessionKey
{
    unsigned long SessionKeyLength;
    unsigned char SEC_FAR * SessionKey;
} SecPkgContext_SessionKey, *PSecPkgContext_SessionKey;


typedef struct _SecPkgContext_PackageInfoW
{
    PSecPkgInfoW PackageInfo;
} SecPkgContext_PackageInfoW, SEC_FAR * PSecPkgContext_PackageInfoW;


typedef struct _SecPkgContext_UserFlags
{
    unsigned long UserFlags;
} SecPkgContext_UserFlags, SEC_FAR * PSecPkgContext_UserFlags;

typedef struct _SecPkgContext_Flags
{
    unsigned long Flags;
} SecPkgContext_Flags, SEC_FAR * PSecPkgContext_Flags;

#define SecPkgContext_PackageInfo   SecPkgContext_PackageInfoW      
#define PSecPkgContext_PackageInfo  PSecPkgContext_PackageInfoW     
typedef struct _SecPkgContext_NegotiationInfoW
{
    PSecPkgInfoW    PackageInfo ;
    unsigned long   NegotiationState ;
} SecPkgContext_NegotiationInfoW, SEC_FAR * PSecPkgContext_NegotiationInfoW ;

#  define SecPkgContext_NativeNames SecPkgContext_NativeNamesW          
#  define PSecPkgContext_NativeNames PSecPkgContext_NativeNamesW        

typedef void
(SEC_ENTRY SEC_FAR * SEC_GET_KEY_FN) (
    void SEC_FAR * Arg,                 // Argument passed in
    void SEC_FAR * Principal,           // Principal ID
    unsigned long KeyVer,               // Key Version
    void SEC_FAR * SEC_FAR * Key,       // Returned ptr to key
    SECURITY_STATUS SEC_FAR * Status    // returned status
    );

//
// Flags for ExportSecurityContext
//

#define SECPKG_CONTEXT_EXPORT_RESET_NEW         0x00000001      // New context is reset to initial state
#define SECPKG_CONTEXT_EXPORT_DELETE_OLD        0x00000002      // Old context is deleted during export


SECURITY_STATUS SEC_ENTRY
AcquireCredentialsHandleW(
#if ISSP_MODE == 0                      // For Kernel mode
    PSECURITY_STRING pPrincipal,
    PSECURITY_STRING pPackage,
#else
    SEC_WCHAR SEC_FAR * pszPrincipal,   // Name of principal
    SEC_WCHAR SEC_FAR * pszPackage,     // Name of package
#endif
    unsigned long fCredentialUse,       // Flags indicating use
    void SEC_FAR * pvLogonId,           // Pointer to logon ID
    void SEC_FAR * pAuthData,           // Package specific data
    SEC_GET_KEY_FN pGetKeyFn,           // Pointer to GetKey() func
    void SEC_FAR * pvGetKeyArgument,    // Value to pass to GetKey()
    PCredHandle phCredential,           // (out) Cred Handle
    PTimeStamp ptsExpiry                // (out) Lifetime (optional)
    );

typedef SECURITY_STATUS
(SEC_ENTRY * ACQUIRE_CREDENTIALS_HANDLE_FN_W)(
#if ISSP_MODE == 0
    PSECURITY_STRING,
    PSECURITY_STRING,
#else
    SEC_WCHAR SEC_FAR *,
    SEC_WCHAR SEC_FAR *,
#endif
    unsigned long,
    void SEC_FAR *,
    void SEC_FAR *,
    SEC_GET_KEY_FN,
    void SEC_FAR *,
    PCredHandle,
    PTimeStamp);

#  define AcquireCredentialsHandle AcquireCredentialsHandleW            
#  define ACQUIRE_CREDENTIALS_HANDLE_FN ACQUIRE_CREDENTIALS_HANDLE_FN_W 

SECURITY_STATUS SEC_ENTRY
FreeCredentialsHandle(
    PCredHandle phCredential            // Handle to free
    );

typedef SECURITY_STATUS
(SEC_ENTRY * FREE_CREDENTIALS_HANDLE_FN)(
    PCredHandle );

SECURITY_STATUS SEC_ENTRY
AddCredentialsW(
    PCredHandle hCredentials,
#if ISSP_MODE == 0                      // For Kernel mode
    PSECURITY_STRING pPrincipal,
    PSECURITY_STRING pPackage,
#else
    SEC_WCHAR SEC_FAR * pszPrincipal,   // Name of principal
    SEC_WCHAR SEC_FAR * pszPackage,     // Name of package
#endif
    unsigned long fCredentialUse,       // Flags indicating use
    void SEC_FAR * pAuthData,           // Package specific data
    SEC_GET_KEY_FN pGetKeyFn,           // Pointer to GetKey() func
    void SEC_FAR * pvGetKeyArgument,    // Value to pass to GetKey()
    PTimeStamp ptsExpiry                // (out) Lifetime (optional)
    );

typedef SECURITY_STATUS
(SEC_ENTRY * ADD_CREDENTIALS_FN_W)(
    PCredHandle,
#if ISSP_MODE == 0
    PSECURITY_STRING,
    PSECURITY_STRING,
#else
    SEC_WCHAR SEC_FAR *,
    SEC_WCHAR SEC_FAR *,
#endif
    unsigned long,
    void SEC_FAR *,
    SEC_GET_KEY_FN,
    void SEC_FAR *,
    PTimeStamp);

SECURITY_STATUS SEC_ENTRY
AddCredentialsA(
    PCredHandle hCredentials,
    SEC_CHAR SEC_FAR * pszPrincipal,   // Name of principal
    SEC_CHAR SEC_FAR * pszPackage,     // Name of package
    unsigned long fCredentialUse,       // Flags indicating use
    void SEC_FAR * pAuthData,           // Package specific data
    SEC_GET_KEY_FN pGetKeyFn,           // Pointer to GetKey() func
    void SEC_FAR * pvGetKeyArgument,    // Value to pass to GetKey()
    PTimeStamp ptsExpiry                // (out) Lifetime (optional)
    );

typedef SECURITY_STATUS
(SEC_ENTRY * ADD_CREDENTIALS_FN_A)(
    PCredHandle,
    SEC_CHAR SEC_FAR *,
    SEC_CHAR SEC_FAR *,
    unsigned long,
    void SEC_FAR *,
    SEC_GET_KEY_FN,
    void SEC_FAR *,
    PTimeStamp);

#ifdef UNICODE
#define AddCredentials  AddCredentialsW
#define ADD_CREDENTIALS_FN  ADD_CREDENTIALS_FN_W
#else
#define AddCredentials  AddCredentialsA
#define ADD_CREDENTIALS_FN ADD_CREDENTIALS_FN_A
#endif

#define SspiLogonUser SspiLogonUserW            

////////////////////////////////////////////////////////////////////////
///
/// Context Management Functions
///
////////////////////////////////////////////////////////////////////////

SECURITY_STATUS SEC_ENTRY
InitializeSecurityContextW(
    PCredHandle phCredential,               // Cred to base context
    PCtxtHandle phContext,                  // Existing context (OPT)
#if ISSP_MODE == 0
    PSECURITY_STRING pTargetName,
#else
    SEC_WCHAR SEC_FAR * pszTargetName,      // Name of target
#endif
    unsigned long fContextReq,              // Context Requirements
    unsigned long Reserved1,                // Reserved, MBZ
    unsigned long TargetDataRep,            // Data rep of target
    PSecBufferDesc pInput,                  // Input Buffers
    unsigned long Reserved2,                // Reserved, MBZ
    PCtxtHandle phNewContext,               // (out) New Context handle
    PSecBufferDesc pOutput,                 // (inout) Output Buffers
    unsigned long SEC_FAR * pfContextAttr,  // (out) Context attrs
    PTimeStamp ptsExpiry                    // (out) Life span (OPT)
    );

typedef SECURITY_STATUS
(SEC_ENTRY * INITIALIZE_SECURITY_CONTEXT_FN_W)(
    PCredHandle,
    PCtxtHandle,
#if ISSP_MODE == 0
    PSECURITY_STRING,
#else
    SEC_WCHAR SEC_FAR *,
#endif
    unsigned long,
    unsigned long,
    unsigned long,
    PSecBufferDesc,
    unsigned long,
    PCtxtHandle,
    PSecBufferDesc,
    unsigned long SEC_FAR *,
    PTimeStamp);

#  define InitializeSecurityContext InitializeSecurityContextW              
#  define INITIALIZE_SECURITY_CONTEXT_FN INITIALIZE_SECURITY_CONTEXT_FN_W   

SECURITY_STATUS SEC_ENTRY
AcceptSecurityContext(
    PCredHandle phCredential,               // Cred to base context
    PCtxtHandle phContext,                  // Existing context (OPT)
    PSecBufferDesc pInput,                  // Input buffer
    unsigned long fContextReq,              // Context Requirements
    unsigned long TargetDataRep,            // Target Data Rep
    PCtxtHandle phNewContext,               // (out) New context handle
    PSecBufferDesc pOutput,                 // (inout) Output buffers
    unsigned long SEC_FAR * pfContextAttr,  // (out) Context attributes
    PTimeStamp ptsExpiry                    // (out) Life span (OPT)
    );

typedef SECURITY_STATUS
(SEC_ENTRY * ACCEPT_SECURITY_CONTEXT_FN)(
    PCredHandle,
    PCtxtHandle,
    PSecBufferDesc,
    unsigned long,
    unsigned long,
    PCtxtHandle,
    PSecBufferDesc,
    unsigned long SEC_FAR *,
    PTimeStamp);



SECURITY_STATUS SEC_ENTRY
CompleteAuthToken(
    PCtxtHandle phContext,              // Context to complete
    PSecBufferDesc pToken               // Token to complete
    );

typedef SECURITY_STATUS
(SEC_ENTRY * COMPLETE_AUTH_TOKEN_FN)(
    PCtxtHandle,
    PSecBufferDesc);


SECURITY_STATUS SEC_ENTRY
ImpersonateSecurityContext(
    PCtxtHandle phContext               // Context to impersonate
    );

typedef SECURITY_STATUS
(SEC_ENTRY * IMPERSONATE_SECURITY_CONTEXT_FN)(
    PCtxtHandle);



SECURITY_STATUS SEC_ENTRY
RevertSecurityContext(
    PCtxtHandle phContext               // Context from which to re
    );

typedef SECURITY_STATUS
(SEC_ENTRY * REVERT_SECURITY_CONTEXT_FN)(
    PCtxtHandle);


SECURITY_STATUS SEC_ENTRY
QuerySecurityContextToken(
    PCtxtHandle phContext,
    void SEC_FAR * SEC_FAR * Token
    );

typedef SECURITY_STATUS
(SEC_ENTRY * QUERY_SECURITY_CONTEXT_TOKEN_FN)(
    PCtxtHandle, void SEC_FAR * SEC_FAR *);



SECURITY_STATUS SEC_ENTRY
DeleteSecurityContext(
    PCtxtHandle phContext               // Context to delete
    );

typedef SECURITY_STATUS
(SEC_ENTRY * DELETE_SECURITY_CONTEXT_FN)(
    PCtxtHandle);



SECURITY_STATUS SEC_ENTRY
ApplyControlToken(
    PCtxtHandle phContext,              // Context to modify
    PSecBufferDesc pInput               // Input token to apply
    );

typedef SECURITY_STATUS
(SEC_ENTRY * APPLY_CONTROL_TOKEN_FN)(
    PCtxtHandle, PSecBufferDesc);



SECURITY_STATUS SEC_ENTRY
QueryContextAttributesW(
    PCtxtHandle phContext,              // Context to query
    unsigned long ulAttribute,          // Attribute to query
    void SEC_FAR * pBuffer              // Buffer for attributes
    );

typedef SECURITY_STATUS
(SEC_ENTRY * QUERY_CONTEXT_ATTRIBUTES_FN_W)(
    PCtxtHandle,
    unsigned long,
    void SEC_FAR *);

#  define QueryContextAttributes QueryContextAttributesW            
#  define QUERY_CONTEXT_ATTRIBUTES_FN QUERY_CONTEXT_ATTRIBUTES_FN_W 

SECURITY_STATUS SEC_ENTRY
QueryCredentialsAttributesW(
    PCredHandle phCredential,              // Credential to query
    unsigned long ulAttribute,          // Attribute to query
    void SEC_FAR * pBuffer              // Buffer for attributes
    );

typedef SECURITY_STATUS
(SEC_ENTRY * QUERY_CREDENTIALS_ATTRIBUTES_FN_W)(
    PCredHandle,
    unsigned long,
    void SEC_FAR *);

#  define QueryCredentialsAttributes QueryCredentialsAttributesW            
#  define QUERY_CREDENTIALS_ATTRIBUTES_FN QUERY_CREDENTIALS_ATTRIBUTES_FN_W 

SECURITY_STATUS SEC_ENTRY
FreeContextBuffer(
    void SEC_FAR * pvContextBuffer      // buffer to free
    );

typedef SECURITY_STATUS
(SEC_ENTRY * FREE_CONTEXT_BUFFER_FN)(
    void SEC_FAR *);

///////////////////////////////////////////////////////////////////
////
////    Message Support API
////
//////////////////////////////////////////////////////////////////

SECURITY_STATUS SEC_ENTRY
MakeSignature(
    PCtxtHandle phContext,              // Context to use
    unsigned long fQOP,                 // Quality of Protection
    PSecBufferDesc pMessage,            // Message to sign
    unsigned long MessageSeqNo          // Message Sequence Num.
    );

typedef SECURITY_STATUS
(SEC_ENTRY * MAKE_SIGNATURE_FN)(
    PCtxtHandle,
    unsigned long,
    PSecBufferDesc,
    unsigned long);



SECURITY_STATUS SEC_ENTRY
VerifySignature(
    PCtxtHandle phContext,              // Context to use
    PSecBufferDesc pMessage,            // Message to verify
    unsigned long MessageSeqNo,         // Sequence Num.
    unsigned long SEC_FAR * pfQOP       // QOP used
    );

typedef SECURITY_STATUS
(SEC_ENTRY * VERIFY_SIGNATURE_FN)(
    PCtxtHandle,
    PSecBufferDesc,
    unsigned long,
    unsigned long SEC_FAR *);


SECURITY_STATUS SEC_ENTRY
EncryptMessage( PCtxtHandle         phContext,
                unsigned long       fQOP,
                PSecBufferDesc      pMessage,
                unsigned long       MessageSeqNo);

typedef SECURITY_STATUS
(SEC_ENTRY * ENCRYPT_MESSAGE_FN)(
    PCtxtHandle, unsigned long, PSecBufferDesc, unsigned long);


SECURITY_STATUS SEC_ENTRY
DecryptMessage( PCtxtHandle         phContext,
                PSecBufferDesc      pMessage,
                unsigned long       MessageSeqNo,
                unsigned long *     pfQOP);


typedef SECURITY_STATUS
(SEC_ENTRY * DECRYPT_MESSAGE_FN)(
    PCtxtHandle, PSecBufferDesc, unsigned long,
    unsigned long SEC_FAR *);


///////////////////////////////////////////////////////////////////////////
////
////    Misc.
////
///////////////////////////////////////////////////////////////////////////


SECURITY_STATUS SEC_ENTRY
EnumerateSecurityPackagesW(
    unsigned long SEC_FAR * pcPackages,     // Receives num. packages
    PSecPkgInfoW SEC_FAR * ppPackageInfo    // Receives array of info
    );

typedef SECURITY_STATUS
(SEC_ENTRY * ENUMERATE_SECURITY_PACKAGES_FN_W)(
    unsigned long SEC_FAR *,
    PSecPkgInfoW SEC_FAR *);

#  define EnumerateSecurityPackages EnumerateSecurityPackagesW              
#  define ENUMERATE_SECURITY_PACKAGES_FN ENUMERATE_SECURITY_PACKAGES_FN_W   

SECURITY_STATUS SEC_ENTRY
QuerySecurityPackageInfoW(
#if ISSP_MODE == 0
    PSECURITY_STRING pPackageName,
#else
    SEC_WCHAR SEC_FAR * pszPackageName,     // Name of package
#endif
    PSecPkgInfoW SEC_FAR *ppPackageInfo              // Receives package info
    );

typedef SECURITY_STATUS
(SEC_ENTRY * QUERY_SECURITY_PACKAGE_INFO_FN_W)(
#if ISSP_MODE == 0
    PSECURITY_STRING,
#else
    SEC_WCHAR SEC_FAR *,
#endif
    PSecPkgInfoW SEC_FAR *);

#  define QuerySecurityPackageInfo QuerySecurityPackageInfoW                
#  define QUERY_SECURITY_PACKAGE_INFO_FN QUERY_SECURITY_PACKAGE_INFO_FN_W   

///////////////////////////////////////////////////////////////////////////
////
////    Context export/import
////
///////////////////////////////////////////////////////////////////////////



SECURITY_STATUS SEC_ENTRY
ExportSecurityContext(
    PCtxtHandle          phContext,             // (in) context to export
    ULONG                fFlags,                // (in) option flags
    PSecBuffer           pPackedContext,        // (out) marshalled context
    void SEC_FAR * SEC_FAR * pToken                 // (out, optional) token handle for impersonation
    );

typedef SECURITY_STATUS
(SEC_ENTRY * EXPORT_SECURITY_CONTEXT_FN)(
    PCtxtHandle,
    ULONG,
    PSecBuffer,
    void SEC_FAR * SEC_FAR *
    );

SECURITY_STATUS SEC_ENTRY
ImportSecurityContextW(
#if ISSP_MODE == 0
    PSECURITY_STRING     pszPackage,
#else
    SEC_WCHAR SEC_FAR * pszPackage,
#endif
    PSecBuffer           pPackedContext,        // (in) marshalled context
    void SEC_FAR *       Token,                 // (in, optional) handle to token for context
    PCtxtHandle          phContext              // (out) new context handle
    );

typedef SECURITY_STATUS
(SEC_ENTRY * IMPORT_SECURITY_CONTEXT_FN_W)(
#if ISSP_MODE == 0
    PSECURITY_STRING,
#else
    SEC_WCHAR SEC_FAR *,
#endif
    PSecBuffer,
    VOID SEC_FAR *,
    PCtxtHandle
    );

#  define ImportSecurityContext ImportSecurityContextW              
#  define IMPORT_SECURITY_CONTEXT_FN IMPORT_SECURITY_CONTEXT_FN_W   

#if ISSP_MODE == 0
NTSTATUS
NTAPI
SecMakeSPN(
    IN PUNICODE_STRING ServiceClass,
    IN PUNICODE_STRING ServiceName,
    IN PUNICODE_STRING InstanceName OPTIONAL,
    IN USHORT InstancePort OPTIONAL,
    IN PUNICODE_STRING Referrer OPTIONAL,
    IN OUT PUNICODE_STRING Spn,
    OUT PULONG Length OPTIONAL,
    IN BOOLEAN Allocate
    );
#endif

#define SECURITY_ENTRYPOINTW SEC_TEXT("InitSecurityInterfaceW")     
#    define SECURITY_ENTRYPOINT SECURITY_ENTRYPOINTW                

#define FreeCredentialHandle FreeCredentialsHandle

typedef struct _SECURITY_FUNCTION_TABLE_W {
    unsigned long                       dwVersion;
    ENUMERATE_SECURITY_PACKAGES_FN_W    EnumerateSecurityPackagesW;
    QUERY_CREDENTIALS_ATTRIBUTES_FN_W   QueryCredentialsAttributesW;
    ACQUIRE_CREDENTIALS_HANDLE_FN_W     AcquireCredentialsHandleW;
    FREE_CREDENTIALS_HANDLE_FN          FreeCredentialsHandle;
#ifndef WIN32_CHICAGO
    void SEC_FAR *                      Reserved2;
#else // WIN32_CHICAGO
    SSPI_LOGON_USER_FN                  SspiLogonUserW;
#endif // WIN32_CHICAGO
    INITIALIZE_SECURITY_CONTEXT_FN_W    InitializeSecurityContextW;
    ACCEPT_SECURITY_CONTEXT_FN          AcceptSecurityContext;
    COMPLETE_AUTH_TOKEN_FN              CompleteAuthToken;
    DELETE_SECURITY_CONTEXT_FN          DeleteSecurityContext;
    APPLY_CONTROL_TOKEN_FN              ApplyControlToken;
    QUERY_CONTEXT_ATTRIBUTES_FN_W       QueryContextAttributesW;
    IMPERSONATE_SECURITY_CONTEXT_FN     ImpersonateSecurityContext;
    REVERT_SECURITY_CONTEXT_FN          RevertSecurityContext;
    MAKE_SIGNATURE_FN                   MakeSignature;
    VERIFY_SIGNATURE_FN                 VerifySignature;
    FREE_CONTEXT_BUFFER_FN              FreeContextBuffer;
    QUERY_SECURITY_PACKAGE_INFO_FN_W    QuerySecurityPackageInfoW;
    void SEC_FAR *                      Reserved3;
    void SEC_FAR *                      Reserved4;
    EXPORT_SECURITY_CONTEXT_FN          ExportSecurityContext;
    IMPORT_SECURITY_CONTEXT_FN_W        ImportSecurityContextW;
    ADD_CREDENTIALS_FN_W                AddCredentialsW ;
    void SEC_FAR *                      Reserved8;
    QUERY_SECURITY_CONTEXT_TOKEN_FN     QuerySecurityContextToken;
    ENCRYPT_MESSAGE_FN                  EncryptMessage;
    DECRYPT_MESSAGE_FN                  DecryptMessage;
} SecurityFunctionTableW, SEC_FAR * PSecurityFunctionTableW;

#  define SecurityFunctionTable SecurityFunctionTableW      
#  define PSecurityFunctionTable PSecurityFunctionTableW    
#define SECURITY_SUPPORT_PROVIDER_INTERFACE_VERSION     1   

PSecurityFunctionTableW SEC_ENTRY
InitSecurityInterfaceW(
    void
    );

typedef PSecurityFunctionTableW
(SEC_ENTRY * INIT_SECURITY_INTERFACE_W)(void);

#  define InitSecurityInterface InitSecurityInterfaceW          
#  define INIT_SECURITY_INTERFACE INIT_SECURITY_INTERFACE_W     
#endif // __SSPI_H__

#ifndef SECURITY_USER_DATA_DEFINED
#define SECURITY_USER_DATA_DEFINED

typedef struct _SECURITY_USER_DATA {
    SECURITY_STRING UserName;           // User name
    SECURITY_STRING LogonDomainName;    // Domain the user logged on to
    SECURITY_STRING LogonServer;        // Server that logged the user on
    PSID            pSid;               // SID of user
} SECURITY_USER_DATA, *PSECURITY_USER_DATA;

typedef SECURITY_USER_DATA SecurityUserData, * PSecurityUserData;


#define UNDERSTANDS_LONG_NAMES  1
#define NO_LONG_NAMES           2

#endif // SECURITY_USER_DATA_DEFINED

HRESULT SEC_ENTRY
GetSecurityUserInfo(
    IN PLUID LogonId,
    IN ULONG Flags,
    OUT PSecurityUserData * UserInformation
    );


SECURITY_STATUS SEC_ENTRY
MapSecurityError( SECURITY_STATUS SecStatus );

#define NTLMSP_NAME              L"NTLM"        
#define NTLMSP_NAME_SIZE        (sizeof(NTLMSP_NAME) - sizeof(WCHAR))  

#define NTLMSSP_SIGNATURE "NTLMSSP"

//
// MessageType for the following messages.
//

typedef enum {
    NtLmNegotiate = 1,
    NtLmChallenge,
    NtLmAuthenticate,
    NtLmUnknown
} NTLM_MESSAGE_TYPE;

//
// Valid values of NegotiateFlags
//

#define NTLMSSP_NEGOTIATE_UNICODE               0x00000001  // Text strings are in unicode
#define NTLMSSP_NEGOTIATE_OEM                   0x00000002  // Text strings are in OEM
#define NTLMSSP_REQUEST_TARGET                  0x00000004  // Server should return its authentication realm

#define NTLMSSP_NEGOTIATE_SIGN                  0x00000010  // Request signature capability
#define NTLMSSP_NEGOTIATE_SEAL                  0x00000020  // Request confidentiality
#define NTLMSSP_NEGOTIATE_DATAGRAM              0x00000040  // Use datagram style authentication
#define NTLMSSP_NEGOTIATE_LM_KEY                0x00000080  // Use LM session key for sign/seal

#define NTLMSSP_NEGOTIATE_NETWARE               0x00000100  // NetWare authentication
#define NTLMSSP_NEGOTIATE_NTLM                  0x00000200  // NTLM authentication
#define NTLMSSP_NEGOTIATE_NT_ONLY               0x00000400  // NT authentication only (no LM)
#define NTLMSSP_NEGOTIATE_NULL_SESSION          0x00000800  // NULL Sessions on NT 5.0 and beyand

#define NTLMSSP_NEGOTIATE_OEM_DOMAIN_SUPPLIED       0x1000  // Domain Name supplied on negotiate
#define NTLMSSP_NEGOTIATE_OEM_WORKSTATION_SUPPLIED  0x2000  // Workstation Name supplied on negotiate
#define NTLMSSP_NEGOTIATE_LOCAL_CALL            0x00004000  // Indicates client/server are same machine
#define NTLMSSP_NEGOTIATE_ALWAYS_SIGN           0x00008000  // Sign for all security levels


//
// Valid target types returned by the server in Negotiate Flags
//

#define NTLMSSP_TARGET_TYPE_DOMAIN              0x00010000  // TargetName is a domain name
#define NTLMSSP_TARGET_TYPE_SERVER              0x00020000  // TargetName is a server name
#define NTLMSSP_TARGET_TYPE_SHARE               0x00040000  // TargetName is a share name
#define NTLMSSP_NEGOTIATE_NTLM2                 0x00080000  // NTLM2 authentication added for NT4-SP4

#define NTLMSSP_NEGOTIATE_IDENTIFY              0x00100000  // Create identify level token

//
// Valid requests for additional output buffers
//

#define NTLMSSP_REQUEST_INIT_RESPONSE           0x00100000  // get back session keys
#define NTLMSSP_REQUEST_ACCEPT_RESPONSE         0x00200000  // get back session key, LUID
#define NTLMSSP_REQUEST_NON_NT_SESSION_KEY      0x00400000  // request non-nt session key
#define NTLMSSP_NEGOTIATE_TARGET_INFO           0x00800000  // target info present in challenge message

#define NTLMSSP_NEGOTIATE_EXPORTED_CONTEXT      0x01000000  // It's an exported context

#define NTLMSSP_NEGOTIATE_128                   0x20000000  // negotiate 128 bit encryption
#define NTLMSSP_NEGOTIATE_KEY_EXCH              0x40000000  // exchange a key using key exchange key
#define NTLMSSP_NEGOTIATE_56                    0x80000000  // negotiate 56 bit encryption

// flags used in client space to control sign and seal; never appear on the wire
#define NTLMSSP_APP_SEQ                 0x0040  // Use application provided seq num


typedef struct _OLD_NEGOTIATE_MESSAGE {
    UCHAR Signature[sizeof(NTLMSSP_SIGNATURE)];
    NTLM_MESSAGE_TYPE MessageType;
    ULONG NegotiateFlags;
} OLD_NEGOTIATE_MESSAGE, *POLD_NEGOTIATE_MESSAGE;

//
// Opaque message returned from first call to AcceptSecurityContext
//
typedef struct _CHALLENGE_MESSAGE {
    UCHAR Signature[sizeof(NTLMSSP_SIGNATURE)];
    NTLM_MESSAGE_TYPE MessageType;
    STRING32 TargetName;
    ULONG NegotiateFlags;
    UCHAR Challenge[MSV1_0_CHALLENGE_LENGTH];
    ULONG64 ServerContextHandle;
    STRING32 TargetInfo;
} CHALLENGE_MESSAGE, *PCHALLENGE_MESSAGE;

//
// Old version of the challenge message
//

typedef struct _OLD_CHALLENGE_MESSAGE {
    UCHAR Signature[sizeof(NTLMSSP_SIGNATURE)];
    NTLM_MESSAGE_TYPE MessageType;
    STRING32 TargetName;
    ULONG NegotiateFlags;
    UCHAR Challenge[MSV1_0_CHALLENGE_LENGTH];
} OLD_CHALLENGE_MESSAGE, *POLD_CHALLENGE_MESSAGE;

//
// Opaque message returned from second call to InitializeSecurityContext
//
typedef struct _AUTHENTICATE_MESSAGE {
    UCHAR Signature[sizeof(NTLMSSP_SIGNATURE)];
    NTLM_MESSAGE_TYPE MessageType;
    STRING32 LmChallengeResponse;
    STRING32 NtChallengeResponse;
    STRING32 DomainName;
    STRING32 UserName;
    STRING32 Workstation;
    STRING32 SessionKey;
    ULONG NegotiateFlags;
} AUTHENTICATE_MESSAGE, *PAUTHENTICATE_MESSAGE;

typedef struct _OLD_AUTHENTICATE_MESSAGE {
    UCHAR Signature[sizeof(NTLMSSP_SIGNATURE)];
    NTLM_MESSAGE_TYPE MessageType;
    STRING32 LmChallengeResponse;
    STRING32 NtChallengeResponse;
    STRING32 DomainName;
    STRING32 UserName;
    STRING32 Workstation;
} OLD_AUTHENTICATE_MESSAGE, *POLD_AUTHENTICATE_MESSAGE;


//
// Additional input message to Initialize for clients to provide a
// user-supplied password
//

typedef struct _NTLM_CHALLENGE_MESSAGE {
    UNICODE_STRING32 Password;
    UNICODE_STRING32 UserName;
    UNICODE_STRING32 DomainName;
} NTLM_CHALLENGE_MESSAGE, *PNTLM_CHALLENGE_MESSAGE;


//
// Non-opaque message returned from second call to InitializeSecurityContext
//

typedef struct _NTLM_INITIALIZE_RESPONSE {
    UCHAR UserSessionKey[MSV1_0_USER_SESSION_KEY_LENGTH];
    UCHAR LanmanSessionKey[MSV1_0_LANMAN_SESSION_KEY_LENGTH];
} NTLM_INITIALIZE_RESPONSE, *PNTLM_INITIALIZE_RESPONSE;

//
// Additional input message to Accept for trusted client skipping the first
// call to Accept and providing their own challenge
//

typedef struct _NTLM_AUTHENTICATE_MESSAGE {
    CHAR ChallengeToClient[MSV1_0_CHALLENGE_LENGTH];
    ULONG ParameterControl;
} NTLM_AUTHENTICATE_MESSAGE, *PNTLM_AUTHENTICATE_MESSAGE;


//
// Non-opaque message returned from second call to AcceptSecurityContext
//

typedef struct _NTLM_ACCEPT_RESPONSE {
    LUID LogonId;
    LARGE_INTEGER KickoffTime;
    ULONG UserFlags;
    UCHAR UserSessionKey[MSV1_0_USER_SESSION_KEY_LENGTH];
    UCHAR LanmanSessionKey[MSV1_0_LANMAN_SESSION_KEY_LENGTH];
} NTLM_ACCEPT_RESPONSE, *PNTLM_ACCEPT_RESPONSE;

#define DD_MUP_DEVICE_NAME L"\\Device\\Mup"    

#define IOCTL_REDIR_QUERY_PATH              CTL_CODE(FILE_DEVICE_NETWORK_FILE_SYSTEM, 99, METHOD_NEITHER, FILE_ANY_ACCESS)

typedef struct _QUERY_PATH_REQUEST {
    ULONG PathNameLength;
    PIO_SECURITY_CONTEXT SecurityContext;
    WCHAR FilePathName[1];
} QUERY_PATH_REQUEST, *PQUERY_PATH_REQUEST;

typedef struct _QUERY_PATH_RESPONSE {
    ULONG LengthAccepted;
} QUERY_PATH_RESPONSE, *PQUERY_PATH_RESPONSE;

//
// Define external data.
// because of indirection for all drivers external to ntoskrnl these are actually ptrs
//

#if defined(_NTDDK_) || defined(_NTIFS_) || defined(_NTHAL_) || defined(_WDMDDK_)

extern PBOOLEAN KdDebuggerNotPresent;
extern PBOOLEAN KdDebuggerEnabled;

#else

extern BOOLEAN KdDebuggerNotPresent;
extern BOOLEAN KdDebuggerEnabled;

#endif




typedef struct _DBGKD_DEBUG_DATA_HEADER64 *PDBGKD_DEBUG_DATA_HEADER64;

BOOLEAN
KdRegisterDebuggerDataBlock(
    IN ULONG Tag,
    IN PDBGKD_DEBUG_DATA_HEADER64 DataHeader,
    IN ULONG Size
    );

VOID
KdDeregisterDebuggerDataBlock32(
    IN PDBGKD_DEBUG_DATA_HEADER64 DataHeader
    );

VOID
KdDisableDebugger(
    VOID
    );

VOID
KdEnableDebugger(
    VOID
    );


#ifdef POOL_TAGGING
#define ExAllocatePool(a,b) ExAllocatePoolWithTag(a,b,' sfI')
#define ExAllocatePoolWithQuota(a,b) ExAllocatePoolWithQuotaTag(a,b,' sfI')
#endif

extern POBJECT_TYPE *PsThreadType;
extern POBJECT_TYPE *IoFileObjectType;
extern POBJECT_TYPE *ExEventObjectType;
extern POBJECT_TYPE *ExSemaphoreObjectType;

//
// Define exported ZwXxx routines to device drivers.
//

NTSYSAPI
NTSTATUS
NTAPI
ZwCreateEvent (
    OUT PHANDLE EventHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
    IN EVENT_TYPE EventType,
    IN BOOLEAN InitialState
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwCreateFile(
    OUT PHANDLE FileHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    IN PLARGE_INTEGER AllocationSize OPTIONAL,
    IN ULONG FileAttributes,
    IN ULONG ShareAccess,
    IN ULONG CreateDisposition,
    IN ULONG CreateOptions,
    IN PVOID EaBuffer OPTIONAL,
    IN ULONG EaLength
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwOpenFile(
    OUT PHANDLE FileHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    IN ULONG ShareAccess,
    IN ULONG OpenOptions
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwDeviceIoControlFile(
    IN HANDLE FileHandle,
    IN HANDLE Event OPTIONAL,
    IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
    IN PVOID ApcContext OPTIONAL,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    IN ULONG IoControlCode,
    IN PVOID InputBuffer OPTIONAL,
    IN ULONG InputBufferLength,
    OUT PVOID OutputBuffer OPTIONAL,
    IN ULONG OutputBufferLength
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwQueryInformationFile(
    IN HANDLE FileHandle,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    OUT PVOID FileInformation,
    IN ULONG Length,
    IN FILE_INFORMATION_CLASS FileInformationClass
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwSetInformationFile(
    IN HANDLE FileHandle,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    IN PVOID FileInformation,
    IN ULONG Length,
    IN FILE_INFORMATION_CLASS FileInformationClass
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwReadFile(
    IN HANDLE FileHandle,
    IN HANDLE Event OPTIONAL,
    IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
    IN PVOID ApcContext OPTIONAL,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    OUT PVOID Buffer,
    IN ULONG Length,
    IN PLARGE_INTEGER ByteOffset OPTIONAL,
    IN PULONG Key OPTIONAL
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwWriteFile(
    IN HANDLE FileHandle,
    IN HANDLE Event OPTIONAL,
    IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
    IN PVOID ApcContext OPTIONAL,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    IN PVOID Buffer,
    IN ULONG Length,
    IN PLARGE_INTEGER ByteOffset OPTIONAL,
    IN PULONG Key OPTIONAL
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwClose(
    IN HANDLE Handle
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwDuplicateObject(
    IN HANDLE SourceProcessHandle,
    IN HANDLE SourceHandle,
    IN HANDLE TargetProcessHandle OPTIONAL,
    OUT PHANDLE TargetHandle OPTIONAL,
    IN ACCESS_MASK DesiredAccess,
    IN ULONG HandleAttributes,
    IN ULONG Options
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwCreateDirectoryObject(
    OUT PHANDLE DirectoryHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwMakeTemporaryObject(
    IN HANDLE Handle
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwCreateKey(
    OUT PHANDLE KeyHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    IN ULONG TitleIndex,
    IN PUNICODE_STRING Class OPTIONAL,
    IN ULONG CreateOptions,
    OUT PULONG Disposition OPTIONAL
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwOpenKey(
    OUT PHANDLE KeyHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwDeleteKey(
    IN HANDLE KeyHandle
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwEnumerateKey(
    IN HANDLE KeyHandle,
    IN ULONG Index,
    IN KEY_INFORMATION_CLASS KeyInformationClass,
    OUT PVOID KeyInformation,
    IN ULONG Length,
    OUT PULONG ResultLength
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwEnumerateValueKey(
    IN HANDLE KeyHandle,
    IN ULONG Index,
    IN KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
    OUT PVOID KeyValueInformation,
    IN ULONG Length,
    OUT PULONG ResultLength
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwFlushKey(
    IN HANDLE KeyHandle
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwQueryKey(
    IN HANDLE KeyHandle,
    IN KEY_INFORMATION_CLASS KeyInformationClass,
    OUT PVOID KeyInformation,
    IN ULONG Length,
    OUT PULONG ResultLength
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwQueryValueKey(
    IN HANDLE KeyHandle,
    IN PUNICODE_STRING ValueName,
    IN KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
    OUT PVOID KeyValueInformation,
    IN ULONG Length,
    OUT PULONG ResultLength
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwSetValueKey(
    IN HANDLE KeyHandle,
    IN PUNICODE_STRING ValueName,
    IN ULONG TitleIndex OPTIONAL,
    IN ULONG Type,
    IN PVOID Data,
    IN ULONG DataSize
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwOpenSection(
    OUT PHANDLE SectionHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwMapViewOfSection(
    IN HANDLE SectionHandle,
    IN HANDLE ProcessHandle,
    IN OUT PVOID *BaseAddress,
    IN ULONG ZeroBits,
    IN ULONG CommitSize,
    IN OUT PLARGE_INTEGER SectionOffset OPTIONAL,
    IN OUT PSIZE_T ViewSize,
    IN SECTION_INHERIT InheritDisposition,
    IN ULONG AllocationType,
    IN ULONG Protect
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwUnmapViewOfSection(
    IN HANDLE ProcessHandle,
    IN PVOID BaseAddress
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwCreateSection (
    OUT PHANDLE SectionHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
    IN PLARGE_INTEGER MaximumSize OPTIONAL,
    IN ULONG SectionPageProtection,
    IN ULONG AllocationAttributes,
    IN HANDLE FileHandle OPTIONAL
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwAllocateVirtualMemory(
    IN HANDLE ProcessHandle,
    IN OUT PVOID *BaseAddress,
    IN ULONG ZeroBits,
    IN OUT PSIZE_T RegionSize,
    IN ULONG AllocationType,
    IN ULONG Protect
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwFreeVirtualMemory(
    IN HANDLE ProcessHandle,
    IN OUT PVOID *BaseAddress,
    IN OUT PSIZE_T RegionSize,
    IN ULONG FreeType
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwWaitForSingleObject(
    IN HANDLE Handle,
    IN BOOLEAN Alertable,
    IN PLARGE_INTEGER Timeout OPTIONAL
    );

NTSYSAPI
NTSTATUS
NTAPI
ZwSetEvent (
    IN HANDLE Handle,
    OUT PLONG PreviousState OPTIONAL
    );

#endif // _NTIFS_

