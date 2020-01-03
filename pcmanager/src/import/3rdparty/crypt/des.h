#ifndef __DES_H 
#define __DES_H  
class DES { 
public: 
        // Encrypt/decrypt the data in "data", according to the "key". 
        // Caller is responsible for confirming the buffer size of "data" 
        // points to is 8*"blocks" bytes. 
        // The data encrypted/decrypted is stored in data. 
        // The return code is 1:success, other:failed. 

        int encrypt ( char key[8], char* data, int blocks = 1 ); 
        int decrypt ( char key[8], char* data, int blocks = 1 );  

        // Encrypt/decrypt any size data,according to a special method. 
        // Before calling yencrypt, copy data to a new buffer with size 
        // calculated by extend. 

        int yencrypt ( char key[8], char* data, int size ); 
        int ydecrypt ( char key[8], char* in, int blocks, int* size = 0 );  
        int extend ( int size ) { return (size/8+1)*8; };  

private: 
        void des(unsigned char* in, unsigned char* out, int blocks); 
        void des_block(unsigned char* in, unsigned char* out);  

private: 
        unsigned long KnL[32];
        enum Mode { ENCRYPT, DECRYPT }; 
        void deskey(unsigned char key[8], Mode md); 
        void usekey(unsigned long *); 
        void cookey(unsigned long *);  

private: 
        void scrunch(unsigned char *, unsigned long *); 
        void unscrun(unsigned long *, unsigned char *); 
        void desfunc(unsigned long *, unsigned long *);  

private: 
        static unsigned char Df_Key[24]; 
        static unsigned short bytebit[8]; 
        static unsigned long bigbyte[24]; 
        static unsigned char pc1[56]; 
        static unsigned char totrot[16]; 
        static unsigned char pc2[48]; 
        static unsigned long SP1[64]; 
        static unsigned long SP2[64]; 
        static unsigned long SP3[64]; 
        static unsigned long SP4[64]; 
        static unsigned long SP5[64]; 
        static unsigned long SP6[64]; 
        static unsigned long SP7[64]; 
        static unsigned long SP8[64];  
}; 
#endif 
