/*
 * Copyright (c) 2004,金山KSG
 * All rights reserved.
 * 
 * 文件名称: mysocket.h
 * 文件标识:
 * 摘    要: 用于md5定义
 * 当前版本: 1.0
 * 作    者: chen.qian.jiang
 * 开始日期: 2004-08-13
 * 完成日期: 
 * 其它说明： 
 * 修改日期        版本号     修改人          修改内容
 * -----------------------------------------------
 * 2004/08/13      V1.0       陈黔江          创建
 */ 

#ifndef _MY_MD5__H
#define _MY_MD5__H

#ifdef _WIN32
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned  int UINT4;

#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21

/* F, G, H and I are basic MD5 functions.*/
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

/* ROTATE_LEFT rotates x left n bits.
 */
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

/* FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
Rotation is separate from addition to prevent recomputation.
 */
#define FF(a, b, c, d, x, s, ac) { \
 (a) += F ((b), (c), (d)) + (x) + (UINT4)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }
#define GG(a, b, c, d, x, s, ac) { \
 (a) += G ((b), (c), (d)) + (x) + (UINT4)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }
#define HH(a, b, c, d, x, s, ac) { \
 (a) += H ((b), (c), (d)) + (x) + (UINT4)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }
#define II(a, b, c, d, x, s, ac) { \
 (a) += I ((b), (c), (d)) + (x) + (UINT4)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }


#pragma pack()

void Md5_calc(unsigned char *output, unsigned char * input , int len);

#endif

#ifdef __cplusplus
}
#endif
