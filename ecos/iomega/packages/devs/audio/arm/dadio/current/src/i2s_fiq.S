#include <dadio/io/audio_asm.h>

	.file  "i2s_fiq.S"

	.extern iome_dsp_data

	.code   32
	.text

	.global i2s_FIQ

i2s_FIQ:

	// register usage:
	//   r12    - i2s registers
	//   r11    - iome_dsp_data->rd_lbuf or temp var
	//   r10    - rdbuf
	//   r9     - temp (zero reg initially)
	//   r8     - temp
	    
	// load r12 w/ ptr to i2s registers
	ldr	r12, =0x80002000

	// load the rdbuf - it's faster to copy every time than it is to check
	ldr	r11, =iome_dsp_data
	add	r11, r11, #iome_dsp_data_t_rd_lbuf
	ldr	r10, [r11]
	ldr	r8, =_FIQ_lbuf
	str	r10, [r8]

	// if this buffer is empty, then play silence
	// else if we hit the end, then switch buffers,
	// else we are in the middle, so play audio
	
	mov	r9, #0

	// switch buffers if we hit the end
	ldr	r8, =_FIQ_pos
	ldr	r8, [r8]
	cmp	r8, #DSP_BUF_SIZE
	bne	1f
	
	//
	// the current rdbuf is empty, so switch buffers
	//

	// first zero out _FIQ_pos
	ldr	r8, =_FIQ_pos
	str	r9, [r8]

	// second, zero out the len on the buffer (just to be sure)
	str	r9, [r10, #lbuf_t_len]

	// then update _FIQ_lbuf
	ldr	r10, [r10, #lbuf_t_next]
	ldr	r8, =_FIQ_lbuf
	str	r10, [r8]

	// finally, update the iome_dsp_data structure so rdbuf is correct
	str	r10, [r11]

	//
	// at this point the buffers are switched
	//

	b	1f

0:	
	//
	// at this point, we have switched buffers, the next buffer is empty, so
	// we have no audio to play
	//
	str	r9, [r10, #lbuf_t_len]

	str	r9, [r12, #0x0080]
	str	r9, [r12, #0x0040]
	str	r9, [r12, #0x0080]
	str	r9, [r12, #0x0040]
	str	r9, [r12, #0x0080]
	str	r9, [r12, #0x0040]
	str	r9, [r12, #0x0080]
	str	r9, [r12, #0x0040]
	b	2f
	
	// we wrote silence, jump around the rest of the code
1:
	//
	// ok, so we didn't hit the end of the buffer, check if there is audio here
	//
	ldr	r8, [r10, #lbuf_t_len]
//	mov	r9, #0    // implicit
	cmp	r8, r9
	bls	0b

	// first off figure out where the data should be read from
	ldr	r8, [r10, #lbuf_t_data]
	ldr	r9, =_FIQ_pos
	ldr	r9, [r9]
	add	r8, r8, r9

	// then copy the data into the fifo
	ldrh	r9, [r8], #2	
	str	r9, [r12, #0x0080]
	ldrh	r9, [r8], #2
	str	r9, [r12, #0x0040]
	ldrh	r9, [r8], #2
	str	r9, [r12, #0x0080]
	ldrh	r9, [r8], #2
	str	r9, [r12, #0x0040]
	ldrh	r9, [r8], #2
	str	r9, [r12, #0x0080]
	ldrh	r9, [r8], #2
	str	r9, [r12, #0x0040]
	ldrh	r9, [r8], #2
	str	r9, [r12, #0x0080]
	ldrh	r9, [r8], #2
	str	r9, [r12, #0x0040]
	
	// finally, update the _FIQ_pos and lbuf->len

	// lbuf->len
	ldr	r8, [r10, #lbuf_t_len]
	sub	r8, r8, #16
	str	r8, [r10, #lbuf_t_len]

	// _FIQ_pos
	ldr	r8, =_FIQ_pos
	ldr	r9, [r8]
	add	r9, r9, #16
	str	r9, [r8]

2:
	// since there is no record buffer, read and forget fifo data
	ldr	r11, [r12, #0x0080]
	ldr	r11, [r12, #0x0040]
	ldr	r11, [r12, #0x0080]
	ldr	r11, [r12, #0x0040]
	ldr	r11, [r12, #0x0080]
	ldr	r11, [r12, #0x0040]
	ldr	r11, [r12, #0x0080]
	ldr	r11, [r12, #0x0040]
		
	// clear overrun/underrun bits in I2S status
	ldr	r11, =0xffffffff
	str	r11, [r12, #0x0100]

	// return
	subs	pc, lr, #4

	.data

_FIQ_pos:
	.long   0
	
_FIQ_lbuf:
	.long	0





