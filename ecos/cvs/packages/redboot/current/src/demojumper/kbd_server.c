#include <redboot.h>
#include "demojumper/kbd_server.h"
#include "demojumper/debug.h"
#include "demojumper/GumiScreen.h"
#include "demojumper/gui_state.h"
#include <kbd.h>

#define MAX_SCRIPT_SIZE 1024

UCHAR g_ucCurrent = 0;
UCHAR g_ucHighlighted = 0;
UCHAR g_ucTop = 0;
UCHAR g_ucBottom = 0;
char* g_psMenu[MENU_ITEM_COUNT];
cyg_uint8 _NewKeyState[NUM_KEYS];
cyg_uint8 _KeyState[NUM_KEYS];
static char m_sScript[MAX_SCRIPT_SIZE];

static short m_cSeqStep = 0;			// how many keypresses have we received in the sequence?
static short m_bScriptSelectMode = 0;	// have we activated script mode and are waiting for script selector keypress?

// a sequence of 4 has a liklihood of about one in 64000, whereas a seq of 5 gets us to about one in a million, which sounds nice.
#define KBD_SEQ_1			6
#define KBD_SEQ_2			15
#define KBD_SEQ_3			14
#define KBD_SEQ_4			7
#define KBD_SEQ_5			8

#define FIS_DUMMY_IMAGE_NAME "dont_delete_me"
#define LOAD_STR "fis delete " FIS_DUMMY_IMAGE_NAME "\nload -v -m xmodem" // cmd string to pause, then initiate xmodem xfer

void ScriptInstructions()
{
	int i;
	printf("\n");
	printf("Script mode active, known images:\n");
	printf("\n");
	printf("0: Cancel\n");
	printf("1: Codecs\n");
	for (i = 0; i < DEMO_COUNT; ++i)
	{
		printf("%d: %s\n", i+2, demo_names[i]);
	}
	printf("\n");
	printf("Enter one of the above numbers to update that image,\n");
	printf("or an out-of-range key (higher than %d) to refresh all images\n",i+1);
	printf("\n");
}

void
KeyboardProc(void)
{
	int Key, Column, Row;
	static int iFirst = 1;			
	
	if (iFirst)
	{
		iFirst = 0;
	    for (Key = 0; Key < NUM_KEYS; ++Key) {
			_KeyState[Key] = 0;
		}
	}
	
	kbd_scan();

	// Reset state
	for (Key = 0; Key < NUM_KBD_KEYS; ++Key) {
		_NewKeyState[Key] = 0;
	}
	
	// Update state
	for (Column = 0; Column < NUM_KBD_COLUMNS; ++Column) {
		for (Row = 0; Row < NUM_KBD_ROWS; ++Row) {
			if (__kbd_col_state[Column] & (1 << Row)) {
				Key = (Column * NUM_KBD_ROWS) + Row;
				_NewKeyState[Key] = 1;
			}
		}
	}

	// Process key
	for (Key = 0; Key < NUM_KEYS; ++Key) {
		if (_KeyState[Key] != _NewKeyState[Key]) {
			_KeyState[Key] = _NewKeyState[Key];
			// If key pressed
			if (_KeyState[Key]) {
				if (m_bScriptSelectMode == 1)
				{
					// last time around we completed the script activation sequence, so this time we're interpreting the key val as the # of demo to refresh.
					if (Key == 0)
					{
						diag_printf("Key 0, exiting script mode\n");
						m_bScriptSelectMode = 0;
					}
					else if (Key == 1) // codecs image refresh
					{
						int script_len = 0;
						printf("codec refresh selected\n");
						memset((void*) m_sScript, 0, MAX_SCRIPT_SIZE*sizeof(char));	// strlen will be accurate hence unless we overflow the script buffer

						// remove any previous image named as codecs are to be (may fail if they don't exist, but ok)
						sprintf(m_sScript+script_len,"fis delete " CODEC_IMAGE_NAME "\n");	
						script_len = strlen(m_sScript);
															
						// load the codecs into ram
						sprintf(m_sScript+script_len,LOAD_STR " -r -b 0x10000\n");	// std load, plus flags to load raw data (non-srec) to 0x10000
						script_len = strlen(m_sScript);
						
						// burn the codecs down to rom
						sprintf(m_sScript+script_len,"fis create -b 0x10000 -f 0x%x -l 0x%x " CODEC_IMAGE_NAME "\n",CODEC_ROM_START,CODEC_ROM_LEN); 
						script_len = strlen(m_sScript);

						printf("After refreshing the codecs, you'll still be in script mode!\n");
					
						script = m_sScript;
						// I would reprint instructions here, if it wouldn't interfere with the script behavior (serial xfers, after all)
					}
					else if (Key <= DEMO_COUNT + 1)	// one of the demo #s, so a single refresh is requested
					{
						int script_len = 0;
						
						printf("Updating demo image %d: %s\n" , Key, demo_names[Key-2]);

						memset((void*) m_sScript, 0, MAX_SCRIPT_SIZE*sizeof(char));	// strlen will be accurate hence unless we overflow the script buffer
					
						// remove any previous image with the name to be refreshed (may fail if it doesn't exist, but ok)
						sprintf(m_sScript+script_len,"fis delete %s\n", demo_aliases[Key-2]);	// 
						script_len = strlen(m_sScript);

						sprintf(m_sScript+script_len, LOAD_STR "\n");
						script_len = strlen(m_sScript);

						sprintf(m_sScript+script_len, "fis create -b 0x%x -l 0x%x -f 0x%x -e 0x%x -r 0x%x %s\n",
										demo_bases[Key-2],
										demo_lens[Key-2],
										demo_flash_bases[Key-2], 
										demo_bases[Key-2]+demo_entry_offsets[Key-2],
										demo_bases[Key-2],
										demo_aliases[Key-2]);
						script_len = strlen(m_sScript);

						ScriptInstructions();
						printf("After refreshing this demo, you'll still be in script mode!\n");

						script = m_sScript;																
						// I would reprint instructions here, if it wouldn't interfere with the script behavior (serial xfers, after all)
					}	
					else
					{
						// update all images.
						int script_len = 0;
						int i;

						printf("Key %d selected, out of %d options, refreshing all images\n",Key,DEMO_COUNT+1);
						
						memset((void*) m_sScript, 0, MAX_SCRIPT_SIZE*sizeof(char));	// strlen will be accurate hence unless we overflow the script buffer
					
						// erase any previously installed images
						sprintf (m_sScript, "fis init\n");
						script_len = strlen(m_sScript);
		
						// create a dummy image in rom that just serves to allow us to prompt the user to delete it, to punctuate the script behavior
						sprintf (m_sScript+script_len, "fis create -b 0x10000 -e 0x10044 -l 0x20000 -f 0xe07c0000 " FIS_DUMMY_IMAGE_NAME "\n");
						script_len = strlen(m_sScript);
						// load the codecs into ram
						sprintf(m_sScript+script_len,LOAD_STR " -r -b 0x10000\n");	// std load, plus flags to load raw data (non-srec) to 0x10000
						script_len = strlen(m_sScript);
						// burn the codecs down to rom
						sprintf(m_sScript+script_len,"fis create -b 0x10000 -f 0x%x -l 0x%x codecs\n",CODEC_ROM_START,CODEC_ROM_LEN); 
						script_len = strlen(m_sScript);
						// load the demo srecs
						for (i = 0; i < DEMO_COUNT; ++i)
						{
							sprintf(m_sScript+script_len, LOAD_STR "\n");
							script_len = strlen(m_sScript);

							sprintf(m_sScript+script_len, "fis create -b 0x%x -l 0x%x -f 0x%x -e 0x%x -r 0x%x %s\n",
										demo_bases[i],
										demo_lens[i],
										demo_flash_bases[i], 
										demo_bases[i]+demo_entry_offsets[i],
										demo_bases[i],
										demo_aliases[i]);
							script_len = strlen(m_sScript);
						}
						script = m_sScript;																
						m_cSeqStep=0;
						// since the user just updated all images, there's nothing more to update, so exit script mode:
						m_bScriptSelectMode=0;
					}
				}
				else
				{
					// check for script sequence keys
					switch(Key) {
						case (KBD_SEQ_1):
							if (m_cSeqStep==0)
								m_cSeqStep=1;
							else
								m_cSeqStep=0;
							break;
						case (KBD_SEQ_2):
							if (m_cSeqStep==1)
								m_cSeqStep=2;
							else
								m_cSeqStep=0;
							break;
						case (KBD_SEQ_3):
							if (m_cSeqStep==2)
								m_cSeqStep=3;
							else
								m_cSeqStep=0;
							break;
						case (KBD_SEQ_4):
							if (m_cSeqStep==3)
								m_cSeqStep=4;
							else
								m_cSeqStep=0;
							break;
						case (KBD_SEQ_5):
							if (m_cSeqStep==4)
							{
								m_bScriptSelectMode = 1;
								ScriptInstructions();

								m_cSeqStep=0;
							}
							else
								m_cSeqStep=0;
							break;
					}

					// process menu nav keys
					switch(Key) {
						case (KBD_MENU_UP): // up
							// if we're not already at the top
							if(g_ucCurrent > 0){
								if(g_ucHighlighted > 0){
									g_ucCurrent--;
									g_ucHighlighted--;
								}
								else{
									int i;
									g_ucCurrent--;
									g_ucTop--;
									g_ucBottom--;
									// push everything down
									for (i = MENU_ITEM_COUNT-1; i > 0; --i)
									{
										g_psMenu[i] = g_psMenu[i-1];
									}
									g_psMenu[0] = demo_names[g_ucCurrent];
								}
							}
							GumiScreen_DrawMenu();
							break;
						case (KBD_MENU_DOWN): // down
							// if we're not already at the bottom
							if(g_ucCurrent < (DEMO_COUNT - 1)){
								if(g_ucHighlighted < MENU_ITEM_COUNT-1){
									g_ucCurrent++;
									g_ucHighlighted++;
								}
								else{
									int i;
									g_ucCurrent++;
									g_ucTop++;
									g_ucBottom++;
									// push everything up
									for (i = 0; i < MENU_ITEM_COUNT - 1; ++i)
									{
										g_psMenu[i] = g_psMenu[i+1];
									}
									g_psMenu[MENU_ITEM_COUNT-1] = demo_names[g_ucCurrent];
								}
							}
							GumiScreen_DrawMenu();
							break;
						case (KBD_MENU_SELECT): // menu/select
							sprintf(m_sScript,"fis load %s\ngo\n",demo_aliases[g_ucCurrent]);
							script = m_sScript;
							break;
					default:
						{
							//SHUNT_PRINT("unhandled key %d\n", Key);
							break;
						}
					}
				}
			}

		}
	}
}
