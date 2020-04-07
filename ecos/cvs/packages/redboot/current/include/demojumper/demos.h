#ifndef DEMOS_H_DEFINED
#define DEMOS_H_DEFINED

#define DEMO_COUNT 5
#define DEMO_DESC_LEN 64
#define DEMO_ALIAS_LEN 32

// note: I'm rearranging these to easier think about the order they'll actually reside on ROM in, but notice that that doesn't have much
// correspondence to the ordering in the menu system, which will be based on the actual # in the define names.

#define DEMO0_TITLE "USB Harddrive Slave"
#define DEMO0_ALIAS "usb_hd_slave"
#define DEMO0_FLASH 0xe0060000		// x060000
#define DEMO0_LEN	0x20000			// to x080000
#define DEMO0_BASE	0x20000
#define DEMO0_ENTRY_OFFSET 0x40

#define DEMO4_TITLE "MP3 Encoder"
#define DEMO4_ALIAS "mp3enc"
#define DEMO4_FLASH 0xe00a0000		// xa0000
#define DEMO4_LEN	0xC0000			// to x160000
#define DEMO4_BASE	0x10000
#define DEMO4_ENTRY_OFFSET 0x44

#define CODEC_ROM_START 0xe0180000	// x180000
#define CODEC_ROM_LEN 0x40000		// to x1C0000
#define CODEC_IMAGE_NAME "codecs"

#define DEMO3_TITLE "DAR"
#define DEMO3_ALIAS "dar"
#define DEMO3_FLASH 0xe0200000		// x0200000 
#define DEMO3_LEN	0x100000		// to x300000
#define DEMO3_BASE	0x20000
#define DEMO3_ENTRY_OFFSET 0x44

// note extra space (x80000) just to avoid moving linein passthrough for the moment

#define DEMO1_TITLE "Line In Passthru"
#define DEMO1_ALIAS "linein_out"
#define DEMO1_FLASH 0xe03a0000		// x3a0000
#define DEMO1_LEN	0xa0000			// to x440000
#define DEMO1_BASE	0x20000
#define DEMO1_ENTRY_OFFSET 0x40

#define DEMO2_TITLE "Rip, Enc, Play"
#define DEMO2_ALIAS "ripencplay"
#define DEMO2_FLASH 0xe0200000		// x460000
#define DEMO2_LEN	0x80000			// to x4e0000	(allocating 512k experimentally, objcopy isn't working)
#define DEMO2_BASE	0x10000
#define DEMO2_ENTRY_OFFSET 0x44

#if 0

#define DEMO5_TITLE "Extra Slot"
#define DEMO5_ALIAS "exslot"
#define DEMO5_FLASH 0xe0000000
#define DEMO5_LEN	0x00000
#define DEMO5_BASE	0x10000
#define DEMO5_ENTRY_OFFSET 0x40

#define DEMO6_TITLE "Extra Slot"
#define DEMO6_ALIAS "exslot"
#define DEMO6_FLASH 0xe0000000
#define DEMO6_LEN	0x00000
#define DEMO6_BASE	0x10000
#define DEMO6_ENTRY_OFFSET 0x40

#endif

extern char demo_names[DEMO_COUNT][DEMO_DESC_LEN];
extern char demo_aliases[DEMO_COUNT][DEMO_ALIAS_LEN];
extern unsigned long demo_lens[DEMO_COUNT];
extern unsigned long demo_flash_bases[DEMO_COUNT];
extern unsigned long demo_bases[DEMO_COUNT];
extern unsigned long demo_bases[DEMO_COUNT];
extern unsigned long demo_entry_offsets[DEMO_COUNT];
// this fn is a little hackish, since you can't just iterate through the #define names based on indexes.  it must be updated in demojumper.c to reflect
// the defines above.
void setup_demos(void);

#endif // DEMOS_H_DEFINED