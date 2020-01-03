//
// Creator: Leo Jiang <JiangFengbing@kingsoft.net>
//

// 使用示范:
//
//      /* 初始化 */
// CSignVerify::Instance().Initialize();                 
//
//
//      /* 添加受信任签名公司, 已经内置Kingsoft和MS */
// CSignVerify::Instance().AddTrustModule( "Leosoft" );     
//
//
//      /* 验证内置数字签名 */
// fRetval = CSignVerify::Instance().VerifyEmbedSignA( "c:\\KAV2007\\KASMain.EXE" );
//
//
//      /* 验证Windows内置Hash集 */
// fRetval = CSignVerify::Instance().VerifyCatSignW(L"C:\\Windows\\explorer.exe");
//
//
//      /* 反初始化 */
// CSignVerify::Instance().UnInitialize();
//
//

#ifndef SIGNVERIFY_INC_
#define SIGNVERIFY_INC_

#ifndef MSCAT_H
#define MSCAT_H
#include <WinCrypt.h>
#include <mssip.h>
#include <string>
#include <vector>
#include <AtlConv.h>
using namespace std;

#ifdef __cplusplus
extern "C"
{
#endif

#define     szOID_CATALOG_LIST                  "1.3.6.1.4.1.311.12.1.1"
#define     szOID_CATALOG_LIST_MEMBER           "1.3.6.1.4.1.311.12.1.2"
#define     CRYPTCAT_FILEEXT                    L"CAT"
#define     CRYPTCAT_MAX_MEMBERTAG              64
#define     CRYPTCAT_OPEN_CREATENEW             0x00000001  // creates/overwrites
#define     CRYPTCAT_OPEN_ALWAYS                0x00000002  // opens/creates
#define     CRYPTCAT_OPEN_EXISTING              0x00000004  // opens only
#define     CRYPTCAT_OPEN_EXCLUDE_PAGE_HASHES   0x00010000  // Exclude page hashes in SPC_INDIRECT_DATA
#define     CRYPTCAT_OPEN_INCLUDE_PAGE_HASHES   0x00020000  // Include page hashes in SPC_INDIRECT_DATA
#define     CRYPTCAT_OPEN_VERIFYSIGHASH         0x10000000  // verifies the signature (not the certs!)
#define     CRYPTCAT_OPEN_NO_CONTENT_HCRYPTMSG  0x20000000  // does CryptMsgDecode without content
#define     CRYPTCAT_OPEN_SORTED                0x40000000  // does CertCreateContext(CERT_CREATE_CONTEXT_SORTED_FLAG)
#define     CRYPTCAT_OPEN_FLAGS_MASK            0xffff0000  // mask for upper word
#define     CRYPTCAT_MEMBER_SORTED              0x40000000
#define     CRYPTCAT_ATTR_AUTHENTICATED         0x10000000
#define     CRYPTCAT_ATTR_UNAUTHENTICATED       0x20000000
#define     CRYPTCAT_ATTR_NAMEASCII             0x00000001  // ascii string
#define     CRYPTCAT_ATTR_NAMEOBJID             0x00000002  // crypt obj id
#define     CRYPTCAT_ATTR_DATAASCII             0x00010000  // do not decode simple ascii chars
#define     CRYPTCAT_ATTR_DATABASE64            0x00020000  // base 64
#define     CRYPTCAT_ATTR_DATAREPLACE           0x00040000  // this data is a replacment for an existing attr
#define     CRYPTCAT_E_AREA_HEADER              0x00000000
#define     CRYPTCAT_E_AREA_MEMBER              0x00010000
#define     CRYPTCAT_E_AREA_ATTRIBUTE           0x00020000
#define     CRYPTCAT_E_CDF_UNSUPPORTED          0x00000001
#define     CRYPTCAT_E_CDF_DUPLICATE            0x00000002
#define     CRYPTCAT_E_CDF_TAGNOTFOUND          0x00000004
#define     CRYPTCAT_E_CDF_MEMBER_FILE_PATH     0x00010001
#define     CRYPTCAT_E_CDF_MEMBER_INDIRECTDATA  0x00010002
#define     CRYPTCAT_E_CDF_MEMBER_FILENOTFOUND  0x00010004
#define     CRYPTCAT_E_CDF_BAD_GUID_CONV        0x00020001
#define     CRYPTCAT_E_CDF_ATTR_TOOFEWVALUES    0x00020002
#define     CRYPTCAT_E_CDF_ATTR_TYPECOMBO       0x00020004

#include <pshpack8.h>

	typedef struct CRYPTCATSTORE_
	{
		DWORD                       cbStruct;       // = sizeof(CRYPTCATSTORE)
		DWORD                       dwPublicVersion;
		LPWSTR                      pwszP7File;
		HCRYPTPROV                  hProv;
		DWORD                       dwEncodingType;
		DWORD                       fdwStoreFlags;
		HANDLE                      hReserved;      // pStack(members) (null if init/pbData) INTERNAL!
		HANDLE                      hAttrs;         // pStack(Catalog attrs) INTERNAL!
		HCRYPTMSG                   hCryptMsg;
		HANDLE                      hSorted;        // INTERNAL ONLY!

	} CRYPTCATSTORE;

	typedef struct CRYPTCATMEMBER_
	{
		DWORD                       cbStruct;           // = sizeof(CRYPTCATMEMBER)
		LPWSTR                      pwszReferenceTag;
		LPWSTR                      pwszFileName;       // used only by the CDF APIs
		GUID                        gSubjectType;       // may be zeros -- see sEncodedMemberInfo
		DWORD                       fdwMemberFlags;
		struct SIP_INDIRECT_DATA_   *pIndirectData;     // may be null -- see sEncodedIndirectData
		DWORD                       dwCertVersion;      // may be zero -- see sEncodedMemberInfo
		DWORD                       dwReserved;         // used by enum -- DO NOT USE!
		HANDLE                      hReserved;          // pStack(attrs) (null if init) INTERNAL!

		CRYPT_ATTR_BLOB             sEncodedIndirectData;   // lazy decode
		CRYPT_ATTR_BLOB             sEncodedMemberInfo;     // lazy decode

	} CRYPTCATMEMBER;

	typedef struct CRYPTCATATTRIBUTE_
	{
		DWORD                       cbStruct;           // = sizeof(CRYPTCATATTRIBUTE)
		LPWSTR                      pwszReferenceTag;
		DWORD                       dwAttrTypeAndAction;
		DWORD                       cbValue;
		BYTE                        *pbValue;           // encoded CAT_NAMEVALUE struct
		DWORD                       dwReserved;         // used by enum -- DO NOT USE!

	} CRYPTCATATTRIBUTE;

	typedef struct CRYPTCATCDF_
	{
		DWORD                       cbStruct;           // = sizeof(CRYPTCATCDF)
		HANDLE                      hFile;
		DWORD                       dwCurFilePos;
		DWORD                       dwLastMemberOffset;
		BOOL                        fEOF;
		LPWSTR                      pwszResultDir;
		HANDLE                      hCATStore;

	} CRYPTCATCDF;

	typedef struct CATALOG_INFO_
	{
		DWORD                       cbStruct;   // set to sizeof(CATALOG_INFO)

		WCHAR                       wszCatalogFile[MAX_PATH];

	} CATALOG_INFO;

	typedef HANDLE          HCATADMIN;
	typedef HANDLE          HCATINFO;

#include <poppack.h>

	typedef void (WINAPI *PFN_CDF_PARSE_ERROR_CALLBACK)(IN DWORD dwErrorArea,
		IN DWORD dwLocalError,
		IN WCHAR *pwszLine);

	extern HANDLE WINAPI CryptCATOpen(        LPWSTR pwszFileName,
		IN          DWORD fdwOpenFlags,
		IN OPTIONAL HCRYPTPROV hProv,
		IN OPTIONAL DWORD dwPublicVersion,
		IN OPTIONAL DWORD dwEncodingType);

	extern void WINAPI
		CryptCATSetCreatePageHashesFlag( IN HANDLE hCatalog, IN BOOL newCreatePageHashesFlag );

	extern BOOL WINAPI CryptCATClose(IN HANDLE hCatalog);

	extern CRYPTCATSTORE * WINAPI CryptCATStoreFromHandle(IN HANDLE hCatalog);

	extern HANDLE WINAPI CryptCATHandleFromStore(IN CRYPTCATSTORE *pCatStore);

	extern BOOL WINAPI CryptCATPersistStore(IN HANDLE hCatalog);

	extern CRYPTCATATTRIBUTE * WINAPI CryptCATGetCatAttrInfo(IN HANDLE hCatalog,
		LPWSTR pwszReferenceTag);

	extern CRYPTCATATTRIBUTE * WINAPI CryptCATPutCatAttrInfo(IN HANDLE hCatalog,
		LPWSTR pwszReferenceTag,
		IN DWORD dwAttrTypeAndAction,
		IN DWORD cbData,
		IN BYTE *pbData);

	extern CRYPTCATATTRIBUTE * WINAPI CryptCATEnumerateCatAttr(IN HANDLE hCatalog,
		IN CRYPTCATATTRIBUTE *pPrevAttr);

	extern CRYPTCATMEMBER * WINAPI CryptCATGetMemberInfo(IN HANDLE hCatalog,
		LPWSTR pwszReferenceTag);

	extern CRYPTCATMEMBER * WINAPI CryptCATAllocSortedMemberInfo(IN HANDLE hCatalog, 
		LPWSTR pwszReferenceTag);

	extern void WINAPI CryptCATFreeSortedMemberInfo(IN HANDLE hCatalog, 
		IN CRYPTCATMEMBER *pCatMember);

	extern CRYPTCATATTRIBUTE * WINAPI CryptCATGetAttrInfo(
		HANDLE hCatalog,
		CRYPTCATMEMBER *pCatMember,
		LPWSTR pwszReferenceTag);

	extern CRYPTCATMEMBER * WINAPI CryptCATPutMemberInfo(
		HANDLE hCatalog,
		LPWSTR pwszFileName,
		LPWSTR pwszReferenceTag,
		GUID *pgSubjectType,
		DWORD dwCertVersion,
		DWORD cbSIPIndirectData,
		BYTE *pbSIPIndirectData);

	extern CRYPTCATATTRIBUTE * WINAPI CryptCATPutAttrInfo(
		HANDLE hCatalog,
		CRYPTCATMEMBER *pCatMember,
		LPWSTR pwszReferenceTag,
		DWORD dwAttrTypeAndAction,
		DWORD cbData,
		BYTE *pbData);

	extern CRYPTCATMEMBER * WINAPI CryptCATEnumerateMember( 
		HANDLE hCatalog,
		CRYPTCATMEMBER *pPrevMember);

	extern CRYPTCATATTRIBUTE * WINAPI CryptCATEnumerateAttr(
		HANDLE hCatalog,
		CRYPTCATMEMBER *pCatMember,
		CRYPTCATATTRIBUTE *pPrevAttr);

	extern CRYPTCATCDF * WINAPI CryptCATCDFOpen(	 
		LPWSTR pwszFilePath,
		PFN_CDF_PARSE_ERROR_CALLBACK pfnParseError);

	extern BOOL WINAPI CryptCATCDFClose(CRYPTCATCDF *pCDF);

	extern CRYPTCATATTRIBUTE * WINAPI CryptCATCDFEnumCatAttributes(CRYPTCATCDF *pCDF,
		CRYPTCATATTRIBUTE *pPrevAttr,
		PFN_CDF_PARSE_ERROR_CALLBACK pfnParseError);

	extern CRYPTCATMEMBER * WINAPI CryptCATCDFEnumMembers(	
		CRYPTCATCDF *pCDF,
		CRYPTCATMEMBER *pPrevMember,
		PFN_CDF_PARSE_ERROR_CALLBACK pfnParseError);

	extern CRYPTCATATTRIBUTE *WINAPI CryptCATCDFEnumAttributes(	
		CRYPTCATCDF *pCDF,
		CRYPTCATMEMBER *pMember,
		CRYPTCATATTRIBUTE *pPrevAttr,
		PFN_CDF_PARSE_ERROR_CALLBACK pfnParseError);

	extern BOOL WINAPI      IsCatalogFile(
		HANDLE hFile,
		WCHAR *pwszFileName);

	extern BOOL WINAPI      CryptCATAdminAcquireContext(
		HCATADMIN *phCatAdmin,
		const GUID *pgSubsystem,
		DWORD dwFlags);

	extern BOOL WINAPI      CryptCATAdminReleaseContext(
		HCATADMIN hCatAdmin,
		DWORD dwFlags);

	extern BOOL WINAPI CryptCATAdminReleaseCatalogContext(HCATADMIN hCatAdmin,
		HCATINFO hCatInfo,
		DWORD dwFlags);

	extern HCATINFO WINAPI CryptCATAdminEnumCatalogFromHash(HCATADMIN hCatAdmin,
		BYTE *pbHash,
		DWORD cbHash,
		DWORD dwFlags,
		HCATINFO *phPrevCatInfo);

	extern BOOL WINAPI CryptCATAdminCalcHashFromFileHandle(HANDLE hFile,
		DWORD *pcbHash,
		BYTE *pbHash,
		DWORD dwFlags);

	extern HCATINFO WINAPI CryptCATAdminAddCatalog(HCATADMIN hCatAdmin,
		PWSTR pwszCatalogFile,
		PWSTR pwszSelectBaseName,
		DWORD dwFlags);

	extern BOOL WINAPI CryptCATAdminRemoveCatalog(HCATADMIN hCatAdmin,
		LPCWSTR pwszCatalogFile,
		DWORD dwFlags);

	extern BOOL WINAPI CryptCATCatalogInfoFromContext(HCATINFO hCatInfo,
		CATALOG_INFO *psCatInfo,
		DWORD dwFlags);

	extern BOOL WINAPI CryptCATAdminResolveCatalogPath(HCATADMIN hCatAdmin,
		WCHAR *pwszCatalogFile,
		CATALOG_INFO *psCatInfo,
		DWORD dwFlags);


	extern BOOL WINAPI CryptCATAdminPauseServiceForBackup(DWORD dwFlags,
		BOOL  fResume);

#ifdef __cplusplus
}
#endif

#endif  // MSCAT_H

#include <string>
#include <vector>
#include <Wintrust.h>
#include <softpub.h>
#include <strsafe.h>

class CSignVerify {

	typedef BOOL (WINAPI *CRYPTCATADMINACQUIRECONTEXT)(
		OUT HCATADMIN *phCatAdmin,
		IN const GUID *pgSubsystem,
		IN DWORD dwFlags
		);

	typedef BOOL (WINAPI *CRYPTCATADMINRELEASECONTEXT)(
		IN HCATADMIN hCatAdmin,
		IN DWORD dwFlags
		);

	typedef BOOL (WINAPI *CRYPTCATADMINRELEASECATALOGCONTEXT)(
		IN HCATADMIN hCatAdmin,
		IN HCATINFO hCatInfo,
		IN DWORD dwFlags
		);

	typedef BOOL (WINAPI *CRYPTCATCATALOGINFOFROMCONTEXT)(
		IN HCATINFO hCatInfo,
		IN OUT CATALOG_INFO *psCatInfo,
		IN DWORD dwFlags
		);
	typedef BOOL (WINAPI *CRYPTCATADMINCALCHASHFROMFILEHANDLE)(
		IN HANDLE hFile,
		IN OUT DWORD *pcbHash,
		OUT OPTIONAL BYTE *pbHash,
		IN DWORD dwFlags
		);

	typedef HCATINFO (WINAPI *CRYPTCATADMINENUMCATALOGFROMHASH)(
		IN HCATADMIN hCatAdmin,
		IN BYTE *pbHash,
		IN DWORD cbHash,
		IN DWORD dwFlags,
		IN OUT HCATINFO *phPrevCatInfo
		);

	typedef LONG (WINAPI *WINVERIFYTRUST)(
		HWND hwnd,
		GUID *pgActionID,
		LPVOID pWVTData
		);

	typedef CRYPT_PROVIDER_DATA* (WINAPI *WTHELPERPROVDATAFROMSTATEDATA)(
		HANDLE hStateData
		);

	typedef DWORD (WINAPI *CERTNAMETOSTRW)( 
		IN DWORD dwCertEncodingType, 
		IN PCERT_NAME_BLOB pName, 
		IN DWORD dwStrType, 
		OUT OPTIONAL LPWSTR psz, 
		IN DWORD csz 
		);

	typedef CRYPT_PROVIDER_CERT* (WINAPI *WTHELPERGETPROVCERTFROMCHAIN)(
		CRYPT_PROVIDER_SGNR *pSgnr, 
		DWORD idxCert
		);

	typedef CRYPT_PROVIDER_SGNR* (WINAPI *WTHELPERGETPROVSIGNERFROMCHAIN)(
		CRYPT_PROVIDER_DATA *pProvData, 
		DWORD idxSigner, 
		BOOL fCounterSigner, 
		DWORD idxCounterSigner
		);


public:
	static CSignVerify& Instance() {

		static CSignVerify _singleton;
		return _singleton;
	}
#define WINTRUST_MODULE     "wintrust.dll"
#define CRYPT32_MODULE      "Crypt32.dll"

	BOOL Initialize() {

		BOOL retval = FALSE;

		if (NULL == m_hDll[0]) {

			m_hDll[0] = ::LoadLibraryA( WINTRUST_MODULE );

			if (NULL == m_hDll[0])
				goto clean0;
		}

		if (NULL == m_hDll[1]) {

			m_hDll[1] = ::LoadLibraryA( CRYPT32_MODULE );

			if (NULL == m_hDll[1])
				goto clean0;
		}

		if (NULL == m_pCryptCATAdminAcquireContext) {

			m_pCryptCATAdminAcquireContext = (CRYPTCATADMINACQUIRECONTEXT)::GetProcAddress(
				m_hDll[0], "CryptCATAdminAcquireContext" );

			if (NULL == m_pCryptCATAdminAcquireContext)
				goto clean0;
		}

		if (NULL == m_pCryptCATAdminReleaseContext) {

			m_pCryptCATAdminReleaseContext = (CRYPTCATADMINRELEASECONTEXT)::GetProcAddress(
				m_hDll[0], "CryptCATAdminReleaseContext" );

			if (NULL == m_pCryptCATAdminReleaseContext)
				goto clean0;
		}

		if (NULL == m_pCryptCATCatalogInfoFromContext) {

			m_pCryptCATCatalogInfoFromContext = (CRYPTCATCATALOGINFOFROMCONTEXT)::GetProcAddress(
				m_hDll[0], "CryptCATCatalogInfoFromContext" );

			if (NULL == m_pCryptCATCatalogInfoFromContext)
				goto clean0;
		}

		if (NULL == m_pCryptCATAdminReleaseCatalogContext) {

			m_pCryptCATAdminReleaseCatalogContext = (CRYPTCATADMINRELEASECATALOGCONTEXT)::GetProcAddress(
				m_hDll[0], "CryptCATAdminReleaseCatalogContext" );

			if (NULL == m_pCryptCATAdminReleaseCatalogContext)
				goto clean0;
		}

		if (NULL == m_pCryptCATAdminCalcHashFromFileHandle) {

			m_pCryptCATAdminCalcHashFromFileHandle = (CRYPTCATADMINCALCHASHFROMFILEHANDLE)::GetProcAddress(
				m_hDll[0], "CryptCATAdminCalcHashFromFileHandle" );

			if (NULL == m_pCryptCATAdminCalcHashFromFileHandle)
				goto clean0;
		}

		if (NULL == m_pCryptCATAdminEnumCatalogFromHash) {

			m_pCryptCATAdminEnumCatalogFromHash = (CRYPTCATADMINENUMCATALOGFROMHASH)::GetProcAddress(
				m_hDll[0], "CryptCATAdminEnumCatalogFromHash" );

			if (NULL == m_pCryptCATAdminEnumCatalogFromHash)
				goto clean0;
		}

		if (NULL == m_pWinVerifyTrust) {

			m_pWinVerifyTrust = (WINVERIFYTRUST)::GetProcAddress(
				m_hDll[0], "WinVerifyTrustEx" );

			if (NULL == m_pWinVerifyTrust)
				goto clean0;
		}

		if (NULL == m_pWTHelperProvDataFromStateData) {

			m_pWTHelperProvDataFromStateData = (WTHELPERPROVDATAFROMSTATEDATA)::GetProcAddress(
				m_hDll[0], "WTHelperProvDataFromStateData" );

			if (NULL == m_pWTHelperProvDataFromStateData)
				goto clean0;
		}

		if (NULL == m_pWTHelperGetProvSignerFromChain) {

			m_pWTHelperGetProvSignerFromChain = (WTHELPERGETPROVSIGNERFROMCHAIN)::GetProcAddress(
				m_hDll[0], "WTHelperGetProvSignerFromChain" );

			if (NULL == m_pWTHelperGetProvSignerFromChain)
				goto clean0;
		}

		if (NULL == m_pWTHelperGetProvCertFromChain) {

			m_pWTHelperGetProvCertFromChain = (WTHELPERGETPROVCERTFROMCHAIN)::GetProcAddress(
				m_hDll[0], "WTHelperGetProvCertFromChain" );

			if (NULL == m_pWTHelperGetProvCertFromChain)
				goto clean0;
		}

		if (NULL == m_pCertNameToStrW) {

			m_pCertNameToStrW = (CERTNAMETOSTRW)::GetProcAddress(
				m_hDll[1], "CertNameToStrW" );

			if (NULL == m_pCertNameToStrW)
				goto clean0;
		}

		retval = TRUE;

clean0:
		if (FALSE == retval) {

			m_pCryptCATAdminAcquireContext = NULL;
			m_pCryptCATAdminReleaseContext = NULL;
			m_pCryptCATAdminReleaseCatalogContext = NULL;
			m_pCryptCATCatalogInfoFromContext = NULL;
			m_pCryptCATAdminCalcHashFromFileHandle = NULL;
			m_pCryptCATAdminEnumCatalogFromHash = NULL;
			m_pWinVerifyTrust = NULL;
			m_pWTHelperProvDataFromStateData = NULL;
			m_pWTHelperGetProvSignerFromChain = NULL;
			m_pWTHelperGetProvCertFromChain = NULL;
			m_pCertNameToStrW = NULL;

			if (m_hDll[0]) {

				::FreeLibrary( m_hDll[0] );
				m_hDll[0] = NULL;
			}

			if (m_hDll[1]) {

				::FreeLibrary( m_hDll[1] );
				m_hDll[1] = NULL;
			}
		}

		return retval;
	}

	VOID UnInitialize() {

		m_pCryptCATAdminAcquireContext = NULL;
		m_pCryptCATAdminReleaseContext = NULL;
		m_pCryptCATAdminReleaseCatalogContext = NULL;
		m_pCryptCATCatalogInfoFromContext = NULL;
		m_pCryptCATAdminCalcHashFromFileHandle = NULL;
		m_pCryptCATAdminEnumCatalogFromHash = NULL;
		m_pWinVerifyTrust = NULL;
		m_pWTHelperProvDataFromStateData = NULL;
		m_pWTHelperGetProvSignerFromChain = NULL;
		m_pWTHelperGetProvCertFromChain = NULL;
		m_pCertNameToStrW = NULL;


		if (m_hDll[0]) {

			::FreeLibrary( m_hDll[0] );
			m_hDll[0] = NULL;
		}

		if (m_hDll[1]) {

			::FreeLibrary( m_hDll[1] );
			m_hDll[1] = NULL;
		}
	}

	DWORD VerifyEmbedSignA(LPCSTR FileFullPath) {

		DWORD retval = FALSE;
		PWCHAR wszFullPath = NULL;
		int nRetCode;
		int nStrLen;

		nStrLen = (int)strlen( FileFullPath );
		wszFullPath = new WCHAR[nStrLen + 1];

		if (NULL == wszFullPath)
			goto clean0;

		nRetCode = MultiByteToWideChar( CP_ACP,
			0,
			FileFullPath,
			-1,
			wszFullPath,
			nStrLen + 1 );

		if (nRetCode) {

			retval = VerifyEmbedSignW( wszFullPath );

		} else {

			retval = FALSE;
		}

clean0:
		if (wszFullPath) {

			delete[] wszFullPath;
			wszFullPath = NULL;
		}

		return retval;
	}

	BOOL VerifyEmbedSignW(LPCWSTR FileFullPath) {

		BOOL fRetval = FALSE;
		DWORD Err;
		WINTRUST_DATA WintrustData;
		WINTRUST_FILE_INFO WintrustFileInfo;
		GUID DriverVerifyGuid = DRIVER_ACTION_VERIFY;
		GUID WintrustVerifyGuid = WINTRUST_ACTION_GENERIC_VERIFY_V2;
		CRYPT_PROVIDER_DATA* pDroviderData = NULL;
		PWCHAR wszInfo = NULL;
		CRYPT_PROVIDER_SGNR *pCryptProviderSgnr = NULL;
		CRYPT_PROVIDER_CERT *pCert = NULL;

		if (NULL == m_pWinVerifyTrust) {

			return FALSE;
		}

		wszInfo = new WCHAR[1024];

		if (NULL == wszInfo)
			goto clean0;

		ZeroMemory( &WintrustData, sizeof(WINTRUST_DATA) );
		WintrustData.cbStruct = sizeof(WINTRUST_DATA);
		WintrustData.dwUIChoice = WTD_UI_NONE;
		WintrustData.fdwRevocationChecks = WTD_REVOKE_NONE;
		WintrustData.dwStateAction = WTD_STATEACTION_VERIFY;
		WintrustData.dwProvFlags = WTD_REVOCATION_CHECK_NONE;
		WintrustData.dwUnionChoice = WTD_CHOICE_FILE;
		WintrustData.pFile = &WintrustFileInfo;

		ZeroMemory(&WintrustFileInfo, sizeof(WINTRUST_FILE_INFO));
		WintrustFileInfo.cbStruct = sizeof(WINTRUST_FILE_INFO);
		WintrustFileInfo.pcwszFilePath = FileFullPath;

		Err = (DWORD)m_pWinVerifyTrust( NULL,
			&WintrustVerifyGuid,
			&WintrustData );

		if (Err != ERROR_SUCCESS) {

			goto clean0;
		}

		fRetval = IsTrustedStatData( WintrustData.hWVTStateData );

clean0:
		if (wszInfo) {

			delete[] wszInfo;
			wszInfo = NULL;
		}

		return fRetval;
	}

	BOOL IsTrustedStatData( HANDLE hWVTStateData )
	{
		CRYPT_PROVIDER_DATA* pDroviderData = m_pWTHelperProvDataFromStateData( hWVTStateData );

		if (NULL == pDroviderData) {
			return FALSE;
		}

		for (int i = 0; i < (int)pDroviderData->csSigners; ++i) {

			CRYPT_PROVIDER_SGNR *pCryptProviderSgnr = m_pWTHelperGetProvSignerFromChain(
				pDroviderData, 0, FALSE, 0 );

			if (NULL == pCryptProviderSgnr) {

				break;
			}

			for (int j = 0; j < (int)pCryptProviderSgnr->csCertChain; j++) {

				CRYPT_PROVIDER_CERT *pCert = m_pWTHelperGetProvCertFromChain(pCryptProviderSgnr, j);

				if (NULL == pCert) {

					break;
				}

				WCHAR wszInfo[1024] = {0};
				DWORD dwRetCode = m_pCertNameToStrW( X509_ASN_ENCODING, 
					&(pCert->pCert->pCertInfo->Subject),
					CERT_X500_NAME_STR | CERT_NAME_STR_REVERSE_FLAG,
					wszInfo,
					1024 );

				if (dwRetCode) {

					if (IsTrusted( wszInfo ))
						return TRUE;
				}
				return FALSE;
			}

		}
		return FALSE;
	}

	BOOL GetEmbedSignSubject(LPCSTR FileFullPath, vector<string>& subInfo)
	{
		BOOL retval = FALSE;
		PWCHAR wszFullPath = NULL;
		int nRetCode;
		int nStrLen;

		nStrLen = (int)strlen( FileFullPath );
		wszFullPath = new WCHAR[nStrLen + 1];

		if (NULL == wszFullPath)
			goto clean0;

		nRetCode = MultiByteToWideChar( CP_ACP,
			0,
			FileFullPath,
			-1,
			wszFullPath,
			nStrLen + 1 );

		if (nRetCode) {

			retval = GetEmbedSignSubject( wszFullPath , subInfo);

		} else {

			retval = FALSE;
		}
clean0:
		if (wszFullPath) {

			delete[] wszFullPath;
			wszFullPath = NULL;
		}

		return retval;
	}

	// 获取数字签名链的主题信息
	BOOL GetEmbedSignSubject(LPCWSTR FileFullPath, vector<string>& subInfo)
	{
		BOOL fRetval = FALSE;
		DWORD Err;
		WINTRUST_DATA WintrustData;
		WINTRUST_FILE_INFO WintrustFileInfo;
		GUID DriverVerifyGuid = DRIVER_ACTION_VERIFY;
		GUID WintrustVerifyGuid = WINTRUST_ACTION_GENERIC_VERIFY_V2;
		CRYPT_PROVIDER_DATA* pDroviderData = NULL;
		PWCHAR wszInfo = NULL;
		CRYPT_PROVIDER_SGNR *pCryptProviderSgnr = NULL;
		CRYPT_PROVIDER_CERT *pCert = NULL;

		if (NULL == m_pWinVerifyTrust) {

			goto clean0;
		}

		wszInfo = new WCHAR[1024];

		if (NULL == wszInfo)
			goto clean0;

		ZeroMemory( &WintrustData, sizeof(WINTRUST_DATA) );
		WintrustData.cbStruct = sizeof(WINTRUST_DATA);
		WintrustData.dwUIChoice = WTD_UI_NONE;
		WintrustData.fdwRevocationChecks = WTD_REVOKE_NONE;
		WintrustData.dwStateAction = WTD_STATEACTION_VERIFY;
		WintrustData.dwProvFlags = WTD_REVOCATION_CHECK_NONE;
		WintrustData.dwUnionChoice = WTD_CHOICE_FILE;
		WintrustData.pFile = &WintrustFileInfo;

		ZeroMemory(&WintrustFileInfo, sizeof(WINTRUST_FILE_INFO));
		WintrustFileInfo.cbStruct = sizeof(WINTRUST_FILE_INFO);
		WintrustFileInfo.pcwszFilePath = FileFullPath;

		Err = (DWORD)m_pWinVerifyTrust( NULL,
			&WintrustVerifyGuid,
			&WintrustData );

		if (Err != ERROR_SUCCESS) {

			goto clean0;
		}

		fRetval = GetProvDataFromTrustHandle( WintrustData.hWVTStateData, subInfo );

clean0:
		if (wszInfo) {

			delete[] wszInfo;
			wszInfo = NULL;
		}

		return fRetval;
	}

	BOOL GetProvDataFromTrustHandle( HANDLE hWVTStateData, vector<string>& subInfo )
	{
		BOOL bRet = FALSE;
		CRYPT_PROVIDER_DATA* pDroviderData = m_pWTHelperProvDataFromStateData( hWVTStateData );

		if (NULL == pDroviderData) {
			return bRet;
		}

		for (int i = 0; i < (int)pDroviderData->csSigners; ++i) 
		{

			CRYPT_PROVIDER_SGNR *pCryptProviderSgnr = m_pWTHelperGetProvSignerFromChain(
				pDroviderData, 0, FALSE, 0 );

			if (NULL == pCryptProviderSgnr) {

				break;
			}

			for (int j = 0; j < (int)pCryptProviderSgnr->csCertChain; j++) {

				CRYPT_PROVIDER_CERT *pCert = m_pWTHelperGetProvCertFromChain(pCryptProviderSgnr, j);

				if (NULL == pCert) {

					break;
				}

				WCHAR wszInfo[1024] = {0};
				DWORD dwRetCode = m_pCertNameToStrW( X509_ASN_ENCODING, 
					&(pCert->pCert->pCertInfo->Subject),
					CERT_X500_NAME_STR | CERT_NAME_STR_REVERSE_FLAG,
					wszInfo,
					1024 );
				//				_tprintf(wszInfo);
				if (dwRetCode) 
				{
					//if (IsTrusted( wszInfo ))
					{
						CHAR szInfo[1024] = {0};

						dwRetCode = WideCharToMultiByte( CP_ACP,
							0,
							wszInfo,
							-1,
							szInfo,
							1024,
							NULL,
							NULL );

						if (0 == dwRetCode)
						{
							subInfo.push_back(string("Error: Subject cannot change to multibyte!"));
							continue;
						}
						else
						{
							subInfo.push_back(string(szInfo));
							bRet = TRUE;
						}
					}
				}
			}

		}
		return bRet;
	}

	BOOL VerifyCatSignA(LPCSTR FileFullPath) {

		DWORD retval = FALSE;
		PWCHAR wszFullPath = NULL;
		int nRetCode;
		int nStrLen;

		nStrLen = (int)strlen( FileFullPath );
		wszFullPath = new WCHAR[nStrLen + 1];

		if (NULL == wszFullPath)
			goto clean0;

		nRetCode = MultiByteToWideChar( CP_ACP,
			0,
			FileFullPath,
			-1,
			wszFullPath,
			nStrLen + 1 );

		if (nRetCode) {

			retval = VerifyCatSignW( wszFullPath );

		} else {

			retval = FALSE;
		}

clean0:
		if (wszFullPath) {

			delete[] wszFullPath;
			wszFullPath = NULL;
		}

		return retval;
	}


	BOOL VerifyCatSignW(LPCWSTR FileFullPath) {

		BOOL fRetval = FALSE;
		LPBYTE Hash = NULL;
		DWORD HashSize;
		CATALOG_INFO CatInfo;
		HANDLE hFile = NULL;
		HCATADMIN hCatAdmin;
		HCATINFO hCatInfo;
		HCATINFO PrevCat;
		WINTRUST_DATA WintrustData;
		WINTRUST_CATALOG_INFO WintrustCatalogInfo;
		WCHAR UnicodeKey[MAX_PATH];
		GUID DriverVerifyGuid = DRIVER_ACTION_VERIFY;
		PWCHAR Key = NULL;

		if (NULL == m_pWinVerifyTrust) {

			return FALSE;
		}

		StringCchCopyW( UnicodeKey, MAX_PATH, FileFullPath );
		Key = MyGetFileTitle( UnicodeKey );

		if (!m_pCryptCATAdminAcquireContext( &hCatAdmin, &DriverVerifyGuid, 0 )) {

			goto clean0;
		}

		hFile = CreateFileW( FileFullPath,
			GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			0,
			NULL );

		if (INVALID_HANDLE_VALUE == hFile) {

			goto clean0;
		}

		ZeroMemory(&WintrustData, sizeof(WINTRUST_DATA));
		WintrustData.cbStruct = sizeof(WINTRUST_DATA);
		WintrustData.dwUIChoice = WTD_UI_NONE;
		WintrustData.fdwRevocationChecks = WTD_REVOKE_NONE;
		WintrustData.dwStateAction = WTD_STATEACTION_AUTO_CACHE;
		WintrustData.dwProvFlags = WTD_REVOCATION_CHECK_NONE;

		HashSize = 100;
		Hash = new BYTE[HashSize];

		if(NULL == Hash) {

			goto clean0;
		}

		if (!m_pCryptCATAdminCalcHashFromFileHandle( hFile, &HashSize, Hash, 0 )) {

			goto clean0;
		}

		WintrustData.dwUnionChoice = WTD_CHOICE_CATALOG;
		WintrustData.pCatalog = &WintrustCatalogInfo;

		ZeroMemory(&WintrustCatalogInfo, sizeof(WINTRUST_CATALOG_INFO));
		WintrustCatalogInfo.cbStruct = sizeof(WINTRUST_CATALOG_INFO);
		WintrustCatalogInfo.pbCalculatedFileHash = Hash;
		WintrustCatalogInfo.cbCalculatedFileHash = HashSize;

		StringCchCopyW( UnicodeKey, MAX_PATH, Key);
		CharLowerW( UnicodeKey );

		WintrustCatalogInfo.pcwszMemberTag = UnicodeKey;

		PrevCat = NULL;
		hCatInfo = m_pCryptCATAdminEnumCatalogFromHash(
			hCatAdmin,
			Hash,
			HashSize,
			0,
			&PrevCat );

		while (hCatInfo) {
			CatInfo.cbStruct = sizeof(CATALOG_INFO);
			if (m_pCryptCATCatalogInfoFromContext( hCatInfo, &CatInfo, 0 )) {

				// if (VerifyEmbedSignW( CatInfo.wszCatalogFile ))
				{
					// m_pCryptCATAdminReleaseCatalogContext( hCatAdmin, hCatInfo, 0 );
					fRetval = TRUE;
					break;
				}                
			}

			PrevCat = hCatInfo;

			hCatInfo = m_pCryptCATAdminEnumCatalogFromHash(
				hCatAdmin, Hash, HashSize, 0, &PrevCat );

		}

clean0:
		if (Hash) {

			delete[] Hash;
			Hash = NULL;
		}

		if (hCatAdmin) {

			m_pCryptCATAdminReleaseContext( hCatAdmin, 0 );
		}

		if (hFile) {

			::CloseHandle( hFile );
			hFile = NULL;
		}

		return fRetval;
	}

	BOOL GetCatSignSubject(LPCSTR FileFullPath, vector<string>& subInfo)
	{
		BOOL retval = FALSE;
		PWCHAR wszFullPath = NULL;
		int nRetCode;
		int nStrLen;

		nStrLen = (int)strlen( FileFullPath );
		wszFullPath = new WCHAR[nStrLen + 1];

		if (NULL == wszFullPath)
			goto clean0;

		nRetCode = MultiByteToWideChar( CP_ACP,
			0,
			FileFullPath,
			-1,
			wszFullPath,
			nStrLen + 1 );

		if (nRetCode) {

			retval = GetCatSignSubject( wszFullPath , subInfo);

		} else {

			retval = FALSE;
		}
clean0:
		if (wszFullPath) {

			delete[] wszFullPath;
			wszFullPath = NULL;
		}

		return retval;
	}

	BOOL GetCatSignSubject(LPCWSTR FileFullPath, vector<string>& subInfo)
	{
		BOOL fRetval = FALSE;
		LPBYTE Hash = NULL;
		DWORD HashSize;
		CATALOG_INFO CatInfo;
		HANDLE hFile = NULL;
		HCATADMIN hCatAdmin;
		HCATINFO hCatInfo;
		HCATINFO PrevCat;
		DWORD Err;
		WINTRUST_DATA WintrustData;
		WINTRUST_CATALOG_INFO WintrustCatalogInfo;
		LPWSTR CatalogFullPath;
		WCHAR UnicodeKey[MAX_PATH];
		GUID DriverVerifyGuid = DRIVER_ACTION_VERIFY;
		PWCHAR Key = NULL;

		if (NULL == m_pWinVerifyTrust) {

			return FALSE;
		}

		StringCchCopyW( UnicodeKey, MAX_PATH, FileFullPath );
		Key = MyGetFileTitle( UnicodeKey );

		if (!m_pCryptCATAdminAcquireContext( &hCatAdmin, &DriverVerifyGuid, 0 )) {

			goto clean0;
		}

		hFile = CreateFileW( FileFullPath,
			GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			0,
			NULL );

		if (INVALID_HANDLE_VALUE == hFile) {

			goto clean0;
		}

		ZeroMemory(&WintrustData, sizeof(WINTRUST_DATA));
		WintrustData.cbStruct = sizeof(WINTRUST_DATA);
		WintrustData.dwUIChoice = WTD_UI_NONE;
		WintrustData.fdwRevocationChecks = WTD_REVOKE_NONE;
		WintrustData.dwStateAction = WTD_STATEACTION_AUTO_CACHE;
		WintrustData.dwProvFlags = WTD_REVOCATION_CHECK_NONE;

		HashSize = 100;
		Hash = new BYTE[HashSize];

		if(NULL == Hash) {

			goto clean0;
		}

		if (!m_pCryptCATAdminCalcHashFromFileHandle( hFile, &HashSize, Hash, 0 )) {

			goto clean0;
		}

		WintrustData.dwUnionChoice = WTD_CHOICE_CATALOG;
		WintrustData.pCatalog = &WintrustCatalogInfo;

		ZeroMemory(&WintrustCatalogInfo, sizeof(WINTRUST_CATALOG_INFO));
		WintrustCatalogInfo.cbStruct = sizeof(WINTRUST_CATALOG_INFO);
		WintrustCatalogInfo.pbCalculatedFileHash = Hash;
		WintrustCatalogInfo.cbCalculatedFileHash = HashSize;

		StringCchCopyW( UnicodeKey, MAX_PATH, Key);
		CharLowerW( UnicodeKey );

		WintrustCatalogInfo.pcwszMemberTag = UnicodeKey;

		PrevCat = NULL;
		hCatInfo = m_pCryptCATAdminEnumCatalogFromHash(
			hCatAdmin,
			Hash,
			HashSize,
			0,
			&PrevCat );

		while (hCatInfo) {
			CatInfo.cbStruct = sizeof(CATALOG_INFO);
			if (m_pCryptCATCatalogInfoFromContext( hCatInfo, &CatInfo, 0 )) {

				CatalogFullPath = CatInfo.wszCatalogFile;
				WintrustCatalogInfo.pcwszCatalogFilePath = CatInfo.wszCatalogFile;

				Err = (DWORD)m_pWinVerifyTrust(NULL,
					&DriverVerifyGuid,
					&WintrustData
					);

				if (NO_ERROR  == Err) {

					if (GetEmbedSignSubject(CatInfo.wszCatalogFile, subInfo))
					{
						CHAR szInfo[1024] = {0};

						WideCharToMultiByte( CP_ACP,
							0,
							CatInfo.wszCatalogFile,
							-1,
							szInfo,
							1024,
							NULL,
							NULL );
						subInfo.push_back(string(szInfo));
						// m_pCryptCATAdminReleaseCatalogContext( hCatAdmin, hCatInfo, 0 );

						fRetval = TRUE;
						break;
					}
				}
			}

			PrevCat = hCatInfo;

			hCatInfo = m_pCryptCATAdminEnumCatalogFromHash(
				hCatAdmin, Hash, HashSize, 0, &PrevCat );

		}

clean0:
		if (Hash) {

			delete[] Hash;
			Hash = NULL;
		}

		if (hCatAdmin) {

			m_pCryptCATAdminReleaseContext( hCatAdmin, 0 );
		}

		if (hFile) {

			::CloseHandle( hFile );
			hFile = NULL;
		}

		return fRetval;
	}

	void AddTrustModule(LPCWSTR lpName)
	{
		USES_CONVERSION;
		AddTrustModule(W2A(lpName));
	}

	void AddTrustModule(LPCSTR lpName) {

		std::vector<std::string>::const_iterator it;
		BOOL fExist = FALSE;

		LPSTR str = new char[strlen(lpName) + 1];
		strcpy(str, lpName);
		_strlwr(str);

		for (it = m_trustNames.begin(); it != m_trustNames.end(); ++it) {

			if (stricmp( str, it->c_str() ) == 0) {

				fExist = TRUE;
				break;
			}
		}

		if (FALSE == fExist) {

			m_trustNames.push_back( str );
		}
		delete [] str;
	}


	BOOL GetSignData(LPCTSTR szPath, BOOL& bMircoSoft, CStringA& strSign)
	{
		std::vector<std::string> vsign;

		strSign.Empty();
		bMircoSoft = FALSE;

		CSignVerify::Instance().GetEmbedSignSubject(szPath , vsign);

		if (vsign.size() <= 0)
		{

			CSignVerify::Instance().GetCatSignSubject(szPath,vsign);

			if (vsign.size() > 0)
			{
				bMircoSoft = TRUE;
			}
		}

		if (vsign.size() > 0)
		{
			CStringA strTemp = vsign[0].c_str();
			CStringA strTemp2;

			int nStart = strTemp.Find("CN=");
			if (nStart == -1) return FALSE;

			nStart += 3;

			int nEnd = strTemp.Find("=", nStart);
			if (nEnd == -1)
				strSign = strTemp.Mid(nStart, strTemp.GetLength());
			else
			{
				strTemp2 = strTemp.Mid(nStart, nEnd - nStart);
				nEnd = strTemp2.ReverseFind(',');
				if (nEnd != -1)
					strSign = strTemp2.Mid(0, nEnd);
			}
			strSign.Remove('"');
		}

		return vsign.size() > 0;
	}

private:
	CSignVerify()
		: m_pCryptCATAdminAcquireContext( NULL )
		, m_pCryptCATAdminReleaseContext( NULL )
		, m_pCryptCATAdminReleaseCatalogContext( NULL )
		, m_pCryptCATCatalogInfoFromContext( NULL )
		, m_pCryptCATAdminCalcHashFromFileHandle( NULL )
		, m_pCryptCATAdminEnumCatalogFromHash( NULL )
		, m_pWinVerifyTrust( NULL )
		, m_pWTHelperProvDataFromStateData( NULL )
		, m_pWTHelperGetProvSignerFromChain( NULL )
		, m_pWTHelperGetProvCertFromChain( NULL )
		, m_pCertNameToStrW( NULL )
	{
		m_hDll[0] = m_hDll[1] = NULL;
	}

	~CSignVerify() {

	}

	PWSTR MyGetFileTitle(PWSTR FilePath) {

		PWSTR LastComponent = FilePath;
		WCHAR  CurChar;

		while ((CurChar = *FilePath) == true) {

			FilePath = CharNextW( FilePath );
			if ((CurChar == TEXT('\\')) || (CurChar == TEXT('/')) || (CurChar == TEXT(':'))) {

				LastComponent = FilePath;
			}
		}

		return LastComponent;
	}

	BOOL CompareStr(LPCSTR s1, LPCSTR s2)
	{
		while ( (*s1) && (*s2) )
		{
			if (*s1 != *s2)
				return FALSE;
			s1++;
			s2++;
		}
		return TRUE;
	}

	BOOL IsTrusted(PWCHAR wszInfo) {

		BOOL fRetval = FALSE;
		PCHAR szInfo = NULL;
		int nRetCode;
		int i;

		int nlen = wcslen(wszInfo);

		szInfo = new CHAR[nlen + 2];
		memset(szInfo, 0,nlen + 2);

		if (NULL == szInfo)
			goto clean0;

		nRetCode = WideCharToMultiByte( CP_ACP,
			0,
			wszInfo,
			-1,
			szInfo,
			nlen+1,
			NULL,
			NULL );
		if (0 == nRetCode)
			goto clean0;

		_strlwr(szInfo);

		for (i = 0; i < (int)m_trustNames.size(); ++i) {
			const char* p = m_trustNames[i].c_str();
			if ( (stricmp( szInfo, p ) == 0) ||
				CompareStr( szInfo, p) )
			{
				fRetval = TRUE;
				goto clean0;
			}
		}

clean0:
		if (szInfo) {

			delete[] szInfo;
			szInfo = NULL;
		}

		return fRetval;
	}

	HMODULE m_hDll[2];

	CRYPTCATADMINACQUIRECONTEXT m_pCryptCATAdminAcquireContext;
	CRYPTCATADMINRELEASECONTEXT m_pCryptCATAdminReleaseContext;
	CRYPTCATADMINRELEASECATALOGCONTEXT m_pCryptCATAdminReleaseCatalogContext;
	CRYPTCATCATALOGINFOFROMCONTEXT m_pCryptCATCatalogInfoFromContext;
	CRYPTCATADMINCALCHASHFROMFILEHANDLE m_pCryptCATAdminCalcHashFromFileHandle;
	CRYPTCATADMINENUMCATALOGFROMHASH m_pCryptCATAdminEnumCatalogFromHash;
	WINVERIFYTRUST m_pWinVerifyTrust;
	WTHELPERPROVDATAFROMSTATEDATA m_pWTHelperProvDataFromStateData;
	WTHELPERGETPROVSIGNERFROMCHAIN m_pWTHelperGetProvSignerFromChain;
	WTHELPERGETPROVCERTFROMCHAIN m_pWTHelperGetProvCertFromChain;
	CERTNAMETOSTRW m_pCertNameToStrW;

	std::vector<std::string> m_trustNames;
};

#endif  // SIGNVERIFY_INC_