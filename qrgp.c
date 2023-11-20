#include "qrcode_encoder.h"
#include "qrgp.h"

VMUINT8 *buffer;
VMWCHAR file_pathw[100];
VMWCHAR editor_title[100];
VMFILE f_read;
VMINT drv;
VMCHAR tekstas[535];
//VMWSTR myPath;
VMINT qVersion = 9;
//VMWSTR path;

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
            //vm_switch_power_saving_mode(turn_off_mode);
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
        //vm_free(path);
        vm_exit_app();
    }

    if (event == VM_KEY_EVENT_UP && keycode == VM_KEY_LEFT_SOFTKEY) {
        vm_selector_run(0, 0, file);
    }

    if (event == VM_KEY_EVENT_UP && keycode == VM_KEY_NUM1) {
        strcpy(tekstas, "");
        vm_ascii_to_ucs2(editor_title, 100, "QR code text:");
        vm_input_set_editor_title((VMWSTR)editor_title);
        vm_input_text3(NULL, 535, 8, get_text);
    }
    if (event == VM_KEY_EVENT_UP && keycode == VM_KEY_NUM2) {
    draw_ucs2_text(file_pathw);
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
    QRCode* old_qrcode = NULL;
    uint8_t* old_qrcodeData = NULL;

    if (!str) {return;}
    if (strlen(str) == 0) {return;}

    //if (strlen(str) > 335) {qVersion = 12;}
    if (strlen(str) > 230) {qVersion = 12;}

    qrcode = vm_calloc(sizeof(QRCode));
    qrcodeDataLen = qrcode_getBufferSize(qVersion);
    qrcodeData = vm_calloc(qrcodeDataLen);

    qrcode_initText(qrcode, qrcodeData, qVersion, ECC_LOW, str);
    //qrcode_initText(qrcode, qrcodeData, qVersion, ECC_HIGH, str);

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
    //draw_ucs2_text(path);

}

void checkFileExist(void) {

   VMCHAR file_path[100];

   VMWCHAR fullPath[100];
   VMWCHAR appName[100];
   VMWCHAR newName[100];
   VMCHAR asciiAppName[100];
   VMCHAR file_name[100];

   vm_get_exec_filename(fullPath);
   vm_get_filename(fullPath, appName);
   vm_ucs2_to_ascii(asciiAppName, 100, appName);
   memcpy(file_name, asciiAppName, strlen(asciiAppName) - 3);
   file_name[strlen(asciiAppName) - 3] = '\0';
   strcat(file_name, "txt");

    if ((drv = vm_get_removable_driver()) < 0) {
        drv = vm_get_system_driver();
    }

    sprintf(file_path, "%c:\\%s", drv, file_name);
    vm_ascii_to_ucs2(file_pathw, (strlen(file_path) + 1) * 2, file_path);
    f_read = vm_file_open(file_pathw, MODE_READ, FALSE);

    if (f_read < 0) {
        vm_file_close(f_read);
        vm_selector_run(0, 0, file);
    } else {

        //size = (strlen(file_pathw) + 1) * 2;
        //path = vm_malloc(size);
        //vm_wstrcpy(path, file_pathw);

        VMCHAR ascii_data[535];
        VMUINT nread;
        vm_file_read(f_read, ascii_data, 535, &nread);
        ascii_data[nread] = '\0';
        vm_file_close(f_read);
        sprintf(tekstas, "%s", ascii_data);
        //set_single_qrcode(tekstas);
    }

}

void get_text(VMINT state, VMWSTR text) {

    VMCHAR new_data[535];
    VMUINT nwrite;
    int lenght;
    struct vm_time_t curr_time;
    VMCHAR fAutoFileName[100];
    VMWCHAR wAutoFileName[101];
    VMINT drv;

    lenght = wstrlen(text);

    if (lenght > 0 && state == VM_INPUT_OK) {

        vm_ucs2_to_ascii(tekstas, lenght + 1, text);
        //set_single_qrcode(tekstas);

        if ((drv = vm_get_removable_driver()) < 0) {
           drv = vm_get_system_driver();
        }
        vm_get_time(&curr_time);
        sprintf(fAutoFileName, "%c:\\%d%d%d%d%d.txt", drv, curr_time.mon, curr_time.day, curr_time.hour, curr_time.min, curr_time.sec);
        vm_ascii_to_ucs2(wAutoFileName, 100, fAutoFileName);

        vm_wstrcpy(file_pathw, wAutoFileName);

        set_single_qrcode(tekstas);

        f_read = vm_file_open(wAutoFileName, MODE_CREATE_ALWAYS_WRITE, FALSE);
        //vm_ascii_to_ucs2(file_pathw, 100, fAutoFileName);
        //f_read = vm_file_open(file_pathw, MODE_CREATE_ALWAYS_WRITE, FALSE);
        vm_chset_convert(VM_CHSET_UCS2, VM_CHSET_UTF8, text, new_data, (lenght + 1) * 2); //src, dst
        vm_file_write(f_read, new_data, strlen(new_data), &nwrite);
        vm_file_close(f_read);
    } else {
        mre_draw_white_rectangle();
        //vm_exit_app();
    }
}

VMINT file(VMWCHAR *FILE_PATH, VMINT wlen) {

    VMCHAR ascii_data[535];
    VMUINT nread;

    //path = vm_realloc(path, (wlen + 1) * 2);
    //vm_wstrcpy(path, FILE_PATH);

    f_read = vm_file_open(FILE_PATH, MODE_READ, FALSE);
    //f_read = vm_file_open(path, MODE_READ, FALSE);

    vm_wstrcpy(file_pathw, FILE_PATH);

    vm_file_read(f_read, ascii_data, 535, &nread);
    ascii_data[nread] = '\0';
    vm_file_close(f_read);
    sprintf(tekstas, "%s", ascii_data);
    //set_single_qrcode(tekstas);
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

    buffer = vm_graphic_get_layer_buffer(layer_hdl[0]);
    vm_graphic_fill_rect(buffer, 0, 248, vm_graphic_get_screen_width(), vm_graphic_get_screen_height(), VM_COLOR_WHITE, VM_COLOR_WHITE);

    vm_graphic_textout(buffer, x, y, ucs2_string, vm_wstrlen(ucs2_string), VM_COLOR_BLACK);
    vm_graphic_flush_layer(layer_hdl, 1);
}