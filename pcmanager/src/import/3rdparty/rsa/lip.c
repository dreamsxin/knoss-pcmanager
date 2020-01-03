/*

Signed arbitrary length long int package.

        Copyright Arjen K. Lenstra, 1989-1997

        Bugs, questions, suggestions, additions, whatever, to

                Arjen K. Lenstra
		Citibank, N.A.
		4 Sylvan Way,
		Parsippany, NJ 07054

		email arjen.lenstra@citicorp.com
		fax 201 397 7657; 

        or to
                Paul C Leyland
                Oxford University Computing Services
                13 Banbury Road
                Oxford
                OX2 6NN
                United Kingdom

                email: pcl@oucs.ox.ac.uk

This version (free-LIP v1.1) is based on the LIP used in the RSA-129
project, with a few bug fixes and minor enhancements.  Permission is
granted for free re-distribution for non-commercial uses only.

*/

#ifdef WIN32
#include <windows.h>
#include <time.h>
#else
#include <sys/time.h>
#include <sys/resource.h>
#endif
#include <malloc.h>
#include <stddef.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "lip.h"


#ifndef KAR_DEPTH
# define KAR_DEPTH      20
#endif

#ifndef KAR_MUL_CROV
# define KAR_MUL_CROV   30
#endif

#ifndef KAR_SQU_CROV
# define KAR_SQU_CROV   30
#endif

#if (!ILLEGAL)


/*Function prototypes for static internal functions.*/

static void zsubmul(
	long r,
	verylong a,
	verylong b
	);

static void zalphasubmul(
	long r,
	verylong a,
	verylong b
	);

static void hidetimer(
	FILE *f,
	long w
	);

static void zhalt(
	char *c
	);

static void zaddls(
	verylong a,
	verylong b,
	verylong c
	);

static void zmstartint(
        verylong n
        );

static void zmkeep(
        verylong n
        );

static void zmback(
        );

static void kar_mul(
	verylong a,
	verylong b,
	verylong *c,
	long shi
	);

static void kar_sq(
	verylong a,
	verylong *c,
	long shi
	);

static long zxxeucl(
	verylong ain,
	verylong nin,
	verylong *invv,
	verylong *uu
	);

static void mont_init_odd_power(
	verylong a,
	verylong *a_sq,
	long m,
	verylong **odd_a_power
	);

static void init_odd_power(
	verylong a,
	verylong n,
	verylong *a_sq,
	long m,
	verylong **odd_a_power
	);

static void zsmexp(
	long a,
	verylong e,
	verylong *bb
	);

static void zpshift(
	void
	);

static long zrootnewton(
	verylong a,
	verylong *b,
	long n
	);

static long ph1set(
	verylong n,
	verylong *rap,
	verylong *alpha,
	verylong *mu,
	verylong *x,
	verylong *f
	);

static long ph1exp(
	verylong n,
	verylong *x,
	verylong e,
	verylong rap,
	verylong *f
	);

static long ph1(
	verylong n,
	verylong *x,
	long m,
	verylong rap,
	verylong *f
	);

static long ph1toph2(
	verylong n,
	verylong *x,
	verylong *y,
	verylong *ra,
	verylong alpha,
	verylong mu,
	verylong *f
	);

static long ph2squ(
	verylong n,
	verylong x1s,
	verylong y1s,
	verylong *x2,
	verylong *y2,
	verylong ra,
	verylong *f
	);

static long ph2mul(
	verylong n,
	verylong x1s,
	verylong y1s,
	verylong x2s,
	verylong y2s,
	verylong *x3,
	verylong *y3,
	verylong ra,
	verylong *f
	);

static long ph2exp(
	verylong n,
	verylong x1,
	verylong yy1,
	verylong *x2,
	verylong *y2,
	long e,
	verylong ra,
	verylong *f
	);

static long ph2set(
	verylong n,
	verylong x,
	verylong y,
	long te,
	verylong ra,
	verylong *f
	);

static void ph2prec(
	verylong n,
	long n1,
	long j
	);

static void ph2eval(
	verylong n,
	long ind,
	long n1,
	long j,
	verylong *result
	);

static long ph2(
	verylong n,
	verylong inx,
	verylong iny,
	long m,
	long te,
	verylong ra,
	verylong *f
	);

static void message(
	char *str,
	double t,
	FILE *fp
	);


#ifdef DOUBLES_LOW_HIGH
#define LO_WD 0
#define HI_WD 1
#else
#define LO_WD 1
#define HI_WD 0
#endif


/* globals for Montgomery multiplication */
static verylong zn = 0;
static verylong zoldzn = 0;
static verylong zr = 0;
static verylong zrr = 0;
static verylong zrrr = 0;
static verylong znm = 0;
static long znotinternal = 0;
static long zntop = 0;
#ifdef PLAIN_OR_KARAT
static long zninv1 = 0;
static long zninv2 = 0;
#else
static long zninv = 0;
#endif


#ifdef SINGLE_MUL

#define ExtractHiLo(Hi,Lo,x) \
{ \
double t=x+(4503599627370496.0); \
unsigned int *it = (unsigned int *)&t; \
Lo = it[LO_WD]; \
Hi = ((it[HI_WD]<<6)|(it[LO_WD]>>26)); \
Lo &= 0x3FFFFFF; \
Hi &= 0x3FFFFFF; \
}

#define zaddmulp(a,b,d,t) \
{ \
   double __lx = ((double) ((*a) + (*t))) + ((double) b)*((double) d); \
   register long  __lhi = 0, __llo = 0;\
   ExtractHiLo(__lhi,__llo,__lx);\
   (*a) = __llo;\
   (*t) = __lhi;\
}

#define zaddmulpsq(a,b,t) \
{ \
   double __lx = ((double) (*a)) + ((double) b)*((double) b); \
   register long  __lhi = 0, __llo = 0;\
   ExtractHiLo(__lhi,__llo,__lx);\
   (*a) = __llo;\
   (*t) = __lhi;\
}


typedef union { double d; unsigned long rep[2]; } d_or_rep;

#define FetchHiLo(hi,lo,x) \
{ \
   d_or_rep _l_xx; \
   _l_xx.d = x; \
   hi = _l_xx.rep[HI_WD]; \
   lo = _l_xx.rep[LO_WD]; \
}

static
void zaddmul(
        long ams,
        long *ama,
        long *amb
        ) 
{ 
        register long carry = 0; 
        register long i = (*amb++);
        register double dams = (double) ams;
        register double xx;
        register double yy;
        register unsigned long lo_wd, lo;
        register unsigned long hi_wd, hi;

        xx  =  ((double) (*amb))*dams + 4503599627370496.0;
        for (; i > 1; i--) 
        { 
                yy =  ((double) (*(++amb)))*dams +4503599627370496.0;
                FetchHiLo(hi_wd, lo_wd, xx);
                lo = lo_wd & 0x3FFFFFF;
                hi = ((hi_wd<<6)|(lo_wd>>26)) & 0x3FFFFFF;
                lo = lo + (*ama) + carry;
                *ama = lo & 0x3FFFFFF;
                carry = hi + (lo >> 26);
                ama++; 
                xx = yy;
        } 

        FetchHiLo(hi_wd, lo_wd, xx);
        lo = lo_wd & 0x3FFFFFF;
        hi = ((hi_wd<<6)|(lo_wd>>26)) & 0x3FFFFFF;
        lo = lo + (*ama) + carry;
        *ama = lo & 0x3FFFFFF;
        carry = hi + (lo >> 26);
        ama++; 
        *ama += carry; 
}


static
void zmmulp(long *ama)
{ 
        register long carry = 0; 
        register long i = (*zn);
        register long *amb = (zn+1);
        register double dams = (double) ((zninv*(*ama))&RADIXM);
        double xx;
	register double yy;
        register unsigned long lo_wd, lo;
        register unsigned long hi_wd, hi;

        xx =  ((double) (*amb))*dams + 4503599627370496.0;
        for (; i > 1; i--) 
        { 
                yy =  ((double) (*(++amb)))*dams +4503599627370496.0;
                FetchHiLo(hi_wd, lo_wd, xx);
                lo = lo_wd & 0x3FFFFFF;
                hi = ((hi_wd<<6)|(lo_wd>>26)) & 0x3FFFFFF;
                lo = lo + (*ama) + carry;
                *ama = lo & 0x3FFFFFF;
                carry = hi + (lo >> 26);
                ama++; 
                xx = yy;
        } 
        FetchHiLo(hi_wd, lo_wd, xx);
        lo = lo_wd & 0x3FFFFFF;
        hi = ((hi_wd<<6)|(lo_wd>>26)) & 0x3FFFFFF;
        lo = lo + (*ama) + carry;
        *ama = lo & 0x3FFFFFF;
        carry = hi + (lo >> 26);
        ama++; 
        if (((*ama += carry) & RADIX) > 0 )
	{
		(*ama++) &= RADIXM;
		(*ama)++;
	}
}


static
void zaddmulsq(
	long ams,
	long *ama,
	long *amb
	) 
{ 
        register long carry = 0; 
        register long i = ams;
        register double dams = (double) (*amb);
        double xx;
	register double yy;
        register unsigned long lo_wd,lo;
	register unsigned long hi_wd,hi;

	if (!ams) return;
        xx =  ((double) (*(++amb)))*dams + 4503599627370496.0;
        for (; i > 1; i--) 
        { 
                yy =  ((double) (*(++amb)))*dams + 4503599627370496.0;
		FetchHiLo(hi_wd, lo_wd, xx);
                lo = lo_wd  & 0x3FFFFFF;
                hi = ((hi_wd <<6)|(lo_wd >>26)) & 0x3FFFFFF;
                lo = lo + (*ama) + carry;
                *ama = lo & 0x3FFFFFF;
                carry = hi + (lo >> 26);
                ama++; 
                xx = yy;
        } 
	if (i==1)
	{
		FetchHiLo(hi_wd, lo_wd, xx);
        	lo = lo_wd  & 0x3FFFFFF;
        	hi = ((hi_wd <<6)|(lo_wd >>26)) & 0x3FFFFFF;
        	lo = lo + (*ama) + carry;
        	*ama = lo & 0x3FFFFFF;
        	carry = hi + (lo >> 26);
        	ama++; 
	}
        *ama += carry; 
}


static
void zsubmul(
	long ams,
	long *ama,
	long *amb
	) 
{ 
        register long carry = RADIX; 
        register long i = (*amb++);
        register double dams = (double) (RADIX-ams);
        double xx;
	register double yy;
        register unsigned long lo_wd,lo;
	register unsigned long hi_wd,hi;

        xx =  ((double) (*amb))*dams + 4503599627370496.0;
        for (; i > 1; i--) 
        { 
                yy =  ((double) (*(amb+1)))*dams + 4503599627370496.0;
		FetchHiLo(hi_wd, lo_wd, xx);
                lo = lo_wd & 0x3FFFFFF;
                hi = ((hi_wd<<6)|(lo_wd>>26)) & 0x3FFFFFF;
                lo = lo + (*ama) + carry;
                *ama = lo & 0x3FFFFFF;
                carry = hi + (lo >> 26);
                ama++; 
		carry += RADIXM - (*(amb++));
                xx = yy;
        } 
	FetchHiLo(hi_wd, lo_wd, xx);
        lo = lo_wd & 0x3FFFFFF;
        hi = ((hi_wd <<6)|(lo_wd >>26)) & 0x3FFFFFF;
        lo = lo + (*ama) + carry;
        *ama = lo & 0x3FFFFFF;
        carry = hi + (lo >> 26);
        ama++; 
	carry += RADIXM - (*amb);
        *ama += carry - RADIX; 
}

#elif KARAT

/*
	This definition of zaddmulp and zaddmulpsq assumes nothing
*/
#define zaddmulp(  a,  b,  d,  t) { \
        register unsigned long b1 = b & RADIXROOTM; \
        register unsigned long d1 = d & RADIXROOTM; \
        register unsigned long bd,b1d1,m,aa= (*a) + (*t); \
	register unsigned long ld = (d>>NBITSH); \
	register unsigned long lb = (b>>NBITSH); \
 \
        bd=lb*ld; \
        b1d1=b1*d1; \
        m=(lb+b1)*(ld+d1) - bd - b1d1; \
        aa += ( b1d1+ ((m&RADIXROOTM)<<NBITSH)); \
        (*t) = (aa >> NBITS) + bd + (m>>NBITSH); \
        (*a) = aa & RADIXM; \
}



#define zaddmulpsq(_a, _b, _t) \
{ \
	register long lb = (_b); \
	register long b1 = (_b) & RADIXROOTM; \
	register long aa = *(_a) + b1 * b1; \
 \
	b1 = (b1 * (lb >>= NBITSH) << 1) + (aa >> NBITSH); \
	aa = (aa & RADIXROOTM) + ((b1 & RADIXROOTM) << NBITSH); \
	*(_t) = lb * lb + (b1 >> NBITSH) + (aa >> NBITS); \
	*(_a) = (aa & RADIXM); \
}

#define zmmulp(mmpa) \
{ \
	register verylong lmmpa = (mmpa); \
	register verylong lmmpb = (zn); \
	register long lmmi = (*lmmpa) >> NBITSH; \
	register long lmmd = (*lmmpa) & RADIXROOTM; \
	long zmmtemp = 0; \
 \
	lmmd = (zninv1 * lmmd + (((zninv1 * lmmi + zninv2 * lmmd) & RADIXROOTM) << NBITSH)) & RADIXM; \
	for (lmmi = *lmmpb++; lmmi > 0; lmmi--) \
	{ \
		zaddmulp(lmmpa, *lmmpb, lmmd, &zmmtemp); \
		lmmpa++; \
		*lmmpb++; \
	} \
	if (((*lmmpa += zmmtemp) & RADIX) > 0) \
	{ \
		(*lmmpa++) &= RADIXM; \
		(*lmmpa)++; \
	} \
}


#define zaddmul(ams, ama, amb) \
{ \
	register long lami; \
	register long lams = (ams); \
	register verylong lama = (ama); \
	register verylong lamb = (amb); \
	long lamcarry = 0; \
 \
	for (lami = (*lamb++); lami > 0; lami--) \
	{ \
		zaddmulp(lama, *lamb, lams, &lamcarry); \
	/* Be careful, the last lama is unnormalized */ \
		lama++; \
		lamb++; \
	} \
        *lama += lamcarry; \
}


#define zaddmulsq(sql, sqa, sqb) \
{ \
	register long lsqi = (sql); \
	register long lsqs = *(sqb); \
	register verylong lsqa = (sqa); \
	register verylong lsqb = (sqb); \
	long lsqcarry = 0; \
 \
	lsqb++; \
	for (; lsqi > 0; lsqi--) \
	{ \
		zaddmulp(lsqa, *lsqb, lsqs, &lsqcarry); \
		lsqa++; \
		lsqb++; \
	} \
	*lsqa += lsqcarry; \
/* Be careful, the last lama is unnormalized */ \
}

static void
zsubmul(
	long r,
	verylong a,
	verylong b
	)
{
	register long rd = RADIX - r;
	register long i;
	long carry = RADIX;

	for (i = (*b++); i > 0; i--)
	{
		zaddmulp(a, *b, rd, &carry);
		a++;
		carry += RADIXM - (*b++);
	}
	*a += carry - RADIX;	/* unnormalized */
}


#elif PLAIN

/*
	This definition of zaddmulp and zaddmulpsq assumes nothing
*/
#define zaddmulp(_a, _b, _d, _t) \
{ \
	register long lb = (_b); \
	register long ld = (_d); \
	register long b1 = (_b) & RADIXROOTM; \
	register long d1 = (_d) & RADIXROOTM; \
	register long aa = *(_a) + b1 * d1; \
 \
	b1 = b1 * (ld >>= NBITSH) + d1 * (lb >>= NBITSH) + (aa >> NBITSH); \
	aa = (aa & RADIXROOTM) + ((b1 & RADIXROOTM) << NBITSH) + *(_t); \
	*(_t) = ld * lb + (b1 >> NBITSH) + (aa >> NBITS); \
	*(_a) = (aa & RADIXM); \
}


#define zaddmulpsq(_a, _b, _t) \
{ \
	register long lb = (_b); \
	register long b1 = (_b) & RADIXROOTM; \
	register long aa = *(_a) + b1 * b1; \
 \
	b1 = (b1 * (lb >>= NBITSH) << 1) + (aa >> NBITSH); \
	aa = (aa & RADIXROOTM) + ((b1 & RADIXROOTM) << NBITSH); \
	*(_t) = lb * lb + (b1 >> NBITSH) + (aa >> NBITS); \
	*(_a) = (aa & RADIXM); \
}

#define zmmulp(mmpa) \
{ \
	register verylong lmmpa = (mmpa); \
	register verylong lmmpb = (zn); \
	register long lmmi = (*lmmpa) >> NBITSH; \
	register long lmmd = (*lmmpa) & RADIXROOTM; \
	long zmmtemp = 0; \
 \
	lmmd = (zninv1 * lmmd + (((zninv1 * lmmi + zninv2 * lmmd) & RADIXROOTM) << NBITSH)) & RADIXM; \
	for (lmmi = *lmmpb++; lmmi > 0; lmmi--) \
	{ \
		zaddmulp(lmmpa, *lmmpb, lmmd, &zmmtemp); \
		lmmpa++; \
		*lmmpb++; \
	} \
	if (((*lmmpa += zmmtemp) & RADIX) > 0) \
	{ \
		(*lmmpa++) &= RADIXM; \
		(*lmmpa)++; \
	} \
}


#define zaddmul(ams, ama, amb) \
{ \
	register long lami; \
	register long lams = (ams); \
	register verylong lama = (ama); \
	register verylong lamb = (amb); \
	long lamcarry = 0; \
 \
	for (lami = (*lamb++); lami > 0; lami--) \
	{ \
		zaddmulp(lama, *lamb, lams, &lamcarry); \
	/* Be careful, the last lama is unnormalized */ \
		lama++; \
		lamb++; \
	} \
        *lama += lamcarry; \
}


#define zaddmulsq(sql, sqa, sqb) \
{ \
	register long lsqi = (sql); \
	register long lsqs = *(sqb); \
	register verylong lsqa = (sqa); \
	register verylong lsqb = (sqb); \
	long lsqcarry = 0; \
 \
	lsqb++; \
	for (; lsqi > 0; lsqi--) \
	{ \
		zaddmulp(lsqa, *lsqb, lsqs, &lsqcarry); \
		lsqa++; \
		lsqb++; \
	} \
	*lsqa += lsqcarry; \
/* Be careful, the last lama is unnormalized */ \
}

static void
zsubmul(
	long r,
	verylong a,
	verylong b
	)
{
	register long rd = RADIX - r;
	register long i;
	long carry = RADIX;

	for (i = (*b++); i > 0; i--)
	{
		zaddmulp(a, *b, rd, &carry);
		a++;
		carry += RADIXM - (*b++);
	}
	*a += carry - RADIX;	/* unnormalized */
}

#else

/*
	This definition of zaddmulp and zaddmulpsq presumes a two's complement
	machine in which integer overflow is ignored and where double
	precision arithmetic is fast. The 0.25 allows round to nearest
	or round towards zero (value being rounded should be integer
	except for roundoff.)

	DO NOT USE FOR alpha because of fradix_inv (unless NBITS is small)
*/

static double fradix_inv = 1.0 / RADIX;	/* Global constant */

#define zaddmulp(_a, _b, _d, _t) \
{ \
	register at = *(_a) + *(_t); \
	register long aa = (at + (_b) * (_d)) & RADIXM; \
 \
	*(_t) = (long) (0.25 + fradix_inv * (((double) (at - aa)) \
	               + ((double) (_b)) * ((double) (_d)))); \
	*(_a) = aa; \
}

#define zaddmulpsq(_a, _b, _t) \
{ \
	register at = *(_a); \
	register long aa = (at + (_b) * (_b)) & RADIXM; \
 \
	*(_t) = (long) (0.25 + fradix_inv * (((double) (at - aa)) \
				      + ((double) (_b)) * ((double) (_b)))); \
	*(_a) = aa; \
}


#define zmmulp(mmpa) \
{ \
	register verylong lmmpa = (mmpa); \
	register verylong lmmpb = (zn); \
	register long lmmi = (*lmmpa); \
	register long lmmd; \
	long zmmtemp = 0; \
 \
	lmmd = (zninv * lmmi) & RADIXM; \
	for (lmmi = *lmmpb++; lmmi > 0; lmmi--) \
	{ \
		zaddmulp(lmmpa, *lmmpb, lmmd, &zmmtemp); \
		lmmpa++; \
		*lmmpb++; \
	} \
	if (((*lmmpa += zmmtemp) & RADIX) > 0) \
	{ \
		(*lmmpa++) &= RADIXM; \
		(*lmmpa)++; \
	} \
}

#define zaddmul(ams, ama, amb) \
{ \
	register long lami; \
	register long lams = (ams); \
	register verylong lama = (ama); \
	register verylong lamb = (amb); \
	long lamcarry = 0; \
 \
	for (lami = (*lamb++); lami > 0; lami--) \
	{ \
		zaddmulp(lama, *lamb, lams, &lamcarry); \
	/* Be careful, the last lama is unnormalized */ \
		lama++; \
		lamb++; \
	} \
        *lama += lamcarry; \
}


#define zaddmulsq(sql, sqa, sqb) \
{ \
	register long lsqi = (sql); \
	register long lsqs = *(sqb); \
	register verylong lsqa = (sqa); \
	register verylong lsqb = (sqb); \
	long lsqcarry = 0; \
 \
	lsqb++; \
	for (; lsqi > 0; lsqi--) \
	{ \
		zaddmulp(lsqa, *lsqb, lsqs, &lsqcarry); \
		lsqa++; \
		lsqb++; \
	} \
	*lsqa += lsqcarry; \
/* Be careful, the last lama is unnormalized */ \
}

static void
zsubmul(
	long r,
	verylong a,
	verylong b
	)
{
	register long rd = RADIX - r;
	register long i;
	long carry = RADIX;

	for (i = (*b++); i > 0; i--)
	{
		zaddmulp(a, *b, rd, &carry);
		a++;
		carry += RADIXM - (*b++);
	}
	*a += carry - RADIX;	/* unnormalized */
}
#endif


#ifdef ALPHA50
#define zaldivaddmulp(  a,  b,  d,  t) { \
        register long b1 = b & ALPHA50RADIXROOTM; \
        register long d1 = d & ALPHA50RADIXROOTM; \
        register long bd,b1d1,m,aa= (*a) + (*t); \
        register long ld = (d>>ALPHA50NBITSH); \
        register long lb = (b>>ALPHA50NBITSH); \
 \
        bd=lb*ld; \
        b1d1=b1*d1; \
        m=(lb+b1)*(ld+d1) - bd - b1d1; \
	aa += ( b1d1+ ((m&ALPHA50RADIXROOTM)<<ALPHA50NBITSH)); \
        (*t) = (aa >> ALPHA50NBITS) + bd + (m>>ALPHA50NBITSH); \
        (*a) = aa & ALPHA50RADIXM; \
}
#endif


#define zaddmulone(ama, amb) \
{ \
	register long lami; \
	register long lams = 0; \
	register verylong lama = (ama); \
	register verylong lamb = (amb); \
 \
	lams = 0; \
	for (lami = (*lamb++); lami > 0; lami--) \
	{ \
		lams += (*lama + *lamb++); \
		*lama++ = lams & RADIXM; \
		lams >>= NBITS; \
	} \
	*lama += lams; \
}

#define zalphaaddmulone(ama, amb) \
{ \
        register long lami; \
        register long lams = 0; \
        register verylong lama = (ama); \
        register verylong lamb = (amb); \
 \
        lams = 0; \
        for (lami = (*lamb++); lami > 0; lami--) \
        { \
                lams += (*lama + *lamb++); \
                *lama++ = lams & ALPHA50RADIXM; \
                lams >>= ALPHA50NBITS; \
        } \
        *lama += lams; \
}


/*
	zdiv21 returns quot, numhigh so

	quot = (numhigh*RADIX + numlow)/denom;
	numhigh  = (numhigh*RADIX + numlow)%denom;

Assumes 0 <= numhigh < denom < RADIX and 0 <= numlow < RADIX.
*/


#ifdef ALPHA

#define zdiv21(numhigh, numlow, denom, deninv, quot) \
{ \
	register long lq21 = (long) (((fradix * (double) (numhigh)) \
			 + (double) (numlow)) * (deninv)); \
	register long lr21; \
	register long dd; \
	long lprodhigh = 0; \
	long lprodlow = 0; \
 \
	zaddmulp(&lprodlow, lq21, denom, &lprodhigh); \
	dd = numhigh-lprodhigh; \
	lprodhigh=0; /* this line here because of compiler error */ \
	if (dd > 1) { \
		register long correction = (((double)dd)*(double)RADIX + numlow - lprodlow)*deninv; \
		lq21 += correction; \
		lprodlow = 0; \
		zaddmulp(&lprodlow, lq21, denom, &lprodhigh); \
		dd = numhigh-lprodhigh; \
	} \
	else if (dd < -1) { \
		register long correction = (((double)dd)*(double)RADIX + numlow - lprodlow)*deninv; \
		lq21 += correction; \
		lprodlow = 0; \
		zaddmulp(&lprodlow, lq21, denom, &lprodhigh); \
		dd = numhigh-lprodhigh; \
	} \
	lr21 = RADIX * dd + (numlow - lprodlow); \
	if (dd=lr21*(deninv)) { \
		lq21 += dd; \
		lr21 -= denom*dd; \
	} \
	if (lr21 < 0) \
	{ \
		while (lr21 < 0) { \
			lq21--; \
			lr21 += denom; \
		}; \
	} \
	else \
	{ \
		while (lr21 >= denom) \
		{ \
			lr21 -= denom; \
			lq21++; \
		}; \
	} \
	quot = lq21; \
	numhigh = lr21; \
}
#else

#ifdef ALPHA50

#define zdiv21(numhigh, numlow, denom, deninv, quot) \
{ \
        register long lr21; \
        register long lq21 = (long) (((alpha50fradix * (double) (numhigh)) \
                         + (double) (numlow)) * (deninv)); \
        long lprodhigh = 0; \
        long lprodlow = 0; \
 \
        zaldivaddmulp(&lprodlow, lq21, denom, &lprodhigh); \
        lr21 = ALPHA50RADIX *(numhigh - lprodhigh) + (numlow - lprodlow); \
        if (lr21 < 0) \
        { \
                do \
                { \
                        lq21--; \
                } while ((lr21 += denom) < 0); \
        } \
        else \
        { \
                while (lr21 >= denom) \
                { \
                        lr21 -= denom; \
                        lq21++; \
                }; \
        } \
        quot = lq21; \
        numhigh = lr21; \
}

#else

#ifndef WIN32
#define zdiv21(numhigh, numlow, denom, deninv, quot) \
{ \
        register long lr21; \
        register long lq21 = (long) (((fradix * (double) (numhigh)) \
                         + (double) (numlow)) * (deninv)); \
        long lprodhigh = 0; \
        long lprodlow = 0; \
 \
        zaddmulp(&lprodlow, lq21, denom, &lprodhigh); \
        lr21 = RADIX * (numhigh - lprodhigh) + (numlow - lprodlow); \
        if (lr21 < 0) \
        { \
                do \
                { \
                        lq21--; \
                } while ((lr21 += denom) < 0); \
        } \
        else \
        { \
                while (lr21 >= denom) \
                { \
                        lr21 -= denom; \
                        lq21++; \
                }; \
        } \
        quot = lq21; \
        numhigh = lr21; \
}
#else

#define zdiv21(numhigh, numlow, denom, deninv, quot) \
{ \
        register long lr21; \
        register long lq21 = (long) (((fradix * (double) (numhigh)) \
                         + (double) (numlow)) * (deninv)); \
 /* Following works in many two's complement architectures. */ \
        lr21 = (numhigh << NBITS) + numlow - lq21 * denom; \
        if (lr21 < 0) \
        { \
                do \
                { \
                        lq21--; \
                } while ((lr21 += denom) < 0); \
        } \
        else \
        { \
                while (lr21 >= denom) \
                { \
                        lr21 -= denom; \
                        lq21++; \
                }; \
        } \
        quot = lq21; \
        numhigh = lr21; \
}

#endif


#endif
#endif


long
zmulmods(
	long a,
	long b,
	long n
	)
{
	register long nn=n;
	register long na= (a >= nn||a<0) ? (a % nn) : a;
	register long nb= (b >= nn||b<0 ) ? (b % nn) : b;
	register long lqmul = (long) (((double)na) * ((double)nb) / ((double)nn));
#ifdef PLAIN_OR_KARAT
	register long lr;
	long lprodhigh1 = 0;
	long lprodhigh2 = 0;
	long lprodlow1 = 0;
	long lprodlow2 = 0;

	zaddmulp(&lprodlow1, na, nb, &lprodhigh1);
	zaddmulp(&lprodlow2, lqmul, nn, &lprodhigh2);
# ifdef ALPHA
	lr = lprodhigh1 - lprodhigh2;
	if (lr > 2) {
		double correction= (((double)lr)*(double)RADIX + lprodlow1 - lprodlow2)/((double) nn);
		lqmul -= (long) correction;
		lprodhigh1 = 0; lprodhigh2 = 0; lprodlow1 = 0; lprodlow2 = 0;
		zaddmulp(&lprodlow1, na, nb, &lprodhigh1);
		zaddmulp(&lprodlow2, lqmul, nn, &lprodhigh2);
	}
	else if (lr < -2) {
		double correction= (((double)lr)*(double)RADIX + lprodlow1 - lprodlow2)/((double) nn);

		lqmul = lqmul+(long) correction;
		lprodhigh1 = 0; lprodhigh2 = 0; lprodlow1 = 0; lprodlow2 = 0;
		zaddmulp(&lprodlow1, na, nb, &lprodhigh1);
		zaddmulp(&lprodlow2, lqmul, nn, &lprodhigh2);
	}
# endif
	lr = RADIX * (lprodhigh1 - lprodhigh2) + (lprodlow1 - lprodlow2);
#else
/* Many machines compute the following modulo 2^32, which is OK */
	register long lr = na * nb - lqmul * nn;
#endif
	while (lr >= nn)
		lr -= nn;
	while (lr < 0)
		lr +=  nn;
	return lr;
}


#define MulLo(rres,a,b) \
{ \
   register double _x = ((double) a) * ((double) b); \
   double _t = _x+(4503599627370496.0); \
   register long *_lo = (long *)&_t; \
   rres =  _lo[LO_WD];\
}

long
zmulmod26(
	long a,
	long b,
	long n,
	double bninv
	)
{
        register long lqmul = (long)(((double)a)*bninv);
        register long p1;
	register long p2;
        MulLo(p1,a,b);
        MulLo(p2,lqmul,n);
        p1 -= p2;
        if (p1 < 0)
           p1 += n; 
        else if (p1 >= n)
	   p1 -= n;
	return(p1);
} 

extern void *calloc();

/* global variables */

/* for long division */
static double log10rad = -1.0;
static double log16rad = -1.0;
static double epsilon = 0.0;
static double fradix = (double)RADIX;
static double fudge = -1.0;
#ifdef ALPHA
static double fudge2 = -1.0;
#endif
#ifdef ALPHA50
static double alpha50fudge = -1.0;
static double alpha50fradix = (double) ALPHA50RADIX;
#endif
/* for random generator */
static verylong zseed = 0;
static verylong zranp = 0;
static verylong zprroot = 0;
/* for small prime genaration */
static short *lowsieve = 0;
static short *movesieve = 0;
static long pindex = 0;
static long pshift = -1;
static long lastp = 0;
/* for convenience */
static long oner[] = {1, 1, 1};
static long glosho[] = {1, 1, 0};
static verylong one = &oner[1];
/* for m_ary exponentiation */
static verylong **exp_odd_powers = 0;
/* for karatsuba */
static verylong kar_mem[5*KAR_DEPTH];
static long kar_mem_initialized = 0;

#ifndef WIN32
#include <sys/times.h>
#include <limits.h>
#ifndef CLK_TCK
#define CLK_TCK 60
#endif
#endif

double 
getutime()
{
#ifdef OKWIN32

      FILETIME create_ft,exit_ft,kernel_ft,user_ft;
      GetProcessTimes(GetCurrentProcess(),
                      &create_ft,&exit_ft,&kernel_ft,&user_ft);

      return ((double)user_ft.dwLowDateTime)/1e7;

#elif WIN32
	return((double)(clock()/CLK_TCK));
#else
	struct tms tbuf; times(&tbuf); return(1.0*tbuf.tms_utime/CLK_TCK);
#endif
}

double 
getstime()
{
#ifdef OKWIN32
      FILETIME create_ft,exit_ft,kernel_ft,user_ft;
      GetProcessTimes(GetCurrentProcess(),
                      &create_ft,&exit_ft,&kernel_ft,&user_ft);

      return ((double)kernel_ft.dwLowDateTime)/1e7;

#elif WIN32
	return((double)(clock()/CLK_TCK));
#else
	struct tms tbuf; times(&tbuf); return(1.0*tbuf.tms_stime/CLK_TCK);
#endif
}

double 
gettime()
{
#ifdef OKWIN32
      FILETIME create_ft,exit_ft,kernel_ft,user_ft;
      GetProcessTimes(GetCurrentProcess(),
                      &create_ft,&exit_ft,&kernel_ft,&user_ft);

      return ((double)(kernel_ft.dwLowDateTime+user_ft.dwLowDateTime))/1e7;
#elif WIN32
	return((double)(clock()/CLK_TCK));
#else
  struct tms tbuf;  times(&tbuf);
  return(1.0*(tbuf.tms_utime+tbuf.tms_stime)/CLK_TCK);
#endif
}

static void
hidetimer(
	FILE *f,
	long what
	)
{
	static double keep_time = 0.0;
	if (what)
	{
		fprintf(f,"%8.5f sec.\n",gettime()-keep_time);
		fflush(f);
	}
	else
		keep_time = gettime();
}

void
starttime()
{
	hidetimer(stderr,0);
}

void
printtime(
	FILE *f
	)
{
	hidetimer(f,1);
}

static void
zhalt(
        char *c
	)
{
#ifdef NOHALT
	fprintf(stderr,"error:\n   %s\ncontinue...\n",c);
	fflush(stderr);
#else
	fprintf(stderr,"fatal error:\n   %s\nexit...\n",c);
	fflush(stderr);
	(void)exit((int)0);
#endif
}

void
zsetlength(
	verylong *v,
	long len,
	char *str
	)
{
	verylong x = *v;

	if (x)
	{
		if (len <= x[-1])
			return;
#if (PRT_REALLOC>0)
			fprintf(stderr,"%s reallocating to %ld\n", str, len);
			fflush(stderr);
#endif
		x[-1] = len;
		if (!(x = (verylong)realloc((void*)(&(x[-1])), (size_t)(len + 2) * SIZEOFLONG)))
		{
			fprintf(stderr,"%d bytes realloc failed\n", ((int)len + 2) * SIZEOFLONG);
			zhalt("reallocation failed in zsetlength");
		}
	}
	else if (len >= 0)
	{
		if (len < SIZE)
			len = SIZE;
#if (PRT_REALLOC>0)
			fprintf(stderr,"%s allocating to %ld\n", str, len);
			fflush(stderr);
#endif
		if (!(x = (verylong)calloc((size_t)(len + 2), (size_t)SIZEOFLONG)))
		{
			fprintf(stderr,"%d bytes calloc failed\n", ((int)len + 2) * SIZEOFLONG);
			zhalt("allocation failed in zsetlength");
		}
		x[0] = len;
		x[1] = 1;
		x[2] = 0;
	}
	else
		zhalt("negative size allocation in zsetlength");
	x++;
	*v = x;
}

void
zfree(
	verylong *x
	)
{
	if (!(*x))
		return;
	{
		verylong y = (*x - 1);
		free((void*)y);
		*x = 0;
		return;
	}
}

double 
zdoub(
	verylong n
	)
{
	double res;
	register long i;

	if (ALLOCATE && !n)
		return ((double) 0);
	if ((i = n[0]) < 0)
		i = -i;
	res = (double) (n[i--]);
	for (; i; i--)
		res = res * fradix + (double) (n[i]);
	if (n[0] > 0)
		return (res);
	return (-res);
}

void
zstart()
{
	double local_one = (double) 1;
	double local_half = 1 / (double) 2;
	epsilon = local_one;
	fudge = local_one + epsilon;

 /* the following loop is sick, but we have to: */
 /* comparing one to one + epsilon does not have */
 /* the desired effect on some machines, because */
 /* one + epsilon is Not cast to double (as it should) */
	while (local_one != fudge)
	{
		epsilon = epsilon * local_half;
		fudge = local_one + epsilon;
	}
	epsilon += epsilon;
#ifndef ALPHA_OR_ALPHA50
	if ((epsilon * RADIX) > local_one)
	{
		fprintf(stderr,
			"zstart failure: recompile with smaller NBITS\n");
		exit(0);
	}
#endif
	epsilon *= 3;
	fudge = fradix + epsilon * fradix;
#ifdef ALPHA
	fudge2 = fradix - epsilon * fradix;
#endif
#ifdef ALPHA50
	alpha50fudge = alpha50fradix + epsilon * alpha50fradix;
#endif
}


void
zzero(
	verylong *aa
	)
{
	if (!(*aa)) zsetlength(aa, (long)SIZE, "in zzero, first argument");
	(*aa)[0] = (long) 1;
	(*aa)[1] = (long) 0;
}

void
zone(
	verylong *aa
	)
{
	if (!(*aa)) zsetlength(aa, (long)SIZE, "in zone, first argument");
	(*aa)[0] = (long) 1;
	(*aa)[1] = (long) 1;
}

void
zcopy(
	verylong a,
	verylong *bb
	)
{
	register long i;
	verylong b = *bb;

	if (ALLOCATE && !a)
	{
		zzero(bb);
		return;
	}
	if (a != b)
	{
		if ((i = *a) < 0)
			i = (-i);
		zsetlength(&b, i, "in zcopy, second argument");
		*bb = b;
		for (; i >= 0; i--)
			*b++ = *a++;
	}
}

void
zintoz(
	long d,
	verylong *aa
	)
{
	register long i = 0;
	register long anegative = 0;
	verylong a = *aa;

	if (!(*aa)) zsetlength(&a, (long)SIZE, "in zintoz, second argument");
	*aa = a;
	if (d < 0)
	{
		anegative = 1;
		d = -d;
	}
	a[1] = 0;
	while (d > 0)
	{
		a[++i] = d & RADIXM;
		d >>= NBITS;
	}
	if (i > 0)
		a[0] = i;
	else
		a[0] = 1;
	if (anegative)
		a[0] = (-a[0]);
}

void
zuintoz(
	unsigned long d,
	verylong *aa
	)
{
	register long i = 0;
	verylong a = *aa;

	if (!(*aa)) zsetlength(&a, (long)SIZE, "in zuintoz, second argument");
	*aa = a;
	a[1] = 0;
	while (d)
	{
		a[++i] = (long)(d & RADIXM);
		d >>= NBITS;
	}
	if (i > 0)
		a[0] = i;
	else
		a[0] = 1;
}

long 
ztoint(
	verylong a
	)
{
	register long d;
	register long sa;

	if (ALLOCATE && !a)
		return (0);
	if ((sa = *a) < 0)
		sa = -sa;
	d = *(a += sa);
	while (--sa)
	{
		d <<= NBITS;
		d += *(--a);
	}
	if ((*(--a)) < 0)
		return (-d);
	return (d);
}

unsigned long 
ztouint(
	verylong a
	)
{
	register unsigned long d;
	register long sa;

	if (ALLOCATE && !a)
		return (0);
	if ((sa = *a) < 0)
		sa = -sa;
	d = *(a += sa);
	while (--sa)
	{
		d <<= NBITS;
		d += *(--a);
	}
	return (d);
}

void
zultoz(
        unsigned long a[],
        long b,
        verylong *c
        )
{
        register long i;
        if (b<0) return;
        zuintoz(a[b-1],c);
        for (i=b-2;i>=0;i--) {
                register long j=1;
                register unsigned long d = a[i];
                zlshift(*c,CHARL*SIZEOFLONG,c);
                while (d) {
                        (*c)[j++] += (long)(d&RADIXM);
                        d >>= NBITS;
                }

        }
}

long
ztoul(
        verylong a,
        unsigned long b[],
        long *c
        )
{
        STATIC verylong ca = 0;
        STATIC verylong aux = 0;
        register long i=1;
        if (*c<=0) return(0);
        if (ziszero(a)) {
                b[0] = 0;
                *c = 1;
                return(1);
        }
        zcopy(a,&ca);
        zabs(&ca);
        for (;;) {
                zlowbits(ca,CHARL*SIZEOFLONG,&aux);
                if (i>*c) return(0);
                b[i-1] = ztouint(aux);
                zrshift(ca,CHARL*SIZEOFLONG,&ca);
                if (ziszero(ca)) {
                        *c = i;
                        return(1);
                }
                i++;
        }
}

void
zsbastoz(
	long base,
	long row[],
	long len,
	verylong *n
	)
{
	register long i;

	zintoz(row[len - 1], n);
	for (i = len - 1; i--;)
	{
		zsmul(*n, base, n);
		zsadd(*n, row[i], n);
	}
}

void
zbastoz(
	verylong base,
	verylong row[],
	long len,
	verylong *n
	)
{
	register long i;

	zcopy(row[len - 1], n);
	for (i = len - 1; i--;)
	{
		zmulin(base, n);
		zadd(*n, row[i], n);
	}
}

long 
zstobas(
	verylong n,
	long base,
	long row[],
	long *len
	)
{
 /* return 1 if it fits, 0 otherwise */
 /* n >= 0, base > 1 */
	register long i = 0;
	register long max = *len;
	STATIC verylong q = 0;
	STATIC verylong nn = 0;

	if (max < 1)
		return (0);
	if (base < 0)
		base = -base;
	if (base <= 1)
		return (0);
	*len = 0;
	row[0] = 0;
	zcopy(n, &nn);
	zabs(&nn);
	for (;;)
	{
		row[i] = zsdiv(nn, base, &q);
		i++;
		if ((q[1] == 0) && (q[0] == 1))
		{
			*len = i;
			FREE2SPACE(q,nn);
			return (1);
		}
		if (i == max) {
			FREE2SPACE(q,nn);
			return (0);
		}
		zswap(&q, &nn);
	}
}

long 
zstosymbas(
	verylong n,
	long base,
	long row[],
	long *len
	)
{
 /* return 1 if it fits, 0 otherwise */
 /* n >= 0, base > 1 */
	register long i = 0;
	register long max = *len;
	register long hbase;
	STATIC verylong q = 0;
	STATIC verylong nn = 0;

	if (max < 1)
		return (0);
	if (base < 0)
		base = -base;
	if (base <= 1)
		return (0);
	hbase = (base >> 1);
	*len = 0;
	row[0] = 0;
	zcopy(n, &nn);
	zabs(&nn);
	for (;;)
	{
		if ((row[i] = zsdiv(nn, base, &q)) > hbase)
		{
			row[i] -= base;
			zsadd(q, 1, &q);
		}
		i++;
		if ((q[1] == 0) && (q[0] == 1))
		{
			*len = i;
			FREE2SPACE(q,nn);
			return (1);
		}
		if (i == max) {
			FREE2SPACE(q,nn);
			return (0);
		}
		zswap(&q, &nn);
	}
}

long 
ztobas(
	verylong n,
	verylong base,
	verylong row[],
	long *len
	)
{
 /* return 1 if it fits, 0 otherwise */
 /* n >= 0, base > 1 */
	register long i = 0;
	register long max = *len;
	STATIC verylong q = 0;
	STATIC verylong nn = 0;
	STATIC verylong lbase = 0;

	if (max < 1)
		return (0);
	zcopy(base, &lbase);
	zabs(&lbase);
	if (zscompare(base, 1) <= 0)
		return (0);
	*len = 0;
	zintoz(0, &(row[0]));
	zcopy(n, &nn);
	zabs(&nn);
	for (;;)
	{
		zdiv(nn, lbase, &q, &(row[i]));
		i++;
		if ((q[1] == 0) && (q[0] == 1))
		{
			*len = i;
			FREE2SPACE(q,nn); FREESPACE(lbase);
			return (1);
		}
		if (i == max) {
			FREE2SPACE(q,nn); FREESPACE(lbase);
			return (0);
		}
		zswap(&q, &nn);
	}
}

long 
ztosymbas(
	verylong n,
	verylong base,
	verylong row[],
	long *len
	)
{
 /* return 1 if it fits, 0 otherwise */
 /* n >= 0, base > 1 */
	register long i = 0;
	register long max = *len;
	STATIC verylong q = 0;
	STATIC verylong nn = 0;
	STATIC verylong lbase = 0;
	STATIC verylong hbase = 0;

	if (max < 1)
		return (0);
	zcopy(base, &lbase);
	zabs(&lbase);
	if (zscompare(base, 1) <= 0)
		return (0);
	z2div(lbase, &hbase);
	*len = 0;
	zintoz(0, &(row[0]));
	zcopy(n, &nn);
	zabs(&nn);
	for (;;)
	{
		zdiv(nn, lbase, &q, &(row[i]));
		if (zcompare(row[i], hbase) > 0)
		{
			zsub(row[i], lbase, &(row[i]));
			zsadd(q, 1, &q);
		}
		i++;
		if ((q[1] == 0) && (q[0] == 1))
		{
			*len = i;
			FREE2SPACE(q,nn); FREE2SPACE(lbase,hbase);
			return (1);
		}
		if (i == max) {
			FREE2SPACE(q,nn); FREE2SPACE(lbase,hbase);
			return (0);
		}
		zswap(&q, &nn);
	}
}

long 
zcompare(
	verylong a,
	verylong b
	)
{
	register long sa;
	register long sb;

	if (ALLOCATE && !a)
	{
		if (!b)
			return (0);
		if (b[0] < 0)
			return (1);
		if (b[0] > 1)
			return (-1);
		if (b[1])
			return (-1);
		return (0);
	}
	if (ALLOCATE && !b)
	{
		if (a[0] < 0)
			return (-1);
		if (a[0] > 1)
			return (1);
		if (a[1])
			return (1);
		return (0);
	}
 /* if (a==b) return (0); but who is comparing identical addresses? */
	if ((sa = *a) > (sb = *b))
		return (1);
	if (sa < sb)
		return (-1);
	if (sa < 0)
		sa = (-sa);
	a += sa;
	b += sa;
	for (; sa; sa--)
	{
		if (*a > *b)
		{
			if (sb < 0)
				return (-1);
			return (1);
		}
		if (*a-- < *b--)
		{
			if (sb < 0)
				return (1);
			return (-1);
		}
	/*
	 * depending on the relative speed of various operations it might be
	 * better to do the following: if ((aux=(*a--)-(*b--))>0) { if (sb<0)
	 * return (-1); return (1); } else if (aux<0) { if (sb<0) return (1);
	 * return (-1); } where aux is a register long
	 */
	}
	return (0);
}

void
znegate(
	verylong *aa
	)
{
	verylong a = *aa;

	if (!a)
		return;
	if (a[1] || a[0] != 1)
		a[0] = (-a[0]);
}

void
zsadd(
	verylong a,
	long d,
	verylong *b
	)
{
	STATIC verylong x = 0;

	zintoz(d, &x);
	zadd(a, x, b);
	FREESPACE(x);
}

static void
zaddls(
	verylong a,
	verylong b,
	verylong c
	)
{
/* low level c = a+b, b shorter than a, output can be input */
	register long sa = *a;
	register long sb = *b;
	register long carry = 0;
	register long i;
	register verylong pc;

 /* we know that sa and sb are both >0 or both <0 */
	pc = &c[0];
	if (sa < 0)
	{
		sa = -sa;
		sb = -sb;
	}
	for (i = 1; i <= sb; i++)
	{
		if ((*(++pc) = (*(++a)) + (*(++b)) + carry) < RADIX)
			carry = 0;
		else
		{
			*pc -= RADIX;
			carry = 1;
		}
	}
	for (; i <= sa; i++)
	{
		if ((*(++pc) = (*(++a)) + carry) < RADIX)
			carry = 0;
		else
		{
			*pc -= RADIX;
			carry = 1;
		}
	}
	if (carry)
	{
		c[0] = sa + 1;
		*(++pc) = 1;
	}
	else
		c[0] = sa;
}

void
zsubpos(
	verylong a,
	verylong b,
	verylong *cc
	)
{
	register long sa = a[0];
	register long sb = b[0];
	register long carry = 0;
	register long i;
	register verylong pc;
	verylong c = *cc;

	if (ALLOCATE && !b)
	{
		if (a)
			zcopy(a, cc);
		else
			zzero(cc);
		return;
	}
	if (ALLOCATE && !a)
	{
		zzero(cc);
		return;
	}
	zsetlength(&c, sa, "in zsubpos, third argument");
	/* if *cc == a, then nothing will happen */
	/* if *cc == b, then b might point to freed space, so */
	if (b == *cc) b = c;
	*cc = c;
	pc = &c[0];
	for (i = 1; i <= sb; i++)
	{
		if ((*(++pc) = (*(++a)) - (*(++b)) - carry) >= 0)
			carry = 0;
		else
		{
			*pc += RADIX;
			carry = 1;
		};
	}
	for (; i <= sa; i++)
	{
		if ((*(++pc) = (*(++a)) - carry) >= 0)
			carry = 0;
		else
		{
			*pc += RADIX;
			carry = 1;
		};
	}
	i = sa;
	while ((i > 1) && (!(*pc)))
	{
		i--;
		pc--;
	}
	c[0] = i;
}

void
zadd(
	verylong a,
	verylong b,
	verylong *cc
	)
{
	register long sa;
	register long sb;
	register long anegative;
	verylong c = *cc;

	if (ALLOCATE && !a)
	{
		if (b)
			zcopy(b, cc);
		else
			zzero(cc);
		return;
	}
	if (ALLOCATE && !b)
	{
		zcopy(a, cc);
		return;
	}
	if ((anegative = ((sa = a[0]) < 0)) == ((sb = b[0]) < 0))
	{
	/* signs a and b are the same */
		register long i;
		if (anegative)
		{
			sa = -sa;
			sb = -sb;
		}
		zsetlength(&c, (sa > sb ? sa : sb) + 1, "in zadd, third argument");
		if (a == *cc) a = c;
		if (b == *cc) b = c;
		*cc = c;
		if (sa == sb)
		{
			register verylong pc;
			pc = &c[0];
			i = 0;
			for (; sa; sa--)
			{
				if ((*(++pc) = (*(++a)) + (*(++b)) + i) < RADIX)
					i = 0;
				else
				{
					*pc -= RADIX;
					i = 1;
				}
			}
			if (i)
			{
				c[0] = sb + 1;
				*(++pc) = 1;
			}
			else
				c[0] = sb;
		}
		else if (sa > sb)
			zaddls(a, b, c);
		else
			zaddls(b, a, c);
		if (anegative)
			c[0] = -c[0];
	/* if anegative, then c cannot be zero */
	}
	else
	{
	/* signs a and b are different */
		verylong old;
		verylong oldc;

		oldc = c;
		if (anegative)
		{
			a[0] = -a[0];
			old = a;
		}
		else
		{
			b[0] = -b[0];
			old = b;
		}
	/* the one that's negative cannot be zero */
		if (!(sa = zcompare(a, b)))
		{
			zzero(&c);
			*cc = c;
			if (anegative)
			{
				if (old != oldc)
					a[0] = -a[0];
			}
			else if (old != oldc)
				b[0] = -b[0];
		}
		else if (sa > 0)
		{
			zsubpos(a, b, &c);
			*cc = c;
			if (anegative)
			{
				c[0] = -c[0];
				if (old != oldc)
					a[0] = -a[0];
			}
			else if (old != oldc)
				b[0] = -b[0];
		}
		else
		{
			zsubpos(b, a, &c);
			*cc = c;
			if (!anegative)
			{
				c[0] = -c[0];
				if (old != oldc)
					b[0] = -b[0];
			}
			else if (old != oldc)
				a[0] = -a[0];
		}
	}
}

void
zsub(
	verylong a,
	verylong b,
	verylong *cc
	)
{
	register long sa;
	register long sb;
	register long anegative;
	verylong c = *cc;

	if (ALLOCATE && !b)
	{
		if (a)
			zcopy(a, cc);
		else
			zzero(cc);
		return;
	}
	if (ALLOCATE && !a)
	{
		zcopy(b, cc);
		znegate(cc);
		return;
	}
	if ((anegative = ((sa = a[0]) < 0)) == ((sb = b[0]) < 0))
	{
	/* signs a and b are the same */
		register long carry = 0;
		register long i;
		register verylong pc;
		register long agrb;

		if (!(agrb = zcompare(a, b)))
		{
			zzero(cc);
			return;
		}
		if ((agrb > 0 && anegative) || (agrb < 0 && !anegative))
		{
			pc = a;
			a = b;
			b = pc;
			sa = *a;
			sb = *b;
		}
		if (anegative)
		{
			sa = -sa;
			sb = -sb;
		}
		zsetlength(&c, sa, "in zsub, third argument");
                if (b == *cc) b = c;
		*cc = c;
		pc = &c[0];
		for (i = 1; i <= sb; i++)
		{
			if ((*(++pc) = (*(++a)) - (*(++b)) - carry) >= 0)
				carry = 0;
			else
			{
				*pc += RADIX;
				carry = 1;
			};
		}
		for (; i <= sa; i++)
		{
			if ((*(++pc) = (*(++a)) - carry) >= 0)
				carry = 0;
			else
			{
				*pc += RADIX;
				carry = 1;
			};
		}
		i = sa;
		while ((i > 1) && (!(*pc)))
		{
			i--;
			pc--;
		}
		if (agrb > 0)
			c[0] = i;
		else
			c[0] = -i;
	}
	else
	{
	/* signs a and b are different */
		verylong old;
		verylong oldc;

		oldc = c;
		if (anegative)
		{
			a[0] = -a[0];
			old = a;
		}
		else
		{
			b[0] = -b[0];
			old = b;
		}
	/* the one that's negative cannot be zero */
		zadd(a, b, &c);
		*cc = c;
		if (anegative)
		{
			c[0] = -c[0];
			if (old != oldc)
				a[0] = -a[0];
		}
		else if (old != oldc)
			b[0] = -b[0];
	}
}

void
zsmul(
	verylong a,
	long d,
	verylong *bb
	)
{
	register long sa;
	register long i;
	register verylong pb;
	register long bnegative = 0;
	verylong b = *bb;

	if (( d >= RADIX) || (-d >= RADIX)) {
		STATIC verylong x = 0;
		zintoz(d,&x);
		zmulin(a,&x);
		zcopy(x,bb);
		FREESPACE(x);
		return;
	}

	if (ALLOCATE && !a)
	{
		zzero(bb);
		return;
	}
	if (ALLOCATE && !d)
	{
		zzero(bb);
		return;
	}
	if ((sa = a[0]) < 0)
	{
		sa = (-sa);
		if (d < 0)
			d = (-d);
		else
			bnegative = 1;
	}
	else if ((bnegative = (d < 0)))
		d = (-d);
	zsetlength(&b, sa + 1, "in zsmul, third argument");
        if (a == *bb) a = b;
	*bb = b;
	pb = &b[0];
	for (i = sa; i >= 0; i--)
		*pb++ = *a++;
	b[0] = sa;
	sa++;
	*pb = 0;
	zaddmul(d - 1, &b[1], &b[0]);
	while ((sa > 1) && (!(b[sa])))
		sa--;
	b[0] = sa;
	if (bnegative && (b[1] || b[0] != 1))
		b[0] = (-b[0]);
}

static void
kar_mul(
	verylong a,
	verylong b,
	verylong *c,
	long shi
	)
{
	register long al;
	register long hal;
	register long i;
	register long restoreb0 = b[0];
	register verylong pc;
	register long bbig = 1;
	verylong *a0;
	verylong *a1;
	verylong *a2;
	verylong *a3;
	verylong *a4;

	zsetlength(c, (hal = (al = a[0]) + (i = b[0])), "in kar_mul, third argument");
	if ((shi >= (5 * KAR_DEPTH)) || (al < KAR_MUL_CROV) || (i < KAR_MUL_CROV))
	{
		pc = &(*c)[1];
		for (i = hal; i > 0; i--)
			*pc++ = 0;
		pc = &(*c)[1];
		if (al <= *b)
			for (i = al; i; i--)
			{
				zaddmul(*(++a), pc++, b);
			}
		else
			for (i = *b; i; i--)
			{
				zaddmul(*(++b), pc++, a);
			}
		while ((hal > 1) && (!((*c)[hal])))
			hal--;
		(*c)[0] = hal;
		return;
	}
	a0 = &(kar_mem[shi]);
	a1 = &(kar_mem[shi + 1]);
	a2 = &(kar_mem[shi + 2]);
	a3 = &(kar_mem[shi + 3]);
	a4 = &(kar_mem[shi + 4]);
	hal = ((al + 1) >> 1);
	zsetlength(a0, al, "in kar_mul, locals\n");
	zsetlength(a1, al, "");
	zsetlength(a2, al, "");
	zsetlength(a3, al + hal, "");
	zsetlength(a4, al + 2, "");
	i = hal;
	while ((i > 1) && (!(a[i])))
		i--;
	a[0] = i;
	if (hal >= b[0])
		bbig = 0;
	else
	{
		i = hal;
		while ((i > 1) && (!(b[i])))
			i--;
		b[0] = i;
	}
	for (i = hal + 1; i <= al; i++)
		(*a1)[i - hal] = a[i];
	(*a1)[0] = al - hal;
	if (bbig)
	{
		for (i = hal + 1; i <= restoreb0; i++)
			(*a3)[i - hal] = b[i];
		(*a3)[0] = restoreb0 - hal;
	}
	kar_mul(a, b, a4, shi + 5);
	zadd(a, (*a1), a0);
	a[0] = al;
	if (bbig)
	{
		kar_mul((*a1), (*a3), c, shi + 5);
		zadd(b, (*a3), a2);
		b[0] = restoreb0;
		kar_mul((*a0), (*a2), a3, shi + 5);
	}
	else
		kar_mul((*a0), b, a3, shi + 5);
	zsubpos((*a3), (*a4), a3);
	if (bbig)
		zsubpos((*a3), *c, a3);
	zlshift((*a3), hal * NBITS, a3);
	hal <<= 1;
	if (bbig)
	{
		for (i = (*c)[0]; i; i--)
			(*c)[i + hal] = (*c)[i];
		for (i = hal; i > (*a4)[0]; i--)
			(*c)[i] = 0;
		for (; i; i--)
			(*c)[i] = (*a4)[i];
		(*c)[0] += hal;
	}
	else
	{
		for (i = (*a4)[0]; i >= 0; i--)
			(*c)[i] = (*a4)[i];
	}
	zadd(*c, (*a3), c);
}

void
zmul(
	verylong a,
	verylong b,
	verylong *c
	)
{	/* output not input */
	register long aneg;
	register long bneg;
	verylong olda;
	verylong oldb;

	if (ALLOCATE && (!a || !b))
	{
		zzero(c);
		return;
	}
	if (a == b)
	{
		zsq(a, c);
		return;
	}
	if (!kar_mem_initialized)
	{
		kar_mem_initialized = 1;
		for (aneg = (5 * KAR_DEPTH) - 1; aneg >= 0; aneg--)
			kar_mem[aneg] = (verylong) 0;
	}
	olda = a;
	oldb = b;
	if ((aneg = (*a < 0)))
		a[0] = -a[0];
	if ((bneg = (*b < 0)))
		b[0] = -b[0];
	if (*a > *b)
		kar_mul(a, b, c, (long) 0);
	else
		kar_mul(b, a, c, (long) 0);
	if (aneg != bneg && ((*c)[1] || (*c)[0] != 1))
		(*c)[0] = -(*c)[0];
	if (aneg)
		olda[0] = -olda[0];
	if (bneg)
		oldb[0] = -oldb[0];
}

static void
kar_sq(
	verylong a,
	verylong *c,
	long shi
	)
{
	register long al;
	register long hal;
	register long i;
	register verylong pc;
	verylong *a0;
	verylong *a1;
	verylong *a2;

	zsetlength(c, (i = ((al = a[0]) << 1)), "in kar_sq, second argument");
	if ((shi >= (3 * KAR_DEPTH)) || (al < KAR_SQU_CROV))
	{
		register unsigned long uncar;
		long carry = 0;
		pc = &(*c)[1];
		for (; i > 0; i--)
			*pc++ = 0;
		for (hal = 1; hal <= al; hal++)
		{
			i += 2;
			{
				zaddmulsq(al - hal, &((*c)[i]), &(a[hal]));
			}
			uncar = ((*c)[i - 1] << 1) + carry;
			(*c)[i - 1] = uncar & RADIXM;
			uncar = ((*c)[i] << 1) + (uncar >> NBITS);
			{
				zaddmulpsq(&(*c)[i - 1], a[hal], &carry);
			}
			uncar += carry;
			carry = uncar >> NBITS;
			(*c)[i] = uncar & RADIXM;
		}
		while ((i > 1) && (!((*c)[i])))
			i--;
		(*c)[0] = i;
		return;
	}
	a0 = &(kar_mem[shi]);
	a1 = &(kar_mem[shi + 1]);
	a2 = &(kar_mem[shi + 2]);
	hal = ((al + 1) >> 1);
	zsetlength(a0, al + hal + 2, "in kar_sq, locals\n");
	zsetlength(a1, al + 2, "");
	zsetlength(a2, al, "");
	i = hal;
	while ((i > 1) && (!(a[i])))
		i--;
	a[0] = i;
	for (i = hal + 1; i <= al; i++)
		(*a0)[i - hal] = a[i];
	(*a0)[0] = al - hal;
	kar_sq(a, a1, shi + 3);
	zadd(a, (*a0), a2);
	kar_sq((*a0), c, shi + 3);
	a[0] = al;
	kar_sq((*a2), a0, shi + 3);
	zsubpos((*a0), (*a1), a0);
	zsubpos((*a0), *c, a0);
	zlshift((*a0), hal * NBITS, a0);
	hal <<= 1;
	for (i = (*c)[0]; i; i--)
		(*c)[i + hal] = (*c)[i];
	for (i = hal; i > (*a1)[0]; i--)
		(*c)[i] = 0;
	for (; i; i--)
		(*c)[i] = (*a1)[i];
	(*c)[0] += hal;
	zadd(*c, (*a0), c);
}

void
zsq(
	verylong a,
	verylong *c
	)
{	/* output is not input */
	register long aneg;

	if (ALLOCATE && !a)
	{
		zzero(c);
		return;
	}
        if (!kar_mem_initialized)
        {
                kar_mem_initialized = 1;
                for (aneg = (5 * KAR_DEPTH) - 1; aneg >= 0; aneg--)
                        kar_mem[aneg] = (verylong) 0;
        }
	if ((aneg = (*a < 0)))
		a[0] = -a[0];
	kar_sq(a, c, (long) 0);
	if (aneg)
		a[0] = -a[0];
}

void
zmul_plain(
	verylong a,
	verylong b,
	verylong *cc
	)
{
	register long i;
	register verylong pc;
	register long sc;
	verylong c = *cc;
	long anegative;
	long bnegative;
	verylong olda;
	verylong oldb;

	if (ALLOCATE && (!a || !b))
	{
		zzero(cc);
		return;
	}
	olda = a;
	oldb = b;
	if ((anegative = (*a < 0)))
		a[0] = -a[0];
	if (olda == oldb)
		bnegative = anegative;
	else if ((bnegative = (*b < 0)))
		b[0] = -b[0];
	zsetlength(&c, (sc = *a + *b), "in zmul_plain, third argument");
	*cc = c;
	pc = &c[1];
	for (i = sc; i > 0; i--)
		*pc++ = 0;
	pc = &c[1];
	if (*a <= *b)
		for (i = *a; i; i--)
		{
			zaddmul(*(++a), pc++, b);
		}
	else
		for (i = *b; i; i--)
		{
			zaddmul(*(++b), pc++, a);
		}
	while ((sc > 1) && (!(c[sc])))
		sc--;
	c[0] = sc;
	if (anegative != bnegative && (c[1] || c[0] != 1))
		c[0] = -c[0];
	if (anegative)
		olda[0] = -olda[0];
	if (bnegative && oldb != olda)
		oldb[0] = -oldb[0];
}

void
zmulin(
	verylong b,
	verylong *a
	)
{
	STATIC verylong mem = 0;

	if (ALLOCATE && (!*a || !b))
	{
		zzero(a);
		return;
	}
	zcopy(*a, &mem);
	zmul(mem, b, a);
	FREESPACE(mem);
}

void
zsq_plain(
	verylong a,
	verylong *cc
	)
{
	register long i;
	register long sc;
	register verylong pc;
	register unsigned long uncar;
	long carry = 0;
	verylong c = *cc;
	long anegative;

	if (ALLOCATE && !a)
	{
		zzero(cc);
		return;
	}
	if ((anegative = (*a)) < 0)
		anegative = -anegative;
	zsetlength(&c, (sc = 2 * anegative), "in zsq_plain, second argument");
	*cc = c;
	pc = &c[1];
	for (i = sc; i > 0; i--)
		*pc++ = 0;
	for (sc = 1; sc <= anegative; sc++)
	{
		i += 2;
		{
			zaddmulsq(anegative - sc, &(c[i]), &(a[sc]));
		}
		uncar = (c[i - 1] << 1) + carry;
		c[i - 1] = uncar & RADIXM;
		uncar = (c[i] << 1) + (uncar >> NBITS);
		{
			zaddmulpsq(&c[i - 1], a[sc], &carry);
		}
		uncar += carry;
		carry = uncar >> NBITS;
		c[i] = uncar & RADIXM;
	}
	while ((i > 1) && (!(c[i])))
		i--;
	c[0] = i;
}

void
zsqin(
	verylong *a
	)
{
	STATIC verylong mem = 0;

	if (ALLOCATE && !*a)
	{
		zzero(a);
		return;
	}
	zcopy(*a, &mem);
	zsq(mem, a);
	FREESPACE(mem);
}


#ifndef ALPHA50
long 
zsdiv(
	verylong a,
	long d,
	verylong *bb
	)
{
	register long sa;
	verylong b = *bb;

#ifndef START
	if (fudge < 0)
		zstart();
#endif
	if (ALLOCATE && !a)
	{
		zzero(bb);
		return (0);
	}
	if (!d)
        {
		zhalt("division by zero in zsdiv");
		return (0);
	}
	if ((sa = a[0]) < 0)
		sa = (-sa);
	zsetlength(&b, sa, "in zsdiv, third argument");
	if (a == *bb) a = b;
	*bb = b;
	if ((d >= RADIX) || (d <= -RADIX))
	{
		STATIC verylong zd = 0;
		STATIC verylong zb = 0;
		zintoz(d, &zb);
		zdiv(a, zb, &b, &zd);
		*bb = b;
		sa = ztoint(zd);
		FREE2SPACE(zd,zb);
		return (sa);
	}
	else
	{
		register long den = d;
		register double deninv;
		register long carry = 0;
		register long i;
		long flag = (*a < 0 ? 2 : 0) | (den < 0 ? 1 : 0);

		if (den < 0)
			den = -den;
		deninv = (double)1/den;
		if (a[sa] < den && sa > 1)
			carry = a[sa--];

		for (i = sa; i; i--)
			zdiv21(carry, a[i], den, deninv, b[i]);

		while ((sa > 1) && (!(b[sa])))
			sa--;
		b[0] = sa;
		if (flag)
		{
			if (flag <= 2)
			{
				if (!carry)
					znegate(&b);
				else
				{
					zadd(b, one, &b);
					b[0] = -b[0];
					if (flag == 1)
						carry = carry - den;
					else
						carry = den - carry;
					*bb = b;
				}
			}
			else
				carry = -carry;
		}
		return (carry);
	}
}

#endif


long 
zsmod(
	verylong a,
	long b
	)
{
	STATIC verylong q = 0;
	long y;

	y= zsdiv(a, b, &q);
	FREESPACE(q);
	return y;
}


#define correct( q, x1, x2, x3, y1, y2, btopinv) { \
	register long ix1=x1,ix2=x2,ix3=x3,iy1=y1,iy2=y2; \
	long lprodlow=0,lprodhigh=0; \
\
	zaddmulp(&lprodlow, q, iy2, &lprodhigh); \
	if ((ix3 -= lprodlow) < 0) { \
		ix3+= RADIX; \
		ix2--; \
	} \
	lprodlow=0; \
	zaddmulp(&lprodlow, q, iy1, &lprodhigh); \
	if ((ix2 -= lprodlow) < 0) { \
		ix2 += RADIX; \
		--ix1; \
	} \
	if (ix1 < lprodhigh) q--; \
	else if ((ix1 -= lprodhigh)) { \
		q += (long) ((fradix * (fradix * ix1 + ix2))*btopinv ); \
	} \
	else { \
		q += (long) ((fradix * ix2 + ix3)*btopinv ); \
	} \
}


#ifndef ALPHA50
void
zdiv(
	verylong in_a,
	verylong in_b,
	verylong *qqq,
	verylong *rrr
	)

{
	register long i;
	register long qq;
	long sa;
	long sb;
	long sq;
	verylong p;
	verylong pc;
	long sign;
	STATIC verylong a = 0;
	STATIC verylong b = 0;
	STATIC verylong c = 0;
	STATIC verylong d = 0;
	double btopinv;
#ifdef ALPHA
	double btopinv2;
#endif
	double aux;
	verylong q = *qqq;
	verylong r = *rrr;

#ifndef START
	if (fudge < 0)
		zstart();
#endif

	if (ALLOCATE && !in_a)
	{
		zzero(qqq);
		zzero(rrr);
		return;
	}
	if ((!in_b) || (((sb = in_b[0]) == 1) && (!in_b[1])))
	{
		zhalt("division by zero in zdiv");
		return;
	}
	zcopy(in_a,&a);

	zcopy(in_b,&b);

	sign = (*a < 0 ? 2 : 0) | (*b < 0 ? 1 : 0);
	if (*a < 0)
		(*a) = (-(*a));
	sa = (*a);
	if (*b < 0)
		(*b) = (-(*b));
	sb = (*b);
	zsetlength(&c, (i = (sa > sb ? sa : sb) + 1), "in zdiv, locals\n");
	zsetlength(&d, i, "");
	zsetlength(&q, i, "in zdiv, third argument");
	*qqq = q;
	zsetlength(&r, sb + 1, "in zdiv, fourth argument");
	*rrr = r;
	p = &b[sb];
	if ((sb == 1) && (*p < RADIX))
		zintoz(zsdiv(a, *p, &q), &r);
	else
	{
		sq = sa - sb;	/* size of quotient */
		btopinv = (double) (*p) * fradix;
		if (sb > 1)
			btopinv += (*(--p));
		btopinv *= fradix;
#ifdef ALPHA
		btopinv2=btopinv;
		btopinv2 = fudge2/btopinv2;
#else
		if (sb > 2)
			btopinv += (*(p - 1));
#endif
		btopinv = fudge / btopinv;
		p = &a[1];
		pc = &c[0];
		for (i = sa; i > 0; i--)
			*pc++ = *p++;
		p = pc;
		sa = 1 - sb;
		for (i = (-sq); i > 0; i--)
			*p++ = 0;
		*pc = 0;
		d[1] = 0;
		p = &d[sq + 1];
		for (i = sq; i >= 0; i--)
		{

			aux = fradix * (fradix * (*pc) + (*(pc - 1))) + 1.0;
#ifndef ALPHA
			if (i > sa)
				aux += (*(pc - 2));
#endif
#ifdef ALPHA
			qq = (long) (btopinv2 * aux + 1.0);
#else
			qq = (long) (btopinv * aux + 0.5);
		/* dirty, but safe. On most machines (long)(btopinv*aux) */
		/* is correct, or one too big; on some however it becomes */
		/* too small. Could change zstart, but +0.5 and a while */
		/* instead of one single if is safer */
#endif
			if (qq > 0)
			{
#ifdef ALPHA
				if (qq > (1L<<48)) {
					correct(qq,*pc,*(pc-1),(i>sa) ? *(pc-2):0,b[sb],b[sb-1],btopinv);
					if (qq >= RADIX)
						qq = RADIX-1;
				}
#else
				if (qq >= RADIX)
					qq = RADIX-1;
#endif
				zsubmul(qq, &c[i], &b[0]);
				while (*pc < 0)
				{
					qq--;
					{
						zaddmulone(&c[i], &b[0]);
					}
				}
			}
			pc--;
			*p-- = qq;
		}
		sb--;
		while ((sb > 0) && (!(c[sb])))
			sb--;
		sb++;
		r[0] = sb;
		p = &r[1];
		pc = &c[0];
		for (i = sb; i > 0; i--)
			*p++ = *pc++;
		if (sq < 0)
		{
			q[0] = 1;
			q[1] = d[1];
		}
		else
		{
			sq++;
			while ((sq > 1) && (!(d[sq])))
				sq--;
			q[0] = sq;
			p = &q[1];
			pc = &d[1];
			for (i = sq; i > 0; i--)
				*p++ = *pc++;
		}
	}	/* non-trivial case */

	if (sign)
	{
		if (sign <= 2)
		{
			if (!(r[1]) && (r[0] == 1))
				q[0] = -(q[0]);
			else
			{
				zadd(q, one, &q);
				q[0] = -q[0];
				if (sign == 1)
					zsub(r, b, &r);
				else
					zsub(b, r, &r);
			}
		}
		else
			znegate(&r);
	}
	*qqq = q;
	*rrr = r;
	FREE2SPACE(a,b); FREE2SPACE(c,d);
}


void
zmod(
	verylong in_a,
	verylong in_b,
	verylong *rr
	)
{
	register long i;
	register long qq;
	verylong r = *rr;
	STATIC verylong a = 0;
	STATIC verylong b = 0;
	STATIC verylong c = 0;
	long sa;
	long sb;
	long sq;
	verylong p;
	verylong pc;
	long sign;
	double btopinv;
#ifdef ALPHA
        double btopinv2;
#endif
	double aux;

/*printf("in zmod: "); zwrite(in_a); printf(" mod "); zwriteln(in_b); fflush(stdout);
*/
#ifndef START
	if (fudge < 0)
		zstart();
#endif
	if (ALLOCATE && !in_a)
	{
		zzero(rr);
		return;
	}
	if ((!in_b) || (((sb = in_b[0]) == 1) && (!in_b[1])))
	{
		zhalt("division by zero in zmod");
		return;
	}
	zcopy(in_a,&a);
	zcopy(in_b,&b);
	sign = (*a < 0 ? 2 : 0) | (*b < 0 ? 1 : 0);
	if (*a < 0)
		(*a) = (-(*a));
	sa = (*a);
	if (*b < 0)
		(*b) = (-(*b));
	sb = (*b);
	zsetlength(&c, (sa > sb ? sa : sb) + 1, "in zmod, local");
	zsetlength(&r, sb + 1, "in zmod, third argument");
	*rr = r;
	p = &b[sb];
	if ((sb == 1) && (*p < RADIX))
		zintoz(zsdiv(a, *p, &c), &r);
	else
	{
		sq = sa - sb;
		btopinv = (double) (*p) * fradix;
		if (sb > 1)
			btopinv += (*(--p));
		btopinv *= fradix;
#ifdef ALPHA
                btopinv2=btopinv;
		btopinv2 = fudge2/btopinv2;
#else
                if (sb > 2)
                        btopinv += (*(p - 1));
#endif
		btopinv = fudge / btopinv;
		p = &a[1];
		pc = &c[0];
		for (i = sa; i > 0; i--)
			*pc++ = *p++;
		p = pc;
		sa = 1 - sb;
		for (i = (-sq); i > 0; i--)
			*p++ = 0;
		*pc = 0;
		for (i = sq; i >= 0; i--)
		{
			aux = fradix * (fradix * (*pc) + (*(pc - 1))) + 1.0;
#ifdef ALPHA
			qq = (long) (btopinv2 * aux + 1.0);
#else
			if (i > sa)
				aux += (*(pc - 2));
			qq = (long) (btopinv * aux + 0.5);

		/* see comment in zdiv */
#endif
			if (qq > 0)
			{
#ifdef ALPHA
				if (qq > (1L<<48)) {
					correct(qq,*pc,*(pc-1),(i>sa) ? *(pc-2):0,b[sb],b[sb-1],btopinv);
					if (qq >= RADIX)
						qq = RADIX-1;
				}
#else
				if (qq >= RADIX)
					qq = RADIX-1;
#endif
				zsubmul(qq, &c[i], &b[0]);
				while (*pc < 0)
				{
					{
						zaddmulone(&c[i], &b[0]);
					}
				}
			}
			pc--;
		}	/* loop on i */
		sb--;
		while ((sb > 0) && (!(c[sb])))
			sb--;
		sb++;
		r[0] = sb;
		p = &r[1];
		pc = &c[0];
		for (i = sb; i > 0; i--)
			*p++ = *pc++;
	}	/* non-trivial case */

	if (sign)
	{
		if ((sign <= 2) && (!((r[0] == 1) && !(r[1]))))
		{
			if (sign == 1)
				zsub(r, b, &r);
			else
				zsub(b, r, &r);
		}
		else
			znegate(&r);	/* negating r==0 doesn't hurt */
	}
	*rr = r;
	FREE2SPACE(a,b); FREESPACE(c);

}
#endif


void
zaddmod(
	verylong a,
	verylong b,
	verylong n,
	verylong *c
	)
{
	if (ALLOCATE && !n)
	{
		zhalt("modulus zero in zaddmod");
		return;
	}
	if (ALLOCATE && !a)
	{
		if (b)
			zcopy(b, c);
		else
			zzero(c);
		return;
	}
	if (ALLOCATE && !b)
	{
		zcopy(a, c);
		return;
	}
	zadd(a, b, c);
	if (zcompare(*c, n) >= 0)
		zsubpos(*c, n, c);

}

void
zsubmod(
	verylong a,
	verylong b,
	verylong n,
	verylong *c
	)
{
	if (ALLOCATE && !n)
	{
		zhalt("modulus zero in zsubmod");
		return;
	}
	if (ALLOCATE && !b)
	{
		if (a)
			zcopy(a, c);
		else
			zzero(c);
		return;
	}
	if (ALLOCATE && !a)
	{
		if (b[1] || (b[0] != 1))
			zsubpos(n, b, c);
		else
			zzero(c);
		return;
	}
	zsub(a, b, c);
	if ((*c)[0] < 0)
		zadd(*c, n, c);
}

void
zsmulmod(
	verylong a,
	long d,
	verylong n,
	verylong *c
	)
{
	STATIC verylong mem = 0;

	if (ALLOCATE && !n)
	{
		zhalt("modulus zero in zsmulmod");
		return;
	}
	if (ALLOCATE && (!a || !d))
	{
		zzero(c);
		return;
	}
	zsmul(a, d, &mem);
	zmod(mem, n, c);
	FREESPACE(mem);
}

void
zmulmod(
	verylong a,
	verylong b,
	verylong n,
	verylong *c
	)
{
	STATIC verylong mem = 0;

	if (ALLOCATE && !n)
	{
		zhalt("modulus zero in zmulmod");
		return;
	}
	if (ALLOCATE && (!a || !b))
	{
		zzero(c);
		return;
	}
	zsetlength(&mem, a[0] + b[0] + 2, "in zmulmod, local");
	zmul(a, b, &mem);
	zmod(mem, n, c);
	FREESPACE(mem);
}

void
zmulinmod(
	verylong a,
	verylong *b,
	verylong n
	)
{
	if (ALLOCATE && !n)
	{
		zhalt("modulus zero in zmulinmod");
		return;
	}
	if (ALLOCATE && (!a || !(*b)))
	{
		zzero(b);
		return;
	}
	zmulin(a, b);
	zmod(*b, n, b);
}

void
zsqmod(
	verylong a,
	verylong n,
	verylong *c
	)
{
	STATIC verylong mem = 0;

	if (ALLOCATE && !n)
	{
		zhalt("modulus zero in zsqmod");
		return;
	}
	if (ALLOCATE && !a)
	{
		zzero(c);
		return;
	}
	zsetlength(&mem, 2 * a[0] + 2, "in zsqmod, local");
	zsq(a, &mem);
	zmod(mem, n, c);
	FREESPACE(mem);
}

void
zsqinmod(
	verylong *a,
	verylong n
	)
{
	if (ALLOCATE && !n)
	{
		zhalt("modulus zero in zsqinmod");
		return;
	}
	if (ALLOCATE && !(*a))
	{
		zzero(a);
		return;
	}
	zsqin(a);
	zmod(*a, n, a);
}

void
zdivmod(
	verylong a,
	verylong b,
	verylong n,
	verylong *c
	)
{
	STATIC verylong g = 0;
	STATIC verylong na = 0;
	STATIC verylong nb = 0;
	STATIC verylong r = 0;

	if (ALLOCATE && !n)
	{
		zhalt("modulus zero in zdivmod");
		return;
	}
	if (ALLOCATE && !a)
	{
		zzero(c);
		return;
	}
	if (ALLOCATE && !b)
	{
		zhalt("division by zero in zdivmod");
		return;
	}
	zgcd(a, b, &g);
	if ((g[1] == 1) && (g[0] == 1))
	{
		if (zinv(b, n, &g))
		{
			zhalt("undefined quotient in zdivmod");
			zcopy(g,c);
		}
		else
			zmulmod(a, g, n, c);
	}
	else
	{
		zdiv(a, g, &na, &r);
		zdiv(b, g, &nb, &r);
		if (zinv(nb, n, &g))
		{
			zhalt("undefined quotient in zdivmod");
                        zcopy(g,c);
                }
		else
			zmulmod(na, g, n, c);
	}
	FREE2SPACE(g,na); FREE2SPACE(nb,r);
}

void
zinvmod(
	verylong a,
	verylong n,
	verylong *c
	)
{
	if (ALLOCATE && !n)
	{
		zhalt("modulus zero in zinvmod");
		return;
	}
	if (ALLOCATE && !a)
	{
		zhalt("division by zero in zinvmod");
		return;
	}
	if (zinv(a, n, c))
		zhalt("undefined inverse in zinvmod");
}

static void
zmstartint(
	verylong n
	)
{
	STATIC verylong x = 0;
	long i;

	if (!n || !(n[1] & 1) || ((zntop = n[0]) < 1)
			|| ((zntop == 1) && (n[1] <= 1)))
	{
		zhalt("even or negative modulus in zmstart");
		return;
	}
	if (n[zntop] >= (RADIX >> 1))
		zntop++;
	zsetlength(&x, (long) (zntop + 1), "in zmstart, local");
	zsetlength(&zn, (long) (zntop + 1), "in zmstart, globals\n");
	zsetlength(&zr, (long) (zntop + 1), "");
	zsetlength(&zrr, (long) (zntop + 1), "");
	zsetlength(&zrrr, (long) (zntop + 1), "");
	zsetlength(&znm, (long) (zntop + 1), "");
	if (zcompare(n, zn))
	{
		zcopy(n, &zn);
#ifdef PLAIN_OR_KARAT
		zninv1 = zinvs(RADIX-zn[1],RADIX);
		zninv2 = zninv1 >> NBITSH;
		zninv1 &= RADIXROOTM;
#else
		zninv = zinvs(RADIX-zn[1],RADIX);
#endif
		for (i = 1; i <= zntop; i++)
			x[i] = 0;
		x[zntop + 1] = 1;
		x[0] = zntop + 1;
		zdiv(x, zn, &zrr, &zr);
		zsqmod(zr, zn, &zrr);
		zmulmod(zr, zrr, zn, &zrrr);
		zsubmod(zn, zr, zn, &znm);
	}
	FREESPACE(x);
}

static void
zmkeep(
	verylong n
	)
{
	if (znotinternal)
		zcopy(zn, &zoldzn);
	zmstartint(n);
}

static void
zmback(
	)
{
	if (znotinternal)
		zmstartint(zoldzn);
}

void
zmstart(
	verylong n
	)
{
	zmstartint(n);
	znotinternal = 1;
}
void
zmfree(
	)
{
	znotinternal = 0;
}

void
ztom(
	verylong a,
	verylong *bb
	)
{
	if (!zn)
	{
		zhalt("undefined Montgomery modulus in ztom");
		return;
	}
	if (ALLOCATE && !a)
	{
		zzero(bb);
		return;
	}
	if ((zscompare(a,0)<0) || (zcompare(zn, a) < 0))
	{
		zmod(a, zn, bb);
		zmontmul(*bb, zrr, bb);
	}
	else
		zmontmul(a, zrr, bb);
}

void
zmtoz(
	verylong a,
	verylong *bb
	)
{
	if (!zn)
	{
		zhalt("undefined Montgomery modulus in zmtoz");
		return;
	}
	if (ALLOCATE && !a)
	{
		zzero(bb);
		return;
	}
	zmontmul(a, one, bb);
}

void
zmontadd(
	verylong a,
	verylong b,
	verylong *c
	)
{
	if (!zn)
	{
		zhalt("undefined Montgomery modulus in zmontadd");
		return;
	}
	zaddmod(a, b, zn, c);
}

void
zmontsub(
	verylong a,
	verylong b,
	verylong *c
	)
{
	if (!zn)
	{
		zhalt("undefined Montgomery modulus in zmontsub");
		return;
	}
	zsubmod(a, b, zn, c);
}

void
zsmontmul(
	verylong a,
	long d,
	verylong *c
	)
{
	if (!zn)
	{
		zhalt("undefined Montgomery modulus in zsmontmul");
		return;
	}
	zsmulmod(a, d, zn, c);
}

void
zmontmul(
	verylong a,
	verylong b,
	verylong *cc
	)
{
	register long i;
	register long j;
	verylong c = *cc;
	STATIC verylong x = 0;
	verylong px;
	verylong pc;

	if (!zn)
	{
		zhalt("undefined Montgomery modulus in zmontmul");
		return;
	}
	if (ALLOCATE && (!a || !b))
	{
		zzero(cc);
		return;
	}
	zsetlength(&x, (i = (zntop << 1) + 1), "in zmontmul, local");
	zsetlength(&c, zntop, "in zmontmul, third argument");
	if (a == *cc) a = c;
	if (b == *cc) b = c;
	*cc = c;
        if (!kar_mem_initialized)
        {
                kar_mem_initialized = 1;
                for (j = (5 * KAR_DEPTH) - 1; j >= 0; j--)
                        kar_mem[j] = (verylong) 0;
        }
	if (*a <= *b)
		kar_mul(b, a, &x, (long) 0);
	else
		kar_mul(a, b, &x, (long) 0);
	for (; i > x[0]; i--)
		x[i] = (long) 0;
	px = &x[1];
	for (i = zntop; i > 0; i--)
		zmmulp(px++);
	pc = &c[1];
	for (i = zntop; i > 0; i--)
		*pc++ = *px++;
	i = zntop;
	while ((i > 1) && (!(*--pc)))
		i--;
	c[0] = i;
	if (zcompare(c, zn) >= 0)
		zsubpos(c, zn, &c);
	*cc = c;
	FREESPACE(x);
}

void
zmontsq(
	verylong a,
	verylong *cc
	)
{
	register long i;
	register long j;
	verylong c = *cc;
	STATIC verylong x = 0;
	verylong px;
	verylong pc;

	if (!zn)
	{
		zhalt("undefined Montgomery modulus in zmontsq");
		return;
	}
	if (ALLOCATE && !a)
	{
		zzero(cc);
		return;
	}
	zsetlength(&x, (i = (zntop << 1) + 1), "in zmontsq, local");
	zsetlength(&c, zntop, "in zmontsq, third argument");
	if (a == *cc) a = c;
	*cc = c;
        if (!kar_mem_initialized)
        {
                kar_mem_initialized = 1;
                for (j = (5 * KAR_DEPTH) - 1; j >= 0; j--)
                        kar_mem[j] = (verylong) 0;
        }
	kar_sq(a, &x, (long) 0);
	for (; i > x[0]; i--)
		x[i] = (long) 0;
	px = &x[1];
	for (i = zntop; i > 0; i--)
		zmmulp(px++);
	pc = &c[1];
	for (i = zntop; i > 0; i--)
		*pc++ = *px++;
	i = zntop;
	while ((i > 1) && (!(*--pc)))
		i--;
	c[0] = i;
	if (zcompare(c, zn) >= 0)
		zsubpos(c, zn, &c);
	*cc = c;
	FREESPACE(x);
}

void
zmontdiv(
	verylong a,
	verylong b,
	verylong *c
	)
{
	STATIC verylong g = 0;
	STATIC verylong na = 0;
	STATIC verylong nb = 0;
	STATIC verylong r = 0;

	if (!zn)
	{
		zhalt("undefined Montgomery modulus in zmontdiv");
		return;
	}
	if (ALLOCATE && !a)
	{
		zzero(c);
		return;
	}
	if (ALLOCATE && !b)
	{
		zhalt("division by zero in zmontdiv");
		return;
	}
	zgcd(a, b, &g);
	if ((g[1] == 1) && (g[0] == 1))
	{
		if (zinv(b, zn, &g))
		{
			zhalt("undefined quotient in zmontdiv");
			zcopy(g,c);
			FREESPACE(g);
			return;
		}
		else
			zmontmul(g, a, &g);
	}
	else
	{
		zdiv(a, g, &na, &r);
		zdiv(b, g, &nb, &r);
		if (zinv(nb, zn, &g))
		{
			zhalt("undefined quotient in zmontdiv");
			zcopy(g,c);
                        return;
		}
		else
			zmontmul(g, na, &g);
	}
	zmontmul(g, zrrr, c);
	FREE2SPACE(g,na); FREE2SPACE(nb,r);
}

void
zmontinv(
	verylong a,
	verylong *c
	)
{
	if (!zn)
	{
		zhalt("undefined Montgomery modulus in zmontinv");
		return;
	}
        if (ALLOCATE && !a)
	{
                zhalt("division by zero in zmontinv");
		return;
	}
	if (zinv(a, zn, c))
	{
		zhalt("undefined inverse in zmontinv");
		return;
	}
	zmontmul(*c, zrrr, c);
}


void
zrstarts(
	long s
	)
{
	register long i;
	register long ones;
	register long res;
	STATIC verylong three = 0;

	zintoz(s, &zseed);
	if (!three)
	{
		zintoz((long) 3, &three);
		ones = 107 / NBITS;
		res = 107 % NBITS;
		zsetlength(&zranp, ones + 1, "in zrstarts, zranp");
		for (i = ones; i; i--)
			zranp[i] = RADIXM;
		if (res)
			zranp[++ones] = (RADIXM >> (NBITS - res));
		zranp[0] = ones;
	}
	zsexpmod(three, 121L, zranp, &zprroot);
	FREESPACE(three);

}

void
zrstart(
	verylong s
	)
{
	if (!zseed)
		zrstarts((long) 7157891);
	zmod(s, zranp, &zseed);
}

void
zrseed(
	verylong *s
	)
{
	zcopy(zseed,s);
}

long 
zrandom(
	long b
	)
{
	STATIC verylong zb = 0;
	STATIC verylong bound = 0;
	long ret;

	if (b <= 0)
		return (0);
	if (!zseed)
		zrstarts((long) 7157891);	/* guess how random 7157891
						 * is */
	zintoz(b, &zb);
	zmod(zranp, zb, &bound);
	zsubpos(zranp, bound, &bound);
	do
	{
		zmulmod(zseed, zprroot, zranp, &zseed);
	} while (zcompare(zseed, bound) >= 0);	/* I agree, this is sick */
	zmod(zseed, zb, &zb);
	ret = (ztoint(zb));
	FREE2SPACE(zb,bound);
	return ret;
}


long 
zinvodds(
	long n,
	long p
	)
{
	register long n1 = n;
	register long n2 = p;
	register long preg = p;
	register long m1 = 1;
	register long m2 = 0;

 /* m1*n == n1 mod p,  m2*n == n2 mod p */
 /* n2 is odd, n1 is odd after initialization */
	if (!(n2 & 1))
		return (0);
	while (!(n1 & 1))
	{
		n1 >>= 1;
		if (m1 & 1)
			m1 += preg;
		m1 >>= 1;
	}
	if (n1 == 1)
		return (m1);
	while (n1 != n2)
	{
		if (n1 > n2)
		{
			n1 -= n2;
			m1 -= m2;
			if (m1 < 0)
				m1 += preg;
			do
			{
				n1 >>= 1;
				if (m1 & 1)
					m1 += preg;
				m1 >>= 1;
			} while (!(n1 & 1));
			if (n1 == 1)
				return (m1);
		}
		else
		{
			n2 -= n1;
			m2 -= m1;
			if (m2 < 0)
				m2 += preg;
			do
			{
				n2 >>= 1;
				if (m2 & 1)
					m2 += preg;
				m2 >>= 1;
			} while (!(n2 & 1));
			if (n2 == 1)
				return (m2);
		}
	}
	zhalt("arguments not coprime in zinvodds");
	return (0);
}

long 
zinvs(
	long n,
	long p
	)
{
	register long q = n;
	register long r;
	register long nq;
	register long nr;
	register long u = 1;
	register long w;
	register long nw;
	register long par = 0;

	if (p < 0)
		p = -p;
	if (n < 0)
		n = -n;
	if (n > p)
	{
		w = 0;
		r = p;
	}
	else
		r = p - (w = p / n) * n;
	nr = r;
	while (nr != 0)
	{
		if ((nr = q - r) < r)
			nq = 1;
		else if ((nr -= r) < r)
			nq = 2;
		else
			nr = q - (nq = q / r) * r;
		nw = nq * w + u;
		u = w;
		w = nw;
		q = r;
		r = nr;
		par = 1 - par;
	}
	if (par == 0)
		return (u);
	else
		return (p - u);
}

static long 
zxxeucl(
	verylong ain,
	verylong nin,
	verylong *invv,
	verylong *uu
	)
{
	STATIC verylong a = 0;
	STATIC verylong n = 0;
	STATIC verylong q = 0;
	STATIC verylong w = 0;
	STATIC verylong x = 0;
	STATIC verylong y = 0;
	STATIC verylong z = 0;
	verylong inv = *invv;
	verylong u = *uu;
	long diff;
	long ilo;
	long sa;
	long sn;
	long temp;
	long e;
	long fast;
	long parity;
	long gotthem;
	verylong pin;
	verylong p;
	long i;
	long try11;
	long try12;
	long try21;
	long try22;
	long got11;
	long got12;
	long got21;
	long got22;
	double hi;
	double lo;
	double dt;
	double fhi;
	double flo;
	double num;
	double den;
	double dirt;

#ifndef START
	if (fudge < 0)
		zstart();
#endif
	zsetlength(&a, (e = (ain[-1] > nin[-1] ? ain[-1] : nin[-1])), 
	           "in zxxeucl, locals\n");
	zsetlength(&n, e, "");
	zsetlength(&q, e, "");
	zsetlength(&w, e, "");
	zsetlength(&x, e, "");
	zsetlength(&y, e, "");
	zsetlength(&z, e, "");
	pin = &ain[0];
	p = &a[0];
	for (i = (*pin); i >= 0; i--)
		*p++ = *pin++;
	zsetlength(&inv, e, "in zxxeucl, third argument");
	*invv = inv;
	zsetlength(&u, e, "in zxxeucl, fourth argument");
	*uu = u;
	fhi = 1.0 + epsilon;
	flo = 1.0 - epsilon;
	pin = &nin[0];
	p = &n[0];
	for (i = (*pin); i >= 0; i--)
		*p++ = *pin++;
	inv[0] = 1;
	inv[1] = 1;
	w[0] = 1;
	w[1] = 0;
	while (n[0] > 1 || n[1] > 0)
	{
		gotthem = 0;
		sa = a[0];
		sn = n[0];
		diff = sa - sn;
		if (!diff || diff == 1)
		{
			sa = a[0];
			p = &a[sa];
			num = (double) (*p) * fradix;
			if (sa > 1)
				num += (*(--p));
			sn = n[0];
			p = &n[sn];
			den = (double) (*p) * fradix;
			if (sn > 1)
				den += (*(--p));
 			hi = fhi * (num + 1.0) / den;
			lo = flo * num / (den + 1.0);
			if (diff > 0)
			{
				hi *= fradix;
				lo *= fradix;
			}
			try11 = 1;
			try12 = 0;
			try21 = 0;
			try22 = 1;
			parity = 1;
			fast = 1;
			while (fast > 0)
			{
				parity = 1 - parity;
				if (hi >= fradix)
					fast = 0;
				else
				{
					ilo = (long)lo;
					dirt = hi - ilo;
					if (!dirt || !ilo || ilo < (long)hi)
						fast = 0;
					else
					{
						dt = lo;
						lo = flo / dirt;
						if (dt > ilo)
							hi = fhi / (dt - ilo);
						else
							hi = fradix;
						temp = try11;
						try11 = try21;
						if ((RADIX - temp) / ilo < try21)
							fast = 0;
						else
							try21 = temp + ilo * try21;
						temp = try12;
						try12 = try22;
						if ((RADIX - temp) / ilo < try22)
							fast = 0;
						else
							try22 = temp + ilo * try22;
						if ((fast > 0) && (parity > 0))
						{
							gotthem = 1;
							got11 = try11;
							got12 = try12;
							got21 = try21;
							got22 = try22;
						}
					}
				}
			}
		}
		if (gotthem)
		{
			zsmul(inv, got11, &x);
			zsmul(w, got12, &y);
			zsmul(inv, got21, &z);
			zsmul(w, got22, &w);
			zadd(x, y, &inv);
			zadd(z, w, &w);
			zsmul(a, got11, &x);
			zsmul(n, got12, &y);
			zsmul(a, got21, &z);
			zsmul(n, got22, &n);
			zsub(x, y, &a);
			zsub(n, z, &n);
		}
		else
		{
			zdiv(a, n, &q, &a);
			zmul(q, w, &x);
			zadd(inv, x, &inv);
			if (a[0] > 1 || a[1] > 0)
			{
				zdiv(n, a, &q, &n);
				zmul(q, inv, &x);
				zadd(w, x, &w);
			}
			else
			{
				p = &a[0];
				pin = &n[0];
				for (i = (*pin); i >= 0; i--)
					*p++ = *pin++;
				n[0] = 1;
				n[1] = 0;
				zsub(nin, w, &inv);
			}
		}
	}
	if ((a[0] == 1) && (a[1] == 1))
		e = 0;
	else
		e = 1;
	p = &u[0];
	pin = &a[0];
	for (i = (*pin); i >= 0; i--)
		*p++ = *pin++;
	*invv = inv;
	*uu = u;
	FREE2SPACE(a,n); FREE2SPACE(q,w); FREE2SPACE(x,y); FREESPACE(z);
	return (e);
}

long 
zinv(
	verylong ain,
	verylong nin,
	verylong *invv
	)
{
	STATIC verylong u = 0;

	if ((!nin) || (!ain) || (ain[0] < 0) || (nin[0] < 0))
	{
		zhalt("zero or negative argument(s) in zinv");
		return (0);
	}
	if (!(zxxeucl(ain, nin, invv, &u))) {
		FREESPACE(u);
		return (0);
	}
	zcopy(u, invv);
	FREESPACE(u);
	return (1);
}

void
zexteucl(
	verylong a,
	verylong *xa,
	verylong b,
	verylong *xb,
	verylong *d
	)
{
	STATIC verylong modcon = 0;
	register long agrb;
	register long anegative = 0;
	register long bnegative = 0;

	if ((anegative = (a[0] < 0)))
		a[0] = -a[0];
	if ((bnegative = (b[0] < 0)))
		b[0] = -b[0];
	if (!a[1] && (a[0] == 1))
	{
		zzero(xa);
		zone(xb);
		zcopy(b, d);
		goto done;
	}
	if ((!b[1] && (b[0] == 1)) || !(agrb = zcompare(a, b)))
	{
		zone(xa);
		zzero(xb);
		zcopy(a, d);
		goto done;
	}
	if (agrb > 0)
	{
		zxxeucl(a, b, xa, d);
		zmul(a, *xa, xb);
		zsub(*d, *xb, xb);
		zdiv(*xb, b, xb, &modcon);
	}
	else
	{
		zxxeucl(b, a, xb, d);
		zmul(b, *xb, xa);
		zsub(*d, *xa, xa);
		zdiv(*xa, a, xa, &modcon);
	}
	if ((modcon[1]) || (modcon[0] != 1))
	{
		zhalt("non-zero remainder in zexteucl   BUG");
		FREESPACE(modcon);
		return;
	}
done:
	if (anegative)
	{
		a[0] = -a[0];
		znegate(xa);
	}
	if (bnegative)
	{
		b[0] = -b[0];
		znegate(xb);
	}
	FREESPACE(modcon);
}

void
zchirem(
	verylong aa,
	verylong maa,
	verylong bb,
	verylong mbb,
	verylong *d
	)
{
	STATIC verylong u = 0;
	STATIC verylong v = 0;
	STATIC verylong w = 0;
	verylong a;
	verylong b;
	verylong ma;
	verylong mb;
	register long agrb;

	if ((!aa) || (!bb) || 
	   (!aa[1] && aa[0] == 1) || (!bb[1] && bb[0] == 1) ||
			(aa[0] < 0) || (bb[0] < 0))
	{
		zhalt("zero or negative argument(s) in zchirem");
return;
        }
	if ((agrb = aa[0]) < bb[0])
		agrb = bb[0];
	zsetlength(&u, agrb, "in zchirem, locals");
	zsetlength(&v, agrb, "");
	zsetlength(&w, agrb, "");
	if (!(agrb = zcompare(aa, bb)))
	{
		zsub(maa, mbb, &u);
		zmod(u, aa, &w);
		if (w[1] || w[0] != 1)
		{
			zhalt("same moduli with different remainders in zchirem");
			FREE2SPACE(u,v); FREESPACE(w);
			return;
		}
		zcopy(maa, d);
		FREE2SPACE(u,v); FREESPACE(w);
		return;
	}
	else if (agrb > 0)
	{
		a = aa;
		b = bb;
		ma = maa;
		mb = mbb;
	}
	else
	{
		a = bb;
		b = aa;
		ma = mbb;
		mb = maa;
	}
	if (zxxeucl(a, b, &u, &w))
	{
		zhalt("moduli not coprime in zchirem");
		FREE2SPACE(u,v); FREESPACE(w);
		return;
	}
	zmod(ma, a, &v);
	zsub(mb, v, &w);
	zmulmod(w, u, b, &w);
	zmul(w, a, &u);
	zadd(v, u, d);
	FREE2SPACE(u,v); FREESPACE(w);
}

void
zmontexp(
	verylong a,
	verylong e,
	verylong *bb
	)
{
	register long i;
	register long j;
	register long k = 0;
	verylong b = *bb;
	STATIC verylong loca = 0;

	if (!zn)
	{
		zhalt("undefined Montgomery modulus in zmontexp");
		return;
	}
	if (ALLOCATE && !e)
	{
		zcopy(zr,bb);
		return;
	}
	if (ALLOCATE && !a)
	{
		zzero(bb);
		return;
	}
	if ((i = e[0]) < 0)
		i = (-i);
	if ((i == 1) && (!(e[1])))
		zcopy(zr,&b);
	else
	{
		zcopy(a, &loca);
		zcopy(loca, &b);
		for (; i; i--)
		{
			j = e[i];
			if (!k)
			{
				k = 1;
				while ((k << 1) <= j)
					k <<= 1;
			}
			while (k >>= 1)
			{
				zmontsq(b, &b);
				if (j & k)
					zmontmul(loca, b, &b);
			}
			k = RADIX;
		}
	}
	if (e[0] < 0)
	{
		if (zinv(b, zn, &b))
		{
			zhalt("undefined quotient in zmontexp");
			FREESPACE(loca);
			return;
		}
		zmontmul(b, zrrr, &b);
	}
	*bb = b;
	FREESPACE(loca);
}

void
zsexpmod(
	verylong a,
	long e,
	verylong n,
	verylong *bb
	)
{
	STATIC verylong le = 0;

	zintoz(e, &le);
	zexpmod(a, le, n, bb);
	FREESPACE(le);
}

void
zexpmod(
	verylong a,
	verylong e,
	verylong n,
	verylong *bb
	)
{
	register long i;
	register long j;
	register long k = 0;
	verylong b = *bb;
	STATIC verylong loca = 0;

	if (ALLOCATE && !n)
	{
		zhalt("modulus zero in zexpmod");
		return;
	}
	if (ALLOCATE && !e)
	{
		zone(bb);
		return;
	}
	if (ALLOCATE && !a)
	{
		zzero(bb);
		return;
	}
	if ((i = e[0]) < 0)
		i = (-i);
	if ((i == 1) && (!(e[1])))
		zone(&b);
	else
	{
		zmod(a, n, &loca);
		zcopy(loca, &b);
		for (; i; i--)
		{
			j = e[i];
			if (!k)
			{
				k = 1;
				while ((k << 1) <= j)
					k <<= 1;
			}
			while (k >>= 1)
			{
				zsqmod(b, n, &b);
				if (j & k)
					zmulmod(loca, b, n, &b);
			}
			k = RADIX;
		}
	}
	if (e[0] < 0 && zinv(b, n, &b))
		zhalt("undefined quotient in zexpmod");
	*bb = b;
	FREESPACE(loca);
}

void
z2expmod(
	verylong e,
	verylong n,
	verylong *bb
	)
{
	register long i;
	register long j;
	register long k = 0;
	verylong b = *bb;

	if (ALLOCATE && !n)
	{
		zhalt("modulus zero in z2expmod");
		return;
	}
	if (ALLOCATE && !e)
	{
		zone(bb);
		return;
	}
	if ((i = e[0]) < 0)
		i = (-i);
	if ((i == 1) && (!(e[1])))
		zone(&b);
	else
	{
		if ((j = n[0]) < 0)
			j = -j;
		zsetlength(&b, j, "in z2expmod, third argument");
		*bb = b;
		zintoz((long) 2, &b);
		for (; i; i--)
		{
			j = e[i];
			if (!k)
			{
				k = 1;
				while ((k << 1) <= j)
					k <<= 1;
			}
			while (k >>= 1)
			{
				zsqmod(b, n, &b);
				if (j & k)
				{
					z2mul(b, &b);
					if (zcompare(b, n) >= 0)
						zsubpos(b, n, &b);
				}
			}
			k = RADIX;
		}
	}
	if (e[0] < 0 && zinv(b, n, &b))
		zhalt("undefined quotient in z2expmod");
	*bb = b;
}

long 
zdefault_m(
	long l
	)
{
	if (l < 2)
		return (2);
	if (l < 3)
		return (3);
	if (l < 7)
		return (4);
	if (l < 16)
		return (5);
	if (l < 35)
		return (6);
	if (l < 75)
		return (7);
	if (l < 160)
		return (8);
	if (l < 340)
		return (9);
	return (10);
}

static void 
mont_init_odd_power(
	verylong a,
	verylong *a_sq,
	long m,
	verylong **odd_a_power
	)
{
 /* odd power setter for montgomery m-ary exponentiation */
 /* initialize odd_a_power with a^i mod n for odd positive i < 2^m */
 /* a on input is montgomery number */
	register long i;
	register long length = (1L << (m - 1));

	if (!(*odd_a_power))
	{
		*odd_a_power = (verylong *)malloc((size_t)(length * sizeof(verylong)));
		for (i = 0; i < length; i++)
			(*odd_a_power)[i] = (verylong) 0;
	}
	zcopy(a, &((*odd_a_power)[0]));
	zmontsq((*odd_a_power)[0], a_sq);
}

void
zmontexp_m_ary(
	verylong a,
	verylong e,
	verylong *bb,
	long m
	)
{
	register long i;
	register long ei;
	STATIC verylong a_sq = 0;

	if (!zn)
	{
		zhalt("undefined Montgomery modulus in zmontexp_m_ary");
		return;
	}
	if (ALLOCATE && !e)
	{
		zcopy(zr,bb);
		return;
	}
	if (ALLOCATE && !a)
	{
		zzero(bb);
		return;
	}
	if ((i = e[0]) < 0)
		i = (-i);
	if (i == 1)
	{
		zmontexp(a, e, bb);
		return;
	}
	if (!exp_odd_powers)
	{
		exp_odd_powers = (verylong **)malloc((size_t)(NBITS * sizeof(verylong *)));
		for (ei = 0; ei < NBITS; ei++)
			exp_odd_powers[ei] = (verylong *) 0;
	}
	if (m <= 1)
		m = zdefault_m(i);
	else if (m >= NBITS)
		m = (NBITS - 1);
	{
		register long k = 0;
		register long od = 0;
		register long odlen;
		register long left;
		register long zeros = 0;
		register long rem = 0;
		register long first = 1;
		register long reach = 1;
		verylong **loc_pow = &(exp_odd_powers[m]);

		mont_init_odd_power(a, &a_sq, m, loc_pow);
		for (; i > 0; i--)
		{
			ei = e[i];
			if (!k)
			{
				k = 1;
				while (k <= ei)
					k <<= 1;
			}
			while (k >>= 1)
			{
				if (zeros)
				{
					if (ei & k)
					{
						odlen = od = 1;
						rem = zeros;
						left = zeros = 0;
					}
					else
						zeros++;
				}
				else if (!od)
				{
					if (ei & k)
					{
						odlen = od = 1;
						left = zeros = 0;
					}
					else
						zeros = 1;
				}
				else
				{
					left++;
					if (odlen + left == m)
					{
						if (ei & k)
						{
							od <<= left;
							odlen += left;
							left = 0;
							od++;
							od >>= 1;
							for (; reach <= od; reach++)
								zmontmul((*loc_pow)[reach - 1], a_sq, &((*loc_pow)[reach]));
							if (first)
							{
								zcopy((*loc_pow)[od], bb);
								first = rem = odlen = od = 0;
							}
							else
							{
								for (rem = rem + odlen; rem; rem--)
									zmontsq(*bb, bb);
								zmontmul(*bb, (*loc_pow)[od], bb);
								odlen = od = 0;
							}
						}
						else
						{
							if (first)
							{
								if (od == 1)
								{
									zcopy(a_sq, bb);
									left--;
								}
								else
								{
									od >>= 1;
									for (; reach <= od; reach++)
										zmontmul((*loc_pow)[reach - 1], a_sq, &((*loc_pow)[reach]));
									zcopy((*loc_pow)[od], bb);
								}
								zeros = left;
								first = left = odlen = od = rem = 0;
							}
							else
							{
								for (rem = rem + odlen; rem; rem--)
									zmontsq(*bb, bb);
								od >>= 1;
								for (; reach <= od; reach++)
									zmontmul((*loc_pow)[reach - 1], a_sq, &((*loc_pow)[reach]));
								zmontmul(*bb, (*loc_pow)[od], bb);
								zeros = left;
								left = odlen = od = 0;
							}
						}
					}
					else if (ei & k)
					{
						od <<= left;
						odlen += left;
						left = 0;
						od++;
					}
				}
			}
			k = RADIX;
		}
		if (od)
		{
			if (first)
			{
				if (od == 1)
				{
					zcopy(a_sq, bb);
					left--;
				}
				else
				{
					od >>= 1;
					for (; reach < od; reach++)
						zmontmul((*loc_pow)[reach - 1], a_sq, &((*loc_pow)[reach]));
					zmontmul((*loc_pow)[od - 1], a_sq, bb);
				}
			}
			else
			{
				for (rem = rem + odlen; rem; rem--)
					zmontsq(*bb, bb);
				od >>= 1;
				for (; reach <= od; reach++)
					zmontmul((*loc_pow)[reach - 1], a_sq, &((*loc_pow)[reach]));
				zmontmul(*bb, (*loc_pow)[od], bb);
			}
		}
		if (left || zeros)
		{
			for (rem = zeros + left; rem; rem--)
				zmontsq(*bb, bb);
		}
	}

	FREESPACE(a_sq);
	if (e[0] < 0)
	{
		if (zinv(*bb, zn, bb))
		{
			zhalt("undefined quotient in zmontexp_m_ary");
			return;
		}
		zmontmul(*bb, zrrr, bb);
	}
}

static void 
init_odd_power(
	verylong a,
	verylong n,
	verylong *a_sq,
	long m,
	verylong **odd_a_power
	)
{
 /* odd power setter for m-ary exponentiation */
 /* initialize odd_a_power with a^i mod n for odd positive i < 2^m */
 /* a on input is not necessarily reduced mod n */
	register long i;
	register long length = (1L << (m - 1));

	if (!(*odd_a_power))
	{
		*odd_a_power = (verylong *)malloc((size_t)(length * sizeof(verylong)));
		for (i = 0; i < length; i++)
			(*odd_a_power)[i] = (verylong) 0;
	}
	zmod(a, n, &((*odd_a_power)[0]));
	zsqmod((*odd_a_power)[0], n, a_sq);
}

void
zexpmod_m_ary(
	verylong a,
	verylong e,
	verylong n,
	verylong *bb,
	long m
	)
{
	register long i;
	register long ei;
	STATIC verylong a_sq = 0;

	if (ALLOCATE && !n)
	{
		zhalt("modulus zero in zexpmod_m_ary");
		return;
	}
	if (ALLOCATE && !e)
	{
		zone(bb);
		return;
	}
	if (ALLOCATE && !a)
	{
		zzero(bb);
		return;
	}
	if ((i = e[0]) < 0)
		i = (-i);
	if (i == 1)
	{
		zexpmod(a, e, n, bb);
		return;
	}
	if (!exp_odd_powers)
	{
		exp_odd_powers = (verylong **)malloc((size_t)(NBITS * sizeof(verylong *)));
		for (ei = 0; ei < NBITS; ei++)
			exp_odd_powers[ei] = (verylong *) 0;
	}
	if (m <= 1)
		m = zdefault_m(i);
	else if (m >= NBITS)
		m = (NBITS - 1);
	{
		register long k = 0;
		register long od = 0;
		register long odlen;
		register long left;
		register long zeros = 0;
		register long rem = 0;
		register long first = 1;
		register long reach = 1;
		verylong **loc_pow = &(exp_odd_powers[m]);

		init_odd_power(a, n, &a_sq, m, loc_pow);
		for (; i > 0; i--)
		{
			ei = e[i];
			if (!k)
			{
				k = 1;
				while (k <= ei)
					k <<= 1;
			}
			while (k >>= 1)
			{
				if (zeros)
				{
					if (ei & k)
					{
						odlen = od = 1;
						rem = zeros;
						left = zeros = 0;
					}
					else
						zeros++;
				}
				else if (!od)
				{
					if (ei & k)
					{
						odlen = od = 1;
						left = zeros = 0;
					}
					else
						zeros = 1;
				}
				else
				{
					left++;
					if (odlen + left == m)
					{
						if (ei & k)
						{
							od <<= left;
							odlen += left;
							left = 0;
							od++;
							od >>= 1;
							for (; reach <= od; reach++)
								zmulmod((*loc_pow)[reach - 1], a_sq, n, &((*loc_pow)[reach]));
							if (first)
							{
								zcopy((*loc_pow)[od], bb);
								first = rem = odlen = od = 0;
							}
							else
							{
								for (rem = rem + odlen; rem; rem--)
									zsqmod(*bb, n, bb);
								zmulmod(*bb, (*loc_pow)[od], n, bb);
								odlen = od = 0;
							}
						}
						else
						{
							if (first)
							{
								if (od == 1)
								{
									zcopy(a_sq, bb);
									left--;
								}
								else
								{
									od >>= 1;
									for (; reach <= od; reach++)
										zmulmod((*loc_pow)[reach - 1], a_sq, n, &((*loc_pow)[reach]));
									zcopy((*loc_pow)[od], bb);
								}
								zeros = left;
								first = left = odlen = od = rem = 0;
							}
							else
							{
								for (rem = rem + odlen; rem; rem--)
									zsqmod(*bb, n, bb);
								od >>= 1;
								for (; reach <= od; reach++)
									zmulmod((*loc_pow)[reach - 1], a_sq, n, &((*loc_pow)[reach]));
								zmulmod(*bb, (*loc_pow)[od], n, bb);
								zeros = left;
								left = odlen = od = 0;
							}
						}
					}
					else if (ei & k)
					{
						od <<= left;
						odlen += left;
						left = 0;
						od++;
					}
				}
			}
			k = RADIX;
		}
		if (od)
		{
			if (first)
			{
				if (od == 1)
				{
					zcopy(a_sq, bb);
					left--;
				}
				else
				{
					od >>= 1;
					for (; reach < od; reach++)
						zmulmod((*loc_pow)[reach - 1], a_sq, n, &((*loc_pow)[reach]));
					zmulmod((*loc_pow)[od - 1], a_sq, n, bb);
				}
			}
			else
			{
				for (rem = rem + odlen; rem; rem--)
					zsqmod(*bb, n, bb);
				od >>= 1;
				for (; reach <= od; reach++)
					zmulmod((*loc_pow)[reach - 1], a_sq, n, &((*loc_pow)[reach]));
				zmulmod(*bb, (*loc_pow)[od], n, bb);
			}
		}
		if (left || zeros)
		{
			for (rem = zeros + left; rem; rem--)
				zsqmod(*bb, n, bb);
		}
	}

	if (e[0] < 0 && zinv(*bb, n, bb))
		zhalt("undefined result in zexpmod_m_ary");
	FREESPACE(a_sq);
}

void
zsexp(
	verylong a,
	long e,
	verylong *bb
	)
{
	STATIC verylong le = 0;

	zintoz(e, &le);
	zexp(a, le, bb);
	FREESPACE(le);
}

void
zexp(
	verylong a,
	verylong e,
	verylong *bb
	)
{
	register long i;
	register long j;
	register long k = 0;
	long sa;
	verylong b = (*bb);
	STATIC verylong temp = 0;
	extern double log10();

	if (ALLOCATE && !e)
	{
		zone(bb);
		return;
	}
	if (ALLOCATE && !a)
	{
		zzero(bb);
		return;
	}
	if ((e[0] == 1) && (!(e[1])))
		zone(bb);
	else if ((a[1] == 1) && (a[0] == 1))
		zone(bb);
	else if ((a[1] == 1) && (a[0] == -1))
	{
		if (e[1] & 1)
			zintoz((long) (-1), bb);
		else
			zone(bb);
	}
	else if (e[0] < 0)
	{
		zhalt("negative exponent in zexp");
		return;
	}
	else if (!(a[1]) && (a[0] == 1))
		zzero(bb);
	else
	{
		if ((sa = a[0]) < 0)
			sa = (-sa);
		i = (long) (zslog(a,RADIX) * ztoint(e) + 4);
		zsetlength(&b, i, "in zexp, third argument");
		zsetlength(&temp, i, "in zexp, local");
		for (i = sa; i >= 0; i--)
			b[i] = a[i];
		for (i = e[0]; i; i--)
		{
			j = e[i];
			if (!k)
			{
				k = 1;
				while ((k << 1) <= j)
					k <<= 1;
			}
			while (k >>= 1)
			{
				zsqin(&b);
				if (j & k)
					zmulin(a, &b);
			}
			k = RADIX;
		}
		*bb = b;
	}
	FREESPACE(temp);
}

long 
zexpmods(
	long a,
	long e,
	long n
	)
{
	if (!e)
		return (1);
	else
	{
		register long aa = a % n;
		register long ipow2 = 1;
		long b = aa;

		if (e<0) e = -e;
		while ((ipow2 << 1) <= e)
			ipow2 <<= 1;
		while (ipow2 >>= 1)
		{
			b = zmulmods(b, b, n);
			if (e & ipow2)
				b = zmulmods(b, aa, n);
		}
		return (b);
	}
}

void
zexpmod_doub1(
	verylong xx1,
	verylong ee1,
	verylong xx2,
	verylong ee2,
	verylong n,
	verylong *r
	)
{
        register long bit11, bit21;
        register long i,j,mask,block;
        STATIC verylong tab[2][2];
        STATIC long non_ini = 1;
        STATIC verylong e1 = 0;
        STATIC verylong e2 = 0;
        if (ALLOCATE && !n)
        {
                zhalt("modulus zero in zexpmod_doub1");
                return;
        }
        if ((zscompare(ee1,0)<0) || (zscompare(ee2,0)<0)) {
                zhalt("negative exponent in zexpmod_doub1");
                zintoz(0,r);
                return;
        }
	if (non_ini) {
		for (i=0;i<2;i++) for (j=0;j<2;j++)
			tab[i][j] = 0;
		non_ini = 0;
	}
#define MOD_INITPAR \
        if (zcompare(ee1,ee2)>=0) { \
		zcopy(xx1,&(tab[1][0])); \
		zcopy(xx2,&(tab[0][1])); \
                zcopy(ee1,&e1); \
                zcopy(ee1,&e2); /* create space for e2 */ \
                zcopy(ee2,&e2); \
        } else { \
		zcopy(xx2,&(tab[1][0])); \
		zcopy(xx1,&(tab[0][1])); \
                zcopy(ee2,&e2); /* create space for e2 */ \
                zcopy(ee1,&e2); \
                zcopy(ee2,&e1); \
        } \
        if (!(zscompare(e1,0))) { \
		zintoz(1,r); \
		FREE2SPACE(e1,e2); FREE2SPACE(tab[1][0],tab[0][1]); \
                return; \
        } \
        for (i=e2[0]+1;i<=e1[0];i++) e2[i] = 0;

	MOD_INITPAR

	zmulmod(tab[1][0],tab[0][1],n,&(tab[1][1]));

	block = z2log(e1)-1;
        mask = ( 1L<<(block % NBITS) );
        block = (block / NBITS) +1;
        bit11 = 1;
        bit21 = (e2[block]&mask) ? 1 : 0;
	zcopy(tab[1][bit21],r);

#define MOD_FIRST \
	mask >>= 1; \
	if (!mask) { \
		block --; \
		if (!block) { \
			FREE2SPACE(e1,e2); \
			FREE2SPACE(tab[1][0],tab[0][1]); \
			FREESPACE(tab[1][1]); \
		} \
		mask  = ( 1L <<(NBITS-1) ); \
	} \
	bit11 = (e1[block]&mask) ? 1 : 0; \
	bit21 = (e2[block]&mask) ? 1 : 0; 

	MOD_FIRST
	for (;;) {
		zsqmod(*r,n,r);
		if (bit11 || bit21) 
			zmulmod(tab[bit11][bit21],*r,n,r);
		MOD_FIRST
	}
	FREE2SPACE(e1,e2); FREE2SPACE(tab[1][0],tab[0][1]);
	FREE2SPACE(tab[1][1],tab[0][0]);
}

void
zexpmod_doub2(
	verylong xx1,
	verylong ee1,
	verylong xx2,
	verylong ee2,
	verylong n,
	verylong *r
	)
{
	register long i,j,mask,block;
	register long bit11, bit12, bit21, bit22;
	STATIC verylong tab[4][4];
	STATIC long non_ini = 1;
	STATIC verylong e1 = 0;
	STATIC verylong e2 = 0;
        if (ALLOCATE && !n)
        {
                zhalt("modulus zero in zexpmod_doub2");
                return;
        }
	if ((zscompare(ee1,0)<0) || (zscompare(ee2,0)<0)) {
		zhalt("negative exponent in zexpmod_doub2");
		zintoz(0,r);
		return;
	}
	if (non_ini) {
		for (i=0;i<4;i++) for (j=0;j<4;j++)
			tab[i][j] = 0;
		non_ini = 0;
	}

	MOD_INITPAR

	zsqmod(tab[1][0],n,&(tab[2][0]));
	zsqmod(tab[0][1],n,&(tab[0][2]));
	zmulmod(tab[2][0],tab[1][0],n,&(tab[3][0]));
	zmulmod(tab[0][2],tab[0][1],n,&(tab[0][3]));
	for (j=1;j<4;j++) {
		for (i=1;i<4;i++) if ((i&1) | (j&1))
			zmulmod(tab[j][0],tab[0][i],n,&(tab[j][i]));
	}

	block = z2log(e1)-1;
	mask = ( 1L<<(block % NBITS) );
	block = (block / NBITS) +1;
	bit11 = 1;
	bit21 = (e2[block]&mask) ? 1 : 0;
	mask >>= 1;
	if (!mask) {
		block --;
		if (!block) {
			zcopy(tab[1][bit21],r);
			FREE2SPACE(e1,e2);
			for (i=0; i<4;i++)
				for (j=0; j<4; j++)
					FREESPACE(tab[i][j]);
			return;
		}
		mask = ( 1L<<(NBITS-1) );
	}
	bit12 = (e1[block]&mask) ? 1 : 0;
	bit22 = (e2[block]&mask) ? 1 : 0;
	if (bit12 || bit22) {
		zcopy(tab[2+bit12][(bit21<<1)+bit22],r);
	} else {
		zcopy(tab[bit11][bit21],r);
		zsqmod(*r,n,r);
	}

#define MOD_SECOND \
	mask >>= 1; \
	if (!mask) { \
		block--; \
		if (!block) { \
			zsqmod(*r,n,r); \
			if (bit11||bit21) \
				zmulmod(tab[bit11][bit21],*r,n,r); \
			FREE2SPACE(e1,e2); \
			for (i=0; i<4;i++) \
				for (j=0; j<4; j++) \
					FREESPACE(tab[i][j]); \
			return; \
		} \
		mask = ( 1L<<(NBITS-1) ); \
	} \
        bit12 = (e1[block]&mask) ? 1 : 0; \
        bit22 = (e2[block]&mask) ? 1 : 0;

	MOD_FIRST
	MOD_SECOND
	for (;;) {
		zsqmod(*r,n,r);
		if (bit11 || bit21) {
			if (bit12 || bit22) {
			  zsqmod(*r,n,r);
			  zmulmod(tab[(bit11<<1)+bit12][(bit21<<1)+bit22],*r,n,r);
			} else {
			  zmulmod(tab[bit11][bit21],*r,n,r);
			  zsqmod(*r,n,r);
			}
			MOD_FIRST
		} else {
			bit11 = bit12;
			bit21 = bit22;
		}
		MOD_SECOND
	}
}

void
zexpmod_doub3(
	verylong xx1,
	verylong ee1,
	verylong xx2,
	verylong ee2,
	verylong n,
	verylong *r
	)
{
	register long i,j, mask, block;
	register long bit11, bit12, bit13, bit21, bit22, bit23;
	STATIC verylong tab[8][8];
	STATIC long non_ini = 1;
	STATIC verylong e1 = 0;
	STATIC verylong e2 = 0;
        if (ALLOCATE && !n)
        {
                zhalt("modulus zero in zexpmod_doub3");
                return;
        }
        if ((zscompare(ee1,0)<0) || (zscompare(ee2,0)<0)) {
                zhalt("negative exponent in zexpmod_doub3");
                zintoz(0,r);
                return;
        }
	if (non_ini) {
		for (i=0;i<8;i++) for (j=0;j<8;j++)
			tab[i][j] = 0;
		non_ini = 0;
	}

	MOD_INITPAR

	zsqmod(tab[1][0],n,&(tab[2][0]));
	zsqmod(tab[0][1],n,&(tab[0][2]));
	for (i=3;i<8;i+=2) zmulmod(tab[2][0],tab[i-2][0],n,&(tab[i][0]));
	for (i=3;i<8;i+=2) zmulmod(tab[0][2],tab[0][i-2],n,&(tab[0][i]));
	for (i=1;i<8;i++) zmulmod(tab[0][1],tab[1][i-1],n,&(tab[1][i]));
        for (i=1;i<8;i+=2) zmulmod(tab[0][i],tab[2][0],n,&(tab[2][i]));
        for (i=1;i<8;i++) zmulmod(tab[0][1],tab[3][i-1],n,&(tab[3][i]));
	for (j=4;j<8;j++) {
		zmulmod(tab[1][0],tab[j-1][1],n,&(tab[j][1]));
		for (i=3;i<8;i+=2) zmulmod(tab[0][2],tab[j][i-2],n,&(tab[j][i]));
		j++;
		for (i=1;i<8;i++) zmulmod(tab[0][1],tab[j][i-1],n,&(tab[j][i]));
	}
        block = z2log(e1)-1;
        mask = ( 1L<<(block % NBITS) );
        block = (block / NBITS) +1;
        bit11 = 1;
        bit21 = (e2[block]&mask) ? 1 : 0;
        mask >>= 1;
        if (!mask) {
                block --;
                if (!block) {
                        zcopy(tab[1][bit21],r);
			FREE2SPACE(e1,e2);
			for (i=0; i<8;i++)
				for (j=0; j<8; j++)
					FREESPACE(tab[i][j]);
                        return;
                }
                mask = ( 1L<<(NBITS-1) );
        }
        bit12 = (e1[block]&mask) ? 1 : 0;
        bit22 = (e2[block]&mask) ? 1 : 0;
	mask >>= 1;
	if (!mask) {
		block --;
		if (!block) {
			if (bit12 || bit22) {
				zcopy(tab[2+bit12][(bit21<<1)+bit22],r);
			} else {
                		zcopy(tab[bit11][bit21],r);
                		zsqmod(*r,n,r);
			}
			FREE2SPACE(e1,e2);
			for (i=0; i<8;i++)
				for (j=0; j<8; j++)
					FREESPACE(tab[i][j]);
			return;
		}
		mask = ( 1L<<(NBITS-1) );
	}
	bit13 = (e1[block]&mask) ? 1 : 0;
        bit23 = (e2[block]&mask) ? 1 : 0;
	if (bit13 || bit23) {
		zcopy(tab[4+(bit12<<1)+bit13][(bit21<<2)+(bit22<<1)+bit23],r);
	} else {
		if (bit12 || bit22) {
			zcopy(tab[2+bit12][(bit21<<1)+bit22],r);
		} else {
			zcopy(tab[1][bit21],r);
			zsqmod(*r,n,r);
		}
		zsqmod(*r,n,r);
	}

#define MOD_THIRD \
	mask >>= 1; \
	if (!mask) { \
		block --; \
                if (!block) { \
			zsqmod(*r,n,r); \
			if (bit11 || bit21) { \
				if (bit12 || bit22) { \
				  zsqmod(*r,n,r); \
				  zmulmod(tab[(bit11<<1)+bit12][(bit21<<1)+bit22],*r,n,r); \
				} else { \
				  zmulmod(tab[bit11][bit21],*r,n,r); \
				  zsqmod(*r,n,r); \
				} \
			} else { \
				zsqmod(*r,n,r); \
				if (bit12 || bit22) { \
					zmulmod(tab[bit12][bit22],*r,n,r); \
				} \
			} \
			FREE2SPACE(e1,e2); \
			for (i=0; i<8;i++) \
				for (j=0; j<8; j++) \
					FREESPACE(tab[i][j]); \
			return; \
		} \
		mask = ( 1L<<(NBITS-1) ); \
	} \
	bit13 = (e1[block]&mask) ? 1 : 0; \
        bit23 = (e2[block]&mask) ? 1 : 0;

	MOD_FIRST
	MOD_SECOND
	MOD_THIRD

	for (;;) {
		zsqmod(*r,n,r);
		if (bit11 || bit21) {
			if (bit13 || bit23) {
				zsqmod(*r,n,r);
				zsqmod(*r,n,r);
				zmulmod(tab[(bit11<<2)+(bit12<<1)+bit13]
					    [(bit21<<2)+(bit22<<1)+bit23],*r,n,r);
			} else {
				if (bit12 || bit22) {
					zsqmod(*r,n,r);
					zmulmod(tab[(bit11<<1)+bit12]
						    [(bit21<<1)+bit22],*r,n,r);
				} else {
					zmulmod(tab[bit11][bit21],*r,n,r);
					zsqmod(*r,n,r);
				}
				zsqmod(*r,n,r);
			}
			MOD_FIRST
			MOD_SECOND
		} else {
			bit11 = bit12;
			bit21 = bit22;
			bit12 = bit13;
			bit22 = bit23;
		}
		MOD_THIRD
	}
}

void
zexpmod_doub(
	verylong xx1,
	verylong ee1,
	verylong xx2,
	verylong ee2,
	verylong n,
	verylong *r
	)
{
        if (ALLOCATE && !n)
        {
                zhalt("modulus zero in zexpmod_doub");
                return;
        }
        if ((zscompare(ee1,0)<0) || (zscompare(ee2,0)<0)) {
                zhalt("negative exponent in zexpmod_doub");
                zintoz(0,r);
                return;
        }
	if (zscompare(ee1,1L<<NBITS)<0 && zscompare(ee2,1L<<NBITS)<0)
		zexpmod_doub1(xx1,ee1,xx2,ee2,n,r);
	else if (ee1[0]<10 && ee2[0]<10)
		zexpmod_doub2(xx1,ee1,xx2,ee2,n,r);
	else
		zexpmod_doub3(xx1,ee1,xx2,ee2,n,r);
}

void
zmontexp_doub1(
	verylong xx1,
	verylong ee1,
	verylong xx2,
	verylong ee2,
	verylong *r
	)
{
        register long bit11, bit21;
        register long i,j,mask,block;
        STATIC verylong tab[2][2];
        STATIC long non_ini = 1;
        STATIC verylong e1 = 0;
        STATIC verylong e2 = 0;
        if (!zn)
        {
                zhalt("undefined Montgomery modulus in zmontexp_doub1");
                return;
        }
        if ((zscompare(ee1,0)<0) || (zscompare(ee2,0)<0)) {
                zhalt("negative exponent in zmontexp_doub1");
                zintoz(0,r);
                return;
        }
	if (non_ini) {
		for (i=0;i<2;i++) for (j=0;j<2;j++)
			tab[i][j] = 0;
		non_ini = 0;
	}
#define MONT_INITPAR \
        if (zcompare(ee1,ee2)>=0) { \
		zcopy(xx1,&(tab[1][0])); \
		zcopy(xx2,&(tab[0][1])); \
                zcopy(ee1,&e1); \
                zcopy(ee1,&e2); /* create space for e2 */ \
                zcopy(ee2,&e2); \
        } else { \
		zcopy(xx2,&(tab[1][0])); \
		zcopy(xx1,&(tab[0][1])); \
                zcopy(ee2,&e2); /* create space for e2 */ \
                zcopy(ee1,&e2); \
                zcopy(ee2,&e1); \
        } \
        if (!(zscompare(e1,0))) { \
		zcopy(zr,r); \
		FREE2SPACE(e1,e2); FREE2SPACE(tab[1][0],tab[0][1]); \
                return; \
        } \
        for (i=e2[0]+1;i<=e1[0];i++) e2[i] = 0;

	MONT_INITPAR

	zmontmul(tab[1][0],tab[0][1],&(tab[1][1]));

	block = z2log(e1)-1;
        mask = ( 1L<<(block % NBITS) );
        block = (block / NBITS) +1;
        bit11 = 1;
        bit21 = (e2[block]&mask) ? 1 : 0;
	zcopy(tab[1][bit21],r);

#define MONT_FIRST \
	mask >>= 1; \
	if (!mask) { \
		block --; \
		if (!block) { \
			FREE2SPACE(e1,e2); \
			FREE2SPACE(tab[1][0],tab[0][1]); \
			FREESPACE(tab[1][1]); \
			return; \
		} \
		mask  = ( 1L <<(NBITS-1) ); \
	} \
	bit11 = (e1[block]&mask) ? 1 : 0; \
	bit21 = (e2[block]&mask) ? 1 : 0; \

	MONT_FIRST
	for (;;) {
		zmontsq(*r,r);
		if (bit11 || bit21) 
			zmontmul(tab[bit11][bit21],*r,r);
		MONT_FIRST
	}
	FREE2SPACE(e1,e2);
	FREE2SPACE(tab[1][0],tab[0][1]);
	FREESPACE(tab[1][1]);
}

void
zmontexp_doub2(
	verylong xx1,
	verylong ee1,
	verylong xx2,
	verylong ee2,
	verylong *r
	)
{
	register long i,j,mask,block;
	register long bit11, bit12, bit21, bit22;
	STATIC verylong tab[4][4];
	STATIC long non_ini = 1;
	STATIC verylong e1 = 0;
	STATIC verylong e2 = 0;
        if (!zn)
        {
                zhalt("undefined Montgomery modulus in zmontexp_doub2");
                return;
        }
	if ((zscompare(ee1,0)<0) || (zscompare(ee2,0)<0)) {
		zhalt("negative exponent in zmontexp_doub2");
		zintoz(0,r);
		return;
	}
	if (non_ini) {
		for (i=0;i<4;i++) for (j=0;j<4;j++)
			tab[i][j] = 0;
		non_ini = 0;
	}

	MONT_INITPAR

	zmontsq(tab[1][0],&(tab[2][0]));
	zmontsq(tab[0][1],&(tab[0][2]));
	zmontmul(tab[2][0],tab[1][0],&(tab[3][0]));
	zmontmul(tab[0][2],tab[0][1],&(tab[0][3]));
	for (j=1;j<4;j++) {
		for (i=1;i<4;i++) if ((i&1) | (j&1))
			zmontmul(tab[j][0],tab[0][i],&(tab[j][i]));
	}

	block = z2log(e1)-1;
	mask = ( 1L<<(block % NBITS) );
	block = (block / NBITS) +1;
	bit11 = 1;
	bit21 = (e2[block]&mask) ? 1 : 0;
	mask >>= 1;
	if (!mask) {
		block --;
		if (!block) {
			zcopy(tab[1][bit21],r);
			FREE2SPACE(e1,e2);
                        for (i=0; i<4;i++)
                                for (j=0; j<4; j++)
                                        FREESPACE(tab[i][j]);
			return;
		}
		mask = ( 1L<<(NBITS-1) );
	}
	bit12 = (e1[block]&mask) ? 1 : 0;
	bit22 = (e2[block]&mask) ? 1 : 0;
	if (bit12 || bit22) {
		zcopy(tab[2+bit12][(bit21<<1)+bit22],r);
	} else {
		zcopy(tab[bit11][bit21],r);
		zmontsq(*r,r);
	}

#define MONT_SECOND \
	mask >>= 1; \
	if (!mask) { \
		block--; \
		if (!block) { \
			zmontsq(*r,r); \
			if (bit11||bit21) \
				zmontmul(tab[bit11][bit21],*r,r); \
                        FREE2SPACE(e1,e2); \
                        for (i=0; i<4;i++) \
                                for (j=0; j<4; j++) \
                                        FREESPACE(tab[i][j]); \
			return; \
		} \
		mask = ( 1L<<(NBITS-1) ); \
	} \
        bit12 = (e1[block]&mask) ? 1 : 0; \
        bit22 = (e2[block]&mask) ? 1 : 0;

	MONT_FIRST
	MONT_SECOND
	for (;;) {
		zmontsq(*r,r);
		if (bit11 || bit21) {
			if (bit12 || bit22) {
			  zmontsq(*r,r);
			  zmontmul(tab[(bit11<<1)+bit12][(bit21<<1)+bit22],*r,r);
			} else {
			  zmontmul(tab[bit11][bit21],*r,r);
			  zmontsq(*r,r);
			}
			MONT_FIRST
		} else {
			bit11 = bit12;
			bit21 = bit22;
		}
		MONT_SECOND
	}
}

void
zmontexp_doub3(
	verylong xx1,
	verylong ee1,
	verylong xx2,
	verylong ee2,
	verylong *r
	)
{
	register long i,j, mask, block;
	register long bit11, bit12, bit13, bit21, bit22, bit23;
	STATIC verylong tab[8][8];
	STATIC long non_ini = 1;
	STATIC verylong e1 = 0;
	STATIC verylong e2 = 0;
        if (!zn)
        {
                zhalt("undefined Montgomery modulus in zmontexp_doub3");
                return;
        }
        if ((zscompare(ee1,0)<0) || (zscompare(ee2,0)<0)) {
                zhalt("negative exponent in zmontexp_doub3");
                zintoz(0,r);
                return;
        }
	if (non_ini) {
		for (i=0;i<8;i++) for (j=0;j<8;j++)
			tab[i][j] = 0;
		non_ini = 0;
	}

	MONT_INITPAR

	zmontsq(tab[1][0],&(tab[2][0]));
	zmontsq(tab[0][1],&(tab[0][2]));
	for (i=3;i<8;i+=2) zmontmul(tab[2][0],tab[i-2][0],&(tab[i][0]));
	for (i=3;i<8;i+=2) zmontmul(tab[0][2],tab[0][i-2],&(tab[0][i]));
	for (i=1;i<8;i++) zmontmul(tab[0][1],tab[1][i-1],&(tab[1][i]));
        for (i=1;i<8;i+=2) zmontmul(tab[0][i],tab[2][0],&(tab[2][i]));
        for (i=1;i<8;i++) zmontmul(tab[0][1],tab[3][i-1],&(tab[3][i]));
	for (j=4;j<8;j++) {
		zmontmul(tab[1][0],tab[j-1][1],&(tab[j][1]));
		for (i=3;i<8;i+=2) zmontmul(tab[0][2],tab[j][i-2],&(tab[j][i]));
		j++;
		for (i=1;i<8;i++) zmontmul(tab[0][1],tab[j][i-1],&(tab[j][i]));
	}
        block = z2log(e1)-1;
        mask = ( 1L<<(block % NBITS) );
        block = (block / NBITS) +1;
        bit11 = 1;
        bit21 = (e2[block]&mask) ? 1 : 0;
        mask >>= 1;
        if (!mask) {
                block --;
                if (!block) {
                        zcopy(tab[1][bit21],r);
			FREE2SPACE(e1,e2);
                        for (i=0; i<8;i++)
                                for (j=0; j<8; j++)
                                        FREESPACE(tab[i][j]);
                        return;
                }
                mask = ( 1L<<(NBITS-1) );
        }
        bit12 = (e1[block]&mask) ? 1 : 0;
        bit22 = (e2[block]&mask) ? 1 : 0;
	mask >>= 1;
	if (!mask) {
		block --;
		if (!block) {
			if (bit12 || bit22) {
				zcopy(tab[2+bit12][(bit21<<1)+bit22],r);
			} else {
                		zcopy(tab[bit11][bit21],r);
                		zmontsq(*r,r);
			}
			FREE2SPACE(e1,e2);
                        for (i=0; i<8;i++)
                                for (j=0; j<8; j++)
                                        FREESPACE(tab[i][j]);
			return;
		}
		mask = ( 1L<<(NBITS-1) );
	}
	bit13 = (e1[block]&mask) ? 1 : 0;
        bit23 = (e2[block]&mask) ? 1 : 0;
	if (bit13 || bit23) {
		zcopy(tab[4+(bit12<<1)+bit13][(bit21<<2)+(bit22<<1)+bit23],r);
	} else {
		if (bit12 || bit22) {
			zcopy(tab[2+bit12][(bit21<<1)+bit22],r);
		} else {
			zcopy(tab[1][bit21],r);
			zmontsq(*r,r);
		}
		zmontsq(*r,r);
	}

#define MONT_THIRD \
	mask >>= 1; \
	if (!mask) { \
		block --; \
                if (!block) { \
			zmontsq(*r,r); \
			if (bit11 || bit21) { \
				if (bit12 || bit22) { \
				  zmontsq(*r,r); \
				  zmontmul(tab[(bit11<<1)+bit12][(bit21<<1)+bit22],*r,r); \
				} else { \
				  zmontmul(tab[bit11][bit21],*r,r); \
				  zmontsq(*r,r); \
				} \
			} else { \
				zmontsq(*r,r); \
				if (bit12 || bit22) { \
					zmontmul(tab[bit12][bit22],*r,r); \
				} \
			} \
			FREE2SPACE(e1,e2); \
                        for (i=0; i<8;i++) \
                                for (j=0; j<8; j++) \
                                        FREESPACE(tab[i][j]); \
			return; \
		} \
		mask = ( 1L<<(NBITS-1) ); \
	} \
	bit13 = (e1[block]&mask) ? 1 : 0; \
        bit23 = (e2[block]&mask) ? 1 : 0;

	MONT_FIRST
	MONT_SECOND
	MONT_THIRD

	for (;;) {
		zmontsq(*r,r);
		if (bit11 || bit21) {
			if (bit13 || bit23) {
				zmontsq(*r,r);
				zmontsq(*r,r);
				zmontmul(tab[(bit11<<2)+(bit12<<1)+bit13]
					    [(bit21<<2)+(bit22<<1)+bit23],*r,r);
			} else {
				if (bit12 || bit22) {
					zmontsq(*r,r);
					zmontmul(tab[(bit11<<1)+bit12]
						    [(bit21<<1)+bit22],*r,r);
				} else {
					zmontmul(tab[bit11][bit21],*r,r);
					zmontsq(*r,r);
				}
				zmontsq(*r,r);
			}
			MONT_FIRST
			MONT_SECOND
		} else {
			bit11 = bit12;
			bit21 = bit22;
			bit12 = bit13;
			bit22 = bit23;
		}
		MONT_THIRD
	}
}


void
zmontexp_doub(
	verylong xx1,
	verylong ee1,
	verylong xx2,
	verylong ee2,
	verylong *r
	)
{
        if (!zn)
        {
                zhalt("undefined Montgomery modulus in zmontexp_doub");
                return;
        }
        if ((zscompare(ee1,0)<0) || (zscompare(ee2,0)<0)) {
                zhalt("negative exponent in zmontexp_doub");
                zintoz(0,r);
                return;
        }
	if (zscompare(ee1,1L<<NBITS)<0 && zscompare(ee2,1L<<NBITS)<0)
		zmontexp_doub1(xx1,ee1,xx2,ee2,r);
	else if (ee1[0]<10 && ee2[0]<10)
		zmontexp_doub2(xx1,ee1,xx2,ee2,r);
	else
		zmontexp_doub3(xx1,ee1,xx2,ee2,r);
}

void
z2mul(
	verylong n,
	verylong *rres
	)
{
	register long sn;
	register long i;
	register long carry = 0;
	verylong res = *rres;

	if (ALLOCATE && !n)
	{
		zzero(rres);
		return;
	}
	if ((!n[1]) && (n[0] == 1))
	{
		zzero(rres);
		return;
	}
	if ((sn = n[0]) < 0)
		sn = -sn;
	zsetlength(&res, sn + 1, "in z2mul, second argument");
	if (n == *rres) n = res;
	*rres = res;
	for (i = 1; i <= sn; i++)
	{
		if ((res[i] = (n[i] << 1) + carry) >= RADIX)
		{
			res[i] -= RADIX;
			carry = 1;
		}
		else
			carry = 0;
	}
	if (carry)
		res[++sn] = 1;
	if (n[0] < 0)
		res[0] = -sn;
	else
		res[0] = sn;
}

long 
z2div(
	verylong n,
	verylong *rres
	)
{
	register long sn;
	register long i;
	register long result;
	verylong res = *rres;

	if ((!n) || ((!n[1]) && (n[0] == 1)))
	{
		zzero(rres);
		return (0);
	}
	if ((sn = n[0]) < 0)
		sn = -sn;
	zsetlength(&res, sn, "in z2div, second argument");
	if (n == *rres) n = res;
	*rres = res;
	result = n[1] & 1;
	for (i = 1; i < sn; i++)
	{
		res[i] = (n[i] >> 1);
		if (n[i + 1] & 1)
			res[i] += (RADIX >> 1);
	}
	if ((res[sn] = (n[sn] >> 1)))
		res[0] = n[0];
	else if (sn == 1)
	{
		res[0] = 1;
	}
	else if (n[0] < 0)
		res[0] = -sn + 1;
	else
		res[0] = sn - 1;
	return (result);
}

long 
z2mod(
	verylong n
	)
{
	if (!n)
		return (0);
	return (n[1] & 1);
}

void
zlshift(
	verylong n,
	long k,
	verylong *rres
	)
{
	register long big;
	register long smallval;
	register long sn;
	register long i;
	register long cosmall;
	verylong res = *rres;

	if (ALLOCATE && !n)
	{
		zzero(rres);
		return;
	}
	if ((!n[1]) && (n[0] == 1))
	{
		zzero(rres);
		return;
	}
	if (!k)
	{
		if (n != *rres)
			zcopy(n, rres);
		return;
	}
	if (k < 0)
	{
		zrshift(n, -k, rres);
		return;
	}
	if (k == 1)
	{
		z2mul(n, rres);
		return;
	}
	if ((sn = n[0]) < 0)
		sn = -sn;
	i = sn + (big = k / NBITS);
	if ((smallval = k - big * NBITS))
	{
		zsetlength(&res, i + 1, "in zlshift, third argument");
		if (n == *rres) n = res;
		*rres = res;
		res[i + 1] = n[sn] >> (cosmall = NBITS - smallval);
		for (i = sn; i > 1; i--)
			res[i + big] = ((n[i] << smallval) & RADIXM) + (n[i - 1] >> cosmall);
		res[big + 1] = (n[1] << smallval) & RADIXM;
		for (i = big; i; i--)
			res[i] = 0;
		if (res[sn + big + 1])
			big++;
	}
	else
	{
		zsetlength(&res, i, "in zlshift, third argument");
		if (n == *rres) n = res;
		*rres = res;
		for (i = sn; i; i--)
			res[i + big] = n[i];
		for (i = big; i; i--)
			res[i] = 0;
	}
	if (n[0] > 0)
		res[0] = n[0] + big;
	else
		res[0] = n[0] - big;
}

void
zrshift(
	verylong n,
	long k,
	verylong *rres
	)
{
	register long big;
	register long smallval;
	register long sn;
	register long i;
	register long cosmall;
	verylong res = *rres;

	if (ALLOCATE && !n)
	{
		zzero(rres);
		return;
	}
	if ((!n[1]) && (n[0] == 1))
	{
		zzero(rres);
		return;
	}
	if (!k)
	{
		if (n != *rres)
			zcopy(n, rres);
		return;
	}
	if (k < 0)
	{
		zlshift(n, -k, rres);
		return;
	}
	if (k == 1)
	{
		z2div(n, rres);
		return;
	}
	big = k / NBITS;
	smallval = k - big * NBITS;
	if ((sn = n[0]) < 0)
		sn = -sn;
	if ((big >= sn) || ((big == sn - 1) && (!(n[sn] >> smallval))))
	{
		zzero(rres);
		return;
	}
	sn -= big;
	zsetlength(&res, sn, "in zrshift, third argument");
	if (n == *rres) n = res;
	*rres = res;
	if (smallval)
	{
		cosmall = NBITS - smallval;
		for (i = 1; i < sn; i++)
			res[i] = (n[i + big] >> smallval) +
				((n[i + big + 1] << cosmall) & RADIXM);
		if (!(res[sn] = (n[sn + big] >> smallval)))
			sn--;
	}
	else
		for (i = 1; i <= sn; i++)
			res[i] = n[i + big];
	if (n[0] > 0)
		res[0] = sn;
	else
		res[0] = -sn;
}

long 
zmakeodd(
	verylong *nn
	)
{
	verylong n = *nn;
	register long i;
	long shift = 1;

	if (!n || (!n[1] && (n[0] == 1)))
		return (-1);
	while (!(n[shift]))
		shift++;
	i = n[shift];
	shift = NBITS * (shift - 1);
	while (!(i & 1))
	{
		shift++;
		i >>= 1;
	}
	zrshift(n, shift, &n);
	return (shift);
}

long 
zcomposite(
	verylong *mm,
	long t,
	long firstbase
	)
{
	register long sm = 0;
	register long i;
	register long j;
	register long s;
	register long loc_rand;
	verylong m = *mm;
	STATIC verylong u = 0;
	STATIC verylong m1 = 0;
	STATIC verylong a = 0;

	if (!m || m[0] < 0)
		return (1);
	if ((m[0] == 1) && (m[1] <= RADIXROOT))
	{
		sm = m[1];
		if (sm < 2)
			return (1);
		if (sm == 2)
			return (0);
		if (!(sm & 1))
			return (1);
		i = 3;
		while (i * i <= sm)
		{
			if (!(sm % i))
				return (1);
			i++;
			i++;
		}
		return (0);
	}
	if (!(m[1] & 1))
		return (1);
	zsetlength(&u, (i = m[0]), "in zcomposite, locals\n");
	zsetlength(&m1, i, "");
	zsetlength(&a, i, "");
	zsubpos(m, one, &m1);
	zcopy(m1, &u);
	s = zmakeodd(&u);
	if (firstbase < 0)
		firstbase = -firstbase;
	if (!firstbase && (sm = (m[1]-3)) <= (RADIXROOT-3))
		sm = RADIXM - 3;
	if (t < 0)
		i = -t;
	else
		i = t;
	for (; i > 0; i--)
	{
		if (sm)
		{
			zintoz((loc_rand = (3 + zrandom(sm)) & RADIXM), &a);
			zexpmod(a, u, m, &a);
		}
		else
		{
			if (firstbase == 2)
				z2expmod(u, m, &a);
			else
			{
				zintoz(firstbase, &a);
				zexpmod(a, u, m, &a);
			}
			loc_rand=firstbase;
			if ((sm = (m[1]-3)) <= (RADIXROOT-3))
				sm = RADIXM - 3;
		}
		if ((a[0] != 1 || a[1] != 1) && zcompare(m1, a))
		{
			for (j = s; j; j--)
			{
				zsqmod(a, m, &a);
				if (!zcompare(a, m1))
					goto nexti;
				if (!zscompare(a,1L)) {
					long k;

					if (t > 0) {
						FREE2SPACE(u,m1);
						FREESPACE(a);
						return 1;
					}
					zintoz(loc_rand,&a);
					zexpmod(a, u, m, &a);
					for (k=s; k>j; k--)
						zsqmod(a, m, &a);
					zsadd(a,-1, &a);
					zgcd(a,m,&u);
					if (!zscompare(u,1L) || !zcompare(u,m)) {
						zhalt("wrong factor in zcomposite   BUG");
						goto nexti;
					}

				zcopy(u,&m);
				FREE2SPACE(u,m1);
				FREESPACE(a);
				return -1;
				}
			}
			if (t > 0) {	
				FREE2SPACE(u,m1);
				FREESPACE(a);
				return (1);
			}
			zsadd(a,-1,&a);
			zintoz(loc_rand,&u);
			zmulmod(a,u,m,&a);
			zgcd(a, m, &u);
			if (u[1] == 1 && u[0] == 1) {
				FREE2SPACE(u,m1);
				FREESPACE(a);
				return (1);
			}
			zcopy(u, &m);
			FREE2SPACE(u,m1);
			FREESPACE(a);
			return (-1);
		}
nexti:		;
	}
	FREE2SPACE(u,m1);
	FREESPACE(a);
	return (0);
}

long 
zprime(
	verylong n,
	long t,
	long firstbase
	)
{
	if (t < 0)
		t = -t;
	return (1 - zcomposite(&n, t, firstbase));
}

static void
zsmexp(
	long a,
	verylong e,
	verylong *bb
	)
{
/* mixed arithmetic exponentiation, only for in zmcomposite */
 /* with mixed montgomery and ordinary arithmetic */
	register long i;
	register long j;
	register long k = 0;
	verylong b = *bb;

	if (ALLOCATE && !zn)
	{
		zhalt("wrong call to zsmexp, you are using an incorrect version of LIP");
		return;
	}
	if (ALLOCATE && !e)
	{
		zcopy(zr, bb);
		return;
	}
	if (ALLOCATE && !a)
	{
		zzero(bb);
		return;
	}
	if ((i = e[0]) < 0)
		i = (-i);	/* negative exponent made positive ... */
	if ((i == 1) && (!(e[1])))
		zcopy(zr, &b);
	else
	{
		zintoz(a, &b);
		ztom(b, &b);
		for (; i; i--)
		{
			j = e[i];
			if (!k)
			{
				k = 1;
				while ((k << 1) <= j)
					k <<= 1;
			}
			while (k >>= 1)
			{
				zmontsq(b, &b);
				if (j & k)
				{
					zsmul(b, a, &b);
					zmod(b, zn, &b);
				}
			}
			k = RADIX;
		}
	}
	*bb = b;
}

long 
zmcomposite(
	verylong m,
	long t
	)
{
	register long sm = 0;
	register long i;
	register long j;
	register long s;
	STATIC verylong u = 0;
	STATIC verylong m1 = 0;
	STATIC verylong a = 0;

	if (!m || m[0] < 0)
		return (1);
	if ((m[0] == 1) && (m[1] <= RADIXROOT))
	{
		sm = m[1];
		if (sm < 2)
			return (1);
		if (sm == 2)
			return (0);
		if (!(sm & 1))
			return (1);
		i = 3;
		while (i * i <= sm)
		{
			if (!(sm % i))
				return (1);
			i++;
			i++;
		}
		return (0);
	}
	if (!(m[1] & 1))
		return (1);
	zsetlength(&u, (i = m[0]), "in zmcomposite, locals\n");
	zsetlength(&m1, i, "");
	zsetlength(&a, i, "");
	zmkeep(m);
	zsubpos(m, zr, &m1);	/* zr is montgomery-one, m1 is
				 * montgomery-(n-1) */
	zsubpos(m, one, &u);
	s = zmakeodd(&u) - 1;
	if ((sm = (m[1]-3)) <= (RADIXROOT-3))
		sm = RADIXM - 3;
	if (t < 0)
		t = -t;
	for (i = t; i > 0; i--)
	{
		zsmexp((3 + zrandom(sm)) & RADIXM, u, &a);
		if (zcompare(zr, a) && zcompare(m1, a))
		{
			for (j = s; j; j--)
			{
				zmontsq(a, &a);
				if (!zcompare(a, m1))
					goto nexti;
			}
			zmback();
			FREESPACE(u);
			FREE2SPACE(m1,a);
			return (1);
		}
nexti:		;
	}
	zmback();
	FREESPACE(u);
	FREE2SPACE(m1,a);
	return (0);
}

long 
zsqrts(
	long n
	)
{
	register long a;
	register long ndiva;
	register long newa;
	STATIC verylong ln=0;
	STATIC verylong rr=0;
	STATIC verylong dif=0;

	if (n <= 0)
		return (0);
	if (n <= 3)
		return (1);
	if (n <= 8)
		return (2);
	if (n >= RADIX)
	{
		long ret;

		zintoz(n,&ln);
		zsqrt(ln,&rr,&dif);
		ret = ztoint(rr);
		FREE2SPACE(rr,dif); FREESPACE(ln);
		return ret;
	}
	FREE2SPACE(rr,dif); FREESPACE(ln);
	newa = 3L << (2 * (NBITSH - 1));
	a = 1L << NBITSH;
	while (!(n & newa))
	{
		newa >>= 2;
		a >>= 1;
	}
	while (1)
	{
		newa = ((ndiva = n / a) + a) / 2;
		if (newa - ndiva <= 1)
		{
			if (newa * newa <= n)
				return (newa);
			else
				return (ndiva);
		}
		a = newa;
	}
}

long 
zsqrt(
	verylong n_in,
	verylong *rr,
	verylong *ddif
	)
{
	STATIC verylong n = 0;
	STATIC verylong a = 0;
	STATIC verylong ndiva = 0;
	STATIC verylong diff = 0;
	register long i;
	verylong dif = *ddif;
	verylong r = *rr;

	if (ALLOCATE && !n_in)
	{
		zzero(rr);
		zzero(ddif);
		return (1);
	}
	if ((i = n_in[0]) < 0)
	{
		zhalt("negative argument in zsqrt");
		return (0);
	}
	zcopy(n_in,&n);
	zsetlength(&a, i, "in zsqrt, locals\n");
	zsetlength(&ndiva, i, "");
	zsetlength(&diff, i, "");
	if (i == 1)
	{
		zintoz(i = zsqrts(n[1]), &r);
		zintoz(i * i, &diff);
		goto done;
	}
	a[(a[0] = (i + 1) / 2)] = zsqrts(n[i]) + 1;
	if (!(i & 1))
		a[a[0]] <<= NBITSH;
	if (a[a[0]] & RADIX)
	{
		a[a[0]] = 0;
		a[0]++;
		a[a[0]] = 1;
	}
	for (i = a[0] - 1; i; i--)
		a[i] = 0;
	while (1)
	{
		zdiv(n, a, &ndiva, &r);
		zadd(a, ndiva, &r);
		zrshift(r, (long) 1, &r);
		if (zcompare(r, ndiva) <= 0)
		{
			zsq(r, &diff);
			goto done;
		}
		zsubpos(r, ndiva, &diff);
		if ((diff[0] == 1) && (diff[1] <= 1))
		{
			zsq(r, &diff);
			if (zcompare(diff, n) > 0)
			{
				zcopy(ndiva, &r);
				zsq(r, &diff);
			}
			goto done;
		}
		zcopy(r, &a);
	}
done:
	*rr = r;
	zsubpos(n, diff, &dif);
	*ddif = dif;
	FREESPACE(n); FREESPACE(a); FREESPACE(ndiva); FREESPACE(diff);
	if (!(dif[1]) && (dif[0] == 1))
		return (1);
	return (0);
}

void
zpstart()
{
	register long i;
	register long j;
	register long jstep;
	register long jstart;
	register long ibnd;
	register short int *p;

	if (!lowsieve)
	{
		lowsieve = (short int *)calloc((size_t)PRIM_BND, sizeof(short int));
		p = &lowsieve[0];
		for (i = PRIM_BND; i; i--)
			*p++ = 1;
		jstep = 1;
		jstart = -1;
		ibnd = (zsqrts((long) (2 * PRIM_BND + 1)) - 3) / 2;
		for (i = 0; i <= ibnd; i++)
		{
			jstart += 2 * ((jstep += 2) - 1);
			if (lowsieve[i])
				for (j = jstart; j < PRIM_BND; j += jstep)
					lowsieve[j] = 0;
		}
	}
	lastp = 0;
	pshift = 0;
	pindex = -2;
}

void
zpstart2()
{
	lastp = 0;
	pshift = -1;
}

static void
zpshift()
{
/* auxiliary routine for prime generator */
	register long i;
	register long j;
	register long jstep;
	register long jstart;
	register long ibound;
	register short *p;
	register short *pi;

	if (!movesieve)
		movesieve = (short int *)calloc((size_t)PRIM_BND, sizeof(short int));
	pi = &movesieve[0];
	if (!pshift)
	{
		p = &lowsieve[0];
		for (i = PRIM_BND; i; i--)
			*pi++ = *p++;
	}
	else
	{
		for (i = PRIM_BND; i; i--)
			*pi++ = 1;
		jstep = 3;
		ibound = pshift + 2 * PRIM_BND + 1;
		for (i = 0; jstep * jstep <= ibound; i++)
		{
			if (lowsieve[i])
			{
				if (!((jstart = (pshift + 2) / jstep + 1) & 1))
					jstart++;
				if (jstart <= jstep)
					jstart = jstep;
				jstart = (jstart * jstep - pshift - 3) / 2;
				for (j = jstart; j < PRIM_BND; j += jstep)
					movesieve[j] = 0;
			}
			jstep += 2;
		}
	}
}

long 
zpnext()
{
	if (pshift < 0)
	{
		zpstart();
		return (lastp = 2);
	}
	if (pindex == -2)
	{
		pindex = 0;
		zpshift();
		return (lastp = 3);
	}
	for (;;)
	{
		while ((++pindex) < PRIM_BND)
		{
			if (movesieve[pindex])
				return (lastp = pshift + 2 * pindex + 3);
		}
		if ((pshift += 2 * PRIM_BND) > 2 * PRIM_BND * (2 * PRIM_BND + 1))
		{
			zpstart();
			return (lastp = 2);
		}
		zpshift();
		pindex = -1;
	}
}

long 
zp()
{
	return (lastp);
}

long
zpnextb(
	long b
	)
{
	if (b >= PRIM_UP)
		return(0);
	zpstart();
	if (b < (2*PRIM_BND))
	{
		zpstart2();
		while (zpnext() < b);
		return(lastp);
	}
	pshift = ((b-2) / (2*PRIM_BND)) * (2*PRIM_BND);
	pindex = -1;
	zpshift();
	while (zpnext() < b);
	return(lastp);
}


void
zgcd(
	verylong mm1,
	verylong mm2,
	verylong *rres
	)
{
	register long agrb;
	register long shibl;
	STATIC verylong aa = 0;
	STATIC verylong bb = 0;
	STATIC verylong cc = 0;
	verylong a;
	verylong b;
	verylong c;
	verylong d;
	long m1negative;
	long m2negative;

	if (!mm1)
	{
		if (mm2 != *rres)
			zcopy(mm2,rres);
		zabs(rres);
		return;
	}
	if (!mm2)
	{
		if (mm1 != *rres)
			zcopy(mm1,rres);
		zabs(rres);
		return;
	}
	if (mm1 == mm2)
	{
		if (mm1 != *rres)
			zcopy(mm1, rres);
		zabs(rres);
		return;
	}
	if ((m1negative = (mm1[0] < 0)))
		mm1[0] = -mm1[0];
	if ((m2negative = (mm2[0] < 0)))
		mm2[0] = -mm2[0];
	if (!(mm1[1]) && (mm1[0] == 1))
	{
		zcopy(mm2,rres);
		goto done;
	}
	if (!(mm2[1]) && (mm2[0] == 1))
	{
		zcopy(mm1,rres);
		goto done;
	}
	if ((agrb = mm1[0]) < mm2[0])
		agrb = mm2[0];
	zsetlength(&aa, agrb, "in zgcd, locals\n");
	zsetlength(&bb, agrb, "");
	zsetlength(&cc, agrb, "");
	if (mm1[0] != mm2[0])
	{
		if (mm1[0] > mm2[0])
		{
			zcopy(mm2, &aa);
			zmod(mm1, aa, &bb);
		}
		else
		{
			zcopy(mm1, &aa);
			zmod(mm2, aa, &bb);
		}
		if (!(bb[1]) && (bb[0] == 1))
		{
			zcopy(aa,rres);
			goto done;
		}
	}
	else
	{
		zcopy(mm1, &aa);
		zcopy(mm2, &bb);
	}
	if ((agrb = zmakeodd(&aa)) < (shibl = zmakeodd(&bb)))
		shibl = agrb;
	if (!(agrb = zcompare(aa, bb)))
	{
		a = aa;
		goto endshift;
	}
	else if (agrb < 0)
	{
		a = bb;
		b = aa;
	}
	else
	{
		a = aa;
		b = bb;
	}
	c = cc;
	zsubpos(a, b, &c);
	do
	{
		zmakeodd(&c);
		if (!(agrb = zcompare(b, c)))
		{
			a = b;
			goto endshift;
		}
		else if (agrb > 0)
		{
			a = b;
			b = c;
			c = a;
		}
		else
		{
			d = a;
			a = c;
			c = d;
		}
		zsubpos(a, b, &c);
	} while (c[1] || c[0] != 1);
endshift:
	zlshift(a, shibl, rres);
done:
	if (m1negative)
		mm1[0] = -mm1[0];
	if (m2negative)
		mm2[0] = -mm2[0];
	FREESPACE(aa); FREE2SPACE(bb,cc);
}

void
zgcdeucl(
	verylong mm1,
	verylong mm2,
	verylong *rres
	)
{
	register long agrb;
	STATIC verylong aa = 0;
	STATIC verylong bb = 0;
	verylong a;
	long m1negative;
	long m2negative;

	if (!mm1)
	{
		if (mm2 != *rres)
			zcopy(mm2, rres);
		zabs(rres);
		return;
	}
	if (!mm2)
	{
		if (mm1 != *rres)
			zcopy(mm1, rres);
		zabs(rres);
		return;
	}
	if (mm1 == mm2)
	{
		if (mm1 != *rres)
			zcopy(mm1, rres);
		zabs(rres);
		return;
	}
	if ((m1negative = (mm1[0] < 0)))
		mm1[0] = -mm1[0];
	if ((m2negative = (mm2[0] < 0)))
		mm2[0] = -mm2[0];
	if (!(mm1[1]) && (mm1[0] == 1))
	{
		a = mm2;
		goto done;
	}
	if (!(mm2[1]) && (mm2[0] == 1))
	{
		a = mm1;
		goto done;
	}
	if ((agrb = mm1[0]) < mm2[0])
		agrb = mm2[0];
	zsetlength(&aa, agrb, "in zgcdeucl, locals\n");
	zsetlength(&bb, agrb, "");
	zcopy(mm1, &aa);
	zcopy(mm2, &bb);
	while (bb[1] || (bb[0] != 1))
	{
		zmod(aa, bb, &aa);
		if (!(aa[1]) && (aa[0] == 1))
		{
			a = bb;
			goto done;
		}
		zmod(bb, aa, &bb);
	}
	a = aa;
done:
	if (m1negative)
		mm1[0] = -mm1[0];
	if (m2negative)
		mm2[0] = -mm2[0];
	zcopy(a, rres);
	FREE2SPACE(aa,bb);
}


long 
zpollardrho(
	verylong n,
	verylong *rres,
	verylong *ccof,
	long t
	)
{
	register long i;
	register long j = 1;
	register long k;
	STATIC verylong xi = 0;
	STATIC verylong x2i = 0;
	STATIC verylong dif = 0;
	verylong res = *rres;
	verylong cof = *ccof;
	verylong adder = &glosho[1];
	static long start=0;

	start++;
	if (ALLOCATE && !n)
	{
		zzero(rres);
		zzero(ccof);
		return (0);
	}
	if (!n[1] && (n[0] == 1))
	{
		zzero(rres);
		zzero(ccof);
		return (0);
	}
	if (n[0] < 0)
	{
		zintoz((long) (-1), rres);
		zcopy(n, ccof);
		return (1);
	}
	if (!(n[1] & 1))
	{
		zintoz((long) 2, rres);
		zrshift(n, (long) 1, ccof);
		if (((*ccof)[1] != 1) || ((*ccof)[0] != 1))
			return (1);
		return (0);
	}
	zsetlength(&xi, (i = n[0] + 1), "in zpollardrho, locals\n");
	zsetlength(&x2i, i, "");
	zsetlength(&dif, i, "");
	zmkeep(n);
	zcopy(one, &dif);
	dif[1] = start & RADIX;
	ztom(dif, &xi);
	zcopy(xi, &x2i);
	dif[1] = 1;
	while(1)
	{
		for (i=0; i < j ; )
		{
			zmontsq(x2i, &x2i);
			zadd(x2i, adder, &x2i);
			zsub(xi, x2i, &res);
			if (res[0] < 0)
				res[0] = -res[0];
			if ((res[1]) || (res[0] != 1))
				zmontmul(dif, res, &dif);
			else
			{
				zcopy(one, &dif);
				ztom(dif, &xi);
				zcopy(xi, &x2i);
				adder[1] += 2;
				i=0;
				continue;
			}
			if ( !((++i)&7L) )
			{
				if (dif[1]==0 && (dif[0] == 1))
					zcopy(one, &dif);
				zgcd(n, dif, &res);
				if (((res[1] != 1) || (res[0] != 1)) && zcompare(res, n))
				{
					zdiv(n, res, &cof, &xi);
					if (xi[1] || xi[0] != 1)
					{
						*rres = res;
						*ccof = cof;
						zhalt("wrong factor in zpollardrho   BUG");
						zmback();
						return (0);
					}
					if (zcompare(res, cof) > 0)
					{
						*rres = cof;
						*ccof = res;
					}
					else
					{
						*rres = res;
						*ccof = cof;
					}
					zmback();
					FREE3SPACE(xi,x2i,dif);
					return i+j;
				}
			}
		}
		if (i+j <t || !t) {
			zcopy(x2i,&xi);
			j = i << 1;
			for (k=0; k<j; k++) {
				zmontsq(x2i, &x2i);
				zadd(x2i, adder, &x2i);
			}
		}
		else {
			zmback();
			FREE3SPACE(xi,x2i,dif);
			return (0);
		}
	}
}



long
ztridiv(
        verylong n,
        verylong *cof,
        long b1,
        long b2
        )
{
        long last = zp();
        long trydiv;

	if ((b1 < 0) || (b1 > b2) || (b2 >= RADIX))
		return(0);

	trydiv = zpnextb(b1);

        while (trydiv <= b2)
        {
                if (!zsdiv(n, trydiv, cof))
                        goto done;
                trydiv = zpnext();
        }
        done:
        if (last)
                zpnextb(last);
        else
                zpstart2();
        return (trydiv);
}

long 
zfread(
	FILE *f,
	verylong *aa
	)
{
 /* return 1 if success, 0 if not */
	static char *inmem = 0;
	char *in;
	register long d = 0;
	register long anegative = 0;
	register long return_value = 1;
	verylong a = *aa;
	verylong digit = &glosho[1];
	extern double log();

	if (log10rad < 0)
		log10rad = log((double)RADIX) / log((double)10);
	if (!inmem)
		inmem = (char *)calloc((size_t)IN_LINE, sizeof(char));
	if (fscanf(f, "%s", inmem) == EOF)
		return (0);
	if ((inmem[0] == '-') || (inmem[0] == '_'))
	{
		anegative = 1;
		in = (inmem + 1);
	}
	else
		in = inmem;
	zsetlength(&a, (long) (strlen(in) / log10rad), "in zfread, second argument");
	a[0] = 1;
	a[1] = 0;
	while (in[d] != '\0')
	{
		if (in[d] == IN_LINE_BREAK)
		{
			if (fscanf(f, "%s", in) == EOF)
				return (0);
			d = 0;
			if (strlen(in) > log10rad * (a[-1] - a[0]))
			{
			/* to avoid multiple reallocations */
				zsetlength(&a, (long) (a[0] + 3 + strlen(in) / log10rad), 
				           "in zfread, second argument");
			}
		}
		else
		{
			zsmul(a, (long) 10, &a);
			digit[1] = (long) (in[d++] - '0');
                        if (digit[1] < 0) {
                                digit[1] = 0;
                                return_value = 0;
                        } else if (digit[1] > 9) {
                                return_value = 0;
                        }
			zadd(a, digit, &a);
		}
	}
	if (anegative)
		znegate(&a);
	*aa = a;
	return (return_value);
}

long 
zread(
	verylong *a
	)
{
	return (zfread(stdin, a));
}

long 
zfwrite_c(
	FILE *f,
	verylong a,
	long linelen,
	char *str1,
	char *str2
	)
{
	STATIC verylong out = 0;
	STATIC verylong ca = 0;
	static long outsize = 0;
	static long div = 0;
	static long ldiv;
	register long i;
	long sa;
	long result;
	long zeros;
	long strlen1 = strlen(str1);
	long strlen2 = strlen(str2);
	extern double log();

	if (log10rad < 0)
		log10rad = log((double)RADIX) / log((double)10);

	if (ALLOCATE && !a)
	{
		fprintf(f, "0");
		return (1);
	}
	if (!div)
	{
		div = 10;
		ldiv = 1;
		while (div * 10 < RADIXROOT)
		{
			div *= 10;
			ldiv++;
		}
	}

	if (linelen && linelen < ldiv)
		linelen = OUT_LINE;
	if (!out) {
		out = (verylong)calloc((size_t)(outsize = SIZE << 1), (size_t)SIZEOFLONG);
	}
	if ((sa = a[0]) < 0)
	{
		sa = -sa;
		fprintf(f, "%s-", str1);
	}
	else
		fprintf(f, "%s", str1);

	if (ldiv * outsize < log10rad * sa)
	{
		free((void*)out);
		if (!(out = (verylong)calloc((size_t)(outsize = (log10rad * sa) / ldiv + 2), (size_t)SIZEOFLONG))) {
			zhalt("allocation failure in zfwrite_c");
			return (0);
		}
	}

	zsetlength(&ca, sa, "in zfwrite_c, local");
	for (i = (ca[0] = sa); i; i--)
		ca[i] = a[i];
	i = -1;
	do
	{
		if ((++i) >= outsize)
		{
			if (!(out = (verylong)realloc((void*)out, (size_t)((outsize += SIZE) * SIZEOFLONG))))
			{
				zhalt("reallocation failure in zfwrite_c");
				return (0);
			}
		}
		out[i] = zsdiv(ca, div, &ca);
	} while (ca[1] || ca[0] != 1);

	sa = 0;
	result = out[i];
	do
	{
		sa++;
	} while (result /= 10);

	result = sa + i * ldiv;;
	fprintf(f, "%ld", out[i--]);
	for (; i >= 0; i--)
	{
		if ((zeros = 10 * out[i]))
		{
			while (zeros < div)
			{
				fprintf(f, "0");
				zeros *= 10;
			}
		}
		else
		{
			for (zeros = ldiv - 1; zeros; zeros--)
				fprintf(f, "0");
		}
		fprintf(f, "%ld", out[i]);
		sa += ldiv;
		if (linelen && (sa > linelen - strlen1) && i)
		{
			fprintf(f, "%c\n%s", OUT_LINE_BREAK, str2);
			sa = 0;
			strlen1 = strlen2;
		}
	}
/* need to free space differently for out */
#ifdef FREE
	free((void*)out);
	FREESPACE(ca);
#endif
	return (result);
}

long
zfwrite(
        FILE *f,
        verylong a
        )
{
	return (zfwrite_c(f, a, (long)OUT_LINE, "", ""));
}

long
zfwriteln(
        FILE *f,
        verylong a
        )
{
	long ret = zfwrite_c(f, a, (long)OUT_LINE, "", "");

        fprintf(f,"\n");
        return (ret);
}

long 
zwrite(
	verylong a
	)
{
	return (zfwrite_c(stdout, a, (long)OUT_LINE, "", ""));
}

long 
zwriteln(
	verylong a
	)
{
	long ret = zfwrite_c(stdout, a, (long)OUT_LINE, "", "");

	printf("\n");
	return (ret);
}

char 
eulav(
	long i
	)
{
	switch (i)
	{
		case 0:
			return ('0');
		case 1:
			return ('1');
		case 2:
			return ('2');
		case 3:
			return ('3');
		case 4:
			return ('4');
		case 5:
			return ('5');
		case 6:
			return ('6');
		case 7:
			return ('7');
		case 8:
			return ('8');
		case 9:
			return ('9');
#ifdef HEX_LOWER_CASE
		case 10:
			return ('a');
		case 11:
			return ('b');
		case 12:
			return ('c');
		case 13:
			return ('d');
		case 14:
			return ('e');
		case 15:
			return ('f');
#else
		case 10:
			return ('A');
		case 11:
			return ('B');
		case 12:
			return ('C');
		case 13:
			return ('D');
		case 14:
			return ('E');
		case 15:
			return ('F');
#endif
		default:
			return ('?');
	}
}

void
zhfwrite(
	FILE *fn,
	verylong a
	)
{
	static char *b = 0;
	static bl = 0;
	STATIC verylong aa = 0;
	register long i;
	register long cnt = 0;
	register long mb;
	register long lab = 0;

	if (!a)
		fprintf(fn, "0");
	zcopy(a, &aa);
	if (aa[0] < 0)
	{
		aa[0] = -aa[0];
		fprintf(fn, "-");
	}
	if (!b)
		b = (char *)malloc((size_t)(bl = (aa[0] << 3)));
	else if (bl < (aa[0] << 3))
		b = (char *)realloc((void*)b, (size_t)((bl = (aa[0] << 3)) * sizeof(char)));
	do
	{
		b[cnt] = eulav(aa[1] & 15);
		cnt++;
		zrshift(aa, (long) 4, &aa);
	} while ((aa[1] != 0) || (aa[0] != 1));
	if (!HEX_BLOCK) mb = 0;
	else mb = HEX_BLOCK - (cnt%HEX_BLOCK);
	if (mb == HEX_BLOCK) mb = 0;
	for (i = cnt; i--;)
	{
		fprintf(fn, "%c", b[i]);
		mb++;
		if (mb == HEX_BLOCK)
		{
			lab++;
			if (lab == HEX_BLOCKS_PER_LINE)
			{
				if (i)
					fprintf(fn, "%c\n", OUT_LINE_BREAK);
				lab = 0;
				mb = 0;
			}
			else
			{
				mb = 0;
				fprintf(fn, "%c", HEX_SEP_CHAR);
			}
		}
	}
	FREESPACE(aa);

}

long
zhswrite(
		 char *str,
		 verylong a
		 )
{
	static char *b = 0;
	static bl = 0;
	STATIC verylong aa = 0;
	register long i;
	register long cnt = 0;
	register long mb;
	register long lab = 0;
	register long cur = 0;
	
	str[0] = '\x0';

	if (!a)
		str[cur++] = '0';

	zcopy(a, &aa);
	if (aa[0] < 0)
	{
		aa[0] = -aa[0];
		str[cur++] = '-';
	}
	if (!b)
	{
		b = (char *)malloc((size_t)(bl = (aa[0] << 3)));
		if (b)
			memset(b, 0, bl);
	}
	else if (bl < (aa[0] << 3))
	{
		b = (char *)realloc((void*)b, (size_t)((bl = (aa[0] << 3)) * sizeof(char)));
		if (b)
			memset(b, 0, bl);
	}
	do
	{
		b[cnt] = eulav(aa[1] & 15);
		cnt++;
		zrshift(aa, (long) 4, &aa);
	} while ((aa[1] != 0) || (aa[0] != 1));
	if (!HEX_BLOCK) mb = 0;
	else mb = HEX_BLOCK - (cnt%HEX_BLOCK);
	if (mb == HEX_BLOCK) mb = 0;
	for (i = cnt; i--;)
	{
		str[cur++] = b[i];
	}
	FREESPACE(aa);

	str[cur] = '\x0';

	return cur;
}


void
zhfwriteln(
        FILE *f,
        verylong a
        )
{
        zhfwrite(f, a);
        fprintf(f, "\n");
}

void
zhwrite(
	verylong a
	)
{
	zhfwrite(stdout, a);
}

void
zhwriteln(
	verylong a
	)
{
	zhfwrite(stdout, a);
	printf("\n");
}

long 
value(
	char c
	)
{
	switch (c)
	{
		case '0':
			return (0);
		case '1':
			return (1);
		case '2':
			return (2);
		case '3':
			return (3);
		case '4':
			return (4);
		case '5':
			return (5);
		case '6':
			return (6);
		case '7':
			return (7);
		case '8':
			return (8);
		case '9':
			return (9);
		case 'A':
		case 'a':
			return (10);
		case 'B':
		case 'b':
			return (11);
		case 'C':
		case 'c':
			return (12);
		case 'D':
		case 'd':
			return (13);
		case 'E':
		case 'e':
			return (14);
		case 'F':
		case 'f':
			return (15);
		default:
			return(0);
	}
}

long
zstrtozbas(
        char *s,
        long base,
        verylong *n
        )
{
        register long v;
        register long cnt = 0;
        register long negative = 0;
        register char c;

        if (base < 0) base = -base;
        if ((!base) || (base > 16))
                return(0);

        if ((s[0] == '-') || (s[0] == '_'))
        {
                negative = 1;
                cnt ++;
        }
        zintoz(0,n);

        c = s[cnt++];
        while (((v = value(c)) < base) && ((v) || (c == '0')) )
        {
                zsmul(*n,base,n);
                zsadd(*n,v,n);
                c = s[cnt++];
        }

        cnt --;
        if (negative)
        {
                znegate(n);
                cnt --;
        }
        return(cnt);
}

long
zstrtoz(
        char *s,
        verylong *n
        )
{
        return(zstrtozbas(s,10,n));
}

void
zhfread(
	FILE *fn,
	verylong *a
	)
{
	char prev;
	char c;
	long anegative = 0;

	zintoz((long) 0, a);
	do
	{
		prev = ' ';
		do
		{
			fscanf(fn, "%c", &c);
		} while (c == '\n');
		if (((c == '-') || (c =='_')) && (!anegative))
		{
			anegative = 1;
			fscanf(fn, "%c", &c);
		}
		while (c != '\n')
		{
			prev = c;
			fscanf(fn, "%c", &c);
			if ((prev != IN_LINE_BREAK) && (prev != ' '))
			{
				zlshift(*a, (long) 4, a);
				zsadd(*a, value(prev), a);
			}
		}
		if (!anegative)
			anegative = -1;
	} while (prev == IN_LINE_BREAK);
	if (anegative > 0)
		if (((*a)[1]) || ((*a)[0] != 1))
			(*a)[0] = -((*a)[0]);
}

void
zhread(
	verylong *a
	)
{
	zhfread(stdin, a);
}

void 
zhsread(
	char *str,
	verylong *aa
	)
{
	static char *inmem = 0;
	char *in;
	register long d = 0;
	register long anegative = 0;
	verylong a = *aa;
	extern double log();

	if (log16rad < 0)
		log16rad = log((double)RADIX) / log((double)16);

	if (!inmem)
		inmem = (char *)calloc((size_t)IN_LINE, sizeof(char));
	if (sscanf(str, "%s", inmem) == EOF)
		return;
	if ((inmem[0] == '-') || (inmem[0] == '_'))
	{
		anegative = 1;
		in = (inmem + 1);
	}
	else
		in = inmem;
	zsetlength(&a, (long) (strlen(in) / log16rad), "in zhsread, second argument");
	a[0] = 1;
	a[1] = 0;
	while (in[d] != '\0')
	{
		if (in[d] == IN_LINE_BREAK)
		{
			if (sscanf(str, "%s", in) == EOF)
				return;
			d = 0;
			if (strlen(in) > log16rad * (a[-1] - a[0]))
			{
				zsetlength(&a, (long) (a[0] + 3 + strlen(in) / log16rad), 
				           "in zhsread, second argument");
			}
		}
		else
		{
			zlshift(a, (long) 4, &a);
			zsadd(a,value(in[d++]),&a);
		}
	}
	if (anegative)
		znegate(&a);
	*aa = a;
}

long 
zsread(
	char *str,
	verylong *aa
	)
{
	static char *inmem = 0;
	char *in;
	register long d = 0;
	register long anegative = 0;
	register long return_value = 1;
	verylong a = *aa;
	verylong digit = &glosho[1];
	extern double log();

	if (log10rad < 0)
		log10rad = log((double)RADIX) / log((double)10);

	if (!inmem)
		inmem = (char *)calloc((size_t)IN_LINE, sizeof(char));
	if (sscanf(str, "%s", inmem) == EOF)
		return (0);
	if ((inmem[0] == '-') || (inmem[0] == '_'))
	{
		anegative = 1;
		in = (inmem + 1);
	}
	else
		in = inmem;
	zsetlength(&a, (long) (strlen(in) / log10rad), "in zsread, second argument");
	a[0] = 1;
	a[1] = 0;
	while (in[d] != '\0')
	{
		if (in[d] == IN_LINE_BREAK)
		{
			if (sscanf(str, "%s", in) == EOF)
				return (0);
			d = 0;
			if (strlen(in) > log10rad * (a[-1] - a[0]))
			{
				zsetlength(&a, (long) (a[0] + 3 + strlen(in) / log10rad), 
				           "in zsread, second argument");
			}
		}
		else
		{
			zsmul(a, (long) 10, &a);
			digit[1] = (long) (in[d++] - '0');
			if (digit[1] < 0) {
				digit[1] = 0;
				return_value = 0;
			} else if (digit[1] > 9) {
				return_value = 0;
			}
			zadd(a, digit, &a);
		};
	}
	if (anegative)
		znegate(&a);
	*aa = a;
	return (return_value);
}

long 
zswrite(
	char *str,
	verylong a
	)
{
	STATIC verylong out = 0;
	STATIC verylong ca = 0;
	static long outsize = 0;
	static long div = 0;
	static long ldiv;
	register long i;
	register long j;
	long sa;
	long result;
	long zeros;
	extern double log();

	if (log10rad < 0)
		log10rad = log((double)RADIX) / log((double)10);

	if (ALLOCATE && !a)
	{
		sprintf(str, "0");
		return (1);
	}
	if (!div)
	{
		div = 10;
		ldiv = 1;
		while (div * 10 < RADIXROOT)
		{
			div *= 10;
			ldiv++;
		}
	}
	if (!out)
		out = (verylong)calloc((size_t)(outsize = SIZE << 1), (size_t)SIZEOFLONG);
	j = 0;
	if ((sa = a[0]) < 0)
	{
		sa = -sa;
		sprintf(str, "-");
		str++;
	}
	if (ldiv * outsize < log10rad * sa)
	{
		free((void*)out);
		if (!(out = (verylong)calloc((size_t)(outsize = (log10rad * sa) / ldiv + 2), (size_t)SIZEOFLONG))) {
			zhalt("allocation failure in zswrite");
			return (0);
		}
	}
	zsetlength(&ca, sa, "in zswrite, local");
	for (i = (ca[0] = sa); i; i--)
		ca[i] = a[i];
	i = -1;
	do
	{
		if ((++i) >= outsize)
		{
			if (!(out = (verylong)realloc((void*)out, (size_t)((outsize += SIZE) * SIZEOFLONG))))
			{
				zhalt("reallocation failure in zswrite");
				return (0);
			}
		}
		out[i] = zsdiv(ca, div, &ca);
	} while (ca[1] || ca[0] != 1);
	sa = 0;
	result = out[i];
	do
	{
		sa++;
	} while (result /= 10);
	result = sa + i * ldiv;;
	sprintf(str + j, "%ld", out[i--]);
	j += sa;
	for (; i >= 0; i--)
	{
		if ((zeros = 10 * out[i]))
		{
			while (zeros < div)
			{
				sprintf(str + j++, "0");
				zeros *= 10;
			}
		}
		else
		{
			for (zeros = ldiv - 1; zeros; zeros--)
				sprintf(str + j++, "0");
		}
		sprintf(str + j, "%ld", out[i]);
		sa += ldiv;
		j = sa;
	}
#ifdef FREE
	free((void*)out);
	FREESPACE(ca);
#endif
	return (result);
}

long 
zbfwrite(
	FILE *fn,
	verylong a
	)
{
	register long i;
	register long j;
	STATIC verylong zero = 0;

	if (!zero)
		zintoz((long) 0, &zero);
	if (!a)
	{
		long zi;
		for (i=0;i<=zero[0];i++) {
			zi = htonl(zero[i]);
			if ((long)fwrite((void*)(&zi), sizeof(long), (size_t)1, fn)
				< (long)1) {
				FREESPACE(zero);
				return (0);
			}
		}
		FREESPACE(zero);
		return (1);
	}
	i = a[0];
	if (i < 0) i = -i;
	for (j=0;j<=i;j++) {
		long aj = htonl(a[j]);
		if ((long)fwrite((void*)(&aj), sizeof(long), (size_t)1, fn) 
			< (long)1) {
			FREESPACE(zero);
			return (0);
		}
	}
	FREESPACE(zero);
	return (1);
}

long 
zbfread(
	FILE *fn,
	verylong *a
	)
{
	register long negative = 0;
	register long j;
	long i;

	if (feof(fn) || ferror(fn))
		return (0);
	if ((long)fread((void*)&i, sizeof(long), 1, fn) < (long)1)
		return (0);
	i = ntohl(i);
	if (i < 0)
	{
		i = -i;
		negative = 1;
	}
	zsetlength(a, i, "in zbfread, second argument");
	if (negative)
		(*a)[0] = -i;
	else
		(*a)[0] = i;
	if (feof(fn) || ferror(fn))
		return (0);
	if ((long)fread((void*)&((*a)[1]), sizeof(long), (size_t)i, fn) < (long)i)
		return (0);
	for (j=1;j<=i;j++) (*a)[j] = ntohl((*a)[j]);
	return (1);
}

long
zfwrite_b(
	FILE *fff,
	verylong aa,
	verylong base,
	long sym_out
	)
{
	register long i;
	static long sl = 0;
	static long ll = 0;
	static long *s;
	STATIC verylong *l = 0;
	STATIC verylong a =0;
	STATIC verylong out_base = 0;
	long negative = 0;
	long need;
	zcopy(aa,&a);
	if (zscompare(a,0)<0)
	{
		negative = 1;
		znegate(&a);
	}
	zcopy(base,&out_base);
	zabs(&out_base);
	if (zscompare(out_base,1) <= 0)
		zhalt("output base < 2 in zfwrite_b");
	if (zscompare(out_base,RADIX) < 0)
	{
		long b;
		long cnt;
		long si;
		if (!zscompare(a,0))
		{
			fprintf(fff,"0");
			FREE2SPACE(out_base,a);
			return (0);
		}
		b = ztoint(out_base);
		need = 5 + (long)zslog(a,b);
		if (!sl)
		{
			if (!(s = (long*)calloc(need,sizeof(long))))
			{
				zhalt("allocation failure in zfwrite_b\n");
				return (0);
			}
		} else if (need > sl)
		{
			if (!(s = (long*)realloc(s,need*sizeof(long))))
			{
				zhalt("reallocation failure in zfwrite_b\n");
				return (0);
			}
		}
		sl = need;
		if (sym_out)
			i = zstosymbas(a,b,s,&need);
		else
			i = zstobas(a,b,s,&need);
		if (!i)
		{
			zhalt("zfwrite_b   BUG\n");
			return (0);
		}
		if (sym_out && negative)
			for (i=0;i<need;i++) s[i] = -s[i];
		if (sym_out || (b>16))
		{ /* in blocks */
			long blockl;
			if ((!sym_out) && negative)
				fprintf(fff,"-");
			zsadd(out_base,-1,&out_base);
			blockl = (long)(zslog(out_base,10))+1;
			zsadd(out_base,1,&out_base);
			cnt = 0;
			for (i=need-1;i>=0;i--)
			{
				si = s[i];
				if (sym_out && b != 2 )
				{
					if (si<0)
					{
						fprintf(fff,"-");
						si = -si;
					} else
						fprintf(fff," ");
					cnt ++;
				}
				switch(blockl)
				{
					case 1:
						fprintf(fff,"%01ld",si);
						break;
					case 2:
						fprintf(fff,"%02ld",si);
						break;
					case 3:
						fprintf(fff,"%03ld",si);
						break;
					case 4:
						fprintf(fff,"%04ld",si);
						break;
					case 5:
						fprintf(fff,"%05ld",si);
						break;
					case 6:
						fprintf(fff,"%06ld",si);
						break;
					case 7:
						fprintf(fff,"%07ld",si);
						break;
					case 8:
						fprintf(fff,"%08ld",si);
						break;
					case 9:
						fprintf(fff,"%09ld",si);
						break;
					case 10:
						fprintf(fff,"%010ld",si);
						break;
				}
				cnt += (blockl+1);
				if (cnt > OUT_LINE - blockl)
				{
					if (i)
					{
						fprintf(fff," %c\n",
							OUT_LINE_BREAK);
						cnt = 0;
					} else
						fprintf(fff,"\n");
				} else if (i)
					fprintf(fff," ");
			}

		} else 
		{ /* without spaces */
			if (negative)
			{
				fprintf(fff,"-");
				cnt = 1;
			} else 
				cnt = 0;
			for (i=need-1;i>=0;i--)
			{
				fprintf(fff,"%c",eulav((s[i])&15));
				cnt ++;
				if (cnt == OUT_LINE)
				{
					if (i)
					{
						fprintf(fff,"%c\n",
							OUT_LINE_BREAK);
						cnt = 0;
					} else
						fprintf(fff,"\n");
				} else if (b==16)
				{
					if (i)
					{
						if (cnt==56)
						{
							fprintf(fff,"%c\n",
							OUT_LINE_BREAK);
							cnt = 0;
						} else if ((cnt&7)==0)
							fprintf(fff," ");
					} else {	
						FREE3SPACE(*l,a,out_base);
						return (need);
					}
				}
			}
		}
		FREE3SPACE(*l,a,out_base);
		return (need);
	} else
	{
		long cnt;
		long blocks;
		if (!zscompare(a,0))
		{
			fprintf(fff,"0");
			FREE3SPACE(*l,a,out_base);
			return (0);
		}
		need = 5 + (long)zlog(a,out_base);
		if (!ll)
		{
			if (!(l = (verylong*)calloc(need,sizeof(verylong))))
			{
				zhalt("allocation failure in zfwrite_b\n");
				return (0);
			}
			for (i=0;i<need;i++)
				l[i] = 0;
		} else if (need > ll)
		{
			if (!(l = (verylong*)realloc(l,need*sizeof(verylong))))
			{
				zhalt("reallocation failure in zfwrite_b\n");
				return (0);
			}
			for (i=ll;i<need;i++)
				l[i] = 0;
		}
		ll = need;
		if (sym_out)
			i = ztosymbas(a,out_base,l,&need);
		else
			i = ztobas(a,out_base,l,&need);
		if (!i)
		{
			zhalt("zfwrite_b   BUG\n");
			return (0);
		}
		if (sym_out && negative)
			for (i=0;i<need;i++) znegate(&(l[i]));
		if ((!sym_out) && negative)
			fprintf(fff,"-");
		zsadd(out_base,-1,&out_base);
		blocks = OUT_LINE / ((long)(zslog(out_base,10))+1);
		zsadd(out_base,1,&out_base);
		if (blocks<=1)
		{
			for (i=need-1;i>=0;i--)
			{
				zfwrite_c(fff,l[i],OUT_LINE,"","");
				if (i)
					fprintf(fff," %c\n", OUT_LINE_BREAK);
			}
		} else
		{
			cnt = 0;
			for (i=need-1;i>=0;i--)
			{
				zfwrite_c(fff,l[i],OUT_LINE,"","");
				cnt ++;
				if (i)
				{
					fprintf(fff," ");
					if (cnt == blocks)
					{
						fprintf(fff," %c\n",
							OUT_LINE_BREAK);
						cnt = 0;
					}
				}
			}
		}
		FREE3SPACE(*l,a,out_base);
		return (need);
	}
}

long
zfwriteln_b(
        FILE *fff,
        verylong aa,
        verylong out_base,
        long sym_out
        )
{
	long result = zfwrite_b(fff,aa,out_base,sym_out);
	fprintf(fff,"\n");
	return(result);
}

long
zfread_b(
	FILE *fff,
	verylong *a,
	verylong base,
	long sym_in
	)
{
	STATIC verylong digit = 0;
	STATIC verylong in_base = 0;
	long negative = 0;
	long reach = 0;
	char y;
	zcopy(base,&in_base);
	zabs(&in_base);
	if (zscompare(in_base,1)<=0) {
		zhalt("input base < 2 in zfread_b");
		return(0);
	}
	if ((zscompare(in_base,16) > 0) || (sym_in))
	{
		reach = 1;
		zfread(fff,a);
		if ((zscompare(*a,0)<0) && (!sym_in))
		{
			znegate(a);
			negative = 1;
		}
		y=(char)fgetc(fff);
		while (y != '\n')
		{
			while (y == ' ')
				y=(char)fgetc(fff);
			if (y != IN_LINE_BREAK)
				ungetc((int)y,fff);
			zfread(fff,&digit);
			if (!sym_in) zabs(&digit);
			zmulin(in_base,a);
			zadd(*a,digit,a);
			y=(char)fgetc(fff);
		}
		if (sym_in) {
			FREE2SPACE(digit,in_base);
			return (1);
		}
	} else
	{
		register long short_in_base;
		register long non_hex;
		register long aftersl = 0;;
		short_in_base = ztoint(in_base);
		non_hex = (short_in_base != 16);
		while ((y=(char)fgetc(fff)) == '\n');
		if ((y == '-') || (y == '_'))
		{
			negative = 1;
			y=(char)fgetc(fff);
		}
		zintoz(0,a);
		while ((strrchr("00123456789AaBbCcDdEeFff ",y) != NULL) ||
			(y == IN_LINE_BREAK))
		{
			if (non_hex && (y == ' ') && (!aftersl))
				goto done;
			if ((y != ' ') && (y != IN_LINE_BREAK))
			{
				zsmul(*a,short_in_base,a);
				zsadd(*a,value(y),a);
				aftersl = 0;
			}
			if (y == IN_LINE_BREAK)
			{
				do
					y=(char)fgetc(fff);
				while (y != '\n');
				aftersl = 1;
			}
			y=(char)fgetc(fff);
		}
		if (y != '\n')
			ungetc((int)y,fff);
		else
			reach = 1;
	}
done:
	if (negative)
		znegate(a);
	FREE2SPACE(digit,in_base);
	return reach;
}

long 
zsign(
	verylong a
	)
{
	if (ALLOCATE && !a)
	{
		return (0);
	}
	if (a[0] < 0)
		return (-1);
	if (a[0] > 1)
		return (1);
	if (a[1])
		return (1);
	return (0);
}

void
zabs(
	verylong *pa
	)
{
	verylong a = *pa;

	if (!a)
		return;
	if (a[0] < 0)
		a[0] = (-a[0]);
}

long 
z2logs(
	long a
	)
{
	long i = 0;

	if (a < 0)
		a = -a;
	while (a>=256)
		i += 8, a >>= 8;
	if (a >=16)
		i += 4, a >>= 4;
	if (a >= 4)
		i += 2, a >>= 2;
	if (a >= 2)
		i += 2;
	else if (a >= 1)
		i++;
	return (i);
}

long 
z2log(
	verylong a
	)
{
	register long la;

	if (!a)
		return (0);
	la = (a[0] > 0 ? a[0] : -a[0]);
	return ( NBITS * (la - 1) + z2logs(a[la]) );
}

double 
zln(
	verylong a
	)
{
	extern double log();
	static double log_2 = -1.0;
	register long sa;

	if (!a)
	{
		zhalt("zero argument in zln");
		return (0.0);
	}
	sa = (a[0] > 0 ? a[0] : -a[0]);
	if (sa == 1) {
		if (!a[1]) {
			zhalt("zero argument in zln");
			return (0.0);
		}
		return (log((double) (a[1])));
	}
	if (log_2 < 0)
	{
		log_2 = log((double) 2);
	}
	return ((sa - 2) * NBITS * log_2 +
		log(((double)RADIX) * a[sa] + a[sa - 1]));
}

double 
zlog(
	verylong a,
	verylong b
	)
{
	return (zln(a) / zln(b));
}

double 
zslog(
	verylong a,
	long b
	)
{
	extern double log();

	return (zln(a) / log((double)b));
}

double 
zdlog(
	verylong a,
	double b
	)
{
	extern double log();

	return (zln(a) / log(b));
}

long
zscompare(
	verylong a,
	long b
	)
{
	STATIC verylong c = 0;
	long ret;

	zintoz(b, &c);
	ret= (zcompare(a, c));
	FREESPACE(c);
	return ret;
}

void
zswap(
	verylong *a,
	verylong *b
	)
{
	register verylong c;

	c = *a;
	*a = *b;
	*b = c;
}

long
ziszero(
	verylong a
	)
{
	if (!a) return (1);
	if (a[1]) return (0);
	if (a[0]==1) return (1);
	return (0);
}

long
zodd(
	verylong a
	)
{
	if (!a) return (0);
	return (a[1]&1);
}

long
zweights(
	long a
	)
{
	register long res = 0;
	if (a < 0) a = -a;
	while (a) {
		if (a & 1) res ++;
		a >>= 1;
	}
	return (res);
}

long
zweight(
        verylong a
        )
{
	register long i;
	register long res = 0;
	if (!a) return (0);
	i = a[0];
	if (i<0) i = -i;
	for (;i;i--)
		res += zweights(a[i]);
	return (res);
}

void
znot(
	verylong a,
	verylong *b
	)
{
	register long sa, i, m;
	if (ziszero(a)) {
		zintoz(1,b);
		return;
	}
	zcopy(a,b);
	sa = a[0];
        if (sa<0) sa = -sa;
        for (i=sa-1;i;i--)
		(*b)[i] = ((~((*b)[i])) & RADIXM);
	m = (RADIX >> 1);
	i = RADIXM;
	while (!(((*b)[sa])&m)) {
		m >>= 1;
		i >>= 1;
	}
	(*b)[sa] = ((~((*b)[sa])) & i);
	while ((sa>1) && (!((*b)[sa])))
		sa --;
	if (((*b)[0] < 0) && ((*b)[1])) (*b)[0] = -sa;
	else (*b)[0] = sa;
}

void
zand(
	verylong a,
	verylong b,
	verylong *cc
	)

{
	verylong c = *cc;
	register long sa;
	register long sb;
	register long sm;
	if (ziszero(a) || ziszero(b)) {
		zzero(cc);
		return;
	}
	sa = a[0];
	if (sa < 0) sa = -sa;
	sb = b[0];
	if (sb < 0) sb = -sb;
	sm = (sa > sb ? sb : sa );
	zsetlength(&c, sm, "in zand, third argument");
	if (a == *cc) a = c;
	if (b == *cc) b = c;
	*cc = c;
	for (sa = 1; sa <= sm; sa ++) 
		c[sa] = a[sa] & b[sa];
	while ((sm > 1) && (!(c[sm])))
		sm --;
	c[0] = sm;
}

void
zxor(
	verylong a,
	verylong b,
	verylong *cc
	)
{
        verylong c = *cc;
        register long sa;
        register long sb;
        register long sm;
        register long la;
        register long i;
	if (ziszero(a)) {
		zcopy(b,cc);
		zabs(cc);
		return;
	}
	if (ziszero(b)) {
		zcopy(a,cc);
		zabs(cc);
		return;
	}
        sa = a[0];
        if (sa < 0) sa = -sa;
        sb = b[0];
        if (sb < 0) sb = -sb;
	if (sa > sb) {
		la = sa;
		sm = sb;
	} else {
		la = sb;
		sm = sa;
	}
        zsetlength(&c, la, "in zxor, third argument");
        if (a == *cc) a = c;
        if (b == *cc) b = c;
        *cc = c;
        for (i = 1; i <= sm; i ++)
                c[i] = a[i] ^ b[i];
	if (sa > sb)
		for (;i <= la; i++) c[i] = a[i];
	else
		for (;i <= la; i++) c[i] = b[i];
        while ((la > 1) && (!(c[la])))
                la --;
        c[0] = la;
}

void
zor(
	verylong a,
	verylong b,
        verylong *cc
        )
{
        verylong c = *cc;
        register long sa;
        register long sb;
        register long sm;
        register long la;
        register long i;
	if (ziszero(a)) {
		zcopy(b,cc);
		zabs(cc);
		return;
	}
	if (ziszero(b)) {
		zcopy(a,cc);
		zabs(cc);
		return;
	}
        sa = a[0];
        if (sa < 0) sa = -sa;
        sb = b[0];
        if (sb < 0) sb = -sb;
        if (sa > sb) {
                la = sa;
                sm = sb;
        } else {
                la = sb;
                sm = sa;
        }
        zsetlength(&c, la, "in zor, third argument");
        if (a == *cc) a = c;
        if (b == *cc) b = c;
        *cc = c;
        for (i = 1; i <= sm; i ++)
                c[i] = a[i] | b[i];
        if (sa > sb)
                for (;i <= la; i++) c[i] = a[i];
        else
                for (;i <= la; i++) c[i] = b[i];
        c[0] = la;
}

long
zslowbits(
	verylong a,
	long b
	)
{
	if (ziszero(a) || (b<=0))
		return(0);
	if (b>NBITS)
		b = NBITS;
	return (a[1]&((1L<<b)-1));
}

void
zlowbits(
	verylong a,
	long b,
        verylong *cc
        )
{
        verylong c = *cc;
	register long bl;
	register long wh;
	register long sa;
	if (ziszero(a) || (b<=0)) {
		zintoz(0,cc);
		return;
	}
        bl = (b/NBITS);
        wh = b - NBITS*bl;
        bl ++;
	sa = a[0];
	if (sa < 0) sa = -sa;
	if (sa < bl) {
		zcopy(a,cc);
		zabs(cc);
		return;
	}
        zsetlength(&c, bl, "in zlowbits, third argument");
        if (a == *cc) a = c;
        *cc = c;
	for (sa=1;sa<bl;sa++)
		c[sa] = a[sa];
	c[bl] = a[bl]&((1L<<wh)-1);
	while ((bl>1) && (!c[bl]))
		bl --;
	c[0] = bl;
}

long
zshighbits(
	verylong a,
	long b
	)
{
	long l;
	STATIC verylong aa=0;
        if (ziszero(a) || (b<0))
                return(0);
        if (b>NBITS)
                b = NBITS;
	zcopy(a,&aa);
	zabs(&aa);
	l=z2log(aa);
	if (l > b)
		zrshift(aa,l-b,&aa);
	l= (ztoint(aa));
	FREESPACE(aa);
	return l;
}

void
zhighbits(
	verylong a,
	long b,
	verylong *c
	)
{
	long l;
        if (ziszero(a) || (b<0)) {
		zzero(c);
		return;
	}
	zcopy(a,c);
	zabs(c);
	l = z2log(*c);
	if (l > b)
		zrshift(*c,l-b,c);
}

void
zcat(
	verylong a,
	verylong b,
	verylong *c
	)
{
	register long signa = 0;
	register long signb = 0;
	STATIC verylong aux = 0; 
	if (ziszero(a)) {
		zcopy(b,c);
		return;
	}
	if (ziszero(b)) {
		zlshift(a,1,c);
		return;
	}
	if (a[0] < 0) {
		signa = 1;
		a[0] = -a[0];
	}
	if (b[0] < 0) {
		signb = 1;
		b[0] = -b[0];
	}
	zlshift(a,z2log(b),&aux);
	zadd(aux,b,c);
	if (signa && (a != *c)) a[0] = -a[0];
	if (signb && (b != *c)) b[0] = -b[0];
	FREESPACE(aux);
}

void
zgetbits(
	verylong a,
	long b,
	long p,
	verylong *c
	)
{
	register long bl;
	register long wh;
	if (b<=0) {
		zzero(c);
		return;
	}
	if (p<0) p = -p;
	zrshift(a,p,c);
	zabs(c);
	if (b>=z2log(*c)) return;
	bl = (b-1)/NBITS +1;
	for (wh=(*c)[0];wh>bl;wh--) (*c)[wh] = 0;
	(*c)[wh] &= ((1L<<(b%NBITS))-1);
	while ((wh>1) && (!((*c)[wh])))
		wh --;
	(*c)[0] = wh;
}

long
zbit(
	verylong a,
	long p
	)
{
        register long bl;
        register long wh;
        register long sa;
	if (ziszero(a)) return (0);
	if (p < 0) p = -p;
	bl = (p/NBITS);
        wh = 1L << (p - NBITS*bl);
	bl ++;
        sa = a[0];
        if (sa < 0) sa = -sa;
        if (sa < bl) return (0);
	if (a[bl] & wh) return (1);
	return (0);
}

long
zsetbit(
        verylong *a,
        long b
        )
{
	register long bl;
	register long wh;
	register long sa;
	if (b<0) b = -b;
	if (!(*a)) {
		zintoz(1,a);
		zlshift(*a,b,a);
		return (0);
	}
	bl = (b/NBITS);
	wh = 1L << (b - NBITS*bl);
	bl ++;
	sa = (*a)[0];
	if (sa<0) sa = -sa;
	if (sa >= bl) {
		sa = (*a)[bl] & wh;
		(*a)[bl] |= wh;
		if (sa) return (1);
		return (0);
	} else {
		zsetlength(a,bl,"in zsetbit, first argument");
		sa ++;
		for (;sa<=bl;sa++) (*a)[sa]=0;
		if ((*a)[0] < 0)
			(*a)[0] = -bl;
		else (*a)[0] = bl;
		(*a)[bl] |= wh;
		return (0);
	}
}

long
zswitchbit(
        verylong *a,
        long p
        )
{
        register long bl;
        register long wh;
        register long sa;
        if (p < 0) p = -p;
        if (ziszero(*a)) {
		zintoz(1,a);
		zlshift(*a,p,a);
		return (0);
	}
        bl = (p/NBITS);
        wh = 1L << (p - NBITS*bl);
        bl ++;
        sa = (*a)[0];
        if (sa < 0) sa = -sa;
        if ((sa < bl) || (!((*a)[bl] & wh))) {
		zsetbit(a,p);
		return (0);
	}
	(*a)[bl] ^= wh;
        while ((sa>1) && (!(*a)[sa]))
		sa --;
	if ((*a)[0] > 0) (*a)[0] = sa;
	else (*a)[0] = -sa;
        return (1);
}

long
zreverses(
	long aa
	)
{
	register long a = aa;
	register long result = 1;
	if (!a) return (0);
	if (a < 0) a = -a;
	while (!(a&1)) a >>= 1;
	a >>= 1;
	while (a) {
		result <<= 1;
		if (a & 1) result ++;
		a >>= 1;
	}
	return (result);
}

void
zreverse(
	verylong a,
	verylong *b
	)
{
	register long sa;
	register long i;
	register long aux;
	register long sh;
	register long ma;
	register long sr;
	STATIC verylong result = 0;
	if (ziszero(a)) {
		zzero(b);
		return;
	}
	sa = a[0];
	if (sa < 0) sa = -sa;
	zsetlength(&result,sa,"in zreverse, local");
	result[sa] = 0;
	if (a[sa]&(RADIX>>1)) sr = sa;
	else sr = sa-1;
	for (i=1;i<=sr;i++) {
		aux = a[sr+1-i];
		if (!aux) 
			result[i] = 0;
		else {
			ma = (RADIX>>1);
			sh = 0;
			while (!(aux&ma)) {
				ma >>= 1;
				sh ++;
			}
			result[i] = (zreverses(aux) << sh);
		}
	}
	while ((sr>1) && (!(result[sr])))
		sr --;
	result[0] = sr;
	if (!(a[sa]&(RADIX>>1))) {
		zlshift(result,z2logs(a[sa]),&result);
		zsadd(result,zreverses(a[sa]),&result);
	}
	zcopy(result,b);
	FREESPACE(result);
}

long 
zjacobi(
	verylong a,
	verylong n
	)
{
	long unit;
	long i;
	verylong temp = 0;

	if (!n || zsign(n) <= 0)
	{
		zhalt("non-positive second argument in zjacobi");
		return (0);
	}
	if (!a || !zsign(a))
	{
		return (0);
	}
	else if (a[0] == 1 && a[1] == 1)
	{
		return (1);
	}
	else if (n[0] == 1 && n[1] == 1)
	{
		return (1);
	}
	else if ((!(a[1] & 1)) && (!(n[1] & 1)))
	{
		return (0);
	}
	else if (!(n[1] & 1))
	{
		zcopy(n, &temp);
		i = zmakeodd(&temp);
		i = (i > 3 ? 8 : 1L << i);
		if (a[1] % i != 1)
			return (-1);
		i = zjacobi(a, temp);
		zfree(&temp);
		return(i);
	}
	else if (a[0] < 0)
	{
		zcopy(a, &temp);
		temp[0] = -temp[0];
	/* check (n-1)/2 parity */
		unit = (n[1] & 2 ? -1 : 1);
		i = (zjacobi(temp, n) * unit);
		zfree(&temp);
		return(i);
	}
	else if (!(a[1] & 1))
	{
	/* check (n*n-1)/8 parity */
		i = n[1] & 7;
		unit = (i == 3 || i == 5 ? -1 : 1);
		zcopy(a, &temp);
		zrshift(temp, (long) 1, &temp);
		i = (zjacobi(temp, n) * unit);
		zfree(&temp);
		return(i);
	}
	else
	{
	/* check ((a-1)*(n-1))/4 parity */
		unit = ((a[1] & 2) && (n[1] & 2) ? -1 : 1);
		zmod(n, a, &temp);	/* temp = n mod a */
		i = (zjacobi(temp, a) * unit);
		zfree(&temp);
		return(i);
	}
}

static long
zrootnewton(
	verylong a,
	verylong *b,
	long n
	)
{
	STATIC verylong c = 0;
	STATIC verylong d = 0;
	long log2a = z2log(a),k;

	zintoz(1L, b);
	zlshift(*b, (k=(log2a + n - 1) / n), b);

/* Newton can converge slowly in certain cases (large n).  If it looks like
Newton will converge slowly, then we do the safe bisection algorithm.
Maybe somebody can find a better way of determining if Newton is safe. */

	if ( n>= 16L && 2*z2logs(log2a) + z2logs(n) > k) {	/* bisection */
		verylong lowb=0, highb=0;

		zcopy(*b,&highb);
		zintoz(1L,&lowb);
		zlshift(lowb, (log2a - n + 1)/n,&lowb);

		while (zcompare(lowb,highb) < 0) {
			zadd(lowb,highb,&c);
			zrshift(c,1,&c);
			zsexp(c, n, &d);
			if ((k=zcompare(d,a)) <= 0)
				if (k==0) {
					zcopy(c,b);
					FREESPACE(c);
					FREESPACE(d);
					return 1;
				}
				else
					zsadd(c,1L,&lowb);
			else
				zcopy(c,&highb);
		}
		zsexp(c, n, &d);
		if ((k=zcompare(d,a)) == 0) {
			zcopy(c,b);
			FREESPACE(c);
			FREESPACE(d);
			return 1;
			}
		else if (k > 0)
			zsadd(c, -1, b);
			
		else
			zcopy(c,b);
		FREESPACE(c);
		FREESPACE(d);
		return 0;
	}
	else {						/* Newton */
		do
		{
			zsexp(*b, n - 1, &c);
			zdiv(a, c, &c, &d);
			zsub(*b, c, &c);
			zsdiv(c, n, &c);
			zsub(*b, c, b);
		} while (zsign(c) > 0);
		zsexp(*b, n, &c);
		while ((k=zcompare(c, a)) > 0) {
			zsadd(*b, (long) (-1), b);
			zsexp(*b, n, &c);
			}
		if (k==0) return 1;
		FREESPACE(c);
		FREESPACE(d);
		return 0;
	}

}

long
zroot(
	verylong a_in,
	long n,
	verylong *b
	)
{
	STATIC verylong a = 0;
	long signa = zsign(a_in),k=1;

	zcopy(a_in,&a);
	if ((signa < 0) && (n & 1))
		a[0] = -a[0];
	if ((signa > 0 && n > 1) || (signa < 0 && (n & 1)))
		k = zrootnewton(a, b, n);
	else if ((n == 1) || ((signa == 0) && (n > 0)))
		zcopy(a, b);
	else if (n == 0)
	{
		zhalt("dth root with d=0 in zroot");
		FREESPACE(a);
		return -1;
	}
	else if ((signa < 0) && (!(n & 1)))
	{
		zhalt("dth root with even d of negative number in zroot");
		FREESPACE(a);
		return -1;
	}
	else if (signa == 0 && n < 0)
	{
		zhalt("dth root with d<0 of zero in zroot");
		FREESPACE(a);
		return -1;
	}
	else
		zintoz( 0L, b);
	if (signa < 0)
	{
		if (((*b)[1]) || ((*b)[0] > 1))
			(*b)[0] = -(*b)[0];
	}
	FREESPACE(a);
	return k;
}


long 
zjacobis(
	long m,
	long n
	)
{
	register long m2;
	register long m1;
	register long jcbi = 1;

	if (n < 0)
		m2 = -n;
	else
		m2 = n;
	if ((m2 & 1) == 0)
	{
		zhalt("even second argument in zjacobis");
		return (0);
	}

	m1 = (m >= 0) ? (m % m2) : (m2 - ((-m) % m2));

	while (1)
	{
		switch (m1 & 15)
		{
			case 0:
				if (m1 == 0)
					return ((m2 == 1) ? jcbi : 0);

				m1 >>= 4;	/* Divide numerator by 16 */
				continue;	/* continue while loop */

			case 8:
				m1 >>= 2;	/* Divide numerator by 4 */
				continue;	/* continue while loop */

			case 4:
				m1 >>= 2;	/* Divide numerator by 4 */
				break;

			case 1:
			case 5:
			case 9:
			case 13:
				break;

			case 2:
			case 10:
				m1 >>= 1;
				if ((m2 & 7) == 3 || (m2 & 7) == 5)
					jcbi = -jcbi;
				break;

			case 12:
				m1 >>= 2;	/* Divide numerator by 4, */
			/* fall through to 3 mod 4 cases */
			case 3:
			case 7:
			case 11:
			case 15:
				if ((m2 & 2) != 0)	/* negate if  m1 == m2
							 * == 3 mod 4 */
					jcbi = -jcbi;
				break;

			case 6:
			case 14:
				m1 >>= 1;
				if ((m2 & 4) != 0)
					jcbi = -jcbi;
				break;
		}	/* end switch */

		{	/* m1 and m2 are odd; invert them */
		/* First divide m2 by 4, mod m1 */
			register long m3;
			m3 = ((((m2 + m1) & 3) == 0) ? m2 + m1 : m2 - m1) >> 2;
			if (m3 >= m1)
				m3 %= m1;
			m2 = m1;
			m1 = m3;
		}
	}	/* end while(1) */
}

long 
zprobprime(
	verylong a,
	long number_of_tests
	)
{
	long result;
	STATIC verylong cofactor_a = 0;

	if (!a || a[0] < 0)
		return (0);
	if ((a[0]==1) && (a[1]==2))
		return (1);
	if (!(a[1] & 1))
		return (0);
	if (a[0] == 1)
	{
		if ((a[1] == 3) || (a[1] == 5) || (a[1] == 7) || (a[1] == 11) )
			return (1);
		if (a[1] < 13) return (0);
		result = zsqrts(a[1]);
		if (ztridiv(a, &cofactor_a, (long) 3, result) <= result) {
			FREESPACE(cofactor_a);
			return (0);
		}
		FREESPACE(cofactor_a);
		return (1);
	}
	result = a[0] * 5 * NBITS;
	if (ztridiv(a, &cofactor_a, (long) 3, result) <= result) {
		FREESPACE(cofactor_a);
		return (0);
	}
	if (zcomposite(&a, (long) 1, (long) 2)) {
		FREESPACE(cofactor_a);
		return (0);
	}
	result = !zmcomposite(a, number_of_tests);
 /*
  * if (!result) { printf("composite, but pseudo prime to the base 2:\n");
  * zwriteln(a); }
  */
	FREESPACE(cofactor_a);
	return (result);
}

void
zrandomb(
        verylong bnd,
        verylong *a
        )
{
        register long i;
        STATIC verylong dummy = 0;

        if (zscompare(bnd, (long) 1) <= 0)
        {
                zintoz((long) 0, a);
                return;
        }
        zcopy(bnd,&dummy);
        zsetlength(a,i=dummy[0]+1,"");
        for (; i>0; i--)
                (*a)[i] = zrandom(RADIX);
        i = dummy[0]+1;
        while ((i>1) && (!(*a)[i]))
                i --;
        (*a)[0] = i;
        zdiv(*a,dummy,&dummy,a);
	FREESPACE(dummy);
}

void
zrandoml(
	long bitlength,
	verylong *a,
	void (*generator) (verylong, verylong*)
	)
{
	register long neg = 0;
	STATIC verylong b = 0;

	if (bitlength == 0)
	{
		zintoz((long) 0, a);
		return;
	}
	if (bitlength < 0)
	{
		bitlength = -bitlength;
		neg = 1;
	}
	if (bitlength == 1)
	{
		zintoz((long) 1, a);
		if (neg)
			(*a)[0] = -(*a)[0];
		return;
	}
	zintoz((long) 1, a);
	zlshift(*a, bitlength - 1, a);
	(*generator)(*a,&b);
	zadd(*a,b,a);
	if (neg)
		(*a)[0] = -(*a)[0];
	FREESPACE(b);
}

long 
zrandomprime(
	long bitlength,
	long nbtests,
	verylong *a,
	void (*generator) (verylong, verylong*)
	)
{
	STATIC verylong t = 0;
	STATIC verylong bnd = 0;
	register long adder = 2;
	long three_mod_four = 0;

	if (bitlength < 0)
	{
		bitlength = - bitlength;
		three_mod_four = 1;
		adder = 4;
	}
	if (bitlength < 2)
		return (0);
	if (bitlength == 2)
	{
		if (three_mod_four)
			zintoz(3,a);
		else
		{
			zintoz(2,a);
			(*generator)(*a,&t);
			zadd(*a,t,a);
		}
		FREESPACE(t);
		return (1);
	}
	zintoz((long) 1, &bnd);
	zlshift(bnd, bitlength, &bnd);
	while (1)
	{
		if (three_mod_four)
		{
			zrandoml(bitlength - 2, &t,generator);
			zlshift(t, (long) 2, a);
			zsadd(*a, (long) 3, a);
		}
		else
		{
			zrandoml(bitlength - 1, &t,generator);
			zlshift(t, (long) 1, a);
			zsadd(*a, (long) 1, a);
		}
		while (zcompare(*a, bnd) < 0)
		{
			if (zprobprime(*a, nbtests))
				return (1);
			zsadd(*a, adder, a);
		}
	}
	FREE2SPACE(t,bnd);
}

long
zrandomqprime(
	long lp,
	long lq,
	long nbtests,
	verylong *p,
	verylong *q,
	verylong *frac,
	void (*generator) (verylong, verylong*)
	)
{
	register long oddq = 1;
	register long cnt = 2 * lp;
	register long escape = 1;
	STATIC verylong tpsizem = 0;
	STATIC verylong tpsize = 0;
	STATIC verylong lown = 0;
	STATIC verylong upn = 0;
	STATIC verylong twoq = 0;

	if (lq < 0)
	{
		lq = z2log(*q);
		if ((!lq) || ((*q)[0] < 0))
		{
			zhalt("wrong q in zrandomqprime");
			return (0);
		}
		if (((*q)[0] == 1) && ((*q)[1] == 1))
		{
			if (!(zrandomprime(lp, nbtests, p, generator)))
				return (0);
			zcopy(*p, frac);
			return (1);
		}
		if (-lq >= lp)
			return (0);
	}
	else if (!(zrandomprime(lq, nbtests, q, generator)))
		return (0);

	zintoz((long) 1, &tpsizem);
	zlshift(tpsizem, lp - 1, &tpsizem);
	zsadd(tpsizem, (long) (-1), &tpsizem);
	zintoz((long) 1, &tpsize);
	zlshift(tpsize, lp, &tpsize);
	zsadd(tpsize, (long) (-1), &tpsize);

	if ((*q)[1] & 1)
		zlshift(*q, (long) 1, &twoq);
	else
	{
		oddq = 0;
		zcopy(*q, &twoq);
	}
	zdiv(tpsizem, twoq, &lown, frac);
	zdiv(tpsize, twoq, &upn, frac);
	zsub(upn, lown, &upn);
	do
	{
		cnt--;
		if (!cnt)
		{
			if (zscompare(upn, lp) <= 0) {
				FREE2SPACE(tpsizem,tpsize);
				FREE3SPACE(lown,upn,twoq);
				return (0);
			}
		}
		(*generator)(upn,p); 
		if (ziszero(*p))
			zsadd(*p,escape++,p);
		zadd(lown, *p, frac);
		zmul(twoq, *frac, p);
		zsadd(*p, (long) 1, p);
	} while (!zprobprime(*p, nbtests));
	if (oddq)
		zlshift(*frac, (long) 1, frac);
	FREE2SPACE(tpsizem,tpsize); FREE3SPACE(lown,upn,twoq);
	return (1);
}

long
zrandomfprime(
	long length,
	long nbtests,
	verylong frac,
	verylong *p,
	verylong *q,
	void (*generator) (verylong, verylong*)
	)
{
	register long pp;
	register long qmp;
	register long bound;
	register long small_frac = 0;
	register long last = zp();

	STATIC verylong aux = 0;

	if ((zscompare(frac,2)<0) || (frac[1]&1))
                return (0);

	if (length < (NBITS>>1))
	{
        	do
        	{
                	if (!zrandomprime(length,nbtests,q,generator))
                        	return (0);
                	zmul(*q,frac,p);
                	zsadd(*p,1,p);
        	} while (!zprobprime(*p,nbtests));
        	return (1);
	}

	if (zscompare(frac,RADIXROOT) <= 0)
		small_frac = ztoint(frac);
	if ((bound = (z2log(frac)+length)*5) > RADIXROOT)
		bound = RADIXROOT;
	for (;;)
	{
		zrandoml(length-1,q,generator);
		zlshift(*q,1,q);
		zsadd(*q,1,q);
		zpstart();
		while ((pp=zpnext()) <= bound) 
		{
			if (!(qmp = zsdiv(*q,pp,&aux)))
				goto next_try;
			if (small_frac)
				qmp *= small_frac;
			else
				qmp *= zsdiv(frac,pp,&aux);
			if (!((qmp+1)%pp))
				goto next_try;
		}
		if (zcomposite(q,1,2))
			goto next_try;
		zmul(*q,frac,p);
		zsadd(*p,1,p);
		if (zcomposite(p,1,2) || zmcomposite(*q,nbtests)
			|| zmcomposite(*p,nbtests))
			goto next_try;
		if (last)
			zpnextb(last);
		else
			zpstart2();
		FREESPACE(aux);
		return (1);
		next_try:;
	}
	FREESPACE(aux);
}

long
zrandomgprime(
	long lq,
	long nbtests,
	long smallval,
	verylong *p,
	verylong *q,
	verylong *g,
	void (*generator) (verylong, verylong*)
	)
{
	register long escape = 1;
	STATIC verylong montone = 0;
	STATIC verylong aux = 0;
	zintoz(2,&aux);
	if (!zrandomfprime(lq,nbtests,aux,p,q,generator))
		return 0;
	zmkeep(*p);
        zintoz((long)1, &montone);
        ztom(montone,&montone);
	if (smallval)
		zcopy(montone,g);
        for (;;)
	{
                if (smallval)
			zaddmod(*g,montone,*p,g);
		else
		{
			(*generator)(*p,g);
			if (ziszero(*g))
				zsadd(*g,escape++,g);
		}
                zmontsq(*g,&aux);
                if (zcompare(aux,montone))
		{
                        zmontexp_m_ary(*g,*q,&aux,0);
                        if (zcompare(aux,montone))
			{
				zmtoz(*g,g);
				zmback();
				FREE2SPACE(aux,montone);
				return 1;
			}
                }
        }
}

#ifndef NO_ECM

#define	ECM_MINBOUND	100
#define	ECM_MAXBOUND	5000000

#ifndef ECM_BATCH
#define	ECM_BATCH	1
#endif

#ifndef ECM_MULT
#define	ECM_MULT	10
#endif

#ifndef ECM_MAXE
#define	ECM_MAXE		60
#endif

#ifndef ECM_MAXT
#define	ECM_MAXT		12
#endif

#define	ECM_MAXR		(1L << ECM_MAXT)

static verylong ecm_tex[ECM_MAXE+1];
static verylong ecm_tey[ECM_MAXE+1];
static verylong ecm_coef[ECM_MAXR];
static verylong ecm_power[ECM_MAXT];
static verylong ecm_eval[ECM_MAXT];

static long 
ph1set(
	verylong n,
	verylong *rap,
	verylong *alpha,
	verylong *mu,
	verylong *x,
	verylong *f
	)
{
 /* Set up elliptic curve */
 /* if 0, success, */
 /* if 1, n factored, factor in f */
	STATIC verylong r = 0;
	STATIC verylong s = 0;
	STATIC verylong t1 = 0;
	STATIC verylong t2 = 0;
	STATIC verylong t3 = 0;
	STATIC verylong u = 0;
	STATIC verylong u2 = 0;
	STATIC verylong u3 = 0;
	STATIC verylong u4 = 0;

	zrandomb(n,&s);
	if (ziszero(s))
		zintoz(1L,&s);
	zsqmod(s,n,&r);
	zsmulmod(s,6L,n,&s);
	zsadd(r,6L,&r);
	if (zinv(r, n, &t1))
	{
		zcopy(t1, f);
		FREE3SPACE(r,s,t1);
		FREE3SPACE(t2,t3,u);
		FREE3SPACE(u2,u3,u4);
		return (1L);
	}
	zmulmod(s, t1, n, &u);
	zsqmod(u, n, &u2);
	zmulmod(u, u2, n, &u3);
	zsqmod(u2, n, &u4);
	zsmul(u4, 3L, &t1);
	zmod(t1, n, &t1);
	zsmul(u2, 6L, &t2);
	zmod(t2, n, &t2);
	zaddmod(t1, t2, n, &t1);
	zone(&t2);
	zsubmod(t2, t1, n, &t1);
	zsmul(u3, 4L, &t2);
	zmod(t2, n, &t2);
	if (zinv(t2, n, &t3))
	{
		zcopy(t3, f);
		FREE3SPACE(r,s,t1);
		FREE3SPACE(t2,t3,u);
		FREE3SPACE(u2,u3,u4);
		return (1L);
	}
	zmulmod(t1, t3, n, alpha);
	zintoz(2L, &t1);
	zaddmod(*alpha, t1, n, &t1);
	zintoz(4L, &t2);
	if (zinv(t2, n, &t3))
	{
		zcopy(t3, f);
		FREE3SPACE(r,s,t1);
		FREE3SPACE(t2,t3,u);
		FREE3SPACE(u2,u3,u4);
		return (1L);
	}
	zmulmod(t1, t3, n, rap);
	zsmul(u, 3L, &t1);
	zmod(t1, n, &t1);
	zmulmod(t1, t3, n, x);
	zintoz(3L, &t1);
	if (zinv(t1, n, &t2))
	{
		zcopy(t2, f);
		FREE3SPACE(r,s,t1);
		FREE3SPACE(t2,t3,u);
		FREE3SPACE(u2,u3,u4);
		return (1L);
	}
	zmulmod(*alpha, t2, n, &t1);
	zsubmod(n, t1, n, mu);
	ztom(*rap, rap);
	ztom(*alpha, alpha);
	ztom(*mu, mu);
	ztom(*x, x);
	FREE3SPACE(r,s,t1);
	FREE3SPACE(t2,t3,u);
	FREE3SPACE(u2,u3,u4);
	return (0L);
}

static long 
ph1exp(
	verylong n,
	verylong *x,
	verylong e,
	verylong rap,
	verylong *f
	)
{
 /* x = x^e on rap mod n */
 /* if 0, success */
 /* if 1, n factored, factor in f */
	STATIC verylong x1 = 0;
	STATIC verylong xn = 0;
	STATIC verylong xn1 = 0;
	STATIC verylong zzn = 0;
	STATIC verylong zn1 = 0;
	STATIC verylong s = 0;
	STATIC verylong s1 = 0;
	STATIC verylong d = 0;
	STATIC verylong d1 = 0;
	STATIC verylong t1 = 0;
	STATIC verylong t2 = 0;
	STATIC verylong t3 = 0;
	STATIC verylong t4 = 0;
	STATIC verylong longmask = 0;
	register long mask;
	register long maskcnt;
	register long eatmaskcnt;
	register long first = 1;

	zcopy(*x, &x1);
	zcopy(x1, &xn1);
	zcopy(zr, &zn1);
	zaddmod(xn1, zn1, n, &s1);
	zsubmod(xn1, zn1, n, &d1);
	zmontsq(s1, &t1);
	zmontsq(d1, &t2);
	zmontmul(t1, t2, &xn);
	zsubmod(t1, t2, n, &t3);
	zmontmul(rap, t3, &t4);
	zaddmod(t2, t4, n, &t4);
	zmontmul(t3, t4, &zzn);
	zintoz(1,&longmask);
	zlshift(longmask,z2log(e),&longmask);
	zsub(longmask,e,&e);
	maskcnt = longmask[0];
	mask = longmask[maskcnt] >> 1;
	FREESPACE(longmask);
	if (!mask)
	{
		mask = (RADIX >> 2);
		maskcnt --;
	}
	else
	{
		mask >>= 1;
		if (!mask)
		{
			mask = (RADIX >> 1);
			maskcnt --;
		}
	}
	for (;maskcnt >0; maskcnt --)
	{
		if (maskcnt > e[0])
			eatmaskcnt = 0;
		else
			eatmaskcnt = e[maskcnt];
		while (mask)
		{
		  zaddmod(xn, zzn, n, &s);
		  zsubmod(xn, zzn, n, &d);
		  zaddmod(xn1, zn1, n, &s1);
		  zsubmod(xn1, zn1, n, &d1);
		  if (mask&eatmaskcnt)
		  {
			if ((maskcnt ) || ((!maskcnt) && (!(mask & 1))))
			{
				if (first)
				{
					zcopy(xn, &xn1);
					zcopy(zzn, &zn1);
				}
				else
				{
					zmontsq(s1, &t1);
					zmontsq(d1, &t2);
					zmontmul(t1, t2, &xn1);
					zsubmod(t1, t2, n, &t3);
					zmontmul(rap, t3, &t4);
					zaddmod(t2, t4, n, &t4);
					zmontmul(t3, t4, &zn1);
				}
			}
			zmontmul(d1, s, &t1);
			zmontmul(d, s1, &t2);
			zaddmod(t1, t2, n, &t3);
			zmontsq(t3, &xn);
			zsubmod(t1, t2, n, &t3);
			zmontsq(t3, &t3);
			zmontmul(x1, t3, &zzn);
		  }
		  else
		  {
			zmontsq(s, &t1);
			zmontsq(d, &t2);
			zmontmul(t1, t2, &xn);
			zsubmod(t1, t2, n, &t3);
			zmontmul(rap, t3, &t4);
			zaddmod(t2, t4, n, &t4);
			zmontmul(t3, t4, &zzn);
			if ((maskcnt ) || ((!maskcnt) && (!(mask & 1L))))
			{
				zmontmul(d1, s, &t1);
				zmontmul(d, s1, &t2);
				zaddmod(t1, t2, n, &t3);
				zmontsq(t3, &xn1);
				zsubmod(t1, t2, n, &t3);
				zmontsq(t3, &t3);
				zmontmul(x1, t3, &zn1);
			}
		  }
		  mask >>= 1; first = 0;
		}
		mask = (RADIX >> 1);
	}
	if (zinv(zzn, n, f)) {
		FREE3SPACE(x1,xn,xn1);
		FREE3SPACE(zzn,zn1,s);
		FREE3SPACE(s,s1,d);
		FREE3SPACE(d1,t1,t2);
		FREE2SPACE(t3,t4);
		return (1);
	}
	zmontmul(*f, zrrr, x);
	zmontmul(*x, xn, x);
	FREE3SPACE(x1,xn,xn1);
	FREE3SPACE(zzn,zn1,s);
	FREE3SPACE(s,s1,d);
	FREE3SPACE(d1,t1,t2);
	FREE2SPACE(t3,t4);
	return (0);
}

static long 
ph1(
	verylong n,
	verylong *x,
	long m,
	verylong rap,
	verylong *f
	)
{
 /* x = x^B, B is product prime powers <=m	 */
 /* if 0, success				 */
 /* if 1, n factored, factor in f		 */
	STATIC verylong e = 0;
	register long p;
	register long q;
	register long mp;
	register long last = zp();
	register long return_value = 1;
#if (ECM_BATCH > 1)
	register long iter_cnt = 0;
#endif

	zpstart2();
	zintoz(1L,&e);
	while ((p = zpnext()) <= m)
	{
		mp = m / p;
		q = p;
		while (q <= mp)
			q *= p;
#if (ECM_BATCH <= 1)
		zintoz(q,&e);
		if (ph1exp(n, x, e, rap, f) > 0)
			goto done;
	}
#else
		zsmul(e,q,&e);
		iter_cnt ++;
		if (iter_cnt == ECM_BATCH)
		{
			if (ph1exp(n, x, e, rap, f) > 0)
				goto done;
			iter_cnt = 0;
			zintoz(1,&e);
		}
	}
	if (iter_cnt)
	{
		if (ph1exp(n, x, e, rap, f) > 0)
		goto done;
	}
#endif
	return_value = 0;
done:
	if (last)
		zpnextb(last);
	else
		zpstart2();
	FREESPACE(e);
	return(return_value);
}

static long 
ph1toph2(
	verylong n,
	verylong *x,
	verylong *y,
	verylong *ra,
	verylong alpha,
	verylong mu,
	verylong *f
	)
{
 /* tranform from first type of curve to second	 */
 /* x, alpha, mu input; x, y, ra output		 */
 /* if 0, success				 */
 /* if 1, n factored, factor in f		 */
	STATIC verylong beta = 0;
	STATIC verylong t = 0;

	zaddmod(*x, alpha, n, &t);
	zmontmul(*x, t, &t);
	zmontmul(*x, t, &t);
	zaddmod(*x, t, n, &beta);
	if (zinv(beta, n, y))
	{
		zcopy(*y, f);
		FREE2SPACE(beta,t);
		return (1L);
	}
	zmontmul(*y, zrrr, y);
	zsubmod(*x, mu, n, &t);
	zmontmul(t, *y, x);
	zmontmul(alpha, mu, &t);
	zaddmod(t, zr, n, &t);
	zmontmul(*y, t, &t);
	zmontmul(*y, t, ra);
	FREE2SPACE(beta,t);
	return (0L);
}

static long 
ph2squ(
	verylong n,
	verylong x1s,
	verylong y1s,
	verylong *x2,
	verylong *y2,
	verylong ra,
	verylong *f
	)
{
 /* (x2,y2)=(x1s,y1s)^2 on y^2=x^3+ra*x-b 	 */
 /* infinity if x1s[0] < 0			 */
 /* if 0, success				 */
 /* if 1, n factored, factor in f		 */
	STATIC verylong x1 = 0;
	STATIC verylong yy1 = 0;
	STATIC verylong t = 0;

	zcopy(x1s, &x1);
	zcopy(y1s, &yy1);
	if (x1s[0] < 0)
	{
		zsetlength(x2, (long)SIZE, "in ph2squ, fourth argument");
		(*x2)[0] = -1;
		zzero(y2);
		zzero(f);
		FREE2SPACE(x1,yy1);
		return (1);
	}
	zaddmod(yy1, yy1, n, &t);
	if (zinv(t, n, y2))
	{
		zcopy(*y2, f);
		FREE3SPACE(x1,yy1,t);
		return (1);
	}
	zmontmul(*y2, zrrr, y2);
	zmontsq(x1,x2);
	zaddmod(*x2,*x2,n,&t);
	zaddmod(*x2,t,n,&t);
	zaddmod(t, ra, n, &t);
	zmontmul(t, *y2, y2);
	zmontsq(*y2, &t);
	zsubmod(t, x1, n, &t);
	zsubmod(t, x1, n, x2);
	zsubmod(x1, *x2, n, &t);
	zmontmul(*y2, t, &t);
	zsubmod(t, yy1, n, y2);
	FREE3SPACE(x1,yy1,t);
	return (0);
}

static long 
ph2mul(
	verylong n,
	verylong x1s,
	verylong y1s,
	verylong x2s,
	verylong y2s,
	verylong *x3,
	verylong *y3,
	verylong ra,
	verylong *f
	)
{
 /* (x3,y3)=(x1s,y1s)*(x2s,y2s) on y^2=x^3+ra*x-b 	 */
 /* infinity if x1s[0] < 0 (or x2s[0]<0)			 */
 /* if 0, success					 */
 /* if 1, n factored, factor in f			 */
	STATIC verylong x1 = 0;
	STATIC verylong yy1 = 0;
	STATIC verylong x2 = 0;
	STATIC verylong y2 = 0;
	STATIC verylong t = 0;

	zcopy(x1s, &x1);
	zcopy(y1s, &yy1);
	zcopy(x2s, &x2);
	zcopy(y2s, &y2);
	if (x1s[0] < 0)
	{
		zcopy(x2, x3);
		zcopy(y2, y3);
		zcopy(y2, f);
		FREE2SPACE(x1,yy1); FREE2SPACE(x2,y2);
		return (1);
	}
	if (x2s[0] < 0)
	{
		zcopy(x1, x3);
		zcopy(yy1, y3);
		zcopy(yy1, f);
		FREE2SPACE(x1,yy1); FREE2SPACE(x2,y2);
		return (1);
	}
	if (!(zcompare(x1, x2)))
	{
		zaddmod(yy1, y2, n, &t);
		if (!t[1] && t[0] == 1)
		{
			zsetlength(x3, (long)SIZE, "in ph2mul, sixth argument");
			(*x3)[0] = -1;
			zzero(y3);
			zzero(f);
			FREE2SPACE(x1,yy1); FREE3SPACE(x2,y2,t);
			return (1);
		}
		else {
			long ret;

			ret = (ph2squ(n, x1, yy1, x3, y3, ra, f));
			FREE2SPACE(x1,yy1); FREE3SPACE(x2,y2,t);
			return ret;
		}
	}
	else
	{
		zsubmod(x1, x2, n, &t);
		if (zinv(t, n, y3))
		{
			zcopy(*y3, f);
			FREE2SPACE(x1,yy1); FREE3SPACE(x2,y2,t);
			return (1);
		}
		zmontmul(*y3, zrrr, y3);
		zsubmod(yy1, y2, n, &t);
		zmontmul(t, *y3, y3);
		zmontsq(*y3, &t);
		zsubmod(t, x1, n, &t);
		zsubmod(t, x2, n, x3);
		zsubmod(x1, *x3, n, &t);
		zmontmul(*y3, t, &t);
		zsubmod(t, yy1, n, y3);
		FREE2SPACE(x1,yy1); FREE3SPACE(x2,y2,t);
		return (0);
	}
}

static long 
ph2exp(
	verylong n,
	verylong x1,
	verylong yy1,
	verylong *x2,
	verylong *y2,
	long e,
	verylong ra,
	verylong *f
	)
{
 /* (x2,y2)=(x1,yy1)^e on y^2=x^3+ra*x-b 	 */
 /* if 0, success			 */
 /* if 1, n factored, factor in f	 */
	STATIC verylong x1s = 0;
	STATIC verylong y1s = 0;
	STATIC verylong x3 = 0;
	STATIC verylong y3 = 0;
	register long i;
	register long two;
	register long three;
	long ee[NBITSH];

	if (e == 1)
	{
		zcopy(x1, x2);
		zcopy(yy1, y2);
		return (0);
	}
	zcopy(x1, &x1s);
	zcopy(yy1, &y1s);
	if (e == 2) {
		long ret;
		ret= (ph2squ(n, x1s, y1s, x2, y2, ra, f));
		FREE2SPACE(x1s,y1s);
		return ret;
	}
	i = -1;
	two = 0;
	three = 0;
	while (e)
	{
		ee[++i] = (e & 3L);
		e >>= 2;
		if (ee[i] == 3L)
			three = 1L;
	}
	if (three || ee[i] == 2L)
	{
		if (ph2squ(n, x1s, y1s, x2, y2, ra, f)) {
			FREE2SPACE(x1s,y1s);
			return (1L);
		}
		two = 1;
		if (three && ph2mul(n, x1s, y1s, *x2, *y2, &x3, &y3, ra, f)) {
			FREE2SPACE(x1s,y1s); FREE2SPACE(x3,y3);
			return (1L);
		}
	}
	if (ee[i] == 3L)
	{
		zcopy(x3, x2);
		zcopy(y3, y2);
	}
	if (ee[i] == 1L)
	{
		if (two)
			two = 0;
		else
		{
			two = 1L;
			zcopy(x1s, x2);
			zcopy(y1s, y2);
		}
	}
	for (i = i - 1; i >= 0; i--)
	{
		if (two)
		{
			if (ph2squ(n, *x2, *y2, x2, y2, ra, f)) {
				FREE2SPACE(x1s,y1s); FREE2SPACE(x3,y3);
				return (1L);
			}
		}
		else
			two = 1L;
		switch (ee[i])
		{
			case 0:
				{
					if (ph2squ(n, *x2, *y2, x2, y2, ra, f)) {
						FREE2SPACE(x1s,y1s); FREE2SPACE(x3,y3);
						return (1);
					}
					break;
				}
			case 1:
				{
					if (ph2squ(n, *x2, *y2, x2, y2, ra, f)) {
						FREE2SPACE(x1s,y1s); FREE2SPACE(x3,y3);
						return (1);
					}
					if (ph2mul(n, x1s, y1s, *x2, *y2, x2, y2, ra, f)) {
						FREE2SPACE(x1s,y1s); FREE2SPACE(x3,y3);
						return (1);
					}
					break;
				}
			case 2:
				{
					if (ph2mul(n, x1s, y1s, *x2, *y2, x2, y2, ra, f)) {
						FREE2SPACE(x1s,y1s); FREE2SPACE(x3,y3);
						return (1);
					}
					if (ph2squ(n, *x2, *y2, x2, y2, ra, f)) {
						FREE2SPACE(x1s,y1s); FREE2SPACE(x3,y3);
						return (1);
					}
					break;
				}
			case 3:
				{
					if (ph2squ(n, *x2, *y2, x2, y2, ra, f)) {
						FREE2SPACE(x1s,y1s); FREE2SPACE(x3,y3);
						return (1);
					}
					if (ph2mul(n, x3, y3, *x2, *y2, x2, y2, ra, f)) {
						FREE2SPACE(x1s,y1s); FREE2SPACE(x3,y3);
						return (1);
					}
					break;
				}
		}
	}
	FREE2SPACE(x1s,y1s); FREE2SPACE(x3,y3);
	return (0);
}

static long 
ph2set(
	verylong n,
	verylong x,
	verylong y,
	long te,
	verylong ra,
	verylong *f
	)
{
 /* set up table for table size te	 */
 /* if 0, success			 */
 /* if 1, n factored, factor in f	 */
	STATIC verylong z = 0;
	register long i;
	register long j;
	long expi;
	long exp2;

#ifdef ALPHA_OR_ALPHA50
	expi = (1L<<30) / (te + 2);
#else
	expi = RADIX / (te + 2);
#endif

	exp2 = 1 + zrandom(expi);
	expi = 1 + zrandom(expi);

	for (i = 0; i <= te; i++)
	{
		zcopy(x, &(ecm_tex[i]));
		zcopy(y, &(ecm_tey[i]));
		for (j = te; j > 0; j--)
			if (ph2exp(n, ecm_tex[i], ecm_tey[i], &(ecm_tex[i]), &(ecm_tey[i]), expi, ra, f))
				return (1);
		expi += exp2;
	}
	for (j = 1; j <= te; j++)
	{
		for (i = te; i >= j; i--)
		{
			zsubmod(n, ecm_tey[i - 1], n, &z);
			if (ph2mul(n, ecm_tex[i], ecm_tey[i], ecm_tex[i - 1], z, &(ecm_tex[i]), &(ecm_tey[i]), ra, f))
				return (1);
		}
	}
	FREESPACE(z);
	return (0);
}

static void
ph2prec(
	verylong n,
	long n1,
	long j
	)
{
	STATIC verylong mcn2j = 0;
	STATIC verylong t = 0;
	register long i;
	register long n2;
	register long n2j;

	if (n1 > 1)
	{
		n2 = ((n1 - 1) >> 1);
		n2j = j + n2;
		zaddmod(ecm_coef[n2j], znm, n, &(ecm_coef[n2j]));
		zsubmod(n, ecm_coef[n2j], n, &mcn2j);
		n2j ++;
		for (i = 0; i < n2; i++)
		{
			zmontmul(ecm_coef[i + n2j], mcn2j, &t);
			zaddmod(ecm_coef[i + j], t, n, &(ecm_coef[i + j]));
		}
		ph2prec(n, n2, n2j);
		ph2prec(n, n2, j);
	}
	FREE2SPACE(mcn2j,t);
}

static void
ph2eval(
	verylong n,
	long ind,
	long n1,
	long j,
	verylong *result
	)
{
	register long n2;

	if (n1 == 1)
		zaddmod(ecm_coef[j], ecm_power[0], n, result);

	else
	{
		n2 = ((n1 - 1) >> 1);
		zaddmod(ecm_coef[n2 + j], ecm_power[ind], n, &(ecm_eval[ind]));
		ph2eval(n, ind - 1, n2, n2 + j + 1, result);
		zmontmul((ecm_eval[ind]), *result, &(ecm_eval[ind]));
		ph2eval(n, ind - 1, n2, j, result);
		zaddmod((ecm_eval[ind]), *result, n, result);
	}
}

static long 
ph2(
	verylong n,
	verylong inx,
	verylong iny,
	long m,
	long te,
	verylong ra,
	verylong *f
	)
{
 /* does second phase for m 		 */
 /* if 0, no factor found		 */
 /* if 1, n factored, factor in f	 */
	static long non_initialized = 1;
	STATIC verylong x = 0;
	STATIC verylong y = 0;
	STATIC verylong x1 = 0;
	STATIC verylong x2 = 0;
	STATIC verylong prod = 0;
	register long j;
	register long k;
	long r;
	long r2;
	long s;
	long ind;

	if (non_initialized)
	{
		for (j = 0; j < ECM_MAXR; j++)
			ecm_coef[j] = 0;
		for (j = 0; j <= ECM_MAXE; j++)
			ecm_tex[j] = ecm_tey[j] = 0;
		for (j = 0; j < ECM_MAXT; j++)
			ecm_power[j] = 0;
		for (j = 0; j < ECM_MAXT; j++)
			ecm_eval[j] = 0;
		non_initialized = 0;
	}
	zcopy(inx, &x);
	zcopy(iny, &y);
	if (ph2set(n, x, y, te, ra, f)) {
		FREE2SPACE(x,y);
		return (1L);
	}
	if ((r = 100 * te) >= ECM_MAXR)
		r = ECM_MAXR - 1;
	r2 = 1;
	ind = 0;
	j = 3;
	while (j <= r)
	{
		ind++;
		r2 = j;
		j <<= 1;
		j++;
	}
	r = r2;
	s = m / r;
	while (r > 3 && s < 2 * r)
	{
		r >>= 1;
		ind--;
		s = m / r;
	}
	zcopy(zr, (&(ecm_coef[0])));
	for (j = 1; j <= r; j++)
	{
		for (k = 0; k < te; k++)
		{
			if (ph2mul(n, ecm_tex[k], ecm_tey[k], ecm_tex[k + 1], ecm_tey[k + 1], &(ecm_tex[k]), &(ecm_tey[k]), ra, f)) {
				FREE2SPACE(x,y);
				return (1L);
			}
		}
		zsubmod(n, ecm_tex[0], n, &x1);
		zcopy(ecm_coef[j - 1], &ecm_coef[j]);
		for (k = j - 1; k > 0; k--)
		{
			zmontmul(x1, ecm_coef[k], &x2);
			zaddmod(x2, ecm_coef[k - 1], n, &(ecm_coef[k]));
		}
		zmontmul(x1, ecm_coef[0], &(ecm_coef[0]));
	}
	ph2prec(n, r, 0L);
	if (ph2set(n, x, y, te, ra, f)) {
		FREE2SPACE(x,y); FREE2SPACE(x1,x2);
		return (1L);
	}
	zone(&prod);
	for (j = s; j > 0; j--)
	{
		for (k = 0; k < te; k++)
		{
			if (ph2mul(n, ecm_tex[k], ecm_tey[k], ecm_tex[k + 1], ecm_tey[k + 1], &(ecm_tex[k]), &(ecm_tey[k]), ra, f)) {
				FREE2SPACE(x,y); FREE3SPACE(x1,x2,prod);
				return (1L);
			}
		}
		zcopy(ecm_tex[0], &(ecm_power[0]));
		for (k = 0; k < ind; k++)
			zmontsq(ecm_power[k], &(ecm_power[k + 1]));
		ph2eval(n, ind, r, 0L, &x2);
		zmontmul(x2, prod, &prod);
	}
	if (zinv(prod, n, f)) {
		FREE2SPACE(x,y); FREE3SPACE(x1,x2,prod);
		return (1L);
	}
	FREE2SPACE(x,y); FREE3SPACE(x1,x2,prod);
	return (0L);
}

static void
message(
	char *str,
	double t,
	FILE *fp
	)
{
	fprintf(fp,"%s, %8.3f seconds\n", str, gettime() - t);
	fflush(fp);
}

long 
zecm_trial(
	verylong n,
	long m,
	verylong *f,
	long info,
	FILE *fp
	)
{
 /* attempt to factor n, smoothness bound phase 1 is m	 */
 /* if 0, no success					 */
 /* if 1, n factored in setup, factor in f 		 */
 /* if 2, n factored in phase 1, factor in f 		 */
 /* if 3, n factored in ph1toph2, factor in f 		 */
 /* if 4, n factored in phase 1, factor in f 		 */
	STATIC verylong x = 0;
	STATIC verylong y = 0;
	STATIC verylong rap = 0;
	STATIC verylong alpha = 0;
	STATIC verylong mu = 0;
	STATIC verylong ra = 0;
	static long te;
	double tcnt = gettime();

	if (ph1set(n, &rap, &alpha, &mu, &x, f) > 0) {
		FREE2SPACE(x,rap); FREE2SPACE(alpha,mu);
		return (1L);
	}
	if (info >= 2)
		message("   curve initialized", tcnt,fp);
	if (m > ECM_MAXBOUND)
		m = ECM_MAXBOUND;
	if (ph1(n, &x, m, rap, f)) {
		FREE2SPACE(x,rap); FREE2SPACE(alpha,mu);
		return (2L);
	}
	if (info >= 2)
		message("   phase 1 completed", tcnt,fp);
	if (ph1toph2(n, &x, &y, &ra, alpha, mu, f)) {
		FREE3SPACE(x,rap,y); FREE3SPACE(alpha,mu,ra);
		return (3L);
	}
  	if (m >= 4500000)
		te = 60;
	else if (m >= 1125000)
		te = 30;
	else if (m >= 720000)
		te = 24;
	else if (m >= 405000)
		te = 18;
	else if (m >= 180000)
		te = 12;
	else if (m >= 45000)
		te = 6;
	else if (m >= 11250)
		te = 3;
	else if (m >= 5000)
		te = 2;
	else
		te = 1;
	if (te > ECM_MAXE)
		te = ECM_MAXE;
	m *= ECM_MULT;
	if (ph2(n, x, y, m, te, ra, f)) {
		FREE3SPACE(x,rap,y); FREE3SPACE(alpha,mu,ra);
		return (4L);
	}
	if (info >= 2)
		message("   phase 2 completed", tcnt,fp);
	FREE3SPACE(x,rap,y); FREE3SPACE(alpha,mu,ra);
	return (0);
}


long
zecm(
	verylong n,
	verylong *f,
	long uni,
	long nb,
	long bound,
	long grow,
	long co,
	long info
	)
{
	return zfecm(n,f,uni,&nb,&bound,grow,co,info,stdout);
}


long 
zfecm(
	verylong n,
	verylong *f,
	long uni,
	long *nb,
	long *bound,
	long grow,
	long co,
	long info,
	FILE *fp
	)
{
 /* attempt to factor n, factor in f */
 /* uni is seed for random number generator */
 /* uni=0 means random generator started already */
 /* *nb is number of curves */
 /* *bound is bound for phase 1 of first curve */
 /* grow is percentage of growth of bound per iteration */
 /* |co| is maximum number of probabilistic compositeness tests */
 /* info = 0: no timings or information */
 /* |info|=1: timing per curve, and some additional info */
 /* |info|>1: timing per phase and per curve */

	STATIC verylong q = 0;
	STATIC verylong r = 0;
	register long i;
	register long j;
	double tcnt = gettime();

	if (fp == NULL)
		info=0;
	if (info < 0)
		info = -info;
	if (*bound < ECM_MINBOUND)
		*bound = ECM_MINBOUND;
	if (grow < 0)
		grow = 0;
	if (uni)
	{
		if (uni < 0)
			uni = ((-uni) << 1);
		else
			uni = 2 * uni + 1;
		zrstarts(uni + 1);
	}
	if ((!n) || (zscompare(n,1) <= 0))
	{
		if (info)
			fprintf(fp,"wrong input to zfecm\n");
		return (0);
	}
#ifndef ALPHA_OR_ALPHA50
	if (n[0] == 1)
#else
	if (n[0]==1 && n[1] < (1L<<40))
#endif
	{
#ifndef ALPHA_OR_ALPHA50
		register long div = ztridiv(n,f,2,RADIXROOT);
#else
		register long div = ztridiv(n,f,2,1L << 20);
#endif
		if ((div > RADIXROOT) || (div == n[1]))
		{
			if (info)
			{
				fprintf(fp,"zfecm input is prime\n");
				fflush(fp);
			}
			*nb=0;
			return(-1);
		}
		zintoz(div,f);
		if (info)
		{
			fprintf(fp,"factor found using trial division\n");
			fflush(fp);
		}
		*nb=0;
		return(1);
	}
	if (!(n[1] & 1))
	{
		zintoz(2, f);
		if (info)
		{
			fprintf(fp,"zfecm input is even\n");
			fflush(fp);
		}
		*nb = 0;
		return (1);
	}
	if (info)
	{
		i = zfwrite_c(fp, n, 70, "zfecm on ", "");
		fprintf(fp,"\n(%ld digits)\n", i);
	}
	if (co)
	{
		if (co > 0)
			co = -co;
		zcopy(n, f);
		i = zcomposite(f, co, 0);
		if (!i)
		{
			if (info)
				message("zfecm input is probably prime", tcnt,fp);
			*nb = 0;
			return (-1);
		}
		if (i == -1)
		{
			if (info)
				message("factor in compositeness test", tcnt,fp);
			*nb=0;
			return (1);
		}
		if (i != 1)
		{
			zhalt("impossible return value of zcomposite in zfecm   BUG");
			return (0);
		}
		if (info >= 2)
			message("zfecm input composite, not a prime power", tcnt,fp);
	}
	else if (info >= 2)
	{
		fprintf(fp,"zfecm input assumed to be composite, and not a prime power\n");
		fflush(fp);
	}
#ifndef ALPHA_OR_ALPHA50
	if (n[0]==2)
#else 
	if (n[0]==1)
#endif
	{
		if (info) {
			fprintf(fp,"attempting to factor with zsquf: ");
			zfwriteln(fp,n);
		}
		if (zsquf(n,f,&q) && zscompare(*f,1L) > 0 && zcompare(*f,n) <0) {
			if (info)
				fprintf(fp,"factored with zsquf\n");
			*nb = 0;
			FREESPACE(q);
			return 1;
		}
		if (info)
			fprintf(fp,"failed to factor with zsquf\n");
	}

	zone(f);
	zmkeep(n);

	for (i = *nb; i > 0; i--)
	{
		if (info)
			fprintf(fp,"%ld-th zfecm trial with bound %ld\n", *nb - i + 1, *bound);
		j = zecm_trial(n, *bound, f, info,fp);
		*bound += (long)(*bound*grow/100);
		if (j)
		{
			if (((*f)[0] == 1 && (*f)[1] <= 1) || !zcompare(n, *f))
			{
				if (info)
					message("trivial factor in zfecm", tcnt,fp);
				continue;
			}
			if (info)
			{
				fprintf(fp,"factor in ");
				switch (j)
				{
					case 1:
						{
							message("curve set-up", tcnt,fp);
							break;
						}
					case 2:
						{
							message("phase 1", tcnt,fp);
							break;
						}
					case 3:
						{
							message("curve tranformation (phase 1 -> 2)", tcnt,fp);
							break;
						}
					case 4:
						{
							message("phase 2", tcnt,fp);
							break;
						}
				}
			}
			zdiv(n, *f, &q, &r);
			if (r[1] || r[0] != 1)
			{
				zfwriteln(stderr,n);
				zfwriteln(stderr,*f);
				zfwriteln(stderr,r);
				zhalt("this is a wrong factor, found in zfecm   BUG");
				zmback();
				FREE2SPACE(q,r);
				return (0);
			}
			zmback();
			FREE2SPACE(q,r);
			*nb = *nb - i + 1;
			return (1L);
		}
		if (info)
		{
			message("no success", tcnt,fp);
		}
	}
	if (info)
		fprintf(fp,"done\n");
	zmback();
	FREE2SPACE(q,r);
	return (0);
}

#else

long 
zfecm(
	verylong n,
	verylong *f,
	long uni,
	long *nb,
	long *bound,
	long grow,
	long co,
	long info,
	FILE *fp
	)
{
	static long message = 0;
	if (!message)
	{
		fprintf(stderr,
			"Compile without the -DNO_ECM flag to get ecm code\n");
		message = 1;
	}
	return(0);
}


long
zecm(
        verylong n,
        verylong *f,
        long uni,
        long nb,
        long bound,
        long grow,
        long co,
        long info
        )
{
	static long message = 0;
	if (!message)
	{
		fprintf(stderr,
			"Compile without the -DNO_ECM flag to get ecm code\n");
		message = 1;
	}
	return(0);
}


#endif


long
zispower(
	verylong in_a,
	verylong *f
	)
{

	register long p,l,c,r=0;
	STATIC verylong temp=0,a=0,t2=0;

	if (zscompare(in_a,1L) <= 0)	/* assume a > 1 */
		return 0L;

	zcopy(in_a,&a);

	while ( ( (a[1]&7)==1 || !(a[1]&1L) ) && zsqrt(a,&temp,&t2)==1) {
	 	if (!r) r=2;
		else r*=2;
		zcopy(temp,&a);
	}
	zpstart();

	l=z2log(a);
	while (1) {
		register long j,q;

		p = zpnext();
		if (p * (z2logs(p)-1) > l)
			goto stage2;
samep:
		if (zsmod(a,p)) {
			if (p < RADIXROOT) {
				c = p*p;
				if(zexpmods( zsmod(a,c),p-1,c) != 1)
					continue;
			}
		}
		else if (p < RADIXROOT && zsmod(a,p*p))
			goto done;

		for (j=2; ; j+=2) {
			q= j*p + 1;
			if (!(q%3)) continue;
			for (c=5; c*c <= q; ) {
				if (!(q % c))
					break;
				c+=2;
				if (c*c > q)
					break;
				if (!(q % c))
					break;
				c+=4;
			}
			if (c*c > q)
				break;
		}
		/* q is prime */
		c = zsmod(a,q);
		if (c > 1 && zexpmods(c,j,q)!=1)
			continue;

		if (zroot(a,p,&temp)==1) {
			if (!r) r=p;
			else r*=p;
			zcopy(temp,&a);
			l=z2log(a);
			goto samep;
		}
		if (zscompare(temp,p) <= 0)
			break;
	}

	p = ztoint(temp) -1;
stage2:
	l=a[0];

	for (; p>1; p--) {
		register e;

		/* make sure p is not a power */
		if (p > 3) {
			register int i,j;

			for (i=2; (j=i*i) <= p; i++) {
				while (j < p)
					j *= i;
				if (j==p)
					break;
			}
			if (j==p)
				continue;
		}

		if (zsmod(a,p))
			continue;
		c=p;
		e=1;
		if (p < RADIXROOT && a[0] > 1) {
			c=p*p;
			e=2;
			if (zsmod(a,c))
				goto done;
			while (z2logs(c)+z2logs(p) < NBITS)
				c *=p, e++;
			if (c > p*p && zsdiv(a,c,&t2))
				continue;
		}


		temp[0]=1;
		temp[1]=c;
		while (temp[0] < l) {
			zsmul(temp,p,&temp);
			e++;
		}

		while ( (c=zcompare(temp,a)) < 0) {
			zsmul(temp,p,&temp);
			e++;
		}
		if (!c) {
			zintoz(p,f);
			FREE3SPACE(temp,a,t2);
			if (!r) return e;
			return r*e;
		}
	}
done:
	if (r)
		zcopy(a,f);
	FREE3SPACE(temp,a,t2);
	return r;
}

static long
is_sq(long n)
{
/* 
   Test whether n is a perfect square.
   If so, return its square root.  If not, return -1.

   Optimized for case where most arguments will be non-squares.
*/
   static int first = 1;
   static unsigned char squtab[667]; /* Quadratic residues for primes <= 53 */
   register long nn = n;
#define MSK371 1
#define MSK517 2
#define MSK559 4
#define MSK589 8
#define MSK615 16
#define MSK629 32
#define MSK667 64
   if (first) {
	register long i;
	first = 0;
	for (i = 0; i < 667; i++) squtab[i] = 0;
	for (i = 0; i < 334; i++) {
	    register long isq = i*i;
	    squtab[isq%371] |= MSK371;  squtab[isq%517] |= MSK517;
	    squtab[isq%559] |= MSK559;  squtab[isq%589] |= MSK589;
	    squtab[isq%615] |= MSK615;  squtab[isq%629] |= MSK629;
	    squtab[isq%667] |= MSK667;  /* Initialize quadratic residues */
	}
   } 
   if (nn < 0) return -1;
   if (nn < 1) return nn;
   while ((nn & 3) == 0 ) nn >>= 2;
   if (    (nn & 7) == 1             && (squtab[nn%371] & MSK371)
	&& (squtab[nn%517] & MSK517) && (squtab[nn%559] & MSK559)
	&& (squtab[nn%589] & MSK589) && (squtab[nn%615] & MSK615)
	&& (squtab[nn%629] & MSK629) && (squtab[nn%667] & MSK667)) {
		/* Checks modulo all primes <= 53 */ 
	long root = zsqrts(n);
        if (n == root*root) return root;
	return -1;
   } else {
	return -1;
   }
#undef MSK371
#undef MSK517
#undef MSK559
#undef MSK589
#undef MSK615
#undef MSK629
#undef MSK667
}

#if NBITS == 26

long
zsquf(verylong n, verylong *f1, verylong *f2) {
#define MSD_60	30
	static verylong t1=0;
	static verylong t2=0;
	register long iter, nsqroot, Qprev, Qnow, Pnow, iterbnd=50000;
	register long den_bound, nsmallden=0, donethat=0;
	long smalldens[MSD_60+1];
	int phase = 1;
	if (zsqrt(n,&t1,&t2)) {
		zcopy(t1,f1);
		zcopy(t1,f2);
		return(1);
	}
	/* dirty: */
	Qnow = t2[1];
	if (t2[0]>1) Qnow += ((t2[2]&((1<<(30-NBITS))-1))<<NBITS);
	nsqroot = t1[1];
	if (t1[0]>1) nsqroot += ((t1[2]&((1<<(30-NBITS))-1))<<NBITS);

	Pnow = nsqroot; Qprev = 1;
	den_bound = zsqrts(2*nsqroot+1);
	for (iter = 1; iter <iterbnd; iter++) {
		long Pprev = Pnow, Qback2 = Qprev, root, den;
		Qprev = Qnow;
		if (nsqroot + Pprev >= 2*Qprev) {
			long quot = (nsqroot + Pprev)/Qprev;
			Pnow = quot * Qprev -Pprev;
			Qnow = Qback2 + quot*(Pprev-Pnow);
		} else {
			Pnow = Qprev - Pprev; Qnow = Qback2+Pprev-Pnow;
		}
		den = Qprev;
		if ((den & 1) == 0) den >>= 1;
		if (Pnow == Pprev) {
			zintoz(den,f1);
			if (zsdiv(n,den,f2)) {
				/* fprintf(stderr,"error 3 in zsquf_60\n"); */
				return 0;
			}
			if (den > 1)
				return(iter);
			else return 0;
		}
		if (phase && den<den_bound && den >1) {
			if (nsmallden <MSD_60) {
				smalldens[++nsmallden] = den;
			} else {
				return 0;
			}
		}
		if (phase && (iter & 1) && ((root=is_sq(Qnow)) > 0)) {
			register long j = nsmallden;
			smalldens[0] = root;
			while (smalldens[j] != root) j--;
			if (root == 1) {
				return 0;
			} else if (j == 0 ) {
				phase = 0;
				if (donethat) {
					return 0;
				}
				iterbnd += iter;
				donethat = 1;
				Pnow = nsqroot - (nsqroot-Pnow)%root;
				zintoz(Pnow,&t1);
				zsq(t1,&t2);
				zsub(n,t2,&t2);
				if (zsdiv(t2,root,&t1)) {
				    /* fprintf(stderr,"error in zsquf_60\n"); */
					return 0;
				}
				Qnow = t1[1];
				if (t1[0]>1) Qnow +=
					((t1[2]&((1<<(30-NBITS))-1))<<NBITS);
				Qprev = root;
			}
		}
	}
	return 0;
}

#else

long zsquf(verylong n,verylong *f1,verylong *f2) {
#define MSD	30
	STATIC verylong t1=0;
	STATIC verylong t2=0;
	register long iter, nsqroot, Qprev, Qnow, Pnow, iterbnd=50000;
	register long den_bound, nsmallden=0, donethat=0;
	long smalldens[MSD+1];
	int phase = 1;
	if (n[0] > 2) return 0;
	if (zsqrt(n,&t1,&t2)) {
		zcopy(t1,f1);
		zcopy(t1,f2);
		FREE2SPACE(t1,t2);
		return(1);
	}
	Qnow = t2[1]; nsqroot = t1[1]; Pnow = nsqroot; Qprev = 1;
	den_bound = zsqrts(2*nsqroot+1);
	for (iter = 1; iter <iterbnd; iter++) {
		long Pprev = Pnow, Qback2 = Qprev, root, den;
		Qprev = Qnow;
		if (nsqroot + Pprev >= 2*Qprev) {
			long quot = (nsqroot + Pprev)/Qprev;
			Pnow = quot * Qprev -Pprev;
			Qnow = Qback2 + quot*(Pprev-Pnow);
		} else {
			Pnow = Qprev - Pprev; Qnow = Qback2+Pprev-Pnow;
		}
		den = Qprev;
		if ((den & 1) == 0) den >>= 1;
		if (Pnow == Pprev) {
			FREE2SPACE(t1,t2);
			zintoz(den,f1);
			if (zsdiv(n,den,f2)) {
				/* printf("error 3 in zsquf\n"); */
				return 0;
			}
			if (den > 1)
				return(iter);
			else return 0;
		}
		if (phase && den<den_bound && den >1) {
			if (nsmallden <MSD) {
				smalldens[++nsmallden] = den;
			} else {
				/* printf("Increase MSD\n"); */
				FREE2SPACE(t1,t2);
				return 0;
			}
		}
		if (phase && (iter & 1) && ((root=is_sq(Qnow)) > 0)) {
			register long j = nsmallden;
			smalldens[0] = root;
			while (smalldens[j] != root) j--;
			if (root == 1) {
				FREE2SPACE(t1,t2);
				return 0;
			} else if (j == 0 ) {
				phase = 0;
				if (donethat) {
					FREE2SPACE(t1,t2);
					return 0;
				}
				iterbnd += iter;
				donethat = 1;
				Pnow = nsqroot - (nsqroot-Pnow)%root;
				zintoz(Pnow,&t1);
				zsq(t1,&t2);
				zsub(n,t2,&t2);
				if (zsdiv(t2,root,&t1)) {
					/* printf("error in zsquf\n"); */
					FREE2SPACE(t1,t2);
					return 0;
				}
				Qnow = t1[1];
				Qprev = root;
			}
		}
	}
	FREE2SPACE(t1,t2);
	return 0;
}
#endif

#ifdef ALPHA50
#define lower_radix(in,out) { \
register long i=1,j=0,rb=0,nrb=0,bits; \
register verylong pin=in; \
long size; \
 \
if ((size= *pin)==2) { \
	pin++; \
	out[1] = *pin & ALPHA50RADIXM; \
	out[2] = *pin >> ALPHA50NBITS; \
	pin++; \
	out[2] |= (*pin & ((1L << (2*ALPHA50NBITS-NBITS))-1L)) << (NBITS - \
	 ALPHA50NBITS); \
	bits = *pin >> (2*ALPHA50NBITS-NBITS); \
	if (bits) { \
	        out[3] = bits; \
	        out[0]=3; \
	        } \
	else \
	        out[0]=2; \
	} \
else { \
	for (j=0; i <= size ; j++) { \
	        if (nrb >= ALPHA50NBITS) { \
	                bits = rb & ALPHA50RADIXM; \
	                rb >>= ALPHA50NBITS; \
	                nrb -= ALPHA50NBITS; \
	                } \
	        else { \
	                bits = rb | ((((1L << (ALPHA50NBITS - nrb)) -1L) & \
	                 pin[i]) << nrb); \
	                rb = pin[i] >> (ALPHA50NBITS - nrb); \
	                nrb += (NBITS-ALPHA50NBITS); \
	                i++; \
	                } \
	        out[j+1]=bits; \
	        } \
	if (rb) { \
	        j++; \
	        if (rb < (1L<<ALPHA50NBITS)) \
	                out[j]= rb; \
	        else { \
	                out[j]= rb & (( 1L << ALPHA50NBITS) -1); \
	                j++; \
	                out[j] = rb>>ALPHA50NBITS; \
	                } \
	        } \
	out[0]=j; \
	} \
}


/* convert from ALPHA50NBITS to NBITS radix */
/* this can probably be made faster */

#define raise_radix(size,in,out) { \
register long i,j,nrb,bits; \
 \
nrb=ALPHA50NBITS; \
for (i=0,j=1; i<size; j++) { \
	bits = (in[i++] >> (ALPHA50NBITS-nrb)); \
	if (i <size ) { \
	        if (nrb + ALPHA50NBITS >= NBITS) { \
	                bits = bits | ((in[i] & ( (1L<< (NBITS-nrb)) - 1)) << \
	                 nrb); \
	                nrb -= (NBITS-ALPHA50NBITS); \
	                if (!nrb) { i++; nrb=ALPHA50NBITS; } \
	                } \
	        else { \
	                bits = bits | (in[i++] << nrb); \
	                if (i< size) \
	                        bits = bits | ((in[i] & ((1L <<( (NBITS- \
	                         ALPHA50NBITS)-nrb)) -1)) << (ALPHA50NBITS \
	                         + nrb)); \
	                nrb = 2*ALPHA50NBITS-NBITS+nrb; \
	                if (!nrb) { i++; nrb=ALPHA50NBITS; } \
	                } \
	        } \
	out[j]=bits; \
	} \
if (j>2 && !out[j-1]) j--; \
out[0]=j-1; \
}


#define raise_radix2(size,in,out) { \
register long i,j,nrb,bits; \
 \
nrb=ALPHA50NBITS; \
for (i=1,j=1; i<=size; j++) { \
	bits = (in[i++] >> (ALPHA50NBITS-nrb)); \
	if (i <=size ) { \
	        if (nrb + ALPHA50NBITS >= NBITS) { \
	                bits = bits | ((in[i] & ( (1L<< (NBITS-nrb)) - 1)) << nrb); \
	                nrb -= (NBITS-ALPHA50NBITS); \
	                if (!nrb) { i++; nrb=ALPHA50NBITS; } \
	                } \
	        else { \
	                bits = bits | (in[i++] << nrb); \
	                if (i<=size) \
	                        bits = bits | ((in[i] & ((1L <<( (NBITS- \
	                         ALPHA50NBITS)-nrb)) -1)) << (ALPHA50NBITS+nrb)); \
	                nrb = 2*ALPHA50NBITS-NBITS+nrb; \
	                if (!nrb) { i++; nrb=ALPHA50NBITS; } \
	                } \
	        } \
	out[j]=bits; \
	} \
if (j>2 && !out[j-1]) j--; \
out[0]= (j > 1) ? j-1:1 ; \
}



/* 50 bit division for the alpha */

void
zdiv(
	verylong in_a,
	verylong in_b,
	verylong *qqq,
	verylong *rrr
	)

{
	register long i;
	register long qq;
	long sa;
	long sb;
	long sq;
	verylong p;
	verylong pc;
	long sign;
	STATIC verylong a = 0;
	STATIC verylong b = 0;
	STATIC verylong c = 0;
	STATIC verylong d = 0;
	double btopinv;
	double aux;
	verylong q = *qqq;
	verylong r = *rrr;

/*printf("zdiv: "); zwrite(in_a); printf(" div "); zwriteln(in_b);*/
#ifndef START
	if (fudge < 0)
	        zstart();
#endif
	if (ALLOCATE && !in_a)
	{
	        zzero(qqq);
	        zzero(rrr);
	        return;
	}
	if ((!in_b) || (((sb = in_b[0]) == 1) && (!in_b[1])))
	{
	        zhalt("division by zero in zdiv");
	        return;
	}

	sign = (*in_a < 0 ? 2 : 0) | (*in_b < 0 ? 1 : 0);
	if (*in_a < 0)
	        (*in_a) = (-(*in_a));
	if (*in_b < 0)
	        (*in_b) = (-(*in_b));

	if ((*in_b == 1) && (in_b[1] < ALPHA50RADIX)) {
	        zintoz(zsdiv(in_a, in_b[1], qqq), rrr);
	        goto done;
	        }

	zsetlength(&a,(long) (NBITS/(double)ALPHA50NBITS*(double)in_a[0]+1.0),
	        "");
	lower_radix(in_a,a);    /* test this for large integers */

	/* convert b */
	zsetlength(&b,(long) (NBITS/(double)ALPHA50NBITS*(double)in_b[0]+1.0),
	        "");
	lower_radix(in_b,b);

	sa = (*a);
	sb = (*b);
	zsetlength(&c, (i = (sa > sb ? sa : sb) + 1), "in zdiv, locals\n");
	zsetlength(&d, i, "");
	zsetlength(&q, i, "in zdiv, third argument");
	*qqq = q;
	zsetlength(&r, sb + 1, "in zdiv, fourth argument");
	*rrr = r;
	p = &b[sb];

	sq = sa - sb;
	btopinv = (double) (*p) * alpha50fradix;
	if (sb > 1)
	        btopinv += (*(--p));
	btopinv *= (double) alpha50fradix;
	if (sb > 2)
	        btopinv += (*(p - 1));
	btopinv = alpha50fudge / btopinv;
	p = &a[1];
	pc = &c[0];
	for (i = sa; i > 0; i--)
	        *pc++ = *p++;
	p = pc;
	sa = 1 - sb;
	for (i = (-sq); i > 0; i--)
	        *p++ = 0;
	*pc = 0;
	d[1] = 0;
	p = &d[sq + 1];
	for (i = sq; i >= 0; i--)
	{
	        aux = alpha50fradix * (alpha50fradix * (*pc) +
	            (*(pc - 1))) + 1.0;
	        if (i > sa)
	                aux += (*(pc - 2));
	        qq = (long) (btopinv * aux + 0.5);
	/* dirty, but safe. On most machines (long)(btopinv*aux) */
	/* is correct, or one too big; on some however it becomes */
	/* too small. Could change zstart, but +0.5 and a while */
	/* instead of one single if is safer */
	        if (qq >= ALPHA50RADIX)
	                qq = ALPHA50RADIX-1;
	        if (qq > 0)
	        {
	                zalphasubmul(qq, &c[i], &b[0]);
	                while (*pc < 0)
	                {
	                        qq--;
	                        {
	                                zalphaaddmulone(&c[i], &b[0]);
	                        }
	                }
	        }
	        pc--;
	        *p-- = qq;
	}
	sb--;
	while ((sb > 0) && (!(c[sb])))
	        sb--;
	sb++;
	raise_radix(sb,c,r);

	if (sq < 0)
	{
	        q[0] = 1;
	        q[1] = d[1];
	}
	else
	{
	        sq++;
	        while ((sq > 1) && (!(d[sq])))
	                sq--;
	        raise_radix2(sq,d,q);
	}

	if (sign)
	{
	        if (sign <= 2)
	        {
	                if (!(r[1]) && (r[0] == 1))
	                        q[0] = -(q[0]);
	                else
	                {
	                        zadd(q, one, &q);
	                        q[0] = -q[0];
	                        if (sign == 1)
	                                zsub(r, b, &r);
	                        else
	                                zsub(b, r, &r);
	                }
	        }
	        else
	                znegate(&r);
	}
	*qqq = q;
	*rrr = r;
done:	;

	FREE2SPACE(a,b); FREE2SPACE(c,d);
}


void
zmod(
	verylong in_a,
	verylong in_b,
	verylong *rr
	)
{
	register long i;
	register long qq;
	verylong r = *rr;
	STATIC verylong a = 0;
	STATIC verylong b = 0;
	STATIC verylong c = 0;
	long sa;
	long sb;
	long sq;
	verylong p;
	verylong pc;
	long sign;
	double btopinv;
	double aux;

#ifndef START
	if (fudge < 0)
	        zstart();
#endif
	if (ALLOCATE && !in_a)
	{
	        zzero(rr);
	        return;
	}
	if ((!in_b) || (((sb = in_b[0]) == 1) && (!in_b[1])))
	{
	        zhalt("division by zero in zmod");
	        return;
	}

	sign = (*in_a < 0 ? 2 : 0) | (*in_b < 0 ? 1 : 0);
	if (*in_a < 0)
	        (*in_a) = (-(*in_a));
	if (*in_b < 0)
	        (*in_b) = (-(*in_b));

	if ((*in_b == 1) && (in_b[1] < ALPHA50RADIX)) {
	        zintoz(zsdiv(in_a, in_b[1], &c), rr);
	        goto done;
	        }

	  zsetlength(&a,(long) (NBITS/(double)ALPHA50NBITS*(double)in_a[0]+1.0)
,
	          "");
	  lower_radix(in_a,a);    /* test this for large integers */

	  zsetlength(&b,(long) (NBITS/(double)ALPHA50NBITS*(double)in_b[0]+1.0)
,
	          "");
	  lower_radix(in_b,b);

	sa = (*a);
	sb = (*b);
	zsetlength(&c, (sa > sb ? sa : sb) + 1, "in zmod, local");
	zsetlength(&r, sb + 1, "in zmod, third argument");
	*rr = r;
	p = &b[sb];

	sq = sa - sb;
	btopinv = (double) (*p) * alpha50fradix;
	if (sb > 1)
	        btopinv += (*(--p));
	btopinv *= alpha50fradix;
	if (sb > 2)
	        btopinv += (*(p - 1));
	btopinv = alpha50fradix / btopinv;
	p = &a[1];
	pc = &c[0];
	for (i = sa; i > 0; i--)
	        *pc++ = *p++;
	p = pc;
	sa = 1 - sb;
	for (i = (-sq); i > 0; i--)
	        *p++ = 0;
	*pc = 0;
	for (i = sq; i >= 0; i--)
	{
	        aux = alpha50fradix * (alpha50fradix * (*pc) +
	         (*(pc - 1))) + 1.0;
	        if (i > sa)
	                aux += (*(pc - 2));
	        qq = (long) (btopinv * aux + 0.5);
	/* see comment in zdiv */
	        if (qq >= ALPHA50RADIX)
	                qq = ALPHA50RADIX-1;
	        if (qq > 0)
	        {
	                zalphasubmul(qq, &c[i], &b[0]);
	                while (*pc < 0)
	                {
	                        {
	                                zalphaaddmulone(&c[i], &b[0]);
	                        }
	                }
	        }
	        pc--;
	}       /* loop on i */
	sb--;
	while ((sb > 0) && (!(c[sb])))
	        sb--;
	sb++;
	raise_radix(sb,c,r);

	if (sign)
	{
	        if ((sign <= 2) && (!((r[0] == 1) && !(r[1]))))
	        {
	                if (sign == 1)
	                        zsub(r, b, &r);
	                else
	                        zsub(b, r, &r);
	        }
	        else
	                znegate(&r);    /* negating r==0 doesn't hurt */
	}
	*rr = r;
done:	;
	FREE2SPACE(a,b); FREESPACE(c);
}


static void
zalphasubmul(
	  long r,
	  verylong a,
	  verylong b
	  )
{
	  register long rd = ALPHA50RADIX - r;
	  register long i;
	  long carry = ALPHA50RADIX;

	  for (i = (*b++); i > 0; i--)
	  {
	          zaldivaddmulp(a, *b, rd, &carry);
	          a++;
	          carry += ALPHA50RADIXM - (*b++);
	  }
	  *a += carry - ALPHA50RADIX;    /* unnormalized */
}




/* alpha zsdiv */

long 
zsdiv(
	verylong in_a,
	long d,
	verylong *bb
	)
{
	register long sa;
	verylong b = *bb;
	STATIC verylong a=0;
	long flag;

#ifndef START
	if (fudge < 0)
	        zstart();
#endif
	if (ALLOCATE && !in_a)
	{
	        zzero(bb);
	        return (0);
	}
	if (!d)
	{
	        zhalt("division by zero in zsdiv");
	        return (0);
	}

	if ((d >= ALPHA50RADIX) || (d <= -ALPHA50RADIX))
	{
	        STATIC verylong zd = 0;
	        STATIC verylong zb = 0;
		long keepzd;

	        zintoz(d, &zb);
	        zdiv(in_a, zb, bb, &zd);
		keepzd = ztoint(zd);
		FREE2SPACE(zd,zb);
	        return (keepzd);
	}

	if (*in_a < 0) {
	        *in_a = - (*in_a);
	        flag = 2L |  (d < 0 ? 1L : 0L);
	        }
	else
	        flag = (d < 0 ? 1L : 0L);

	zsetlength(&a,(long) (NBITS/(double)ALPHA50NBITS*(double) *in_a+1.0), "");
	lower_radix(in_a,a);
	sa = a[0];

	zsetlength(&b, sa, "in zsdiv, third argument");
	*bb = b;
	{
	        register long den = d;
	        register double deninv;
	        register long carry = 0;
	        register long i;

	        if (den < 0)
	                den = -den;
	        deninv = (double)1.0/den;
	        if (a[sa] < den && sa > 1)
	                carry = a[sa--];
	        for (i = sa; i; i--)
	        {
	                zdiv21(carry, a[i], den, deninv, b[i]);
	        }
	        while ((sa > 1) && (!(b[sa])))
	                sa--;
	        b[0] = sa;

	        raise_radix2(sa,b,(*bb));

	        if (flag)
	        {
	                if (flag <= 2)
	                {
	                        if (!carry)
	                                znegate(bb);
	                        else
	                        {
	                                zadd(*bb, one, bb);
	                                *bb[0] = -(*bb[0]);
	                                if (flag == 1)
	                                        carry = carry - den;
	                                else
	                                        carry = den - carry;
	                        }
	                }
	                else
	                        carry = -carry;
	        }
		FREESPACE(a);
	        return (carry);
	}
	FREESPACE(a);
}


#endif

void
zsqrtmod(
	verylong a,
	verylong p,
	verylong *s
	)

{

	STATIC verylong q=0,nn=0,x=0,y=0,t=0,b=0,z=0;
	register long n,i,m,r;

	if ((p[1]&3) == 3) {
		zsadd(p,1, &q);
		zrshift(q,2,&q);
		zexpmod(a,q,p,s);
		zsqmod(*s,p,&y);
		if (zcompare(y,a))
			zzero(s);
		FREE2SPACE(y,q);
		return;
	}

	zsadd(p,-1,&q);
	r= zmakeodd(&q);
	n = 3;
	if ( ((p[1]&3)&n) == 3)
		m = -zjacobis( zsmod(p,n) , n);
	else
		m = zjacobis( zsmod(p,n) , n);

	while (m != -1) {
loop:
		n +=2;
		if (!(n %3))
			n+=2;
		for (i=5; i* i <= n; )  {
			if (!(n %i))
				goto loop;
			i += 2;
			if (!(n %i))
				goto loop;
			i +=4;
		}
		if ( ((p[1]&3)&n) == 3)
			m = -zjacobis( zsmod(p,n) , n);
		else
			m = zjacobis( zsmod(p,n) , n);
	}

	zintoz(n,&nn);

	zexpmod(nn,q,p,&z);

	zcopy(z,&y);
	zsadd(q,-1,&t);
	zrshift(t,1,&t);	/* t = (q-1)/2 */
	zexpmod(a,t,p,&x);
	zsqmod(x,p,&t);
	zmulmod(t,a,p,&b);	/* b = a*x^2 mod p */
	zmulmod(a,x,p,&x);

	while (1) {
		if (b[1]==1 && b[0]==1) {
			zcopy(x,s);
			FREE3SPACE(q,nn,x);
			FREE3SPACE(y,t ,b);
			FREESPACE(z);
			return;
		}

		zsqmod(b,p,&t);
		m=1;
		while (t[1]!=1 || t[0]!=1) {
			if (m==r) {
				zzero(s);
				FREE3SPACE(q,nn,x);
				FREE3SPACE(y,t ,b);
				FREESPACE(z);
				return ;
			}
			zsqmod(t,p,&t);
			m++;
		}

		zcopy(y, &t);
		for (i=r-m-1; i; i--)
			zsqmod(t,p,&t);
		zsqmod(t,p,&y);
		r=m;
		zmulmod(x,t,p,&x);
		zmulmod(b,y,p,&b);
	}
}


#endif


