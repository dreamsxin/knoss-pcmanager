///////////////////////////////////////////////////////////////////////////////
// Name:        rijndael.h
// Purpose:     
// Author:      Ulrich Telle
// Modified by:
// Created:     2006-12-06
// Copyright:   (c) Ulrich Telle (Copyright for original code see below)
// Licence:     wxWindows licence
//
// The original code is unchanged
///////////////////////////////////////////////////////////////////////////////

/// \file rijndael.h Interface of the Rijndael cipher

#ifndef _RIJNDAEL_H_
#define _RIJNDAEL_H_
#include <windows.h>
// File : rijndael.h
// Creation date : Sun Nov 5 2000 03:21:05 CEST
// Author : Szymon Stefanek (stefanek@tin.it)
//
// Another implementation of the Rijndael cipher.
// This is intended to be an easily usable library file.
// This code is public domain.
// Based on the Vincent Rijmen and K.U.Leuven implementation 2.4.
//
// Original Copyright notice:
//
//    rijndael-alg-fst.c   v2.4   April '2000
//    rijndael-alg-fst.h
//    rijndael-api-fst.c
//    rijndael-api-fst.h
//
//    Optimised ANSI C code
//
//    authors: v1.0: Antoon Bosselaers
//             v2.0: Vincent Rijmen, K.U.Leuven
//             v2.3: Paulo Barreto
//             v2.4: Vincent Rijmen, K.U.Leuven
//
//    This code is placed in the public domain.
//

//
// This implementation works on 128 , 192 , 256 bit keys
// and on 128 bit blocks
//

//
// Example of usage:
//
//  // Input data
//  unsigned char key[32];                       // The key
//  initializeYour256BitKey();                   // Obviously initialized with sth
//  const unsigned char * plainText = getYourPlainText(); // Your plain text
//  int plainTextLen = strlen(plainText);        // Plain text length
//
//  // Encrypting
//  Rijndael rin;
//  unsigned char output[plainTextLen + 16];
//
//  rin.init(enum EM_CBC,enum EM_Encrypt,key,enum EM_Key32Bytes);
//  // It is a good idea to check the error code
//  int len = rin.padEncrypt(plainText,len,output);
//  if(len >= 0)useYourEncryptedText();
//  else encryptError(len);
//
//  // Decrypting: we can reuse the same object
//  unsigned char output2[len];
//  rin.init(enum EM_CBC,enum EM_Decrypt,key,enum EM_Key32Bytes));
//  len = rin.padDecrypt(output,len,output2);
//  if(len >= 0)useYourDecryptedText();
//  else decryptError(len);
//

#define _MAX_KEY_COLUMNS (256/32)
#define _MAX_ROUNDS      14
#define MAX_IV_SIZE      16

// We assume that unsigned int is 32 bits long.... 
typedef unsigned char  UINT8;
typedef unsigned int   UINT32;
typedef unsigned short UINT16;

// Error codes
#define RIJNDAEL_SUCCESS 0
#define RIJNDAEL_UNSUPPORTED_MODE -1
#define RIJNDAEL_UNSUPPORTED_DIRECTION -2
#define RIJNDAEL_UNSUPPORTED_KEY_LENGTH -3
#define RIJNDAEL_BAD_KEY -4
#define RIJNDAEL_NOT_INITIALIZED -5
#define RIJNDAEL_BAD_DIRECTION -6
#define RIJNDAEL_CORRUPTED_DATA -7

typedef enum EM_Direction { em_Rijndael_Encrypt , em_Rijndael_Decrypt } EM_Direction; 
typedef enum EM_Mode { em_Rijndael_ECB , em_Rijndael_CBC , em_Rijndael_CFB1 } EM_Mode;
typedef enum EM_KeyLength { em_Rijndael_Key16Bytes , em_Rijndael_Key24Bytes , em_Rijndael_Key32Bytes } EM_KeyLength;
typedef enum EM_State { em_Rijndael_Valid , em_Rijndael_Invalid } EM_State;

/// Class implementing the Rijndael cipher. (For internal use only)
typedef struct Rijndael
{
    enum EM_State       m_state;
    enum EM_Mode        m_mode;
    enum EM_Direction   m_direction;
    UINT8               m_initVector[MAX_IV_SIZE];
    UINT32              m_uRounds;
    UINT8               m_expandedKey[_MAX_ROUNDS+1][4][4];
} Rijndael;


//////////////////////////////////////////////////////////////////////////////////////////
// API
//////////////////////////////////////////////////////////////////////////////////////////

// init(): Initializes the crypt session
// Returns RIJNDAEL_SUCCESS or an error code
// mode      : enum EM_ECB, enum EM_CBC or enum EM_CFB1
//             You have to use the same mode for encrypting and decrypting
// dir       : enum EM_Encrypt or enum EM_Decrypt
//             A cipher instance works only in one direction
//             (Well , it could be easily modified to work in both
//             directions with a single init() call, but it looks
//             useless to me...anyway , it is a matter of generating
//             two expanded keys)
// key       : array of unsigned octets , it can be 16 , 24 or 32 bytes long
//             this CAN be binary data (it is not expected to be null terminated)
// keyLen    : enum EM_Key16Bytes , enum EM_Key24Bytes or enum EM_Key32Bytes
// initVector: initialization vector, you will usually use 0 here
int RijndaelInit(struct Rijndael* ctx, EM_Mode mode, EM_Direction dir, const UINT8 *key, EM_KeyLength keyLen, UINT8 * initVector);
// Encrypts the input array (can be binary data)
// The input array length must be a multiple of 16 bytes, the remaining part
// is DISCARDED.
// so it actually encrypts inputLen / 128 blocks of input and puts it in outBuffer
// Input len is in BITS!
// outBuffer must be at least inputLen / 8 bytes long.
// Returns the encrypted buffer length in BITS or an error code < 0 in case of error
int RijndaelBlockEncrypt(Rijndael* ctx, const UINT8 *input, int inputLen, UINT8 *outBuffer);
// Encrypts the input array (can be binary data)
// The input array can be any length , it is automatically padded on a 16 byte boundary.
// Input len is in BYTES!
// outBuffer must be at least (inputLen + 16) bytes long
// Returns the encrypted buffer length in BYTES or an error code < 0 in case of error
int RijndaelPadEncrypt(Rijndael* ctx, const UINT8 *input, int inputOctets, UINT8 *outBuffer);
// Decrypts the input vector
// Input len is in BITS!
// outBuffer must be at least inputLen / 8 bytes long
// Returns the decrypted buffer length in BITS and an error code < 0 in case of error
int RijndaelBlockDecrypt(Rijndael* ctx, const UINT8 *input, int inputLen, UINT8 *outBuffer);
// Decrypts the input vector
// Input len is in BYTES!
// outBuffer must be at least inputLen bytes long
// Returns the decrypted buffer length in BYTES and an error code < 0 in case of error
int RijndaelPadDecrypt(Rijndael* ctx, const UINT8 *input, int inputOctets, UINT8 *outBuffer);

void RijndaelInvalidate(Rijndael* ctx);

void RijndaelKeySched(Rijndael* ctx, UINT8 key[_MAX_KEY_COLUMNS][4]);
void RijndaelKeyEncToDec(Rijndael* ctx);
void RijndaelEncrypt(Rijndael* ctx, const UINT8 a[16], UINT8 b[16]);
void RijndaelDecrypt(Rijndael* ctx, const UINT8 a[16], UINT8 b[16]);
	
#endif // _RIJNDAEL_H_
