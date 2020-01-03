#if 0
{
Copyright Arjen K. Lenstra, 1989-1997

			      free  L I P

                 l o n g   i n t e g e r   p a c k a g e

                            Arjen K. Lenstra

                               version 1.1

Introduction
============
This very long int package is supposed to be easy to use, portable, and
not too slow.  It should also not be hard to make it fast by converting
a few macros to your favorite assembly code.

This version has an include file lip.h that defines all functions
and their arguments for ANSI C and contains:

typedef long * verylong;

Older programs need not be changed to use this type.  It is added for
enhancing program readability.

As an example, the following program reads the decimal representation
of two arbitrary length signed integers a and b from stdin, computes
their product in c, and prints c in decimal on stdout, followed by a
newline:

        #include "lip.h"
        main()
        {
        /* declare verylong ints a, b, c */
                verylong a = 0;
                verylong b = 0;
                verylong c = 0;
        /*********************************\
        *   or declare them by
        *       long *a = 0, *b = 0, *c = 0
        \*********************************/
                zread(&a);      /* read a from stdin */
                zread(&b);      /* read b from stdin */
                zmul(a, b, &c); /* multiply a anb b, put result in c */
                zwriteln(c);    /* print c, followed by a newline */
        }

Sample input:

        7419043440758059956596 -60967068778579064460287972

with output:

        -452317331723962514217511611516823866219980863312

(To run this example, first produce lip.o:

	gcc -O -c lip.c

 where gcc is assumed to be your ansi-C compiler, and next

	gcc -O -o example1 example1.c lip.o -lm

 to get the executable example1.)

As you can see, very long ints a, b, and c are declared simply by

        verylong a = 0;
        verylong b = 0;
        verylong c = 0;

If a very long int is input to a function (like a and b in zmul(a, b, &c)),
just give its name as argument. Long ints that get new values in
a function (like a in zread(&a), b in zread(&b), c in zmul(a, b, &c))
are given by their address, which means that they are preceded by a &
in the function call.

As a slightly more challenging example, the following program starts
the random generator with a seed read from stdin, and attempts to
generate and print 5 probable primes of binary lengths 64, 80, 96, 112,
128:

        #include "lip.h"
        main()
        {
                verylong seed = 0;
                long  bl;
                verylong p = 0;

                zread(&seed);   /* get seed from stdin */
                zrstart(seed);  /* start the random generator */
                for (bl = 64; bl <= 128; bl += 16)
                {
                /* find prime of bl bits */
                        if (zrandomprime(bl, 5, &p, zrandomb))
                        {
                        /* refer to description below for the 5 */
                                printf("%3d bits: ", bl);
                                zwriteln(p);
                        }
                        else
                                printf("couldn`t find a %3d bit prime\n", bl);
                }
        }

Sample input:

        742434390077967024401578982882

with output:

	 64 bits: 10513989034112217947
	 80 bits: 775541835531627786320957
	 96 bits: 58185536691780358241291462137
	112 bits: 3464241483388970526627866839605371
	128 bits: 198790427312192931901507582677867621703



WARNING  for old users: some names have been changed.
=======  If zxxx is a function operating on very long ints, then there
         may be two variants of zxxx: zsxxx where one op the operands
         is an ordinary long, and zxxxs where all operands are ordinary
         longs. Compared to the previous version, the following
         functions are affected (with some other name changes as well):

                Old name        New name
                --------        --------
                ztimes2         z2mul
                zdivide2        z2div
                zsmulmod        zmulmods (there`s a new zsmulmod)
                zsinv           zinvs
                zsexp           zexpmods (there`s a new zsexp)
                zexps           zsexp
                zexpsmod        zsexpmod
                zexp2mod        z2expmod
                zssqrt          zsqrts
                zsodinv         zinvodds
                zsjacobi        zjacobis
                zrstart         zrstarts (there`s a new zrstart)
                zmont           zmontmul
                zmexp           zmontexp
                zmexp_m_ary     zmontexp_m_ary


         Some arguments of some functions have changed:
                zmulin          switched order
                zmakeodd        *verylong (**long) instead of verylong (*long)
                znegate         *verylong (**long) instead of verylong (*long)
                zcomposite      *verylong (**long) instead of verylong (*long)


Overview of available functions
===============================

  Basic arithmetic
  ----------------
        zstart, zsadd, zadd, zsub, zsubpos,
        zsmul, zmul, zmulin, zmul_plain, zsq, zsqin, zsq_plain,
        zsdiv, zdiv, zsmod, zmod

  Shifting and bit manipulation
  -----------------------------
        z2mul, z2div, z2mod, zlshift, zrshift, zmakeodd,
        zodd, znot, zand, zor, zxor, zslowbits, zlowbits,
        zshighbits, zhighbits, zweights, zweight, zcat,
	zbit, zgetbits, zsetbit, zswitchbit, zreverses,
	zreverse

  Comparison, signs, copying, logarithms
  --------------------------------------
        zscompare, zcompare, ziszero, zsign, zabs, znegate, zcopy, zswap
        z2logs, z2log, zln, zslog, zlog, zdlog

  Conversion
  ----------
        zzero, zone, zintoz, zuintoz, zultoz, ztoint, ztouint, ztoul,
        sztrtozbas, zstrtoz, zdoub, zsbastoz, zbastoz, zstobas, ztobas,
	zstosymbas, ztosymbas

  Non-modular exponentiation
  --------------------------
        zsexp, zexp, zsqrts, zsqrt, zroot, zispower

  Modular arithmetic
  ------------------
        zaddmod, zsubmod, zmulmods, zsmulmod, zmulmod, zsqmod, zdivmod,
        zinvmod, zexpmods, z2expmod, zsexpmod, zexpmod, zexpmod_m_ary,
        zdefault_m, zexpmod_doub1, zexpmod_doub2, zexpmod_doub3, zexpmod_doub,
        zmulmod26

  Montgomery modular arithmetic
  -----------------------------
        zmstart, zmfree, ztom, zmtoz, zmontadd, zmontsub, zsmontmul, zmontmul,
        zmontsq, zmontdiv, zmontinv, zmontexp, zmontexp_m_ary,
        zmontexp_doub1, zmontexp_doub2, zmontexp_doub3, zmontexp_doub

  Euclidean algorithms
  --------------------
        zgcd, zgcdeucl, zexteucl, zinvs, zinvodds, zinv, zchirem,
        zjacobis, zjacobi

  Random number generation
  ------------------------
        zrstarts, zrstart, zrseed, zrandom, zrandomb, zrandoml,
        zrandomprime, zrandomqprime, zrandomfprime, zrandomgprime

  Small prime generation
  ----------------------
        zpstart, zpstart2, zpnext, zpnextb, zp

  Compositeness testing and factorization
  ---------------------------------------
        zcomposite, zmcomposite, zprime, zprobprime,
        ztridiv, zpollardrho, zecm_trial, zecm, zfecm, zsquf

  Allocation
  ----------
        zsetlength, zfree

  Timing
  ------
        gettime, getutime, getstime, starttime, printtime

  Input and output
  ----------------
          from       to        to      from     to      to       from    to
          file      file      file     stdin  stdout  stdout    string  string
       ------------------------------------------------------------------------
decimal| zfread   zfwrite   zfwriteln  zread  zwrite  zwriteln  zsread  zswrite
hex    | zhfread  zhfwrite  zhfwriteln zhread zhwrite zhwriteln
nits   | zbfread  zbfwrite
anybase| zfread_b zfwrite_b zfwriteln_b


Remarks
=======
- Unless stated otherwise, output can be input, but it`s not advised
  to make output arguments identical (if there are more output
  parameters).
- Very long integers are represented by arrays of longs, in blocks
  of NBITS bits (these blocks will be referred to as "nits").
  A very long int (declared as  verylong a=0 ) either satisfies a==0,
  in which case it is treated as zero, or it satisfies the following:

        - |a[0]| is the significant length, say n, with n>0,
          even for a with value zero.

        - a[1], a[2], ..., a[n] are the nits, most significant nit
          is a[n], with always 0<a[n]<RADIX and 0<=a[i]<RADIX for
          i=1,2,...,n-1; here RADIX is (1<<NBITS).
          Exception: a[n] can be zero if n=1, in which case a has
          value zero.

        - the sign of a[0] is the sign of a.

        - a[-1] gives the amount of space currently allocated
          for a. The functions check this location to see if
          reallocation is needed.

        - the values of a[n+1],...,a[a[-1]] are undefined, and
          cannot be assumed to be zero.

        - except a==0, the only other correct representation
          of an a with value zero is: a[0]==1, a[1]==0. Negative
          zero is not recognized.

  Unless you know what you`re doing, don`t play with any of the a[i].

- Because of the way verylongs are represented, local change (in
  a routine) of a verylong parameter can affect the global value
  of that parameter (and even destroy it entirely). As an example,
  consider the following program:

	#include "lip.h"

	change_verylong(
		verylong verylong_a
		)
	{
		zwriteln(verylong_a);
		zsadd(verylong_a,1,&verylong_a);
		zwriteln(verylong_a);
	}
	
	change_long(
		long long_a
		)
	{
		printf("%ld\n",long_a);
		long_a ++;
		printf("%ld\n",long_a);
	}
	
	main () {
		long a = 5;
		verylong b = 0;
		zintoz(5,&b);
		change_long(a);
		printf("%ld\n",a);
		change_verylong(b);
		zwriteln(b);
	}

  Ouput:
	5
	6
	5
	5
	6
	6

  Although the parameter long_a gets a new value in change_long, this does
  not affect the value of the argument a upon call: it is 5 before and after
  the call. In change_verylong, however, the change made to the parameter
  verylong_a affects the value of the argument b: it is 5 before, but 6
  after the call. More dramatic things might happen too, if for instance
  verylong_a gets reallocated in change_verylong and its original space
  gets freed up....  To avoid these possible side-effects, either make
  verylong_a a *verylong parameter (which should and does keep the local
  changes made to it), or copy verylong_a to a local variable, and work
  with the local variable:

	better_change_verylong(
                verylong verylong_a
                )
        {
		static local_verylong_a = 0;
		zcopy(verylong_a,&local_verylong_a);
                zwriteln(local_verylong_a);
                zsadd(local_verylong_a,1,&local_verylong_a);
                zwriteln(local_verylong_a);
        }


- For those who know what the sizes of input and output
  arguments are going to be, you can allocate variables by hand using
  zsetlength, and make things slightly faster by using the
  -DNO_ALLOCATE flag. Internal local variables will always be allocated
  to the proper length, no matter what flags you use, and output
  variables will also be reallocated if they didn`t get enough
  space, irrespective of the -DNO_ALLOCATE flag.

- To get an indication of what (re)allocations take place,
  you can use the -DPRT_REALLOC flag. The indications will be
  printed on stderr.

- If an error is detected (division by zero, undefined Montgomery
  modulus, undefined results, etc) a message is printed on stderr
  and the program exits. If the -DNO_HALT flag is used, the
  program won`t exit, but values of variables might be undefined.
  In the function descriptions below the possible error messages are
  described. They are supposed to be self-explanatory, if not
  a short explanation follows between (). If the message implies a bug
  in LIP, please report it to arjen.lenstra@citicorp.com as soon as possible.
  There is one message (`wrong call to zsmexp...BUG`) which can only be
  generated by a wrong call to the internal (and undocumented) function
  zsmexp; if you get this message you`re not using the original code.

- If you don`t want to think about -DNO_ALLOCATE or -DPRT_REALLOC
  flags, allocations, or other unpleasant matters, everything
  should work fine, as long as you make sure that you declare
  the very long ints as indicated above (i.e., verylong a=0, b=0, etc...),
  and give them a & in a function call if they are output.
  So, as you can see below, zadd(a, a, &a) adds a to a and puts
  the result in a. On the other hand, zmul(a, b, &a) leads to
  trouble, because input cannot be output in zmul; use
  zmulin(b, &a) instead.

- If you`re writing your own functions with local very long ints,
  then it`s a good idea to declare the very long ints in frequently
  called non-recursive functions as statics:

        static verylong a = 0;
        static verylong b = 0;
        etc...

  instead of

        verylong a = 0;
        verylong b = 0;
        etc...

  If static is used, reallocation of space for the local
  very long ints is avoided in later calls to that same function,
  unless one of the local very long ints needs more space than
  in any of the previous calls: they always keep their longest
  length. If you don`t use static, new space for the local
  very long ints will be allocated for each new call to the
  function, which is less efficient; also, you should use zfree
  in that case at the end of the function, see below.

Acknowledgments
===============
  Acknowledgments are due to many users for reporting bugs,
  to Achim Flammenkamp for initializing the process of writing
  this documentation, and to Bob Cain for converting to ansi.

Before you compile
==================
  You should make sure that some constants get the right value
  for your environment. Here`s a short description of what you
  need. If in doubt, don`t change it, and just try how it works.

  Include Files Needed
  --------------------
        #include <stdio.h>
        #include <math.h>
        #include <malloc.h>
        #include <sys/resource.h>
        #include "lip.h"

  Machine dependent constants 
  --------------------------- 
        Have a look at the constants with <------, and set them
        to the right value for your environment

        #define CHARL           8       <------ Set this to the number of
                                                bits in a byte. Usually it`s 8.

        #define SIZEOFLONG      4       <------ Set this to the number of
                                                bytes in a long, equals
                                                sizeof(long).

        #define NBITS           30      <------ Set this even, and as large as
                                                possible such that
                                                0 < NBITS < CHARL*SIZEOFLONG.

	(Addition: use -DSINGLE_MUL flag to get NBITS=26 and faster macros
			(at least, on most machines); assumes that words
        in doubles are ordered high-low. Use -DDOUBLE_LOW_HIGH if it`s
        the other way around)

        #define RADIX           (1<<NBITS)      Don`t touch this, but it`s
                                                good to know what the radix is.

        #define KAR_MUL_CROV      30              If in a call zmul(a, b, &c) the
        #define KAR_SQU_CROV    30              number of nits of a or b is
        #define KAR_DEPTH        20              less than KAR_MUL_CROV, then a
                                                and b are multiplied using the
                plain quadratic multiplication function; if that`s not the
                case Karatsuba will be applied, recursively, but to a
                maximum of at most KAR_DEPTH recursions. Same for zsq and
                ZKAR_SQU_CROV. The optimal values of these two cross-over
                values depend on the machine you are going to use. The
                choices above are not too far from optimal on a DEC5000;
                on Sparcs the optimal values are somewhat smaller. You
                can make KAR_DEPTH as large as you like, as long as you
                have enough memory.

        #define SIZE            20      <------ Set this to anything such that
                                                SIZE*NBITS>=CHARL*SIZEOFLONG
                SIZE is the default and minimum allocation size for very
                long ints. Any value >= 2 should work. Depending on your
                application smaller or larger values than 20 might lead
                to more efficient code, because it either uses less space
                (for a smaller SIZE), or it uses fewer allocations (for
                a larger SIZE). If you`re not sure what SIZE to pick,
                compile the package and your program with the -DPRT_REALLOC
                flag, and run a representative example: the output will give
                you an impression of the actual sizes that will be used, and
                SIZE can be set accordingly before you compile again without
                the -DPRT_REALLOC flag. If you don`t change it, it should work
                fine.

        #define OUT_LINE        68      <------ An approximate bound for
                                                the maximal number of digits
                per line of output. You might want to change this to 40
                if you have an unusually narrow screen, or to 132 if you`re
                still using one of these nice old lineprinters.
                
        #define IN_LINE         2048            Input accepts at most
                                                IN_LINE characters per line.
                This should not be too restrictive, because long lines
                can easily be split into smaller lines, see below.

        #define PRIM_BND         16500            This enables you to
                                                generate the primes
                less than (2*PRIM_BND+1)^2 using zpnext, see below.
                For 16500 the last prime that can thus be generated is
                1089065981.
}
#endif



#include <stdio.h>
//#include <math.h>
#include <sys/types.h>
#ifndef WIN32
#include <netinet/in.h>
#endif
#include "lippar.h"

/*The type of very long ints.*/
typedef long * verylong;

#ifdef FREE
#define STATIC
#define FREESPACE(x)    zfree(&x);
#define FREE2SPACE(x,y) zfree(&x); zfree(&y);
#define FREE3SPACE(x,y,z) zfree(&x); zfree(&y); zfree(&z);
#else
#define STATIC          static
#define FREESPACE(x)
#define FREE2SPACE(x,y) 
#define FREE3SPACE(x,y,z)
#endif

#define ILLEGAL 0


#ifdef NO_ALLOCATE
# define ALLOCATE 0
#else
# define ALLOCATE 1
#endif

#ifdef PRT_REALLOC
# undef PRT_REALLOC
# define PRT_REALLOC 1
#else
# define PRT_REALLOC 0
#endif

#ifndef CHARL
# define CHARL           8      /* 8 bits per char */
#endif

#ifndef SIZEOFLONG
# define SIZEOFLONG      4      /* set this to sizeof(long) */
#endif

#define BITSOFLONG      (CHARL*SIZEOFLONG)

#ifdef ALPHA
# ifdef ALPHA50
#  undef ILLEGAL
#  define ILLEGAL	1
# endif
# ifndef PLAIN
#  undef KARAT
#  define KARAT  1
# endif
# ifdef SINGLE_MUL
#  undef ILLEGAL
#  define ILLEGAL	1
# endif
# define NBITS          62
# undef BITSOFLONG
# define BITSOFLONG     64
# undef  SIZEOFLONG
# define SIZEOFLONG      8
# define PRIM_BND      (1L<<14)
# define ALPHA_OR_ALPHA50       1
#endif

#ifdef ALPHA50
# ifndef PLAIN
#  undef KARAT
#  define KARAT  1
# endif
# ifdef SINGLE_MUL
#  undef ILLEGAL
#  define ILLEGAL	1
# endif
# define NBITS          62
# undef BITSOFLONG
# define BITSOFLONG     64
# undef  SIZEOFLONG
# define SIZEOFLONG      8
# define PRIM_BND      (1L<<14)
# define ALPHA50NBITS   50
# define ALPHA50NBITSH  (ALPHA50NBITS>>1)
# define ALPHA50RADIX   (1L<<ALPHA50NBITS)
# define        ALPHA50RADIXM   (ALPHA50RADIX-1)
# define        ALPHA50RADIXROOT        (1L<< ALPHA50NBITSH)
# define ALPHA50RADIXROOTM      (ALPHA50RADIXROOT-1)
# define ALPHA_OR_ALPHA50       1
#endif


#ifdef PLAIN
# define PLAIN_OR_KARAT  1
#endif

#ifdef KARAT
# define PLAIN_OR_KARAT  1
#endif


#ifndef NBITS
# ifdef SINGLE_MUL
#  define NBITS        26
# else
#  define NBITS         30
# endif
#endif

#define NBITSH          (NBITS>>1)
#define RADIX           (1L<<NBITS)
#define RADIXM          (RADIX-1)
#define RADIXROOT       (1L<<NBITSH)
#define RADIXROOTM      (RADIXROOT-1)

#ifndef SIZE
# define SIZE           20      /* SIZE*NBITS must be >= BITSOFLONG */
#endif


#ifndef OUT_LINE
# define OUT_LINE       68      /* approximate bound # digits per line */
#endif

#define OUT_LINE_BREAK  '\\'

#ifndef IN_LINE
# define IN_LINE      2048      /* at most 2048 characters per line */
#endif

#define IN_LINE_BREAK   '\\'

#ifndef HEX_BLOCK
# define HEX_BLOCK	8
#endif

#ifndef HEX_BLOCKS_PER_LINE
# define HEX_BLOCKS_PER_LINE	7
#endif

#define HEX_SEP_CHAR	' '

#ifndef PRIM_BND
# ifdef SINGLE_MUL
#  define PRIM_BND      (1L<<14)
# else
#  define PRIM_BND      (1L<<(NBITSH-1))
# endif
                        /* to generate primes <= (2*PRIM_BND+1)^2 */
                        /* last prime for NBITS == 30 is 1073807359 */
			/* same for SINGLE_MUL */
#elif (PRIM_BND>(1L<<(NBITSH-1)))
# undef PRIM_BND
# define PRIM_BND       (1L<<(NBITSH-1))
#endif

#define PRIM_UP         ((((PRIM_BND<<1)+1)*((PRIM_BND<<1)+1))-(NBITS<<2))

#if (NBITS&1)
# undef ILLEGAL
# define ILLEGAL 1
#endif

#if (NBITS <= 0)
# undef ILLEGAL
# define ILLEGAL 1
#endif

#if (NBITS >= BITSOFLONG)
# undef ILLEGAL
# define ILLEGAL 1
#endif

#if (SIZE*NBITS<=BITSOFLONG)
# undef ILLEGAL
# define ILLEGAL 1
#endif

#ifdef SINGLE_MUL
# if (NBITS != 26)
#  undef ILLEGAL
#  define ILLEGAL 1
# endif
# ifdef PLAIN
#  undef ILLEGAL
#  define ILLEGAL 1
# endif
# ifdef KARAT
#  undef ILLEGAL
#  define ILLEGAL 1
# endif
#endif

#ifdef PLAIN
# ifdef KARAT
#  undef ILLEGAL
#  define ILLEGAL 1
# endif
#endif

/******************************************************************************\
*   Internal macros
*
*   Although the package is supposed to be portable, you might want to
*   fine tune it to your particular machine to get better performance.
*   The easiest way to do this is to replace the following macros, which
*   you will find in the source code, by appropriate assembly language
*   versions:
*
*   Also the C-code for zsubmul, zdiv21 and zmulmods can be made
*   much faster in a similar way. I always simply used the macros
*   or the C-code, and I get acceptable performance.
\******************************************************************************/

    static void zaddmulp(long *a, long b, long d, long *t);
        /******************************************************************\
        * a = (a + t + b * d) % RADIX; and simultaneously
        * t = (a + t + b * d) / RADIX;
        \******************************************************************/

    static void zaddmulpsq(long *a, long b, long *t);
        /******************************************************************\
        * a = (a + b * b) % RADIX; and simultaneously
        * t = (a + b * b) / RADIX;
        \******************************************************************/

    static void zaddmulone(verylong a, verylong b);
        /******************************************************************\
        * a += b;
        * a and b not at overlapping addresses
        \******************************************************************/

    static void zaddmul(long d, verylong a, verylong b);
        /******************************************************************\
        * a += d * b;
        * a and b not at overlapping addresses (except if d=1)
        \******************************************************************/

    static void zaddmulsq(long d, verylong a, verylong b);
        /******************************************************************\
        * a += b[0] * b[1:d];
        * a and b not at overlapping addresses (except if d=1)
        \******************************************************************/

    static void zmmulp(verylong a);
        /******************************************************************\
        * a[s:] += (a[s] * zminv) * zn;
        * to make a[s] == 0, where s and zminv are clear from
        * the context, only in Montgomery multiplication
        \******************************************************************/


/******************************************************************************\
*  Basic functions
*  
*  Addition, subtraction, multiplication, squaring, and
*  division with remainder on signed arbitrary length integers.
*  Multiplication and squaring use Karatsuba, if inputs large enough;
*  see KAR_MUL_CROV, KAR_SQU_CROV, and KAR_DEPTH as explained above.
\******************************************************************************/

    void zstart(void);
        /******************************************************************\
        * To initialize some global machine dependent values
        * that have to be computed only once, call this only once per run.
        * Everything still works fine if you forget to call zstart.
        * If you`re sure that you`re not going to forget it, you may
        * compile the package with the -DSTART flag and get slightly
        * faster code.
        *
        * possible error message:
        *   recompile with smaller NBITS
        * result undefined if error occurs
        \******************************************************************/

    void zsadd(verylong a, long d, verylong *b);
        /******************************************************************\
        * *b = a + d;
        * 
        \******************************************************************/

    void zadd(verylong a, verylong b, verylong *c);
        /******************************************************************\
        * *c = a + b;
        * 
        \******************************************************************/

    void zsub(verylong a, verylong b, verylong *c);
        /******************************************************************\
        * *c = a - b;
        * 
        \******************************************************************/

    void zsubpos(verylong a, verylong b, verylong *c);
        /******************************************************************\
        * *c = a - b;
        * 
        * only for a >= b >= 0
        \******************************************************************/

    void zsmul(verylong a, long d, verylong *b);
        /******************************************************************\
        * *b = d * a;
        * 
        \******************************************************************/

    void zmul(verylong a, verylong b, verylong *c);
        /******************************************************************\
        * *c = a * b;
        * 
        * output cannot be input
        \******************************************************************/

    void zmulin(verylong a, verylong *b);
        /******************************************************************\
        * *b = a * b;
        * 
        * output cannot be input
        \******************************************************************/

    void zmul_plain(verylong a, verylong b, verylong *c);
        /******************************************************************\
        * *c = a * b;
        * 
        * output cannot be input, uses ordinary multiplication
        \******************************************************************/

    void zsq(verylong a, verylong *c);
        /******************************************************************\
        * *c = a * a;
        * 
        * output cannot be input
        \******************************************************************/

    void zsqin(verylong *a);
        /******************************************************************\
        * *a = a ^ 2;
        * 
        \******************************************************************/

    void zsq_plain(verylong a, verylong *b);
        /******************************************************************\
        * *b = a ^ 2;
        * 
        * output cannot be input, uses ordinary squaring
        \******************************************************************/

    long zsdiv(verylong a, long d, verylong *b);
        /******************************************************************\
        * *b = a / d;
        * return (a % d);
        * 
        * d != 0,
        * always b * q + (a % d) == a and,
        * unless b divides a, sign(a % d) == sign(d),
        * calls zdiv if |d| >= RADIX
        *
        * possible error message:
        *   division by zero in zsdiv
        * result undefined if error occurs
        \******************************************************************/

    void zdiv(verylong a, verylong b, verylong *q, verylong *r);
        /******************************************************************\
        * *q = a / b;
        * *r = a % b;
        * 
        * b != 0,
        * always b * q + r == a,
        * unless b divides a, sign(r) == sign(b)
        *
        * possible error message:
        *   division by zero in zdiv
        * result undefined if error occurs
        \******************************************************************/

    long zsmod(verylong a, long d);
        /******************************************************************\
        * return (a % d);
        * 
        * calls zsdiv
        \******************************************************************/

    void zmod(verylong a, verylong b, verylong *r);
        /******************************************************************\
        * *r = a % b;
        * 
        * unless b divides a, sign(r) == sign(b),
        * slightly faster than zdiv
        *
        * possible error message:
        *   division by zero in zmod
        * result undefined if error occurs
        \******************************************************************/


/******************************************************************************\
*  Shifting and bit manipulation
*
*  Left and right shifting, removal of all factors 2,
*  parity test, logical and/(x)or, bit selections, weight,
*  concatenation, bit-reverse
*
*  WARNING: The bit manipulation routines need to be debugged carefully
\******************************************************************************/

    void z2mul(verylong n, verylong *a);
        /******************************************************************\
        * *a = 2 * n;
        * 
        \******************************************************************/

    long z2div(verylong n, verylong *a);
        /******************************************************************\
        * *a = n / 2; return (n % 2);
        * 
        * warning: for n == -2 * k + 1 (k > 0), z2div(n, &a)
        * gives a = -k + 1, but zsdiv(a, 2, &b) gives b = -k,
        * both return 1
        \******************************************************************/

    long z2mod(verylong n);
        /******************************************************************\
        * return (n % 2);
        \******************************************************************/

    void zlshift(verylong n, long k, verylong *a);
        /******************************************************************\
        * *a = 2 ^ k * n;
        * 
        * i.e., shifts left over k positions,
        * calls zrshift(n, -k, a) if k < 0
        \******************************************************************/

    void zrshift(verylong n, long k, verylong *a);
        /******************************************************************\
        * *a = n / (2 ^ k);
        * 
        * i.e., shifts right over k positions,
        * calls zlshift(n, -k, a) if k<0
        \******************************************************************/
    
    long zmakeodd(verylong *n);
        /******************************************************************\
        * if (n != 0)
        *     *n = m;
        *     return (k such that n == 2 ^ k * m with m odd);
        * else
        *     return (-1);
        \******************************************************************/

    long zodd(verylong a);
        /******************************************************************\
        * returns 1 if a is odd, returns 0 if a is even
        \******************************************************************/

    void znot(verylong a, verylong *b);
        /******************************************************************\
        * if (a==0) then b gets 1,
        * else b gets the negated bit pattern of the first z2log(|a|)
        * bits of a, and b gets the same sign as a (unless b is zero)
        \******************************************************************/

    void zand(verylong a, verylong b, verylong *c);
        /******************************************************************\
        * c gets bit pattern `bits of |a|` and `bits of |b|`
        \******************************************************************/

    void zor(verylong a, verylong b, verylong *c);
        /******************************************************************\
        * c gets bit pattern `bits of |a|` inclusive or `bits of |b|`
        \******************************************************************/

    void zxor(verylong a, verylong b, verylong *c);
        /******************************************************************\
        * c gets bit pattern `bits of |a|` exclusive or `bits of |b|`
        \******************************************************************/

    long zslowbits(verylong a, long b);
        /******************************************************************\
	* returns b (or NBITS if b>NBITS) lowest order bits of |a|,
        * 0 if b <= 0
        \******************************************************************/

    void zlowbits(verylong a, long b, verylong *c);
        /******************************************************************\
	* c gets b lowest order bits of |a|, c gets 0 if b <= 0
        \******************************************************************/


    long zshighbits(verylong a, long b);
        /******************************************************************\
        * returns b (or NBITS if b>NBITS) highest order bits of |a|,
        * 0 if b <= 0
        \******************************************************************/

    void zhighbits(verylong a, long b, verylong *c);
        /******************************************************************\
        * c gets b highest order bits of |a|, c gets 0 if b <= 0
        \******************************************************************/

    long zweights(long a);
        /******************************************************************\
        * returns the number of one bits in |a|
        \******************************************************************/

    long zweight(verylong a);
        /******************************************************************\
        * returns the number of one bits in |a|
        \******************************************************************/

    void zcat(verylong a, verylong b, verylong *c);
        /******************************************************************\
        * c gets the concatenation of bits of a followed by the bits of b
        * (a becomes the high order and b the low order part of c)
        \******************************************************************/

    long zbit(verylong a, long p);
        /******************************************************************\
        * returns 1 if |p|th bit of |a| is on, 0 if |p|th bit of |a| is off
        * p starts counting at 0
        \******************************************************************/

    void zgetbits(verylong a, long b, long p, verylong *c);
        /******************************************************************\
        * c gets the b bits of |a| starting at |a|`s |p|th bit
        * c gets 0 if b <= 0
        * p starts counting at 0
        \******************************************************************/

    long zsetbit(verylong *a, long p);
        /******************************************************************\
        * returns original value of |p|th bit of |a|, and replaces
        * |p|th bit of a by 1 if it was zero
        * p starts counting at 0
        \******************************************************************/

    long zswitchbit(verylong *a, long p);
        /******************************************************************\
        * returns original value of |p|th bit of |a|, and switches
        * the value of |p|th bit of a
        * p starts counting at 0
        \******************************************************************/

    long zreverses(long aa);
        /******************************************************************\
        * returns the bit reverse of |a|
        * i.e., zreverses(11001010) is 1010011
        \******************************************************************/

    void zreverse(verylong a, verylong *b);
        /******************************************************************\
        * b gets the bit reverse of |a|
	* i.e., zintoz(11,&a); zlshift(a,100,&a); zreverse(a,&b);
        *       produces b with value 13
        \******************************************************************/


/******************************************************************************\
*  Comparison, signs, copying, logarithms
\******************************************************************************/

    long zscompare(verylong a, long b);
        /******************************************************************\
        * if (a > b)
        *     return (1);
        * if (a == b)
        *     return (0);
        * if (a < b)
        *     return (-1);
        \******************************************************************/

    long zcompare(verylong a, verylong b);
        /******************************************************************\
        * if (a > b)
        *     return (1);
        * if (a == b)
        *     return (0);
        * if (a < b)
        *     return (-1);
        \******************************************************************/

    long ziszero (verylong a);
        /******************************************************************\
        * if (a == 0)
        *     return (1);
        * if (a != 0)
        *     return (0);
        \******************************************************************/


    long zsign(verylong a);
        /******************************************************************\
        * if (a > 0)
        *     return (1);
        * if (a == 0)
        *     return (0);
        * if (a < 0)
        *     return (-1);
        \******************************************************************/

    void zabs(verylong *a);
        /******************************************************************\
        * *a = abs(a);
        * 
        \******************************************************************/

    void znegate(verylong *a);
        /******************************************************************\
        * *a = -a;
        * 
        * has no effect on zero a
        \******************************************************************/

    void zcopy(verylong a, verylong *b);
        /******************************************************************\
        * *b = a;
        \******************************************************************/

    void zswap(verylong *a, verylong *b);
        /******************************************************************\
        * exchange values of *a and *b
        \******************************************************************/

    long z2logs(long a);
        /******************************************************************\
        * return (1 + log2(abs(a));
        * 
        * the number of binary bits to represent a,
        * returns 0 if a = 0
        \******************************************************************/

    long z2log(verylong a);
        /******************************************************************\
        * return (1 + log2(abs(a));
        * 
        * the number of binary bits to represent a,
        * returns 0 if a = 0
        \******************************************************************/


    double zln(verylong a);
        /******************************************************************\
        * return (approximation to the natural logarithm of |a|);
        * 
        * only for a != 0
        *
        * possible error message:
        *   non-positive argument in zln
        * result undefined if error occurs
        \******************************************************************/

    double zslog(verylong a, long b);
        /******************************************************************\
        * return (approximation of the logarithm of |a|, base b);
        * 
        * only for a != 0, b >= 2
        \******************************************************************/

    double zlog(verylong a, verylong b);
        /******************************************************************\
        * return (approximation of the logarithm of |a|, base b);
        * 
        * only for a != 0, b >= 2
        \******************************************************************/

    double zdlog(verylong a, double b);
        /******************************************************************\
        * return (approximation of the logarithm of |a|, base b);
        * 
        * only for a != 0, b > 1
        \******************************************************************/


/******************************************************************************\
*  Conversion
*
*  Conversion between very long ints and ordinary longs or doubles.
*  Conversion for very long ints to arbitrary basis and back.
\******************************************************************************/
        
    void zzero(verylong *a);
        /******************************************************************\
        * *a = 0;
        * 
        * allocated zero
        \******************************************************************/

    void zone(verylong *a);
        /******************************************************************\
        * *a = 1;
        * 
        \******************************************************************/

    void zintoz(long d, verylong *a);
        /******************************************************************\
        * *a = d;
        * 
        \******************************************************************/

    void zuintoz(unsigned long d, verylong *a);
        /******************************************************************\
        * *a = d;
        * 
        \******************************************************************/

    void zultoz(unsigned long row[], long len, verylong *a);
        /******************************************************************\
        * *a = sum row[i]X^i, sum from i=0 to len-1, where
        * X = 2^{CHARL*SIZEOFLONG}
        * to convert base 2^32 (or 2^64) representation to verylong 
        * 
        \******************************************************************/

    long ztoint(verylong a);
        /******************************************************************\
        * return (attempted conversion of a`s value to long);
        * no overflow check
        * 
        \******************************************************************/

    unsigned long ztouint(verylong a);
        /******************************************************************\
        * return (attempted conversion of |a|`s value to unsigned long);
        * no overflow check
        * 
        \******************************************************************/

    long ztoul(verylong a, unsigned long row[], long *len);
        /******************************************************************\
        * sets *len to m and row[i] such that
        * |a| = sum from i = 0 to i = m - 1 of row[i] * X ^ i,
        * with 0 <= row[i] < X = 2^{CHARL*SIZEOFLONG}
        * returns 1 if m <= *len on input (which should on input be
        * the length of row[]) returns 0 if *len on input is too small to
        * represent |a| in *len blocks
        \******************************************************************/

    long zstrtozbas(char *s, long b, verylong *a);
        /******************************************************************\
        * a gets the value of s whose characters are interpreted as base |b|
        * integers, for 0<|b|<=16. First character of s may be a `-` or a `_`
        * to indicate negative number. Reads until it finds a character that
        * cannot be correctly interpreted as a base |b| digit. Returns the
        * number of characters read (excluding the sign), returns 0 for
        * b zero, or |b| > 16.
        * 
        \******************************************************************/

    long zstrtoz(char *s, verylong *a);
        /******************************************************************\
        * as zstrtozbas with b = 10
        \******************************************************************/

    double zdoub(verylong n);
        /******************************************************************\
        * return (attempted conversion of a`s value to double);
        * 
        * no overflow check
        \******************************************************************/

    void zsbastoz(long base, long row[], long len, verylong *n);
        /******************************************************************\
        * *n = sum from i = 0 to i = len - 1 of row[i] * base ^ i
        * 
        \******************************************************************/

    void zbastoz(verylong base, verylong row[], long len, verylong *n);
        /******************************************************************\
        * *n = sum from i = 0 to i = len - 1 of row[i] * base ^ i
        * 
        \******************************************************************/

    long zstobas(verylong n, long base, long row[], long *len);
        /******************************************************************\
        * sets *len to m and row[i] such that |n| equals
        * sum from i = 0 to i = m - 1 of row[i] * |base| ^ i,
        * with 0 <= row[i] < |base|
        * returns 1 if m <= *len on input
        * (which should on input be the length of row[])
        * returns 0 if *len on input is too small to represent
        * n in *len blocks or if |base| <= 1
        \******************************************************************/

    long ztobas(verylong n, verylong base, verylong row[], long *len);
        /******************************************************************\
        * same as zstobas, but with a very long int base
        * 
        \******************************************************************/

    long zstosymbas(verylong n, long base, long row[], long *len);
        /******************************************************************\
        * same as zstobas, except that |row[i]| <= |base| / 2
        * 
        \******************************************************************/

    long ztosymbas(verylong n, verylong base, verylong row[], long *len);
        /******************************************************************\
        * same as zstosymbas, but with a very long int base
        * 
        \******************************************************************/


/******************************************************************************\
*  Non-modular exponentiation 
*
*  Exponentiation, squareroots  and arbitrary roots.
\******************************************************************************/

    void zsexp(verylong a, long e, verylong *b);
        /******************************************************************\
        * *b = a ^ e;
        * 
        * arguments cannot be the same,
        * e must be >= 0 if abs(a) != 1
        \******************************************************************/

    void zexp(verylong a, verylong e, verylong *b);
        /******************************************************************\
        * *b = a ^ e;
        * 
        * arguments cannot be the same,
        * e must be >= 0 if abs(a) != 1,
        * better be sure that you know what you`re 
        * doing before you call this
        *
        * possible error message:
        *   negative exponent in zexp
        * result undefined if error occurs
        \******************************************************************/

    long zsqrts(long n);
        /******************************************************************\
        * if (n > 0)
        *     return ([sqrt(n)]);
        * else
        *     return (0);
        \******************************************************************/

    long zsqrt(verylong n, verylong *r, verylong *dif);
        /******************************************************************\
        * *r = [sqrt(n)];
        * *dif = n - r ^ 2;
        * if (n perfect square)
        *     return (1)
        * else
        *     return (0)
        * 
        * n must be >= 0
        *
        * possible error message:
        *   negative argument in zsqrt
        * result undefined if error occurs
        \******************************************************************/

    long zroot(verylong a, long n, verylong *b);
        /******************************************************************\
        * *b = nth root of a;
        * 
	* if (n perfect dth root)
	*	return 1
	* else if (no dth root)
	*	return -1
	* else
	*	return 0
	*
        * a must be >= 0 if n even, n >= 0 if a = 0,
        *
        * possible error message:
        *   dth root with d=0 in zroot
        *   dth root with even d of negative number in zroot
        *   dth root with d<0 of zero in zroot
        * result undefined if error occurs
        \******************************************************************/


long zispower( verylong a, verylong *f);
        /******************************************************************\
	* if (a== x^k)
	*	*f = x, return k
	* else
	*	return 0
	*
        * assumes a>1
        * 
        \******************************************************************/


/******************************************************************************\
*  Modular arithmetic 
*  
*  Addition, subtraction, multiplication, squaring division, inversion,
*  and exponentiation modulo a positive modulus n, where all operands
*  (except for the exponent in exponentiation) and results are in the
*  range [0, n-1]. For heavy computations modulo a fixed modulus it
*  might be better to use Montgomery arithmetic.
\******************************************************************************/

    void zaddmod(verylong a, verylong b, verylong n, verylong *c);
        /******************************************************************\
        * *c = (a + b) % n;
        * 
        * with 0 <= a,b,c < n (and n positive)
        *
        * possible error message:
        *   modulus zero in zaddmod
        * result undefined if error occurs
        \******************************************************************/

    void zsubmod(verylong a, verylong b, verylong n, verylong *c);
        /******************************************************************\
        * *c = (a - b) % n;
        * 
        * with 0 <= a,b,c < n (and n positive)
        *
        * possible error message:
        *   modulus zero in zsubmod
        * result undefined if error occurs
        \******************************************************************/

    long zmulmods(long a, long b, long n);
        /******************************************************************\
        * return ((a * b) % n);
        \******************************************************************/

    long zmulmod26(long a, long b, long n, double bninv);
        /******************************************************************\
        * return ((a * b) % n);
        * where bninv = (double)b/(double)n
	* only for 0<=a,b,n<2^26
        \******************************************************************/

    void zsmulmod(verylong a, long b, verylong n, verylong *c);
        /******************************************************************\
        * *c = (a * b) % n;
        * 
        * with 0 <= a,b,c < n (and n positive)
        *
        * possible error message:
        *   modulus zero in zsmulmod
        * result undefined if error occurs
        \******************************************************************/

    void zmulmod(verylong a, verylong b, verylong n, verylong *c);
        /******************************************************************\
        * *c = (a * b) % n;
        * 
        * with 0 <= a,b,c < n (and n positive)
        *
        * possible error message:
        *   modulus zero in zmulmod
        * result undefined if error occurs
        \******************************************************************/

    void zmulinmod(verylong a, verylong *b, verylong n);
        /******************************************************************\
        * *b = (a * b) % n;
        * 
        * with 0 <= a,b < n (and n positive)
        *
        * possible error message:
        *   modulus zero in zmulinmod
        * result undefined if error occurs
        \******************************************************************/

    void zsqmod(verylong a, verylong n, verylong *c);
        /******************************************************************\
        * *c = (a ^ 2) % n;
        * 
        * with 0 <= a,c < n (and n positive)
        *
        * possible error message:
        *   modulus zero in zsqmod
        * result undefined if error occurs
        \******************************************************************/

    void zsqrtmod(verylong a, verylong p, verylong *s);
        /******************************************************************\
        * computes x so that x^2 == a mod p for prime p, and puts x in *s.
	* if no such x exists or if p is not prime, then sets *s=0.
        \******************************************************************/


    void zsqinmod(verylong *a, verylong n);
        /******************************************************************\
        * *a = (a ^ 2) % n;
        * 
        * with 0 <= a < n (and n positive)
        *
        * possible error message:
        *   modulus zero in zsqinmod
        * result undefined if error occurs
        \******************************************************************/

    void zdivmod(verylong a, verylong b, verylong n, verylong *c);
        /******************************************************************\
        * *c = (a / b) % n;
        * 
        * with 0 <= a,b,c < n (and n positive),
        *
        * possible error message:
        *   modulus zero in zdivmod
        *   division by zero in zdivmod
        *   undefined quotient in zdivmod
        * result undefined if error occurs, except if the quotient
        * is undefined, in which case a factor of n will be returned in c
        * (of course, only if the -DNO_HALT flag is used)
        \******************************************************************/

    void zinvmod(verylong a, verylong n, verylong *c);
        /******************************************************************\
        * *c = (1 / a) % n;
        * 
        * with 0 <= a,c < n (and n positive),
        *
        * possible error message:
        *   modulus zero in zinvmod
        *   division by zero in zinvmod
        *   undefined inverse in zinvmod
        * result undefined if error occurs, except if the inverse
        * is undefined, in which case a factor of n will be returned in c
        * (of course, only if the -DNO_HALT flag is used)
        \******************************************************************/

    long zexpmods(long a, long e, long n);
        /******************************************************************\
        * return ((a ^ |e|) % n);
        * 
        \******************************************************************/

    void zsexpmod(verylong a, long e, verylong n, verylong *b);
        /******************************************************************\
        * *b = (a ^ e) % n;
        * 
        * arguments cannot be the same (but a and b can be the same)
        * (a^(-e)) and n coprime if e negative
        \******************************************************************/

    void z2expmod(verylong e, verylong n, verylong *b);
        /******************************************************************\
        * *b = (2 ^ e) % n;
        * 
        * arguments cannot be the same,
        * (2^(-e)) and n coprime if e negative
        *
        * possible error message:
        *   modulus zero in z2expmod
        *   undefined quotient in z2expmod      (caused by negative exponent)
        * result undefined if error occurs, except if the quotient
        * is undefined, in which case a factor of n will be returned in b
        * (of course, only if the -DNO_HALT flag is used)
        \******************************************************************/

    void zexpmod(verylong a, verylong e, verylong n, verylong *b);
        /******************************************************************\
        * *b = (a ^ e) % n;
        * 
        * a and b can be the same, but both unequal
        * to e and n, (a^(-e)) and n coprime if e negative
        *
        * possible error message:
        *   modulus zero in zexpmod
        *   undefined quotient in zexpmod       (caused by negative exponent)
        * result undefined if error occurs, except if the quotient
        * is undefined, in which case a factor of n will be returned in b
        * (of course, only if the -DNO_HALT flag is used)
        \******************************************************************/

    void zexpmod_m_ary(verylong a, verylong e, verylong n, verylong *b, long m);
        /******************************************************************\
        * b = (a ^ e) % n;
        * 
        * using m-ary method which is faster than
        * zexpmod if a is not small, a and b can be the same, but both
        * unequal to e and n, (a^(-e)) and n coprime if e negative,
        * if m <= 1, default m will be used for m-ary method, if
        * m >= NBITS, NBITS-1 will be used
        *
        * possible error message:
        *   modulus zero in zexpmod_m_ary
        *   undefined quotient in zexpmod_m_ary    (caused by negative exponent)
        * result undefined if error occurs, except if the quotient
        * is undefined, in which case a factor of n will be returned in b
        * (of course, only if the -DNO_HALT flag is used)
        \******************************************************************/

    long zdefault_m(long l);
        /******************************************************************\
        * return (default m for m-ary exponentiation);
        \******************************************************************/

    void zexpmod_doub1(verylong x1, verylong e1, verylong x2, verylong e2,
                       verylong n, verylong *b);
        /******************************************************************\
        * b = ( x1 ^ e1) * (x2 ^ e2) % n;
        *
        * uses Shamir`s method, i.e., does the exponentiations simultaneously
        * after precomputation of (x1*x2)%n, only for e1 and e2 non-negative,
        * use if e1 and e2 approximately same size and small
        *
        * possible error message:
        *  modulus zero in zexpmod_doub1
        *  negative exponent in zexpmod_doub1     (if e1 or e2 negative)
        * result undefined if error occurs
        \******************************************************************/

    void zexpmod_doub2(verylong x1, verylong e1, verylong x2, verylong e2,
                       verylong n, verylong *b);
        /******************************************************************\
        * b = ( x1 ^ e1) * (x2 ^ e2) % n;
        *
        * uses Shamir`s method with sliding window of size 2 on exponents
        * after precomputation of table of products (xi^i1)*(x2^i2)%n with
        * 0<=i1,i2<=3 and i1 and i2 not both even, only for e1 and e2
        * non-negative, use if e1 and e2 approximately same size and up to
        * approximately 270 bits
        *
        * possible error message:
        *  modulus zero in zexpmod_doub2
        *  negative exponent in zexpmod_doub2     (if e1 or e2 negative)
        * result undefined if error occurs
        \******************************************************************/

    void zexpmod_doub3(verylong x1, verylong e1, verylong x2, verylong e2,
                       verylong n, verylong *b);
        /******************************************************************\
        * b = ( x1 ^ e1) * (x2 ^ e2) % n;
        *
        * uses Shamir`s method with sliding window of size 3 on exponents
        * after precomputation of table of products (xi^i1)*(x2^i2)%n with
        * 0<=i1,i2<=7 and i1 and i2 not both even, only for e1 and e2
        * non-negative, use if e1 and e2 approximately same size and 
        * larger than approximately 270 bits
        *
        * possible error message:
        *  modulus zero in zexpmod_doub3
        *  negative exponent in zexpmod_doub3     (if e1 or e2 negative)
        * result undefined if error occurs
        \******************************************************************/

    void zexpmod_doub(verylong x1, verylong e1, verylong x2, verylong e2,
                       verylong n, verylong *b);
        /******************************************************************\
        * b = ( x1 ^ e1) * (x2 ^ e2) % n;
        *
        * uses Shamir`s method with sliding window of size depending on
        * maximal size of e1 or e2, using zexpmod_doub1, zexpmod_doub2,
        * or zexpmod_doub3, only for e1 and e2 non-negative, use if e1
        * and e2 approximately same size
        *
        * possible error message:
        *  modulus zero in zexpmod_doub
        *  negative exponent in zexpmod_doub     (if e1 or e2 negative)
        * result undefined if error occurs
        \******************************************************************/


/******************************************************************************\
*  Montgomery modular arithmetic
*
*  Modular multiplications can be done division free and therefore
*  somewhat faster (about 20%), if the Montgomery representation is
*  used. Converting to and from Montgomery representation takes one
*  Montgomery multiplication each, so it only pays to use Montgomery
*  representation if many multiplications have to be carried out
*  modulo a fixed odd modulus.
*
*  To use Montgomery arithmetic, first initialize the modulus zn (use
*  zmstart), and convert all operands to their Montgomery representation
*  (ztom), but do Not convert exponents. Use the addition, subtraction,
*  multiplication, squaring, division, inversion, and exponentiation
*  functions (zmontxxx) below on the converted operands, just as you
*  would use the ordinary modular functions (zxxxmod). The results can
*  be converted back from Montgomery representation to ordinary numbers
*  modulo zn using zmtoz. zmfree makes current Montgomery modulus undefined;
*  this has no effect except that some routines that use Montgomery
*  arithmetic (with a possibly different modulus) get slightly faster.
*
*  Once you have figured out how this works, you might want to try
*  `mixed arithmetic` by using ordinary modular functions for operations
*  on (non-converted) small constants and Montgomery numbers; zsmontmul
*  might be helpful. See the source code of zmcomposite for an example.
*
*  For how it works, see P.L. Montgomery, Modular multiplication without
*  trial division, Math. Comp, 44 (1985), 519--521.
\******************************************************************************/

    void zmstart(verylong n);
        /******************************************************************\
        * zn = n;
        * 
        * initializes the Montgomery modulus zn as n,
        * only for odd positive n
        *
        * possible error message:
        *   zero, or even, or negative modulus in zmstart
        * result undefined if error occurs
        \******************************************************************/

    void zmfree();
        /******************************************************************\
        * Allows internal arithmetic to overwrite the current Montgomery
        * modulus without restoring it
        \******************************************************************/

    void ztom(verylong a, verylong *ma);
        /******************************************************************\
        * *ma = montgomery(a);
        * 
        * converts the ordinary integer a modulo zn to its
        * Montgomery representation ma
        *
        * possible error message:
        *   undefined Montgomery modulus in ztom
        * result undefined if error occurs
        \******************************************************************/

    void zmtoz(verylong ma, verylong *a);
        /******************************************************************\
        * *a = verylong(ma);
        * 
        * converts the Montgomery representation ma of a back to a
        *
        * possible error message:
        *   undefined Montgomery modulus in zmtoz
        * result undefined if error occurs
        \******************************************************************/

    void zmontadd(verylong ma, verylong mb, verylong *mc);
        /******************************************************************\
        * *mc = (ma + mb) % zn;
        * 
        * for Montgomery numbers ma and mb only
        *
        * possible error message:
        *   undefined Montgomery modulus in zmontadd
        * result undefined if error occurs
        \******************************************************************/
            
    void zmontsub(verylong ma, verylong mb, verylong *mc);
        /******************************************************************\
        * *mc = (ma - mb) % zn;
        * 
        * for Montgomery numbers ma and mb only
        *
        * possible error message:
        *   undefined Montgomery modulus in zmontsub
        * result undefined if error occurs
        \******************************************************************/

    void zsmontmul(verylong ma, long d, verylong *mc);
        /******************************************************************\
        * mc = (d * ma) % zn;
        * 
        * for ordinary long d (Not converted), and Montgomery number ma
        *
        * possible error message:
        *   undefined Montgomery modulus in zsmontmul
        * result undefined if error occurs
        \******************************************************************/

    void zmontmul(verylong ma, verylong mb, verylong *mc);
        /******************************************************************\
        * *mc = (ma * mb) % zn;
        * 
        * for Montgomery numbers ma and mb only
        *
        * possible error message:
        *   undefined Montgomery modulus in zmontmul
        * result undefined if error occurs
        \******************************************************************/

    void zmontsq(verylong ma, verylong *mb);
        /******************************************************************\
        * *mb = (ma * ma) % zn;
        * 
        * for Montgomery number ma only
        *
        * possible error message:
        *   undefined Montgomery modulus in zmontsq
        * result undefined if error occurs
        \******************************************************************/

    void zmontdiv(verylong ma, verylong mb, verylong *mc);
        /******************************************************************\
        * *mc becomes the Montgomery quotient of ma and mb,
        * i.e., the Montgomery representation mc of c = (a/b)%zn,
        * where ma and mb are the Montgomery representations of a and b
        * 
        * for Montgomery numbers ma and mb only
        *
        * possible error message:
        *   undefined Montgomery modulus in zmontdiv
        *   division by zero in zmontdiv
        *   undefined quotient in zmontdiv
        * result undefined if error occurs, except if the quotient
        * is undefined, in which case a factor of zn will be returned in mc
        * (of course, only if the -DNO_HALT flag is used)
        \******************************************************************/

    void zmontinv(verylong ma, verylong *mb);
        /******************************************************************\
        * *mb becomes the Montgomery inverse of ma
        * i.e., the Montgomery representation mb of b = (1/a)%zn,
        * where ma is the Montgomery representation of a
        * 
        * for Montgomery number ma only
        *
        * possible error message:
        *   undefined Montgomery modulus in zmontinv
        *   division by zero in zmontinv
        *   undefined inverse in zmontinv
        * result undefined if error occurs, except if the inverse
        * is undefined, in which case a factor of zn will be returned in mb
        * (of course, only if the -DNO_HALT flag is used)
        \******************************************************************/

    void zmontexp(verylong ma, verylong e, verylong *mb);
        /******************************************************************\
        * *mb = (ma ^ e) % zn;
        * 
        * for Montgomery number ma and normal e
        *
        * possible error message:
        *   undefined Montgomery modulus in zmontexp
        *   undefined quotient in zmontexp      (caused by negative exponent)
        * result undefined if error occurs, except if the quotient
        * is undefined, in which case a factor of zn will be returned in mb
        * (of course, only if the -DNO_HALT flag is used)
        \******************************************************************/

    void zmontexp_m_ary(verylong ma, verylong e, verylong *mb, long m);
        /******************************************************************\
        * *mb = (ma ^ e) % zn;
        * 
        * for Montgomery number ma and normal e using m-ary
        * exponentiation which is usually faster than zmontexp,
        * if m <= 1, default m will be used (see above under Modular
        * arithmetic), if m >= NBITS, NBITS-1 will be used
        *
        * possible error message:
        *   undefined Montgomery modulus in zmontexp_m_ary
        *   undefined quotient in zmontexp_m_ary   (caused by negative exponent)
        * result undefined if error occurs, except if the quotient
        * is undefined, in which case a factor of zn will be returned in mb
        * (of course, only if the -DNO_HALT flag is used)
        \******************************************************************/

    void zmontexp_doub1(verylong x1, verylong e1, verylong x2, verylong e2,
                       verylong *b);
        /******************************************************************\
        * b = ( x1 ^ e1) * (x2 ^ e2) % zn;
        *
        * works as and use as zexpmod_doub1, but for Montgomery numbers x1
        * and x2 (and normal e1 and e2) only
        *
        * possible error message:
        *  undefined Montgomery modulus in zmontexp_doub1
        *  negative exponent in zmontexp_doub1     (if e1 or e2 negative)
        * result undefined if error occurs
        \******************************************************************/

    void zmontexp_doub2(verylong x1, verylong e1, verylong x2, verylong e2,
                       verylong *b);
        /******************************************************************\
        * b = ( x1 ^ e1) * (x2 ^ e2) % zn;
        *
        * works as and use as zexpmod_doub2, but for Montgomery numbers x1
        * and x2 (and normal e1 and e2) only
        *
        * possible error message:
        *  undefined Montgomery modulus in zmontexp_doub2
        *  negative exponent in zmontexp_doub2     (if e1 or e2 negative)
        * result undefined if error occurs
        \******************************************************************/

    void zmontexp_doub3(verylong x1, verylong e1, verylong x2, verylong e2,
                       verylong *b);
        /******************************************************************\
        * b = ( x1 ^ e1) * (x2 ^ e2) % zn;
        *
        * works as and use as zexpmod_doub3, but for Montgomery numbers x1
        * and x2 (and normal e1 and e2) only
        *
        * possible error message:
        *  undefined Montgomery modulus in zmontexp_doub3
        *  negative exponent in zmontexp_doub3     (if e1 or e2 negative)
        * result undefined if error occurs
        \******************************************************************/

    void zmontexp_doub(verylong x1, verylong e1, verylong x2, verylong e2,
                       verylong *b);
        /******************************************************************\
        * b = ( x1 ^ e1) * (x2 ^ e2) % zn;
        *
        * for Montgomery numbers x1 and x2 (and normal e1 and e2),
        * uses Shamir`s method with sliding window of size depending on
        * maximal size of e1 or e2, using zmontexp_doub1, zmontexp_doub2,
        * or zmontexp_doub3, only for e1 and e2 non-negative, use if e1
        * and e2 approximately same size
        *
        * possible error message:
        *  undefined Montgomery modulus in zmontexp_doub
        *  negative exponent in zmontexp_doub     (if e1 or e2 negative)
        * result undefined if error occurs
        \******************************************************************/


/******************************************************************************\
*  Euclidean algorithms 
*
*  Greatest common divisors, extended Euclidean algorithm,
*  modular inversion, Chinese remaindering, Jacobi symbols
\******************************************************************************/

    void zgcd(verylong m1, verylong m2, verylong *r);
        /******************************************************************\
        * *r = greatest common divisor of m1 and m2;
        * 
        * uses binary gcd algorithm
        \******************************************************************/

    void zgcdeucl(verylong m1, verylong m2, verylong *r);
        /******************************************************************\
        * *r = greatest common divisor of m1 and m2;
        * 
        * uses plain Euclidean algorithm (which might be
        * faster than the binary method in special cases)
        \******************************************************************/

    void zexteucl(verylong a, verylong *xa,
                 verylong b, verylong *xb,
                 verylong *d);
        /******************************************************************\
        * *d = a * *xa + b * *xb = gcd(a, b);
        * 
        * sets *d, *xa and *xb given a and b,
        * arguments cannot be the same,
        * uses Lehmer`s trick
        *
        * possible error message:
        *   non-zero remainder in zexteucl...BUG      (report this)
        * result undefined if error occurs
        \******************************************************************/

    long zinvs(long n, long p);
        /******************************************************************\
        * return (x);
        * 
        * such that (x * abs(n)) % abs(p) = gcd(n, p)
        \******************************************************************/

    long zinvodds(long n, long p);
        /******************************************************************\
        * return (x);
        * 
        * such that x * n = 1 % p,
        * only for n > 0 and odd p >= 3 with n and p coprime
        * returns 0 for p even
        *
        * possible error message:
        *   arguments not coprime in zinvodds
        * result undefined if error occurs
        \******************************************************************/

    long zinv(verylong a, verylong b, verylong *c);
        /******************************************************************\
        * if (a and b coprime)
        * {
        *     *c = inv; 
        *     return(0);
        * }
        * else
        * {
        *     *c = gcd(a, b);
        *     return(1);
        * }
        * 
        * where inv is such that (inv * a) % b == 1
        * a,b > 0, uses Lehmer`s trick
        *
        * possible error message:
        *   zero or negative argument(s) in zinv
        * result undefined if error occurs
        \******************************************************************/

    void zchirem(verylong a, verylong ma,
                 verylong b, verylong mb, verylong *d);
        /******************************************************************\
        * *d = chi;
        * 
        * such that chi == ma % a and chi == mb % b,
        * a and b coprime if ma != mb, a and b positive,
        * 0 <= d < a * b, unless a == b
        *
        * possible error message:
        *   zero or negative argument(s) in zchirem
        *   same moduli with different remainders in zchirem
        *   moduli not coprime in zchirem
        * result undefined if error occurs
        \******************************************************************/

    long zjacobi(verylong a, verylong n);
        /******************************************************************\
        * for n>0 only:
        *     return(Jacobi symbol J(a, n)), i.e.,
        *           if (gcd(a, n) != 1)
        *               return(0);
        *           if (x exists such that x * x == a % n)
        *               return(1);
        *           else
        *               return(-1);
        *
        * possible error message:
        *   non-positive second argument in zjacobi
        * result undefined if error occurs
        \******************************************************************/

    long zjacobis(long a, long n);
        /******************************************************************\
        * as above, accepts only odd positive n
        *
        * possible error message:
        *   even second argument in zjacobis
        * result undefined if error occurs
        \******************************************************************/


/******************************************************************************\
*  Random number generation 
*
*  Because some algorithms need some more or less random bits every now
*  and then, I`ve included a few functions to get random looking
*  numbers.  These functions are only intended as a source of random
*  numbers in probabilistic algorithms, like guessing a next base for a
*  compositeness test, or guessing an elliptic curve for an ECM
*  factoring attempt. They are not intended as a source of
*  cryptographically secure random bits. There are some
*  functions below that might have cryptographic applications (like
*  zrandomprime and zrandomqprime). You should call them with
*  your own favorite cryptographically strong random number generator
*  if you are going to use these functions in real life. If security
*  is not an important issue, you can call them with  zrandomb
\******************************************************************************/

    void zrstarts(long s);
        /******************************************************************\
        * seed = s;
        * 
        * initialize the random number generator zrandom with seed = s,
        * if you don`t call zrstarts or zrstart, or forget to call any
        * of them seed gets initial value 7157891
        *
        * if you call zrstarts with s==0, zrandom will only produce zeroes
        \******************************************************************/

    void zrstart(verylong s);
        /******************************************************************\
        * seed = s;
        * 
        * initialize the random number generator zrandom with seed = s,
        * if you don`t call zrstarts or zrstart, or forget to call any
        * of them seed gets initial value 7157891
        *
        * if you call zrstart with s==0, zrandom will only produce zeroes
        \******************************************************************/

    void zrseed(verylong *a);
        /******************************************************************\
        * sets a to the current value of the seed
        \******************************************************************/

    long zrandom(long b);
        /******************************************************************\
        * seed = (seed * primitive_root) % prime;
        * return (seed % b);
        * 
        * return value is supposed to be a more or less random integer in [0,b),
        * if b <= 0 seed will not be changed, and 0 will be returned,
        * prime == 2 ^ 107 - 1,
        * primitive_root == (3 ^ 121) % prime
        * 
        * notice that zrandom returns only 0 if seed is initialized
        * as zero, and that only the value of seed modulo prime is significant
        \******************************************************************/

    void zrandomb(verylong bnd, verylong *a);
        /******************************************************************\
        * sets a to a random integer in the interval [0, bnd-1],
        * a gets zero if interval is empty.
        *
        * zrandomb uses zrandom to generate large enough random number,
        * which is modded out by bnd to get the resulting a
        \******************************************************************/

    void zrandoml(long length, verylong *a,
                               void (*generator) (verylong, verylong*));
        /******************************************************************\
        * sets a to a random integer of precisely abs(length) bits,
        * sign a becomes sign length
        *
        * generator is your own favorite random generator; a call to
        * generator(b,&c) should assign a more or less random
        * number in the range [0,b-1] to c; if cryptographic security is not
        * an issue, you can use zrandomb
        *
        \******************************************************************/

    long zrandomprime(long length, long nbtests, verylong *a,
                      void (*generator) (verylong, verylong*));
        /******************************************************************\
        * if |length| >= 2, *a is set to a random probable prime
        * of precisely |length| bits, if |length| < 2, *a is set to 0,
        * if length < 0 then *a will be 3 modulo 4,
        * uses nbtests + 1 probabilistic compositeness tests
        * (see below, zprobprime), returns 1 if success, 0 if not
        *
        * zrandomprime works by picking odd number of right size, keep adding
        * two until probably prime, or too large in which case pick again,
        * and start adding again (in accordance with NIST DSS Appendix),
        * generator as in zrandoml
        \******************************************************************/
            
    long zrandomqprime(long lp, long lq, long nbtests,
                       verylong *p, verylong *q, verylong *frac,
                      void (*generator) (verylong, verylong*));
        /******************************************************************\
        * if lq < 0 it expects a positive number (not necessarily prime) in q,
        * if lq >= 0 it attempts to set q to a random probable prime
        * having precisely lq bits, returns 0 if no success
        * 
        * attempts to set p to a random probable prime having precisely
        * lp bits, such that q divides p-1, returns 0 if no success
        * 
        * sets frac to [p/q] and returns 1 if everything successful
        * uses nbtests+1 probabilistic compositeness tests
        * (see below, zprobprime)
        * 
        * q-search as in zrandomprime, p-search: keep generating random
        * p of right size with q|p-1 until p is probably prime (in accordance
        * with NIST DSS Appendix), so this only works if lp is substantially
        * larger than |lq|, generator as in zrandoml
        *
        * possible error message:
        *   wrong second argument in zrandomqprime
        * result undefined if error occurs
        \******************************************************************/

    long zrandomfprime(long lq, long nbtests, verylong frac,
                       verylong *p, verylong *q,
                      void (*generator) (verylong, verylong*));
        /******************************************************************\
        * returns 1 if successfully generated random probable primes q
        * and p such that p = frac*q+1, where q has binary length lq,
        * repeatedly uses zrandoml to set q until q and p prime
        *
        * returns 0 if frac<2 or frac odd, or if it couldn`t find a
        * prime q of lq bits, generator as in zrandoml
        \******************************************************************/

    long zrandomgprime(long lq, long nbtests, long smallval,
                       verylong *p, verylong *q, verylong *g,
                      void (*generator) (verylong, verylong*));
        /******************************************************************\
        * returns 1 if successfully generated random probable primes q
        * and p such that p = 2*q+1, where q has binary length lq
        * (p and q determined using zrandomfprime with frac == 2),
        * and a generator g of the multiplicative group modulo p;
        * if smallval==0 g will be randomly selected (using zrandomb),
        * otherwise the smallest positive g will be selected, generator as
        * in zrandoml
        *
        * returns 0 if it couldn`t find a prime q of lq bits
        \******************************************************************/


/******************************************************************************\
*  Small prime generation
*
*  Functions to generate the sequence of primes up to (2*PRIM_BND+1)^2
\******************************************************************************/

    long zpnext(void);
        /******************************************************************\
        * returns the next prime, starting at 2, unless zpstart
        * has been called in which case the first subsequent zpnext call
        * returns 3
        * after returning the last prime before (2*PRIM_BND+1)^2 it
        * returns 2 on the next call
        \******************************************************************/

    long zpnextb(long b);
        /******************************************************************\
        * returns the next prime >= b
        * and repositions primegenerator so that zpnext will return
        * the next bigger prime, etc.
        \******************************************************************/

    void zpstart(void);
        /******************************************************************\
        * restart the prime generator in such a way
        * that the next call to zpnext will produce 3
        \******************************************************************/

    void zpstart2(void);
        /******************************************************************\
        * restart the prime generator in such a way
        * that the next call to zpnext will produce 2
        \******************************************************************/

    long zp(void);
        /******************************************************************\
        * returns 0 if zpnext has not been called yet,
        * returns 0 if zpnext has not been called after the most recent
        * zpstart or zpstart2, returns the prime previously returned
        * by zpnext otherwise
        \******************************************************************/


/******************************************************************************\
*  Compositeness testing and factorization 
*
*  Probabilistic compositeness tests (often referred to as
*  pseudo-prime tests, or primality tests), trial division,
*  Pollard rho factoring algorithm, elliptic curve factoring.
\******************************************************************************/

    long zcomposite(verylong *m, long t, long firstbase);
        /******************************************************************\
        * if t > 0, or t < 0 and ( m < 0, or m even, or m < sqrt(RADIX) ):
        * 
        * - returns 1 if m<2 or if zcomposite proved that m
        *   is composite
        * 
        * - returns 0 if m>=2 and zcomposite could not prove
        *   the compositeness of m after t compositeness tests,
        *   which should happen with probability <=(1/4)^|t| for
        *   composite m, so that one may safely assume that
        *   m is prime if t is large enough (say, 5); in this
        *   case m is called a `probable prime.`
        *
        * if t < 0 and m > 0, and m odd, and m > sqrt RADIX:
        * 
        * - returns 1 if zcomposite proved that m is composite
        *   and not a prime power
        * 
        * - returns 0 if m>=2 and zcomposite could not prove
        *   the compositeness of m after -t compositeness tests,
        *   which should happen with probability <=(1/4)^(-t) for
        *   composite m, so that one may safely assume that
        *   m is prime if t is large enough
        * 
        * - returns -1 if zcomposite proved that m is composite
        *   and not a prime power and furthermore an a has
        *   been found satisfying gcd(a^m-a, m)=m; in this case
        *   m is set to a, so that the original m can be factored
        *   by looking at the factorization of a^m-a
        * 
        * - return -2 if zcomposite has found a factor of m,
        *   in this case m is set to the factor
        * 
        * zcomposite uses (a stronger version of) Fermat`s little
        * theorem to prove compositeness, which means, roughly speaking,
        * that it attempts to find a number a for which a^m != a mod m,
        * where a is called the `base;` if firstbase = 0, then at most
        * |t| bases will be tried, all randomly selected in the range
        * [3, RADIX-1]; if firstbase != 0, then firstbase will be the
        * first base to be tried, the other |t|-1 will be randomly
        * selected in the range [3, RADIX-1]; this allows you to select
        * 2 as the first base (which allows faster arithmetic)
        * 
        * It`s better not to use zcomposite for large t to convince
        * yourself of the m`s primality: use zcomposite with |t|=1, and
        * if that returns 0, call zmcomposite to do as many additional
        * tests as you like
        \******************************************************************/

    long zprime(verylong m, long t, long firstbase);
        /******************************************************************\
        * return (1 - zcomposite(&m, abs(t), firstbase));
        \******************************************************************/

    long zmcomposite(verylong m, long t);
        /******************************************************************\
        * like zcomposite but faster because it does mixed ordinary/
        * Montgomery arithmetic, which makes it faster, only for t >= 0,
        * only intended for many compositeness tests on a number that is
        * already suspected to be prime
        \******************************************************************/

    long zprobprime(verylong n, long nbtests);
        /******************************************************************\
        * returns 1 if n is probably prime, 0 if n is composite,
        * after at most nbtest + 1 probabilistic compositeness tests, and
        * some trial division: uses ztridiv, if no factor found uses
        * zcomposite with firstbase 2, if not yet proved composite uses
        * zmcomposite with t = nbtests
        * 
        * faster than zprime for most randomly selected composites
        * because small prime divisors are detected quickly
        \******************************************************************/

    long ztridiv(verylong n, verylong *cof, long b1, long b2);
        /******************************************************************\
        * attempts to find smallest prime divisor >= b1 and <= b2 of n,
        * returns this divisor if found, or first prime >b2 if nothing found,
        * if factor found, cof will be set to the cofactor (n divided by the
        * factor), only intended for large n, and only for
        * b2 <= (2 * PRIM_BND + 1) ^ 2 so it is Not clever about trial
        * division with primes > sqrt(n)
        * 
        \******************************************************************/

    long zpollardrho(verylong n, verylong *res, verylong *cof, long t);
        /******************************************************************\
        * returns positive integer if Pollard rho found factor of n, puts factor
        * in res and n/res in cof, res <= cof unless res=2, cof=1,
        * for negative n sets res = -1, returns 0 if no factor found,
        * does at most t (t=0 is infinity) iterations of main loop (see
        * source code)
        *
        * possible error message:
        *   wrong factor in zpollardrho...BUG     (report this)
        * result undefined if error occurs
        \******************************************************************/

    long zsquf(verylong n,verylong *f1,verylong *f2);
        /******************************************************************\
	* returns positive integer if zsquf found factor of n, puts factors
	* in f1 and f2.
	* returns 0 if no factor found
        *
        * assumes n is < RADIX^2
        *
        \******************************************************************/


    long zecm_trial(verylong n, long m, verylong *f, long info, FILE *fp);
        /******************************************************************\
        * not documented, but accessible
        \******************************************************************/

    long zfecm(verylong n, verylong *f, long s, long *curvebnd,
              long *phase1bnd, long grow, long nbtests, long info, FILE *fp);
        /******************************************************************\
        * returns 0 after unsuccessful attempt to factor n using
        * *curvebnd different randomly generated (starting from seed s, unless
        * s=0 in which case zrandom simply continues or starts at default)
        * elliptic curves with first phase bound *phase1bnd (which increases
        * by grow percent for each new curve) or if n <= 2^(NBITS/2),
        * returns 1 if the attempt was successful and puts the factor
        * found in f, returns 2 (happens only in Very exceptional cases)
        * if a factor of n can be found by looking at the factorization
        * of f^n-f for the f on output, returns -1 if n found to be
        * probably prime after nbtests probabilistic compositeness tests.
	* upon returning, the value of *curvebnd is set to the number of
	* curves used and the value of *phase1bnd is updated to the last
	* value used (same as the initial value when grow == 0).
        * 
        * runs silently with info=0, gives timing per curve
        * and some additional information for info=1, and gives timings
        * for phase 1 and phase 2 for info>1, all output goes to file *fp.
	* Runs silently if fp is NULL.
        * 
        * If you wish you can change the value of ECM_BATCH (default
        * set at 1): phase one of ecm combines ECM_BATCH prime powers
        * and does only one inversion per batch. Higher values of
        * ECM_BATCH might save some run time on some machines.
        * 
        * if you find a factor of 38 or more decimal digits using
        * zfecm, I`d like to hear about it
        \******************************************************************/

    long zecm(verylong n, verylong *f, long s, long curvebnd,
              long phase1bnd, long grow, long nbtests, long info);
        /******************************************************************\
        * as zecm, with fp=stdout and no changes to curvebnd or phase1bnd
        \******************************************************************/



/******************************************************************************\
*  Allocation
\******************************************************************************/

    void zsetlength(verylong *v, long len, char *str);
        /******************************************************************\
        * (re)allocates enough space for v to hold an integer of
        * absolute value < 2 ^ (len * RADIX), uses SIZE instead of len if
        * len < SIZE, use this to allocate space by hand (if you`re using
        * the -DNO_ALLOCATE flag), don`t use this unless you know what you`re
        * doing
        *
        * possible error messages:
        *   reallocation failed in zsetlength         (out of space)
        *   allocation failed in zsetlength           (out of space)
        *   negative size allocation in zsetlength    (*x==0 and len<0,
        *          ====> BUG if you didn`t call zsetlength yourself)
        * result undefined if error occurs
        \******************************************************************/

    void zfree(verylong *x);
        /******************************************************************\
        * frees the memory allocated for x, and sets x back to 0,
        * where x is a very long int allocated by the functions internally
        * or by zsetlength, use this to free the memory of very long ints
        * which are local to functions (unless the locals are static)
        \******************************************************************/


/******************************************************************************\
*  Timing 
*
*  Functions to get timing information
\******************************************************************************/

    double gettime(void);
        /******************************************************************\
        * returns (user time + system time) spent on current process
        * in seconds, use this to get timing information about your_job
        * as follows:
        *     extern double gettime();
        *     double time_it_took;
        *     ...
        *     time_it_took = gettime();
        *     your_job(and_its_parameters);
        *     time_it_took = gettime() - time_it_took;
        *     printf("your_job took %12.8lf seconds\n", time_it_took);
        *     ...
        \******************************************************************/

    double getutime(void);
        /******************************************************************\
        * same as gettime, but returns user time only
        \******************************************************************/

    double getstime(void);
        /******************************************************************\
        * same as gettime, but returns system time only
        \******************************************************************/

    void starttime(void);
        /******************************************************************\
        * (re)starts a timer, to be used with printtime()
        \******************************************************************/

    void printtime(FILE *f);
        /******************************************************************\
        * prints time spent since last call to starttime() to file f,
        * followed by newline, and flushes f
        \******************************************************************/


/******************************************************************************\
*  Input and output 
*
*       Functions for (hexa)decimal/direct in/output from/to files,
*       stdin/stdout, or strings
\******************************************************************************/

    long zfread(FILE *f, verylong *a);
        /******************************************************************\
        * reads decimal representation of very long int from file f until
        * first space into a; skips newlines, continues on next line only if
        * line ends with a \, accepts sign on first line, returns 1 if success,
        * 0 if not, may use _ instead of -
        \******************************************************************/

    long zfwrite_c(FILE *f, verylong a, long linelen, char *str1, char *str2);
        /******************************************************************\
        * writes decimal representation of a to file f with at most
        * approximately linelen char per line, \ indicates continuation on
        * next line, first line begins with str1, each consecutive line
        * with str2, returns decimal length of a
        *
        * possible error message:
        *   allocation failure in zfwrite      (a too large to print)
        *   reallocation failure in zfwrite    (a too large to print)
        * result undefined if error occurs
        \******************************************************************/

    long zfwrite(FILE *f, verylong a);
        /******************************************************************\
        * writes decimal representation of a to file f,
        * format as in zfwrite_c(f,a,OUT_LINE,"",""), returns decimal length a
        \******************************************************************/

    long zfwriteln(FILE *f, verylong a);
        /******************************************************************\
        * writes decimal representation of a to file f followed by a newline,
        * format as in zfwrite_c(f,a,OUT_LINE,"",""), returns decimal length a
        \******************************************************************/

    long zread(verylong *a);
        /******************************************************************\
        * reads decimal representation of very long int from standard input
        * into a, format as in zfread, returns 1 if success, 0 if not
        \******************************************************************/

    long zwrite(verylong a);
        /******************************************************************\
        * writes decimal representation of a to standard output,
        * format as in zfwrite, returns decimal length a
        \******************************************************************/

    long zwriteln(verylong a);
        /******************************************************************\
        * writes decimal representation of a to standard output
        * followed by a newline, format as in zfwrite, returns decimal
        * length a
        \******************************************************************/

    long zsread(char *str, verylong *a);

        /******************************************************************\
        * reads decimal representation of very long int from string str
        * into a, returns 1 if success, 0 if not
        \******************************************************************/

	long
		zhswrite(
		char *str,
		verylong a
		 );

    long zswrite(char *str, verylong a);
        /******************************************************************\
        * writes decimal representation of a to string str, returns
        * decimal length a
        *
        * possible error message:
        *   allocation failure in zfwrite      (a too large to print)
        *   reallocation failure in zfwrite    (a too large to print)
        * result undefined if error occurs
        \******************************************************************/

    void zhfread(FILE *f, verylong *a);
        /******************************************************************\
        * reads hexadecimal representation of very long int from file f
        * into a, skips newlines and spaces, but continues on next line only if
        * line ends with a \, end of input indicated by a newline, accepts
        * sign on first line, accepts both lower and upper case a, b, c, d, e, f
        * for digits 10 thru 15
        \******************************************************************/

    void zhfwrite(FILE *f, verylong x);
        /******************************************************************\
        * writes hexadecimal representation of x to file f in blocks
        * of 8 characters separated by spaces, at most seven blocks per line,
        * \ indicates continuation on next line, the digits 10 thru 15 are
        * represented by A, B, C, D, E, F, unless package compiled with
        * -DHEX_LOWER_CASE flag, in which case 10 thru 15 are
        * are represented by a, b, c, d, e, f. The 8 characters per block
        * can be changed by giving HEX_BLOCK (default 8) a different
        * value during compilation, the seven blocks per line can be changed
        * similarly using HEX_BLOCKS_PER_LINE (default 7). The space
        * separating the blocks can be changed in the source (HEX_SEP_CHAR)
        \******************************************************************/

    void zhfwriteln(FILE *f, verylong x);
        /******************************************************************\
        * writes hexadecimal representation of x to file f followed by a
        * newline, format as in zhfwrite
        \******************************************************************/

    void zhread(verylong *a);
        /******************************************************************\
        * reads hexadecimal representation of very long int from stdin
        * into a, format as in zhfread
        \******************************************************************/

    void zhwrite(verylong a);
        /******************************************************************\
        * writes hexadecimal representation of a to stdout, format
        * as in zhfwrite
        \******************************************************************/

    void zhwriteln(verylong a);
        /******************************************************************\
        * writes hexadecimal representation of a to stdout followed
        * by a newline, format as in zhfwrite
        \******************************************************************/

    void zhsread(char *str, verylong *a);
        /******************************************************************\
        * reads hexadecimal representation of very long int from string str
        * into a, format as in zhfread
        \******************************************************************/

    long zbfread(FILE *f, verylong *a);
        /******************************************************************\
        * reads very long int from binary file f into a, returns 1 if success,
        * 0 if not, file f written using zbfwrite
        \******************************************************************/

    long zbfwrite(FILE *f, verylong a);
        /******************************************************************\
        * writes very long int a to binary file f, returns 1 if success,
        * 0 if not, file f can be read using zbfread
        \******************************************************************/

    long zfread_b(FILE *f, verylong *a, verylong in_base, long sym_in);
        /******************************************************************\
        * reads base in_base representation of very long int from file f
        * into a, skips newlines and spaces, but continues on next line only if
        * line ends with a \, end of input indicated by a newline, accepts
        * sign on first line, accepts both lower and upper case a, b, c, d, e, f
        * for digits 10 thru 15, requires decimal representation of digits
        * for in_base > 16, accepts symmetric digits if sym_in == 1, but
        * only >=0 digits for sym_in = 0 (with exception of most significant
        * (is first) digit which might be negative for negative input
        * if sym_in == 1: -10 -5 (newline ) is interpreted as -10*base -5,
        * but with sym_in = 0 it is interpreted as -(10*base + 5).
        * may use _ instead of -, return 1 if success, 0 if not
        \******************************************************************/

    long zfwrite_b(FILE *f, verylong a, verylong out_base, long sym_out);
        /******************************************************************\
        * writes base out_base representation of a to file f, representation
        * uses digits in [0,...,out_base-1] is sym_out is 0, but uses
        * digits centered around zero (as far as possible) if sym_out is 1,
        * only for out_base > 1, for out_base > 16 the digits will be
        * represented in decimal, for digits 10 thru 15 see zhfwrite
        * returns the number of digits used for the representation
        *
        * possible error message:
        *   output base < 2 in zfwrite_b        (only for output base > 1)
        *   allocation failure in zfwrite_b     (a too large to print)
        *   reallocation failure in zfwrite_b   (a too large to print)
        *   zfwrite_b...BUG                     (report this)
        * result undefined if error occurs
        \******************************************************************/

    long zfwriteln_b(FILE *f, verylong a, verylong out_base, long sym_out);
        /******************************************************************\
        * as zfwrite_b, but followed by a newline
        \******************************************************************/


#if 0

Example 1
=========
The following program attempts to find primes p and q with
q dividing p-1 and a generator of a subgroup of order q of (Z/pZ)*.
The input consists of a seed to initialize the random generator
(using zrstarts, so seed is an ordinary long), and two integers
specifying the binary lengths of the primes p and q.

Notice the use of Montgomery arithmetic in find_p_q_q: after
finding p, the call zmstart(*p) initializes the Montgomery modulus
zn as *p (p is a *verylong in find_p_q_g, so *p is the verylong required
for zmstart), and mont_one contains the Montgomery representation
of the constant 1. Also notice the use of the *`s and &`s in the
other function calls, to get all arguments at the right pointer-level.
If you don`t get all *`s and &`s right, it won`t work. Fortunately,
in ansi-C the compiler will complain unless you get it right.

    #include "lip.h"

    long 
    find_p_q_g(
        long lp,
        long lq,
        verylong *p,
        verylong *q,
        verylong *g,
	void (*generator) (verylong, verylong*)
        )
    {
        static verylong frac = 0, mont_one = 0;
	long escape = 1;
        if (!zrandomqprime(lp, lq, 1, p, q, &frac, generator))
            return (0);
        zmstart(*p);
        zintoz((long)1, &mont_one);
        ztom(mont_one, &mont_one);
        do
        {
	    (*generator)(*p,g);
	    if (ziszero(*g))
		zsadd(*g,escape++,g);
            zmontexp_m_ary(*g, frac, g, 0);
        } while (!zcompare(*g, mont_one));
        zmtoz(*g, g);
        return (1);
    }

    main()
    {
        verylong p = 0;
        verylong q = 0;
        verylong g = 0;
        long s;
        long lp;
        long lq;

        scanf("%ld %ld %ld", &s, &lp, &lq);
        zrstarts(s);
        if (find_p_q_g(lp, lq, &p, &q, &g, zrandomb))
        {
            zwriteln(g);
            printf("generates a subgroup of order\n");
            zwriteln(q);
            printf("in the multiplicative group of integers modulo\n");
            zwriteln(p);
        }
        else
            printf("couldn`t find primes p and q with q dividing p-1\n");
    }

Sample input:

        12345 128 112

with output:

	138645574047913030172742234323958638849
	generates a subgroup of order
	4619954241760735487231034762886657
	in the multiplicative group of integers modulo
	269149294216496928015105623216250863507


Example 2
=========
The following program does the same as the previous one,
except that it generates two different p`s and g`s (both
with the same q). The second p is generated as above, but
the first p is such that p equals f(m) for a fifth degree
polynomial f with small coefficients, and m close to the
fifth root of p. The method below works only for a very
limited range of lq`s (given lp); it works for instance
for lp=512 and lq=160. These p are interesting because the
discrete logarithm problem modulo such p is much easier (though
still hard...) then for `randomly` chosen p. Generating them,
however, takes much more time. Notice that such p can easily be
recognized using zroot and ztosymbas.

#include "lip.h"

#define BOUND    (2)

double tim;

find_x_data(
    long lc,
    long lp,
    verylong *xlow,
    verylong *xint
    )
{
    static verylong aux=0;
    zintoz(1,&aux); zlshift(aux,lp-1,&aux);
    zsdiv(aux,lc,&aux); zroot(aux,5,xlow); zsadd(*xlow,1,xlow);
    zintoz(1,&aux); zlshift(aux,lp,&aux);
    zsdiv(aux,lc,&aux); zroot(aux,5,xint);
    zsub(*xint,*xlow,xint);
}

find_x(
    long lc,
    verylong *x,
    long lp
    )
{
    static verylong xlow[BOUND], xint[BOUND];
    static long non_init=1, lastlp = 0;
    register long i;
    if (non_init) {
        non_init = 0;
        for (i=0;i<BOUND;i++) xlow[i]=0;
        for (i=0;i<BOUND;i++) xint[i]=0;
    }
    if (!(xlow[lc-1]) || (lp!=lastlp)) {
        find_x_data(lc,lp,&(xlow[lc-1]),&(xint[lc-1]));
        lastlp = lp;
    }
    zrandomb(xint[lc-1],x);
    zadd(*x,xlow[lc-1],x);
}

long find_q(
    verylong x,
    verylong *q,
    long lq
    )
{
    static verylong lowq=0, highq=0, nq=0; 
    static long lastlq=0, p;
    if (lq != lastlq) {
        lastlq = lq;
        zintoz(1,&lowq); zlshift(lowq,lq-1,&lowq);
        zintoz(1,&highq); zlshift(highq,lq,&highq);
    }
    zsq(x,q); zsadd(*q,1,q);
    zpstart2();
    /* if lq small compared to 2*lp/5, then increase the 20000 */
    while ((p=zpnext()) < 20000) {
        while (!zsdiv(*q,p,&nq)) {
            if (zcompare(nq,lowq)<0) return 0;
            zswap(&nq,q);
        }
    }
    if (zcompare(*q,highq) >= 0) return 0;
    if (zprime(*q,2,2)) return 1;
    return 0;
}

long find_p(
    long lp,
    verylong *p,
    long a3,
    verylong x1 
    )
{
    register long a2, a1, a0;
    static verylong lowp=0, highp=0;
    static long lastlp=0;
    static verylong x2=0, x3=0, x4=0, x5=0, aux=0;
    if (lp != lastlp) {
        lastlp = lp;
        zintoz(1,&lowp); zlshift(lowp,lp-1,&lowp);
        zintoz(1,&highp); zlshift(highp,lp,&highp);
    }
    zsq(x1,&x2); zmul(x1,x2,&x3); zsq(x2,&x4); zmul(x1,x4,&x5);
    zsmul(x5,a3,&x5);
    for (a2 = -BOUND; a2 <= BOUND; a2 ++)
    for (a1 = -BOUND; a1 <= BOUND; a1 ++)
    for (a0 = -BOUND; a0 <= BOUND; a0 ++) {
        zsmul(x4,a2,p); zadd(x5,*p,p);
        zsmul(x3,(a1+a3),&aux); zadd(*p,aux,p);
        zsmul(x2,(a0+a2),&aux); zadd(*p,aux,p);
        zsmul(x1,a1,&aux); zadd(*p,aux,p);
        zsadd(*p,(a0+1),p);
        if ((zcompare(*p,lowp)>=0) && (zcompare(*p,highp)<0) &&
            (zprobprime(*p,1))) {
            printf("(%ld,%ld,%ld,%ld,%ld,%ld) in x=",
                    a3,a2,a1+a3,a2+a0,a1,a0+1);
            zwriteln(x1);
            fflush(stdout);
            return 1;
        }
    }
    return(0);
}

long 
find_p_q_g(
    long lp,
    long lq,
    verylong *p,
    verylong *q,
    verylong *g,
    long p_initialized,
    long q_initialized
    )
{
    long escape = 1;
    static verylong frac = 0, mont_one = 0;
    if (p_initialized) zdiv(*p,*q,&frac,&mont_one);
    else if (q_initialized) {
        if (!zrandomqprime(lp,-1,1,p,q,&frac,zrandomb))
            return 0;
    } else if (!zrandomqprime(lp, lq, 1, p, q, &frac,zrandomb))
        return (0);
    zmstart(*p);
    zintoz((long)1, &mont_one);
    ztom(mont_one, &mont_one);
    do
    {
        zrandomb(*p, g);
	if (ziszero(*g))
		zsadd(*g,escape++,g);
        zmontexp_m_ary(*g, frac, g, 0);
    } while (!zcompare(*g, mont_one));
    zmtoz(*g, g);
    return (1);
}

long search(
    long lp, 
    long lq,
    verylong *p,
    verylong *q,
    verylong *g
    )
{
    register long a3,cnt=100,cnt2=0;
    verylong x1=0;
    tim = gettime();
    for (;;) {
        for (a3=BOUND;a3 > 0; a3 --) {
            find_x(a3,&x1,lp);
            if (find_q(x1,q,lq)) {
                if (find_p(lp,p,a3,x1)) {
                    if (find_p_q_g(lp,lq,p,q,g,1,0)) {
                        return 1;
                    } else {
                        printf("failure, bug\n");
                        return 0;
                    }
                }
            }
            cnt --; cnt2 ++;
            if (!cnt) {
                cnt = 100;
                printf("%12.8lf\n",gettime()-tim);
            }
        }
    }
}

main () {
    long seed, lp, lq;
    verylong p=0, q=0, g=0;
    scanf("%ld %ld %ld",&seed,&lp, &lq);
    zrstarts(seed);
    if ((5*lq < lp) || (5*lq >= 2*lp)) {
        printf("wrong choice of lq, given lp\n");
    } else {
        search(lp,lq,&p,&q,&g);
        zwriteln(g);
        printf("generates a subgroup of order\n");
        zwriteln(q);
        printf("in the multiplicative group of integers modulo\n");
        zwriteln(p);
        printf("\n"); fflush(stdout);
        find_p_q_g(lp,lq,&p,&q,&g,0,1);
        zwriteln(g);
        printf("generates a subgroup of order\n");
        zwriteln(q);
        printf("in the multiplicative group of integers modulo\n");
        zwriteln(p);
    }
}

Sample input:

	12345 512 160

with output (on a Sparc 10 model 50):

  3.76000000
  7.52000000
 11.29000000
 15.08000000
 18.87000000
 22.58000000
 26.40000000
 30.24000000
 34.03000000
 37.80000000
 41.58000000
 45.35000000
 49.11000000
 52.88000000
 56.67000000
 60.48000000
 64.24000000
 68.00000000
 71.79000000
 75.55000000
 79.26000000
 83.03000000
 86.80000000
 90.57000000
(1,2,0,0,-1,-1) in x=6669732341656104964502029382432
9362751779753305485633684911118505038419037405636648687671782771219104\
877220810690684615006144515338661060121963795907231644972439047091535229\
187627385769
generates a subgroup of order
943747661700036218559086949811091293194571309721
in the multiplicative group of integers modulo
13199030411146338143272886712220741028036629749389126263587401471402712\
464999500921756267373698996752386196832220041295603564662001282857874989\
403775126751

5158023791521930252670666673259965118816058379295632263374746360271437\
123929142602529310646812773838180339874626531306680071095321932808886119\
841427044863
generates a subgroup of order
943747661700036218559086949811091293194571309721
in the multiplicative group of integers modulo
11700597700389993490187022197000056687592911421118908770341871059151849\
025129646776595865327886910657123092425509373722802817105366518832401446\
762516493543


============================================================================

The package was tested on  ULTRIX V4.2,  HP-UX 8.05,  SunOS 4.1.1&2,
                           AIX V3.1, IRIX 4.0.5, Digital Unix 4.0

Bugs, questions, suggestions, additions, whatever, to

                Arjen K. Lenstra
		Citibank, N.A.
		4 Sylvan Way,
		Parsippany, NJ 07054

		email arjen.lenstra@citicorp.com
		fax 201 397 7657; 

#endif

