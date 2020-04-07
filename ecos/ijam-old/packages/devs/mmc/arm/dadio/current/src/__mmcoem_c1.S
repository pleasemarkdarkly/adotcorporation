// __mmcoem_c1.S: assembly versions of the >= 1 byte mmcoem routines
// this is for the second controller
// dan conti danc@iobjects.com

#define __MMC_TIMEOUT   0x10000

#define PD_MMC_CLK1	0x01
#define PD_MMC_CMD1	0x02
#define PD_MMC_DATA1	0x04
#define PD_MMC_CARD1	0x08
	
#define PD_MMC_CARD2	0x10
#define PD_MMC_CLK2	0x20
#define PD_MMC_CMD2	0x40
#define PD_MMC_DATA2	0x80
	
	.file	"__mmcoem_c0.S"
	.code	32

	// set the registers up
	.macro __mmc_setup_regs
	stmdb	sp!, {r4-r9}
	mov	r8, #0x80000003
	.endm

	// restore registers
	.macro __mmc_restore_regs
	ldmia	sp!, {r4-r9}
	.endm


	// precondition: r4 is value to write, r5 is clock-hi equivilant
	.macro __mmc_write_port
	strb	r4, [r8]
	strb	r5, [r8]
	strb	r4, [r8]
	.endm

	// precondition: r4 is clock low port value, r5 is clock high
	// postcondition: r5 is the port value
	.macro __mmc_read_port
	strb	r5, [r8]
	ldrb	r5, [r8]
	strb	r4, [r8]
	.endm

#if 1

// UCHAR MMCGetData_c0( SDVOID )
// Reads a single byte of data
	.globl MMCGetData_c1
MMCGetData_c1:	
	__mmc_setup_regs

	mov	r6, #8
	ldrb	r4, [r8]
	bic	r4, r4, #PD_MMC_CLK2
0:	
	orr	r5, r4, #PD_MMC_CLK2   // might be able to take this out

	__mmc_read_port
	
	mov	r0, r0, lsl #1
	tst	r5, #PD_MMC_DATA2
	orrne	r0, r0, #1
	
	subs	r6, r6, #1
	bne	0b

	and	r0, r0, #0xff
	__mmc_restore_regs
	mov	pc, lr

// end MMCGetData_c1

// SDVOID MMCSendData_c1( UCHAR odata )
	.globl	MMCSendData_c1
MMCSendData_c1:
	__mmc_setup_regs
	mov	r6, #0x80
	ldrb	r4, [r8]
	bic	r4, r4, #PD_MMC_CLK2
0:
	tst	r0, r6
	orrne	r4, r4, #PD_MMC_DATA2
	biceq	r4, r4, #PD_MMC_DATA2
	
	orr	r5, r4, #PD_MMC_CLK2    // need this in
	
	__mmc_write_port

	movs	r6, r6, lsr #1
	bne	0b

	__mmc_restore_regs
	mov	pc, lr

// end MMCSendData_c1


// SDVOID MMCSendCommand_c1( UINT32 Arg, UINT16 Cmd, UINT16 crcData )
// send a command to the card. in actuality, the Cmd part is 1 byte (in the lower half)
// the order is Cmd, Arg (high byte first), crcData
	.globl	MMCSendCommand_c1
MMCSendCommand_c1:
	__mmc_setup_regs

// the card needs to be clocked twice before a command
	ldrb	r4, [r8]
	bic	r4, r4, #PD_MMC_CLK2
	orr	r5, r4, #PD_MMC_CLK2

	__mmc_write_port
	__mmc_write_port

// first send cmd (r4 is already set up)
	mov	r6, #0x80
0:
	tst	r1, r6
	orrne	r4, r4, #PD_MMC_CMD2
	biceq	r4, r4, #PD_MMC_CMD2

	orr	r5, r4, #PD_MMC_CLK2

	__mmc_write_port

	movs	r6, r6, lsr #1
	bne	0b

// next send arg
	mov	r6, #0x80000000
0:
	tst	r0, r6
	orrne	r4, r4, #PD_MMC_CMD2
	biceq	r4, r4, #PD_MMC_CMD2
	
	orr	r5, r4, #PD_MMC_CLK2

	__mmc_write_port

	movs	r6, r6, lsr #1
	bne	0b

// last send crc
	mov	r6, #0x80
0:
	tst	r2, r6
	orrne	r4, r4, #PD_MMC_CMD2
	biceq	r4, r4, #PD_MMC_CMD2

	orr	r5, r4, #PD_MMC_CLK2

	__mmc_write_port

	movs	r6, r6, lsr #1
	bne	0b

	__mmc_restore_regs
	mov	pc, lr

// end MMCSendCommand_c1

// MMC_CC MMCReceive_c1( UCHAR* dBuf, UINT16 dataLength )
// read incoming data
	.globl MMCReceive_c1
MMCReceive_c1:
	__mmc_setup_regs

// first, we clock it until the the data line goes low, or we timeout
	mov	r6, #__MMC_TIMEOUT
	ldrb	r4, [r8]
	bic	r4, r4, #PD_MMC_CLK2
0:
	sub	r6, r6, #1
	cmp	r6, #0
	beq	13f       // timeout
	orr	r5, r4, #PD_MMC_CLK2
	__mmc_read_port

	tst	r5, #PD_MMC_DATA2
	bne	0b

	// okily dokily, r0 is our ptr, we can change it, same w/ r1
	add	r1, r0, r1       // so r1 is now the end of the array
0:
	mov	r7, #0
	mov	r6, #0x80
1:
	orr	r5, r4, #PD_MMC_CLK2

	__mmc_read_port
	
	mov	r7, r7, lsl #1
	tst	r5, #PD_MMC_DATA2
	orrne	r7, r7, #1
	
	movs	r6, r6, lsr #1
	bne	1b

	strb	r7, [r0], #1
	cmp	r0, r1
	bne	0b

	// MMC card had no error
	mov	r0, #0
	b	15f
	
13:	// MMC card is not responding
	mov	r0, #201
	b	15f
14:	// MMC data error
	mov	r0, #203
15:	
	__mmc_restore_regs
	mov	pc, lr

// end MMCReceive_c1

// MMC_CC MMCTransmit_c1( UCHAR* dBuf, UINT16 dataLength )
// send data out
	.extern calculateDataCRC16
	.globl MMCTransmit_c1
MMCTransmit_c1:
	
	__mmc_setup_regs

	// set up the ddr
	ldr	r4, =0x80000000
	add	r4, r4, #0x43
	ldrb	r5, [r4]
	bic	r5, r5, #PD_MMC_DATA2
	strb	r5, [r4]
	
	// we need a crc on this one, so save r0, r1, lr off and make a call
	stmdb	sp!, {r0, r1, lr}
	bl	calculateDataCRC16
	mov	r9, r0
	ldmia	sp!, {r0, r1, lr}

	// ok, at this point r0 is dBuf, r1 is dataLength, r9 is the crc
	// clock the card twice w/ data high (Nwr)
	ldrb	r4, [r8]
	orr	r4, r4, #PD_MMC_DATA2
	orr	r4, r4, #PD_MMC_CMD2
	
	bic	r4, r4, #PD_MMC_CLK2
	orr	r5, r4, #PD_MMC_CLK2

	__mmc_write_port
	__mmc_write_port

	// send the start bit
	bic	r4, r4, #PD_MMC_DATA2
	orr	r5, r4, #PD_MMC_CLK2

	__mmc_write_port

	// set up r1 as the end of the buffer
	add	r1, r0, r1
	// ship off the data
0:
	mov	r6, #0x80
	ldrb	r7, [r0], #1
1:
	tst	r7, r6
	orrne	r4, r4, #PD_MMC_DATA2
	biceq	r4, r4, #PD_MMC_DATA2

	orr	r5, r4, #PD_MMC_CLK2

	__mmc_write_port

	movs	r6, r6, lsr #1
	bne	1b

	teq	r0, r1
	bne	0b

	// now send the crc (2 bytes in r9)
	mov	r6, #0x8000
0:
	tst	r9, r6
	orrne	r4, r4, #PD_MMC_DATA2
	biceq	r4, r4, #PD_MMC_DATA2

	orr	r5, r4, #PD_MMC_CLK2

	__mmc_write_port

	movs	r6, r6, lsr #1
	bne	0b
	
	// clock the card 3 times
	ldrb	r4, [r8]
	orr	r4, r4, #PD_MMC_DATA2
	bic	r4, r4, #PD_MMC_CLK2
	orr	r5, r4, #PD_MMC_CLK2

	__mmc_write_port
	__mmc_write_port
	__mmc_write_port

	// this is where it gets fun
	// switch the ddr back over so we can read back the crc response
	ldr	r4, =0x80000000
	add	r4, r4, #0x43
	ldrb	r5, [r4]
	orr	r5, r5, #PD_MMC_DATA2
	strb	r5, [r4]

	// clock the card and look at the goodies
	ldrb	r4, [r8]
	bic	r4, r4, #PD_MMC_CLK2
	orr	r5, r4, #PD_MMC_CLK2

	__mmc_read_port

	// the data line should be low
	tst	r5, #PD_MMC_DATA2
	bne	13f   // error out if it is still high

	// now we read the crc response (1 nibble)
	mov	r6, #0x8
	mov	r7, #0
0:
	orr	r5, r4, #PD_MMC_CLK2
	
	__mmc_read_port
	tst	r5, #PD_MMC_DATA2
	orrne	r7, r7, r6

	movs	r6, r6, lsr #1
	bne	0b
	
	teq	r7, #0x5
	bne	14f     // crc error
	
	mov	r0, #0  // no error
	b	15f
	
13:	// no crc came back
	mov	r0, #220
	b	15f
14:     // the crc was naughty
	mov	r0, #204
15:
	__mmc_restore_regs
	mov	pc, lr
	
// end MMCTransmit_c1
	
// MMC_CC mmc_get_response_c1( UCHAR* resp_bytes, UINT16 respBitLength )
// get a response over the CMD line
	.globl	mmc_get_response_c1
mmc_get_response_c1:
	__mmc_setup_regs

	// set the DDR so CMD is an input
	ldr	r4, =0x80000000
	add	r4, r4, #0x43
	ldrb	r5, [r4]
	orr	r5, r5, #PD_MMC_CMD2
	strb	r5, [r4]

	// first part, clock the card until the CMD line goes low
	mov	r6, #__MMC_TIMEOUT
	ldrb	r4, [r8]
	bic	r4, r4, #PD_MMC_CLK2
0:
	subs	r6, r6, #1
	beq	14f
	
	orr	r5, r4, #PD_MMC_CLK2

	__mmc_read_port

	tst	r5, #PD_MMC_CMD2
	bne	0b

	// so now the party is on
	// note that the first bit read in the previous code counts
0:
	mov	r6, #0x80
	mov	r7, #0
1:
	tst	r5, #PD_MMC_CMD2
	orrne	r7, r7, r6
	biceq	r7, r7, r6
	
	sub	r1, r1, #1
	
	orr	r5, r4, #PD_MMC_CLK2

	__mmc_read_port

	movs	r6, r6, lsr #1
	bne	1b

	strb	r7, [r0], #1
	cmp	r1, #0
	bne	0b

	mov	r0, #0
	b	15f
	
14:	// card is not responding
	mov	r0, #201
15:	
	// set the DDR back
	ldr	r4, =0x80000000
	add	r4, r4, #0x43
	ldrb	r5, [r4]
	bic	r5, r5, #PD_MMC_CMD2
	strb	r5, [r4]

	__mmc_restore_regs
	mov	pc, lr

// end mmc_get_response
	
#endif
