;*****************************************************************************
;
; SUPPORT.S - Routines used by the C compiler to perform various C functions
;             which are not directly supported by the ARM processor (such as
;             division).
;
; Copyright (c) 1999-2000 Cirrus Logic, Inc.
;
;*****************************************************************************

;*****************************************************************************
;
; Read-only code area.  These routines were shamelessly stolen from the ARM
; C library source code and the "fast" versions were chosen instead of the
; "memory efficient" versions.
;
;*****************************************************************************
    AREA    |Assembly$$code|, CODE, READONLY

;*****************************************************************************
;
; 32-bit signed division.
;
;*****************************************************************************
    EXPORT  __rt_sdiv
__rt_sdiv
    ands    r2, r0, #1 << 31
    rsbmi   r0, r0, #0
    eors    r3, r2, r1, asr #32
    rsbcs   r1, r1, #0
    rsbs    r12, r0, r1, lsr #3
    blo     %F03
    rsbs    r12, r0, r1, lsr #8
    blo     %F01
    mov     r0, r0, lsl #8
    orr     r2, r2, #255 << 24
    rsbs    r12, r0, r1, lsr #4
    blo     %F02
    rsbs    r12, r0, r1, lsr #8
    blo     %F01
    mov     r0, r0, lsl #8
    orr     r2, r2, #255 << 16
    rsbs    r12, r0, r1, lsr #8
    movcs   r0, r0, lsl #8
    orrcs   r2, r2, #255 << 8
    rsbs    r12, r0, r1, lsr #4
    blo     %F02
    rsbs    r12, r0, #0
    movcs   pc, r14
00
    movcs   r0, r0, lsr #8
01
    rsbs    r12, r0, r1, lsr #7
    subcs   r1, r1, r0, lsl #7
    adc     r2, r2, r2
    rsbs    r12, r0, r1, lsr #6
    subcs   r1, r1, r0, lsl #6
    adc     r2, r2, r2
    rsbs    r12, r0, r1, lsr #5
    subcs   r1, r1, r0, lsl #5
    adc     r2, r2, r2
    rsbs    r12, r0, r1, lsr #4
    subcs   r1, r1, r0, lsl #4
    adc     r2, r2, r2
02
    rsbs    r12, r0, r1, lsr #3
    subcs   r1, r1, r0, lsl #3
    adc     r2, r2, r2
03
    rsbs    r12, r0, r1, lsr #2
    subcs   r1, r1, r0, lsl #2
    adc     r2, r2, r2
    rsbs    r12, r0, r1, lsr #1
    subcs   r1, r1, r0, lsl #1
    adc     r2, r2, r2
    rsbs    r12, r0, r1
    subcs   r1, r1, r0
    adcs    r2, r2, r2
    bcs     %B00
    eors    r0, r2, r3, asr #31
    add     r0, r0, r3, lsr #31
    rsbcs   r1, r1, #0
    mov     pc, r14

;*****************************************************************************
;
; 32-bit unsigned division.
;
;*****************************************************************************
    EXPORT  __rt_udiv
__rt_udiv
    mov     r2, #0
    rsbs    r12, r0, r1, lsr #3
    blo     %F03
    rsbs    r12, r0, r1, lsr #8
    blo     %F01
    mov     r0, r0, lsl #8
    orr     r2, r2, #255 << 24
    rsbs    r12, r0, r1, lsr #4
    blo     %F02
    rsbs    r12, r0, r1, lsr #8
    blo     %F01
    mov     r0, r0, lsl #8
    orr     r2, r2, #255 << 16
    rsbs    r12, r0, r1, lsr #8
    movcs   r0, r0, lsl #8
    orrcs   r2, r2, #255 << 8
    rsbs    r12, r0, r1, lsr #4
    blo     %F02
    rsbs    r12, r0, #0
    movcs   pc, r14
00
    movcs   r0, r0, lsr #8
01
    rsbs    r12, r0, r1, lsr #7
    subcs   r1, r1, r0, lsl #7
    adc     r2, r2, r2
    rsbs    r12, r0, r1, lsr #6
    subcs   r1, r1, r0, lsl #6
    adc     r2, r2, r2
    rsbs    r12, r0, r1, lsr #5
    subcs   r1, r1, r0, lsl #5
    adc     r2, r2, r2
    rsbs    r12, r0, r1, lsr #4
    subcs   r1, r1, r0, lsl #4
    adc     r2, r2, r2
02
    rsbs    r12, r0, r1, lsr #3
    subcs   r1, r1, r0, lsl #3
    adc     r2, r2, r2
03
    rsbs    r12, r0, r1, lsr #2
    subcs   r1, r1, r0, lsl #2
    adc     r2, r2, r2
    rsbs    r12, r0, r1, lsr #1
    subcs   r1, r1, r0, lsl #1
    adc     r2, r2, r2
    rsbs    r12, r0, r1
    subcs   r1, r1, r0
    adcs    r2, r2, r2
    bcs     %B00
    mov     r0, r2
    mov     pc, r14

;*****************************************************************************
;
; 32-bit signed division by 10.
;
;*****************************************************************************
    EXPORT  __rt_sdiv10
__rt_sdiv10
    movs    r3, r0
    rsbmi   r0, r0, #0
    sub     r1, r0, #10
    sub     r0, r0, r0, lsr #2
    add     r0, r0, r0, lsr #4
    add     r0, r0, r0, lsr #8
    add     r0, r0, r0, lsr #16
    mov     r0, r0, lsr #3
    add     r2, r0, r0, asl #2
    subs    r1, r1, r2, asl #1
    addpl   r0, r0, #1
    addmi   r1, r1, #10
    movs    r3, r3
    rsbmi   r0, r0, #0
    rsbmi   r1, r1, #0
    mov     pc, r14

;*****************************************************************************
;
; 32-bit unsigned division by 10.
;
;*****************************************************************************
    EXPORT  __rt_udiv10
__rt_udiv10
    sub     r1, r0, #10
    sub     r0, r0, r0, lsr #2
    add     r0, r0, r0, lsr #4
    add     r0, r0, r0, lsr #8
    add     r0, r0, r0, lsr #16
    mov     r0, r0, lsr #3
    add     r2, r0, r0, asl #2
    subs    r1, r1, r2, asl #1
    addpl   r0, r0, #1
    addmi   r1, r1, #10
    mov     pc, r14

;*****************************************************************************
;
; Division test.  This is used in cases where a division is performed but the
; result is never used in the code.  Therefore, simply do not perform any
; test.
;*****************************************************************************
    EXPORT  __rt_divtest
__rt_divtest
    mov     pc, r14

;*****************************************************************************
;
; Word aligned source and destination memory copy (used to initialize
; automatics).
;
;*****************************************************************************
    EXPORT  _memcpy
_memcpy
    stmdb   r13!, {r4, r14}
    subs    r2, r2, #32
loop
    ldmgeia r1!, {r3, r4, r12, r14}
    stmgeia r0!, {r3, r4, r12, r14}
    ldmgeia r1!, {r3, r4, r12, r14}
    stmgeia r0!, {r3, r4, r12, r14}
    subges  r2, r2, #32
    bge     loop
    adds    r2, r2, #16
    ldmgeia r1!, {r3, r4, r12, r14}
    stmgeia r0!, {r3, r4, r12, r14}
    movs    r2, r2, lsl #29
    ldmcsia r1!, {r3, r4}
    stmcsia r0!, {r3, r4}
    ldrmi   r3, [r1]
    strmi   r3, [r0]
    ldmia   r13!, {r4, pc}

;*****************************************************************************
;
; Standard C library function "memset".
;
;*****************************************************************************
    EXPORT  memset
memset
    stmdb   r13!, {r0, r14}
    subs    r2, r2, #4
    bmi     TrailingBytes
    ands    r12, r0, #3
    bne     AlignDst
DstAligned
    and     r1, r1, #&ff
    orr     r1, r1, r1, ASL #8
    orr     r1, r1, r1, ASL #16
    mov     r3, r1
    mov     r12, r1
    mov     r14, r1
    subs    r2, r2, #12-4
    blt     TrailingWords
    subs    r2, r2, #32-12
    blt     %F1
0
    stmia   r0!, {r1, r3, r12, r14}
    stmia   r0!, {r1, r3, r12, r14}
    subs    r2, r2, #32
    bge     %B0
    cmn     r2, #16
    stmgeia r0!, {r1, r3, r12, r14}
    subge   r2, r2, #16
1
    adds    r2, r2, #32-12
2
    stmgeia r0!, {r3, r12, r14}
    subges  r2, r2, #12
    bge     %B2
TrailingWords
    adds    r2, r2, #12-4
    blt     TrailingBytes
    subs    r2, r2, #4
    strlt   r1, [r0], #4
    stmgeia r0!, {r1, r3}
    subge   r2, r2, #4
TrailingBytes
    adds    r2, r2, #4
    ldmeqia r13!, {r0, pc}
    cmp     r2, #2
    strb    r1, [r0], #1
    strgeb  r1, [r0], #1
    strgtb  r1, [r0], #1
    ldmia   r13!, {r0, pc}
AlignDst
    rsb     r12, r12, #4
    cmp     r12, #2
    strb    r1, [r0], #1
    strgeb  r1, [r0], #1
    strgtb  r1, [r0], #1
    subs    r2, r2, r12
    blt     TrailingBytes
    b       DstAligned

;*****************************************************************************
;
; Standard C library function "memcpy".
;
;*****************************************************************************
    EXPORT  memcpy
memcpy
    stmdb   r13!, {r0, r14}
    subs    r2, r2, #4
    blt     Up_TrailingBytes
    ands    r12, r0, #3
    bne     Up_AlignDst
    ands    r12, r1, #3
    bne     Up_SrcUnaligned
Up_SrcDstAligned
    subs    r2, r2, #12-4
    blt     Up_TrailingWords
    subs    r2, r2, #32-12
    blt     %F1
    stmfd   r13!, {r4}
0
    ldmia   r1!, {r3, r4, r12, r14}
    stmia   r0!, {r3, r4, r12, r14}
    ldmia   r1!, {r3, r4, r12, r14}
    stmia   r0!, {r3, r4, r12, r14}
    subs    r2, r2, #32
    bge     %B0
    cmn     r2, #16
    ldmgeia r1!, {r3, r4, r12, r14}
    stmgeia r0!, {r3, r4, r12, r14}
    subge   r2, r2, #16
    ldmfd   r13!, {r4}
1
    adds    r2, r2, #32-12
2
    ldmgeia r1!, {r3, r12, r14}
    stmgeia r0!, {r3, r12, r14}
    subges  r2, r2, #12
    bge     %B2
Up_TrailingWords
    adds    r2, r2, #12-4
    blt     Up_TrailingBytes
    subs    r2, r2, #4
    ldrlt   r3, [r1], #4
    strlt   r3, [r0], #4
    ldmgeia r1!, {r3, r12}
    stmgeia r0!, {r3, r12}
    subge   r2, r2, #4
Up_TrailingBytes
    adds    r2, r2, #4
    ldmeqia r13!, {r0, pc}
    cmp     r2, #2
    ldrb    r3, [r1], #1
    strb    r3, [r0], #1
    ldrgeb  r3, [r1], #1
    strgeb  r3, [r0], #1
    ldrgtb  r3, [r1], #1
    strgtb  r3, [r0], #1
    ldmia   r13!, {r0, pc}
Up_AlignDst
    rsb     r12, r12, #4
    cmp     r12, #2
    ldrb    r3, [r1], #1
    strb    r3, [r0], #1
    ldrgeb  r3, [r1], #1
    strgeb  r3, [r0], #1
    ldrgtb  r3, [r1], #1
    strgtb  r3, [r0], #1
    subs    r2, r2, r12
    blt     Up_TrailingBytes
    ands    r12, r1, #3
    beq     Up_SrcDstAligned
Up_SrcUnaligned
    bic     r1, r1, #3
    ldr     r14, [r1], #4
    cmp     r12, #2
    bgt     Up_OneByte
    beq     Up_TwoBytes
Up_ThreeBytes
    cmp     r2, #16-4
    blt     %F1
    sub     r2, r2, #16-4
    stmfd   r13!, {r4, r5}
0
    mov     r3, r14, lsr #8
    ldmia   r1!, {r4, r5, r12, r14}
    orr     r3, r3, r4, lsl #24
    mov     r4, r4, lsr #8
    orr     r4, r4, r5, lsl #24
    mov     r5, r5, lsr #8
    orr     r5, r5, r12, lsl #24
    mov     r12, r12, lsr #8
    orr     r12, r12, r14, lsl #24
    stmia   r0!, {r3, r4, r5, r12}
    subs    r2, r2, #16
    bge     %B0
    ldmfd   r13!, {r4, r5}
    adds    r2, r2, #16-4
    blt     %F2
1
    mov     r12, r14, lsr #8
    ldr     r14, [r1], #4
    orr     r12, r12, r14, lsl #24
    str     r12, [r0], #4
    subs    r2, r2, #4
    bge     %B1
2
    sub     r1, r1, #3
    b       Up_TrailingBytes
Up_TwoBytes
    cmp     r2, #16-4
    blt     %F1
    sub     r2, r2, #16-4
    stmfd   r13!, {r4, r5}
0
    mov     r3, r14, lsr #16
    ldmia   r1!, {r4, r5, r12, r14}
    orr     r3, r3, r4, lsl #16
    mov     r4, r4, lsr #16
    orr     r4, r4, r5, lsl #16
    mov     r5, r5, lsr #16
    orr     r5, r5, r12, lsl #16
    mov     r12, r12, lsr #16
    orr     r12, r12, r14, lsl #16
    stmia   r0!, {r3, r4, r5, r12}
    subs    r2, r2, #16
    bge     %B0
    ldmfd   r13!, {r4, r5}
    adds    r2, r2, #16-4
    blt     %F2
1
    mov     r12, r14, lsr #16
    ldr     r14, [r1], #4
    orr     r12, r12, r14, lsl #16
    str     r12, [r0], #4
    subs    r2, r2, #4
    bge     %B1
2
    sub     r1, r1, #2
    b       Up_TrailingBytes
Up_OneByte
    cmp     r2, #16-4
    blt     %F1
    sub     r2, r2, #16-4
    stmfd   r13!, {r4, r5}
0
    mov     r3, r14, lsr #24
    ldmia   r1!, {r4, r5, r12, r14}
    orr     r3, r3, r4, lsl #8
    mov     r4, r4, lsr #24
    orr     r4, r4, r5, lsl #8
    mov     r5, r5, lsr #24
    orr     r5, r5, r12, lsl #8
    mov     r12, r12, lsr #24
    orr     r12, r12, r14, lsl #8
    stmia   r0!, {r3, r4, r5, r12}
    subs    r2, r2, #16
    bge     %B0
    ldmfd   r13!, {r4, r5}
    adds    r2, r2, #16-4
    blt     %F2
1
    mov     r12, r14, lsr #24
    ldr     r14, [r1], #4
    orr     r12, r12, r14, lsl #8
    str     r12, [r0], #4
    subs    r2, r2, #4
    bge     %B1
2
    sub     r1, r1, #1
    b       Up_TrailingBytes

;*****************************************************************************
;
; Standard C library function "memmove".
;
;*****************************************************************************
    EXPORT  memmove
memmove
    cmp     r1, r0
    bhi     memcpy
    add     r1, r1, r2
    add     r0, r0, r2
    subs    r2, r2, #4
    blt     Down_TrailingBytes
    ands    r12, r0, #3
    bne     Down_AlignDst
    ands    r12, r1, #3
    bne     Down_SrcUnaligned
Down_SrcDstAligned
    subs    r2, r2, #12-4
    blt     Down_TrailingWords
    stmfd   r13!, {r4, r14}
    subs    r2, r2, #32-12
    blt     %f1
0
    ldmdb   r1!, {r3, r4, r12, r14}
    stmdb   r0!, {r3, r4, r12, r14}
    ldmdb   r1!, {r3, r4, r12, r14}
    stmdb   r0!, {r3, r4, r12, r14}
    subs    r2, r2, #32
    bge     %b0
1
    cmn     r2, #16
    ldmgedb r1!, {r3, r4, r12, r14}
    stmgedb r0!, {r3, r4, r12, r14}
    subge   r2, r2, #16
    adds    r2, r2, #32-12
    ldmgedb r1!, {r3, r12, r14}
    stmgedb r0!, {r3, r12, r14}
    subge   r2, r2, #12
    ldmfd   r13!, {r4, r14}
Down_TrailingWords
    adds    r2, r2, #12-4
    blt     Down_TrailingBytes
    subs    r2, r2, #4
    ldrlt   r3, [r1, #-4]!
    strlt   r3, [r0, #-4]!
    ldmgedb r1!, {r3, r12}
    stmgedb r0!, {r3, r12}
    subge   r2, r2, #4
Down_TrailingBytes
    adds    r2, r2, #4
    moveq   pc, r14
    cmp     r2, #2
    ldrb    r3, [r1, #-1]!
    strb    r3, [r0, #-1]!
    ldrgeb  r3, [r1, #-1]!
    strgeb  r3, [r0, #-1]!
    ldrgtb  r3, [r1, #-1]!
    strgtb  r3, [r0, #-1]!
    mov     pc, r14
Down_AlignDst
    cmp     r12, #2
    ldrb    r3, [r1, #-1]!
    strb    r3, [r0, #-1]!
    ldrgeb  r3, [r1, #-1]!
    strgeb  r3, [r0, #-1]!
    ldrgtb  r3, [r1, #-1]!
    strgtb  r3, [r0, #-1]!
    subs    r2, r2, r12
    blt     Down_TrailingBytes
    ands    r12, r1, #3
    beq     Down_SrcDstAligned
Down_SrcUnaligned
    bic     r1, r1, #3
    ldr     r3, [r1]
    cmp     r12, #2
    blt     Down_OneByte
    beq     Down_TwoBytes
Down_ThreeBytes
    cmp     r2, #16-4
    blt     %f1
    sub     r2, r2, #16-4
    stmfd   r13!, {r4, r5, r14}
0
    mov     r14, r3, lsl #8
    ldmdb   r1!, {r3, r4, r5, r12}
    orr     r14, r14, r12, lsr #24
    mov     r12, r12, lsl #8
    orr     r12, r12, r5, lsr #24
    mov     r5, r5, lsl #8
    orr     r5, r5, r4, lsr #24
    mov     r4, r4, lsl #8
    orr     r4, r4, r3, lsr #24
    stmdb   r0!, {r4, r5, r12, r14}
    subs    r2, r2, #16
    bge     %b0
    ldmfd   r13!, {r4, r5, r14}
    adds    r2, r2, #16-4
    blt     %f2
1
    mov     r12, r3, lsl #8
    ldr     r3, [r1, #-4]!
    orr     r12, r12, r3, lsr #24
    str     r12, [r0, #-4]!
    subs    r2, r2, #4
    bge     %b1
2
    add     r1, r1, #3
    b       Down_TrailingBytes
Down_TwoBytes
    cmp     r2, #16-4
    blt     %f1
    sub     r2, r2, #16-4
    stmfd   r13!, {r4, r5, r14}
0
    mov     r14, r3, lsl #16
    ldmdb   r1!, {r3, r4, r5, r12}
    orr     r14, r14, r12, lsr #16
    mov     r12, r12, lsl #16
    orr     r12, r12, r5, lsr #16
    mov     r5, r5, lsl #16
    orr     r5, r5, r4, lsr #16
    mov     r4, r4, lsl #16
    orr     r4, r4, r3, lsr #16
    stmdb   r0!, {r4, r5, r12, r14}
    subs    r2, r2, #16
    bge     %b0
    ldmfd   r13!, {r4, r5, r14}
    adds    r2, r2, #16-4
    blt     %f2
1
    mov     r12, r3, lsl #16
    ldr     r3, [r1, #-4]!
    orr     r12, r12, r3, lsr #16
    str     r12, [r0, #-4]!
    subs    r2, r2, #4
    bge     %b1
2
    add     r1, r1, #2
    b       Down_TrailingBytes
Down_OneByte
    cmp     r2, #16-4
    blt     %f1
    sub     r2, r2, #16-4
    stmfd   r13!, {r4, r5, r14}
0
    mov     r14, r3, lsl #24
    ldmdb   r1!, {r3, r4, r5, r12}
    orr     r14, r14, r12, lsr #8
    mov     r12, r12, lsl #24
    orr     r12, r12, r5, lsr #8
    mov     r5, r5, lsl #24
    orr     r5, r5, r4, lsr #8
    mov     r4, r4, lsl #24
    orr     r4, r4, r3, lsr #8
    stmdb   r0!, {r4, r5, r12, r14}
    subs    r2, r2, #16
    bge     %b0
    ldmfd   r13!, {r4, r5, r14}
    adds    r2, r2, #16-4
    blt     %f2
1
    mov     r12, r3, lsl #24
    ldr     r3, [r1, #-4]!
    orr     r12, r12, r3, lsr #8
    str     r12, [r0, #-4]!
    subs    r2, r2, #4
    bge     %b1
2
    add     r1, r1, #1
    b       Down_TrailingBytes

;*****************************************************************************
;
; Standard C library function "memcmp".
;
;*****************************************************************************
    EXPORT  memcmp
memcmp
    stmdb   r13!, {r3-r4, r14}
    cmp     r2, #4
    blt     CheckBytes
    tst     r0, #3
    bne     CheckBytes
    tst     r1, #3
    bne     CheckBytes
CheckWords
    ldr     r3, [r0], #4
    ldr     r4, [r1], #4
    cmp     r3, r4
    bne     DoneWords
    sub     r2, r2, #4
    cmp     r2, #4
    bge     CheckWords
DoneWords
    cmp     r3, r4
    subne   r0, r0, #4
    subne   r1, r1, #4
CheckBytes
    cmp     r2, #0
    moveq   r0, #0
    ldmeqia r13!, {r3-r4, pc}
CheckByte
    ldrb    r3, [r0], #1
    ldrb    r4, [r1], #1
    cmp     r3, r4
    bne     DoneBytes
    subs    r2, r2, #1
    bne     CheckByte
DoneBytes
    sub     r0, r3, r4
    ldmia   r13!, {r3-r4, pc}

;*****************************************************************************
;
; Standard C library function "strlen".
;
;*****************************************************************************
    EXPORT  strlen
strlen
    mov     r2, #0
0
    ldrb    r1, [r0], #1
    cmp     r1, #0
    addne   r2, r2, #1
    bne     %b0
    mov     r0, r2
    mov     pc, r14

;*****************************************************************************
;
; Standard C library function "strcmp".
;
;*****************************************************************************
    EXPORT  strcmp
strcmp
    tst     r0, #3
    tsteq   r1, #3
    bne     strcmp_byteloop
    str     r14, [r13, #-4]!
    ldr     r14, =0x01010101
strcmp_loop
    ldr     r2, [r0], #4
    ldr     r3, [r1], #4
    sub     r12, r2, r14
    bic     r12, r12, r2
    ands    r12, r12, r14, lsl #7
    cmpeq   r2, r3
    beq     strcmp_loop
    ldr     r14, [r13], #4
    mov     r0, r3, lsl #24
    subs    r0, r0, r2, lsl #24
    tsteq   r12, #0x0FF0
    bne     strcmp_return
    mov     r0, r3, lsl #16
    subs    r0, r0, r2, lsl #16
    tsteq   r12, #0xFF00
    bne     strcmp_return
    mov     r0, r3, lsl #8
    subs    r0, r0, r2, lsl #8
    tsteq   r12, #0xFF0000
    subeqs  r0, r3, r2
strcmp_return
    movne   r0, r0, rrx
    mov     pc, r14
strcmp_byteloop
    ldrb    r2, [r0], #1
    ldrb    r3, [r1], #1
    cmp     r2, #1
    cmpcs   r2, r3
    bne     strcmp_byteend
    ldrb    r2, [r0], #1
    ldrb    r3, [r1], #1
    cmp     r2, #1
    cmpcs   r2, r3
    beq     strcmp_byteloop
strcmp_byteend
    sub     r0, r2, r3
    mov     pc, r14

;*****************************************************************************
;
; Convert an unsigned long to a float.
;
;*****************************************************************************
    EXPORT  _ffltu
_ffltu
    mov     r3, #0x40000000
    movs    r1, r0, lsr #16
    addne   r3, r3, #0x08000000
    moveqs  r0, r0, lsl #16
    moveq   pc, r14
    tst     r0, #0xFF000000
    addne   r3, r3, #0x04000000
    moveq   r0, r0, lsl #8
    tst     r0, #0xF0000000
    addne   r3, r3, #0x02000000
    moveq   r0, r0, lsl #4
    tst     r0, #0xC0000000
    addne   r3, r3, #0x01000000
    moveqs  r0, r0, lsl #2
    addmi   r3, r3, #0x00800000
    movpl   r0, r0, lsl #1
    movs    r1, r0, lsl #25
    adc     r0, r3, r0, asr #8
    movcc   pc, r14
    movne   pc, r14
    bic     r0, r0, #1
    mov     pc, r14

;*****************************************************************************
;
; Convert a float to a double.
;
;*****************************************************************************
    EXPORT  _f2d
_f2d
    add     r12, r0, #0x00800000
    tst     r12, #0x7f000000
    beq     _f2d_uncommon
    mov     r1, r0, lsl #29
    movs    r0, r0, asr #3
    add     r0, r0, #0x38000000
    movpl   pc, r14
    sub     r0, r0, #0x70000000
    mov     pc, r14
_f2d_uncommon
    tst     r12, #0x00800000
    beq     _f2d_Inf_or_NaN
_f2d_denorm
    movs    r1, r0, lsl #1
    moveq   pc, r14
    and     r2, r0, #0x80000000
    add     r3, r2, #0x36800000
    mov     r0, r0, lsl #3
    movs    r1, r0, lsr #16
    addne   r3, r3, #0x01000000
    moveqs  r0, r0, lsl #16
    moveq   pc, r14
    tst     r0, #0xff000000
    addne   r3, r3, #0x00800000
    moveq   r0, r0, lsl #8
    tst     r0, #0xf0000000
    addne   r3, r3, #0x00400000
    moveq   r0, r0, lsl #4
    tst     r0, #0xc0000000
    addne   r3, r3, #0x00200000
    moveqs  r0, r0, lsl #2
    addmi   r3, r3, #0x00100000
    movpl   r0, r0, lsl #1
    mov     r1, r0, lsl #21
    add     r0, r3, r0, ASR #11
    mov     pc, r14
_f2d_Inf_or_NaN
    movs    r1, r0, lsl #9
    orreq   r0, r0, #0x00700000
    moveq   pc, r14
    ldr     r0, dNaN
    mov     r1, #0
    mov     pc, r14
dNaN
    DCD     &7FF80000

;*****************************************************************************
;
; Multiply two doubles.
;
;*****************************************************************************
    EXPORT  _dmul
_dmul
    stmdb   r13!, {r4-r9, r11, r14}
    movs    r12, r2, lsl #1
    teqeq   r3, #0
    mov     r8, r12, lsl #10
    mov     r2, r12, lsr #20
    mov     r5, r3, lsl #11
    orr     r4, r8, r3, lsr #21
    addne   r2, r2, #0x00007800
    mov     r3, r2, RRX
    orrne   r4, r4, #0x80000000
    movs    r12, r12, asr #21
    addeq   r14, pc, #8
    beq     __fp_norm_op2
    cmn     r12, #1
    beq     dmul_uncommon1
    movs    r12, r0, lsl #1
    teqeq   r1, #0
    mov     r8, r12, lsl #10
    mov     r0, r12, lsr #20
    mov     r2, r1, lsl #11
    orr     r1, r8, r1, lsr #21
    addne   r0, r0, #0x00007800
    mov     r0, r0, RRX
    orrne   r1, r1, #0x80000000
    movs    r12, r12, asr #21
    addeq   r14, pc, #8
    beq     __fp_norm_op1
    cmn     r12, #1
    beq     dmul_uncommon
    bl      __fp_mult_common
dmul_return
    bl      __fp_e2d
    tst     r3, #0x20000000
    ldmeqia r13!, {r4-r9, r11, pc}
    orr     r3, r3, #0x00040000
    ldmia    r13!, {r4-r9, r11, r14}
dmul_uncommon1
    orr     r3, r3, #0x40000000
    movs    r12, r0, lsl #1
    teqeq   r1, #0
    mov     r8, r12, lsl #10
    mov     r0, r12, lsr #20
    mov     r2, r1, lsl #11
    orr     r1, r8, r1, lsr #21
    addne   r0, r0, #0x00007800
    mov     r0, r0, RRX
    orrne   r1, r1, #0x80000000
    movs    r12, r12, asr #21
    addeq   r14, pc, #8
    beq     __fp_norm_op1
    cmn     r12, #1
dmul_uncommon
    orreq   r0, r0, #0x40000000
    adr     r14, dmul_return
    mov     r11, #2
    b       __fp_mult_uncommon
__fp_norm_op1
    tst     r1, #0x80000000
    moveq   pc, r14
    stmdb   r13!, {r3, r14}
    bics    r1, r1, #0x80000000
    beq     __fp_denorm1_low
    movs    r3, r1, lsr #16
    moveq   r1, r1, lsl #16
    moveq   r12, #0x10
    movne   r12, #0
    movs    r3, r1, lsr #24
    moveq   r1, r1, lsl #8
    addeq   r12, r12, #8
    movs    r3, r1, lsr #28
    moveq   r1, r1, lsl #4
    addeq   r12, r12, #4
    movs    r3, r1, lsr #30
    moveq   r1, r1, lsl #2
    addeq   r12, r12, #2
    movs    r3, r1, lsr #31
    moveq   r1, r1, lsl #1
    addeq   r12, r12, #1
    rsb     r3, r12, #0x20
    orr     r1, r1, r2, lsr r3
    mov     r2, r2, lsl r12
    sub     r0, r0, r12
    add     r0, r0, #1
    ldmia   r13!, {r3, pc}
__fp_denorm1_low
    movs    r3, r2, lsr #16
    moveq   r2, r2, lsl #16
    moveq   r12, #0x10
    movne   r12, #0
    movs    r3, r2, lsr #24
    moveq   r2, r2, lsl #8
    addeq   r12, r12, #8
    movs    r3, r2, lsr #28
    moveq   r2, r2, lsl #4
    addeq   r12, r12, #4
    movs    r3, r2, lsr #30
    moveq   r2, r2, lsl #2
    addeq   r12, r12, #2
    movs    r3, r2, lsr #31
    moveq   r2, r2, lsl #1
    addeq   r12, r12, #1
    mov     r1, r2
    mov     r2, #0
    sub     r0, r0, #0x1f
    sub     r0, r0, r12
    ldmia   r13!, {r3, pc}
__fp_norm_op2
    tst     r4, #0x80000000
    moveq   pc, r14
    stmdb   r13!, {r0, r14}
    bics    r4, r4, #0x80000000
    beq     __fp_denorm2_low
    movs    r0, r4, lsr #16
    moveq   r4, r4, lsl #16
    moveq   r12, #0x10
    movne   r12, #0
    movs    r0, r4, lsr #24
    moveq   r4, r4, lsl #8
    addeq   r12, r12, #8
    movs    r0, r4, lsr #28
    moveq   r4, r4, lsl #4
    addeq   r12, r12, #4
    movs    r0, r4, lsr #30
    moveq   r4, r4, lsl #2
    addeq   r12, r12, #2
    movs    r0, r4, lsr #31
    moveq   r4, r4, lsl #1
    addeq   r12, r12, #1
    rsb     r0, r12, #0x20
    orr     r4, r4, r5, lsr r0
    mov     r5, r5, lsl r12
    sub     r3, r3, r12
    add     r3, r3, #1
    ldmia   r13!, {r0, pc}
__fp_denorm2_low
    movs    r0, r5, lsr #16
    moveq   r5, r5, lsl #16
    moveq   r12, #0x10
    movne   r12, #0
    movs    r0, r5, lsr #24
    moveq   r5, r5, lsl #8
    addeq   r12, r12, #8
    movs    r0, r5, lsr #28
    moveq   r5, r5, lsl #4
    addeq   r12, r12, #4
    movs    r0, r5, lsr #30
    moveq   r5, r5, lsl #2
    addeq   r12, r12, #2
    movs    r0, r5, lsr #31
    moveq   r5, r5, lsl #1
    addeq   r12, r12, #1
    mov     r4, r5
    mov     r5, #0
    sub     r3, r3, #0x1f
    sub     r3, r3, r12
    ldmia   r13!, {r0, pc}
__fp_mult_common
    bic      r8, r0, #0xc0000000
    bic      r9, r3, #0xc0000000
    eor      r0, r0, r3
    and      r0, r0, #0x80000000
    add      r3, r8, r9
    sub      r3, r3, #0x3f00
    sub      r3, r3, #0xfe
Mult_Mantissas
    teq      r2, #0
    beq      Mult_32xX
Mult_64xX
    teq      r5, #0
    beq      Mult_64x32
Mult_64x64
    stmdb    r13!, {r0, r7, r11, r14}
    mov      r0, r1, lsr #16
    bic      r7, r1, r0, lsl #16
    mov      r6, r4, lsr #16
    bic      r8, r4, r6, lsl #16
    mul      r9, r0, r6
    mul      r6, r7, r6
    mul      r7, r8, r7
    adds     r7, r7, r6, lsl #16
    adc      r9, r9, r6, lsr #16
    mul      r8, r0, r8
    adds     r7, r7, r8, lsl #16
    adc      r0, r9, r8, lsr #16
    mov      r11, r2, lsr #16
    bic      r14, r2, r11, lsl #16
    mov      r6, r5, lsr #16
    bic      r8, r5, r6, lsl #16
    mul      r9, r11, r6
    mul      r6, r14, r6
    mul      r14, r8, r14
    adds     r14, r14, r6, lsl #16
    adc      r9, r9, r6, lsr #16
    mul      r8, r11, r8
    adds     r14, r14, r8, lsl #16
    adc      r11, r9, r8, lsr #16
    adds     r7, r7, r11
    adc      r0, r0, #0
    adds     r11, r7, r14
    adcs     r7, r7, r0
    adc      r0, r0, #0
    subs     r8, r1, r2
    mov      r1, #0
    mov      r6, #0
    mvncc    r1, r1
    subcc    r6, r4, r5
    subnes   r9, r5, r4
    moveq    r1, #0
    mvncc    r1, r1
    subcc    r6, r6, r8
    mov      r4, r8, lsr #16
    bic      r5, r8, r4, lsl #16
    mov      r8, r9, lsr #16
    bic      r9, r9, r8, lsl #16
    mla      r2, r4, r8, r6
    mul      r8, r5, r8
    mul      r6, r9, r5
    adds     r6, r6, r8, lsl #16
    adc      r2, r2, r8, lsr #16
    mul      r9, r4, r9
    adds     r6, r6, r9, lsl #16
    adc      r2, r2, r9, lsr #16
    adds     r6, r11, r6
    adcs     r2, r7, r2
    adcs     r1, r0, r1
    orr      r14, r14, r14, lsl #2
    orr      r6, r6, r14, lsr #2
    ldmmiia  r13!, {r0, r7, r11, pc}
    adds     r6, r6, r6
    adcs     r2, r2, r2
    adc      r1, r1, r1
    sub      r3, r3, #1
    ldmia    r13!, {r0, r7, r11, pc}
Mult_64x32
    mov      r5, r4, lsr #16
    bic      r6, r4, r5, lsl #16
    mov      r8, r1, lsr #16
    bic      r9, r1, r8, lsl #16
    mul      r4, r5, r8
    mul      r8, r6, r8
    mul      r1, r9, r6
    adds     r1, r1, r8, lsl #16
    adc      r4, r4, r8, lsr #16
    mul      r9, r5, r9
    adds     r1, r1, r9, lsl #16
    adc      r4, r4, r9, lsr #16
    mov      r8, r2, lsr #16
    bic      r9, r2, r8, lsl #16
    mul      r2, r5, r8
    mul      r8, r6, r8
    mul      r6, r9, r6
    adds     r6, r6, r8, lsl #16
    adc      r2, r2, r8, lsr #16
    mul      r9, r5, r9
    adds     r6, r6, r9, lsl #16
    adc      r5, r2, r9, lsr #16
    adds     r2, r5, r1
    adcs     r1, r4, #0
    movmi    pc, r14
    adds     r6, r6, r6
    adcs     r2, r2, r2
    adc      r1, r1, r1
    sub      r3, r3, #1
    mov      pc, r14
Mult_32xX
    teq      r5, #0
    beq      Mult_32x32
Mult_32x64
    mov      r2, r1, lsr #16
    bic      r6, r1, r2, lsl #16
    mov      r8, r4, lsr #16
    bic      r9, r4, r8, lsl #16
    mul      r1, r2, r8
    mul      r8, r6, r8
    mul      r4, r9, r6
    adds     r4, r4, r8, lsl #16
    adc      r1, r1, r8, lsr #16
    mul      r9, r2, r9
    adds     r4, r4, r9, lsl #16
    adc      r1, r1, r9, lsr #16
    mov      r8, r5, lsr #16
    bic      r9, r5, r8, lsl #16
    mul      r5, r2, r8
    mul      r8, r6, r8
    mul      r6, r9, r6
    adds     r6, r6, r8, lsl #16
    adc      r5, r5, r8, lsr #16
    mul      r9, r2, r9
    adds     r6, r6, r9, lsl #16
    adc      r2, r5, r9, lsr #16
    adds     r2, r2, r4
    adcs     r1, r1, #0
    movmi    pc, r14
    adds     r6, r6, r6
    adcs     r2, r2, r2
    adc      r1, r1, r1
    sub      r3, r3, #1
    mov      pc, r14
__fp_mult_fast_common
    bic      r8, r0, #0xc0000000
    bic      r9, r3, #0xc0000000
    eor      r0, r0, r3
    and      r0, r0, #0x80000000
    add      r3, r8, r9
    sub      r3, r3, #0x3f00
    sub      r3, r3, #0xfe
Mult_32x32
    mov      r5, r4, lsr #16
    bic      r6, r4, r5, lsl #16
    mov      r8, r1, lsr #16
    bic      r9, r1, r8, lsl #16
    mul      r1, r5, r8
    mul      r8, r6, r8
    mul      r2, r9, r6
    adds     r2, r2, r8, lsl #16
    adc      r1, r1, r8, lsr #16
    mul      r9, r5, r9
    adds     r2, r2, r9, lsl #16
    adcs     r1, r1, r9, lsr #16
    mov      r6, #0
    movmi    pc, r14
    adds     r2, r2, r2
    adc      r1, r1, r1
    sub      r3, r3, #1
    mov      pc, r14
__fp_mult_uncommon
    mov      r9, r3, lsl #17
    cmn      r9, #0x20000
    andcc    r9, r9, r4
    ands     r9, r9, r3, lsl #1
    mov      r8, r0, lsl #17
    cmn      r8, #0x20000
    andcc    r8, r8, r1
    ands     r8, r8, r0, lsl #1
    bmi      Mult_NaNInf1
    tst      r9, #0x80000000
    bne      Mult_NaNInf2Only
    orrs     r8, r1, r2
    orrnes   r8, r4, r5
    beq      Mult_Zero
    ands     r6, r1, r0, lsl #1
    bicmi    r1, r1, #0x80000000
    addmi    r0, r0, #1
    ands     r6, r4, r3, lsl #1
    bicmi    r4, r4, #0x80000000
    addmi    r3, r3, #1
    bic      r8, r0, #0xc0000000
    bic      r9, r3, #0xc0000000
    eor      r0, r0, r3
    and      r0, r0, #0x80000000
    add      r3, r8, r9
    sub      r3, r3, #0x3f00
    sub      r3, r3, #0xfe
    stmdb    r13!, {r14}
    tst      r1, #0x80000000
    bleq     __fp_normalise_op1
    tst      r4, #0x80000000
    bleq     __fp_normalise_op2
    ldmia    r13!, {r14}
    b        Mult_Mantissas
Mult_Zero
    eor      r0, r0, r3
    and      r0, r0, #0x80000000
    mov      r1, #0
    mov      r2, #0
    mov      r3, #0
    mov      r6, #0
    mov      pc, r14
Mult_NaNInf1
    tst      r9, #0x80000000
    beq      Mult_NanInf1Only
    orr      r8, r2, r1, lsl #1
    orr      r8, r8, r5
    orrs     r8, r8, r4, lsl #1
    bne      __fp_convert_NaNs
Mult_InfShared
    eor      r8, r0, r3
    and      r8, r8, #0x80000000
    adr      r0,  Prototype_Infinity
    ldmia    r0, {r0-r2}
    orr      r0, r0, r8
    mov      pc, r14
Mult_NanInf1Only
    orrs     r8, r2, r1, lsl #1
    bne      __fp_convert_NaN_1Of2
    orrs     r8, r4, r5
    bne      Mult_InfShared
    orr      r0, r0, #0x61000000
    mov      pc, r14
Mult_NaNInf2Only
    orrs     r8, r5, r4, lsl #1
    bne      __fp_convert_NaN_2Of2
    orrs     r8, r1, r2
    bne      Mult_InfShared
    orr      r0, r0, #0x61000000
    mov      pc, r14
Prototype_Infinity
    DCD      0x40007fff
    DCD      0x00000000
    DCD      0x00000000
__fp_normalise_op1
    teq      r1, #0
    moveq    r1, r2
    moveq    r2, #0
    subeq    r3, r3, #0x20
    mov      r8, #0
    movs     r9, r1, lsr #16
    moveq    r1, r1, lsl #16
    addeq    r8, r8, #0x10
    movs     r9, r1, lsr #24
    moveq    r1, r1, lsl #8
    addeq    r8, r8, #8
    movs     r9, r1, lsr #28
    moveq    r1, r1, lsl #4
    addeq    r8, r8, #4
    movs     r9, r1, lsr #30
    moveq    r1, r1, lsl #2
    addeq    r8, r8, #2
    movs     r9, r1, lsr #31
    moveq    r1, r1, lsl #1
    addeq    r8, r8, #1
    rsbs     r9, r8, #0x20
    orr      r1, r1, r2, lsr r9
    mov      r2, r2, lsl r8
    sub      r3, r3, r8
    mov      pc, r14
__fp_normalise_op2
    teq      r4, #0
    moveq    r4, r5
    moveq    r5, #0
    subeq    r3, r3, #0x20
    mov      r8, #0
    movs     r9, r4, lsr #16
    moveq    r4, r4, lsl #16
    addeq    r8, r8, #0x10
    movs     r9, r4, lsr #24
    moveq    r4, r4, lsl #8
    addeq    r8, r8, #8
    movs     r9, r4, lsr #28
    moveq    r4, r4, lsl #4
    addeq    r8, r8, #4
    movs     r9, r4, lsr #30
    moveq    r4, r4, lsl #2
    addeq    r8, r8, #2
    movs     r9, r4, lsr #31
    moveq    r4, r4, lsl #1
    addeq    r8, r8, #1
    rsbs     r9, r8, #0x20
    orr      r4, r4, r5, lsr r9
    mov      r5, r5, lsl r8
    sub      r3, r3, r8
    mov      pc, r14
__fp_convert_NaNs
    orrs     r8, r2, r1, lsl #1
    beq      __fp_convert_NaN_2Of2
    tst      r1, #0x40000000
    beq      __fp_convert_NaN_1Of2
    orrs     r8, r5, r4, lsl #1
    beq      __fp_convert_NaN_1Of2
    tst      r4, #0x40000000
    bne      __fp_convert_NaN_1Of2
__fp_convert_NaN_2Of2
    tst      r4, #0x40000000
    beq      ReturnIVO
    mov      r0, r3
    mov      r1, r4
    mov      r2, r5
    b        ConvertNaN1_NoTrap
__fp_convert_NaN1
    tst      r1, #0x40000000
    beq      ReturnIVO
    b        ConvertNaN1_NoTrap
__fp_convert_NaN_1Of2
    tst      r1, #0x40000000
    beq      ReturnIVO
ConvertNaN1_NoTrap
    tst      r11, #1
    bne      ConvertNaN1_ToSingle
    tst      r11, #2
    bne      ConvertNaN1_ToDouble
    tst      r11, #0x200
    bne      ConvertNaN1_ToExtended_MantissaDone
    bic      r8, r0, #0xc0000000
    mov      r9, #0xff
    orr      r9, r9, #0x4300
    cmp      r8, r9
    biccc    r2, r2, #1
    orreq    r2, r2, #1
ConvertNaN1_ToExtended_MantissaDone
    and      r0, r0, #0xc0000000
    orr      r0, r0, #0xff
    orr      r0, r0, #0x7f00
    teq      r0, r0
    mov      pc, r14
ConvertNaN1_ToDouble
    and      r0, r0, #0xc0000000
    orr      r0, r0, #0xff
    orr      r0, r0, #0x4300
    mov      r2, r2, lsr #11
    mov      r2, r2, lsl #11
    orr      r1, r1, #0x80000000
    teq      r0, r0
    mov      pc, r14
ConvertNaN1_ToSingle
    and      r0, r0, #0xc0000000
    orr      r0, r0, #0x7f
    orr      r0, r0, #0x4000
    mov      r2, #0
    bic      r1, r1, #0xff
    orr      r1, r1, #0x80000000
    mov      pc, r14
ReturnIVO
    orr      r0, r0, #0x61000000
    mov      pc, r14
__fp_e2d
    bics     r12, r1, r0, lsl #1
    bpl      _e2d_SpecialCase
    subs     r3, r3, #0x3c00
    bmi      _e2d_ExpUnderflow
    addne    r12, r3, #1
    cmpne    r12, #0x800
    bge      _e2d_ExpOverflow
    movs     r12, r2, lsr #11
    bcs      _e2d_NeedsRounding
    orr      r0, r0, r3, lsl #20
    bic      r1, r1, #0x80000000
    orr      r0, r0, r1, lsr #11
    orr      r1, r12, r1, lsl #21
    mov      r3, #0
    mov      pc, r14
_e2d_SpecialCase
    tst      r0, #0x40000000
    bne      _e2d_Uncommon
_e2d_UnitsBit
    and      r0, r0, #0x80000000
    mov      r1, #0
    mov      r3, #0
    mov      pc, r14
_e2d_ExpOverflow
    teq      r3, #0
    beq      _e2d_ExpUnderflow
    and      r0, r0, #0x80000000
    orr      r0, r0, #0x70000000
    orr      r0, r0, #0xff00000
    mov      r1, #0
    mov      r3, #0x64000000
    mov      pc, r14
_e2d_ExpUnderflow
    adds     r3, r3, #0x34
    beq      _e2d_ExpJustUnderflow
    andmi    r0, r0, #0x80000000
    movmi    r1, #0
    movmi    r3, #0
    movmi    pc, r14
    subs     r3, r3, #0x20
    beq      _e2d_Underflow32
    blt      _e2d_ShortUnderflow
    movs     r12, r2, lsl r3
    bmi      _e2d_LongUnderflowNeedsRounding
    rsb      r12, r3, #0x20
    orr      r0, r0, r1, lsr r12
    mov      r2, r2, lsr r12
    orr      r1, r2, r1, lsl r3
    mov      r3, #0
    mov      pc, r14
_e2d_LongUnderflowNeedsRounding
    orrs     r12, r6, r12, lsl #1
    rsb      r12, r3, #0x20
    orr      r0, r0, r1, lsr r12
    mov      r2, r2, lsr r12
    orr      r1, r2, r1, lsl r3
    mov      r3, #0
    tsteq    r1, #1
    moveq    pc, r14
    adds     r1, r1, #1
    addcs    r0, r0, #1
    mov      pc, r14
_e2d_Underflow32
    mov      r3, #0
    teq      r2, #0x80000000
    movmi    pc, r14
    tsteq    r1, #1
    moveq    pc, r14
    adds     r1, r1, #1
    addcs    r0, r0, #1
    mov      pc, r14
_e2d_ShortUnderflow
    add      r3, r3, #0x20
    movs     r12, r1, lsl r3
    bmi      _e2d_ShortUnderflowNeedsRounding
    rsb      r12, r3, #0x20
    mov      r1, r1, lsr r12
    and      r0, r0, #0x80000000
    mov      r3, #0
    mov      pc, r14
_e2d_ShortUnderflowNeedsRounding
    teq      r12, #0x80000000
    orreqs   r12, r2, r6
    rsb      r12, r3, #0x20
    mov      r1, r1, lsr r12
    tsteq    r1, #1
    addne    r1, r1, #1
    mov      r3, #0
    mov      pc, r14
_e2d_ExpJustUnderflow
    teq      r1, #0x80000000
    teqeq    r2, #0
    moveq    r1, #0
    movne    r1, #1
    mov      r3, #0
    mov      pc, r14
_e2d_Uncommon
    tst      r0, #0x20000000
    bne      _e2d_Error
    mov      r3, #0
    teq      r1, #0
    movnes   r1, r1, lsl #1
    mvnmi    r0, #0
    movmi    pc, r14
    teqeq    r2, #0
    movne    r3, #0x61000000
    movne    pc, r14
    and      r0, r0, #0x80000000
    orr      r0, r0, #0x70000000
    orr      r0, r0, #0xff00000
    mov      pc, r14
_e2d_Error
    mov      r3, r0
    mov      pc, r14
_e2d_NeedsRounding
    bic      r1, r1, #0x80000000
    mov      r12, r2, lsl #22
    orrs     r12, r12, r6
    and      r12, r0, #0x80000000
    mov      r0, r3, lsl #20
    orr      r0, r0, r1, lsr #11
    mov      r1, r1, lsl #21
    orr      r1, r1, r2, lsr #11
    mov      r3, #0
    beq      _e2d_RoundToEven
    adds     r1, r1, #1
    addcs    r0, r0, #1
    adds     r2, r0, #0x100000
    orr      r0, r0, r12
    movpl    pc, r14
    mov      r3, #0x64000000
    mov      pc, r14
_e2d_RoundToEven
    tsteq    r1, #1
    orreq    r0, r0, r12
    moveq    pc, r14
    adds     r1, r1, #1
    addcs    r0, r0, #1
    adds     r2, r0, #0x100000
    orr      r0, r0, r12
    movpl    pc, r14
    mov      r3, #0x64000000
    mov      pc, r14

;*****************************************************************************
;
; Convert a double to an unsigned long.
;
;*****************************************************************************
    EXPORT  _dfixu
_dfixu
    movs    r3, r0, asr #20
    bmi     dfixu_error
    mov     r2, r0, lsl #11
    orr     r2, r2, r1, lsr #21
    orr     r2, r2, #0x80000000
    sub     r3, r3, #0x00000380
    rsbs    r3, r3, #0x0000009e
    movcs   r0, r2, lsr r3
    movcs   pc, r14
dfixu_error
    mov     r0, #0
    mov     pc, r14

;*****************************************************************************
;
; Shift right a 64-bit value.
;
;*****************************************************************************
    EXPORT  _ll_ushift_r
_ll_ushift_r
    subs    r3, r2, #32
    bpl     %f01
    rsb     r3, r2, #32
    mov     r0, r0, lsr r2
    orr     r0, r0, r1, lsl r3
    mov     r1, r1, lsr r2
    mov     pc, r14
01
    mov     r0, r1, lsr r3
    mov     r1, #0
    mov     pc, r14

;*****************************************************************************
;
; Shift right a signed 64-bit value.
;
;*****************************************************************************
    EXPORT  _ll_sshift_r
_ll_sshift_r
    subs    r3, r2, #32
    bpl     %f01
    rsb     r3, r2, #32
    mov     r0, r0, lsr r2
    orr     r0, r0, r1, lsl r3
    mov     r1, r1, asr r2
    mov     pc, r14
01
    mov     r0, r1, asr r3
    mov     r1, r1, asr #31
    mov     pc, r14

;*****************************************************************************
;
; Compare two 64-bit values to see if they are not equal.
;
;*****************************************************************************
    EXPORT  _ll_cmpne
_ll_cmpne
    subs    r0, r0, r2
    cmpeq   r1, r3
    movne   r0, #1
    mov     pc, r14

;*****************************************************************************
;
; Compare two signed 64-bit values to see if one is less than the other.
;
;*****************************************************************************
    EXPORT  _ll_scmplt
_ll_scmplt
    cmp     r1, r3
    movlt   r0, #1
    movgt   r0, #0
    movne   pc, r14
    cmp     r0, r2
    movlo   r0, #1
    movhs   r0, #0
    mov     pc, r14

;*****************************************************************************
;
; 64-bit signed division.
;
;*****************************************************************************
    EXPORT  _ll_sdiv
_ll_sdiv
    stmdb   r13!, {r4-r7, r14}
    mov     r4, r0
    mov     r5, r1
    mov     r6, r2
    mov     r7, r3
__ll_sdiv1
    ands    r12, r7, #1 << 31
    bpl     %f00
    rsbs    r6, r6, #0
    rsc     r7, r7, #0
00
    eors    r12, r12, r5, asr #32
    bcc     %f01
    rsbs    r4, r4, #0
    rsc     r5, r5, #0
01
    mov     r0, #0
    mov     r1, #0
    mov     r3, r5
    mov     r2, r4
    teq     r7, #0
    teqeq   r6, #0
    beq     %f05
    movs    r14, #0
02
    adds    r6, r6, r6
    adcs    r7, r7, r7
    bcs     %f03
    cmp     r7, r3
    cmpeq   r6, r2
    addls   r14, r14, #1
    bls     %b02
    adds    r14, r14, #0
03
    movs    r7, r7, rrx
    mov     r6, r6, rrx
04
    subs    r4, r2, r6
    sbcs    r5, r3, r7
    movcs   r3, r5
    movcs   r2, r4
    adcs    r0, r0, r0
    adc     r1, r1, r1
    movs    r7, r7, lsr #1
    mov     r6, r6, rrx
    subs    r14, r14, #1
    bge     %b04
05
    movs    r12, r12, lsl #1
    bcc     %f06
    rsbs    r0, r0, #0
    rsc     r1, r1, #0
06
    movs    r12, r12, lsl #1
    bcc     %f07
    rsbs    r2, r2, #0
    rsc     r3, r3, #0
07
    ldmia   r13!, {r4-r7, pc}

;*****************************************************************************
;
; 64-bit unsigned division.
;
;*****************************************************************************
    EXPORT  _ll_udiv
_ll_udiv
    stmdb   r13!, {r4-r6, r14}
    mov     r4, r0
    mov     r5, r1
    mov     r6, r2
    mov     lr, r3
__ll_udiv1
    mov     r0, #0
    mov     r1, #0
    mov     r3, r5
    mov     r2, r4
    teq     r14, #0
    teqeq   r6, #0
    beq     %f03
    movs    r12, #0
00
    adds    r6, r6, r6
    adcs    r14, r14, r14
    bcs     %f01
    cmp     r14, r3
    cmpeq   r6, r2
    addls   r12, r12, #1
    bls     %b00
    adds    r12, r12, #0
01
    movs    r14, r14, rrx
    mov     r6, r6, rrx
02
    subs    r4, r2, r6
    sbcs    r5, r3, r14
    movcs   r3, r5
    movcs   r2, r4
    adcs    r0, r0, r0
    adc     r1, r1, r1
    movs    r14, r14, lsr #1
    mov     r6, r6, rrx
    subs    r12, r12, #1
    bge     %b02
03
    ldmia   r13!, {r4-r6, pc}

    END
