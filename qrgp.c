#include "qrcode_encoder.h"
#include "qrgp.h"

VMUINT8 *buffer;
VMWCHAR file_pathw[100];
VMWCHAR editor_title[100];
VMFILE f_read;
VMCHAR tekstas[535];
VMINT qVersion = 9;
VMINT trigeris = 1;
VMINT trigeris1 = 0;
VMINT trigeris2 = 0;
VMINT lenght12 = 0;
VMINT lenght1 = 0;
char * uodega;
VMINT uodeg_trg = 0;

void vm_main(void) {

    layer_hdl[0] = -1;
    vm_reg_sysevt_callback(handle_sysevt);
    vm_reg_keyboard_callback(handle_keyevt);
    vm_reg_pen_callback(handle_penevt);
    vm_font_set_font_size(VM_SMALL_FONT);
    checkFileExist();

}

void handle_sysevt(VMINT message, VMINT param) {

    switch (message) {
        case VM_MSG_CREATE:
        case VM_MSG_ACTIVE:

            layer_hdl[0] = vm_graphic_create_layer(0, 0, vm_graphic_get_screen_width(),
                                        vm_graphic_get_screen_height(), -1);
            vm_graphic_set_clip(0, 0, vm_graphic_get_screen_width(),
                                vm_graphic_get_screen_height());
            vm_switch_power_saving_mode(turn_off_mode);
            break;

        case VM_MSG_PAINT:
            vm_switch_power_saving_mode(turn_off_mode);
            if (trigeris1 == 1) {trigeris2 = 1;}
            if (trigeris == 0) {trigeris1 = 1;}
            if (trigeris2 == 1) {vm_exit_app();}
            set_single_qrcode(tekstas);
            break;

        case VM_MSG_INACTIVE:
            vm_switch_power_saving_mode(turn_on_mode);
            if (layer_hdl[0] != -1) vm_graphic_delete_layer(layer_hdl[0]);

            break;
        case VM_MSG_QUIT:
            if (layer_hdl[0] != -1) vm_graphic_delete_layer(layer_hdl[0]);
            break;
    }

}

void handle_keyevt(VMINT event, VMINT keycode) {

    if (event == VM_KEY_EVENT_UP && keycode == VM_KEY_RIGHT_SOFTKEY) {
        if (layer_hdl[0] != -1) {
            vm_graphic_delete_layer(layer_hdl[0]);
            layer_hdl[0] = -1;
        }
        vm_exit_app();
    }

    if (event == VM_KEY_EVENT_UP && keycode == VM_KEY_LEFT_SOFTKEY) {
        vm_selector_run(0, 0, file);
    }

    if (event == VM_KEY_EVENT_UP && keycode == VM_KEY_DOWN) {
       if (lenght12 == 367 && lenght1 > lenght12) {
          uodega = strsubstr((char *)tekstas , 367, lenght1);
          if (strlen(uodega) > 10){qVersion = 12;}
          uodeg_trg = 1;
          strcpy(tekstas, uodega);
          set_single_qrcode(tekstas);
          //set_single_qrcode(uodega);
       }
    }

    if (event == VM_KEY_EVENT_UP && keycode == VM_KEY_NUM1) {
        strcpy(tekstas, "");
        vm_ascii_to_ucs2(editor_title, 100, "QR code text:");
        vm_input_set_editor_title((VMWSTR)editor_title);
        vm_input_text3(NULL, 535, 8, get_text);
    }

    //if (event == VM_KEY_EVENT_UP && keycode == VM_KEY_NUM2) {
    //   set_single_qrcode(uodega);
    //}
 
}

void handle_penevt(VMINT event, VMINT x, VMINT y) {

    if (layer_hdl[0] != -1) {
        vm_graphic_delete_layer(layer_hdl[0]);
        layer_hdl[0] = -1;
    }

    vm_exit_app();

}

void set_single_qrcode(char* str) {

    QRCode* qrcode = NULL;
    uint8_t* qrcodeData = NULL;
    size_t qrcodeDataLen = 0;
    QRCode* old_qrcode = NULL;
    uint8_t* old_qrcodeData = NULL;

    VMCHAR str1[367];
    //VMINT lenght1 = 0;
    VMINT lenght9 = 0;
    //VMINT lenght12 = 0;
    VMINT trigerisb = 0;

    lenght1 = strlen(str);
    if (!str) {return;}
    if (lenght1 == 0) {return;}

    if (isNumeric(str, 100)) {
       lenght9 = 552;
       lenght12 = 883;
    } else if (isAlphanumeric(str, 100)) {
       lenght9 = 335;
       lenght12 = 535;
       //vm_vibrator_once();
    } else {
       lenght9 = 230;
       lenght12 = 367;
       //vm_vibrator_once();
    }

    if (lenght1 > 230) {
       qVersion = 12;
       //vm_wstrncpy((VMWSTR)str1, (VMWSTR)str, 366);
       strncpy(str1, str, 367);
       str1[strlen(str1)] = '\0';
       trigerisb = 1;
    }

    qrcode = vm_calloc(sizeof(QRCode));
    qrcodeDataLen = qrcode_getBufferSize(qVersion);
    qrcodeData = vm_calloc(qrcodeDataLen);

    if (trigerisb == 1) {
    qrcode_initText(qrcode, qrcodeData, qVersion, ECC_LOW, str1);
    } else {
    qrcode_initText(qrcode, qrcodeData, qVersion, ECC_LOW, str);
    }

    uint8_t x = 0;
    uint8_t y = 0;

    buffer = vm_graphic_get_layer_buffer(layer_hdl[0]);
    vm_graphic_fill_rect(buffer, 0, 0, vm_graphic_get_screen_width(), vm_graphic_get_screen_height(), VM_COLOR_WHITE, VM_COLOR_WHITE);

    for (y = 0; y < qrcode->size; y++) {
        for (x = 0; x < qrcode->size; x++) {
            VMBOOL is_pixel_black = qrcode_getModule(qrcode, x, y);
            if (is_pixel_black) {
               if (qVersion == 12) {vm_graphic_fill_rect(buffer, 1 + 0 + x*3.66, 1 + 0 + y*3.66, 4, 4, VM_COLOR_BLACK, VM_COLOR_BLACK);}
               if (qVersion == 9) {vm_graphic_fill_rect(buffer, 1 + 0 + x * 4.49, 1 + 0 + y * 4.49, 5, 5, VM_COLOR_BLACK, VM_COLOR_BLACK);}
            }
        }
    }
    vm_graphic_flush_layer(layer_hdl, 1);
    qVersion = 9;
    draw_ucs2_text(file_pathw);

}

void checkFileExist(void) {

    //VMWCHAR file_pathw[100];
    VMWCHAR appTxtName[100];

    create_app_txt_filename(appTxtName);
    create_auto_full_path_name(file_pathw, appTxtName);

    f_read = vm_file_open(file_pathw, MODE_READ, FALSE);

    if (f_read < 0) {
        vm_file_close(f_read);
        trigeris = vm_selector_run(0, 0, file);
    } else {
        VMCHAR ascii_data[535];
        VMUINT nread;
        vm_file_read(f_read, ascii_data, 535, &nread);
        ascii_data[nread] = '\0';
        vm_file_close(f_read);
        sprintf(tekstas, "%s", ascii_data);
    }

}

void get_text(VMINT state, VMWSTR text) {

    VMCHAR new_data[535];
    VMUINT nwrite;
    int lenght;
    VMWCHAR xAutoFileName[100];
    VMWCHAR wAutoFileName[100];

    lenght = wstrlen(text);

    if (lenght > 0 && state == VM_INPUT_OK) {

        vm_ucs2_to_ascii(tekstas, lenght + 1, text); //--------------------------------------------kaip del UTF8?????????????????????
        create_auto_filename(xAutoFileName);
        create_auto_full_path_name(wAutoFileName, xAutoFileName);
        vm_wstrcpy(file_pathw, wAutoFileName);

        set_single_qrcode(tekstas);

        f_read = vm_file_open(wAutoFileName, MODE_CREATE_ALWAYS_WRITE, FALSE);
        vm_chset_convert(VM_CHSET_UCS2, VM_CHSET_UTF8, (VMCHAR*)text, new_data, (lenght + 1) * 2); //src, dst
        vm_file_write(f_read, new_data, strlen(new_data), &nwrite);
        vm_file_close(f_read);
    } else {
        mre_draw_white_rectangle();
    }
}

VMINT file(VMWCHAR *FILE_PATH, VMINT wlen) {

    trigeris = 1;
    trigeris1 = 0;
    trigeris2 = 0;
    VMCHAR ascii_data[535];
    VMUINT nread;

    f_read = vm_file_open(FILE_PATH, MODE_READ, FALSE);

    vm_wstrcpy(file_pathw, FILE_PATH);

    vm_file_read(f_read, ascii_data, 535, &nread);
    ascii_data[nread] = '\0';
    vm_file_close(f_read);
    sprintf(tekstas, "%s", ascii_data);
    //set_single_qrcode(tekstas);
    lenght12 = 0;
    lenght1 = 0;
    uodeg_trg = 0;
    return 0;
}

void mre_draw_white_rectangle(void) {

    buffer = vm_graphic_get_layer_buffer(layer_hdl[0]);
    vm_graphic_fill_rect(buffer, 0, 0, vm_graphic_get_screen_width(),
                         vm_graphic_get_screen_height(), VM_COLOR_WHITE,
                         VM_COLOR_WHITE);
    vm_graphic_flush_layer(layer_hdl, 1);
}

void draw_ucs2_text(VMWSTR ucs2_string) {

    int font_height = 16;
    int wstr_len = vm_graphic_get_string_width(ucs2_string);

    int x = (vm_graphic_get_screen_width() - wstr_len) / 2;
    int y = vm_graphic_get_screen_height() - font_height;
    VMWCHAR text1[8];
    VMWCHAR text2[8];

    vm_ascii_to_ucs2(text1, 8, "1/2");
    vm_ascii_to_ucs2(text2, 8, "2/2");

    int wstr_len1 = vm_graphic_get_string_width(text1);
    int wstr_len2 = vm_graphic_get_string_width(text2);
    int x1 = (vm_graphic_get_screen_width() - wstr_len1) / 2;
    int x2 = (vm_graphic_get_screen_width() - wstr_len2) / 2;

    //if (lenght12 == 367 && lenght1 > lenght12 && strlen(uodega) == 0) {wstrcat(ucs2_string, text1);}
    //if (lenght12 == 367 && lenght1 > lenght12 && strlen(uodega) > 0) {wstrcat(ucs2_string, text2);}

    buffer = vm_graphic_get_layer_buffer(layer_hdl[0]);
    vm_graphic_fill_rect(buffer, 0, 248, vm_graphic_get_screen_width(), vm_graphic_get_screen_height(), VM_COLOR_WHITE, VM_COLOR_WHITE);

    if (lenght12 == 367 && lenght1 > lenght12 && strlen(uodega) == 0) {
    vm_graphic_textout(buffer, x1, y - (font_height * 2) , text1, 8, VM_COLOR_BLACK);
    }
    if (lenght12 == 367 && lenght1 > lenght12 && strlen(uodega) > 0) {
    vm_graphic_textout(buffer, x1, y - (font_height * 2) , text1, 8, VM_COLOR_BLACK);
    }
    if (uodeg_trg == 1) {
    vm_graphic_textout(buffer, x2, y - (font_height * 2) , text2, 8, VM_COLOR_BLACK);
    }
    vm_graphic_textout(buffer, x, y, ucs2_string, vm_wstrlen(ucs2_string), VM_COLOR_BLACK);
    vm_graphic_flush_layer(layer_hdl, 1);
}

void create_app_txt_filename(VMWSTR text) {

    VMWCHAR fullPath[100];
    VMWCHAR appName[100];
    VMCHAR asciiAppName[100];
    VMCHAR file_name[100];

    vm_get_exec_filename(fullPath);
    vm_get_filename(fullPath, appName);
    vm_ucs2_to_ascii(asciiAppName, wstrlen(appName) + 1, appName);
    memcpy(file_name, asciiAppName, strlen(asciiAppName) - 3);
    file_name[strlen(asciiAppName) - 3] = '\0';
    strcat(file_name, "txt");
    vm_ascii_to_ucs2(text, (strlen(file_name) + 1) * 2, file_name);

}

void create_auto_filename(VMWSTR text) {

    struct vm_time_t curr_time;
    VMCHAR fAutoFileName[100];

    vm_get_time(&curr_time);
    sprintf(fAutoFileName, "%02d%02d%02d%02d%02d.txt", curr_time.mon, curr_time.day, curr_time.hour, curr_time.min, curr_time.sec);
    vm_ascii_to_ucs2(text, (strlen(fAutoFileName) + 1) * 2, fAutoFileName);

}

void create_auto_full_path_name(VMWSTR result, VMWSTR fname) {

    VMINT drv;
    VMCHAR fAutoFileName[100];
    VMWCHAR wAutoFileName[100];

    if ((drv = vm_get_removable_driver()) < 0) {
       drv = vm_get_system_driver();
    }

    sprintf(fAutoFileName, "%c:\\", drv);
    vm_ascii_to_ucs2(wAutoFileName, (strlen(fAutoFileName) + 1) * 2, fAutoFileName);
    vm_wstrcat(wAutoFileName, fname);
    vm_wstrcpy(result, wAutoFileName);

}

//static int8_t getAlphanumeric(char c) {
static int getAlphanumeric(char c) {
    
    if (c >= '0' && c <= '9') { return (c - '0'); }
    if (c >= 'A' && c <= 'Z') { return (c - 'A' + 10); }
    
    switch (c) {
        case ' ': return 36;
        case '$': return 37;
        case '%': return 38;
        case '*': return 39;
        case '+': return 40;
        case '-': return 41;
        case '.': return 42;
        case '/': return 43;
        case ':': return 44;
    }
    
    return -1;
}

//static bool isAlphanumeric(const char *text, uint16_t length) {
static bool isAlphanumeric(const char *text, int length) {
    while (length != 0) {
        if (getAlphanumeric(text[--length]) == -1) { return false; }
    }
    return true;
}

//static bool isNumeric(const char *text, uint16_t length) {
static bool isNumeric(const char *text, int length) {
    while (length != 0) {
        char c = text[--length];
        if (c < '0' || c > '9') { return false; }
    }
    return true;
}

char * strsubstr(char * str , int from, int count) {
    char * result;

    if(str == NULL) return NULL;

    result = vm_malloc((count+1) * sizeof(char));

    if(result == NULL) return NULL;

    strncpy(result, str+from, count);
    result[count] = '\0';

    vm_free(result);
    return result;
}