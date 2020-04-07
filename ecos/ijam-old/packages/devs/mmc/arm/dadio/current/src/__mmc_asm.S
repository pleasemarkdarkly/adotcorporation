// __mmc_asm.S: mmc assembly routines
// danc@iobjects.com   Dan Conti

// functions in this file
//	
// void mmc_write_cmd_c0( char* buf, int len )
// void mmc_read_resp_c0( char* buf, int len )
// void mmc_write_data_c0( char* buf, int len )
// void mmc_read_data_c0( char* buf, int len )
// void mmc_write_cmd_c1( char* buf, int len )
// void mmc_read_resp_c1( char* buf, int len )
// void mmc_write_data_c1( char* buf, int len )
// void mmc_read_data_c1( char* buf, int len )

	
#define PORT_D_OFFSET		0x03

#define PD_MMC_CLK1	0x01
#define PD_MMC_CMD1	0x02
#define PD_MMC_DATA1	0x04
#define PD_MMC_CARD1	0x08
#define PD_MMC_CARD2	0x10
#define PD_MMC_CLK2	0x20
#define PD_MMC_CMD2	0x40
#define PD_MMC_DATA2	0x80

	.file	"__mmc_asm.S"
	
	.code	32

	.extern portIO
	
	// set the registers up
	.macro __mmc_setup_regs
	stmdb	sp!, {r4-r9}
	ldr	r8, =0x80000000
	add	r8, r8, #PORT_D_OFFSET
	.endm

	// restore registers
	.macro __mmc_restore_regs
	ldmia	sp!, {r4-r10}
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
	

// void mmc_write_cmd_c0( char* val, int len )
// register usage
// r0 is the char* we are incrementing
// r1 is the end of the string
// r4 is the port value we are writing
// r5 is r4 with the clock set high
// r6 is the current char
// r7 is the status of the current bit in the char
// r8 is portD
// r9 is our mask
	
	.globl mmc_write_cmd_c0
mmc_write_cmd_c0:
	__mmc_setup_regs

	// r0,r1 are molestable
	add	r1, r1, r0  // len is the end ptr
0:	
	ldr	r6, [r0], #1

	// r9 is the mask
	mov	r9, #0x80
1:
	// load in the current port value
	ldr	r4, [r8]
	
	tst	r6, r9
	
	biceq	r4, r4, #PD_MMC_CMD1
	orrne	r4, r4, #PD_MMC_CMD1

	// set r5 to be the clock-hi equivilant of r4
	orr	r5, r4, #PD_MMC_CLK1

	// write out the bit
	__mmc_write_port
	
	movs	r9, r9, lsr #1
	bne	1b      // while( not at end of byte )

	cmp	r0, r1
	bne	0b      // while( not at end of cmd )

	__mmc_restore_regs
	mov	pc, lr


	
// void mmc_read_resp_c0( char* val, int len )
// register usage
// r0 is the char* we are incrementing
// r1 is the end of the string
// r4 is the port with the clock set low
// r5 is the port with the clock set high, or the port value read
// r6 is the current char
// r7 is the status of the current bit in the char
// r8 is portD
// r9 is our mask
	
	.globl mmc_read_resp_c0
mmc_read_resp_c0:
	__mmc_setup_regs

	// r0,r1 are molestable
	add	r1, r1, r0  // len is the end ptr
	
0:	
	// r10 is the mask
	mov	r9, #0x80
1:
	// load in the current port value
	ldr	r4, [r8]
	
	// set r5 to be the clock-hi equivilant of r4
	orr	r5, r4, #PD_MMC_CLK1

	// read the bit off the port (into r5)
	__mmc_read_port

	tst	r5, #PD_MMC_CMD1
	
	orrne	r6, r6, r9
	biceq	r6, r6, r9
	
	movs	r9, r9, lsr #1
	bne	1b      // while( not at end of byte )

	strb	r6, [r0], #1

	cmp	r0, r1
	bne	0b      // while( not at end of cmd )

	__mmc_restore_regs
	mov	pc, lr


	.globl  mmc_write_data_c0
mmc_write_data_c0:
	__mmc_setup_regs

	// r0,r1 are molestable
	add	r1, r1, r0  // len is the end ptr
	
0:	
	ldr	r6, [r0], #1

	// r10 is the mask
	mov	r9, #0x80
1:
	// load in the current port value
	ldr	r4, [r8]
	
	tst	r6, r9
	
	biceq	r4, r4, #PD_MMC_DATA1
	orrne	r4, r4, #PD_MMC_DATA1

	// set r5 to be the clock-hi equivilant of r4
	orr	r5, r4, #PD_MMC_CLK1

	// write out the bit
	__mmc_write_port
	
	movs	r9, r9, lsr #1
	
	bne	1b      // while( not at end of byte )

	cmp	r0, r1
	bne	0b      // while( not at end of cmd )

	__mmc_restore_regs
	mov	pc, lr

	
	.globl mmc_read_data_c0
mmc_read_data_c0:
	__mmc_setup_regs

	// r0,r1 are molestable
	add	r1, r1, r0  // len is the end ptr
0:	
	// r10 is the mask
	mov	r9, #0x80
1:
	// load in the current port value
	ldr	r4, [r8]
	
	// set r5 to be the clock-hi equivilant of r4
	orr	r5, r4, #PD_MMC_CLK1

	// read the bit off the port (into r5)
	__mmc_read_port

	tst	r5, #PD_MMC_DATA1
	
	orrne	r6, r6, r9
	biceq	r6, r6, r9
	
	movs	r9, r9, lsr #1
	
	bne	1b      // while( not at end of byte )

	strb	r6, [r0], #1

	cmp	r0, r1
	bne	0b      // while( not at end of cmd )

	__mmc_restore_regs
	mov	pc, lr


// int  mmc_exchange_data_c0( unsigned char odata, unsigned char* idata);
// register usage
// inputs:
// r0    odata
// r1    idata
// outputs:
// r0	always 0
	
// register usage
// r6 is port value w/ clock high
// r7 is original port value
// r8 is portD
#if 0
	.globl	mmc_exchange_data_c0
mmc_exchange_data_c0:
	stmdb	sp!, {r6-r8}
	ldr	r8, =0x80000000
	add	r8, r8, #3

	and	r0, r0, #0x03   // bottom two bits in this value are CMD,DATA
	mov	r0, r0, lsl #1
	
	ldrb	r7, [r8]
	orr	r7, r7, r0   // value with the bits set
	orr	r6, r7, #PD_MMC_CLK1    // clock high value

	strb	r7, [r8]   // set value
	strb	r6, [r8]   // clock up
	ldrb	r6, [r8]   // read response
	strb	r7, [r8]   // clock down

	and	r7, r7, #0x06         // grab the CMD and DAT lines
	mov	r7, r7, lsl #3     // shift em' up

	// now r5 is our return value, save it out
	strb	r7, [r1]
	
	mov	r0, #0   // always return 0
	
	ldmia	sp!, {r6-r8}
	mov	pc, lr

#endif

	.globl mmc_exchange_data_c0
mmc_exchange_data_c0:
	mov	ip, sp
	stmdb	sp!, {r4, fp, ip, lr, pc}
	sub	fp, ip, #4
	sub	sp, sp, #8
	mov	r3, r0
	str	r1, [fp, #-24]
	strb	r3, [fp, #-17]
	ldrb	r1, [fp, #-17]
	and	ip, r1, #3
	mov	r1, ip
	strb	r1, [fp, #-17]
	ldrb	r3, [fp, #-17]
	
	// this part checks portIO
	ldr	r1, [pc, #0xbc]
	ldr	ip, [r1]
	cmp	ip, #0
	beq	0f
	mov	ip, r3, lsl #6
	mov	r1, ip
	b	10f
0:
	mov	ip, r3, lsl #1
	mov	r1, ip
10:
	// end of part
	
	strb	r1, [fp, #-17]
	
	mov	r1, #0x80000003
	ldrb	ip, [fp, #-17]
	strb	ip, [r1]
	mov	r1, #0x80000003
	ldrb	r2, [fp, #-17]

	// checking port here too
	ldr	ip, [pc, #0x80]
	ldr	lr, [ip]
	cmp	lr, #0
	beq	4f
	orr	lr, r2, #32
	mov	ip, lr
	b	5f
4:	
	orr	lr, r2, #1
	mov	ip, lr
5:	
	strb	ip, [r1]
	ldr	r1, [fp, #-24]
	mov	ip, #0x80000003
	ldrb	lr, [ip]
	strb	lr, [r1]
	mov	r1, #0x80000003
	ldrb	ip, [fp, #-17]
	strb	ip, [r1]
	ldr	r1, [fp, #-24]
	
	// last one
	ldrb	ip, [pc, #0x38]
	ldr	lr, [ip]
	cmp	lr, #0
	beq	8f
	ldr	ip,[fp, #-24]
	ldrb	lr, [ip]
	mov	r4, lr, lsr #2
	mov	ip, r4
	b	9f
8:
	ldr	lr, [fp, #-24]
	ldrb	r4, [lr]
	mov	lr, r4, lsl #3
	mov	ip, lr
9:
	strb	ip, [r1]
	mov	r0, #0
	b	2f
2:	.long	0
	ldmdb	fp, {r4, fp, sp, pc}
	

///////////////////////////////////////////////////////////////////////////////////////	

// void mmc_write_cmd_c1( char* val, int len )
// register usage
// r0 is the char* we are incrementing
// r1 is the end of the string
// r4 is the port value we are writing
// r5 is r4 with the clock set high
// r6 is the current char
// r7 is the status of the current bit in the char
// r8 is portD
// r9 is our mask
	
	.globl mmc_write_cmd_c1
mmc_write_cmd_c1:
	__mmc_setup_regs

	// r0,r1 are molestable
	add	r1, r1, r0  // len is the end ptr
	
0:	
	ldr	r6, [r0], #1

	// r10 is the mask
	mov	r9, #0x80
1:
	// load in the current port value
	ldr	r4, [r8]
	
	tst	r6, r9
	
	biceq	r4, r4, #PD_MMC_CMD2
	orrne	r4, r4, #PD_MMC_CMD2

	// set r5 to be the clock-hi equivilant of r4
	orr	r5, r4, #PD_MMC_CLK2

	// write out the bit
	__mmc_write_port
	
	movs	r9, r9, lsr #1
	
	bne	1b      // while( not at end of byte )

	cmp	r0, r1
	bne	0b      // while( not at end of cmd )

	__mmc_restore_regs
	mov	pc, lr


// void mmc_read_resp_c1( char* val, int len )
// register usage
// r0 is the char* we are incrementing
// r1 is the end of the string
// r4 is the port with the clock set low
// r5 is the port with the clock set high, or the port value read
// r6 is the current char
// r7 is the status of the current bit in the char
// r8 is portD
// r9 is our mask
	
	.globl mmc_read_resp_c1
mmc_read_resp_c1:
	__mmc_setup_regs

	// r0,r1 are molestable
	add	r1, r1, r0  // len is the end ptr
	
0:	
	// r10 is the mask
	mov	r9, #0x80
1:
	// load in the current port value
	ldr	r4, [r8]
	
	// set r5 to be the clock-hi equivilant of r4
	orr	r5, r4, #PD_MMC_CLK2

	// read the bit off the port (into r5)
	__mmc_read_port

	tst	r5, #PD_MMC_CMD2
	
	orrne	r6, r6, r9
	biceq	r6, r6, r9
	
	movs	r9, r9, lsr #1
	
	bne	1b      // while( not at end of byte )

	strb	r6, [r0], #1

	cmp	r0, r1
	bne	0b      // while( not at end of cmd )

	__mmc_restore_regs
	mov	pc, lr

	.globl  mmc_write_data_c1
mmc_write_data_c1:
	__mmc_setup_regs

	// r0,r1 are molestable
	add	r1, r1, r0  // len is the end ptr
	
0:	
	ldr	r6, [r0], #1

	// r10 is the mask
	mov	r9, #0x80
1:
	// load in the current port value
	ldr	r4, [r8]
	
	tst	r6, r9
	
	biceq	r4, r4, #PD_MMC_DATA2
	orrne	r4, r4, #PD_MMC_DATA2

	// set r5 to be the clock-hi equivilant of r4
	orr	r5, r4, #PD_MMC_CLK2

	// write out the bit
	__mmc_write_port
	
	movs	r9, r9, lsr #1
	
	bne	1b      // while( not at end of byte )

	cmp	r0, r1
	bne	0b      // while( not at end of cmd )

	__mmc_restore_regs
	mov	pc, lr

	
	.globl mmc_read_data_c1
mmc_read_data_c1:
	__mmc_setup_regs

	// r0,r1 are molestable
	add	r1, r1, r0  // len is the end ptr
	
0:	
	// r10 is the mask
	mov	r9, #0x80
1:
	// load in the current port value
	ldr	r4, [r8]
	
	// set r5 to be the clock-hi equivilant of r4
	orr	r5, r4, #PD_MMC_CLK2

	// read the bit off the port (into r5)
	__mmc_read_port

	tst	r5, #PD_MMC_DATA2
	
	orrne	r6, r6, r9
	biceq	r6, r6, r9
	
	movs	r9, r9, lsr #1
	
	bne	1b      // while( not at end of byte )

	strb	r6, [r0], #1

	cmp	r0, r1
	bne	0b      // while( not at end of cmd )

	__mmc_restore_regs
	mov	pc, lr
	
// int  mmc_exchange_data_c1( unsigned char odata, unsigned char* idata);
// register usage
// inputs:
// r0    odata
// r1    idata
// outputs:
// r0	always 0
// register usage
// r6 is port value w/ clock high
// r7 is original port value
	
	.globl	mmc_exchange_data_c1
mmc_exchange_data_c1:
	stmdb	sp!, {r6-r8}
	ldr	r8, =0x80000000
	add	r8, r8, #3

	and	r0, r0, #3   // bottom two bits in this value are CMD,DATA
	mov	r0, r0, lsl #6
	
	ldrb	r7, [r8]
	orr	r7, r7, r0   // value with the bits set
	orr	r6, r7, #PD_MMC_CLK2    // clock high value
	
	strb	r7, [r8]   // set value
	strb	r6, [r8]   // clock up
	ldrb	r6, [r8]   // read response
	strb	r7, [r8]   // clock down

	and	r7, r7, #0xc0         // grab the CMD and DAT lines
	mov	r7, r7, lsr #2     // shift em' down

	// now r5 is our return value, save it out
	strb	r7, [r1]
	
	mov	r0, #0   // always return 0
	
	ldmia	sp!, {r6-r8}
	mov	pc, lr	



