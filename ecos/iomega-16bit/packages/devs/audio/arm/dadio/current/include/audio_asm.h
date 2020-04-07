#ifndef DSP_ASM_H
#define DSP_ASM_H
// ====================================================================
//
//      dsp_asm.h
//
//      Device I/O 
//
// ====================================================================
// ====================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   toddm
// Contributors:        toddm
// Date:        1999-12-28
// Purpose:     Structure offsets for assembly referencing.
// Description:
//
//####DESCRIPTIONEND####
//
// ====================================================================

#if 0
typedef struct {
  unsigned char*          data;
  unsigned int            len;
  struct _lbuf_t*         next; /* Pointer to next lbuf in (circularly) linked list */
} lbuf_t;
#endif

#define lbuf_t_data       0x00
#define lbuf_t_len        0x04
#define lbuf_t_next       0x08

#if 0
typedef struct {
  lbuf_t * wr_lbuf;              /* Buffer that cyg_io_write uses */
  lbuf_t * rd_lbuf;              /* Buffer that the ISR uses */
} iome_dsp_data_t;
#endif

#define iome_dsp_data_t_wr_lbuf      0x00
#define iome_dsp_data_t_rd_lbuf      0x04

#define DSP_BUF_SIZE 4608

#endif /* DSP_ASM_H */
