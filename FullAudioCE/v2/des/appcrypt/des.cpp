/* Portable C code to create DES key schedules from user-provided keys
 * This doesn't have to be fast unless you're cracking keys or UNIX
 * passwords
 */
#include <windows.h>
//#include <memory.h>
#include "des.h"

typedef unsigned long DES_KS[16][2];	/* Single-key DES key schedule */

/* In des.cpp: */
void deskey(DES_KS,unsigned char *,int);
void des(DES_KS,unsigned char *);

/* Key schedule-related tables from FIPS-46 */

/* permuted choice table (key) */
static unsigned char pc1[] = {
	57, 49, 41, 33, 25, 17,  9,
	 1, 58, 50, 42, 34, 26, 18,
	10,  2, 59, 51, 43, 35, 27,
	19, 11,  3, 60, 52, 44, 36,

	63, 55, 47, 39, 31, 23, 15,
	 7, 62, 54, 46, 38, 30, 22,
	14,  6, 61, 53, 45, 37, 29,
	21, 13,  5, 28, 20, 12,  4
};

/* number left rotations of pc1 */
static unsigned char totrot[] = {
	1,2,4,6,8,10,12,14,15,17,19,21,23,25,27,28
};

/* permuted choice key (table) */
static unsigned char pc2[] = {
	14, 17, 11, 24,  1,  5,
	 3, 28, 15,  6, 21, 10,
	23, 19, 12,  4, 26,  8,
	16,  7, 27, 20, 13,  2,
	41, 52, 31, 37, 47, 55,
	30, 40, 51, 45, 33, 48,
	44, 49, 39, 56, 34, 53,
	46, 42, 50, 36, 29, 32
};

/* End of DES-defined tables */


/* bit 0 is left-most in byte */
static int bytebit[] = {
	0200,0100,040,020,010,04,02,01
};


/* Generate key schedule for encryption or decryption
 * depending on the value of "decrypt"
 */
void deskey(DES_KS k,unsigned char* key,int decrypt)
//DES_KS k;			/* Key schedule array */
//unsigned char *key;		/* 64 bits (will use only 56) */
//int decrypt;			/* 0 = encrypt, 1 = decrypt */
{
	unsigned char pc1m[56];		/* place to modify pc1 into */
	unsigned char pcr[56];		/* place to rotate pc1 into */
	register int i,j,l;
	int m;
	unsigned char ks[8];

	for (j=0; j<56; j++) {		/* convert pc1 to bits of key */
		l=pc1[j]-1;		/* integer bit location	 */
		m = l & 07;		/* find bit		 */
		pc1m[j]=(key[l>>3] &	/* find which key byte l is in */
			bytebit[m])	/* and which bit of that byte */
			? 1 : 0;	/* and store 1-bit result */
	}
	for (i=0; i<16; i++) {		/* key chunk for each iteration */
		memset(ks,0,sizeof(ks));	/* Clear key schedule */
		for (j=0; j<56; j++)	/* rotate pc1 the right amount */
			pcr[j] = pc1m[(l=j+totrot[decrypt? 15-i : i])<(j<28? 28 : 56) ? l: l-28];
			/* rotate left and right halves independently */
		for (j=0; j<48; j++){	/* select bits individually */
			/* check bit that goes to ks[j] */
			if (pcr[pc2[j]-1]){
				/* mask it in if it's there */
				l= j % 6;
				ks[j/6] |= bytebit[l] >> 2;
			}
		}
		/* Now convert to packed odd/even interleaved form */
		k[i][0] = ((long)ks[0] << 24)
		 | ((long)ks[2] << 16)
		 | ((long)ks[4] << 8)
		 | ((long)ks[6]);
		k[i][1] = ((long)ks[1] << 24)
		 | ((long)ks[3] << 16)
		 | ((long)ks[5] << 8)
		 | ((long)ks[7]);
	}
}

////////////////
//
// From Desport.c
//


/* Portable C version of des() function */

#include "des.h"

/* Tables defined in the Data Encryption Standard documents
 * Three of these tables, the initial permutation, the final
 * permutation and the expansion operator, are regular enough that
 * for speed, we hard-code them. They're here for reference only.
 * Also, the S and P boxes are used by a separate program, gensp.c,
 * to build the combined SP box, Spbox[]. They're also here just
 * for reference. 
 */

/* Combined SP lookup table, linked in
 * For best results, ensure that this is aligned on a 32-bit boundary;
 * Borland C++ 3.1 doesn't guarantee this!
 */
/* Combined S and P boxes */
static unsigned long Spbox[8][64] = {
0x01010400,0x00000000,0x00010000,0x01010404,
0x01010004,0x00010404,0x00000004,0x00010000,
0x00000400,0x01010400,0x01010404,0x00000400,
0x01000404,0x01010004,0x01000000,0x00000004,
0x00000404,0x01000400,0x01000400,0x00010400,
0x00010400,0x01010000,0x01010000,0x01000404,
0x00010004,0x01000004,0x01000004,0x00010004,
0x00000000,0x00000404,0x00010404,0x01000000,
0x00010000,0x01010404,0x00000004,0x01010000,
0x01010400,0x01000000,0x01000000,0x00000400,
0x01010004,0x00010000,0x00010400,0x01000004,
0x00000400,0x00000004,0x01000404,0x00010404,
0x01010404,0x00010004,0x01010000,0x01000404,
0x01000004,0x00000404,0x00010404,0x01010400,
0x00000404,0x01000400,0x01000400,0x00000000,
0x00010004,0x00010400,0x00000000,0x01010004,
0x80108020,0x80008000,0x00008000,0x00108020,
0x00100000,0x00000020,0x80100020,0x80008020,
0x80000020,0x80108020,0x80108000,0x80000000,
0x80008000,0x00100000,0x00000020,0x80100020,
0x00108000,0x00100020,0x80008020,0x00000000,
0x80000000,0x00008000,0x00108020,0x80100000,
0x00100020,0x80000020,0x00000000,0x00108000,
0x00008020,0x80108000,0x80100000,0x00008020,
0x00000000,0x00108020,0x80100020,0x00100000,
0x80008020,0x80100000,0x80108000,0x00008000,
0x80100000,0x80008000,0x00000020,0x80108020,
0x00108020,0x00000020,0x00008000,0x80000000,
0x00008020,0x80108000,0x00100000,0x80000020,
0x00100020,0x80008020,0x80000020,0x00100020,
0x00108000,0x00000000,0x80008000,0x00008020,
0x80000000,0x80100020,0x80108020,0x00108000,
0x00000208,0x08020200,0x00000000,0x08020008,
0x08000200,0x00000000,0x00020208,0x08000200,
0x00020008,0x08000008,0x08000008,0x00020000,
0x08020208,0x00020008,0x08020000,0x00000208,
0x08000000,0x00000008,0x08020200,0x00000200,
0x00020200,0x08020000,0x08020008,0x00020208,
0x08000208,0x00020200,0x00020000,0x08000208,
0x00000008,0x08020208,0x00000200,0x08000000,
0x08020200,0x08000000,0x00020008,0x00000208,
0x00020000,0x08020200,0x08000200,0x00000000,
0x00000200,0x00020008,0x08020208,0x08000200,
0x08000008,0x00000200,0x00000000,0x08020008,
0x08000208,0x00020000,0x08000000,0x08020208,
0x00000008,0x00020208,0x00020200,0x08000008,
0x08020000,0x08000208,0x00000208,0x08020000,
0x00020208,0x00000008,0x08020008,0x00020200,
0x00802001,0x00002081,0x00002081,0x00000080,
0x00802080,0x00800081,0x00800001,0x00002001,
0x00000000,0x00802000,0x00802000,0x00802081,
0x00000081,0x00000000,0x00800080,0x00800001,
0x00000001,0x00002000,0x00800000,0x00802001,
0x00000080,0x00800000,0x00002001,0x00002080,
0x00800081,0x00000001,0x00002080,0x00800080,
0x00002000,0x00802080,0x00802081,0x00000081,
0x00800080,0x00800001,0x00802000,0x00802081,
0x00000081,0x00000000,0x00000000,0x00802000,
0x00002080,0x00800080,0x00800081,0x00000001,
0x00802001,0x00002081,0x00002081,0x00000080,
0x00802081,0x00000081,0x00000001,0x00002000,
0x00800001,0x00002001,0x00802080,0x00800081,
0x00002001,0x00002080,0x00800000,0x00802001,
0x00000080,0x00800000,0x00002000,0x00802080,
0x00000100,0x02080100,0x02080000,0x42000100,
0x00080000,0x00000100,0x40000000,0x02080000,
0x40080100,0x00080000,0x02000100,0x40080100,
0x42000100,0x42080000,0x00080100,0x40000000,
0x02000000,0x40080000,0x40080000,0x00000000,
0x40000100,0x42080100,0x42080100,0x02000100,
0x42080000,0x40000100,0x00000000,0x42000000,
0x02080100,0x02000000,0x42000000,0x00080100,
0x00080000,0x42000100,0x00000100,0x02000000,
0x40000000,0x02080000,0x42000100,0x40080100,
0x02000100,0x40000000,0x42080000,0x02080100,
0x40080100,0x00000100,0x02000000,0x42080000,
0x42080100,0x00080100,0x42000000,0x42080100,
0x02080000,0x00000000,0x40080000,0x42000000,
0x00080100,0x02000100,0x40000100,0x00080000,
0x00000000,0x40080000,0x02080100,0x40000100,
0x20000010,0x20400000,0x00004000,0x20404010,
0x20400000,0x00000010,0x20404010,0x00400000,
0x20004000,0x00404010,0x00400000,0x20000010,
0x00400010,0x20004000,0x20000000,0x00004010,
0x00000000,0x00400010,0x20004010,0x00004000,
0x00404000,0x20004010,0x00000010,0x20400010,
0x20400010,0x00000000,0x00404010,0x20404000,
0x00004010,0x00404000,0x20404000,0x20000000,
0x20004000,0x00000010,0x20400010,0x00404000,
0x20404010,0x00400000,0x00004010,0x20000010,
0x00400000,0x20004000,0x20000000,0x00004010,
0x20000010,0x20404010,0x00404000,0x20400000,
0x00404010,0x20404000,0x00000000,0x20400010,
0x00000010,0x00004000,0x20400000,0x00404010,
0x00004000,0x00400010,0x20004010,0x00000000,
0x20404000,0x20000000,0x00400010,0x20004010,
0x00200000,0x04200002,0x04000802,0x00000000,
0x00000800,0x04000802,0x00200802,0x04200800,
0x04200802,0x00200000,0x00000000,0x04000002,
0x00000002,0x04000000,0x04200002,0x00000802,
0x04000800,0x00200802,0x00200002,0x04000800,
0x04000002,0x04200000,0x04200800,0x00200002,
0x04200000,0x00000800,0x00000802,0x04200802,
0x00200800,0x00000002,0x04000000,0x00200800,
0x04000000,0x00200800,0x00200000,0x04000802,
0x04000802,0x04200002,0x04200002,0x00000002,
0x00200002,0x04000000,0x04000800,0x00200000,
0x04200800,0x00000802,0x00200802,0x04200800,
0x00000802,0x04000002,0x04200802,0x04200000,
0x00200800,0x00000000,0x00000002,0x04200802,
0x00000000,0x00200802,0x04200000,0x00000800,
0x04000002,0x04000800,0x00000800,0x00200002,
0x10001040,0x00001000,0x00040000,0x10041040,
0x10000000,0x10001040,0x00000040,0x10000000,
0x00040040,0x10040000,0x10041040,0x00041000,
0x10041000,0x00041040,0x00001000,0x00000040,
0x10040000,0x10000040,0x10001000,0x00001040,
0x00041000,0x00040040,0x10040040,0x10041000,
0x00001040,0x00000000,0x00000000,0x10040040,
0x10000040,0x10001000,0x00041040,0x00040000,
0x00041040,0x00040000,0x10041000,0x00001000,
0x00000040,0x10040040,0x00001000,0x00041040,
0x10001000,0x00000040,0x10000040,0x10040000,
0x10040040,0x10000000,0x00040000,0x10001040,
0x00000000,0x10041040,0x00040040,0x10000040,
0x10040000,0x10001000,0x10001040,0x00000000,
0x10041040,0x00041000,0x00041000,0x00001040,
0x00001040,0x00040040,0x10000000,0x10041000,
};
/* Primitive function F.
 * Input is r, subkey array in keys, output is XORed into l.
 * Each round consumes eight 6-bit subkeys, one for
 * each of the 8 S-boxes, 2 longs for each round.
 * Each long contains four 6-bit subkeys, each taking up a byte.
 * The first long contains, from high to low end, the subkeys for
 * S-boxes 1, 3, 5 & 7; the second contains the subkeys for S-boxes
 * 2, 4, 6 & 8 (using the origin-1 S-box numbering in the standard,
 * not the origin-0 numbering used elsewhere in this code)
 * See comments elsewhere about the pre-rotated values of r and Spbox.
 */
#define	F(l,r,key){\
	work = ((r >> 4) | (r << 28)) ^ key[0];\
	l ^= Spbox[6][work & 0x3f];\
	l ^= Spbox[4][(work >> 8) & 0x3f];\
	l ^= Spbox[2][(work >> 16) & 0x3f];\
	l ^= Spbox[0][(work >> 24) & 0x3f];\
	work = r ^ key[1];\
	l ^= Spbox[7][work & 0x3f];\
	l ^= Spbox[5][(work >> 8) & 0x3f];\
	l ^= Spbox[3][(work >> 16) & 0x3f];\
	l ^= Spbox[1][(work >> 24) & 0x3f];\
}
/* Encrypt or decrypt a block of data in ECB mode */
void des(unsigned long ks[16][2],unsigned char block[8])
//unsigned long ks[16][2];	/* Key schedule */
//unsigned char block[8];		/* Data block */
{
	unsigned long left,right,work;
	
	/* Read input block and place in left/right in big-endian order */
	left = ((unsigned long)block[0] << 24)
	 | ((unsigned long)block[1] << 16)
	 | ((unsigned long)block[2] << 8)
	 | (unsigned long)block[3];
	right = ((unsigned long)block[4] << 24)
	 | ((unsigned long)block[5] << 16)
	 | ((unsigned long)block[6] << 8)
	 | (unsigned long)block[7];

	/* Hoey's clever initial permutation algorithm, from Outerbridge
	 * (see Schneier p 478)	
	 *
	 * The convention here is the same as Outerbridge: rotate each
	 * register left by 1 bit, i.e., so that "left" contains permuted
	 * input bits 2, 3, 4, ... 1 and "right" contains 33, 34, 35, ... 32	
	 * (using origin-1 numbering as in the FIPS). This allows us to avoid
	 * one of the two rotates that would otherwise be required in each of
	 * the 16 rounds.
	 */
	work = ((left >> 4) ^ right) & 0x0f0f0f0f;
	right ^= work;
	left ^= work << 4;
	work = ((left >> 16) ^ right) & 0xffff;
	right ^= work;
	left ^= work << 16;
	work = ((right >> 2) ^ left) & 0x33333333;
	left ^= work;
	right ^= (work << 2);
	work = ((right >> 8) ^ left) & 0xff00ff;
	left ^= work;
	right ^= (work << 8);
	right = (right << 1) | (right >> 31);
	work = (left ^ right) & 0xaaaaaaaa;
	left ^= work;
	right ^= work;
	left = (left << 1) | (left >> 31);

	/* Now do the 16 rounds */
	F(left,right,ks[0]);
	F(right,left,ks[1]);
	F(left,right,ks[2]);
	F(right,left,ks[3]);
	F(left,right,ks[4]);
	F(right,left,ks[5]);
	F(left,right,ks[6]);
	F(right,left,ks[7]);
	F(left,right,ks[8]);
	F(right,left,ks[9]);
	F(left,right,ks[10]);
	F(right,left,ks[11]);
	F(left,right,ks[12]);
	F(right,left,ks[13]);
	F(left,right,ks[14]);
	F(right,left,ks[15]);

	/* Inverse permutation, also from Hoey via Outerbridge and Schneier */
	right = (right << 31) | (right >> 1);
	work = (left ^ right) & 0xaaaaaaaa;
	left ^= work;
	right ^= work;
	left = (left >> 1) | (left  << 31);
	work = ((left >> 8) ^ right) & 0xff00ff;
	right ^= work;
	left ^= work << 8;
	work = ((left >> 2) ^ right) & 0x33333333;
	right ^= work;
	left ^= work << 2;
	work = ((right >> 16) ^ left) & 0xffff;
	left ^= work;
	right ^= work << 16;
	work = ((right >> 4) ^ left) & 0x0f0f0f0f;
	left ^= work;
	right ^= work << 4;

	/* Put the block back into the user's buffer with final swap */
	block[0] = (BYTE)(right >> 24);
	block[1] = (BYTE)(right >> 16);
	block[2] = (BYTE)(right >> 8);
	block[3] = (BYTE)(right);
	block[4] = (BYTE)(left >> 24);
	block[5] = (BYTE)(left >> 16);
	block[6] = (BYTE)(left >> 8);
	block[7] = (BYTE)(left);
}

#define DES_ENCRYPT 0
#define DES_DECRYPT 1

HRESULT descrypt(BYTE* pKey,int coding,BYTE* pSrc,DWORD dwSrcLen,BYTE** ppDst,DWORD* pdwDstLen)
{
	if((pKey == NULL) || (pSrc == NULL) || (ppDst == NULL) || (pdwDstLen == NULL))
		return E_POINTER;

	DES_KS ks;
	deskey(ks,pKey,coding);
	*pdwDstLen = (dwSrcLen&0xFFFFFFF8) + ((dwSrcLen%8)?8:0);
	*ppDst = new BYTE[*pdwDstLen];
	if(!(*ppDst))
		return E_OUTOFMEMORY;

	DWORD dw = dwSrcLen & 0xFFFFFFF8; //integer division by 8 then multiply by 8.
	BYTE* pSrcPos = pSrc;
	BYTE* pDstPos = *ppDst;
	for(DWORD i=0;i<dw;i+=8)
	{
		memcpy(pDstPos,pSrcPos,8);
		pSrcPos += 8;
		des(ks,pDstPos);
		pDstPos += 8;
	}
	
	if(*pdwDstLen > dw)
	{
		memset(pDstPos,0,8);
		memcpy(pDstPos,pSrcPos,dwSrcLen-dw);
		des(ks,pDstPos);
	}

	return S_OK;
}

HRESULT Cipher(BYTE* pKey,BYTE* pPlain,DWORD dwPlainLen,BYTE** ppCipher,DWORD* pdwCipherLen)
{
	return descrypt(pKey,DES_ENCRYPT,pPlain,dwPlainLen,ppCipher,pdwCipherLen);
}

HRESULT Decipher(BYTE* pKey,BYTE* pCipher,DWORD dwCipherLen,BYTE** ppPlain,DWORD* pdwPlainLen)
{
	return descrypt(pKey,DES_DECRYPT,pCipher,dwCipherLen,ppPlain,pdwPlainLen);
}