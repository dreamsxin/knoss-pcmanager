//#include "StdAfx.h"
#include "stdlib.h"
#include <string.h>
#include "RSAEncrypt.h"
extern "C"  
{
	#include "lip.h"
}

//Return length of C
int RSAEncrypt(
			   char M[],	//字符串 
			   size_t uM, 
			   char D[],	//16进制字符串
			   char N[],	//16进制字符串
			   char *C		//16进制字符串
)
{
	int nRet = 0;
	size_t i = 0;
	int nCLen = 0;
	char *pM16 = NULL;
	verylong vN = 0, vD = 0, vM = 0, vC = 0; //Freelip中的大数类型为verylong。 

	if (uM > (MAX_CRYPT_MES_LEN_16/2))
		goto Exit0;

	pM16 = (char *)malloc((uM+1)<<1);
	if (pM16)
		memset(pM16, 0, (uM+1)<<1);
	
	for (i = 0; i<uM; i++)
	{
		itoa(M[i], &pM16[i<<1], 16);
	}
	
	zhsread(pM16, &vM); //初始化明文M，M等于用户名的十六进制表示
	zhsread(N, &vN); //初始化模数N 
	zhsread(D, &vD); //初始化私钥D 
	zexpmod(vM, vD, vN, &vC); //计算密文C = (M ^ D) mod N 
	nCLen = zhswrite(C, vC); //将C的16进制串表示写入C中
	nRet = 1;

Exit0:
	if (pM16)
	{
		free(pM16);
		pM16 = NULL;
	}

	return nRet>0?nCLen:-1;
}
