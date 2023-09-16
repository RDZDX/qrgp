#include "qrcode_encoder.h"
#include "qrcode.h"

VMCHAR file_name[100] = "newqr.txt";
VMWCHAR file_pathw[101];
VMFILE f_read;
VMINT drv;
VMINT trigeris = 0;
VMWSTR data_str;
VMCHAR tekstas[2000];

void vm_main(void) {

	layer_hdl[0] = -1;	
	vm_reg_sysevt_callback(handle_sysevt);
	vm_reg_keyboard_callback(handle_keyevt);
	vm_reg_pen_callback(handle_penevt);
        checkFileExist();

}

void handle_sysevt(VMINT message, VMINT param) {
	switch (message) {
	case VM_MSG_CREATE:
	case VM_MSG_ACTIVE:

		layer_hdl[0] = vm_graphic_create_layer(0, 0, vm_graphic_get_screen_width(),
			vm_graphic_get_screen_height(),	-1);

		vm_graphic_set_clip(0, 0, 
			vm_graphic_get_screen_width(), vm_graphic_get_screen_height());
                vm_switch_power_saving_mode(turn_off_mode);
		break;
		
	case VM_MSG_PAINT:
                vm_switch_power_saving_mode(turn_off_mode);
                //checkFileExist();
                set_single_qrcode(tekstas);
		break;
		
	case VM_MSG_INACTIVE:
                vm_switch_power_saving_mode(turn_on_mode);
		if( layer_hdl[0] != -1 )
			vm_graphic_delete_layer(layer_hdl[0]);
		
		break;	
	case VM_MSG_QUIT:
		if( layer_hdl[0] != -1 )
			vm_graphic_delete_layer(layer_hdl[0]);
		vm_res_deinit();
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
       set_single_qrcode(tekstas);
    }

}

void handle_penevt(VMINT event, VMINT x, VMINT y)
{

	if( layer_hdl[0] != -1 )
	{
		vm_graphic_delete_layer(layer_hdl[0]);
		layer_hdl[0] = -1;
	}
	
	vm_exit_app();
}

//void set_single_qrcode(char* str, int view_enum) {
void set_single_qrcode(char* str) {

	QRCode *qrcode = NULL;
	uint8_t *qrcodeData = NULL;
	size_t qrcodeDataLen = 0;

	QRCode* old_qrcode = NULL;
	uint8_t* old_qrcodeData = NULL;

	if (!str)
	{
		return;
	}
	
	qrcode = vm_calloc(sizeof(QRCode));
	qrcodeDataLen = qrcode_getBufferSize(QRCODE_VERSION);
	qrcodeData = vm_calloc(qrcodeDataLen);

    qrcode_initText(qrcode, qrcodeData, QRCODE_VERSION, ECC_LOW, str);
  
        uint8_t x = 0;
        uint8_t y = 0;
        //VMINT x = 0;
        //VMINT y = 0;


        void* buffer = vm_graphic_get_layer_buffer(layer_hdl[0]);
        //vm_graphic_fill_rect(buffer, horizontal_offset, vertical_offset, width, height, VM_COLOR_WHITE, VM_COLOR_WHITE);
        vm_graphic_fill_rect(buffer, 0, 0, vm_graphic_get_screen_width(), vm_graphic_get_screen_height(), VM_COLOR_WHITE, VM_COLOR_WHITE);

        for (y = 0; y < qrcode->size; y++) {
            for (x = 0; x < qrcode->size; x++) {
                VMBOOL is_pixel_black = qrcode_getModule(qrcode, x, y);
                if (is_pixel_black) {
                   //vm_graphic_fill_rect(buffer, horizontal_offset + 4 + x*4, vertical_offset + 4 + y*4, 4, 4, VM_COLOR_BLACK, VM_COLOR_BLACK);
                   //vm_graphic_fill_rect(buffer, 0 + 4 + x*4, 0 + 4 + y*4, 4, 4, VM_COLOR_BLACK, VM_COLOR_BLACK);
                   vm_graphic_fill_rect(buffer, 9 + 4 + x*4, 9 + 4 + y*4, 4, 4, VM_COLOR_BLACK, VM_COLOR_BLACK);
                }
            }
       }
       vm_graphic_flush_layer(layer_hdl, 1);

}

void checkFileExist(void) {

    VMCHAR file_path[100];

    if ((drv = vm_get_removable_driver()) < 0) {
        drv = vm_get_system_driver();
    }

    sprintf(file_path, "%c:\\%s", drv, file_name);
    vm_ascii_to_ucs2(file_pathw, 100, file_path);
    vm_input_set_editor_title((VMWSTR)file_pathw);
    f_read = vm_file_open(file_pathw, MODE_READ, FALSE);

    if (f_read < 0) {
        vm_file_close(f_read);
        vm_input_text3(NULL, 2000, 8, save_text);
    } else {
        VMCHAR ascii_data[1999];
        VMUINT nread;
        vm_file_read(f_read, ascii_data, 1999, &nread);
        ascii_data[nread] = '\0';
        vm_file_close(f_read);
        sprintf(tekstas, "%s", ascii_data);
    }
    set_single_qrcode(tekstas);
}

void save_text(VMINT state, VMWSTR text) {

    if (text != NULL && state == VM_INPUT_OK) {
       vm_ucs2_to_ascii(tekstas, 2000, (VMWSTR)text);
       set_single_qrcode(tekstas);
    }
}