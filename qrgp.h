#ifndef _VRE_APP_WIZARDTEMPLATE_
#define	_VRE_APP_WIZARDTEMPLATE_

#include "vmsys.h"
#include "vmio.h"
#include "vmgraph.h"
#include "vmchset.h"
#include "vmstdlib.h"
#include "stdio.h"
#include "vmsms.h"
#include <string.h>
#include "stdint.h"
#include <time.h>
#include "qrcode_encoder.h"
//#include "vmmm.h"
//#include "vmtimer.h"

/* ---------------------------------------------------------------------------
 * global variables
 * ------------------------------------------------------------------------ */

VMINT		layer_hdl[1];				////layer handle array.

/* ---------------------------------------------------------------------------
 * local variables
 * ------------------------------------------------------------------------ */

void handle_sysevt(VMINT message, VMINT param);
void handle_keyevt(VMINT event, VMINT keycode);
void handle_penevt(VMINT event, VMINT x, VMINT y);
void set_single_qrcode(char* str);
void checkFileExist(void);
void get_text(VMINT state, VMWSTR text);
VMINT file(VMWCHAR *FILE_PATH, VMINT wlen);
//VMINT file1(VMWCHAR *FILE_PATH, VMINT wlen);
void mre_draw_white_rectangle(void);
void draw_ucs2_text(VMWSTR ucs2_string);
void create_app_txt_filename(VMWSTR text, VMSTR extt);
void create_auto_filename(VMWSTR text, VMSTR extt);
void create_auto_full_path_name(VMWSTR result, VMWSTR fname);
static int getAlphanumeric(char c);
static bool isAlphanumeric(const char *text, int length);
static bool isNumeric(const char *text, int length);
char * strsubstr(char * str , int from, int count);
VMINT mre_read_file_chunk(VMWSTR path, VMINT chunk);
void mre_draw_black_rectangle(void);
unsigned char* createBitmapFileHeader(int fileSize);
unsigned char* createBitmapInfoHeader(int height, int width);
void mre_read_sms();
static void mre_sms_read_callback_function(vm_sms_callback_t *callback_data);
void write_uint32_le(unsigned char *buf, unsigned int val);
int screenshot_grayscale_8bit(void);
//void timer(int a);
#endif

