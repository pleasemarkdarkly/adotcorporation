//****************************************************************************
//
// _DMUL.S - Routine to multiply two doubles.
//
// Copyright (c) 1999,2000,2001 Cirrus Logic, Inc.
//
//****************************************************************************

#include "../../asmdefs.h"

//****************************************************************************
//
// Read-only code area.
//
//****************************************************************************
    _TEXT_

//****************************************************************************
//
// Multiply two doubles.
//
//****************************************************************************
    _EXPORT_ _dmul
_dmul _LABEL_
    stmdb   r13!, {r4-r9, r11, r14}
    movs    r12, r2, lsl _CONST_ 1
    teqeq   r3, _CONST_ 0
    mov     r8, r12, lsl _CONST_ 10
    mov     r2, r12, lsr _CONST_ 20
    mov     r5, r3, lsl _CONST_ 11
    orr     r4, r8, r3, lsr _CONST_ 21
    addne   r2, r2, _CONST_ 0x00007800
    mov     r3, r2, RRX
    orrne   r4, r4, _CONST_ 0x80000000
    movs    r12, r12, asr _CONST_ 21
    addeq   r14, pc, _CONST_ 8
    beq     __fp_norm_op2
    cmn     r12, _CONST_ 1
    beq     dmul_uncommon1
    movs    r12, r0, lsl _CONST_ 1
    teqeq   r1, _CONST_ 0
    mov     r8, r12, lsl _CONST_ 10
    mov     r0, r12, lsr _CONST_ 20
    mov     r2, r1, lsl _CONST_ 11
    orr     r1, r8, r1, lsr _CONST_ 21
    addne   r0, r0, _CONST_ 0x00007800
    mov     r0, r0, RRX
    orrne   r1, r1, _CONST_ 0x80000000
    movs    r12, r12, asr _CONST_ 21
    addeq   r14, pc, _CONST_ 8
    beq     __fp_norm_op1
    cmn     r12, _CONST_ 1
    beq     dmul_uncommon
    bl      __fp_mult_common
dmul_return _LABEL_
    bl      __fp_e2d
    tst     r3, _CONST_ 0x20000000
    ldmeqia r13!, {r4-r9, r11, pc}
    orr     r3, r3, _CONST_ 0x00040000
    ldmia   r13!, {r4-r9, r11, r14}
dmul_uncommon1 _LABEL_
    orr     r3, r3, _CONST_ 0x40000000
    movs    r12, r0, lsl _CONST_ 1
    teqeq   r1, _CONST_ 0
    mov     r8, r12, lsl _CONST_ 10
    mov     r0, r12, lsr _CONST_ 20
    mov     r2, r1, lsl _CONST_ 11
    orr     r1, r8, r1, lsr _CONST_ 21
    addne   r0, r0, _CONST_ 0x00007800
    mov     r0, r0, RRX
    orrne   r1, r1, _CONST_ 0x80000000
    movs    r12, r12, asr _CONST_ 21
    addeq   r14, pc, _CONST_ 8
    beq     __fp_norm_op1
    cmn     r12, _CONST_ 1
dmul_uncommon _LABEL_
    orreq   r0, r0, _CONST_ 0x40000000
    adr     r14, dmul_return
    mov     r11, _CONST_ 2
    b       __fp_mult_uncommon
__fp_norm_op1 _LABEL_
    tst     r1, _CONST_ 0x80000000
    moveq   pc, r14
    stmdb   r13!, {r3, r14}
    bics    r1, r1, _CONST_ 0x80000000
    beq     __fp_denorm1_low
    movs    r3, r1, lsr _CONST_ 16
    moveq   r1, r1, lsl _CONST_ 16
    moveq   r12, _CONST_ 0x10
    movne   r12, _CONST_ 0
    movs    r3, r1, lsr _CONST_ 24
    moveq   r1, r1, lsl _CONST_ 8
    addeq   r12, r12, _CONST_ 8
    movs    r3, r1, lsr _CONST_ 28
    moveq   r1, r1, lsl _CONST_ 4
    addeq   r12, r12, _CONST_ 4
    movs    r3, r1, lsr _CONST_ 30
    moveq   r1, r1, lsl _CONST_ 2
    addeq   r12, r12, _CONST_ 2
    movs    r3, r1, lsr _CONST_ 31
    moveq   r1, r1, lsl _CONST_ 1
    addeq   r12, r12, _CONST_ 1
    rsb     r3, r12, _CONST_ 0x20
    orr     r1, r1, r2, lsr r3
    mov     r2, r2, lsl r12
    sub     r0, r0, r12
    add     r0, r0, _CONST_ 1
    ldmia   r13!, {r3, pc}
__fp_denorm1_low _LABEL_
    movs    r3, r2, lsr _CONST_ 16
    moveq   r2, r2, lsl _CONST_ 16
    moveq   r12, _CONST_ 0x10
    movne   r12, _CONST_ 0
    movs    r3, r2, lsr _CONST_ 24
    moveq   r2, r2, lsl _CONST_ 8
    addeq   r12, r12, _CONST_ 8
    movs    r3, r2, lsr _CONST_ 28
    moveq   r2, r2, lsl _CONST_ 4
    addeq   r12, r12, _CONST_ 4
    movs    r3, r2, lsr _CONST_ 30
    moveq   r2, r2, lsl _CONST_ 2
    addeq   r12, r12, _CONST_ 2
    movs    r3, r2, lsr _CONST_ 31
    moveq   r2, r2, lsl _CONST_ 1
    addeq   r12, r12, _CONST_ 1
    mov     r1, r2
    mov     r2, _CONST_ 0
    sub     r0, r0, _CONST_ 0x1f
    sub     r0, r0, r12
    ldmia   r13!, {r3, pc}
__fp_norm_op2 _LABEL_
    tst     r4, _CONST_ 0x80000000
    moveq   pc, r14
    stmdb   r13!, {r0, r14}
    bics    r4, r4, _CONST_ 0x80000000
    beq     __fp_denorm2_low
    movs    r0, r4, lsr _CONST_ 16
    moveq   r4, r4, lsl _CONST_ 16
    moveq   r12, _CONST_ 0x10
    movne   r12, _CONST_ 0
    movs    r0, r4, lsr _CONST_ 24
    moveq   r4, r4, lsl _CONST_ 8
    addeq   r12, r12, _CONST_ 8
    movs    r0, r4, lsr _CONST_ 28
    moveq   r4, r4, lsl _CONST_ 4
    addeq   r12, r12, _CONST_ 4
    movs    r0, r4, lsr _CONST_ 30
    moveq   r4, r4, lsl _CONST_ 2
    addeq   r12, r12, _CONST_ 2
    movs    r0, r4, lsr _CONST_ 31
    moveq   r4, r4, lsl _CONST_ 1
    addeq   r12, r12, _CONST_ 1
    rsb     r0, r12, _CONST_ 0x20
    orr     r4, r4, r5, lsr r0
    mov     r5, r5, lsl r12
    sub     r3, r3, r12
    add     r3, r3, _CONST_ 1
    ldmia   r13!, {r0, pc}
__fp_denorm2_low _LABEL_
    movs    r0, r5, lsr _CONST_ 16
    moveq   r5, r5, lsl _CONST_ 16
    moveq   r12, _CONST_ 0x10
    movne   r12, _CONST_ 0
    movs    r0, r5, lsr _CONST_ 24
    moveq   r5, r5, lsl _CONST_ 8
    addeq   r12, r12, _CONST_ 8
    movs    r0, r5, lsr _CONST_ 28
    moveq   r5, r5, lsl _CONST_ 4
    addeq   r12, r12, _CONST_ 4
    movs    r0, r5, lsr _CONST_ 30
    moveq   r5, r5, lsl _CONST_ 2
    addeq   r12, r12, _CONST_ 2
    movs    r0, r5, lsr _CONST_ 31
    moveq   r5, r5, lsl _CONST_ 1
    addeq   r12, r12, _CONST_ 1
    mov     r4, r5
    mov     r5, _CONST_ 0
    sub     r3, r3, _CONST_ 0x1f
    sub     r3, r3, r12
    ldmia   r13!, {r0, pc}
__fp_mult_common _LABEL_
    bic      r8, r0, _CONST_ 0xc0000000
    bic      r9, r3, _CONST_ 0xc0000000
    eor      r0, r0, r3
    and      r0, r0, _CONST_ 0x80000000
    add      r3, r8, r9
    sub      r3, r3, _CONST_ 0x3f00
    sub      r3, r3, _CONST_ 0xfe
Mult_Mantissas _LABEL_
    teq      r2, _CONST_ 0
    beq      Mult_32xX
Mult_64xX _LABEL_
    teq      r5, _CONST_ 0
    beq      Mult_64x32
Mult_64x64 _LABEL_
    stmdb    r13!, {r0, r7, r11, r14}
    mov      r0, r1, lsr _CONST_ 16
    bic      r7, r1, r0, lsl _CONST_ 16
    mov      r6, r4, lsr _CONST_ 16
    bic      r8, r4, r6, lsl _CONST_ 16
    mul      r9, r0, r6
    mul      r6, r7, r6
    mul      r7, r8, r7
    adds     r7, r7, r6, lsl _CONST_ 16
    adc      r9, r9, r6, lsr _CONST_ 16
    mul      r8, r0, r8
    adds     r7, r7, r8, lsl _CONST_ 16
    adc      r0, r9, r8, lsr _CONST_ 16
    mov      r11, r2, lsr _CONST_ 16
    bic      r14, r2, r11, lsl _CONST_ 16
    mov      r6, r5, lsr _CONST_ 16
    bic      r8, r5, r6, lsl _CONST_ 16
    mul      r9, r11, r6
    mul      r6, r14, r6
    mul      r14, r8, r14
    adds     r14, r14, r6, lsl _CONST_ 16
    adc      r9, r9, r6, lsr _CONST_ 16
    mul      r8, r11, r8
    adds     r14, r14, r8, lsl _CONST_ 16
    adc      r11, r9, r8, lsr _CONST_ 16
    adds     r7, r7, r11
    adc      r0, r0, _CONST_ 0
    adds     r11, r7, r14
    adcs     r7, r7, r0
    adc      r0, r0, _CONST_ 0
    subs     r8, r1, r2
    mov      r1, _CONST_ 0
    mov      r6, _CONST_ 0
    mvncc    r1, r1
    subcc    r6, r4, r5
    subnes   r9, r5, r4
    moveq    r1, _CONST_ 0
    mvncc    r1, r1
    subcc    r6, r6, r8
    mov      r4, r8, lsr _CONST_ 16
    bic      r5, r8, r4, lsl _CONST_ 16
    mov      r8, r9, lsr _CONST_ 16
    bic      r9, r9, r8, lsl _CONST_ 16
    mla      r2, r4, r8, r6
    mul      r8, r5, r8
    mul      r6, r9, r5
    adds     r6, r6, r8, lsl _CONST_ 16
    adc      r2, r2, r8, lsr _CONST_ 16
    mul      r9, r4, r9
    adds     r6, r6, r9, lsl _CONST_ 16
    adc      r2, r2, r9, lsr _CONST_ 16
    adds     r6, r11, r6
    adcs     r2, r7, r2
    adcs     r1, r0, r1
    orr      r14, r14, r14, lsl _CONST_ 2
    orr      r6, r6, r14, lsr _CONST_ 2
    ldmmiia  r13!, {r0, r7, r11, pc}
    adds     r6, r6, r6
    adcs     r2, r2, r2
    adc      r1, r1, r1
    sub      r3, r3, _CONST_ 1
    ldmia    r13!, {r0, r7, r11, pc}
Mult_64x32 _LABEL_
    mov      r5, r4, lsr _CONST_ 16
    bic      r6, r4, r5, lsl _CONST_ 16
    mov      r8, r1, lsr _CONST_ 16
    bic      r9, r1, r8, lsl _CONST_ 16
    mul      r4, r5, r8
    mul      r8, r6, r8
    mul      r1, r9, r6
    adds     r1, r1, r8, lsl _CONST_ 16
    adc      r4, r4, r8, lsr _CONST_ 16
    mul      r9, r5, r9
    adds     r1, r1, r9, lsl _CONST_ 16
    adc      r4, r4, r9, lsr _CONST_ 16
    mov      r8, r2, lsr _CONST_ 16
    bic      r9, r2, r8, lsl _CONST_ 16
    mul      r2, r5, r8
    mul      r8, r6, r8
    mul      r6, r9, r6
    adds     r6, r6, r8, lsl _CONST_ 16
    adc      r2, r2, r8, lsr _CONST_ 16
    mul      r9, r5, r9
    adds     r6, r6, r9, lsl _CONST_ 16
    adc      r5, r2, r9, lsr _CONST_ 16
    adds     r2, r5, r1
    adcs     r1, r4, _CONST_ 0
    movmi    pc, r14
    adds     r6, r6, r6
    adcs     r2, r2, r2
    adc      r1, r1, r1
    sub      r3, r3, _CONST_ 1
    mov      pc, r14
Mult_32xX _LABEL_
    teq      r5, _CONST_ 0
    beq      Mult_32x32
Mult_32x64 _LABEL_
    mov      r2, r1, lsr _CONST_ 16
    bic      r6, r1, r2, lsl _CONST_ 16
    mov      r8, r4, lsr _CONST_ 16
    bic      r9, r4, r8, lsl _CONST_ 16
    mul      r1, r2, r8
    mul      r8, r6, r8
    mul      r4, r9, r6
    adds     r4, r4, r8, lsl _CONST_ 16
    adc      r1, r1, r8, lsr _CONST_ 16
    mul      r9, r2, r9
    adds     r4, r4, r9, lsl _CONST_ 16
    adc      r1, r1, r9, lsr _CONST_ 16
    mov      r8, r5, lsr _CONST_ 16
    bic      r9, r5, r8, lsl _CONST_ 16
    mul      r5, r2, r8
    mul      r8, r6, r8
    mul      r6, r9, r6
    adds     r6, r6, r8, lsl _CONST_ 16
    adc      r5, r5, r8, lsr _CONST_ 16
    mul      r9, r2, r9
    adds     r6, r6, r9, lsl _CONST_ 16
    adc      r2, r5, r9, lsr _CONST_ 16
    adds     r2, r2, r4
    adcs     r1, r1, _CONST_ 0
    movmi    pc, r14
    adds     r6, r6, r6
    adcs     r2, r2, r2
    adc      r1, r1, r1
    sub      r3, r3, _CONST_ 1
    mov      pc, r14
__fp_mult_fast_common _LABEL_
    bic      r8, r0, _CONST_ 0xc0000000
    bic      r9, r3, _CONST_ 0xc0000000
    eor      r0, r0, r3
    and      r0, r0, _CONST_ 0x80000000
    add      r3, r8, r9
    sub      r3, r3, _CONST_ 0x3f00
    sub      r3, r3, _CONST_ 0xfe
Mult_32x32 _LABEL_
    mov      r5, r4, lsr _CONST_ 16
    bic      r6, r4, r5, lsl _CONST_ 16
    mov      r8, r1, lsr _CONST_ 16
    bic      r9, r1, r8, lsl _CONST_ 16
    mul      r1, r5, r8
    mul      r8, r6, r8
    mul      r2, r9, r6
    adds     r2, r2, r8, lsl _CONST_ 16
    adc      r1, r1, r8, lsr _CONST_ 16
    mul      r9, r5, r9
    adds     r2, r2, r9, lsl _CONST_ 16
    adcs     r1, r1, r9, lsr _CONST_ 16
    mov      r6, _CONST_ 0
    movmi    pc, r14
    adds     r2, r2, r2
    adc      r1, r1, r1
    sub      r3, r3, _CONST_ 1
    mov      pc, r14
__fp_mult_uncommon _LABEL_
    mov      r9, r3, lsl _CONST_ 17
    cmn      r9, _CONST_ 0x20000
    andcc    r9, r9, r4
    ands     r9, r9, r3, lsl _CONST_ 1
    mov      r8, r0, lsl _CONST_ 17
    cmn      r8, _CONST_ 0x20000
    andcc    r8, r8, r1
    ands     r8, r8, r0, lsl _CONST_ 1
    bmi      Mult_NaNInf1
    tst      r9, _CONST_ 0x80000000
    bne      Mult_NaNInf2Only
    orrs     r8, r1, r2
    orrnes   r8, r4, r5
    beq      Mult_Zero
    ands     r6, r1, r0, lsl _CONST_ 1
    bicmi    r1, r1, _CONST_ 0x80000000
    addmi    r0, r0, _CONST_ 1
    ands     r6, r4, r3, lsl _CONST_ 1
    bicmi    r4, r4, _CONST_ 0x80000000
    addmi    r3, r3, _CONST_ 1
    bic      r8, r0, _CONST_ 0xc0000000
    bic      r9, r3, _CONST_ 0xc0000000
    eor      r0, r0, r3
    and      r0, r0, _CONST_ 0x80000000
    add      r3, r8, r9
    sub      r3, r3, _CONST_ 0x3f00
    sub      r3, r3, _CONST_ 0xfe
    stmdb    r13!, {r14}
    tst      r1, _CONST_ 0x80000000
    bleq     __fp_normalise_op1
    tst      r4, _CONST_ 0x80000000
    bleq     __fp_normalise_op2
    ldmia    r13!, {r14}
    b        Mult_Mantissas
Mult_Zero _LABEL_
    eor      r0, r0, r3
    and      r0, r0, _CONST_ 0x80000000
    mov      r1, _CONST_ 0
    mov      r2, _CONST_ 0
    mov      r3, _CONST_ 0
    mov      r6, _CONST_ 0
    mov      pc, r14
Mult_NaNInf1 _LABEL_
    tst      r9, _CONST_ 0x80000000
    beq      Mult_NanInf1Only
    orr      r8, r2, r1, lsl _CONST_ 1
    orr      r8, r8, r5
    orrs     r8, r8, r4, lsl _CONST_ 1
    bne      __fp_convert_NaNs
Mult_InfShared _LABEL_
    eor      r8, r0, r3
    and      r8, r8, _CONST_ 0x80000000
    adr      r0,  Prototype_Infinity
    ldmia    r0, {r0-r2}
    orr      r0, r0, r8
    mov      pc, r14
Mult_NanInf1Only _LABEL_
    orrs     r8, r2, r1, lsl _CONST_ 1
    bne      __fp_convert_NaN_1Of2
    orrs     r8, r4, r5
    bne      Mult_InfShared
    orr      r0, r0, _CONST_ 0x61000000
    mov      pc, r14
Mult_NaNInf2Only _LABEL_
    orrs     r8, r5, r4, lsl _CONST_ 1
    bne      __fp_convert_NaN_2Of2
    orrs     r8, r1, r2
    bne      Mult_InfShared
    orr      r0, r0, _CONST_ 0x61000000
    mov      pc, r14
Prototype_Infinity _LABEL_
    _WORD_   0x40007fff
    _WORD_   0x00000000
    _WORD_   0x00000000
__fp_normalise_op1 _LABEL_
    teq      r1, _CONST_ 0
    moveq    r1, r2
    moveq    r2, _CONST_ 0
    subeq    r3, r3, _CONST_ 0x20
    mov      r8, _CONST_ 0
    movs     r9, r1, lsr _CONST_ 16
    moveq    r1, r1, lsl _CONST_ 16
    addeq    r8, r8, _CONST_ 0x10
    movs     r9, r1, lsr _CONST_ 24
    moveq    r1, r1, lsl _CONST_ 8
    addeq    r8, r8, _CONST_ 8
    movs     r9, r1, lsr _CONST_ 28
    moveq    r1, r1, lsl _CONST_ 4
    addeq    r8, r8, _CONST_ 4
    movs     r9, r1, lsr _CONST_ 30
    moveq    r1, r1, lsl _CONST_ 2
    addeq    r8, r8, _CONST_ 2
    movs     r9, r1, lsr _CONST_ 31
    moveq    r1, r1, lsl _CONST_ 1
    addeq    r8, r8, _CONST_ 1
    rsbs     r9, r8, _CONST_ 0x20
    orr      r1, r1, r2, lsr r9
    mov      r2, r2, lsl r8
    sub      r3, r3, r8
    mov      pc, r14
__fp_normalise_op2 _LABEL_
    teq      r4, _CONST_ 0
    moveq    r4, r5
    moveq    r5, _CONST_ 0
    subeq    r3, r3, _CONST_ 0x20
    mov      r8, _CONST_ 0
    movs     r9, r4, lsr _CONST_ 16
    moveq    r4, r4, lsl _CONST_ 16
    addeq    r8, r8, _CONST_ 0x10
    movs     r9, r4, lsr _CONST_ 24
    moveq    r4, r4, lsl _CONST_ 8
    addeq    r8, r8, _CONST_ 8
    movs     r9, r4, lsr _CONST_ 28
    moveq    r4, r4, lsl _CONST_ 4
    addeq    r8, r8, _CONST_ 4
    movs     r9, r4, lsr _CONST_ 30
    moveq    r4, r4, lsl _CONST_ 2
    addeq    r8, r8, _CONST_ 2
    movs     r9, r4, lsr _CONST_ 31
    moveq    r4, r4, lsl _CONST_ 1
    addeq    r8, r8, _CONST_ 1
    rsbs     r9, r8, _CONST_ 0x20
    orr      r4, r4, r5, lsr r9
    mov      r5, r5, lsl r8
    sub      r3, r3, r8
    mov      pc, r14
__fp_convert_NaNs _LABEL_
    orrs     r8, r2, r1, lsl _CONST_ 1
    beq      __fp_convert_NaN_2Of2
    tst      r1, _CONST_ 0x40000000
    beq      __fp_convert_NaN_1Of2
    orrs     r8, r5, r4, lsl _CONST_ 1
    beq      __fp_convert_NaN_1Of2
    tst      r4, _CONST_ 0x40000000
    bne      __fp_convert_NaN_1Of2
__fp_convert_NaN_2Of2 _LABEL_
    tst      r4, _CONST_ 0x40000000
    beq      ReturnIVO
    mov      r0, r3
    mov      r1, r4
    mov      r2, r5
    b        ConvertNaN1_NoTrap
__fp_convert_NaN1 _LABEL_
    tst      r1, _CONST_ 0x40000000
    beq      ReturnIVO
    b        ConvertNaN1_NoTrap
__fp_convert_NaN_1Of2 _LABEL_
    tst      r1, _CONST_ 0x40000000
    beq      ReturnIVO
ConvertNaN1_NoTrap _LABEL_
    tst      r11, _CONST_ 1
    bne      ConvertNaN1_ToSingle
    tst      r11, _CONST_ 2
    bne      ConvertNaN1_ToDouble
    tst      r11, _CONST_ 0x200
    bne      ConvertNaN1_ToExtended_MantissaDone
    bic      r8, r0, _CONST_ 0xc0000000
    mov      r9, _CONST_ 0xff
    orr      r9, r9, _CONST_ 0x4300
    cmp      r8, r9
    biccc    r2, r2, _CONST_ 1
    orreq    r2, r2, _CONST_ 1
ConvertNaN1_ToExtended_MantissaDone _LABEL_
    and      r0, r0, _CONST_ 0xc0000000
    orr      r0, r0, _CONST_ 0xff
    orr      r0, r0, _CONST_ 0x7f00
    teq      r0, r0
    mov      pc, r14
ConvertNaN1_ToDouble _LABEL_
    and      r0, r0, _CONST_ 0xc0000000
    orr      r0, r0, _CONST_ 0xff
    orr      r0, r0, _CONST_ 0x4300
    mov      r2, r2, lsr _CONST_ 11
    mov      r2, r2, lsl _CONST_ 11
    orr      r1, r1, _CONST_ 0x80000000
    teq      r0, r0
    mov      pc, r14
ConvertNaN1_ToSingle _LABEL_
    and      r0, r0, _CONST_ 0xc0000000
    orr      r0, r0, _CONST_ 0x7f
    orr      r0, r0, _CONST_ 0x4000
    mov      r2, _CONST_ 0
    bic      r1, r1, _CONST_ 0xff
    orr      r1, r1, _CONST_ 0x80000000
    mov      pc, r14
ReturnIVO _LABEL_
    orr      r0, r0, _CONST_ 0x61000000
    mov      pc, r14
__fp_e2d _LABEL_
    bics     r12, r1, r0, lsl _CONST_ 1
    bpl      _e2d_SpecialCase
    subs     r3, r3, _CONST_ 0x3c00
    bmi      _e2d_ExpUnderflow
    addne    r12, r3, _CONST_ 1
    cmpne    r12, _CONST_ 0x800
    bge      _e2d_ExpOverflow
    movs     r12, r2, lsr _CONST_ 11
    bcs      _e2d_NeedsRounding
    orr      r0, r0, r3, lsl _CONST_ 20
    bic      r1, r1, _CONST_ 0x80000000
    orr      r0, r0, r1, lsr _CONST_ 11
    orr      r1, r12, r1, lsl _CONST_ 21
    mov      r3, _CONST_ 0
    mov      pc, r14
_e2d_SpecialCase _LABEL_
    tst      r0, _CONST_ 0x40000000
    bne      _e2d_Uncommon
_e2d_UnitsBit _LABEL_
    and      r0, r0, _CONST_ 0x80000000
    mov      r1, _CONST_ 0
    mov      r3, _CONST_ 0
    mov      pc, r14
_e2d_ExpOverflow _LABEL_
    teq      r3, _CONST_ 0
    beq      _e2d_ExpUnderflow
    and      r0, r0, _CONST_ 0x80000000
    orr      r0, r0, _CONST_ 0x70000000
    orr      r0, r0, _CONST_ 0xff00000
    mov      r1, _CONST_ 0
    mov      r3, _CONST_ 0x64000000
    mov      pc, r14
_e2d_ExpUnderflow _LABEL_
    adds     r3, r3, _CONST_ 0x34
    beq      _e2d_ExpJustUnderflow
    andmi    r0, r0, _CONST_ 0x80000000
    movmi    r1, _CONST_ 0
    movmi    r3, _CONST_ 0
    movmi    pc, r14
    subs     r3, r3, _CONST_ 0x20
    beq      _e2d_Underflow32
    blt      _e2d_ShortUnderflow
    movs     r12, r2, lsl r3
    bmi      _e2d_LongUnderflowNeedsRounding
    rsb      r12, r3, _CONST_ 0x20
    orr      r0, r0, r1, lsr r12
    mov      r2, r2, lsr r12
    orr      r1, r2, r1, lsl r3
    mov      r3, _CONST_ 0
    mov      pc, r14
_e2d_LongUnderflowNeedsRounding _LABEL_
    orrs     r12, r6, r12, lsl _CONST_ 1
    rsb      r12, r3, _CONST_ 0x20
    orr      r0, r0, r1, lsr r12
    mov      r2, r2, lsr r12
    orr      r1, r2, r1, lsl r3
    mov      r3, _CONST_ 0
    tsteq    r1, _CONST_ 1
    moveq    pc, r14
    adds     r1, r1, _CONST_ 1
    addcs    r0, r0, _CONST_ 1
    mov      pc, r14
_e2d_Underflow32 _LABEL_
    mov      r3, _CONST_ 0
    teq      r2, _CONST_ 0x80000000
    movmi    pc, r14
    tsteq    r1, _CONST_ 1
    moveq    pc, r14
    adds     r1, r1, _CONST_ 1
    addcs    r0, r0, _CONST_ 1
    mov      pc, r14
_e2d_ShortUnderflow _LABEL_
    add      r3, r3, _CONST_ 0x20
    movs     r12, r1, lsl r3
    bmi      _e2d_ShortUnderflowNeedsRounding
    rsb      r12, r3, _CONST_ 0x20
    mov      r1, r1, lsr r12
    and      r0, r0, _CONST_ 0x80000000
    mov      r3, _CONST_ 0
    mov      pc, r14
_e2d_ShortUnderflowNeedsRounding _LABEL_
    teq      r12, _CONST_ 0x80000000
    orreqs   r12, r2, r6
    rsb      r12, r3, _CONST_ 0x20
    mov      r1, r1, lsr r12
    tsteq    r1, _CONST_ 1
    addne    r1, r1, _CONST_ 1
    mov      r3, _CONST_ 0
    mov      pc, r14
_e2d_ExpJustUnderflow _LABEL_
    teq      r1, _CONST_ 0x80000000
    teqeq    r2, _CONST_ 0
    moveq    r1, _CONST_ 0
    movne    r1, _CONST_ 1
    mov      r3, _CONST_ 0
    mov      pc, r14
_e2d_Uncommon _LABEL_
    tst      r0, _CONST_ 0x20000000
    bne      _e2d_Error
    mov      r3, _CONST_ 0
    teq      r1, _CONST_ 0
    movnes   r1, r1, lsl _CONST_ 1
    mvnmi    r0, _CONST_ 0
    movmi    pc, r14
    teqeq    r2, _CONST_ 0
    movne    r3, _CONST_ 0x61000000
    movne    pc, r14
    and      r0, r0, _CONST_ 0x80000000
    orr      r0, r0, _CONST_ 0x70000000
    orr      r0, r0, _CONST_ 0xff00000
    mov      pc, r14
_e2d_Error _LABEL_
    mov      r3, r0
    mov      pc, r14
_e2d_NeedsRounding _LABEL_
    bic      r1, r1, _CONST_ 0x80000000
    mov      r12, r2, lsl _CONST_ 22
    orrs     r12, r12, r6
    and      r12, r0, _CONST_ 0x80000000
    mov      r0, r3, lsl _CONST_ 20
    orr      r0, r0, r1, lsr _CONST_ 11
    mov      r1, r1, lsl _CONST_ 21
    orr      r1, r1, r2, lsr _CONST_ 11
    mov      r3, _CONST_ 0
    beq      _e2d_RoundToEven
    adds     r1, r1, _CONST_ 1
    addcs    r0, r0, _CONST_ 1
    adds     r2, r0, _CONST_ 0x100000
    orr      r0, r0, r12
    movpl    pc, r14
    mov      r3, _CONST_ 0x64000000
    mov      pc, r14
_e2d_RoundToEven _LABEL_
    tsteq    r1, _CONST_ 1
    orreq    r0, r0, r12
    moveq    pc, r14
    adds     r1, r1, _CONST_ 1
    addcs    r0, r0, _CONST_ 1
    adds     r2, r0, _CONST_ 0x100000
    orr      r0, r0, r12
    movpl    pc, r14
    mov      r3, _CONST_ 0x64000000
    mov      pc, r14

    _END_
