/*
 * Copyright (c) 2004,金山KSG
 * All rights reserved.
 * 
 * 文件名称: mysocket.h
 * 文件标识:
 * 摘    要: 用于md5实现
 * 当前版本: 1.0
 * 作    者: chen.qian.jiang
 * 开始日期: 2004-08-13
 * 完成日期: 
 * 其它说明： 
 * 修改日期        版本号     修改人          修改内容
 * -----------------------------------------------
 * 2004/08/13      V1.0       陈黔江          创建
 */ 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "my_md5.h"

/* MD5 context. */
typedef struct {
  UINT4 iState[4];   /* state (ABCD) */
  UINT4 iCount[2];        /* number of bits, modulo 2^64 (lsb first) */
  unsigned char cBuffer[64];                         /* input buffer */
} MD5_CTX;


static unsigned char cPADDING[64];
static void MD5Init    ( MD5_CTX *context );   
static void MD5Update  (MD5_CTX *context , unsigned char * input, int len);
static void MD5Final   (MD5_CTX *context ,unsigned char[16]);
static void MD5Transform(UINT4[4], unsigned char[64]);
static void Encode(unsigned char * output, UINT4 * input, int len);
static void Decode(UINT4 *output, unsigned char * input, unsigned int);


static unsigned char cPADDING[64] = {
  0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

void Md5_calc (unsigned char *output, unsigned char * input,int  ilen)
{
	MD5_CTX context;
	memset(&context, 0, sizeof(MD5_CTX));
	MD5Init( &context );
	MD5Update( &context ,input,ilen);
	MD5Final ( &context , output); 
}

static  void MD5Init( MD5_CTX *context )
{
  context->iCount[0] = context->iCount[1] = 0;
  /** Load magic initialization constants.  */
  context->iState[0] = 0x67452301;
  context->iState[1] = 0xefcdab89;
  context->iState[2] = 0x98badcfe;
  context->iState[3] = 0x10325476;
}

static void MD5Update( MD5_CTX *context ,unsigned char * input,int inputLen )
{
  unsigned int    i, index, partLen;
  
    /* Compute number of bytes mod 64 */
    index = (unsigned int) ((context->iCount[0] >> 3) & 0x3F);

    /* Update number of bits */
   if ((context->iCount[0] += ((UINT4) inputLen << 3))
                     < ((UINT4) inputLen << 3))
                context->iCount[1]++;
        context->iCount[1] += ((UINT4) inputLen >> 29);

        partLen = 64 - index;

        /*
         * Transform as many times as possible.
         */
        if (inputLen >= partLen)
        {
                memcpy((unsigned char *)& context->cBuffer[index], (unsigned char * ) input, partLen);
                MD5Transform (context->iState, context->cBuffer);

                for (i = partLen; i + 63 < inputLen; i += 64)
                        MD5Transform (context->iState, &input[i]);

                index = 0;
        }
        else
                i = 0;
       
        /* Buffer remaining input */
        memcpy((unsigned char *) & context->cBuffer[index], (unsigned char *) & input[i],
                 inputLen - i);
}

 
static void MD5Final( MD5_CTX *context ,unsigned char digest[16] ) 
{
   unsigned char   bits[8];
        unsigned int    index,
                        padLen;

        /* Save number of bits */
        Encode (bits, context->iCount, 8);

        /*
         * Pad out to 56 mod 64.
         */
        index = (unsigned int) ((context->iCount[0] >> 3) & 0x3f);
        padLen = (index < 56) ? (56 - index) : (120 - index);
        MD5Update ( context,cPADDING, padLen);

        /* Append length (before padding) */
        MD5Update (context , bits, 8);

        /* Store context->iState in digest */
        Encode (digest, context->iState, 16);

        /*
         * Zeroize sensitive information.
         */
        memset ((unsigned char * ) &context->iState, 0, sizeof (context->iState));
        memset ((unsigned char * ) &context->iCount, 0, sizeof (context->iCount));
        memset ((unsigned char * ) context->cBuffer, 0, sizeof (context->cBuffer));
}

static void MD5Transform(UINT4 iState[4], unsigned char block[64]) 
{
      UINT4           a = iState[0],
                        b = iState[1],
                        c = iState[2],
                        d = iState[3],
                        x[16];

        Decode (x, block, 64);

        /* Round 1 */
        FF (a, b, c, d, x[0], S11, 0xd76aa478); /* 1 */
        FF (d, a, b, c, x[1], S12, 0xe8c7b756); /* 2 */
        FF (c, d, a, b, x[2], S13, 0x242070db); /* 3 */
        FF (b, c, d, a, x[3], S14, 0xc1bdceee); /* 4 */
        FF (a, b, c, d, x[4], S11, 0xf57c0faf); /* 5 */
        FF (d, a, b, c, x[5], S12, 0x4787c62a); /* 6 */
        FF (c, d, a, b, x[6], S13, 0xa8304613); /* 7 */
        FF (b, c, d, a, x[7], S14, 0xfd469501); /* 8 */
        FF (a, b, c, d, x[8], S11, 0x698098d8); /* 9 */
        FF (d, a, b, c, x[9], S12, 0x8b44f7af); /* 10 */
        FF (c, d, a, b, x[10], S13, 0xffff5bb1);        /* 11 */
        FF (b, c, d, a, x[11], S14, 0x895cd7be);        /* 12 */
        FF (a, b, c, d, x[12], S11, 0x6b901122);        /* 13 */
        FF (d, a, b, c, x[13], S12, 0xfd987193);        /* 14 */
        FF (c, d, a, b, x[14], S13, 0xa679438e);        /* 15 */
        FF (b, c, d, a, x[15], S14, 0x49b40821);        /* 16 */
        /* Round 2 */
        GG (a, b, c, d, x[1], S21, 0xf61e2562); /* 17 */
        GG (d, a, b, c, x[6], S22, 0xc040b340); /* 18 */
        GG (c, d, a, b, x[11], S23, 0x265e5a51);        /* 19 */
        GG (b, c, d, a, x[0], S24, 0xe9b6c7aa); /* 20 */
        GG (a, b, c, d, x[5], S21, 0xd62f105d); /* 21 */
        GG (d, a, b, c, x[10], S22, 0x2441453); /* 22 */
        GG (c, d, a, b, x[15], S23, 0xd8a1e681);        /* 23 */
        GG (b, c, d, a, x[4], S24, 0xe7d3fbc8); /* 24 */
        GG (a, b, c, d, x[9], S21, 0x21e1cde6); /* 25 */
        GG (d, a, b, c, x[14], S22, 0xc33707d6);        /* 26 */
        GG (c, d, a, b, x[3], S23, 0xf4d50d87); /* 27 */
        GG (b, c, d, a, x[8], S24, 0x455a14ed); /* 28 */
        GG (a, b, c, d, x[13], S21, 0xa9e3e905);        /* 29 */
        GG (d, a, b, c, x[2], S22, 0xfcefa3f8); /* 30 */
        GG (c, d, a, b, x[7], S23, 0x676f02d9); /* 31 */
        GG (b, c, d, a, x[12], S24, 0x8d2a4c8a);        /* 32 */
         /* Round 3 */
        HH (a, b, c, d, x[5], S31, 0xfffa3942); /* 33 */
        HH (d, a, b, c, x[8], S32, 0x8771f681); /* 34 */
        HH (c, d, a, b, x[11], S33, 0x6d9d6122);        /* 35 */
        HH (b, c, d, a, x[14], S34, 0xfde5380c);        /* 36 */
        HH (a, b, c, d, x[1], S31, 0xa4beea44); /* 37 */
        HH (d, a, b, c, x[4], S32, 0x4bdecfa9); /* 38 */
        HH (c, d, a, b, x[7], S33, 0xf6bb4b60); /* 39 */
        HH (b, c, d, a, x[10], S34, 0xbebfbc70);        /* 40 */
        HH (a, b, c, d, x[13], S31, 0x289b7ec6);        /* 41 */
        HH (d, a, b, c, x[0], S32, 0xeaa127fa); /* 42 */
        HH (c, d, a, b, x[3], S33, 0xd4ef3085); /* 43 */
        HH (b, c, d, a, x[6], S34, 0x4881d05);  /* 44 */
        HH (a, b, c, d, x[9], S31, 0xd9d4d039); /* 45 */
        HH (d, a, b, c, x[12], S32, 0xe6db99e5);        /* 46 */
        HH (c, d, a, b, x[15], S33, 0x1fa27cf8);        /* 47 */
        HH (b, c, d, a, x[2], S34, 0xc4ac5665); /* 48 */

       /* Round 4 */
        II (a, b, c, d, x[0], S41, 0xf4292244); /* 49 */
        II (d, a, b, c, x[7], S42, 0x432aff97); /* 50 */
        II (c, d, a, b, x[14], S43, 0xab9423a7);        /* 51 */
        II (b, c, d, a, x[5], S44, 0xfc93a039); /* 52 */
        II (a, b, c, d, x[12], S41, 0x655b59c3);        /* 53 */
        II (d, a, b, c, x[3], S42, 0x8f0ccc92); /* 54 */
        II (c, d, a, b, x[10], S43, 0xffeff47d);        /* 55 */
        II (b, c, d, a, x[1], S44, 0x85845dd1); /* 56 */
        II (a, b, c, d, x[8], S41, 0x6fa87e4f); /* 57 */
        II (d, a, b, c, x[15], S42, 0xfe2ce6e0);        /* 58 */
        II (c, d, a, b, x[6], S43, 0xa3014314); /* 59 */
        II (b, c, d, a, x[13], S44, 0x4e0811a1);        /* 60 */
        II (a, b, c, d, x[4], S41, 0xf7537e82); /* 61 */
        II (d, a, b, c, x[11], S42, 0xbd3af235);        /* 62 */
        II (c, d, a, b, x[2], S43, 0x2ad7d2bb); /* 63 */
        II (b, c, d, a, x[9], S44, 0xeb86d391); /* 64 */
         iState[0] += a;
        iState[1] += b;
        iState[2] += c;
        iState[3] += d;

        /*
         * Zeroize sensitive information.
         */
        memset ((unsigned char *) x, 0, sizeof (x));
}

 /* Encodes input (UINT4) into output (unsigned char). Assumes len is
  a multiple of 4.
 */
static void Encode (unsigned char * output, UINT4 * input, int len)
{
        unsigned int    i,
                        j;

        for (i = 0, j = 0; j < len; i++, j += 4)
        {
                output[j] = (unsigned char) (input[i] & 0xff);
                output[j + 1] = (unsigned char) ((input[i] >> 8) & 0xff);
                output[j + 2] = (unsigned char) ((input[i] >> 16) & 0xff);
                output[j + 3] = (unsigned char) ((input[i] >> 24) & 0xff);
        }
}

/* Decodes input (unsigned char) into output (UINT4). Assumes len is
  a multiple of 4.
 */
static  void Decode (UINT4 * output, unsigned char * input, unsigned int len)
{
        unsigned int    i,
                        j;

        for (i = 0, j = 0; j < len; i++, j += 4)
                output[i] = ((UINT4) input[j]) | (((UINT4) input[j + 1]) << 8) |
                        (((UINT4) input[j + 2]) << 16) | (((UINT4) input[j + 3]) << 24);
}

 
