#include "pogo_keyboard.h"
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/clk.h>
#include <linux/debugfs.h>
#include <linux/fb.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <uapi/linux/sched/types.h>
#include <linux/pm.h>
#include <linux/pm_wakeirq.h>
#include <linux/serial_8250.h>
#include <linux/proc_fs.h>
#include "owb.h"

#if IS_ENABLED(CONFIG_QCOM_PANEL_EVENT_NOTIFIER) && defined(CONFIG_OPLUS_POGOPIN_FUNCTION)
#include <linux/soc/qcom/panel_event_notifier.h>
#include <linux/msm_drm_notify.h>
#include <drm/drm_panel.h>
#endif

struct pogo_keyboard_data *pogo_keyboard_client = NULL;
static int test_type = 0;
char TAG[60] = { 0 };

//max numbers of interval for plug-out detection.
//heartbeat interval from keyboard is 100ms.
//keybaord initialization takes about 400ms and after that first heartbeat packet is reported.
//that's to say during keyboard plug-in stage host will treat keyboard plug-out if host cannot receive heartbeat packet within 400ms.
//but host only wait 200ms after keyboard attechment is finished.
//note the host timer is 50ms.
static int max_disconnect_count = 40;
static int max_plug_in_disconnect_count = 8;

static DECLARE_WAIT_QUEUE_HEAD(waiter);
static DECLARE_WAIT_QUEUE_HEAD(read_waiter);

//static BLOCKING_NOTIFIER_HEAD(pogo_keyboard_notifier_list);

static int pogo_keyboard_event_process(unsigned char pogo_keyboard_event);
#ifdef CONFIG_PLUG_SUPPORT
static irqreturn_t keybaord_core_plug_irq_handler(int irq, void *data);
#endif
static irqreturn_t pogo_keyboard_irq_handler(int irq, void *data);
static struct file *pogo_keyboard_port_to_file(struct uart_port *port);
#ifdef CONFIG_POWER_CTRL_SUPPORT
static void pogo_keyboard_power_enable(int value);
#endif
static int pogo_keyboard_enable_uart_tx(int value);
static int pogo_keyboard_plat_remove(struct platform_device *device);
static int pogo_keyboard_plat_suspend(struct platform_device *device);
static int pogo_keyboard_plat_resume(struct platform_device *device);
static int pogo_keyboard_input_connect(void);
static void pogo_keyboard_input_disconnect(void);

void pogo_keyboard_show_buf(void *buf, int count)
{
    int i = 0;
    char temp[513] = { 0 };
    int len = 0;
    char *pbuf = (char *)buf;
    if (count > UART_BUFFER_SIZE)
        count = UART_BUFFER_SIZE;
    for (i = 0; i < count; i++) {
        len += sprintf(temp + len, "%02x", pbuf[i]);
    }
    kb_debug("%s:show_buf  len:[%d]  %s\n", TAG, count, temp);
    memset(TAG, 0, sizeof(TAG));
}

void pogo_keyboard_info_buf(void *buf, int count)
{
    int i = 0;
    char temp[513] = { 0 };
    int len = 0;
    char sync_buf[] = { 0x2f,0x04,0x05,0xaa,0xbb };
    char *pbuf = (char *)buf;
    if (count > UART_BUFFER_SIZE)
        count = UART_BUFFER_SIZE;
    if (memcmp(sync_buf, buf, sizeof(sync_buf)) == 0) {
        return;
    }
    for (i = 0; i < count; i++) {
        len += sprintf(temp + len, "%02x", pbuf[i]);
    }
    kb_info("%s:info_buf  len:[%d]  %s\n", TAG, count, temp);
    memset(TAG, 0, sizeof(TAG));
}

// start heartbeat monitor timer only when keyboard is attached. value = 1 starts timer while 0 stops.
void pogo_keyboard_heartbeat_switch(int value)
{
    ktime_t ktime = ktime_set(0, 1000 * 1000 * 50); //50ms, note that heartbeat from keyboard is 100ms(may be adjusted in the future)
    if (value == 1) {
        if (pogo_keyboard_client)
            hrtimer_start(&pogo_keyboard_client->heartbeat_timer, ktime, HRTIMER_MODE_REL);
    } else if (value == 0) {
        if (pogo_keyboard_client)
            hrtimer_cancel(&pogo_keyboard_client->heartbeat_timer);
    }
}

// start/stop keyboard attachement first detection timer. value = 1 starts timer while 0 stops.
void pogo_keyboard_plug_switch(int value)
{
    ktime_t ktime = ktime_set(0, 1000 * 1000 * 20); //20ms
    if (value == 1) {
        if (pogo_keyboard_client->file_client)
            hrtimer_start(&pogo_keyboard_client->plug_timer, ktime, HRTIMER_MODE_REL);
    } else if (value == 0) {
        if (pogo_keyboard_client->file_client)
            hrtimer_cancel(&pogo_keyboard_client->plug_timer);
    }
}

void pogo_keyboard_poweroff_timer_switch(bool state)
{
    ktime_t ktime = ktime_set(0, 1000 * 1000 * POWEROFF_TIMER_EXPIRY);

    if (pogo_keyboard_client == NULL) {
        return;
    }

    if (state) {
        pm_wakeup_event(&pogo_keyboard_client->plat_dev->dev, POWEROFF_TIMER_EXPIRY + 20);
        hrtimer_start(&pogo_keyboard_client->poweroff_timer, ktime, HRTIMER_MODE_REL);
    } else {
        hrtimer_cancel(&pogo_keyboard_client->poweroff_timer);
    }
}

// api to send event to "pogo_keyboard_task".
static void pogo_keyboard_event_send(unsigned char value)
{
    struct pogo_keyboard_event send_event = { 0 };
    unsigned long flags;

    kb_info("%s %d new event:%d\n", __func__, __LINE__, value);

    spin_lock_irqsave(&pogo_keyboard_client->event_fifo_lock, flags);
    if (kfifo_avail(&pogo_keyboard_client->event_fifo) >= sizeof(struct pogo_keyboard_event)) {
        send_event.event = value;
        kfifo_in(&pogo_keyboard_client->event_fifo, &send_event, sizeof(struct pogo_keyboard_event));
        pogo_keyboard_client->flag = 1;
        wake_up_interruptible(&waiter);
    }
    spin_unlock_irqrestore(&pogo_keyboard_client->event_fifo_lock, flags);
}

//计算crc16
static unsigned short app_compute_crc16(unsigned short crc, unsigned char data, unsigned short polynomial)
{
    unsigned char i = 0;
    for (i = 0; i < 8; i++) {
        if ((((crc & 0x8000) >> 8) ^ (data & 0x80)) != 0) {
            crc <<= 1;         // shift left once
            crc ^= polynomial; // XOR with polynomial
        } else {
            crc <<= 1;         // shift left once
        }
        data <<= 1;            // Next data bit
    }
    return crc;
}

//获取crc16
unsigned short app_crc16_get(unsigned char *buf, unsigned short len, unsigned char crc_type)
{
    unsigned char i = 0;
    unsigned short crc = 0;
    unsigned short polynomial = 0;

    polynomial = (crc_type == CRC_TYPE_IBM) ? POLYNOMIAL_IBM : POLYNOMIAL_CCITT;
    crc = (crc_type == CRC_TYPE_IBM) ? CRC_IBM_INIT_VAL : CRC_CCITT_INIT_VAL;

    for (i = 0; i < len; i++) {
        crc = app_compute_crc16(crc, buf[i], polynomial);
    }
    if (crc_type == CRC_TYPE_IBM) {
        return crc;
    } else {
        return (unsigned short)(~crc);
    }
}

// put payloads into one wire bus protocol packet.
int uart_package_data(char *buf, int len, unsigned char *p_out, int *p_len)
{
    unsigned short i = 0;
    unsigned short need_len = 0;
    unsigned short crc16 = 0;

    //头同步码8Byte 0x55
    for (i = 0; i < 8; i++) {
        p_out[i] = ONE_WIRE_BUS_PACKET_HEAD_SYNC_CODE;
    }
    //起始码
    p_out[8] = ONE_WIRE_BUS_PACKET_XXX_START_CODE;
    //源地址
    p_out[9] = ONE_WIRE_BUS_PACKET_PAD_ADDR;
    //目标地址
    p_out[10] = ONE_WIRE_BUS_PACKET_KEYBOARD_ADDR;
    //主命令
    p_out[11] = buf[0];
    len = buf[1];
    //总长度
    p_out[12] = len;
    //子命令数据
    for (i = 0; i < len; i++) {
        p_out[13 + i] = buf[i + 2];
    }
    crc16 = app_crc16_get(&p_out[8], len + 5, CRC_TYPE_IBM);
    //CRC16
    p_out[13 + len] = (unsigned char)(crc16 >> 8);
    p_out[14 + len] = (unsigned char)(crc16 & 0x00ff);
    //结束码
    p_out[15 + len] = ONE_WIRE_BUS_PACKET_XXX_END_CODE;
    //尾同步码4byte 0xAA
    for (i = 0; i < 4; i++) {
        p_out[16 + len + i] = ONE_WIRE_BUS_PACKET_TAIL_SYNC_CODE;
    }
    need_len = 16 + len + 4;
    *p_len = need_len;
    sprintf(TAG, "%s ciphertext %d", __func__, __LINE__);
    pogo_keyboard_show_buf(p_out, *p_len);
    return 0;
}

int uart_unpack_data(char *buf, int len, unsigned char *out_buf, int *out_len)
{
    *out_len = len - 6;//6:start+addr1+addr2 +  end+sync+sync
    memcpy(out_buf, &buf[3], *out_len);
    return 0;
}

// handle uart received data. NOTE: called by uart rx interrupt handler.
static int pogo_keyboard_mod_data_process(char *buf, int len)
{
    int   value = buf[0];
    struct input_dev *input_dev = NULL;
    pogo_keyboard_client->disconnect_count = 0;
    switch (value) {
        case ONE_WIRE_BUS_PACKET_GENRRAL_KEY_CMD:
            kb_debug("%s %d GENRRAL_KEY\n", __func__, __LINE__);
            pogo_keyboard_input_report(&buf[1]);
            break;
        case ONE_WIRE_BUS_PACKET_MEDIA_KEY_CMD:
            kb_debug("%s %d MEDIA_KEY\n", __func__, __LINE__);
            pogo_keyboard_mm_input_report(&buf[1]);
            break;
        case ONE_WIRE_BUS_PACKET_TOUCHPAD_CMD:
            kb_debug("%s %d TOUCHPAD\n", __func__, __LINE__);
            touchpad_input_report(&buf[1]);
            break;
        case ONE_WIRE_BUS_PACKET_SYNC_UPLOAD_CMD:
            kb_debug("%s %d SYNC status:%2x\n", __func__, __LINE__, pogo_keyboard_client->pogo_keyboard_status);
            // packet format: 0x2F + 0x04(len=4) + 1-byte sub-cmd + 3-byte data.
            if ((pogo_keyboard_client->pogo_keyboard_status & KEYBOARD_CONNECT_STATUS) == 0) {
                //获取MAC地址及品牌标志
                if (buf[2] == 0x01 && buf[3] == 0x02) {
                    pogo_keyboard_client->keyboard_brand = buf[4];
                    memcpy((unsigned char *)&pogo_keyboard_client->mac_addr, &buf[5], 6);
                } else if (buf[2] == 0x05 && buf[3] == 0x02) {
                    pogo_keyboard_client->keyboard_brand = buf[5];
                    memcpy((unsigned char *)&pogo_keyboard_client->mac_addr, &buf[6], 6);
                }

                kb_info("%s %d plug in\n", __func__, __LINE__);
                pogo_keyboard_client->plug_in_count = 0; // reset heartbeat counter.
                pogo_keyboard_event_send(KEYBOARD_PLUG_IN_EVENT);

            } else if ((pogo_keyboard_client->pogo_keyboard_status & KEYBOARD_CONNECT_STATUS) != 0) {
                //     sub-cmd = 0x01: power-up sync packet, 3-byte data = 0xCC.
                //     sub-cmd = 0x05: heartbeat packet, 3-byte data = 0xAA 0xBB 0BIT:CAPSLOCK|1BIT:NUMLOCK|2BIT:MUTE|3BIT:MIC|4BIT:FNLOCK.
                if (buf[2] == 0x01) { //by power on data for after plug out quick plug in connect status not update
                    //获取MAC地址及品牌标志
                    if (buf[3] == 0x02) {
                        pogo_keyboard_client->keyboard_brand = buf[4];
                        memcpy((unsigned char *)&pogo_keyboard_client->mac_addr, &buf[5], 6);
                    }
                    pogo_keyboard_client->pogo_keyboard_status &= ~KEYBOARD_CONNECT_STATUS;
                    pogo_keyboard_client->plug_in_count = 0;
                    kb_info("%s %d plug in\n", __func__, __LINE__);
                    pogo_keyboard_event_send(KEYBOARD_PLUG_IN_EVENT);
                } else if (buf[2] == 0x05 && buf[3] == 0x02 && (buf[4] & 0x01) != ((pogo_keyboard_client->pogo_keyboard_status >> 2) & 0x01)) { //by heartbeet data sync capslock status
                    //sync capslock led status if status reported from keyboard differs from host.
                    //note definition: KEYBOARD_CAPSLOCK_ON_STATUS  (1<<2)
                    kb_info("%s %d sync capslock:%02x\n", __func__, __LINE__, buf[5]);
                    if ((pogo_keyboard_client->pogo_keyboard_status & KEYBOARD_LCD_ON_STATUS) != 0) {//send led cmd to kb only if lcd is on.
                        if (((pogo_keyboard_client->pogo_keyboard_status >> 2) & 0x01) == 0x01)
                            pogo_keyboard_event_send(KEYBOARD_CAPSLOCK_ON_EVENT);
                        else
                            pogo_keyboard_event_send(KEYBOARD_CAPSLOCK_OFF_EVENT);
                    }
                }
            }
            break;
        case ONE_WIRE_BUS_PACKET_PARAM_SET_ACK_CMD:
        case ONE_WIRE_BUS_PACKET_OTA_ACK_CMD:
        case ONE_WIRE_BUS_PACKET_I2C_READ_ACK_CMD:
        case ONE_WIRE_BUS_PACKET_USER_GENERAL_ACK_CMD:
        case ONE_WIRE_BUS_PACKET_OTA_CMD:
        case ONE_WIRE_BUS_PACKET_PARAM_SET_CMD:
        case ONE_WIRE_BUS_PACKET_USER_GENERAL_CMD:
        case ONE_WIRE_BUS_PACKET_USER_PASSTHROUGH_CMD:
        case ONE_WIRE_BUS_PACKET_USER_PASSTHROUGH_ACK_CMD:
            if (pogo_keyboard_client->read_flag == 1) {
                kb_debug("%s %d	value:0x%2x cmd:0x%2x \n", __func__, __LINE__, value, pogo_keyboard_client->write_buf[0]);
                // judgment below is based on the assumption that response cmd code = send cmd code + 1. not universal.
                if (value == (int)pogo_keyboard_client->write_buf[0] || value == (int)pogo_keyboard_client->write_buf[0] + 1) {
                    pogo_keyboard_client->read_flag = 0;
                    wake_up_interruptible(&read_waiter);
                }
            }

            if (value == ONE_WIRE_BUS_PACKET_USER_GENERAL_ACK_CMD && buf[2] == 0x03) {
                // got keyboard enable/disable report.
                //packet format: 3B 03 03 01 00(DISABLE)/01(ENABLE)
                input_dev = pogo_keyboard_client->input_wakeup;
                if (buf[4] == 0x01) {
                    input_report_key(input_dev, KEY_KB_ENABLE, 1);
                    input_sync(input_dev);
                    input_report_key(input_dev, KEY_KB_ENABLE, 0);
                    input_sync(input_dev);
                } else if (buf[4] == 0x00) {
                    input_report_key(input_dev, KEY_KB_DISABLE, 1);
                    input_sync(input_dev);
                    input_report_key(input_dev, KEY_KB_DISABLE, 0);
                    input_sync(input_dev);
                }
            }
            break;
        default:
            return -EINVAL;
    }
    return 0;
}

bool pogo_keyboard_data_is_valid(char *buf, int len)
{
    char flag = buf[2];
    unsigned short crc16 = (buf[len - 5] << 8) | buf[len - 4];
    kb_debug("flag:%2x crc16:%4x cal_crc:%4x\n", flag, crc16, app_crc16_get(buf, len - 5, CRC_TYPE_IBM));
    if (flag == ONE_WIRE_BUS_PACKET_KEYBOARD_ADDR || flag == ONE_WIRE_BUS_PACKET_PAD_ADDR) {
        if (crc16 == app_crc16_get(buf, len - 5, CRC_TYPE_IBM))
            return true;
        else
            return false;
    } else {
        return false;
    }
}

int pogo_keyboard_store_recv_data(char *buf, int len)
{
    int ret = 0;
    if (buf[0] == pogo_keyboard_client->write_buf[0]) {
        memcpy(pogo_keyboard_client->write_check_buf, buf, len);
        pogo_keyboard_client->write_len = len;
        ret = len;
        sprintf(TAG, "%s  %d write_check_buf ", __func__, __LINE__);
        pogo_keyboard_show_buf(pogo_keyboard_client->write_check_buf, len);
    } else if (buf[0] == (int)pogo_keyboard_client->write_buf[0] + 1) {
        memcpy(pogo_keyboard_client->read_buf, buf, len);
        pogo_keyboard_client->read_len = len;
        ret = len;
        sprintf(TAG, "%s  %d read_buf ", __func__, __LINE__);
        pogo_keyboard_show_buf(pogo_keyboard_client->read_buf, len);
    }
    return ret;
}

// handle uart received data, called by uart rx interrupt handler.
int pogo_keyboard_recv(char *buf, int len)
{
    static bool recv_start_flag = false;
    static bool recv_decode_flag = false;
    static unsigned char head_sync_code_cnt = 0;
    static unsigned char rec_temp_buf_index = 0;
    static char recv_buf[UART_BUFFER_SIZE] = { 0 };
    char data_buf[UART_BUFFER_SIZE] = { 0 };
    int out_len = 0;
    char rx_data;
    int i = 0;
    int ret = 0;
    unsigned char recv_decode_cnt = 0;
    sprintf(TAG, "%s  %d recv ", __func__, __LINE__);
    if (len > 1)
        pogo_keyboard_show_buf(buf, len);

    // the process below is based on the assumption that only one packet per transfer!!
    for (i = 0; i < len; i++) {
        rx_data = buf[i];

        if (rx_data == ONE_WIRE_BUS_PACKET_HEAD_SYNC_CODE) {
            head_sync_code_cnt++;
        } else if (rx_data == ONE_WIRE_BUS_PACKET_XXX_START_CODE || rx_data == ONE_WIRE_BUS_PACKET_XXX_REPEAT_CODE) {
            //收到起始码后，再判断起始码之前有没有收到连续的头同步码
            if (head_sync_code_cnt >= 4) {
                rec_temp_buf_index = 0;
                recv_start_flag = true;
            }
            head_sync_code_cnt = 0;
        } else {
            head_sync_code_cnt = 0;
        }

        if (recv_start_flag == true) {
            recv_buf[rec_temp_buf_index++] = rx_data;
            if (rec_temp_buf_index >= 3
                && recv_buf[rec_temp_buf_index - 1] == ONE_WIRE_BUS_PACKET_TAIL_SYNC_CODE
                && recv_buf[rec_temp_buf_index - 2] == ONE_WIRE_BUS_PACKET_TAIL_SYNC_CODE
                && recv_buf[rec_temp_buf_index - 3] == ONE_WIRE_BUS_PACKET_XXX_END_CODE) {
                //当收到0xFE,0xAA,0xAA字段时，认为接收到完整一包了，并开始进行包处理
                recv_start_flag = false;
                recv_decode_flag = true;
                recv_decode_cnt++;
                if(recv_decode_cnt > 2) {
                    break;
                }
            }
        }
        if (rec_temp_buf_index >= UART_BUFFER_SIZE)
            rec_temp_buf_index = 0;
    }

    if (recv_decode_flag == true) {
        recv_decode_flag = false;
        if (pogo_keyboard_data_is_valid(recv_buf, rec_temp_buf_index)) {
            uart_unpack_data(recv_buf, rec_temp_buf_index, data_buf, &out_len);
            sprintf(TAG, "%s  %d recv ", __func__, __LINE__);
            pogo_keyboard_info_buf(data_buf, out_len);
            pogo_keyboard_store_recv_data(data_buf, out_len);

            ret = pogo_keyboard_mod_data_process(data_buf, out_len);
            if (ret) {
                kb_err("%s %d pogo_keyboard_mod_data_process fail: %d\r\n", __func__, __LINE__, ret);
            }

        }
        rec_temp_buf_index = 0;
    }

    return ret;

}

int pogo_keyboard_recv_callback(char *buf, int len)
{
    return pogo_keyboard_recv(buf, len);
}

int pogo_keyboard_get_valid_data(char *buf, int len, unsigned char *out_buf, int *out_len)
{
    int count = 0;
    count = len - 11 - 4; //11: sync*8+start+addr1+addr2  4:sync*4
    memcpy(out_buf, &buf[11], count);
    *out_len = count;
    return count;
}

ssize_t tty_write_ex(const char *buf, size_t count)
{
    ssize_t ret = 0;
    if (pogo_keyboard_client->file_client == NULL) {
        kb_err("TN %s %d err: file_client is null!\n", __func__, __LINE__);
        return -1;
    }
    ret = pogo_tty_write(pogo_keyboard_client->file_client, buf, count, NULL);
    if (ret < 0) {
        kb_err("TN %s %d err:%d!\n", __func__, __LINE__, ret);
        return -1;
    }
    return ret;
}

static int pogo_keyboard_write(void *buf, int len)
{
    int ret = 0;
    int out_len = 0;
    int count = 3;
    void *pbuf = buf;
    char write_buf[255] = { 0 };
    //char read_buf[255] = {0};
    int write_len = 0;
    int i = 0;
    sprintf(TAG, "%s text %d", __func__, __LINE__);
    pogo_keyboard_info_buf(pbuf, len);
    memset(pogo_keyboard_client->write_buf, 0, sizeof(pogo_keyboard_client->write_buf));
    //memcpy(pogo_keyboard_client->write_buf, buf, len);
    ret = uart_package_data(pbuf, len, write_buf, &write_len);
    if (ret) {
        kb_err("%s %d err\r\n", __func__, __LINE__);
        return ret;
    }
    pogo_keyboard_get_valid_data(write_buf, write_len, pogo_keyboard_client->write_buf, &out_len);
    pogo_keyboard_client->read_flag = 1;
    for (i = 0; i < count; i++) {
        ret = tty_write_ex(write_buf, write_len);
        if (ret > 0) {
            break;
        }
    }
    if (i >= count) {
        kb_err("%s %d ret:%d i:%d err\r\n", __func__, __LINE__, ret, i);
        return ret;
    }

    wait_event_interruptible_timeout(read_waiter, pogo_keyboard_client->read_flag != 1, HZ / 20);//timeout 50ms

    if (pogo_keyboard_client->read_flag == 1) {
        kb_debug("%s %d read_flag:%d  \r\n", __func__, __LINE__, pogo_keyboard_client->read_flag);
    }
    if (pogo_keyboard_client->write_len <= 0) {
        kb_err("%s %d read_flag:%d  write_len:%d\r\n", __func__, __LINE__, pogo_keyboard_client->read_flag, pogo_keyboard_client->write_len);
        pogo_keyboard_client->read_flag = 0;
        return -1;
    }

    kb_debug("%s %d  %2x %2x\n", __func__, __LINE__, pogo_keyboard_client->write_buf[out_len - 2], pogo_keyboard_client->write_check_buf[out_len - 2]);
    if (pogo_keyboard_client->write_buf[0] == pogo_keyboard_client->write_check_buf[0] &&
        pogo_keyboard_client->write_buf[out_len - 2] == pogo_keyboard_client->write_check_buf[out_len - 2]) { //compare cmd and crc
        ret = 0;
        kb_debug("%s %d is same,write %d data ok ret:%d i:%d!\n", __func__, __LINE__, out_len, ret, i);
    } else {
        ret = -1;
        kb_err("%s %d is not same %d data err!\n", __func__, __LINE__, out_len);
    }

    pogo_keyboard_client->write_len = 0;
    memset(pogo_keyboard_client->write_check_buf, 0, UART_BUFFER_SIZE);
    return ret;
}

static int pogo_keyboard_read(void *buf, int *r_len)
{

    kb_debug("%s %d start\r\n", __func__, __LINE__);
    pogo_keyboard_client->read_flag = 1;
    wait_event_interruptible_timeout(read_waiter, pogo_keyboard_client->read_flag != 1, HZ / 10);//timeout 100ms

    if (pogo_keyboard_client->read_flag == 1) {
        kb_debug("%s %d read_flag:%d  \r\n", __func__, __LINE__, pogo_keyboard_client->read_flag);
    }
    if (pogo_keyboard_client->read_len <= 0) {
        kb_err("%s %d read_len:%d  err\r\n", __func__, __LINE__, pogo_keyboard_client->read_len);
        pogo_keyboard_client->read_flag = 0;
        return -1;
    }
    kb_info("%s %d read_flag:%d len:%d ok\r\n", __func__, __LINE__, pogo_keyboard_client->read_flag, pogo_keyboard_client->read_len);


    memcpy(buf, pogo_keyboard_client->read_buf, pogo_keyboard_client->read_len);
    *r_len = pogo_keyboard_client->read_len;


    memset(pogo_keyboard_client->read_buf, 0, sizeof(pogo_keyboard_client->read_buf));
    sprintf(TAG, "%s text %d", __func__, __LINE__);
    pogo_keyboard_show_buf(buf, *r_len);
    pogo_keyboard_client->read_len = 0;
    memset(pogo_keyboard_client->read_buf, 0, UART_BUFFER_SIZE);

    return 0;
}

static int pogo_keyboard_write_and_read(void *w_buf, int w_len, void *r_buf, int *r_len)
{
    int ret = 0;
    int i = 0;
    int count = 3;
    pm_stay_awake(&pogo_keyboard_client->plat_dev->dev);

    for (i = 0; i < count; i++) {
        kb_debug("%s %d read_flag:%d i:%d start\r\n", __func__, __LINE__, pogo_keyboard_client->read_flag, i);
        ret = pogo_keyboard_write(w_buf, w_len);
        if (ret) {
            mdelay(50);
            continue;
        }

        if (r_buf == NULL) {
            break;
        }

        ret = pogo_keyboard_read(r_buf, r_len);
        if (ret == 0) {
            break;
        }
    }
    pm_relax(&pogo_keyboard_client->plat_dev->dev);
    if (i >= count) {
        kb_err("%s %d  ret:%d i:%d err\r\n", __func__, __LINE__, ret, i);
        return ret;
    }
    kb_debug("%s %d ret:%d i:%d ok\r\n", __func__, __LINE__, ret, i);
    return 0;
}

#if defined(CONFIG_KB_DEBUG_FS)
//read touchpad version from keyboard.
int  pogo_keyboard_tp_ver(void)
{

    char ver_reg[] = { ONE_WIRE_BUS_PACKET_I2C_WRITE_CMD, 0x01, 0x07 };

    char temp[100] = { 0 };
    int read_len = 0;
    int ret = 0;

    ret = pogo_keyboard_write_and_read(ver_reg, sizeof(ver_reg), temp, &read_len);
    if (ret < 0) {
        kb_err("%s %d err:ret:%d \n", __func__, __LINE__, ret);
        return ret;
    }
    //kb_debug("%s %d write:ret:%d \n",__func__,__LINE__,ret);

    pogo_keyboard_show_buf(temp, read_len);
    return 0;
}

//read keybaord version.
int  pogo_keyboard_ver(void)
{

    char ver_reg[] = { ONE_WIRE_BUS_PACKET_USER_PASSTHROUGH_CMD, 0x03, 0x08,0x01, 0x01 };
    char buf[] = { ONE_WIRE_BUS_PACKET_USER_PASSTHROUGH_ACK_CMD };

    char temp[100] = { 0 };
    int read_len = 0;
    int count = 3;
    int ret = 0;
    int i = 0;
    for (i = 0; i < count; i++) {
        ret = pogo_keyboard_write_and_read(ver_reg, sizeof(ver_reg), temp, &read_len);
        if (ret < 0) {
            kb_err("%s %d err:ret:%d \n", __func__, __LINE__, ret);
            continue;
        }
        //kb_debug("%s %d write:ret:%d \n",__func__,__LINE__,ret);
        if (memcmp(temp, buf, sizeof(buf)) == 0) {
            break;
        }
    }
    pogo_keyboard_show_buf(temp, read_len);
    if (i >= count) {
        kb_err("%s %d err:ret:%d \n", __func__, __LINE__, ret);
        return -1;
    }
    return 0;

}

void pogo_keyboard_reliability_test(void)
{
    int i = 0;
    int ret = 0;
    int sum = 0;
    int count = 3;//2000;
    kb_debug("%s %d start i:%d\n", __func__, __LINE__, i);
    for (i = 0; i < count; i++) {
        if (test_type == 1) {
            ret = pogo_keyboard_ver();
        } else if (test_type == 2) {
            ret = pogo_keyboard_tp_ver();
        } else {
            kb_err("%s %d no test mode %d\r\n", __func__, __LINE__, test_type);
            return;
        }
        if (ret != 0) {
            kb_err("%s %d i:%d err\r\n", __func__, __LINE__, i);
            sum++;
        }
    }
    if (sum != 0) {
        kb_err("%s %d fail! sum:%d\r\n", __func__, __LINE__, sum);
    }
    kb_debug("%s %d end ok i:%d sum:%d \n", __func__, __LINE__, i, sum);
}

void pogo_keyboard_test(void)
{
    pogo_keyboard_reliability_test();
}
#endif//CONFIG_KB_DEBUG_FS

// output leds control cmd to keyboard.
static int pogo_keyboard_set_led(char event)
{
    int ret = 0;
    char led_buf[] = { ONE_WIRE_BUS_PACKET_PARAM_SET_CMD, 0x06, 0x0D, 0x04, 0x00, 0x00, 0x00, 0x00 };

    kb_debug("%s %d pogo_keyboard_event:%d  pogo_keyboard_status:0x%02x\n", __func__, __LINE__, event, pogo_keyboard_client->pogo_keyboard_status);


    if (pogo_keyboard_client->pogo_keyboard_status & KEYBOARD_CAPSLOCK_ON_STATUS) {
        led_buf[4] = 0x01;
    }
    if (pogo_keyboard_client->pogo_keyboard_status & KEYBOARD_MUTEDISABLE_ON_STATUS) {
        led_buf[6] = 0x01;
    }
    if (pogo_keyboard_client->pogo_keyboard_status & KEYBOARD_MICDISABLE_ON_STATUS) {
        led_buf[7] = 0x01;
    }

    kb_debug("%s %d pogo_keyboard_event:%d  pogo_keyboard_status:0x%02x\n", __func__, __LINE__, event, pogo_keyboard_client->pogo_keyboard_status);

    // if(event >= KEYBOARD_CAPSLOCK_ON_EVENT)
    mdelay(15);
    switch (event) {
        case  KEYBOARD_HOST_LCD_ON_EVENT:
            kb_debug("%s %d %d\r\n", __func__, __LINE__, event);
            ret = pogo_keyboard_write(led_buf, sizeof(led_buf));
            if (ret) {
                kb_err("%s %d err\r\n", __func__, __LINE__);
                return ret;
            }
            break;
        case KEYBOARD_HOST_LCD_OFF_EVENT:
            kb_debug("%s %d %d\r\n", __func__, __LINE__, event);
            memset(&led_buf[4], 0, 4);
            ret = pogo_keyboard_write(led_buf, sizeof(led_buf));
            if (ret) {
                kb_err("%s %d err\r\n", __func__, __LINE__);
                //return ret;
            }
            break;
        case KEYBOARD_PLUG_IN_EVENT:
            kb_debug("%s %d %d\r\n", __func__, __LINE__, event);;
            ret = pogo_keyboard_write(led_buf, sizeof(led_buf));
            if (ret) {
                kb_err("%s %d err\r\n", __func__, __LINE__);
                //return ret;
            }
            break;
        case KEYBOARD_PLUG_OUT_EVENT:  /* for pre development test*/
            kb_debug("%s %d %d\r\n", __func__, __LINE__, event);
            memset(&led_buf[4], 0, 4);
            ret = pogo_keyboard_write(led_buf, sizeof(led_buf));
            if (ret) {
                kb_err("%s %d err\r\n", __func__, __LINE__);
                //return ret;
            }
            break;

        case KEYBOARD_CAPSLOCK_ON_EVENT:
            kb_debug("%s %d %d\r\n", __func__, __LINE__, event);

            led_buf[4] = 0x01;
            ret = pogo_keyboard_write(led_buf, sizeof(led_buf));
            if (ret) {
                kb_err("%s %d err\r\n", __func__, __LINE__);
                //return ret;
            }

            break;
        case KEYBOARD_CAPSLOCK_OFF_EVENT:
            kb_debug("%s %d %d\r\n", __func__, __LINE__, event);

            led_buf[4] = 0x00;
            ret = pogo_keyboard_write(led_buf, sizeof(led_buf));
            if (ret) {
                kb_err("%s %d err\r\n", __func__, __LINE__);
                //return ret;
            }

            break;


        case KEYBOARD_MUTEDISABLE_ON_EVENT:
            kb_debug("%s %d %d\r\n", __func__, __LINE__, event);

            led_buf[6] = 0x01;
            ret = pogo_keyboard_write(led_buf, sizeof(led_buf));
            if (ret) {
                kb_err("%s %d err\r\n", __func__, __LINE__);
                //return ret;
            }

            break;
        case KEYBOARD_MUTEDISABLE_OFF_EVENT:
            kb_debug("%s %d %d\r\n", __func__, __LINE__, event);

            led_buf[6] = 0x00;
            ret = pogo_keyboard_write(led_buf, sizeof(led_buf));
            if (ret) {
                kb_err("%s %d err\r\n", __func__, __LINE__);
                //return ret;
            }


            break;

        case KEYBOARD_MICDISABLE_ON_EVENT:
            kb_debug("%s %d %d\r\n", __func__, __LINE__, event);

            led_buf[7] = 0x01;
            ret = pogo_keyboard_write(led_buf, sizeof(led_buf));
            if (ret) {
                kb_err("%s %d err\r\n", __func__, __LINE__);
                return ret;
            }

            break;
        case KEYBOARD_MICDISABLE_OFF_EVENT:
            kb_debug("%s %d %d\r\n", __func__, __LINE__, event);

            led_buf[7] = 0x00;
            ret = pogo_keyboard_write(led_buf, sizeof(led_buf));
            if (ret) {
                kb_err("%s %d err\r\n", __func__, __LINE__);
                //return ret;
            }

            break;
        default:
            kb_err("%s %d no event to do err\r\n", __func__, __LINE__);
            break;
    }
    sprintf(TAG, "%s %d", __func__, __LINE__);
    pogo_keyboard_show_buf(led_buf, sizeof(led_buf));
    kb_debug("%s %d pogo_keyboard_event:%d  pogo_keyboard_status:0x%02x\n", __func__, __LINE__, event, pogo_keyboard_client->pogo_keyboard_status);
    return ret;
}

// sync host lcd/screen on/off(sleep/wakeup state) state to keyboard. state=1 means wakeup while 0 means going to sleep.
static int pogo_keyboard_set_lcd_state(bool state)
{
    int ret = 0;
    char write_buf[] = { ONE_WIRE_BUS_PACKET_USER_GENERAL_CMD, 0x03, 0x02, 0x01, 0x01 };
    char buf[] = { ONE_WIRE_BUS_PACKET_USER_GENERAL_ACK_CMD, 0x04, 0x02, 0x02 };
    char read_buf[255] = { 0 };
    int read_len = 0;
    int i = 0;

    kb_debug("%s %d state:%d\r\n", __func__, __LINE__, state);
    if (state) {
        write_buf[4] = 0x00;
    } else {
        write_buf[4] = 0x01;
    }
    for (i = 0; i < 3; i++) {
        ret = pogo_keyboard_write_and_read(write_buf, sizeof(write_buf), read_buf, &read_len);
        if (ret) {
            continue;
        }
        if (memcmp(read_buf, buf, sizeof(buf)) == 0) {

            if (read_buf[5] == 1)
                break;

        }
    }
    if (i >= 3) {
        kb_err("%s %d err ret:0x%02x status:%d\n", __func__, __LINE__, ret, read_buf[5]);
        return ret;
    }

    sprintf(TAG, "%s %d", __func__, __LINE__);
    pogo_keyboard_show_buf(read_buf, read_len);
    pogo_keyboard_heartbeat_switch(state);
    kb_debug("%s %d ok ret:0x%02x status:%d\n", __func__, __LINE__, ret, read_buf[5]);

    return 0;
}

void pogo_keyboard_led_report(int key_value)
{
    char value = 0;
    if (!(key_value == KEY_CAPSLOCK || key_value == KEY_MUTE || key_value == KEY_MICDISABLE))
        return;
    kb_debug("%s %d key_value:%d\r\n", __func__, __LINE__, key_value);

    // NOTE: currently caps lock led is controlled by upper system layer while nothing is done here.
    // should other leds follow the same? need further consideration in the future.
    if (key_value == KEY_CAPSLOCK) {
        return;
        if ((pogo_keyboard_client->pogo_keyboard_status & KEYBOARD_CAPSLOCK_ON_STATUS) == 0) {
            pogo_keyboard_client->pogo_keyboard_status |= KEYBOARD_CAPSLOCK_ON_STATUS;
        } else {
            pogo_keyboard_client->pogo_keyboard_status &= (~KEYBOARD_CAPSLOCK_ON_STATUS);
        }

        kb_debug("%s %d key_value:%d value:%d\r\n", __func__, __LINE__, key_value, value);

    } else if (key_value == KEY_MUTE) {
        if ((pogo_keyboard_client->pogo_keyboard_status & KEYBOARD_MUTEDISABLE_ON_STATUS) == 0) {
            pogo_keyboard_client->pogo_keyboard_status |= KEYBOARD_MUTEDISABLE_ON_STATUS;
        } else {
            pogo_keyboard_client->pogo_keyboard_status &= (~KEYBOARD_MUTEDISABLE_ON_STATUS);
        }
        value = pogo_keyboard_client->pogo_keyboard_status & KEYBOARD_MUTEDISABLE_ON_STATUS;
        input_event(pogo_keyboard_client->input_pogo_keyboard, EV_LED, LED_MUTE, !!value);
        kb_debug("%s %d key_value:%d value:%d\r\n", __func__, __LINE__, key_value, value);

    } else if (key_value == KEY_MICDISABLE) {
        if ((pogo_keyboard_client->pogo_keyboard_status & KEYBOARD_MICDISABLE_ON_STATUS) == 0) {
            pogo_keyboard_client->pogo_keyboard_status |= KEYBOARD_MICDISABLE_ON_STATUS;
        } else {
            pogo_keyboard_client->pogo_keyboard_status &= (~KEYBOARD_MICDISABLE_ON_STATUS);
        }
        value = pogo_keyboard_client->pogo_keyboard_status & KEYBOARD_MICDISABLE_ON_STATUS;
        input_event(pogo_keyboard_client->input_pogo_keyboard, EV_LED, LED_MIC_MUTE, !!value);
        kb_debug("%s %d key_value:%d value:%d\r\n", __func__, __LINE__, key_value, value);
    }

}

void pogo_keyboard_led_process(int code, int value)
{
    unsigned char event = 0;

    kb_info("%s %d  type:led code:0x%02x value:0x%02x\n", __func__, __LINE__, code, value);
    switch (code) {
        case LED_CAPSL:
            if (value == 1) {
                event = KEYBOARD_CAPSLOCK_ON_EVENT;
                pogo_keyboard_client->pogo_keyboard_status |= KEYBOARD_CAPSLOCK_ON_STATUS;
            } else {
                event = KEYBOARD_CAPSLOCK_OFF_EVENT;
                pogo_keyboard_client->pogo_keyboard_status &= (~KEYBOARD_CAPSLOCK_ON_STATUS);
            }
            break;
        case LED_MUTE:
            if (value == 1) {
                event = KEYBOARD_MUTEDISABLE_ON_EVENT;
                pogo_keyboard_client->pogo_keyboard_status |= KEYBOARD_MUTEDISABLE_ON_STATUS;
            } else {
                event = KEYBOARD_MUTEDISABLE_OFF_EVENT;
                pogo_keyboard_client->pogo_keyboard_status &= (~KEYBOARD_MUTEDISABLE_ON_STATUS);
            }
            break;
        case LED_MIC_MUTE:
            if (value == 1) {
                event = KEYBOARD_MICDISABLE_ON_EVENT;
                pogo_keyboard_client->pogo_keyboard_status |= KEYBOARD_MICDISABLE_ON_STATUS;
            } else {
                event = KEYBOARD_MICDISABLE_OFF_EVENT;
                pogo_keyboard_client->pogo_keyboard_status &= (~KEYBOARD_MICDISABLE_ON_STATUS);
            }
            break;
        default:
            kb_debug("%s %d  type:led code:0x%2x value:0x%2x no support err!\n", __func__, __LINE__, code, value);
            return;
    }
    if ((pogo_keyboard_client->pogo_keyboard_status & KEYBOARD_LCD_ON_STATUS) == 0)
        return;

    pogo_keyboard_event_send(event);
}

// monitor lcd/screen on/off state.
void pogo_keyboard_sync_lcd_state(bool lcd_on_state)
{
    kb_debug("%s %d lcd_on_state:%d\n", __func__, __LINE__, lcd_on_state);
    if (lcd_on_state) {
        kb_info("%s %d pogo_keyboard goto wakeup\r\n", __func__, __LINE__);
        pogo_keyboard_event_send(KEYBOARD_HOST_LCD_ON_EVENT);
    } else {
        kb_info("%s %d pogo_keyboard goto sleep\r\n", __func__, __LINE__);
        pogo_keyboard_event_send(KEYBOARD_HOST_LCD_OFF_EVENT);
    }

    return;
}

static int pogo_keyboard_enable_uart_tx(int value)
{
    if (pogo_keyboard_client->file_client == NULL)
        return -1;
    if (value == 1) {
        if (gpio_get_value(pogo_keyboard_client->tx_en_gpio) == 0) {
            kb_info("%s %d %d 0x%02x\n", __func__, __LINE__, value, pogo_keyboard_client->pogo_keyboard_status);
            gpio_direction_output(pogo_keyboard_client->tx_en_gpio, 1);
            udelay(450);//for set mode valid
        }
    } else if (value == 0) {
        if (gpio_get_value(pogo_keyboard_client->tx_en_gpio) == 1) {
            kb_info("%s %d %d\n", __func__, __LINE__, value);
            udelay(300); //for set mode valid
            gpio_direction_output(pogo_keyboard_client->tx_en_gpio, 0);
        }
    }
    return 0;
}

bool pogo_keyboard_is_support(struct uart_port *port)
{
    bool ret = false;
    if (!port || !pogo_keyboard_client)
        return false;
    if (!strncmp(port->name, pogo_keyboard_client->tty_name, strlen(pogo_keyboard_client->tty_name))) {
        ret = true;
    }
    return ret;
}

static int pogo_keyboard_get_dts_info(struct platform_device *pdev)
{
    struct device_node *node = NULL;
    struct platform_device *device = pdev;
    int ret = 0;

    kb_debug("%s %d start\n", __func__, __LINE__);

    node = device->dev.of_node;
    if (!node) {
        kb_err("of node is not null\n");
        return -EINVAL;
    }

    ret = of_property_read_string(node, "tty-name-string", (const char **)&pogo_keyboard_client->tty_name);
    if (ret) {
        kb_err("%s %d read tty name err: %d\n", __func__, __LINE__, ret);
        return ret;
    }

    ret = of_property_read_u32_index(node, "touchpad-xy-max", 0, &pogo_keyboard_client->touchpad_x_max);
    if (ret) {
        kb_err("%s %d read touchpad-xy-max err: %d\n", __func__, __LINE__, ret);
        return ret;
    }
    ret = of_property_read_u32_index(node, "touchpad-xy-max", 1, &pogo_keyboard_client->touchpad_y_max);
    if (ret) {
        kb_err("%s %d read touchpad-xy-max err: %d\n", __func__, __LINE__, ret);
        return ret;
    }

#ifdef CONFIG_PLUG_SUPPORT
    pogo_keyboard_client->plug_gpio = of_get_named_gpio(node, "plug-gpios", 0);
    if (!gpio_is_valid(pogo_keyboard_client->plug_gpio)) {
        kb_err("plug_gpio is not valid %d\n", pogo_keyboard_client->plug_gpio);
        return -EINVAL;
    }
    kb_debug("%s %d plug_gpio:%d\n", __func__, __LINE__, pogo_keyboard_client->plug_gpio);

    ret = gpio_request_one(pogo_keyboard_client->plug_gpio, GPIOF_DIR_IN, "keybaord_plug_gpio");
    if (ret) {
        kb_err("%s %d gpio_request_one err:%d\n", __func__, __LINE__, ret);
        return ret;
    }

    pogo_keyboard_client->plug_irq = gpio_to_irq(pogo_keyboard_client->plug_gpio);
    ret = devm_request_threaded_irq(&device->dev, pogo_keyboard_client->plug_irq, keybaord_core_plug_irq_handler,
        NULL, IRQF_TRIGGER_LOW | IRQF_ONESHOT, "keybaord_plug_irq", NULL);
    if (ret < 0) {
        kb_err("request irq failed : %d\n", pogo_keyboard_client->plug_irq);
        return -EINVAL;
    }

    enable_irq_wake(pogo_keyboard_client->plug_irq);


    /*
    pogo_keyboard_client->power_en_gpio = of_get_named_gpio(node,"power-en-gpios", 0);
    if (!gpio_is_valid(pogo_keyboard_client->power_en_gpio)) {
        kb_err("power_en_gpio is not valid %d\n", pogo_keyboard_client->power_en_gpio);
        return -EINVAL;
    }
    gpio_direction_output(pogo_keyboard_client->power_en_gpio,0);

*/

#endif

    pogo_keyboard_client->pinctrl = devm_pinctrl_get(&device->dev);
    if (IS_ERR(pogo_keyboard_client->pinctrl)) {
        kb_err("%s %d devm_pinctrl_get  err\n", __func__, __LINE__);
        return -1;
    }

    // pogo_keyboard_client->uart_tx_set = pinctrl_lookup_state(pogo_keyboard_client->pinctrl, "uart_tx_set");
    // if(IS_ERR(pogo_keyboard_client->uart_tx_set)) {
    // 	kb_err("%s %d pinctrl_lookup_state uart_tx_set err\n",__func__,__LINE__);
    // 	return -1;
    // }
    // pogo_keyboard_client->uart_tx_clear = pinctrl_lookup_state(pogo_keyboard_client->pinctrl,"uart_tx_clear");
    // if(IS_ERR(pogo_keyboard_client->uart_tx_clear)){
    // 	kb_err("%s %d pinctrl_lookup_state uart_tx_clear err\n",__func__,__LINE__);
    // 	return -1;
    // }

    pogo_keyboard_client->uart_rx_set = pinctrl_lookup_state(pogo_keyboard_client->pinctrl, "uart_rx_set");
    if (IS_ERR(pogo_keyboard_client->uart_rx_set)) {
        kb_err("%s %d pinctrl_lookup_state uart_rx_set err\n", __func__, __LINE__);
        return -1;
    }
    pogo_keyboard_client->uart_rx_clear = pinctrl_lookup_state(pogo_keyboard_client->pinctrl, "uart_rx_clear");
    if (IS_ERR(pogo_keyboard_client->uart_rx_clear)) {
        kb_err("%s %d pinctrl_lookup_state uart_rx_clear err\n", __func__, __LINE__);
        return -1;
    }
    ret = pinctrl_select_state(pogo_keyboard_client->pinctrl, pogo_keyboard_client->uart_rx_set);
    kb_debug("%s %d pinctrl_select_state:%d\n", __func__, __LINE__, ret);

#ifdef CONFIG_BOARD_V4_SUPPORT
    pogo_keyboard_client->uart_wake_gpio_pin = pinctrl_lookup_state(pogo_keyboard_client->pinctrl, "uart_wake_gpio");
    if (IS_ERR(pogo_keyboard_client->uart_wake_gpio_pin)) {
        kb_err("%s %d pinctrl_lookup_state uart_wake_gpio_pin err\n", __func__, __LINE__);
        return -1;
    }
    pinctrl_select_state(pogo_keyboard_client->pinctrl, pogo_keyboard_client->uart_wake_gpio_pin);

    pogo_keyboard_client->uart_wake_gpio = of_get_named_gpio(node, "uart-wake-gpio", 0);
    if (!gpio_is_valid(pogo_keyboard_client->uart_wake_gpio)) {
        kb_err("uart_wake_gpio is not valid %d\n", pogo_keyboard_client->uart_wake_gpio);
        return -EINVAL;
    }
    ret = gpio_request_one(pogo_keyboard_client->uart_wake_gpio, GPIOF_DIR_IN, "uart_wake_gpio");
    if (ret) {
        kb_err("%s %d gpio_request_one err:%d\n", __func__, __LINE__, ret);
        return ret;
    }
    pogo_keyboard_client->uart_wake_gpio_irq = gpio_to_irq(pogo_keyboard_client->uart_wake_gpio);
    dev_pm_set_wake_irq(&pogo_keyboard_client->plat_dev->dev, pogo_keyboard_client->uart_wake_gpio_irq);

    ret = devm_request_threaded_irq(&device->dev, pogo_keyboard_client->uart_wake_gpio_irq, NULL,
        pogo_keyboard_irq_handler, IRQF_TRIGGER_FALLING | IRQF_ONESHOT, "keybaord_wake_irq", NULL);
    if (ret < 0) {
        kb_err("request irq failed : %d\n", pogo_keyboard_client->uart_wake_gpio_irq);
        return -EINVAL;
    }

    enable_irq_wake(pogo_keyboard_client->uart_wake_gpio_irq);
    kb_debug("%s %d  uart_wake_gpio_irq \n", __func__, __LINE__);


    pogo_keyboard_client->tx_en_gpio = of_get_named_gpio(node, "uart-tx-en-gpio", 0);
    if (!gpio_is_valid(pogo_keyboard_client->uart_wake_gpio)) {
        kb_err("uart_wake_gpio is not valid %d\n", pogo_keyboard_client->uart_wake_gpio);
        return -EINVAL;
    }
    kb_debug("%s %d uart_wake_gpio:%d tx_en_gpio:%d uart_wake_gpio_irq:%d\n", __func__, __LINE__, pogo_keyboard_client->uart_wake_gpio,
        pogo_keyboard_client->tx_en_gpio, pogo_keyboard_client->uart_wake_gpio_irq);
    gpio_direction_output(pogo_keyboard_client->tx_en_gpio, 0);

#endif

    pogo_keyboard_client->pogo_power_enable = pinctrl_lookup_state(pogo_keyboard_client->pinctrl, "pogo_power_enable");
    if (IS_ERR(pogo_keyboard_client->pogo_power_enable)) {
        kb_err("%s %d pinctrl_lookup_state pogo_power_enable err\n", __func__, __LINE__);
        return -1;
    }
    pogo_keyboard_client->pogo_power_disable = pinctrl_lookup_state(pogo_keyboard_client->pinctrl, "pogo_power_disable");
    if (IS_ERR(pogo_keyboard_client->pogo_power_disable)) {
        kb_err("%s %d pinctrl_lookup_state pogo_power_disable err\n", __func__, __LINE__);
        return -1;
    }
    pinctrl_select_state(pogo_keyboard_client->pinctrl, pogo_keyboard_client->pogo_power_disable);

    kb_debug("%s %d ret: %d ok\n", __func__, __LINE__, ret);
    return 0;
}

static char *pogo_keyboard_get_keyboard_name(void)
{
    int ret = 0, name_cnt = 0;

    kb_debug("%s %d\n", __func__, __LINE__);

    if (pogo_keyboard_client == NULL) {
        kb_err("pogo_keyboard_client is null\n");
        return NULL;
    }

    if (pogo_keyboard_client->keyboard_brand == 0) {
        kb_err("keyboard_brand is empty\n");
        return NULL;
    }

    name_cnt = of_property_count_strings(pogo_keyboard_client->plat_dev->dev.of_node, "keyboard-name-strings");
    if (pogo_keyboard_client->keyboard_brand > name_cnt) {
        kb_err("keyboard_brand out of range\n");
        return NULL;
    }

    ret = of_property_read_string_index(pogo_keyboard_client->plat_dev->dev.of_node, "keyboard-name-strings",
        pogo_keyboard_client->keyboard_brand - 1, (const char **)&pogo_keyboard_client->keyboard_name);
    if (ret) {
        kb_err("%s %d read keyboard name err: %d\n", __func__, __LINE__, ret);
        return NULL;
    }

    return pogo_keyboard_client->keyboard_name;
}

static char *pogo_keyboard_get_keyboard_ble_name(void)
{
    int ret = 0, name_cnt = 0;

    kb_debug("%s %d\n", __func__, __LINE__);

    if (pogo_keyboard_client == NULL) {
        kb_err("%s %d pogo_keyboard_client is null\n", __func__, __LINE__);
        return NULL;
    }

    if (pogo_keyboard_client->keyboard_brand == 0) {
        kb_err("%s %d keyboard_brand error\n", __func__, __LINE__);
        return NULL;
    }

    name_cnt = of_property_count_strings(pogo_keyboard_client->plat_dev->dev.of_node, "keyboard-ble-name-strings");
    if (pogo_keyboard_client->keyboard_brand > name_cnt) {
        kb_err("%s %d keyboard_brand out of range\n", __func__, __LINE__);
        return NULL;
    }

    ret = of_property_read_string_index(pogo_keyboard_client->plat_dev->dev.of_node, "keyboard-ble-name-strings",
        pogo_keyboard_client->keyboard_brand - 1, (const char **)&pogo_keyboard_client->keyboard_ble_name);
    if (ret) {
        kb_err("%s %d read keyboard name err: %d\n", __func__, __LINE__, ret);
        return NULL;
    }

    return pogo_keyboard_client->keyboard_ble_name;
}

#ifdef CONFIG_POWER_CTRL_SUPPORT
// turn on/off vcc for keyboard.
static void pogo_keyboard_power_enable(int value)
{
    if (pogo_keyboard_client == NULL)
        return;

    if (value == 1) {
        pinctrl_select_state(pogo_keyboard_client->pinctrl, pogo_keyboard_client->pogo_power_enable);
        kb_debug("%s %d enable value:%d\n", __func__, __LINE__, value);
    } else if (value == 0) {
        pinctrl_select_state(pogo_keyboard_client->pinctrl, pogo_keyboard_client->pogo_power_disable);
        kb_debug("%s %d enable value:%d\n", __func__, __LINE__, value);
    }
}
#endif

#ifdef CONFIG_BOARD_V4_SUPPORT
// handler for uart rx wakup interrupt(press key while pad is in sleep) and keyboard attachment detection.
static irqreturn_t pogo_keyboard_irq_handler(int irq, void *data)
{
    int value;
    // process only when keyboard is not connected.
    if ((pogo_keyboard_client->pogo_keyboard_status & KEYBOARD_CONNECT_STATUS) == 0 && pogo_keyboard_client->file_client != NULL) {
        /*low level:plug in irq handler*/

        kb_debug("%s %d\n", __func__, __LINE__);
        // keyboard will drive a low signal through TRX data pin by keyboard hardware design.
        value = gpio_get_value(pogo_keyboard_client->uart_wake_gpio);
        if (value == 0) {
            kb_debug("%s %d disable_irq_nosync\n", __func__, __LINE__);
            disable_irq_nosync(irq);
            // turn on vcc only while it's off.
            if (!atomic_read(&pogo_keyboard_client->vcc_on)) {
                atomic_set(&pogo_keyboard_client->vcc_on, 1);
                kb_debug("%s %d vcc_on\n", __func__, __LINE__);
                pogo_keyboard_event_send(KEYBOARD_POWER_ON_EVENT);// signal main event task to do the following attachement procedure.
                pm_wakeup_event(&pogo_keyboard_client->plat_dev->dev, 100); //wake up lock 100ms
            }
        }
    }
    return IRQ_HANDLED;
}
#else
static irqreturn_t pogo_keyboard_rx_gpio_irq_handler(int irq, void *data)
{
    kb_debug("%s %d   \n", __func__, __LINE__);
    disable_irq_wake(pogo_keyboard_client->uart_rx_gpio_irq);
    pogo_keyboard_event_send(KEYBOARD_HOST_RX_UART_EVENT);
    return IRQ_HANDLED;
}
#endif

#if defined(CONFIG_KB_DEBUG_FS) //debugging file nodes.
static int  pogo_keyboard_plug_set(int value)
{
    int ret = 0;

    if (value == 1) {
        kb_debug("%s %d \n", __func__, __LINE__);
        ret = pogo_keyboard_input_connect();
    } else {
        pogo_keyboard_input_disconnect();
        kb_debug("%s %d \n", __func__, __LINE__);
    }

    return ret;
}

static int  pogo_keyboard_gpio_input_set(int value)
{
    int ret;
    static int data = 0;
    if (data == 0)
        data = 1;
    else
        data = 0;
    kb_debug("%s %d  value:%d data:%d\n", __func__, __LINE__, value, data);
    if (value == 9) {
        if (data == 1) {
            pogo_keyboard_client->pogo_gpio_clear = pinctrl_lookup_state(pogo_keyboard_client->pinctrl, "uart_wake_pull_up");
            if (IS_ERR(pogo_keyboard_client->pogo_gpio_clear)) {
                kb_err("%s %d pinctrl_lookup_state uart_wake_pull_up err\n", __func__, __LINE__);
                return -1;
            }
            ret = pinctrl_select_state(pogo_keyboard_client->pinctrl, pogo_keyboard_client->pogo_gpio_clear);
            if (ret) {
                kb_err("%s %d pinctrl_select_state uart_wake_pull_up err:%d\n", __func__, __LINE__, ret);
                return ret;
            }
            kb_debug("%s %d pinctrl_select_state uart_wake_pull_up ret:%d\n", __func__, __LINE__, ret);
        } else {
            pogo_keyboard_client->pogo_gpio_clear = pinctrl_lookup_state(pogo_keyboard_client->pinctrl, "uart_wake_gpio");
            if (IS_ERR(pogo_keyboard_client->pogo_gpio_clear)) {
                kb_err("%s %d pinctrl_lookup_state uart_wake_gpio err\n", __func__, __LINE__);
                return -1;
            }
            ret = pinctrl_select_state(pogo_keyboard_client->pinctrl, pogo_keyboard_client->pogo_gpio_clear);
            if (ret) {
                kb_err("%s %d pinctrl_select_state uart_wake_gpio err:%d\n", __func__, __LINE__, ret);
                return ret;
            }
            kb_debug("%s %d pinctrl_select_state uart_wake_gpio ret:%d\n", __func__, __LINE__, ret);
        }



    } else if (value == 10) {
        if (data == 1) {
            pogo_keyboard_client->pogo_gpio_clear = pinctrl_lookup_state(pogo_keyboard_client->pinctrl, "uart_rx_pull_up");
            if (IS_ERR(pogo_keyboard_client->pogo_gpio_clear)) {
                kb_err("%s %d pinctrl_lookup_state uart_rx_pull_up err\n", __func__, __LINE__);
                return -1;
            }
            ret = pinctrl_select_state(pogo_keyboard_client->pinctrl, pogo_keyboard_client->pogo_gpio_clear);
            if (ret) {
                kb_err("%s %d pinctrl_select_state uart_rx_pull_up err:%d\n", __func__, __LINE__, ret);
                return ret;
            }
            kb_debug("%s %d pinctrl_select_state uart_rx_pull_up ret:%d\n", __func__, __LINE__, ret);
        } else {
            pogo_keyboard_client->pogo_gpio_clear = pinctrl_lookup_state(pogo_keyboard_client->pinctrl, "uart_rx_set");
            if (IS_ERR(pogo_keyboard_client->pogo_gpio_clear)) {
                kb_err("%s %d pinctrl_lookup_state uart_rx_set err\n", __func__, __LINE__);
                return -1;
            }
            ret = pinctrl_select_state(pogo_keyboard_client->pinctrl, pogo_keyboard_client->pogo_gpio_clear);
            if (ret) {
                kb_err("%s %d pinctrl_select_state uart_rx_set err:%d\n", __func__, __LINE__, ret);
                return ret;
            }
            kb_debug("%s %d pinctrl_select_state uart_rx_set ret:%d\n", __func__, __LINE__, ret);
        }
    }
    return 0;
}

static ssize_t tx_mode_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    char *after;
    unsigned long value = simple_strtoul(buf, &after, 10);
    kb_debug("%s %d   %s  %d\n", __func__, __LINE__, buf, value);
    if (value == 1) {
        pogo_keyboard_enable_uart_tx(1);
    } else if (value == 0) {
        pogo_keyboard_enable_uart_tx(0);
    }
    return count;
}

static ssize_t tx_mode_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    return 0;
}

static ssize_t test_mode_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    char *after;
    unsigned long value = simple_strtoul(buf, &after, 10);
    static int data = 0;
    kb_debug("%s %d   %s  %d\n", __func__, __LINE__, buf, value);
    switch (value) {
        case 1:
        case 2:
            test_type = value;
            kb_debug("%s %d   %s  test_type:%d\n", __func__, __LINE__, buf, test_type);
            pogo_keyboard_event_send(KEYBOARD_TEST_EVENT);
            break;
        case 3:
            if (pogo_keyboard_client->port)
                pogo_keyboard_port_to_file(pogo_keyboard_client->port);
            break;

        case 4:
            if (data == 0) {
                data = 1;
            } else {
                data = 0;
            }
            kb_debug("%s %d   tx_en_gpio value:%d\n", __func__, __LINE__, data);
            gpio_direction_output(pogo_keyboard_client->tx_en_gpio, data);
            break;

        case 5:
            if (data == 0) {
                data = 1;
                if (!regulator_is_enabled(pogo_keyboard_client->vcc_reg))
                    (void)regulator_enable(pogo_keyboard_client->vcc_reg);
            } else {
                data = 0;
                if (regulator_is_enabled(pogo_keyboard_client->vcc_reg))
                    (void)regulator_disable(pogo_keyboard_client->vcc_reg);
            }
            kb_debug("%s %d   power en value:%d\n", __func__, __LINE__, data);

            break;
        case 6:
            if (data == 0) {
                data = 1;
                pinctrl_select_state(pogo_keyboard_client->pinctrl, pogo_keyboard_client->uart_rx_clear);
                kb_debug("%s %d   uart_rx_clear value:%d\n", __func__, __LINE__, data);
            } else {
                data = 0;
                pinctrl_select_state(pogo_keyboard_client->pinctrl, pogo_keyboard_client->uart_wake_clear);
                kb_debug("%s %d   uart_wake_clear value:%d\n", __func__, __LINE__, data);
            }
            break;
        case 7:
            if (data == 0) {
                data = 1;

            } else {
                data = 0;

            }
            kb_debug("%s %d   heartbeat_switch value:%d\n", __func__, __LINE__, data);
            pogo_keyboard_heartbeat_switch(data);
            break;
        case 8:
            if (data == 0) {
                data = 1;
            } else {
                data = 0;
            }
            kb_debug("%s %d   input_connect value:%d\n", __func__, __LINE__, data);
            pogo_keyboard_plug_set(data);
            break;
        case 9:
        case 10:
            pogo_keyboard_gpio_input_set(value);
            break;
        case 11:
            if (data == 0) {
                data = 1;
                pogo_debug_en = true;
            } else {
                data = 0;
                pogo_debug_en = false;
            }
            break;
        case 12:
            max_disconnect_count++;
            kb_debug("%s %d  max_disconnect_count:%d\n", __func__, __LINE__, max_disconnect_count);
            break;
        case 13:
            max_disconnect_count--;
            kb_debug("%s %d  max_disconnect_count:%d\n", __func__, __LINE__, max_disconnect_count);
            break;
        case 14:
            max_plug_in_disconnect_count++;
            kb_debug("%s %d  max_plug_in_disconnect_count:%d\n", __func__, __LINE__, max_plug_in_disconnect_count);
            break;
        case 15:
            max_plug_in_disconnect_count--;
            kb_debug("%s %d  max_plug_in_disconnect_count:%d\n", __func__, __LINE__, max_plug_in_disconnect_count);
            break;

        default:
            break;

    }
    return count;
}

static ssize_t test_mode_show(struct device *dev, struct device_attribute *attr, char *buf)
{

    return 0;
}

static ssize_t keyboard_status_show(struct device *dev, struct device_attribute *attr, char *buf)
{

    return sprintf(buf, "status:0x%02x\n", pogo_keyboard_client->pogo_keyboard_status);
}

static DEVICE_ATTR(tx_mode, S_IRUGO | S_IWUSR, tx_mode_show, tx_mode_store);
static DEVICE_ATTR(test_mode, S_IRUGO | S_IWUSR, test_mode_show, test_mode_store);
static DEVICE_ATTR(keyboard_status, S_IRUGO | S_IWUSR, keyboard_status_show, NULL);

static struct attribute *pogo_keyboard_attributes[] = {
    &dev_attr_tx_mode.attr,
    &dev_attr_test_mode.attr,
    &dev_attr_keyboard_status.attr,
    NULL
};

static struct attribute_group pogo_keyboard_attribute_group = {
    .attrs = pogo_keyboard_attributes
};
#endif//CONFIG_KB_DEBUG_FS

static int pogo_keyboard_input_connect(void)
{
    int ret = 0;
    if (!pogo_keyboard_client->input_pogo_keyboard) {
        ret = pogo_keyboard_input_init(pogo_keyboard_get_keyboard_name());
        if (ret) {
            kb_err("%s %d pogo_keyboard_input_init err:%d\n", __func__, __LINE__, ret);
            return ret;
        }
    }
    if (!pogo_keyboard_client->input_touchpad) {
        ret = touchpad_input_init();
        if (ret) {
            kb_err("%s %d touchpad_input_init err:%d\n", __func__, __LINE__, ret);
            return ret;
        }
    }
    return 0;
}

static void pogo_keyboard_input_disconnect(void)
{
    if (pogo_keyboard_client->input_touchpad) {

        input_unregister_device(pogo_keyboard_client->input_touchpad);
        //input_free_device(pogo_keyboard_client->input_touchpad);
        kb_debug("%s %d input_unregister_device \n", __func__, __LINE__);
        pogo_keyboard_client->input_touchpad = NULL;
    }
    if (pogo_keyboard_client->input_pogo_keyboard) {
        input_unregister_device(pogo_keyboard_client->input_pogo_keyboard);
        //input_free_device(pogo_keyboard_client->input_pogo_keyboard);
        kb_debug("%s %d input_unregister_device \n", __func__, __LINE__);
        pogo_keyboard_client->input_pogo_keyboard = NULL;
    }
    return;
}

static bool pogo_keyboard_key_and_touch_up(void)
{
    bool ret = false;
    if (!pogo_keyboard_client || !pogo_keyboard_client->input_pogo_keyboard || !pogo_keyboard_client->input_touchpad)
        return ret;

    if (pogo_keyboard_client->is_down) {
        input_report_key(pogo_keyboard_client->input_pogo_keyboard, pogo_keyboard_client->down_code, 0);
        input_sync(pogo_keyboard_client->input_pogo_keyboard);
        memset(pogo_keyboard_client->old, 0, sizeof(pogo_keyboard_client->old));
        pogo_keyboard_client->is_down = false;
        kb_debug("%s %d key code %d up\n", __func__, __LINE__, pogo_keyboard_client->down_code);
        ret = true;
    }
    if (pogo_keyboard_client->is_mmdown) {
        input_report_key(pogo_keyboard_client->input_pogo_keyboard, pogo_keyboard_client->down_mmcode, 0);
        input_sync(pogo_keyboard_client->input_pogo_keyboard);
        memset(pogo_keyboard_client->mm_old, 0, sizeof(pogo_keyboard_client->mm_old));
        pogo_keyboard_client->is_mmdown = false;
        kb_debug("%s %d key mmcode %d up\n", __func__, __LINE__, pogo_keyboard_client->down_mmcode);
        ret = true;
    }
    if (pogo_keyboard_client->touch_down) { //finger all up
        input_report_key(pogo_keyboard_client->input_touchpad, BTN_TOUCH, 0);
        input_sync(pogo_keyboard_client->input_touchpad);
        pogo_keyboard_client->touch_temp = 0;
        pogo_keyboard_client->touch_down = 0;
        kb_debug("%s %d finger all up\n", __func__, __LINE__);
        ret = true;
    }
    return ret;
}

static void pogo_keyboard_connect_send_uevent(void)
{
    char status_string[32], mac_string[32], name_string[64];
    char *envp[] = { status_string, mac_string, name_string, NULL };
    char *keyboard_ble_name = NULL;
    int ret = 0;

    snprintf(status_string, sizeof(status_string), "pogopin_status=%d", pogo_keyboard_client->pogo_keyboard_status & KEYBOARD_CONNECT_STATUS);
    snprintf(mac_string, sizeof(mac_string), "mac_addr=%012llx", pogo_keyboard_client->mac_addr);
    keyboard_ble_name = pogo_keyboard_get_keyboard_ble_name();
    if (keyboard_ble_name == NULL) {
        keyboard_ble_name = KEYBOARD_NAME;
    }
    snprintf(name_string, sizeof(name_string), "keyboard_ble_name=%s", keyboard_ble_name);
    ret = kobject_uevent_env(&pogo_keyboard_client->uevent_dev->kobj, KOBJ_CHANGE, envp);
    if (ret) {
        kb_err("%s: kobject_uevent_fail, ret = %d", __func__, ret);
    }

    kb_debug("send uevent:%s %s %s.\n", status_string, mac_string, name_string);
}

static int pogo_keyboard_event_process(unsigned char pogo_keyboard_event)
{
    int ret = 0;
    kb_debug("%s %d pogo_keyboard_event:%d  pogo_keyboard_status:0x%02x\n", __func__, __LINE__, pogo_keyboard_event, pogo_keyboard_client->pogo_keyboard_status);
    switch (pogo_keyboard_event) {
        case KEYBOARD_PLUG_IN_EVENT:
            if ((pogo_keyboard_client->pogo_keyboard_status & KEYBOARD_CONNECT_STATUS) == 0 &&
                atomic_read(&pogo_keyboard_client->vcc_on)) {
                pogo_keyboard_client->pogo_keyboard_status |= KEYBOARD_CONNECT_STATUS;
#ifdef CONFIG_POWER_CTRL_SUPPORT
                // pogo_keyboard_power_enable(1);
#endif
                ret = pogo_keyboard_input_connect();
                if (ret) {
                    kb_err("%s %d pogo_keyboard_input_connect err!\n", __func__, __LINE__);
                    //break;
                }

                pogo_keyboard_heartbeat_switch(1);
                pogo_keyboard_connect_send_uevent();
                pogo_keyboard_client->poweroff_timer_check_count = 0;
            }
            break;
        case KEYBOARD_PLUG_OUT_EVENT:
            if ((pogo_keyboard_client->pogo_keyboard_status & KEYBOARD_CONNECT_STATUS) != 0) {
                pogo_keyboard_enable_uart_tx(0);
#ifdef CONFIG_POWER_CTRL_SUPPORT
                // pogo_keyboard_power_enable(0);
#endif
                kb_debug("%s %d \n", __func__, __LINE__);
                if (pogo_keyboard_key_and_touch_up())
                    mdelay(10);
                pogo_keyboard_input_disconnect();
                pogo_keyboard_client->pogo_keyboard_status &= ~KEYBOARD_CONNECT_STATUS;
                pogo_keyboard_connect_send_uevent();

                kb_debug("%s %d \n", __func__, __LINE__);
            }

            break;


        case KEYBOARD_HOST_LCD_ON_EVENT:
            pogo_keyboard_client->pogo_keyboard_status |= KEYBOARD_LCD_ON_STATUS;
            if ((pogo_keyboard_client->pogo_keyboard_status & KEYBOARD_CONNECT_STATUS) != 0) {

                ret = pogo_keyboard_set_lcd_state(true);
                if (ret) {
                    kb_err("%s %d pogo_keyboard_set_lcd_state err!\n", __func__, __LINE__);
                }
                pogo_keyboard_set_led(pogo_keyboard_event);
            }
            pogo_keyboard_heartbeat_switch(1);
            (void)pogo_keyboard_key_and_touch_up();
            break;

        case KEYBOARD_HOST_LCD_OFF_EVENT:
            if ((pogo_keyboard_client->pogo_keyboard_status & KEYBOARD_CONNECT_STATUS) != 0) {
                ret = pogo_keyboard_set_lcd_state(false);
                if (ret) {
                    kb_err("%s %d pogo_keyboard_set_lcd_state err!\n", __func__, __LINE__);
                }
            }
            pogo_keyboard_client->pogo_keyboard_status &= ~KEYBOARD_LCD_ON_STATUS;
            break;

        case KEYBOARD_CAPSLOCK_ON_EVENT:
        case KEYBOARD_CAPSLOCK_OFF_EVENT:
        case KEYBOARD_MUTEDISABLE_ON_EVENT:
        case KEYBOARD_MUTEDISABLE_OFF_EVENT:
        case KEYBOARD_MICDISABLE_ON_EVENT:
        case KEYBOARD_MICDISABLE_OFF_EVENT:
            if ((pogo_keyboard_client->pogo_keyboard_status & KEYBOARD_CONNECT_STATUS) != 0) {
                pogo_keyboard_set_led(pogo_keyboard_event);
            }
            break;

        case KEYBOARD_HOST_RX_GPIO_EVENT:
            enable_irq_wake(pogo_keyboard_client->uart_wake_gpio_irq);
            break;
        case KEYBOARD_HOST_RX_UART_EVENT:
            disable_irq_wake(pogo_keyboard_client->uart_wake_gpio_irq);
            break;
        case KEYBOARD_POWER_ON_EVENT:
            if (atomic_read(&pogo_keyboard_client->vcc_on)) {
                pogo_keyboard_heartbeat_switch(1);
#ifdef CONFIG_POWER_CTRL_SUPPORT
                pogo_keyboard_power_enable(1);
#endif
            }
            break;
        case KEYBOARD_POWER_OFF_EVENT:
            if (!atomic_read(&pogo_keyboard_client->vcc_on)) {
#ifdef CONFIG_POWER_CTRL_SUPPORT
                pogo_keyboard_power_enable(0);
#endif
                if ((pogo_keyboard_client->pogo_keyboard_status & KEYBOARD_CONNECT_STATUS) != 0) {
                    pogo_keyboard_event_send(KEYBOARD_PLUG_OUT_EVENT);
                }
            }

            kb_debug("%s %d KEYBOARD_POWER_OFF_EVENT %d\n", __func__, __LINE__, pogo_keyboard_client->poweroff_timer_check_count);
            if (pogo_keyboard_client->poweroff_timer_check_count < POWEROFF_TIMER_CHECK_MAX) {
                pogo_keyboard_client->poweroff_disconnect_count = 0;
                pogo_keyboard_client->poweroff_connect_count = 0;
                pogo_keyboard_client->poweroff_timer_check_count++;
                pogo_keyboard_poweroff_timer_switch(true);
            } else {
                kb_debug("%s %d enable_irq\n", __func__, __LINE__);
                enable_irq(pogo_keyboard_client->uart_wake_gpio_irq);
            }
            break;

#if defined(CONFIG_KB_DEBUG_FS) // debugging apis.
        case KEYBOARD_HOST_CHECK_EVENT:
            ret = pogo_keyboard_ver();
            if (ret != 0) {
                pogo_keyboard_input_disconnect();
                pogo_keyboard_client->pogo_keyboard_status &= ~KEYBOARD_CONNECT_STATUS;
#ifdef CONFIG_POWER_CTRL_SUPPORT
                pogo_keyboard_power_enable(0);
#endif
            }
            break;

        case KEYBOARD_TEST_EVENT:
            pogo_keyboard_test(); //for test
            break;
#endif//CONFIG_KB_DEBUG_FS

        default:
            kb_err("%s %d no event do!\n", __func__, __LINE__);
            break;
    }
    kb_debug("%s %d pogo_keyboard_event:%d  pogo_keyboard_status:0x%02x\n", __func__, __LINE__, pogo_keyboard_event, pogo_keyboard_client->pogo_keyboard_status);
    return 0;
}

#ifdef CONFIG_PLUG_SUPPORT
static irqreturn_t keybaord_core_plug_irq_handler(int irq, void *data)
{
    int value = 0;
    value = gpio_get_value(pogo_keyboard_client->plug_gpio);
    if (value == 1) {
        irq_set_irq_type(pogo_keyboard_client->plug_irq, IRQ_TYPE_LEVEL_LOW);
    } else {
        irq_set_irq_type(pogo_keyboard_client->plug_irq, IRQ_TYPE_LEVEL_HIGH);
    }
    kb_info("plug irq gpio = %d\r\n", value);
    if (value == 0) {
#ifdef CONFIG_POWER_CTRL_SUPPORT
        pogo_keyboard_power_enable(1); // different from trx low signal interrupt, why turn on vcc directly within int handler?
#endif
        pm_stay_awake(&pogo_keyboard_client->plat_dev->dev);
        kb_info("%s %d %d\n", __func__, __LINE__, value);
    } else {
        kb_debug("%s %d %d\n", __func__, __LINE__, value);
        pogo_keyboard_event_send(KEYBOARD_PLUG_OUT_EVENT);
    }
    return IRQ_HANDLED;
}
#endif

static int pogo_keyboard_event_handler(void *unused)
{
    struct pogo_keyboard_event new_event = { 0 };
    int ret = 0;

    do {
        if (kfifo_is_empty(&pogo_keyboard_client->event_fifo)) {
            kb_debug("%s %d waiter\n", __func__, __LINE__);
            wait_event_interruptible(waiter, pogo_keyboard_client->flag != 0);
        } else {
            ret = kfifo_out(&pogo_keyboard_client->event_fifo, &new_event, sizeof(struct pogo_keyboard_event));
            if (ret == sizeof(struct pogo_keyboard_event)) {
                pogo_keyboard_event_process(new_event.event);
            } else {
                kb_err("%s %d  kfifo_out err!\n", __func__, __LINE__);
            }
            pogo_keyboard_client->flag = 0;
        }
    } while (!kthread_should_stop());

    kb_debug("touch_event_handler exit\n");
    return 0;
}

int pogo_keyboard_write_callback(void *param, int enable)
{
    if (enable == 1) {
        if ((pogo_keyboard_client->pogo_keyboard_status & KEYBOARD_LCD_ON_STATUS) != 0) {//send cmd to kb only if lcd is on.
            pogo_keyboard_enable_uart_tx(1);
            kb_debug("%s %d write start\n", __func__, __LINE__);
        } else {
            kb_info("%s %d no do 0x%02x\n", __func__, __LINE__, pogo_keyboard_client->pogo_keyboard_status);
        }
    } else if (enable == 0) {
        pogo_keyboard_enable_uart_tx(0);
        kb_debug("%s %d write end\n", __func__, __LINE__);
    } else {
        kb_err("%s %d  param err!\n", __func__, __LINE__);
        return -1;
    }
    return 0;
}

struct file *pogo_keyboard_port_to_file(struct uart_port *port)
{
    struct uart_port *uart_port = port;
    struct uart_state *state = uart_port->state;
    struct tty_struct *tty = state->port.itty;
    struct tty_file_private *priv;
    struct file *filp = NULL;
    spin_lock(&tty->files_lock);
    list_for_each_entry(priv, &tty->tty_files, list)
    {
        if (priv != NULL) {
            filp = priv->file;
            kb_debug("%s %d  %p major:%d minor:%d", __func__, __LINE__, filp, imajor(filp->f_inode), iminor(filp->f_inode));
            if (!strncmp(port->name, pogo_keyboard_client->tty_name, strlen(pogo_keyboard_client->tty_name))) {
                kb_debug("%s %d  %p found", __func__, __LINE__, filp);
                break;
            }
        }
    }
    spin_unlock(&tty->files_lock);
    return filp;
}

// called only once by system uart driver when uart port is opened/ready to detect keyboard attachment.
int pogo_keyboard_init_callback(void *port, int type)
{
    struct file *filp;
    kb_debug("%s %d %d\n", __func__, __LINE__, type);
    if (!port)
        return -1;
    if (type == 1) {
        pogo_keyboard_client->port = (struct uart_port *)port;
        filp = pogo_keyboard_port_to_file(pogo_keyboard_client->port);
        if (pogo_keyboard_client->file_client == NULL && filp != NULL) {
            pogo_keyboard_client->file_client = filp;
            pogo_keyboard_plug_switch(1);
            kb_info("%s %d  init ok\n", __func__, __LINE__);
        }
        kb_debug("%s %d  start %p\n", __func__, __LINE__, pogo_keyboard_client->file_client);
    } else if (type == 0) {
        filp = pogo_keyboard_port_to_file(pogo_keyboard_client->port);
        if (pogo_keyboard_client->file_client != NULL && pogo_keyboard_client->file_client == filp) {
            pogo_keyboard_client->file_client = NULL;
            kb_err("%s %d  init err\n", __func__, __LINE__);
        }
        pogo_keyboard_client->port = NULL;
        kb_debug("%s %d  end\n", __func__, __LINE__);
    } else {
        kb_err("%s %d  param err!\n", __func__, __LINE__);
        return -1;
    }
    return 0;
}

#if IS_ENABLED(CONFIG_QCOM_PANEL_EVENT_NOTIFIER) && defined(CONFIG_OPLUS_POGOPIN_FUNCTION)
static void pogo_keyboard_drm_notifier_callback(enum panel_event_notifier_tag tag,
    struct panel_event_notification *notification, void *priv)
{
    if (!notification) {
        kb_err("Invalid notification\n");
        return;
    }

    switch (notification->notif_type) {
        case DRM_PANEL_EVENT_UNBLANK:
            kb_info("%s %d event:%d pogo_keyboard goto wakeup\n", __func__, __LINE__, notification->notif_data.early_trigger);
            if (!notification->notif_data.early_trigger) {
                pogo_keyboard_sync_lcd_state(true);
            }
            break;
        case DRM_PANEL_EVENT_BLANK:
            kb_info("%s %d event:%d pogo_keyboard goto sleep\n", __func__, __LINE__, notification->notif_data.early_trigger);
            if (!notification->notif_data.early_trigger) {
                pogo_keyboard_sync_lcd_state(false);
            }
            break;
        case DRM_PANEL_EVENT_BLANK_LP:
            break;
        case DRM_PANEL_EVENT_FPS_CHANGE:
            break;
        default:
            break;
    }
}

static int pogo_keyboard_register_drm_notify(void)
{
    int i = 0, count = 0;
    struct device_node *np = NULL;
    struct device_node *node = NULL;
    struct drm_panel *panel = NULL;
    void *cookie = NULL;

    np = of_find_node_by_name(NULL, "oplus,dsi-display-dev");
    if (!np) {
        kb_err("device tree info. missing\n");
        return 0;
    }
    count = of_count_phandle_with_args(np, "oplus,dsi-panel-primary", NULL);
    if (count <= 0) {
        kb_err("primary panel no found\n");
        return 0;
    }
    for (i = 0; i < count; i++) {
        node = of_parse_phandle(np, "oplus,dsi-panel-primary", i);
        panel = of_drm_find_panel(node);
        of_node_put(node);
        if (!IS_ERR(panel)) {
            pogo_keyboard_client->active_panel = panel;
            kb_err("find active_panel\n");
            break;
        }
    }

    if (i >= count) {
        kb_err("%s %d can't find active panel\n", __func__, __LINE__);
        return -ENODEV;
    }

    cookie = panel_event_notifier_register(
        PANEL_EVENT_NOTIFICATION_PRIMARY,
        PANEL_EVENT_NOTIFIER_CLIENT_POGOPIN,
        panel, &pogo_keyboard_drm_notifier_callback,
        NULL);
    if (!cookie) {
        kb_err("Unable to register pogo_keyboard_panel_notifier\n");
        return -EINVAL;
    } else {
        pogo_keyboard_client->notifier_cookie = cookie;
        kb_info("success register pogo_keyboard_panel_notifier\n");
    }

    kb_info("%s %d ok\n", __func__, __LINE__);
    return 0;
}
#endif

static int pogo_keyboard_lcd_event_register(void)
{
    int ret = 0;

#if IS_ENABLED(CONFIG_QCOM_PANEL_EVENT_NOTIFIER) && defined(CONFIG_OPLUS_POGOPIN_FUNCTION)
    ret = pogo_keyboard_register_drm_notify();
#endif
    return ret;
}

static void pogo_keyboard_lcd_event_unregister(void)
{
    if (!pogo_keyboard_client->lcd_notify_reg) {
        return;
    }

#if IS_ENABLED(CONFIG_QCOM_PANEL_EVENT_NOTIFIER) && defined(CONFIG_OPLUS_POGOPIN_FUNCTION)
    if (pogo_keyboard_client->active_panel && pogo_keyboard_client->notifier_cookie)
        panel_event_notifier_unregister(pogo_keyboard_client->notifier_cookie);
#endif
}

#define LCD_REG_RETRY_COUNT_MAX		100
#define LCD_REG_RETRY_DELAY_MS		100
static void pogo_keyboard_lcd_notify_reg_work(struct work_struct *work)
{
    static int retry_count = 0;
    int ret = 0;

    if (retry_count >= LCD_REG_RETRY_COUNT_MAX)
        return;

    ret = pogo_keyboard_lcd_event_register();
    if (ret < 0) {
        retry_count++;
        kb_info("lcd panel not ready, count=%d\n", retry_count);
        schedule_delayed_work(&pogo_keyboard_client->lcd_notify_reg_work,
            msecs_to_jiffies(LCD_REG_RETRY_DELAY_MS));
        return;
    }
    retry_count = 0;
    pogo_keyboard_client->lcd_notify_reg = true;
}

#ifdef CONFIG_OPLUS_POGOPIN_FUNCTION
extern struct pogo_keyboard_operations *get_pogo_keyboard_operations(void);
#else
struct pogo_keyboard_operations *get_pogo_keyboard_operations(void)
{
    return NULL;
}
#endif

static void pogo_keyboard_register_callback(void)
{
    struct pogo_keyboard_operations *client = get_pogo_keyboard_operations();
    kb_debug("%s %d start\n", __func__, __LINE__);
    if (client == NULL)
        return;
    if (client->init == NULL) {
        client->init = pogo_keyboard_init_callback;
        client->write = pogo_keyboard_write_callback;
        client->recv = pogo_keyboard_recv_callback;
        client->resume = pogo_keyboard_plat_resume;
        client->suspend = pogo_keyboard_plat_suspend;
        client->remove = pogo_keyboard_plat_remove;
        client->check = pogo_keyboard_is_support;
        kb_info("%s %d end\n", __func__, __LINE__);
    }
    kb_debug("%s %d end\n", __func__, __LINE__);
}

static void pogo_keyboard_unregister_callback(void)
{
    struct pogo_keyboard_operations *client = get_pogo_keyboard_operations();
    kb_debug("%s %d start\n", __func__, __LINE__);
    if (client == NULL)
        return;
    if (client->init) {
        client->init = NULL;
        client->write = NULL;
        client->recv = NULL;
        client->resume = NULL;
        client->suspend = NULL;
        client->remove = NULL;
        client->check = NULL;
        kb_info("%s %d end\n", __func__, __LINE__);
    }
    kb_debug("%s %d end\n", __func__, __LINE__);
}

// timer for first keyboard attachment detection after system init. run only once.
static enum hrtimer_restart keybaord_core_plug_hrtimer(struct hrtimer *timer)
{
    int value = 0;

    if (pogo_keyboard_client->file_client == NULL) {
        return HRTIMER_NORESTART;
    }

    value = gpio_get_value(pogo_keyboard_client->uart_wake_gpio); // will get 0 if keyboard is attached(trx pin low).

#ifdef CONFIG_PLUG_SUPPORT
    value = gpio_get_value(pogo_keyboard_client->plug_gpio); // will get 0 if keyboard is attached(hall sensor).
#endif
    kb_debug("gpio read value = %d\r\n", value);
    if (value == 0) {
        if (!atomic_read(&pogo_keyboard_client->vcc_on)) {
            atomic_set(&pogo_keyboard_client->vcc_on, 1);
            kb_debug("%s %d %d disable_irq_nosync\n", __func__, __LINE__, value);
            pogo_keyboard_event_send(KEYBOARD_POWER_ON_EVENT);
            disable_irq_nosync(pogo_keyboard_client->uart_wake_gpio_irq);
        }

    }

    return HRTIMER_NORESTART;
}

// timer for monitoring keyboard heartbeat report periodically.
static enum hrtimer_restart keybaord_core_heartbeat_hrtimer(struct hrtimer *timer)
{
    kb_debug("disconnect_count = %d max_disconnect_count = %d max_plug_in_disconnect_count = %d\n",
        pogo_keyboard_client->disconnect_count, max_disconnect_count, max_plug_in_disconnect_count);

    if (pogo_keyboard_client->disconnect_count >= max_disconnect_count && pogo_keyboard_client->plug_in_count >= max_plug_in_disconnect_count) {
        if ((pogo_keyboard_client->pogo_keyboard_status & KEYBOARD_CONNECT_STATUS) || atomic_read(&pogo_keyboard_client->vcc_on)) {
            atomic_set(&pogo_keyboard_client->vcc_on, 0);
            kb_info("%s %d plug out for disconnect_count = %d\r\n", __func__, __LINE__, pogo_keyboard_client->disconnect_count);
            pogo_keyboard_event_send(KEYBOARD_POWER_OFF_EVENT);
        }
        pogo_keyboard_client->disconnect_count = 0;
        pogo_keyboard_client->plug_in_count = 0;
    } else {
        pogo_keyboard_client->disconnect_count++;
        if ((pogo_keyboard_client->pogo_keyboard_status & KEYBOARD_CONNECT_STATUS) || atomic_read(&pogo_keyboard_client->vcc_on)) {
            // kb_debug("keyboard is connected");
            pogo_keyboard_heartbeat_switch(1); // restart the timer.
        }
    }

    if (pogo_keyboard_client->plug_in_count <= max_plug_in_disconnect_count)
        pogo_keyboard_client->plug_in_count++;

    return HRTIMER_NORESTART;
}

static enum hrtimer_restart keybaord_core_poweroff_hrtimer(struct hrtimer *timer)
{
    int value = 0;

    kb_debug("%s %d\n", __func__, __LINE__);

    value = gpio_get_value(pogo_keyboard_client->uart_wake_gpio);
    if (value == 0) {
        pogo_keyboard_client->poweroff_disconnect_count = 0;
        pogo_keyboard_client->poweroff_connect_count++;
    } else {
        pogo_keyboard_client->poweroff_connect_count = 0;
        pogo_keyboard_client->poweroff_disconnect_count++;
    }

    if (pogo_keyboard_client->poweroff_disconnect_count >= POWEROFF_DISCONNECT_MAX) {
        kb_info("%s %d no restart and enable irq\n", __func__, __LINE__);
        enable_irq(pogo_keyboard_client->uart_wake_gpio_irq);
        return HRTIMER_NORESTART;
    }

    if (pogo_keyboard_client->poweroff_connect_count >= POWEROFF_CONNECT_MAX) {
        kb_info("%s %d power on! %d %d\n", __func__, __LINE__, pogo_keyboard_client->pogo_keyboard_status, atomic_read(&pogo_keyboard_client->vcc_on));
        if ((pogo_keyboard_client->pogo_keyboard_status & KEYBOARD_CONNECT_STATUS) == 0 && pogo_keyboard_client->file_client != NULL) {
            if (!atomic_read(&pogo_keyboard_client->vcc_on)) {
                atomic_set(&pogo_keyboard_client->vcc_on, 1);
                pogo_keyboard_event_send(KEYBOARD_POWER_ON_EVENT);// signal main event task to do the following attachement procedure.
                pm_wakeup_event(&pogo_keyboard_client->plat_dev->dev, 2500);
            }
        }
        return HRTIMER_NORESTART;
    }

    pogo_keyboard_poweroff_timer_switch(true);
    return HRTIMER_NORESTART;
}

static int pogo_keyboard_start_up_init(void)
{
    pogo_keyboard_client->pogo_keyboard_status |= KEYBOARD_LCD_ON_STATUS;
    kb_debug("%s %d pogo_keyboard_status:0x%02x\n", __func__, __LINE__, pogo_keyboard_client->pogo_keyboard_status);
    atomic_set(&pogo_keyboard_client->vcc_on, 0);
    hrtimer_init(&pogo_keyboard_client->plug_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
    pogo_keyboard_client->plug_timer.function = keybaord_core_plug_hrtimer;

    hrtimer_init(&pogo_keyboard_client->heartbeat_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
    pogo_keyboard_client->heartbeat_timer.function = keybaord_core_heartbeat_hrtimer;

    hrtimer_init(&pogo_keyboard_client->poweroff_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
    pogo_keyboard_client->poweroff_timer.function = keybaord_core_poweroff_hrtimer;

    return 0;
}

static ssize_t proc_tty_name_read(struct file *file, char __user *user_buf,
    size_t count, loff_t *ppos)
{
    int ret = 0;

    kb_debug("%s, %d\n", __func__, __LINE__);
    if (pogo_keyboard_client && pogo_keyboard_client->tty_name) {
        ret = simple_read_from_buffer(user_buf, count, ppos, pogo_keyboard_client->tty_name, strlen(pogo_keyboard_client->tty_name));
    }
    return ret;
}

static const struct proc_ops proc_tty_name_ops = {
    .proc_read = proc_tty_name_read,
    .proc_open = simple_open,
    .proc_lseek = default_llseek,
};

static int pogo_keyboard_init_proc(void)
{
    int ret = 0;
    struct proc_dir_entry *prEntry_keyboard = NULL;
    struct proc_dir_entry *prEntry_tmp = NULL;

    kb_err("%s %d\n", __func__, __LINE__);
    prEntry_keyboard = proc_mkdir("pogopin", NULL);
    if (prEntry_keyboard == NULL) {
        kb_err("%s %d couldn't create entry\n", __func__, __LINE__);
        ret = -ENOMEM;
    }

    prEntry_tmp = proc_create("tty_name", 0444, prEntry_keyboard, &proc_tty_name_ops);
    if (prEntry_tmp == NULL) {
        kb_err("%s %d couldn't create proc entry\n", __func__, __LINE__);
        ret = -ENOMEM;
    }

    return ret;
}

static int pogo_keyboard_init_device_uevent(void)
{
    int status = 0;
    dev_t devt;

    pogo_keyboard_client->uevent_class = class_create(THIS_MODULE, "pogopin");
    if (pogo_keyboard_client->uevent_class == NULL) {
        return EINVAL;
    }

    status = alloc_chrdev_region(&devt, 0, 1, "keyboard");
    if (status < 0) {
        class_destroy(pogo_keyboard_client->uevent_class);
        return status;
    }

    pogo_keyboard_client->uevent_dev = device_create(pogo_keyboard_client->uevent_class, NULL, devt, NULL, "%s", KEYBOARD_NAME);
    if (pogo_keyboard_client->uevent_dev == NULL) {
        unregister_chrdev_region(devt, 1);
        class_destroy(pogo_keyboard_client->uevent_class);
        return EINVAL;
    }

    pogo_keyboard_client->uevent_dev->devt = devt;

    return 0;
}

static void pogo_keyboard_deinit_device_uevent(void)
{
    device_destroy(pogo_keyboard_client->uevent_class, pogo_keyboard_client->uevent_dev->devt);
    unregister_chrdev_region(pogo_keyboard_client->uevent_dev->devt, 1);
    class_destroy(pogo_keyboard_client->uevent_class);
}

int pogo_keyboard_plat_probe(struct platform_device *device)
{
    int ret = 0;
    struct device *dev = &device->dev;

    kb_debug("%s %d start\n", __func__, __LINE__);

    pogo_keyboard_client = kzalloc(sizeof(*pogo_keyboard_client), GFP_KERNEL);
    if (!pogo_keyboard_client) {
        kb_err("%s %d kzalloc err\n", __func__, __LINE__);
        return -ENOMEM;
    }
    mutex_init(&pogo_keyboard_client->mutex);

    ret = kfifo_alloc(&pogo_keyboard_client->event_fifo,
        sizeof(struct pogo_keyboard_event) * POGO_KEYBOARD_EVENT_MAX, GFP_KERNEL);
    if (ret) {
        kb_err("%s %d kfifo_alloc fail\n", __func__, __LINE__);
        return ret;
    }
    spin_lock_init(&pogo_keyboard_client->event_fifo_lock);

    pogo_keyboard_client->pogo_keyboard_task = kthread_run(pogo_keyboard_event_handler, 0, "pogo_keyboard_task");
    if (IS_ERR_OR_NULL(pogo_keyboard_client->pogo_keyboard_task)) {
        kb_err("%s %d kthread_run err\n", __func__, __LINE__);
        pogo_keyboard_client->pogo_keyboard_task = NULL;
    }
    pogo_keyboard_client->plat_dev = device;
    ret = pogo_keyboard_get_dts_info(device);
    if (ret != 0) {
        return ret;
    }
    ret = pogo_keyboard_input_wakeup_init();
    if (ret != 0) {
        return ret;
    }

    pogo_keyboard_register_callback();
    device_init_wakeup(dev, 1);
    pogo_keyboard_start_up_init();

#if defined(CONFIG_KB_DEBUG_FS)
    ret = sysfs_create_group(&device->dev.kobj, &pogo_keyboard_attribute_group);
    if (ret != 0) {
        kb_err("%s %d sysfs_create_group err\n", __func__, __LINE__);
        sysfs_remove_group(&device->dev.kobj, &pogo_keyboard_attribute_group);
        return -EIO;
    }
#endif//CONFIG_KB_DEBUG_FS

    ret = pogo_keyboard_init_device_uevent();
    if (ret != 0) {
        kb_err("%s %d pogo_keyboard_init_device_uevent err\n", __func__, __LINE__);
        return ret;
    }

    ret = pogo_keyboard_init_proc();
    if (ret != 0) {
        kb_err("%s %d pogo_keyboard_init_proc err\n", __func__, __LINE__);
        return ret;
    }

    INIT_DELAYED_WORK(&pogo_keyboard_client->lcd_notify_reg_work, pogo_keyboard_lcd_notify_reg_work);
    schedule_delayed_work(&pogo_keyboard_client->lcd_notify_reg_work, 0);

    kb_info("%s %d ok\n", __func__, __LINE__);

    return 0;
}

static int pogo_keyboard_plat_remove(struct platform_device *device)
{
    pogo_keyboard_unregister_callback();
    if (pogo_keyboard_client->input_wakeup) {
        input_unregister_device(pogo_keyboard_client->input_wakeup);
        input_free_device(pogo_keyboard_client->input_wakeup);
        kb_debug("%s %d input_unregister_device \n", __func__, __LINE__);
        pogo_keyboard_client->input_touchpad = NULL;
    }
    if (pogo_keyboard_client->input_touchpad) {

        input_unregister_device(pogo_keyboard_client->input_touchpad);
        input_free_device(pogo_keyboard_client->input_touchpad);
        kb_debug("%s %d input_unregister_device \n", __func__, __LINE__);
        pogo_keyboard_client->input_touchpad = NULL;
    }
    if (pogo_keyboard_client->input_pogo_keyboard) {
        input_unregister_device(pogo_keyboard_client->input_pogo_keyboard);
        input_free_device(pogo_keyboard_client->input_pogo_keyboard);
        kb_debug("%s %d input_unregister_device \n", __func__, __LINE__);
        pogo_keyboard_client->input_pogo_keyboard = NULL;
    }
    sysfs_remove_group(&device->dev.kobj, &pogo_keyboard_attribute_group);
    if (pogo_keyboard_client) {
        kfree(pogo_keyboard_client);
    }

    pogo_keyboard_lcd_event_unregister();
    pogo_keyboard_deinit_device_uevent();

    kfree(pogo_keyboard_client);
    kb_debug("%s %d \n", __func__, __LINE__);
    return 0;
}

static int pogo_keyboard_plat_suspend(struct platform_device *device)
{
    kb_info("%s %d \n", __func__, __LINE__);
    return 0;
}

static int pogo_keyboard_plat_resume(struct platform_device *device)
{
    kb_info("%s %d \n", __func__, __LINE__);
    return 0;
}

static void pogo_keyboard_plat_shutdown(struct platform_device *device)
{
    kb_debug("%s %d \n", __func__, __LINE__);
}


static const struct of_device_id pogo_keyboard_plat_of_match[] = {
    {.compatible = KEYBOARD_CORE_NAME,},
    {},
};
static struct platform_driver pogo_keyboard_plat_driver = {
    .probe = pogo_keyboard_plat_probe,
    .remove = pogo_keyboard_plat_remove,
    // .suspend = pogo_keyboard_plat_suspend,
    // .resume = pogo_keyboard_plat_resume,
    .shutdown = pogo_keyboard_plat_shutdown,
    .driver = {
        .name = KEYBOARD_CORE_NAME,
        .owner = THIS_MODULE,
        .of_match_table = pogo_keyboard_plat_of_match,
    }
};


static int __init pogo_keyboard_mod_init(void)
{
    int ret = 0;
    kb_debug("%s %d start\n", __func__, __LINE__);


    ret = platform_driver_register(&pogo_keyboard_plat_driver);
    if (ret) {
        kb_err("%s %d platform_driver_register err\n", __func__, __LINE__);
        return ret;
    }
    kb_info("%s %d ok\n", __func__, __LINE__);
    return 0;
}

static void __exit pogo_keyboard_mod_exit(void)
{
    platform_driver_unregister(&pogo_keyboard_plat_driver);
    kb_info("%s %d ok\n", __func__, __LINE__);
}

module_init(pogo_keyboard_mod_init);
module_exit(pogo_keyboard_mod_exit);

MODULE_AUTHOR("Tinno Team Inc");
MODULE_DESCRIPTION("Tinno Keyboard Driver v1.0");
MODULE_LICENSE("GPL");
