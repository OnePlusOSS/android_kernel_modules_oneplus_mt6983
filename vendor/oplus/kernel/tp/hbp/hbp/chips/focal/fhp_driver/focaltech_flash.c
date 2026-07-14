/*
 *
 * FocalTech fts TouchScreen driver.
 *
 * Copyright (c) 2012-2020, Focaltech Ltd. All rigfhps reserved.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

/*****************************************************************************
*
* File Name: focaltech_flash.c
*
* Author: Focaltech Driver Team
*
* Created: 2016-08-08
*
* Abstract:
*
* Reference:
*
*****************************************************************************/

/*****************************************************************************
* 1.Included header files
*****************************************************************************/
#include "fhp_core.h"

/*****************************************************************************
* Private constant and macro definitions using #define
*****************************************************************************/

/******************FW upgrade configuration******************/
/*Enable FW auto upgrade function*/
#define FTS_AUTO_UPGRADE_EN                 0
#define FTS_UPGRADE_FW_FILE                 "fw_sample.i"

#define FTS_FW_REQUEST_SUPPORT              0
/* Example: focaltech_ts_fw_tianma.bin */
#define FTS_FW_NAME_REQ                     "focaltech_ts_fw_xxx.bin"

#define FTS_BINOFF_FWVER                    0x010E

#define BYTE_OFF_0(x)                       (u8)((x) & 0xFF)
#define BYTE_OFF_8(x)                       (u8)(((x) >> 8) & 0xFF)
#define BYTE_OFF_16(x)                      (u8)(((x) >> 16) & 0xFF)
#define BYTE_OFF_24(x)                      (u8)(((x) >> 24) & 0xFF)


/*Please don't modify the value of following macroes*/
#define AL2_FCS_COEF                ((1 << 15) + (1 << 10) + (1 << 3))

#define FTS_CMD_RESET                               0x07
#define FTS_CMD_FLASH_MODE                          0x09
#define FLASH_MODE_UPGRADE_VALUE                    0x0B
#define FTS_CMD_ERASE_APP                           0x61
#define FTS_RETRIES_REASE                           50
#define FTS_RETRIES_DELAY_REASE                     400
#define FTS_CMD_FLASH_STATUS                        0x6A
#define FTS_CMD_FLASH_STATUS_LEN                    2
#define FTS_CMD_FLASH_STATUS_NOP                    0x0000
#define FTS_CMD_FLASH_STATUS_ECC_OK                 0xF055
#define FTS_CMD_FLASH_STATUS_ERASE_OK               0xF0AA
#define FTS_CMD_FLASH_STATUS_WRITE_OK               0x1000
#define FTS_CMD_ECC_INIT                            0x64
#define FTS_CMD_ECC_CAL                             0x65
#define FTS_CMD_ECC_CAL_LEN                         7
#define FTS_RETRIES_ECC_CAL                         10
#define FTS_RETRIES_DELAY_ECC_CAL                   50
#define FTS_CMD_ECC_READ                            0x66
#define FTS_CMD_DATA_LEN                            0x7A
#define FTS_CMD_DATA_LEN_LEN                        4
#define FTS_CMD_SET_WFLASH_ADDR                     0xAB
#define FTS_CMD_SET_RFLASH_ADDR                     0xAC
#define FTS_LEN_SET_ADDR                            4
#define FTS_CMD_WRITE                               0xBF
#define FTS_RETRIES_WRITE                           100
#define FTS_RETRIES_DELAY_WRITE                     1
#define FTS_DELAY_READ_ID                           20
#define FTS_DELAY_UPGRADE_RESET                     80
#define FTS_FLASH_PACKET_LENGTH                     32     /* max=128 */
#define FTS_MIN_LEN                                 0x120
#define FTS_MAX_LEN_FILE                            (256 * 1024)

#define FTS_REG_UPGRADE                             0xFC
#define FTS_UPGRADE_AA                              0xAA
#define FTS_UPGRADE_55                              0x55
#define FTS_DELAY_UPGRADE_AA                        10
#define FTS_DELAY_UPGRADE_55                        10
#define FTS_UPGRADE_LOOP                            10

#define FTS_CMD_START1                              0x55
#define FTS_CMD_START_DELAY                         12
#define FTS_CMD_READ_ID                             0x90
#define FTS_ROMBOOT_CMD_SET_PRAM_ADDR               0xAD
#define FTS_ROMBOOT_CMD_WRITE                       0xAE
#define FTS_ROMBOOT_CMD_ECC                         0xCC
#define FTS_ROMBOOT_CMD_ECC_READ                    0xCD
#define FTS_ROMBOOT_CMD_START_APP                   0x08

#define FTS_ROMBOOT_CMD_SET_PRAM_ADDR_LEN           4
#define FTS_CMD_WRITE_LEN                           6
#define PRAMBOOT_MIN_SIZE                           0x120
#define PRAMBOOT_MAX_SIZE                           (64*1024)
#define FTS_ROMBOOT_CMD_ECC_NEW_LEN                 7
#define FTS_DELAY_PRAMBOOT_START                    100

/*****************************************************************************
* Private enumerations, structures and unions using typedef
*****************************************************************************/
enum FW_STATUS {
	FTS_RUN_IN_ERROR,
	FTS_RUN_IN_APP,
	FTS_RUN_IN_ROM,
	FTS_RUN_IN_PRAM,
	FTS_RUN_IN_BOOTLOADER,
};

/*****************************************************************************
* Global variable or extern global variabls/functions
*****************************************************************************/
u8 fw_file[] = {
#include FTS_UPGRADE_FW_FILE
};

u8 pb_file_ft5662[] = {
#include "FT5662_Pramboot_V1.5_20221031.i"
};
/************************************************************************
 * Name: fts_fw_check_valid
 * Brief: The function is used to check whether there is a valid firmware
 *   running in touch chip.
 *
 *   After reset, touch chip shall be in initialization phase, firmware
 *   shall be run after initialization if there is a valid firmware,
 *   otherwise touch chip shall stop in BOOT mode(upgrade mode).
 *
 * Input:
 * Output:
 *
 * Return: return 1 if there is a valid firmware, otherwise return 0.
 ***********************************************************************/
static bool fts_fw_check_valid(void)
{
	int ret = 0;
	int i = 0;
	u8 val = 0;

	for (i = 0; i < 50; i++) {
		ret = fts_read_reg(FTS_REG_CHIP_ID, &val);
		if (FTS_CHIP_ID == val) {
			return 1;
		} else {
			FTS_DEBUG("read chip id[0x%x] fails,ret:%d,times:%d", val, ret, i);
		}
		msleep(10);
	}

	return 0;
}


/************************************************************************
 * Name: fts_check_flash_status
 * Brief: read status from tp
 * Input: flash_status: correct value from tp
 *        retries: read retry times
 *        retries_delay: retry delay
 * Output:
 * Return: return true if flash status check pass, otherwise return false
***********************************************************************/
static bool fts_check_flash_status(
	u16 flash_status, int retries, int retries_delay)
{
	int ret = 0;
	int i = 0;
	u8 cmd = FTS_CMD_FLASH_STATUS;
	u8 val[FTS_CMD_FLASH_STATUS_LEN] = { 0 };
	u16 read_status = 0;

	for (i = 0; i < retries; i++) {
		ret = fts_read(&cmd, 1, val, FTS_CMD_FLASH_STATUS_LEN);
		read_status = (((u16)val[0]) << 8) + val[1];
		if (flash_status == read_status) {
			/* FTS_DEBUG("[UPGRADE]flash status ok"); */
			return true;
		}
		/* FTS_DEBUG("flash status fail,ok:%04x read:%04x, retries:%d", flash_status, read_status, i); */
		msleep(retries_delay);
	}

	return false;
}

/************************************************************************
* Name: fts_reset_in_boot
* Brief: RST CMD(07), reset in boot environment
* Input:
* Output:
* Return: return 0 if success, otherwise return error code
***********************************************************************/
static int fts_reset_in_boot(void)
{
	int ret = 0;
	u8 cmd = FTS_CMD_RESET;

	FTS_INFO("reset in boot environment");
	ret = fts_write(&cmd, 1);
	if (ret < 0) {
		FTS_ERROR("reset cmd(07) write fail");
		return ret;
	}

	return 0;
}

static int ft5662_fwupg_get_boot_state(enum FW_STATUS *fw_sts)
{
	int ret = 0;
	u8 cmd = 0;
	u8 val[2] = { 0 };

	FTS_INFO("**********read boot id**********");
	if (!fw_sts) {
		FTS_ERROR("fw_sts is null");
		return -EINVAL;
	}

	cmd = FTS_CMD_START1;
	ret = fts_write(&cmd, 1);
	if (ret < 0) {
		FTS_ERROR("write 55 cmd fail");
		return ret;
	}

	msleep(FTS_CMD_START_DELAY);
	cmd = FTS_CMD_READ_ID;
	ret = fts_read(&cmd, 1, val, 2);
	if (ret < 0) {
		FTS_ERROR("write 90 cmd fail");
		return ret;
	}

	FTS_INFO("read boot id:0x%02x%02x", val[0], val[1]);
	if ((val[0] == 0x56) && (val[1] == 0x62)) {
		FTS_INFO("tp run in romboot");
		*fw_sts = FTS_RUN_IN_ROM;
	} else if ((val[0] == 0x56) && (val[1] == 0xE2)) {
		FTS_INFO("tp run in pramboot");
		*fw_sts = FTS_RUN_IN_PRAM;
	}

	return 0;
}


static int ft5662_fwupg_reset_to_romboot(void)
{
	int ret = 0;
	int i = 0;
	u8 cmd = FTS_CMD_RESET;
	enum FW_STATUS state = FTS_RUN_IN_ERROR;

	ret = fts_write(&cmd, 1);
	if (ret < 0) {
		FTS_ERROR("pram/rom/bootloader reset cmd write fail");
		return ret;
	}
	mdelay(10);

	for (i = 0; i < FTS_UPGRADE_LOOP; i++) {
		ret = ft5662_fwupg_get_boot_state(&state);
		if (FTS_RUN_IN_ROM == state) {
			break;
		}
		mdelay(5);
	}
	if (i >= FTS_UPGRADE_LOOP) {
		FTS_ERROR("reset to romboot fail");
		return -EIO;
	}

	return 0;
}

static int ft5662_pram_write_buf(u8 *buf, u32 len)
{
	int ret = 0;
	u32 i = 0;
	u32 j = 0;
	u32 offset = 0;
	u32 remainder = 0;
	u32 packet_number;
	u32 packet_len = 0;
	u8 packet_buf[FTS_FLASH_PACKET_LENGTH + FTS_CMD_WRITE_LEN] = { 0 };
	u32 cmdlen = 0;

	FTS_INFO("write pramboot to pram,pramboot len=%d", len);
	if (!buf || (len < PRAMBOOT_MIN_SIZE) || (len > PRAMBOOT_MAX_SIZE)) {
		FTS_ERROR("buf/pramboot length(%d) fail", len);
		return -EINVAL;
	}

	packet_number = len / FTS_FLASH_PACKET_LENGTH;
	remainder = len % FTS_FLASH_PACKET_LENGTH;
	if (remainder > 0) {
		packet_number++;
	}
	packet_len = FTS_FLASH_PACKET_LENGTH;

	for (i = 0; i < packet_number; i++) {
		offset = i * FTS_FLASH_PACKET_LENGTH;
		/* last packet */
		if ((i == (packet_number - 1)) && remainder) {
			packet_len = remainder;
		}


		packet_buf[0] = FTS_ROMBOOT_CMD_SET_PRAM_ADDR;
		packet_buf[1] = BYTE_OFF_16(offset);
		packet_buf[2] = BYTE_OFF_8(offset);
		packet_buf[3] = BYTE_OFF_0(offset);

		ret = fts_write(packet_buf, FTS_ROMBOOT_CMD_SET_PRAM_ADDR_LEN);
		if (ret < 0) {
			FTS_ERROR("pramboot set write address(%d) fail", i);
			return ret;
		}

		packet_buf[0] = FTS_ROMBOOT_CMD_WRITE;
		cmdlen = 1;


		for (j = 0; j < packet_len; j++) {
			packet_buf[cmdlen + j] = buf[offset + j];
		}

		ret = fts_write(packet_buf, packet_len + cmdlen);
		if (ret < 0) {
			FTS_ERROR("pramboot write data(%d) fail", i);
			return ret;
		}
	}

	return 0;
}

static void ft5662_crc16_calc_host(u8 *pbuf, u32 length, u16 *ecc)
{
	u32 i = 0;
	u32 j = 0;
	u16 tmp_ecc = 0;

	for (i = 0; i < length; i += 2) {
		tmp_ecc ^= ((pbuf[i] << 8) | (pbuf[i + 1]));
		for (j = 0; j < 16; j ++) {
			if (tmp_ecc & 0x01) {
				tmp_ecc = (u16)((tmp_ecc >> 1) ^ AL2_FCS_COEF);
			} else {
				tmp_ecc >>= 1;
			}
		}
	}

	*ecc = tmp_ecc;
}


static int ft5662_pram_ecc_cal(u32 start_addr, u32 ecc_length, u16 *ecc)
{
	int ret = 0;
	u8 val[2] = { 0 };
	u8 cmd[FTS_ROMBOOT_CMD_ECC_NEW_LEN] = { 0 };

	FTS_INFO("read out pramboot checksum");
	cmd[0] = FTS_ROMBOOT_CMD_ECC;
	cmd[1] = BYTE_OFF_16(start_addr);
	cmd[2] = BYTE_OFF_8(start_addr);
	cmd[3] = BYTE_OFF_0(start_addr);
	cmd[4] = BYTE_OFF_16(ecc_length);
	cmd[5] = BYTE_OFF_8(ecc_length);
	cmd[6] = BYTE_OFF_0(ecc_length);
	ret = fts_write(cmd, FTS_ROMBOOT_CMD_ECC_NEW_LEN);
	if (ret < 0) {
		FTS_ERROR("write pramboot ecc cal cmd fail");
		return ret;
	}

	msleep(10);
	cmd[0] = FTS_ROMBOOT_CMD_ECC_READ;
	ret = fts_read(cmd, 1, val, 2);
	if (ret < 0) {
		FTS_ERROR("read pramboot ecc fail");
		return ret;
	}

	*ecc = ((u16)(val[0] << 8) + val[1]);
	return 0;
}


static int ft5662_pram_start(void)
{
	u8 cmd = FTS_ROMBOOT_CMD_START_APP;
	int ret = 0;

	FTS_INFO("remap to start pramboot");

	ret = fts_write(&cmd, 1);
	if (ret < 0) {
		FTS_ERROR("write start pram cmd fail");
		return ret;
	}
	msleep(FTS_DELAY_PRAMBOOT_START);

	return 0;
}

static bool ft5662_fwupg_check_state(enum FW_STATUS rstate)
{
	int ret = 0;
	int i = 0;
	enum FW_STATUS cstate = FTS_RUN_IN_ERROR;

	for (i = 0; i < FTS_UPGRADE_LOOP; i++) {
		ret = ft5662_fwupg_get_boot_state(&cstate);
		/* FTS_DEBUG("fw state=%d, retries=%d", cstate, i); */
		if (cstate == rstate) {
			return true;
		}
		msleep(FTS_DELAY_READ_ID);
	}

	return false;
}


static int fts_ft5662_write_pramboot_private(void)
{
	int ret = 0;
	bool state = 0;
	enum FW_STATUS status = FTS_RUN_IN_ERROR;
	u16 ecc_in_host = 0;
	u16 ecc_in_tp = 0;
	u8 *pb_buf = pb_file_ft5662;
	u32 pb_len = sizeof(pb_file_ft5662);

	FTS_INFO("**********pram write and init**********");
	if (pb_len < FTS_MIN_LEN) {
		FTS_ERROR("pramboot length(%d) fail", pb_len);
		return -EINVAL;
	}

	FTS_DEBUG("check whether tp is in romboot or not ");
	/* need reset to romboot when non-romboot state */
	ret = ft5662_fwupg_get_boot_state(&status);
	if (status != FTS_RUN_IN_ROM) {
		FTS_INFO("tp isn't in romboot, need send reset to romboot");
		ret = ft5662_fwupg_reset_to_romboot();
		if (ret < 0) {
			FTS_ERROR("reset to romboot fail");
			return ret;
		}
	}

	/* write pramboot to pram */
	ret = ft5662_pram_write_buf(pb_buf, pb_len);
	if (ret < 0) {
		FTS_ERROR("write pramboot buffer fail");
		return ret;
	}

	/* check CRC */
	ft5662_crc16_calc_host(pb_buf, pb_len, &ecc_in_host);
	ret = ft5662_pram_ecc_cal(0, pb_len, &ecc_in_tp);
	if (ret < 0) {
		FTS_ERROR("read pramboot ecc fail");
		return ret;
	}

	FTS_INFO("pram ecc in tp:%x, host:%x", ecc_in_tp, ecc_in_host);
	/*  pramboot checksum != fw checksum, upgrade fail */
	if (ecc_in_host != ecc_in_tp) {
		FTS_ERROR("pramboot ecc check fail");
		return -EIO;
	}

	/*start pram*/
	ret = ft5662_pram_start();
	if (ret < 0) {
		FTS_ERROR("pram start fail");
		return ret;
	}

	FTS_DEBUG("after write pramboot, confirm run in pramboot");
	state = ft5662_fwupg_check_state(FTS_RUN_IN_PRAM);
	if (!state) {
		FTS_ERROR("not in pramboot");
		return -EIO;
	}

	return 0;
}

/************************************************************************
* Name: fts_fwupg_enter_into_boot
* Brief: enter into boot mode, be ready for upgrade
* Input:
* Output:
* Return: return 0 if success, otherwise return error code
***********************************************************************/
static int fts_enter_into_boot(void)
{
	int ret = 0;
	int i = 0;
	bool fwvalid = false;
	u8 cmd = 0;
	u8 val[2] = { 0 };

	FTS_INFO("enter into pramboot/bootloader");
	fwvalid = fts_fw_check_valid();
	if (fwvalid) {
		/*Software reset to boot mode while TP FW is valid*/
		ret = fts_write_reg(FTS_REG_UPGRADE, FTS_UPGRADE_AA);
		if (ret < 0) {
			FTS_ERROR("write FC=0xAA fail");
			return ret;
		}
		msleep(FTS_DELAY_UPGRADE_AA);

		ret = fts_write_reg(FTS_REG_UPGRADE, FTS_UPGRADE_55);
		if (ret < 0) {
			FTS_ERROR("write FC=0x55 fail");
			return ret;
		}
		msleep(FTS_DELAY_UPGRADE_55);
	}

	ret = fts_ft5662_write_pramboot_private();

	return ret;
}

/************************************************************************
 * Name: fts_fwupg_erase
 * Brief: erase flash area
 * Input: delay - delay after erase
 * Output:
 * Return: return 0 if success, otherwise return error code
 ***********************************************************************/
int fts_flash_erase(u32 delay)
{
	int ret = 0;
	u8 cmd = 0;
	bool flag = false;

	FTS_INFO("**********erase now**********");

	/*send to erase flash*/
	cmd = FTS_CMD_ERASE_APP;
	ret = fts_write(&cmd, 1);
	if (ret < 0) {
		FTS_ERROR("erase cmd fail");
		return ret;
	}
	msleep(delay);

	/* read status 0xF0AA: success */
	flag = fts_check_flash_status(FTS_CMD_FLASH_STATUS_ERASE_OK,
				      FTS_RETRIES_REASE,
				      FTS_RETRIES_DELAY_REASE);
	if (!flag) {
		FTS_ERROR("ecc flash status check fail");
		return -EIO;
	}

	return 0;
}

/************************************************************************
 * Name: fts_flash_write_buf
 * Brief: write buf data to flash address
 * Input: saddr - start address data write to flash
 *        buf - data buffer
 *        len - data length
 *        delay - delay after write
 * Output:
 * Return: return 0 if success, otherwise return error code
 ***********************************************************************/
int fts_flash_write_buf(u32 saddr, u8 *buf, u32 len, u32 delay)
{
	int ret = 0;
	u32 i = 0;
	u32 j = 0;
	u32 packet_number = 0;
	u32 packet_len = 0;
	u32 addr = 0;
	u32 offset = 0;
	u32 remainder = 0;
	u8 packet_buf[FTS_FLASH_PACKET_LENGTH + 1] = { 0 };
	u8 cmd = 0;
	u8 val[FTS_CMD_FLASH_STATUS_LEN] = { 0 };
	u16 read_status = 0;
	u16 wr_ok = 0;

	FTS_INFO("**********write data to flash**********");
	if (!buf || !len) {
		FTS_ERROR("buf/len is invalid");
		return -EINVAL;
	}

	FTS_INFO("data buf start addr=0x%x, len=0x%x", saddr, len);
	packet_number = len / FTS_FLASH_PACKET_LENGTH;
	remainder = len % FTS_FLASH_PACKET_LENGTH;
	if (remainder > 0) {
		packet_number++;
	}
	packet_len = FTS_FLASH_PACKET_LENGTH;
	FTS_INFO("write data, num:%d remainder:%d", packet_number, remainder);

	for (i = 0; i < packet_number; i++) {
		offset = i * FTS_FLASH_PACKET_LENGTH;
		addr = saddr + offset;

		/* last packet */
		if ((i == (packet_number - 1)) && remainder) {
			packet_len = remainder;
		}

		packet_buf[0] = FTS_CMD_SET_WFLASH_ADDR;
		packet_buf[1] = (addr >> 16);
		packet_buf[2] = (addr >> 8);
		packet_buf[3] = (addr);
		ret = fts_write(packet_buf, FTS_LEN_SET_ADDR);
		if (ret < 0) {
			FTS_ERROR("set flash address fail");
			return ret;
		}

		packet_buf[0] = FTS_CMD_WRITE;
		for (j = 0; j < packet_len; j++) {
			packet_buf[1 + j] = buf[offset + j];
		}

		ret = fts_write(packet_buf, packet_len + 1);
		if (ret < 0) {
			FTS_ERROR("app write fail");
			return ret;
		}
		mdelay(delay);

		/* read status */
		wr_ok = FTS_CMD_FLASH_STATUS_WRITE_OK + addr / packet_len;
		for (j = 0; j < FTS_RETRIES_WRITE; j++) {
			cmd = FTS_CMD_FLASH_STATUS;
			ret = fts_read(&cmd, 1, val, FTS_CMD_FLASH_STATUS_LEN);
			read_status = (((u16)val[0]) << 8) + val[1];
			/*  FTS_INFO("%x %x", wr_ok, read_status); */
			if (wr_ok == read_status) {
				break;
			}
			mdelay(FTS_RETRIES_DELAY_WRITE);
		}
	}

	return 0;
}

static u16 fts_host_ecc_calc_crc16(u8 *buf, u32 len)
{
	u16 ecc = 0;
	u32 i = 0;
	u32 j = 0;

	for (i = 0; i < len; i += 2) {
		ecc ^= ((buf[i] << 8) | (buf[i + 1]));
		for (j = 0; j < 16; j ++) {
			if (ecc & 0x01) {
				ecc = (u16)((ecc >> 1) ^ AL2_FCS_COEF);
			} else {
				ecc >>= 1;
			}
		}
	}

	return ecc;
}

/************************************************************************
 * Name: fts_fwupg_ecc_cal
 * Brief: calculate and get ecc from tp
 * Input: saddr - start address need calculate ecc
 *        len - length need calculate ecc
 * Output:
 * Return: return data ecc of tp if success, otherwise return error code
 ***********************************************************************/
static int fts_tp_ecc_read(u32 saddr, u32 len, u16 *ecc_tp)
{
	int ret = 0;
	u8 wbuf[FTS_CMD_ECC_CAL_LEN] = { 0 };
	u8 val[FTS_CMD_FLASH_STATUS_LEN] = { 0 };
	bool bflag = false;

	FTS_INFO("**********read out checksum**********");
	/* ecc init */
	wbuf[0] = FTS_CMD_ECC_INIT;
	ret = fts_write(wbuf, 1);
	if (ret < 0) {
		FTS_ERROR("ecc init cmd write fail");
		return ret;
	}

	FTS_INFO("saddr:%d, len:%d", saddr, len);
	/* send commond to start checksum */
	wbuf[0] = FTS_CMD_ECC_CAL;
	wbuf[1] = (saddr >> 16);
	wbuf[2] = (saddr >> 8);
	wbuf[3] = (saddr);
	wbuf[4] = (len >> 16);
	wbuf[5] = (len >> 8);
	wbuf[6] = (len);
	ret = fts_write(wbuf, FTS_CMD_ECC_CAL_LEN);
	if (ret < 0) {
		FTS_ERROR("ecc calc cmd write fail");
		return ret;
	}

	msleep(len / 256);
	/* read status if check sum is finished */
	bflag = fts_check_flash_status(FTS_CMD_FLASH_STATUS_ECC_OK,
				       FTS_RETRIES_ECC_CAL,
				       FTS_RETRIES_DELAY_ECC_CAL);
	if (!bflag) {
		FTS_ERROR("ecc flash status read fail");
		return -EIO;
	}

	/* read out check sum */
	wbuf[0] = FTS_CMD_ECC_READ;
	ret = fts_read(wbuf, 1, val, 2);
	if (ret < 0) {
		FTS_ERROR("ecc read cmd write fail");
		return ret;
	}

	*ecc_tp = ((val[0] << 8) + val[1]);
	return 0;
}

static int fts_ft5662_upgrade(u8 *buf, u32 len)
{
	int ret = 0;
	u32 start_addr = 0;
	u8 cmd[4] = { 0 };
	u32 delay = 0;
	u16 ecc_in_host = 0;
	u16 ecc_in_tp = 0;

	if ((NULL == buf) || (len < FTS_MIN_LEN)) {
		FTS_ERROR("buffer/len(%x) is invalid", len);
		return -EINVAL;
	}

	/*step 1:software reset to boot mode*/
	ret = fts_enter_into_boot();
	if (ret < 0) {
		FTS_ERROR("enter into pramboot/bootloader fail,ret=%d", ret);
		goto fw_reset;
	}

	/*step 2:initialize upgrade mode*/
	cmd[0] = FTS_CMD_DATA_LEN;
	cmd[1] = (len >> 16);
	cmd[2] = (len >> 8);
	cmd[3] = (len);
	ret = fts_write(cmd, FTS_CMD_DATA_LEN_LEN);
	if (ret < 0) {
		FTS_ERROR("data len cmd write fail");
		goto fw_reset;
	}

	cmd[0] = FTS_CMD_FLASH_MODE;
	cmd[1] = FLASH_MODE_UPGRADE_VALUE;
	ret = fts_write(cmd, 2);
	if (ret < 0) {
		FTS_ERROR("upgrade mode(09) cmd write fail");
		goto fw_reset;
	}

	/*step 3:Erase flash*/
	delay = 80 * (len / 2048);
	ret = fts_flash_erase(delay);
	if (ret < 0) {
		FTS_ERROR("erase cmd write fail");
		goto fw_reset;
	}

	/*step 4:write firmware to touch chip*/
	ret = fts_flash_write_buf(start_addr, buf, len, 1);
	if (ret < 0) {
		FTS_ERROR("flash write fail");
		goto fw_reset;
	}

	/*step 5: ECC verification*/
	FTS_INFO("ECC check");
	ecc_in_host = fts_host_ecc_calc_crc16(buf, len);
	ret = fts_tp_ecc_read(start_addr, len, &ecc_in_tp);
	if (ret < 0) {
		FTS_ERROR("read ecc from tp fails");
		goto fw_reset;
	}

	FTS_INFO("check ecc: TP[0x%x],HOST[0x%x]", ecc_in_tp, ecc_in_host);
	if (ecc_in_tp != ecc_in_host) {
		FTS_ERROR("ecc check fail");
		ret = -EIO;
		goto fw_reset;
	}

	FTS_INFO("upgrade success, reset to normal boot");
	ret = 0;

fw_reset:
	fts_reset_in_boot();
	msleep(200);
	return ret;
}

static bool fts_fw_need_upgrade(u8 *buf, u32 len)
{
	int ret = 0;
	bool fwvalid = false;
	u8 fw_ver_in_host = 0;
	u8 fw_ver_in_tp = 0;

	if (len < FTS_BINOFF_FWVER) {
		FTS_INFO("fw len(%d) is invalid, no need upgrade fw", len);
		return false;
	}

	fwvalid = fts_fw_check_valid();
	if (fwvalid) {
		fw_ver_in_host = buf[FTS_BINOFF_FWVER];
		ret = fts_read_reg(FTS_REG_FW_VER, &fw_ver_in_tp);
		if (ret < 0) {
			FTS_ERROR("read fw ver from tp fail");
			return false;
		}

		FTS_INFO("fw version in tp:%x, host:%x", fw_ver_in_tp, fw_ver_in_host);
		if (fw_ver_in_tp != fw_ver_in_host) {
			return true;
		}
	} else {
		FTS_INFO("fw invalid, need upgrade fw");
		return true;
	}

	return false;
}

/************************************************************************
 * Name: fts_fw_auto_upgrade
 * Brief: The entry of firmware auto upgrade while system powers on.
 *
 * Input:
 * Output:
 * Return: return 0 if success, otherwise return error code
 ***********************************************************************/
static int fts_fw_auto_upgrade(u8 *fw, u32 fwlen)
{
	int ret = 0;
	int upgrade_count = 0;
	u8 ver = 0;
	bool upgrade_flag = false;

	if (!fw || (fwlen < FTS_MIN_LEN)) {
		FTS_ERROR("FW/len(%x) is invalid", fwlen);
		return -EINVAL;
	}

	/*check if FW need upgrade or not*/
	upgrade_flag = fts_fw_need_upgrade(fw, fwlen);
	FTS_INFO("FW upgrade flag:%d", upgrade_flag);

	/*run upgrade procedures*/
	if (upgrade_flag) {
		do {
			upgrade_count++;
			FTS_INFO("upgrade FW(times:%d)", upgrade_count);
			ret = fts_ft5662_upgrade(fw, fwlen);
			if (ret == 0) {
				fts_read_reg(FTS_REG_FW_VER, &ver);
				FTS_INFO("success upgrade to fw version %02x", ver);
				break;
			}
		} while (upgrade_count < 2);
	}

	return ret;
}

static void fts_fwupg_work(struct work_struct *work)
{
	int ret = 0;
	const struct firmware *request_fw = NULL;
	u8 *fw = NULL;
	u32 fwlen = 0;
	bool get_fw_i_flag = false;
	struct fts_ts_data *ts_data = container_of(work,
				      struct fts_ts_data, fwupg_work);

	FTS_INFO("fw upgrade work function");
	if (!ts_data) {
		FTS_ERROR("ts_data is null");
		return ;
	}

	/* get fw file */
	FTS_DEBUG("get upgrade fw file");
	if (FTS_FW_REQUEST_SUPPORT) {
		ret = request_firmware(&request_fw, FTS_FW_NAME_REQ,
				       &ts_data->fhp_data->spi->dev);
		if (0 == ret) {
			FTS_INFO("firmware(%s) request successfully", FTS_FW_NAME_REQ);
			fw = (u8 *)request_fw->data;
			fwlen = (u32)request_fw->size;
		} else {
			get_fw_i_flag = true;
			FTS_INFO("firmware(%s) request fail,ret=%d", FTS_FW_NAME_REQ, ret);
		}
	} else {
		get_fw_i_flag = true;
	}

	if (get_fw_i_flag) {
		fw = fw_file;
		fwlen = sizeof(fw_file);
	}

	/* run auto upgrade */
	ret = fts_fw_auto_upgrade(fw, fwlen);
	if (ret) {
		FTS_ERROR("FW auto upgrade fails");
	}

	/*upgrade fw exit*/
	if (request_fw != NULL) {
		release_firmware(request_fw);
		request_fw = NULL;
	}

	ts_data->chip_init_done = 1;
}

int fts_fwupg_init(struct fts_ts_data *ts_data)
{
	FTS_INFO("FW upgrade init function");

#if !FTS_AUTO_UPGRADE_EN
	FTS_INFO("FTS_AUTO_UPGRADE_EN is disabled, not upgrade when power on");
	ts_data->chip_init_done = 1;
	return 0;
#endif

	if (!ts_data || !ts_data->ts_workqueue) {
		FTS_ERROR("ts_data/workqueue is NULL, can't run upgrade function");
		return -EINVAL;
	}

	INIT_WORK(&ts_data->fwupg_work, fts_fwupg_work);
	queue_work(ts_data->ts_workqueue, &ts_data->fwupg_work);

	return 0;
}



