#include "pogo_keyboard.h"


#define KEYBOARD_NUM_KEYS   256
#define KEYBOARD_MM_NUM_KEYS 20
#define unk	KEY_UNKNOWN


// implementations below may be referenced from usb keyboard kernel drvier(usbkbd.c).
// pogo_keyboard is similar to usb_kbd_keycode(usbkbd.c)
static const unsigned char pogo_keyboard[KEYBOARD_NUM_KEYS] = {
      0,  0,  0,  0, 30, 48, 46, 32, 18, 33, 34, 35, 23, 36, 37, 38,
     50, 49, 24, 25, 16, 19, 31, 20, 22, 47, 17, 45, 21, 44,  2,  3,
      4,  5,  6,  7,  8,  9, 10, 11, 28,  1, 14, 15, 57, 12, 13, 26,
     27, 43, 43, 39, 40, 41, 51, 52, 53, 58, 59, 60, 61, 62, 63, 64,
     65, 66, 67, 68, 87, 88, 99, 70,119,110,102,104,111,107,109,106,
    105,108,103, 69, 98, 55, 74, 78, 96, 79, 80, 81, 75, 76, 77, 71,
     72, 73, 82, 83, 86,127,116,117,183,184,185,186,187,188,189,190,
    191,192,193,194,134,138,130,132,128,129,131,137,133,135,136,113,
    115,114,unk,unk,unk,121,unk, 89, 93,124, 92, 94, 95,unk,unk,unk,
    122,123, 90, 91, 85,unk,unk,unk,unk,unk,unk,unk,111,unk,unk,unk,
    unk,unk,unk,unk,unk,unk,unk,unk,unk,unk,unk,unk,unk,unk,unk,unk,
    unk,unk,unk,unk,unk,unk,179,180,unk,unk,unk,unk,unk,unk,unk,unk,
    unk,unk,unk,unk,unk,unk,unk,unk,unk,unk,unk,unk,unk,unk,unk,unk,
    unk,unk,unk,unk,unk,unk,unk,unk,111,unk,unk,unk,unk,unk,unk,unk,
     29, 42, 56,125, 97, 54,100,126,164,166,165,163,161,115,114,113,
    150,158,159,128,136,177,178,176,142,152,173,140,unk,unk,unk,unk
};



/*0x224,0xe2,0xea,0xe9,0x391,0x70,0x6f,0x392,0x394,0x395,0x397,0x38e,0x398,0x399,0x390*/
static const unsigned short mm_pogo_keyboard[KEYBOARD_MM_NUM_KEYS][2] = {
    {0x70,   KEY_BRIGHTNESSDOWN},
    {0x6f,   KEY_BRIGHTNESSUP},
    {0xe2,   KEY_MUTE},
    {0xea,   KEY_VOLUMEDOWN},
    {0xe9,   KEY_VOLUMEUP},
    {0x224,   KEY_BACK},
    {0x38e,   KEY_LOCKSCREEN},
    {0x390,   KEY_SWITCHLANUAGE},
    {0x391,   KEY_MICDISABLE},
    {0x392,   KEY_TOUCHPANELMUTE},
    {0x393,   KEY_GLOBALSEARCH},
    {0x394,   KEY_FULLSCREEN},
    {0x395,   KEY_SPLITSCREEN},
    {0x397,   KEY_SUPERINTCON},
    {0x398,   KEY_CUSTOMERAPP1},
    {0x399,   KEY_CUSTOMERAPP2},
    {0xcd,   KEY_PLAYPAUSE},
    {0xb5,   KEY_NEXTSONG},
    {0xb6,   KEY_PREVIOUSSONG},
    {unk,   unk},
};


int pogo_keyboard_input_wakeup_init(void)
{
    int ret = 0;
    struct input_dev *input_dev = input_allocate_device();

    if (!input_dev) {
        kb_err("%s %d input_allocate_device err \n", __func__, __LINE__);
        return -ENOMEM;
    }
    input_dev->name = WAKEUP_NAME;
    input_dev->id.bustype = BUS_HOST;
    input_dev->id.vendor = 0x8286;
    input_dev->id.product = 0x8002;
    input_dev->id.version = 0x0010;
    __set_bit(EV_KEY, input_dev->evbit);

    __set_bit(KEY_POWER, input_dev->keybit);
    __set_bit(KEY_KB_ENABLE, input_dev->keybit);
    __set_bit(KEY_KB_DISABLE, input_dev->keybit);
    ret = input_register_device(input_dev);
    if (ret) {
        input_unregister_device(input_dev);
        input_free_device(input_dev);
        kb_err("%s %d input_register_device err \n", __func__, __LINE__);
        return ret;
    }
    pogo_keyboard_client->input_wakeup = input_dev;


    kb_debug("%s %d ok \n", __func__, __LINE__);
    return 0;
}

static int pogo_keyboard_event_hander(struct input_dev *dev,
			unsigned int type, unsigned int code, int value)
{
    int ret = 0;
    switch (type) {
        case EV_LED:
            pogo_keyboard_led_process(code, value);
            break;
        case EV_MSC:
            break;
        default:
            kb_err("%s %d no type  err \n", __func__, __LINE__);
            ret = -1;
    }
    return ret;
}

int pogo_keyboard_input_init(char *keyboard_name)
{
    int i = 0, ret = 0;
    struct input_dev *pogo_keyboard_input = input_allocate_device();

    if (!pogo_keyboard_input) {
        kb_err("%s %d input_allocate_device err \n", __func__, __LINE__);
        return -ENOMEM;
    }

    if (keyboard_name == NULL) {
        keyboard_name = KEYBOARD_NAME;
    }
    pogo_keyboard_input->name = keyboard_name;
    pogo_keyboard_input->id.bustype = BUS_HOST;
    pogo_keyboard_input->id.vendor = 0x22d9;
    pogo_keyboard_input->id.product = 0x3869;
    pogo_keyboard_input->id.version = 0x0010;
    __set_bit(EV_KEY, pogo_keyboard_input->evbit);

    __set_bit(EV_LED, pogo_keyboard_input->evbit);
    __set_bit(LED_CAPSL, pogo_keyboard_input->ledbit);
    //__set_bit(LED_MUTE, pogo_keyboard_input->ledbit);
    //__set_bit(LED_MIC_MUTE, pogo_keyboard_input->ledbit);
    pogo_keyboard_input->event = pogo_keyboard_event_hander;
    for (i = 0; i < KEYBOARD_NUM_KEYS; i++) {
        if (pogo_keyboard[i] != 0 && pogo_keyboard[i] != unk) {
            __set_bit(pogo_keyboard[i], pogo_keyboard_input->keybit);
        }
    }
    /*input mm key set*/
    for (i = 0; i < KEYBOARD_MM_NUM_KEYS; i++) {
        if (mm_pogo_keyboard[i][1] != 0 && mm_pogo_keyboard[i][1] != unk) {
            __set_bit(mm_pogo_keyboard[i][1], pogo_keyboard_input->keybit);
        }
    }
    //__set_bit(KEY_POWER, pogo_keyboard_input->keybit);

    ret = input_register_device(pogo_keyboard_input);
    if (ret) {
        input_free_device(pogo_keyboard_input);
        kb_err("%s %d input_register_device err \n", __func__, __LINE__);
        return ret;
    }
    pogo_keyboard_client->input_pogo_keyboard = pogo_keyboard_input;


    kb_debug("%s %d ok \n", __func__, __LINE__);
    return 0;

}

int pogo_keyboard_input_report(char *buf)
{
    int i = 0;
    struct pogo_keyboard_data *kbd = pogo_keyboard_client;
    struct input_dev *input_dev = kbd->input_pogo_keyboard;
    if (!buf)
        return -EINVAL;
    if (!input_dev)
        return -EINVAL;
    //sprintf(TAG,"%s  %d",__func__,__LINE__);
   // pogo_keyboard_show_buf(pogo_keyboard_client->new,8);

    // 8 bytes key data: 1 ctrl byte + 1 reserved byte + 6 key codes.
    memcpy(pogo_keyboard_client->new, &buf[1], 8);

    // ctrl byte contains 8 ctrl bits.
    for (i = 0; i < 8; i++) {
        input_report_key(input_dev, pogo_keyboard[i + 224], (kbd->new[0] >> i) & 1);
    }

    // process 6 bytes key codes.
    for (i = 2; i < 8; i++) {

        // previous key DOES NOT appear in current key list, meaning this key has been released.
        if (kbd->old[i] > 3 && memscan(kbd->new + 2, kbd->old[i], 6) == kbd->new + 8) {
            if (pogo_keyboard[kbd->old[i]]) {
                input_report_key(input_dev, pogo_keyboard[kbd->old[i]], 0);
                kbd->is_down = false;
                pogo_keyboard_led_report(pogo_keyboard[kbd->old[i]]);
                kb_debug("%s %d key:%d \n", __func__, __LINE__, pogo_keyboard[kbd->old[i]]);
            } else
                kb_debug("Unknown key (scancode %#x) released.\n", kbd->old[i]);
        }


        // current key DOES NOT appear in previous key list, meaning a new key has been pressed.
        if (kbd->new[i] > 3 && memscan(kbd->old + 2, kbd->new[i], 6) == kbd->old + 8) {
            if (pogo_keyboard[kbd->new[i]]) {
                input_report_key(input_dev, pogo_keyboard[kbd->new[i]], 1);
                kbd->is_down = true;
                kbd->down_code = pogo_keyboard[kbd->new[i]];

                kb_debug("%s %d key:%d \n", __func__, __LINE__, pogo_keyboard[kbd->new[i]]);
            } else
                kb_debug("Unknown key (scancode %#x) pressed.\n", kbd->new[i]);
        }
    }

    input_sync(input_dev);
    kb_debug("%s %d input end\n", __func__, __LINE__);
    memcpy(kbd->old, kbd->new, 8);
    return 0;

}

void *memscan_ex(void *addr, int c, size_t size)
{
    unsigned char *p = addr;
    unsigned short key = 0;
    while (size) {
        key = *p | ((*(p + 1)) << 8);
        if (key == c)
            return (void *)p;
        p = p + 2;
        size = size - 2;
    }
    return (void *)p;
}

int pogo_keyboard_mm_input_report(char *buf)
{
    int i = 0, j = 0;
    unsigned short keywords = 0;
    struct pogo_keyboard_data *kbd = pogo_keyboard_client;
    struct input_dev *input_dev = kbd->input_pogo_keyboard;
    if (!buf)
        return -EINVAL;
    if (!input_dev)
        return -EINVAL;

    //sprintf(TAG,"%s  %d",__func__,__LINE__);
    //pogo_keyboard_show_buf(pogo_keyboard_client->mm_old,4);
    //pogo_keyboard_show_buf(pogo_keyboard_client->mm_new,4);
    memcpy(pogo_keyboard_client->mm_new, &buf[1], 4);
    //keywords = kbd->mm_new[2*i] | kbd->mm_new[2*i+1] << 8;
    //kb_debug("%s %d key:0x%x \n",__func__,__LINE__,keywords);


    for (i = 0; i < 2; i++) {

        // previous key DOES NOT appear in current key list, meaning this key has been released.
        keywords = kbd->mm_old[2 * i] | kbd->mm_old[2 * i + 1] << 8;
        if (memscan_ex(kbd->mm_new, keywords, 4) == kbd->mm_new + 4) {
            if (keywords) {
                for (j = 0; j < KEYBOARD_MM_NUM_KEYS; j++) {
                    if (keywords == mm_pogo_keyboard[j][0])
                        break;
                }
                kb_debug("%s %d i:%d j:%d %p!=%p keywords:0x%x\n", __func__, __LINE__, i, j, memscan_ex(kbd->mm_new, keywords, 4), kbd->mm_new + 4, keywords);
                if (j < KEYBOARD_MM_NUM_KEYS) {
                    input_report_key(input_dev, mm_pogo_keyboard[j][1], 0);
                    kbd->is_mmdown = false;
                    kb_debug("%s %d key:0x%x \n", __func__, __LINE__, keywords);
                    pogo_keyboard_led_report(mm_pogo_keyboard[j][1]);
                } else {
                    kb_debug("Unknown key (scancode %#x) %d released.\n", kbd->mm_old[0], __LINE__);
                }

            } else
                kb_debug("Unknown key (scancode %#x) %d released.\n", kbd->mm_old[0], __LINE__);
        }

        // current key DOES NOT appear in previous key list, meaning a new key has been pressed.
        keywords = kbd->mm_new[2 * i] | kbd->mm_new[2 * i + 1] << 8;
        if (memscan_ex(kbd->mm_old, keywords, 4) == kbd->mm_old + 4) {
            if (keywords) {
                for (j = 0; j < KEYBOARD_MM_NUM_KEYS; j++) {
                    if (keywords == mm_pogo_keyboard[j][0])
                        break;
                }
                kb_debug("%s %d i:%d j:%d %p!=%p keywords:0x%x\n", __func__, __LINE__, i, j, memscan_ex(kbd->mm_old, keywords, 4), kbd->mm_new + 4, keywords);
                if (j < KEYBOARD_MM_NUM_KEYS) {
                    input_report_key(input_dev, mm_pogo_keyboard[j][1], 1);
                    kbd->is_mmdown = true;
                    kbd->down_mmcode = mm_pogo_keyboard[j][1];

                    kb_debug("%s %d key:%x \n", __func__, __LINE__, keywords);
                } else {
                    kb_debug("Unknown key (scancode %#x) %d  pressed.\n", kbd->mm_new[0], __LINE__);
                }
            } else
                kb_debug("Unknown key (scancode %#x) pressed.\n", kbd->mm_new[0]);
        }
    }

    input_sync(input_dev);
    memcpy(kbd->mm_old, kbd->mm_new, 4);
    return 0;
}

