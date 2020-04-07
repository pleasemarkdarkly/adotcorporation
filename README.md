## Interactive Objects / Fullplay Media Source Code

The following codebase was the bases of the aforementioned companies between 1998 and 2001. The code comprises of various configurations of an embedded media player ala mp3. The system uses both a fixed operating environment per the Cirrus Logic reference code and [eCos](http://ecos.sourceware.org/).

The system ran mostly on the Cirrus Logic Maverick line of ARM7 32bit with internal 32kb of SRAM. Support for ethernet, CD-ROM, HDD, Dataplay, SD, MMC, and Smart Media was supported as well as various sized LCD 2-bit, 4-bit, etc. 

The Microsoft Windows Media Codec has been removed due to licensing restrictions. As well as the FreeDB .dat file, circa 1998.  If you need this, feel free to email. 

The following individuals contributed to the code, if I have left anyone out, feel free to make a pull request. 

Todd Malsbary
Edward Miller
Matt Tadd
Ezra Driebach
Dan Conti
Kenn Gordon

In 1998, before mp3 was even a thing. I wanted to make a playlist songs for a girl that I had a crush on. Making a mix-CD seemed like a lot of work. 

MP3 had just been formalized for a streaming audio format for satelite radio and I had a Sony mini-disc which I wondered why I couldn't write to the disk arbitrary data.

Additionally, Microsoft had just released the first PalmPC Casseopia which used the VR4111 MIPS processor. Michael Hipp had written a reference mp3 implementation, which I purchased, using this as a starting point, I read the NEC processor specification for the VR4111 and realized that the Microsft Visual Studio compiler for Windows CE didn't recognize the MACC processor instruction. With the C implementation of the mp3 decoder not fast enough, something had to be done.

First moving floats to fixed point.

```
#ifdef REAL_IS_FIXED
#define WRITE_SAMPLE(samples, sum, clip)         \
    if (sum.i > (32767 << FRACBITS))             \
    {                                            \
        *(samples) = 0x7fff;                     \
    }                                            \
    else if (sum.i < (-32768 << FRACBITS))       \
    {                                            \
        *(samples) = -0x8000;                    \
    }                                            \
    else                                         \
    {                                            \
        *(samples) = (short)(sum.i >> FRACBITS); \
    }
#elif defined(REAL_IS_INT)
```

The VR4111 required a hack to access the MACC.

```
inline int mult_real(int a, int b)
{
    int result;

#if defined(UNDER_CE) && defined(OPT_MULTREAL) 
    #if defined(_MIPS_)     
#if 1
        __asm("mult %0,%1",a,b);
        __asm("mflo %t1");
        __asm("srl %t1,%t1,15");  //FRACBITS
        __asm("mfhi %t2");
        __asm("sll %t2,%t2,32-15"); //FRACBITS
        __asm("or %t2,%t2,%t1");
        __asm("sw %t2,0(%0)",&result); 
#else    
        __asm("mult %0,%1",a,b);
        __asm("mflo %t1");
        __asm("mfhi %t0");
        __asm("dsrl %t0,%t0,15");  //FRACBITS
        __asm("sw %t1,0(%0)",&result); 
#endif
    #elif defined(_SH3_)
        __asm("mov.l   @R4,R2",&b);
        __asm(
              "mov     #H'FFFFFFF1,R0\n"  //FFFFFFF1 = -15
              "mov.l   @R4,R3\n"
              "dmuls.l R3,R2\n"
              "sts     MACL,R3\n"
              "sts     MACH,R2\n"
              "shld    R0,R3\n"
              "mov     #D'17,R0\n"
              "shad    R0,R2\n"
              "or      R2,R3\n"
              "mov.l   R3,@R5\n",
              &a, &result);
    #elif defined(_SH4_)
        __asm("mov.l   @R4,R2\n"
              "mov.l   @R5,R3\n",&a,&b);
        __asm("dmuls.l R3,R2");
        __asm("sts     MACL,R3");
        __asm("sts     MACH,R0");
        __asm("mov     #-15,R1");  //FFFFFFF4 = -15              
        __asm("shld    R1,R3");
        __asm("mov     #17,R1");
        __asm("shld    R1,R0");
        __asm("or      R3,R0");
        __asm("mov.l   R0,@R4",&result);
    #endif
#else
     result = (int)((((LONGLONG)a)*((LONGLONG)b))>>FRACBITS);
#endif  //UNDER_CE
    return result;
}

```

Using the MACC to speed up the decode process.

```
#elif defined(_MIPS_) && defined(MIPS_4111)
            LONGLONG _sum, tmp;
            _sum = 0;

            mips_macc(-*(--window), *b0++, &tmp);
            _sum += tmp;
            mips_macc(*(--window), *b0++, &tmp);
            _sum -= tmp;
            mips_macc(*(--window), *b0++, &tmp);
            _sum -= tmp;
            mips_macc(*(--window), *b0++, &tmp);
            _sum -= tmp;
            mips_macc(*(--window), *b0++, &tmp);
            _sum -= tmp;
            mips_macc(*(--window), *b0++, &tmp);
            _sum -= tmp;
            mips_macc(*(--window), *b0++, &tmp);
            _sum -= tmp;
            mips_macc(*(--window), *b0++, &tmp);
            _sum -= tmp;
            mips_macc(*(--window), *b0++, &tmp);
            _sum -= tmp;
            mips_macc(*(--window), *b0++, &tmp);
            _sum -= tmp;
            mips_macc(*(--window), *b0++, &tmp);
            _sum -= tmp;
            mips_macc(*(--window), *b0++, &tmp);
            _sum -= tmp;
            mips_macc(*(--window), *b0++, &tmp);
            _sum -= tmp;
            mips_macc(*(--window), *b0++, &tmp);
            _sum -= tmp;
            mips_macc(*(--window), *b0++, &tmp);
            _sum -= tmp;
            mips_macc(*(--window), *b0++, &tmp);
            _sum -= tmp;

            mips_macc_end(_sum, &sum);
```

The next generation Casseopia came with the next version of the VR4121, along with MADD16.

```
#if defined(MIPS_4121) && defined(OPT_MACC)
//#pragma optimize("", off)  // turn all optimizations off, otherwise
// clmips will mess up the registers used by
// the 4121 macc instruction
static real mips_4121_helper1(real *window, real *b0)
{
    real res;              //42
                          /*
      sum  = MULT_REAL(*window++, *b0++);
      sum -= MULT_REAL(*window++, *b0++);
      sum += MULT_REAL(*window++, *b0++);
      sum -= MULT_REAL(*window++, *b0++);
      sum += MULT_REAL(*window++, *b0++);
      sum -= MULT_REAL(*window++, *b0++);
      sum += MULT_REAL(*window++, *b0++);
      sum -= MULT_REAL(*window++, *b0++);
      sum += MULT_REAL(*window++, *b0++);
      sum -= MULT_REAL(*window++, *b0++);
      sum += MULT_REAL(*window++, *b0++);
      sum -= MULT_REAL(*window++, *b0++);
      sum += MULT_REAL(*window++, *b0++);
      sum -= MULT_REAL(*window++, *b0++);
      sum += MULT_REAL(*window++, *b0++);
      sum -= MULT_REAL(*window++, *b0++);
*/
    __asm("lw %t3,0(%0);" //load pointer to first array in %t3
          "lw %t4,0(%1);" //load pointer to second array in %t4
          "mtlo zero;"      //clear accumulate register
          "mthi zero;",
          &window, &b0);

    __asm("lw %t5,0x0(%t3);" //load value from %t3 at offset 0 (index 0)
          "lw %t6,0x0(%t4);" //load value from %t4 at offset 0
          "madd16 %t5,%t6;"
          //".word 0x01AE0028;"    //macc %t5, %t6

          "lw %t5,0x4(%t3);"  //load value from %t3 at offset 4 (index 1)
          "sub %t5,zero,%t5;" //negate %t5
          "lw %t6,0x4(%t4);"  //load value from %t4 at offset 4
          "madd16 %t5,%t6;"
          //".word 0x01AE0028;"    //macc %t5, %t6

          "lw %t5,0x8(%t3);" //load value from %t3 at offset 8 (index 2)
          "lw %t6,0x8(%t4);" //load value from %t4 at offset 8
          "madd16 %t5,%t6;"
          //".word 0x01AE0028;" //macc %t5, %t6

          "lw %t5,0xC(%t3);"  //load value from %t3 at offset 12 (index 3)
          "sub %t5,zero,%t5;" //negate %t5
          "lw %t6,0xC(%t4);"  //load value from %t4 at offset 12
          "madd16 %t5,%t6;"
          //".word 0x01AE0028;"    //macc %t5, %t6

          "lw %t5,0x10(%t3);" //load value from %t3 at offset 0 (index 4)
          "lw %t6,0x10(%t4);" //load value from %t4 at offset 0
          "madd16 %t5,%t6;"
          //".word 0x01AE0028;"    //macc %t5, %t6

          "lw %t5,0x14(%t3);" //load value from %t3 at offset 4 (index 5)
          "sub %t5,zero,%t5;" //negate %t5
          "lw %t6,0x14(%t4);" //load value from %t4 at offset 4
          "madd16 %t5,%t6;"
          //".word 0x01AE0028;"    //macc %t5, %t6

          "lw %t5,0x18(%t3);" //load value from %t3 at offset 8 (index 6)
          "lw %t6,0x18(%t4);" //load value from %t4 at offset 8
          "madd16 %t5,%t6;"
          //".word 0x01AE0028;" //macc %t5, %t6

          "lw %t5,0x1C(%t3);" //load value from %t3 at offset 12 (index 7)
          "sub %t5,zero,%t5;" //negate %t5
          "lw %t6,0x1C(%t4);" //load value from %t4 at offset 12
          "madd16 %t5,%t6;"
          //".word 0x01AE0028;"    //macc %t5, %t6

          "lw %t5,0x20(%t3);" //load value from %t3 at offset 0 (index 8)
          "lw %t6,0x20(%t4);" //load value from %t4 at offset 0
          "madd16 %t5,%t6;"
          //".word 0x01AE0028;"    //macc %t5, %t6

          "lw %t5,0x24(%t3);" //load value from %t3 at offset 4 (index 9)
          "sub %t5,zero,%t5;" //negate %t5
          "lw %t6,0x24(%t4);" //load value from %t4 at offset 4
          "madd16 %t5,%t6;"
          //".word 0x01AE0028;"    //macc %t5, %t6

          "lw %t5,0x28(%t3);" //load value from %t3 at offset 8 (index 10)
          "lw %t6,0x28(%t4);" //load value from %t4 at offset 8
          "madd16 %t5,%t6;"
          //".word 0x01AE0028;" //macc %t5, %t6

          "lw %t5,0x2C(%t3);" //load value from %t3 at offset 12 (index 11)
          "sub %t5,zero,%t5;" //negate %t5
          "lw %t6,0x2C(%t4);" //load value from %t4 at offset 12
          "madd16 %t5,%t6;"
          //".word 0x01AE0028;"    //macc %t5, %t6

          "lw %t5,0x30(%t3);" //load value from %t3 at offset 0 (index 12)
          "lw %t6,0x30(%t4);" //load value from %t4 at offset 0
          "madd16 %t5,%t6;"
          //".word 0x01AE0028;"    //macc %t5, %t6

          "lw %t5,0x34(%t3);" //load value from %t3 at offset 4 (index 13)
          "sub %t5,zero,%t5;" //negate %t5
          "lw %t6,0x34(%t4);" //load value from %t4 at offset 4
          "madd16 %t5,%t6;"
          //".word 0x01AE0028;"    //macc %t5, %t6

          "lw %t5,0x38(%t3);" //load value from %t3 at offset 8 (index 14)
          "lw %t6,0x38(%t4);" //load value from %t4 at offset 8
          "madd16 %t5,%t6;"
          //".word 0x01AE0028;" //macc %t5, %t6

          "lw %t5,0x3C(%t3);" //load value from %t3 at offset 12 (index 15)
          "sub %t5,zero,%t5;" //negate %t5
          "lw %t6,0x3C(%t4);" //load value from %t4 at offset 12
          "madd16 %t5,%t6;"
          //".word 0x01AE0028;"    //macc %t5, %t6

          "mflo %t1;"
          "srl %t1,%t1,15;" //FRACBITS
          "mfhi %t2;"
          "sll %t2,%t2,32-15;" //FRACBITS
          "or %t2,%t2,%t1;"
          "sw %t2,0(%0)",
          &res);

    return res;
}
```

This allowed us to have the fastest decoders in the industry.
