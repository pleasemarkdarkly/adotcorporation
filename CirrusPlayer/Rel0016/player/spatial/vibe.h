/*****************************************************************************/
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*****************************************************************************/

struct _vibe 
	{
	long foo[22];
	} typedef VIBE;

/*****************************************************************************/
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*****************************************************************************/

void vibe_process_block(VIBE *thiss,short *left,short *right,long pairs);

/*****************************************************************************/

void vibe_set_gain(VIBE *thiss,long gain);			     // 0 - 0x1000
void vibe_set_effect(VIBE *thiss,long lpf,long virtual); // 0 - 99

/*****************************************************************************/

long vibe_get_gain(VIBE *thiss);
long vibe_get_gain_lpf(VIBE *thiss);
long vibe_get_gain_virt(VIBE *thiss);

/*****************************************************************************/

void vibe_initialize(VIBE *thiss,long rate);

/*****************************************************************************/
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*****************************************************************************/

