//VMWCHAR pathx[100] = vm_get_default_folder_path(VM_FMGR_DEFAULT_FOLDER_IMAGES);
//vm_start_app(w_filepath, GRP_ID_ROOT, 1);//start up vxp
//vm_graphic_textout(layer_buf[0], 10, 10,vm_ucs2_string("Hello, MRE!"), vm_wstrlen(vm_ucs2_string("Hello, MRE!")), VM_COLOR_RED); 

#include "qrcode_encoder.h"
#include "qrgp.h"

const int BYTES_PER_PIXEL = 3;  /// red, green, & blue
const int FILE_HEADER_SIZE = 14;
const int INFO_HEADER_SIZE = 40;

VMUINT8 *buffer;
VMWCHAR file_pathw[100];
VMWCHAR editor_title[100];
VMFILE f_read;
VMUINT nread;
VMCHAR tekstas[535];
VMINT qVersion = 9;
VMINT trigeris = 1;
VMINT trigeris1 = 0;
VMINT trigeris2 = 0;
VMINT trigeris3 = 0;
VMINT trigeris4 = 0;
VMINT trigeris5 = 0;
VMINT lenght9 = 0;
VMINT lenght12 = 0;
VMINT lenght1 = 0;
VMINT total12a = 0;
VMINT counteris = 0;
int dynamicPosition = 0;
int endFlPosition = 0;

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

    if (event == VM_KEY_EVENT_UP && keycode == VM_KEY_OK) {

       if (trigeris3 == 0 && trigeris4 == 1){
          strcpy(tekstas, "");
          if (counteris != 0 && trigeris5 == 0) {counteris = counteris + 1;};
          if (qVersion == 9) {mre_read_file_chunk(file_pathw, lenght9);}
          if (qVersion == 12) {mre_read_file_chunk(file_pathw, lenght12);}
          set_single_qrcode(tekstas);
       }
    }

    if (event == VM_KEY_EVENT_UP && keycode == VM_KEY_NUM1) {
        strcpy(tekstas, "");
        vm_ascii_to_ucs2(editor_title, 100, "QR code text:");
        vm_input_set_editor_title((VMWSTR)editor_title);
        vm_input_text3(NULL, 535, 8, get_text);
    }

    if (event == VM_KEY_EVENT_UP && keycode == VM_KEY_NUM2) {
       screenshot();
    }

    if (event == VM_KEY_EVENT_UP && keycode == VM_KEY_NUM3) {
       screenshot1();
    }
 
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

    VMINT trigerisb = 0;

    lenght1 = strlen(str);
    if (!str) {return;}
    if (lenght1 == 0) {return;}
    if (lenght1 < 10) {qVersion = 9;}

    qrcode = vm_calloc(sizeof(QRCode));
    qrcodeDataLen = qrcode_getBufferSize(qVersion);
    qrcodeData = vm_calloc(qrcodeDataLen);

    qrcode_initText(qrcode, qrcodeData, qVersion, ECC_LOW, str);

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
    draw_ucs2_text(file_pathw);
    vm_graphic_flush_layer(layer_hdl, 1);
}

void checkFileExist(void) {

    VMWCHAR appTxtName[100];

    create_app_txt_filename(appTxtName, "txt");
    create_auto_full_path_name(file_pathw, appTxtName);

    f_read = vm_file_open(file_pathw, MODE_READ, FALSE);

    if (f_read < 0) {
        vm_file_close(f_read);
        trigeris = vm_selector_run(0, 0, file);
    } else {
        VMCHAR ascii_data[535];
        vm_file_read(f_read, ascii_data, 535, &nread);
        ascii_data[nread] = '\0';
        vm_file_seek(f_read, 0, BASE_END);
        endFlPosition = vm_file_tell(f_read);
        vm_file_close(f_read);

       if (endFlPosition > 230) {

          if (isNumeric(ascii_data, strlen(ascii_data))) {
             lenght9 = 552;
             lenght12 = 883;
          } else if (isAlphanumeric(ascii_data, strlen(ascii_data))) {
             lenght9 = 335;
             lenght12 = 535;
             //vm_vibrator_once();
          } else {
             lenght9 = 230;
             lenght12 = 367;
             //vm_vibrator_once();
          }

          if (lenght9 == 335 && endFlPosition <= 335) {qVersion = 9;} 
          if (lenght9 == 230 && endFlPosition <= 230) {qVersion = 9;} 
          if (lenght12 == 535 && endFlPosition <= 535) {qVersion = 12;}
          if (lenght12 == 367 && endFlPosition <= 367) {qVersion = 12;}
          if (lenght12 == 535 && endFlPosition > 335 || lenght12 == 535 && endFlPosition <= 535) {qVersion = 12;} 
          if (lenght12 == 367 && endFlPosition > 230 || lenght12 == 367 && endFlPosition <= 367) {qVersion = 12;} 
          if (lenght12 == 535 && endFlPosition > 535) {
             qVersion = 12;
             trigeris4 = 1;
             counteris = 1;
             //total12a = endFlPosition / 535;
             total12a = endFlPosition / lenght12;
             //if (((float)endFlPosition / 535) - total12a != 0) {total12a = total12a + 1;}
             if (((float)endFlPosition / lenght12) - total12a != 0) {total12a = total12a + 1;}
          } 
          if (lenght12 == 367 && endFlPosition > 367) {
             qVersion = 12;
             trigeris4 = 1;
             counteris = 1;
             //total12a = endFlPosition / 367;
             total12a = endFlPosition / lenght12;
             //if (((float)endFlPosition / 367) - total12a != 0) {total12a = total12a + 1;}
             if (((float)endFlPosition / lenght12) - total12a != 0) {total12a = total12a + 1;}
         } 

         if (lenght9 == 335 && qVersion == 9) {mre_read_file_chunk(file_pathw, lenght9);}
         if (lenght9 == 230 && qVersion == 9) {mre_read_file_chunk(file_pathw, lenght9);}
         if (lenght12 == 535 && qVersion == 12) {mre_read_file_chunk(file_pathw, lenght12);}
         if (lenght12 == 367 && qVersion == 12) {mre_read_file_chunk(file_pathw, lenght12);}
       } else {
          qVersion = 9;
          lenght9 = 230;
          mre_read_file_chunk(file_pathw, lenght9);
       }
    }

}

void get_text(VMINT state, VMWSTR text) {

    VMCHAR new_data[535];
    VMUINT nwrite;
    int lenght;
    int lenght1;
    VMWCHAR xAutoFileName[100];
    VMWCHAR wAutoFileName[100];

    lenght = wstrlen(text);

    if (lenght > 0 && state == VM_INPUT_OK) {

        counteris = 0;
        trigeris3 = 1;
        trigeris5 = 0;
        //vm_ucs2_to_ascii(tekstas, lenght + 1, text); //--------------------------------------------kaip del UTF8?????????????????????
        create_auto_filename(xAutoFileName, "txt");
        create_auto_full_path_name(wAutoFileName, xAutoFileName);
        vm_wstrcpy(file_pathw, wAutoFileName);

        //if lenght > 230; check tekstas alpha/byte;
        //if (byte12) {tekstas = strsubstr(tekstas , 0, 367);}

        vm_chset_convert(VM_CHSET_UCS2, VM_CHSET_UTF8, (VMCHAR*)text, new_data, (lenght + 1) * 2); //src, dst
        lenght1 = strlen(new_data);


    if (lenght1 > 230) {

          if (isNumeric(new_data, lenght1)) {
             lenght9 = 552;
             lenght12 = 883;
          } else if (isAlphanumeric(new_data, lenght1)) {
             lenght9 = 335;
             lenght12 = 535;
          } else {
             lenght9 = 230;
             lenght12 = 367;
          }

          if (lenght9 == 335 && lenght1 <= 335) {qVersion = 9;} 
          if (lenght12 == 535 && lenght1 <= 535) {qVersion = 12;}
          if (lenght12 == 367 && lenght1 <= 367) {qVersion = 12;}
          if (lenght12 == 535 && lenght1 > 335 || lenght12 == 535 && lenght1 <= 535) {qVersion = 12;} 
          if (lenght12 == 367 && lenght1 > 230 || lenght12 == 367 && lenght1 <= 367) {qVersion = 12;} 
          if (lenght12 == 367 && lenght1 > 367) {
             qVersion = 12;
             //trigeris4 = 1;
             //counteris = 1;
             //new_data1 = strsubstr(new_data , 0, 367);
             strcpy(new_data, strsubstr(new_data , 0, 367));
         } 
          strcpy(tekstas, new_data);
          set_single_qrcode(tekstas);

         //if (lenght9 == 335 && qVersion == 9) {mre_read_file_chunk(file_pathw, lenght9);} //alpha9
         //if (lenght12 == 535 && qVersion == 12) {mre_read_file_chunk(file_pathw, lenght12);} //alpha12
         //if (lenght12 == 367 && qVersion == 12) {mre_read_file_chunk(file_pathw, lenght12);} //byte12
       } else {
          qVersion = 9;
          //lenght9 = 230;
          //mre_read_file_chunk(file_pathw, lenght9);
          strcpy(tekstas, new_data);
          set_single_qrcode(tekstas);
       }

        //set_single_qrcode(tekstas);

        f_read = vm_file_open(wAutoFileName, MODE_CREATE_ALWAYS_WRITE, FALSE);
        vm_file_write(f_read, new_data, strlen(new_data), &nwrite);
        vm_file_close(f_read);
    } else {
        mre_draw_black_rectangle();
    }
}

VMINT file(VMWCHAR *FILE_PATH, VMINT wlen) {

    counteris = 0;
    trigeris = 1;
    trigeris1 = 0;
    trigeris2 = 0;
    trigeris3 = 0;
    trigeris5 = 0;
    total12a = 0;

    lenght12 = 0;
    lenght9 = 0;
    lenght1 = 0;

    VMCHAR ascii_data[535];
    dynamicPosition = 0;
    endFlPosition = 0;

    vm_wstrcpy(file_pathw, FILE_PATH);

    f_read = vm_file_open(file_pathw, MODE_READ, FALSE);

    vm_file_read(f_read, ascii_data, 535, &nread);
    ascii_data[nread] = '\0';
    vm_file_seek(f_read, 0, BASE_END);
    endFlPosition = vm_file_tell(f_read);
    vm_file_close(f_read);

    if (endFlPosition > 230) {

       if (isNumeric(ascii_data, strlen(ascii_data))) {
          lenght9 = 552;
          lenght12 = 883;
       } else if (isAlphanumeric(ascii_data, strlen(ascii_data))) {
          lenght9 = 335;
          lenght12 = 535;
          //vm_vibrator_once();
       } else {
          lenght9 = 230;
          lenght12 = 367;
          //vm_vibrator_once();
       }

       if (lenght9 == 335 && endFlPosition <= 335) {qVersion = 9;} 
       if (lenght9 == 230 && endFlPosition <= 230) {qVersion = 9;} 
       if (lenght12 == 535 && endFlPosition <= 535) {qVersion = 12;}
       if (lenght12 == 367 && endFlPosition <= 367) {qVersion = 12;}
       if (lenght12 == 535 && endFlPosition > 335 || lenght12 == 535 && endFlPosition <= 535) {qVersion = 12;} 
       if (lenght12 == 367 && endFlPosition > 230 || lenght12 == 367 && endFlPosition <= 367) {qVersion = 12;} 
       if (lenght12 == 535 && endFlPosition > 535) {
          qVersion = 12;
          trigeris4 = 1;
          counteris = 1;
          //total12a = endFlPosition / 535;
          total12a = endFlPosition / lenght12;
          //if (((float)endFlPosition / 535) - total12a != 0) {total12a = total12a + 1;}
          if (((float)endFlPosition / lenght12) - total12a != 0) {total12a = total12a + 1;}

       } 
       if (lenght12 == 367 && endFlPosition > 367) {
          qVersion = 12;
          trigeris4 = 1;
          counteris = 1;
          //total12a = endFlPosition / 367;
          total12a = endFlPosition / lenght12;
          //if (((float)endFlPosition / 535) - total12a != 0) {total12a = total12a + 1;}
          if (((float)endFlPosition / lenght12) - total12a != 0) {total12a = total12a + 1;}
       } 

       if (lenght9 == 335 && qVersion == 9) {mre_read_file_chunk(file_pathw, lenght9);}
       if (lenght9 == 230 && qVersion == 9) {mre_read_file_chunk(file_pathw, lenght9);}
       if (lenght12 == 535 && qVersion == 12) {mre_read_file_chunk(file_pathw, lenght12);}
       if (lenght12 == 367 && qVersion == 12) {mre_read_file_chunk(file_pathw, lenght12);}
    } else {
        qVersion = 9;
        lenght9 = 230;
        mre_read_file_chunk(file_pathw, lenght9);
    }  

    //lenght12 = 0;
    //lenght9 = 0;
    lenght1 = 0; //???????????????????????????????????????????????????????????????????????????????????????????????????????
    mre_draw_white_rectangle();
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
    VMCHAR text3[100];
    VMWCHAR text1[100];

    sprintf(text3, "%d/%d", counteris, total12a);
    vm_ascii_to_ucs2(text1, (strlen(text3) + 1) *2 , text3);

    int wstr_len1 = vm_graphic_get_string_width(text1);
    int x1 = (vm_graphic_get_screen_width() - wstr_len1) / 2;

    buffer = vm_graphic_get_layer_buffer(layer_hdl[0]);
    vm_graphic_fill_rect(buffer, 0, 248, vm_graphic_get_screen_width(), vm_graphic_get_screen_height(), VM_COLOR_WHITE, VM_COLOR_WHITE);

    if (counteris > 0) {
    vm_graphic_textout(buffer, x1, y - (font_height * 2) , text1, 8, VM_COLOR_BLACK);
    }
    vm_graphic_textout(buffer, x, y, ucs2_string, vm_wstrlen(ucs2_string), VM_COLOR_BLACK);
    //vm_graphic_flush_layer(layer_hdl, 1);
}

void create_app_txt_filename(VMWSTR text, VMSTR extt) {

    VMWCHAR fullPath[100];
    VMWCHAR appName[100];
    VMWCHAR wfile_extension[4];

    vm_get_exec_filename(fullPath);
    vm_get_filename(fullPath, appName);
    vm_ascii_to_ucs2(wfile_extension, 8, extt);
    vm_wstrncpy(text, appName, vm_wstrlen(appName) - 3);
    vm_wstrcat(text, wfile_extension);

}

void create_auto_filename(VMWSTR text, VMSTR extt) {

    struct vm_time_t curr_time;
    VMCHAR fAutoFileName[100];

    vm_get_time(&curr_time);
    sprintf(fAutoFileName, "%02d%02d%02d%02d%02d.%s", curr_time.mon, curr_time.day, curr_time.hour, curr_time.min, curr_time.sec, extt);
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

VMINT mre_read_file_chunk(VMWSTR path, VMINT chunk) {

    int myFlPosBackCurr = 0;

    //strcpy(tekstas, "");

    f_read = vm_file_open(path, MODE_READ, FALSE);
    vm_file_seek(f_read, dynamicPosition, BASE_CURR);  // Permetame kursoriu i pozicija
    myFlPosBackCurr = dynamicPosition;   
    vm_file_read(f_read, tekstas, chunk, &nread);
    tekstas[nread] = '\0';
    dynamicPosition = vm_file_tell(f_read);  // Issaugome dabartine kursoriaus pozicija faile

    if (vm_file_is_eof(f_read)) { trigeris3 = 1; }

    //dynamicPosition = myFlPosBackCurr;

    if (endFlPosition == dynamicPosition) {
       //dynamicPosition = myFlPosBackCurr;
       trigeris5 = 1;
    } 
    vm_file_close(f_read);
    return 0;
}

void mre_draw_black_rectangle(void) {

    buffer = vm_graphic_get_layer_buffer(layer_hdl[0]);
    vm_graphic_fill_rect(buffer, 0, 0, vm_graphic_get_screen_width(),
                         vm_graphic_get_screen_height(), VM_COLOR_BLACK,
                         VM_COLOR_BLACK);
    vm_graphic_flush_layer(layer_hdl, 1);
}

unsigned char* createBitmapFileHeader(int fileSize) {

    //const int FILE_HEADER_SIZE = 14;
    //const int INFO_HEADER_SIZE = 40;

    // int fileSize = FILE_HEADER_SIZE + INFO_HEADER_SIZE + (stride * height);

    static unsigned char fileHeader[] = {
        0, 0,        /// signature
        0, 0, 0, 0,  /// image file size in bytes
        0, 0, 0, 0,  /// reserved
        0, 0, 0, 0,  /// start of pixel array
    };

    fileHeader[0] = (unsigned char)('B');
    fileHeader[1] = (unsigned char)('M');
    fileHeader[2] = (unsigned char)(fileSize);
    fileHeader[3] = (unsigned char)(fileSize >> 8);
    fileHeader[4] = (unsigned char)(fileSize >> 16);
    fileHeader[5] = (unsigned char)(fileSize >> 24);
    fileHeader[10] = (unsigned char)(FILE_HEADER_SIZE + INFO_HEADER_SIZE);

    return fileHeader;
}

unsigned char* createBitmapInfoHeader(int height, int width) {

    //const int INFO_HEADER_SIZE = 40;
    //const int BYTES_PER_PIXEL = 3;

    static unsigned char infoHeader[] = {
        0, 0, 0, 0,  /// header size
        0, 0, 0, 0,  /// image width
        0, 0, 0, 0,  /// image height
        0, 0,        /// number of color planes
        0, 0,        /// bits per pixel
        0, 0, 0, 0,  /// compression
        0, 0, 0, 0,  /// image size
        0, 0, 0, 0,  /// horizontal resolution
        0, 0, 0, 0,  /// vertical resolution
        0, 0, 0, 0,  /// colors in color table
        0, 0, 0, 0,  /// important color count
    };

    infoHeader[0] = (unsigned char)(INFO_HEADER_SIZE);
    infoHeader[4] = (unsigned char)(width);
    infoHeader[5] = (unsigned char)(width >> 8);
    infoHeader[6] = (unsigned char)(width >> 16);
    infoHeader[7] = (unsigned char)(width >> 24);
    infoHeader[8] = (unsigned char)(height);
    infoHeader[9] = (unsigned char)(height >> 8);
    infoHeader[10] = (unsigned char)(height >> 16);
    infoHeader[11] = (unsigned char)(height >> 24);
    infoHeader[12] = (unsigned char)(1);
    infoHeader[14] = (unsigned char)(BYTES_PER_PIXEL * 8);

    return infoHeader;
}

int screenshot(void) {

    //const int FILE_HEADER_SIZE = 14;
    //const int INFO_HEADER_SIZE = 40;
    //VMFILE f_read;

    VMWCHAR s[100];
    VMWCHAR e[100];
    char ss[100];
    int i;
    int j;
    VMUINT p;

    create_auto_filename(e, "bmp");
    create_auto_full_path_name(s, e);

    f_read = vm_file_open((VMWSTR)s, MODE_CREATE_ALWAYS_WRITE, 1);
    int w = vm_graphic_get_screen_width(), h = vm_graphic_get_screen_height();

    vm_file_write(f_read, createBitmapFileHeader(FILE_HEADER_SIZE + INFO_HEADER_SIZE + vm_graphic_get_screen_width() * vm_graphic_get_screen_height() * 3), FILE_HEADER_SIZE, &p);
    vm_file_write(f_read, createBitmapInfoHeader(vm_graphic_get_screen_height(), vm_graphic_get_screen_width()), INFO_HEADER_SIZE, &p);

    for (i = h - 1; i > -1; --i) {
        for (j = 0; j < w; ++j) {
            long long tmp =  VM_COLOR_565_TO_888(((unsigned short*)buffer)[i * w + j]);
            vm_file_write(f_read, &tmp, 3, &p);
        }
    }
    vm_file_close(f_read);
    return 0;
}

int screenshot1(void) {

    VMWCHAR s[100];
    VMWCHAR e[100];
    char ss[100];
    int i, j, q, w, h;
    VMUINT p;

    //const int FILE_HEADER_SIZE = 14;
    //const int INFO_HEADER_SIZE = 40;
    //VMFILE f_read;
    //VMUINT8 *buffer;

    create_auto_filename(e, "bmp");
    create_auto_full_path_name(s, e);

    f_read = vm_file_open((VMWSTR)s, MODE_CREATE_ALWAYS_WRITE, 1);
    w = vm_graphic_get_screen_width();
    h = vm_graphic_get_screen_height();

    vm_file_write(f_read, createBitmapFileHeader(FILE_HEADER_SIZE + INFO_HEADER_SIZE + vm_graphic_get_screen_width() * vm_graphic_get_screen_height() * 3), FILE_HEADER_SIZE, &p);
    vm_file_write(f_read, createBitmapInfoHeader(vm_graphic_get_screen_height(), vm_graphic_get_screen_width()), INFO_HEADER_SIZE, &p);

    // static unsigned char *temp_line_buf = vm_malloc(w * 3);
    unsigned char *temp_line_buf = vm_malloc(w * 3);

    for (i = h - 1; i > -1; --i) {

        for (j = 0; j < w; ++j) {

            unsigned long tmp = VM_COLOR_565_TO_888(((unsigned short *)buffer)[i * w + j]);
            unsigned char *tmp_b = (unsigned char *)&tmp;

            for (q = 0; q < 3; ++q) {

                temp_line_buf[j * 3 + q] = tmp_b[q];
            }
        }
        vm_file_write(f_read, &temp_line_buf, w * 3, &p);
    }

    vm_file_close(f_read);

    vm_free(temp_line_buf);
    temp_line_buf = NULL;

    return 0;
}