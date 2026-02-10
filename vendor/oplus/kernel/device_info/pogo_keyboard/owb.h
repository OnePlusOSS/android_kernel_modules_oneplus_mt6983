#ifndef  __ONE_WIRE_BUS_PHY_H__
#define  __ONE_WIRE_BUS_PHY_H__
#include <linux/kernel.h>
#include <linux/random.h>

#define ONE_WIRE_BUS_SIMPLE_PROTOCOL

/**
 * @brief 一帧数据的开头同步字
 */
#define  ONE_WIRE_BUS_PACKET_HEAD_SYNC_CODE        0x55


/**
 * @brief 一帧数据的尾部同步字
 */
#define  ONE_WIRE_BUS_PACKET_TAIL_SYNC_CODE        0xAA


/**
 * @brief 一帧数据的启始标志,XXX项目
 */
#define  ONE_WIRE_BUS_PACKET_XXX_START_CODE       0xF1

/**
 * @brief 重传一帧数据的启始标志，XXX项目
 */
#define  ONE_WIRE_BUS_PACKET_XXX_REPEAT_CODE      0xF2

/**
 * @brief 一帧数据的结束标志
 */
#define  ONE_WIRE_BUS_PACKET_XXX_END_CODE         0xFE

/**
 * @brief 数据包来自键盘
 */
#define  ONE_WIRE_BUS_PACKET_KEYBOARD_ADDR         0xA1

/**
 * @brief 数据包来自PAD
 */
#define  ONE_WIRE_BUS_PACKET_PAD_ADDR              0xA2


//==============================单总线数据包主命令=====================================
typedef enum
{
    ONE_WIRE_BUS_PACKET_GENRRAL_KEY_CMD          = 0x01, //普通按键上传
    ONE_WIRE_BUS_PACKET_MEDIA_KEY_CMD            = 0x02, //多媒体按键上传
    ONE_WIRE_BUS_PACKET_TOUCHPAD_CMD             = 0x03, //TP数据上传
    ONE_WIRE_BUS_PACKET_MOUSE_CMD                = 0x04, //鼠标数据上传
    ONE_WIRE_BUS_PACKET_GSENSOR_CMD              = 0x05, //Gsensor数据上传
    ONE_WIRE_BUS_PACKET_COUSTOM_CMD              = 0x06, //自定义数据上传
    ONE_WIRE_BUS_PACKET_PARAM_SET_CMD            = 0x20, //参数设置
    ONE_WIRE_BUS_PACKET_PARAM_SET_ACK_CMD        = 0x21, //参数设置应答
    ONE_WIRE_BUS_PACKET_STATUS_READ_CMD          = 0x22, //状态读取
    ONE_WIRE_BUS_PACKET_STATUS_READ_ACK_CMD      = 0x23, //状态读取应答
    ONE_WIRE_BUS_PACKET_OTA_CMD                  = 0x24, //OTA升级
    ONE_WIRE_BUS_PACKET_OTA_ACK_CMD              = 0x25, //OTA升级应答
    ONE_WIRE_BUS_PACKET_SOFT_RESET_CMD           = 0x2A, //软件复位
    ONE_WIRE_BUS_PACKET_SOFT_RESET_ACK_CMD       = 0x2B, //软件复位应答
    ONE_WIRE_BUS_PACKET_LOWPOWER_CMD             = 0x2C, //低功耗
    ONE_WIRE_BUS_PACKET_LOWPOWER_ACK_CMD         = 0x2D, //低功耗应答
    ONE_WIRE_BUS_PACKET_SYNC_DOWNLOAD_CMD        = 0x2E, //主机下传同步包
    ONE_WIRE_BUS_PACKET_SYNC_UPLOAD_CMD          = 0x2F, //键盘上传同步包
    ONE_WIRE_BUS_PACKET_SOFT_DEBUG_CMD           = 0x30, //软件调试
    ONE_WIRE_BUS_PACKET_SOFT_DEBUG_ACK_CMD       = 0x31, //软件调试应答
    ONE_WIRE_BUS_PACKET_I2C_WRITE_CMD            = 0x34, //I2C写指令
    ONE_WIRE_BUS_PACKET_I2C_WRITE_ACK_CMD        = 0x35, //I2C写指令应答
    ONE_WIRE_BUS_PACKET_I2C_READ_CMD             = 0x36, //I2C读指令
    ONE_WIRE_BUS_PACKET_I2C_READ_ACK_CMD         = 0x37, //I2C读指令应答
    ONE_WIRE_BUS_PACKET_USER_PASSTHROUGH_CMD     = 0x38, //用户透传指令
    ONE_WIRE_BUS_PACKET_USER_PASSTHROUGH_ACK_CMD = 0x39, //用户透传指令应答
    ONE_WIRE_BUS_PACKET_USER_GENERAL_CMD         = 0x3A, //用户通用指令
    ONE_WIRE_BUS_PACKET_USER_GENERAL_ACK_CMD     = 0x3B, //用户通用指令应答
}em_one_wire_bus_package_main_cmd_t;

#endif

/* [] END OF FILE */

