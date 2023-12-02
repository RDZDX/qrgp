#ifndef _VRE_APP_WIZARDTEMPLATE_
#define	_VRE_APP_WIZARDTEMPLATE_

#define MRE_STR_SIZE_MAX            (100)
#define MRE_SCREEN_START_X 		(0)
#define MRE_SCREEN_START_Y 		(0)

#include "vmsys.h"
#include "vmio.h"
#include "vmgraph.h"
#include "vmchset.h"
#include "vmstdlib.h"
//#include "ResID.h"
//#include "vm4res.h"
#include <string.h>
#include <time.h>
//#include "vmmm.h"

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
void mre_draw_white_rectangle(void);
void draw_ucs2_text(VMWSTR ucs2_string);
void create_app_txt_filename(VMWSTR text);
void create_auto_filename(VMWSTR text);
void create_auto_full_path_name(VMWSTR result, VMWSTR fname);
static int getAlphanumeric(char c);
static bool isAlphanumeric(const char *text, int length);
static bool isNumeric(const char *text, int length);
char * strsubstr(char * str , int from, int count);

#endif

