///////////////////////////////////////////////////////////////////////////////
// Name:        codec.cpp
// Purpose:     
// Author:      Ulrich Telle
// Modified by:
// Created:     2006-12-06
// RCS-ID:      $$
// Copyright:   (c) Ulrich Telle
// Licence:     wxWindows licence + RSA Data Security license
///////////////////////////////////////////////////////////////////////////////

/// \file codec.cpp Implementation of MD5, RC4 and AES algorithms

/*
 **********************************************************************
 ** Copyright (C) 1990, RSA Data Security, Inc. All rights reserved. **
 **                                                                  **
 ** License to copy and use this software is granted provided that   **
 ** it is identified as the "RSA Data Security, Inc. MD5 Message     **
 ** Digest Algorithm" in all material mentioning or referencing this **
 ** software or this function.                                       **
 **                                                                  **
 ** License is also granted to make and use derivative works         **
 ** provided that such works are identified as "derived from the RSA **
 ** Data Security, Inc. MD5 Message Digest Algorithm" in all         **
 ** material mentioning or referencing the derived work.             **
 **                                                                  **
 ** RSA Data Security, Inc. makes no representations concerning      **
 ** either the merchantability of this software or the suitability   **
 ** of this software for any particular purpose.  It is provided "as **
 ** is" without express or implied warranty of any kind.             **
 **                                                                  **
 ** These notices must be retained in any copies of any part of this **
 ** documentation and/or software.                                   **
 **********************************************************************
 */

#include "codec.h"
#include "enc_md5.h"

//cli_md5_ctx ctx;
//cli_md5_init(&ctx);
//cli_md5_update(&ctx, data, length);
//cli_md5_final(digest,&ctx);
#define cli_md5_ctx md5_state_t
#define cli_md5_init(c)         md5_init(c)
#define cli_md5_update(c, d, l) md5_append(c, d, l)
#define cli_md5_final(dg, c)    md5_finish(c, dg)


/// Generate encryption key
void CodecGenerateEncryptionKey(Codec* codec, const char* userPassword, int passwordLength, unsigned char encryptionKey[KEYLENGTH]);

void CodecPadPassword(Codec* codec, const char* password, int pswdlen, unsigned char pswd[32]);

void CodecRC4(unsigned char* key, int keylen,
         unsigned char* textin, int textlen,
         unsigned char* textout);

void CodecGetMD5Binary(const unsigned char* data, int length, unsigned char* digest);

void CodecGenerateInitialVector(int seed, unsigned char iv[16]);

void CodecAES(Codec* codec, int page, BOOL encrypt, unsigned char encryptionKey[KEYLENGTH],
         unsigned char* datain, int datalen, unsigned char* dataout);


#define MD5_HASHBYTES 16


// ---------------------------
// RC4 implementation
// ---------------------------

/**
* RC4 is the standard encryption algorithm used in PDF format
*/

void
CodecRC4(unsigned char* key, int keylen,
           unsigned char* textin, int textlen,
           unsigned char* textout)
{
  int i;
  int j;
  int t;
  unsigned char rc4[256];
  int a = 0;
  int b = 0;
  unsigned char k;

  for (i = 0; i < 256; i++)
  {
    rc4[i] = i;
  }
  j = 0;
  for (i = 0; i < 256; i++)
  {
    t = rc4[i];
    j = (j + t + key[i % keylen]) % 256;
    rc4[i] = rc4[j];
    rc4[j] = t;
  }

  for (i = 0; i < textlen; i++)
  {
    a = (a + 1) % 256;
    t = rc4[a];
    b = (b + t) % 256;
    rc4[a] = rc4[b];
    rc4[b] = t;
    k = rc4[(rc4[a] + rc4[b]) % 256];
    textout[i] = textin[i] ^ k;
  }
}

void
CodecGetMD5Binary(const unsigned char* data, int length, unsigned char* digest)
{
  cli_md5_ctx ctx;
  cli_md5_init(&ctx);
  cli_md5_update(&ctx, data, length);
  cli_md5_final(digest,&ctx);
}

#define MODMULT(a, b, c, m, s) q = s / a; s = b * (s - a * q) - c * q; if (s < 0) s += m

void
CodecGenerateInitialVector(int seed, unsigned char iv[16])
{
  unsigned char initkey[MD5_HASHBYTES];
  int j, q;
  int z = seed + 1;
  for (j = 0; j < 4; j++)
  {
    MODMULT(52774, 40692,  3791, 2147483399L, z);
    initkey[4*j+0] = 0xff &  z;
    initkey[4*j+1] = 0xff & (z >>  8);
    initkey[4*j+2] = 0xff & (z >> 16);
    initkey[4*j+3] = 0xff & (z >> 24);
  }
  CodecGetMD5Binary((const unsigned char*) initkey, 16, iv);
}

void
CodecAES(Codec* codec, int page, BOOL encrypt, unsigned char encryptionKey[MD5_HASHBYTES],
           unsigned char* datain, int datalen, unsigned char* dataout)
{
  unsigned char initial[MD5_HASHBYTES];
  unsigned char pagekey[MD5_HASHBYTES];
  unsigned char nkey[MD5_HASHBYTES+4+4];
  int keyLength = MD5_HASHBYTES;
  int nkeylen = keyLength + 4 + 4;
  int j;
  EM_Direction direction;
  int len = 0;

  for (j = 0; j < keyLength; j++)
  {
    nkey[j] = encryptionKey[j];
  }
  nkey[keyLength+0] = 0xff &  page;
  nkey[keyLength+1] = 0xff & (page >>  8);
  nkey[keyLength+2] = 0xff & (page >> 16);
  nkey[keyLength+3] = 0xff & (page >> 24);

  // AES encryption needs some 'salt'
  nkey[keyLength+4] = 0x73;
  nkey[keyLength+5] = 0x41;
  nkey[keyLength+6] = 0x6c;
  nkey[keyLength+7] = 0x54;

  CodecGetMD5Binary(nkey, nkeylen, pagekey);
  CodecGenerateInitialVector(page, initial);

  direction = (encrypt) ? em_Rijndael_Encrypt : em_Rijndael_Decrypt;
  RijndaelInit(&codec->m_aes, em_Rijndael_CBC, direction, pagekey, em_Rijndael_Key16Bytes, initial);
  if (encrypt)
  {
    len = RijndaelBlockEncrypt(&codec->m_aes, datain, datalen*8, dataout);
  }
  else
  {
    len = RijndaelBlockDecrypt(&codec->m_aes, datain, datalen*8, dataout);
  }
  
  // It is a good idea to check the error code
  if (len < 0)
  {
    // AES: Error on encrypting.
  }
}

static unsigned char padding[] =
  "\x28\xBF\x4E\x5E\x4E\x75\x8A\x41\x64\x00\x4E\x56\xFF\xFA\x01\x08\x2E\x2E\x00\xB6\xD0\x68\x3E\x80\x2F\x0C\xA9\xFE\x64\x53\x69\x7A";


void
CodecCopy(Codec* codec, Codec* other)
{
  int j;
  codec->m_isEncrypted = other->m_isEncrypted;
  codec->m_hasReadKey  = other->m_hasReadKey;
  codec->m_hasWriteKey = other->m_hasWriteKey;
  for (j = 0; j < 16; j++)
  {
    codec->m_readKey[j]  = other->m_readKey[j];
    codec->m_writeKey[j] = other->m_writeKey[j];
  }
  codec->m_bt = other->m_bt;
  RijndaelInvalidate(&codec->m_aes);
}

void
CodecCopyKey(Codec* codec, BOOL read2write)
{
  int j;
  if (read2write)
  {
    for (j = 0; j < 16; j++)
    {
      codec->m_writeKey[j] = codec->m_readKey[j];
    }
  }
  else
  {
    for (j = 0; j < 16; j++)
    {
      codec->m_readKey[j] = codec->m_writeKey[j];
    }
  }
}

void
CodecPadPassword(Codec* codec, const char* password, int pswdlen, unsigned char pswd[32])
{
  int m = pswdlen;
  int j;
  int p = 0;

  if (m > 32) m = 32;


  for (j = 0; j < m; j++)
  {
    pswd[p++] = (unsigned char) password[j];
  }
  for (j = 0; p < 32 && j < 32; j++)
  {
    pswd[p++] = padding[j];
  }
}

void
CodecGenerateReadKey(Codec* codec, const char* userPassword, int passwordLength)
{
  CodecGenerateEncryptionKey(codec, userPassword, passwordLength, codec->m_readKey);
}

void
CodecGenerateWriteKey(Codec* codec, const char* userPassword, int passwordLength)
{
  CodecGenerateEncryptionKey(codec, userPassword, passwordLength, codec->m_writeKey);
}

void
CodecGenerateEncryptionKey(Codec* codec, const char* userPassword, int passwordLength, unsigned char encryptionKey[KEYLENGTH])
{
  unsigned char userPad[32];
  unsigned char ownerPad[32];
  unsigned char ownerKey[32];

  unsigned char mkey[MD5_HASHBYTES];
  unsigned char digest[MD5_HASHBYTES];
  int keyLength = MD5_HASHBYTES;
  int i, j, k;
  cli_md5_ctx ctx;

  // Pad passwords
  CodecPadPassword(codec, userPassword, passwordLength, userPad);
  CodecPadPassword(codec, "", 0, ownerPad);

  // Compute owner key


  cli_md5_init(&ctx);
  cli_md5_update(&ctx, ownerPad, 32);
  cli_md5_final(digest,&ctx);

  // only use for the input as many bit as the key consists of
  for (k = 0; k < 50; ++k)
  {
    cli_md5_init(&ctx);
    cli_md5_update(&ctx, digest, keyLength);
    cli_md5_final(digest,&ctx);
  }
  memcpy(ownerKey, userPad, 32);
  for (i = 0; i < 20; ++i)
  {
    for (j = 0; j < keyLength ; ++j)
    {
      mkey[j] = (digest[j] ^ i);
    }
    CodecRC4(mkey, keyLength, ownerKey, 32, ownerKey);
  }

  // Compute encryption key

  cli_md5_init(&ctx);
  cli_md5_update(&ctx, userPad, 32);
  cli_md5_update(&ctx, ownerKey, 32);
  cli_md5_final(digest,&ctx);

  // only use the really needed bits as input for the hash
  for (k = 0; k < 50; ++k)
  {
    cli_md5_init(&ctx);
    cli_md5_update(&ctx, digest, keyLength);
    cli_md5_final(digest, &ctx);
  }
  memcpy(encryptionKey, digest, keyLength);
}

void
CodecEncrypt(Codec* codec, int page, unsigned char* data, int len, BOOL useWriteKey)
{
  unsigned char* key = (useWriteKey) ? codec->m_writeKey : codec->m_readKey;
  CodecAES(codec, page, TRUE, key, data, len, data);

}

void
CodecDecrypt(Codec* codec, int page, unsigned char* data, int len)
{
  CodecAES(codec, page, FALSE, codec->m_readKey, data, len, data);
}

