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

/******************FW upgrade configuration******************/
/*Enable FW auto upgrade function*/
#define FTS_AUTO_UPGRADE_EN                 0
#define FTS_UPGRADE_FW_FILE                 "fw_sample.i"

#define FTS_FW_REQUEST_SUPPORT              0
/* Example: focaltech_ts_fw_tianma.bin */
#define FTS_FW_NAME_REQ                     "focaltech_ts_fw_xxx.bin"

#define FTS_BINOFF_FWVER                    0x010E

#define FTS_CMD_SET_DPRAM_ADDR                      0xAD
#define FTS_CMD_WRITE_DPRAM                         0xAE
#define FTS_CMD_ECC_CALC                            0xCC
#define FTS_CMD_ECC_FINISH                          0xCE
#define FTS_CMD_ECC_READ                            0xCD
#define FTS_CMD_FLASH_MODE                          0x09
#define FLASH_MODE_WRITE_FLASH_VALUE                0x0A
#define FLASH_MODE_UPGRADE_VALUE                    0x0B
#define FTS_CMD_APP_DATA_LEN_INCELL                 0x7A
#define FTS_CMD_SET_WFLASH_ADDR                     0xAB
#define FTS_CMD_SET_RFLASH_ADDR                     0xAC
#define FTS_CMD_WRITE                               0xBF
#define FTS_REG_UPGRADE                             0xFC
#define FTS_UPGRADE_AA                              0xAA
#define FTS_UPGRADE_55                              0x55

#define FTS_CMD_START1                              0x55
#define FTS_CMD_START2                              0xAA
#define FTS_CMD_START_DELAY                         12
#define FTS_CMD_READ_ID                             0x90
#define FTS_CMD_FLASH_STATUS                        0x6A
#define FTS_CMD_FLASH_STATUS_LEN                    2
#define FTS_CMD_FLASH_STATUS_NOP                    0x0000
#define FTS_CMD_FLASH_STATUS_ECC_OK                 0xF055
#define FTS_CMD_FLASH_STATUS_ERASE_OK               0xF0AA
#define FTS_CMD_ERASE_APP                           0x61
#define FTS_RETRIES_REASE                           50
#define FTS_RETRIES_DELAY_REASE                     400
#define FTS_CMD_ECC_INIT                            0x64
#define FTS_CMD_ECC_CAL                             0x65
#define FTS_RETRIES_ECC_CAL                         10
#define FTS_RETRIES_DELAY_ECC_CAL                   50
#define FTS_CMD_FLASH_ECC_READ                      0x66
#define FTS_CMD_RESET                               0x07
#define FTS_PRAM_SADDR                              0x000000
#define FTS_DRAM_SADDR                              0xD00000

#define FTS_MIN_LEN                                 0x120

#define AL2_FCS_COEF                        ((1 << 15) + (1 << 10) + (1 << 3))

#define FTS_FW_DOWNLOAD_RETRY_TIMES                 3

/*****************************************************************************
* Global variable or extern global variabls/functions
*****************************************************************************/
u8 fw_file[] = {
#include FTS_UPGRADE_FW_FILE
};


#define FTS_ID_H_PRAMBOOT       0x36
#define FTS_ID_L_PRAMBOOT       0xB3


#define FTS_FLASH_PACKET_LENGTH_SPI                 (4 * 1024 - 16)

#define FTS_FLASH_PACKET_LENGTH                     (256)



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
			FHP_DEBUG("read chip id[0x%x] fails,ret:%d,times:%d", val, ret, i);
		}
		msleep(10);
	}

	return 0;
}

static int fts_enter_into_boot(void)
{
	fts_write_reg(FTS_REG_UPGRADE, FTS_UPGRADE_AA);
	fts_write_reg(FTS_REG_UPGRADE, FTS_UPGRADE_55);
	msleep(80);

	return 0;
}

static int fts_check_id(u8 id_h, u8 id_l)
{
	u8 cmd = FTS_CMD_READ_ID;
	u8 buf[2] = { 0 };

	/*confirm in boot*/
	fts_write_command(FTS_CMD_START1);
	msleep(10);
	fts_read(&cmd, 1, buf, 2);
	if ((buf[0] != id_h) || (buf[1] != id_l)) {
		FHP_ERROR("check id fail,read id:0x%02x%02x != 0x%02x%02x",
			  id_h, id_l, buf[0], buf[1]);
		return -EINVAL;
	}

	return 0;
}

static int fts_ecc_cal_host(const u8 *data, u32 data_len, u16 *ecc_value)
{
	u16 ecc = 0;
	u32 i = 0;
	u32 j = 0;
	u16 al2_fcs_coef = AL2_FCS_COEF;

	for (i = 0; i < data_len; i += 2) {
		ecc ^= ((data[i] << 8) | (data[i + 1]));
		for (j = 0; j < 16; j ++) {
			if (ecc & 0x01) {
				ecc = (u16)((ecc >> 1) ^ al2_fcs_coef);
			} else {
				ecc >>= 1;
			}
		}
	}

	*ecc_value = ecc & 0x0000FFFF;
	return 0;
}

static bool fts_fwupg_check_status(u16 flash_status, int retries, int retries_delay)
{
	int i = 0;
	u8 cmd = 0;
	u8 val[2] = { 0 };
	u16 read_status = 0;

	for (i = 0; i < retries; i++) {
		cmd = FTS_CMD_FLASH_STATUS;
		fts_read(&cmd, 1, val, 2);
		read_status = (((u16)val[0]) << 8) + val[1];
		if (flash_status == read_status) {
			FHP_DEBUG("[UPGRADE]flash status ok");
			return true;
		}
		FHP_DEBUG("flash status fail,ok:%04x read:%04x, retries:%d", flash_status, read_status, i);
		msleep(retries_delay);
	}

	return false;
}

static int fts_fwupg_erase(u32 delay)
{
	int ret = 0;
	bool flag = false;

	FHP_INFO("erase.");
	/*send to erase flash*/
	ret = fts_write_command(FTS_CMD_ERASE_APP);
	if (ret < 0) {
		FHP_ERROR("erase cmd fail");
		return ret;
	}
	msleep(delay);

	/* read status 0xF0AA: success */
	flag = fts_fwupg_check_status(FTS_CMD_FLASH_STATUS_ERASE_OK,
				      FTS_RETRIES_REASE,
				      FTS_RETRIES_DELAY_REASE);
	if (!flag) {
		FHP_ERROR("ecc flash status check fail");
		return -EIO;
	}

	return 0;
}

static int fts_fwupg_ecc_host(u8 *buf, u32 len)
{
	u16 ecc = 0;

	FHP_DEBUG("ecc_host");
	fts_ecc_cal_host(buf, len, &ecc);
	return (int)ecc;
}

static int fts_fwupg_ecc_tp(u32 saddr, u32 len)
{
	int ret = 0;
	u8 wbuf[7] = { 0 };
	u8 val[2] = { 0 };
	bool bflag = false;

	FHP_DEBUG("ecc_tp");
	/* check sum init */
	ret = fts_write_command(FTS_CMD_ECC_INIT);
	if (ret < 0) {
		FHP_ERROR("ecc init cmd write fail");
		return ret;
	}

	/* send commond to start checksum */
	wbuf[0] = FTS_CMD_ECC_CAL;
	wbuf[1] = (saddr >> 16);
	wbuf[2] = (saddr >> 8);
	wbuf[3] = (saddr);
	wbuf[4] = (len >> 16);
	wbuf[5] = (len >> 8);
	wbuf[6] = (len);

	ret = fts_write(wbuf, sizeof(wbuf));
	if (ret < 0) {
		FHP_ERROR("ecc calc cmd write fail");
		return ret;
	}

	msleep(len / 256);

	/* read status if check sum is finished */
	bflag = fts_fwupg_check_status(FTS_CMD_FLASH_STATUS_ECC_OK,
				       FTS_RETRIES_ECC_CAL,
				       FTS_RETRIES_DELAY_ECC_CAL);
	if (!bflag) {
		FHP_ERROR("ecc flash status read fail");
		return -EIO;
	}

	/* read out check sum */
	wbuf[0] = FTS_CMD_FLASH_ECC_READ;
	ret = fts_read(wbuf, 1, val, 2);
	if (ret < 0) {
		FHP_ERROR("ecc read cmd write fail");
		return ret;
	}

	return (int)((u16)(val[0] << 8) + val[1]);
}

static int fts_flash_write_buf(u32 saddr, u8 *buf, u32 len)
{
	int ret = 0;
	u32 i = 0;
	u32 j = 0;
	u32 packet_number = 0;
	u32 packet_len = 0;
	u32 addr = 0;
	u32 offset = 0;
	u32 remainder = 0;
	u8 packet_buf[6] = { 0 };
	u8 cmd = 0;
	u8 val[2] = { 0 };
	u16 read_status = 0;
	u16 wr_ok = 0;

	if (!buf || !len) {
		FHP_ERROR("buf/len is invalid");
		return -EINVAL;
	}

	FHP_INFO("data buf start addr=0x%x, len=0x%x", saddr, len);
	packet_number = len / FTS_FLASH_PACKET_LENGTH;
	remainder = len % FTS_FLASH_PACKET_LENGTH;
	if (remainder > 0) {
		packet_number++;
	}
	packet_len = FTS_FLASH_PACKET_LENGTH;
	FHP_INFO("write data, num:%d remainder:%d", packet_number, remainder);


	for (i = 0; i < packet_number; i++) {
		offset = i * FTS_FLASH_PACKET_LENGTH;
		addr = saddr + offset;
		packet_buf[0] = FTS_CMD_SET_WFLASH_ADDR;
		packet_buf[1] = (addr >> 16);
		packet_buf[2] = (addr >> 8);
		packet_buf[3] = (addr);

		/* last packet */
		if ((i == (packet_number - 1)) && remainder) {
			packet_len = remainder;
		}

		packet_buf[4] = (packet_len >> 8);
		packet_buf[5] = (packet_len);

		ret = fts_write(packet_buf, 6);
		if (ret < 0) {
			FHP_ERROR("write ac command fails");
			return ret;
		}

		packet_buf[0] = FTS_CMD_WRITE;
		memcpy(&packet_buf[1], buf + offset, packet_len);
		ret = fts_write(packet_buf, (packet_len + 1));
		if (ret < 0) {
			FHP_ERROR("app write fail");
			return ret;
		}
		msleep(1);

		/* read status */
		wr_ok = 0x1000 + addr / packet_len;
		for (j = 0; j < 100; j++) {
			cmd = FTS_CMD_FLASH_STATUS;
			ret = fts_read(&cmd, 1, val, 2);
			read_status = (((u16)val[0]) << 8) + val[1];
			FHP_DEBUG("%x %x", wr_ok, read_status);
			if (wr_ok == read_status) {
				break;
			}
			msleep(1);
		}
	}

	return 0;
}


static int fts_flash_write_ecc(u32 saddr, u8 *buf, u32 len)
{
	int ret = 0;
	int ecc_host = 0;
	int ecc_tp = 0;

	ret = fts_flash_write_buf(saddr, buf, len);
	if (ret < 0) {
		FHP_ERROR("write buffer fails");
		return ret;
	}

	ecc_host = fts_fwupg_ecc_host(buf, len);

	ecc_tp = fts_fwupg_ecc_tp(saddr, len);
	if (ecc_tp < 0) {
		FHP_ERROR("read ecc from tp fails");
		return ret;
	}

	FHP_INFO("ecc_host:%d, ecc_tp:%d", ecc_host, ecc_tp);
	if (ecc_host != ecc_tp) {
		FHP_ERROR("ecc check error,ecc_host(%d) != ecc_tp(%d)", ecc_host, ecc_tp);
		return -EINVAL;
	}

	return 0;
}


static int fts_flash_upgrade_mode(u8 mode, u8 *buf, u32 len)
{
	int ret = 0;
	u8 cmd[4] = { 0 };

	/*send firmware size*/
	cmd[0] = 0xB0;
	cmd[1] = (len >> 16);
	cmd[2] = (len >> 8);
	cmd[3] = (len);
	ret = fts_write(cmd, 4);
	if (ret < 0) {
		FHP_ERROR("write 7A fails");
		return ret;
	}

	/*choose upgrade mode*/
	cmd[0] = FTS_CMD_FLASH_MODE;
	cmd[1] = mode;
	ret = fts_write_reg(cmd[0], cmd[1]);
	if (ret < 0) {
		FHP_ERROR("write 09 0B fails");
		return ret;
	}

	ret = fts_fwupg_erase(60 * (len / 4096));
	if (ret < 0) {
		FHP_ERROR("erase cmd write fail");
		return ret;
	}

	ret = fts_flash_write_ecc(0, buf, len);
	if (ret < 0) {
		FHP_ERROR("write and check ecc fail");
		return ret;
	}

	return 0;
}


static int fts_ft5662_upgrade_app(u8 *fw, u32 fw_len)
{
	int ret = 0;

	/*enter into boot*/
	fts_enter_into_boot();

	if (ret < 0) {
		FHP_ERROR("write pram fail");
		goto upg_err;
	}

	/*check id*/
	ret = fts_check_id(FTS_ID_H_PRAMBOOT, FTS_ID_L_PRAMBOOT);
	if (ret < 0) {
		FHP_ERROR("checking id fails");
		goto upg_err;
	}

	/*delay 20ms to wait for pramboot initialization to be finished*/
	msleep(20);
	ret = fts_flash_upgrade_mode(FLASH_MODE_UPGRADE_VALUE, fw, fw_len);
	if (ret < 0) {
		FHP_ERROR("write flash fail");
		goto upg_err;
	}

	ret = 0;
upg_err:
	fts_write_command(FTS_CMD_RESET);
	msleep(100);
	return ret;
}


static int fts_upgrade_app(u8 *fw, u32 fw_len)
{
	int ret = 0;
	int i = 0;
	u8 val = 0;

	FHP_INFO("fw upgrade download function");
	if (!fw || (!fw_len) || (fw_len <= FTS_MIN_LEN)) {
		FHP_ERROR("firmware buffer is null, or length(%d) is invalid", fw_len);
		return -EINVAL;
	}


	for (i = 0; i < FTS_FW_DOWNLOAD_RETRY_TIMES; i++) {
		FHP_INFO("fw upgarde times:%d", i + 1);
		ret = fts_ft5662_upgrade_app(fw, fw_len);
		if (0 == ret) {
			break;
		}
	}

	if (i >= FTS_FW_DOWNLOAD_RETRY_TIMES) {
		FHP_ERROR("fw download fail");
		return -EIO;
	}
	fts_read_reg(0xA6, &val);
	FHP_ERROR("upgrade success, firmware version:%x\n", val);
	return 0;
}


static bool fts_fw_need_upgrade(u8 *buf, u32 len)
{
	int ret = 0;
	bool fwvalid = false;
	u8 fw_ver_in_host = 0;
	u8 fw_ver_in_tp = 0;

	if (len < FTS_BINOFF_FWVER) {
		FHP_INFO("fw len(%d) is invalid, no need upgrade fw", len);
		return false;
	}

	fwvalid = fts_fw_check_valid();
	if (fwvalid) {
		fw_ver_in_host = buf[FTS_BINOFF_FWVER];
		ret = fts_read_reg(FTS_REG_FW_VER, &fw_ver_in_tp);
		if (ret < 0) {
			FHP_ERROR("read fw ver from tp fail");
			return false;
		}

		FHP_INFO("fw version in tp:%x, host:%x", fw_ver_in_tp, fw_ver_in_host);
		if (fw_ver_in_tp != fw_ver_in_host) {
			return true;
		}
	} else {
		FHP_INFO("fw invalid, need upgrade fw");
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
			ret = fts_upgrade_app(fw, fwlen);
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
	FHP_DEBUG("get upgrade fw file");
	if (FTS_FW_REQUEST_SUPPORT) {
		ret = request_firmware(&request_fw, FTS_FW_NAME_REQ,
				       &ts_data->fhp_data->spi->dev);
		if (0 == ret) {
			FHP_INFO("firmware(%s) request successfully", FTS_FW_NAME_REQ);
			fw = (u8 *)request_fw->data;
			fwlen = (u32)request_fw->size;
		} else {
			get_fw_i_flag = true;
			FHP_INFO("firmware(%s) request fail,ret=%d", FTS_FW_NAME_REQ, ret);
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
	if (ts_data->ts_workqueue) {
		destroy_workqueue(ts_data->ts_workqueue);
	}
}

int fts_fwupg_init(struct fts_ts_data *ts_data)
{
	FTS_INFO("FW upgrade init function");

#if !FTS_AUTO_UPGRADE_EN
	FHP_INFO("FTS_AUTO_UPGRADE_EN is disabled, not upgrade when power on");
	ts_data->chip_init_done = 1;
	return 0;
#endif

	if (!ts_data || !ts_data->ts_workqueue) {
		FHP_ERROR("ts_data/workqueue is NULL, can't run upgrade function");
		return -EINVAL;
	}

	INIT_WORK(&ts_data->fwupg_work, fts_fwupg_work);
	queue_work(ts_data->ts_workqueue, &ts_data->fwupg_work);

	return 0;
}

