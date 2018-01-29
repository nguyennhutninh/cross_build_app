#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>

#include "../ak/ak.h"
#include "../ak/timer.h"

#include "../common/firmware.h"

#include "app.h"
#include "app_if.h"
#include "app_dbg.h"
#include "app_data.h"

#include "task_list.h"
#include "task_list_if.h"
#include "task_firmware.h"
#include "task_debug_msg.h"
#include "task_if.h"

using namespace std;

q_msg_t mt_task_firmware_mailbox;

static uint32_t bin_index;
static uint8_t checksum_flag = 0;

firmware_header_t current_firmware_info;
firmware_header_t file_firmware_info;

string firmware_binary_path;
string firmware_file_name;

void* mt_task_firmware_entry(void*) {
	wait_all_tasks_started();

	APP_DBG("gw_task_firmware_entry\n");

	firmware_binary_path = static_cast<string>(APP_ROOT_PATH_DISK) + static_cast<string>("/dev_firmware/");
	firmware_file_name = firmware_binary_path + static_cast<string>("outdoor_slave_application.bin");

	struct stat st = {0};
	if (stat(firmware_binary_path.data(), &st) == -1) {
		mkdir(firmware_binary_path.data(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	}

	ak_msg_t* msg;

	while (1) {
		/* get messge */
		msg = msg_get(MT_TASK_FIRMWARE_ID);

		switch (msg->header->sig) {
		case MT_FIRMWARE_SL_FW_UPDATE_REQ: {
			APP_DBG("MT_FIRMWARE_SL_FW_UPDATE_REQ\n");

			ak_msg_t* s_msg = get_pure_msg();
			set_if_src_task_id(s_msg, MT_TASK_FIRMWARE_ID);
			set_if_des_task_id(s_msg, SL_TASK_FIRMWARE_ID);
			set_if_des_type(s_msg, IF_TYPE_CPU_SERIAL_SL);
			set_if_sig(s_msg, SL_FIRMWARE_FW_INFO_REQ);

			set_msg_sig(s_msg, MT_IF_PURE_MSG_OUT);
			set_msg_src_task_id(s_msg, MT_TASK_FIRMWARE_ID);
			task_post(MT_TASK_IF_ID, s_msg);
		}
			break;

		case MT_FIRMWARE_SL_FW_INFO_RES: {
			APP_DBG("MT_FIRMWARE_SL_FW_INFO_RES\n");

			get_data_common_msg(msg, (uint8_t*)&current_firmware_info, sizeof(firmware_header_t));
			APP_DBG("current firmware checksum: %04x\n", current_firmware_info.checksum);
			APP_DBG("current firmware bin_len: %d\n", current_firmware_info.bin_len);

			APP_DBG("FILE:%s\n", firmware_file_name.data());
			if (stat(firmware_file_name.data(), &st) == -1) {
				APP_DBG("binary file:%s does not exist !\n", firmware_file_name.data());
			}
			else {
				firmware_get_info(&file_firmware_info, firmware_file_name.data());
				APP_DBG("file firmware checksum: %04x\n", file_firmware_info.checksum);
				APP_DBG("file firmware bin_len: %d\n", file_firmware_info.bin_len);
				APP_DBG("file firmware checksum: %04x\n", current_firmware_info.checksum);
				APP_DBG("file firmware bin_len: %d\n", current_firmware_info.bin_len);

				/* checking firmware version */
				if (current_firmware_info.checksum == file_firmware_info.checksum		\
						&& current_firmware_info.bin_len == file_firmware_info.bin_len) {
					APP_DBG("USER_NO_NEED_TO_UPDATE\n");

					{
						ak_msg_t* s_msg = get_common_msg();

						set_if_src_type(s_msg, IF_TYPE_APP_MT);
						set_if_des_type(s_msg, IF_TYPE_APP_GU);
						set_if_src_task_id(s_msg,MT_TASK_FIRMWARE_ID);
						set_if_des_task_id(s_msg, GU_TASK_FIRMWARE_ID);
						set_if_sig(s_msg, GU_FIRMWARE_NO_NEED_TO_UPDATE);
						set_if_data_common_msg(s_msg, (uint8_t*)&current_firmware_info, sizeof(firmware_header_t));

						set_msg_sig(s_msg, MT_IF_COMMON_MSG_OUT);
						set_msg_src_task_id(s_msg, MT_TASK_FIRMWARE_ID);
						task_post(MT_TASK_IF_ID, s_msg);
					}

					{
						ak_msg_t* s_msg = get_pure_msg();
						set_msg_sig(s_msg, MT_SM_SL_FW_NO_NEED_UPDATE_REQ);
						task_post(MT_TASK_SM_ID, s_msg);
					}
				}
				else {

					APP_DBG("USER_NEED_TO_UPDATE\n");

					{
						ak_msg_t* s_msg = get_common_msg();

						set_if_src_type(s_msg, IF_TYPE_APP_MT);
						set_if_des_type(s_msg, IF_TYPE_APP_GU);
						set_if_src_task_id(s_msg,MT_TASK_FIRMWARE_ID);
						set_if_des_task_id(s_msg, GU_TASK_FIRMWARE_ID);
						set_if_sig(s_msg, GU_FIRMWARE_STARTED_TRANSFER);
						set_if_data_common_msg(s_msg, (uint8_t*)&file_firmware_info, sizeof(firmware_header_t));

						set_msg_sig(s_msg, MT_IF_COMMON_MSG_OUT);
						set_msg_src_task_id(s_msg, MT_TASK_FIRMWARE_ID);
						task_post(MT_TASK_IF_ID, s_msg);
					}

					{
						ak_msg_t* s_msg = get_pure_msg();
						set_msg_sig(s_msg, MT_SM_SL_FW_NEED_UPDATE_REQ);
						task_post(MT_TASK_SM_ID, s_msg);
					}

					{
						ak_msg_t* s_msg = get_pure_msg();
						set_msg_sig(s_msg, MT_SENSOR_SL_STOP_GET_INFO_REQ);
						task_post(MT_TASK_SENSOR_ID, s_msg);
					}
				}
			}
		}
			break;

		case MT_FIRMWARE_SL_FW_TRANF_REQ: {
			APP_DBG("MT_FIRMWARE_SL_FW_TRANF_REQ\n");

			ak_msg_t* s_msg = get_common_msg();
			set_if_src_task_id(s_msg, MT_TASK_FIRMWARE_ID);
			set_if_des_task_id(s_msg, SL_TASK_FIRMWARE_ID);
			set_if_des_type(s_msg, IF_TYPE_CPU_SERIAL_SL);
			set_if_sig(s_msg, SL_FIRMWARE_SL_FW_TRANF_REQ);
			set_if_data_common_msg(s_msg, (uint8_t*)&file_firmware_info, sizeof(firmware_header_t));

			set_msg_sig(s_msg, MT_IF_COMMON_MSG_OUT);
			set_msg_src_task_id(s_msg, MT_TASK_FIRMWARE_ID);
			task_post(MT_TASK_IF_ID, s_msg);
		}
			break;

		case MT_FIRMWARE_SL_FW_TRANF_RES: {
			APP_DBG("MT_FIRMWARE_SL_FW_TRANF_RES\n");\

			ak_msg_t* s_msg = get_pure_msg();
			set_msg_sig(s_msg, MT_SM_SL_FW_START_TRANF_REP);
			task_post(MT_TASK_SM_ID, s_msg);

			/* reset bin_file_cursor & checksum flag */
			bin_index = 0;
			checksum_flag = 0;

			/*set timer for control flow tranfer data*/
			timer_set(MT_TASK_FIRMWARE_ID, MT_FIRMWARE_SL_FW_TRANF_DATA_REQ, MT_FIRMWARE_SL_FW_TRANF_DATA_REQ_INTERVAL, TIMER_ONE_SHOT);
		}
			break;

		case MT_FIRMWARE_SL_FW_TRANF_DATA_RES:
			APP_DBG("MT_FIRMWARE_SL_FW_TRANF_DATA_RES\n");
			//APP_UI_FW("|", &ui_setting);
			/*set timer for control flow tranfer data*/
			timer_set(MT_TASK_FIRMWARE_ID, MT_FIRMWARE_SL_FW_TRANF_DATA_REQ, MT_FIRMWARE_SL_FW_TRANF_DATA_REQ_INTERVAL, TIMER_ONE_SHOT);
			break;

		case MT_FIRMWARE_SL_FW_TRANF_DATA_REQ: {
			APP_DBG("MT_FIRMWARE_SL_FW_TRANF_DATA_REQ\n");

			uint8_t data_temp[AK_COMMON_MSG_DATA_SIZE];
			uint32_t remain;
			uint8_t len;

			memset(data_temp, 0, AK_COMMON_MSG_DATA_SIZE);
			remain = file_firmware_info.bin_len - bin_index;

			if (file_firmware_info.bin_len - bin_index <= AK_COMMON_MSG_DATA_SIZE) {
				len = (uint8_t)remain;
			}
			else {
				len = AK_COMMON_MSG_DATA_SIZE;
			}

			firmware_read(data_temp, bin_index, len, firmware_file_name.data());
			bin_index += len;

			uint32_t firmware_transferred = bin_index;

			APP_DBG("bin_index: %d\n",firmware_transferred);

			{
				ak_msg_t* s_msg = get_common_msg();

				set_if_src_type(s_msg, IF_TYPE_APP_MT);
				set_if_des_type(s_msg, IF_TYPE_APP_GU);
				set_if_src_task_id(s_msg,MT_TASK_FIRMWARE_ID);
				set_if_des_task_id(s_msg, GU_TASK_FIRMWARE_ID);
				set_if_sig(s_msg, GU_FIRMWARE_TRANSFER_STATUS);
				set_if_data_common_msg(s_msg, (uint8_t*)&firmware_transferred, sizeof(uint32_t));

				set_msg_sig(s_msg, MT_IF_COMMON_MSG_OUT);
				set_msg_src_task_id(s_msg, MT_TASK_FIRMWARE_ID);
				task_post(MT_TASK_IF_ID, s_msg);
			}

			/*tranfer firmware not complete*/
			if (bin_index <= file_firmware_info.bin_len && checksum_flag == 0) {
				/*set timeout for release tranfer data failed*/
				timer_set(MT_TASK_FIRMWARE_ID, MT_FIRMWARE_SL_FW_PACKED_TIMEOUT_REQ, MT_FIRMWARE_SL_FW_PACKED_TIMEOUT_REQ_INTERVAL, TIMER_ONE_SHOT);

				/*firmware data is tranferred*/
				{
					ak_msg_t* s_msg = get_common_msg();
					set_msg_sig(s_msg, MT_SM_SL_FW_TRANF_DATA_REP);
					set_data_common_msg(s_msg, (uint8_t*)&bin_index, sizeof(uint32_t));
					task_post(MT_TASK_SM_ID, s_msg);
				}

				/*tranfer firmware data to task_firmware slave*/
				{
					ak_msg_t* s_msg = get_common_msg();
					set_if_src_task_id(s_msg, MT_TASK_FIRMWARE_ID);
					set_if_des_task_id(s_msg, SL_TASK_FIRMWARE_ID);
					set_if_des_type(s_msg, IF_TYPE_CPU_SERIAL_SL);
					set_if_sig(s_msg, SL_FIRMWARE_SL_FW_TRANF_DATA_REQ);
					set_if_data_common_msg(s_msg, data_temp, AK_COMMON_MSG_DATA_SIZE);

					set_msg_sig(s_msg, MT_IF_COMMON_MSG_OUT);
					task_post(MT_TASK_IF_ID, s_msg);
				}

				if (bin_index == file_firmware_info.bin_len) checksum_flag = 1;
			}
			/*tranfer firmware complete*/
			else {
				/*set timeout for release checksum failed*/
				timer_set(MT_TASK_FIRMWARE_ID, MT_FIRMWARE_SL_FW_PACKED_TIMEOUT_REQ, MT_FIRMWARE_SL_FW_CHECKSUM_TIMEOUT_REQ_INTERVAL, TIMER_ONE_SHOT);

				timer_remove_attr(MT_TASK_FIRMWARE_ID, MT_FIRMWARE_SL_FW_TRANF_DATA_REQ);

				ak_msg_t* s_msg = get_pure_msg();
				set_if_src_task_id(s_msg, MT_TASK_FIRMWARE_ID);
				set_if_des_task_id(s_msg, SL_TASK_FIRMWARE_ID);
				set_if_des_type(s_msg, IF_TYPE_CPU_SERIAL_SL);
				set_if_sig(s_msg, SL_FIRMWARE_SL_FW_CALC_CHECKSUM_REQ);

				set_msg_sig(s_msg, MT_IF_PURE_MSG_OUT);
				set_msg_src_task_id(s_msg, MT_TASK_FIRMWARE_ID);
				task_post(MT_TASK_IF_ID, s_msg);
			}
		}
			break;

		case MT_FIRMWARE_SL_FW_CHECKSUM_CORRECT_RES: {
			APP_DBG("MT_FIRMWARE_SL_FW_CHECKSUM_CORRECT_RES\n");

			/*set timeout for release checksum failed*/
			timer_set(MT_TASK_FIRMWARE_ID, MT_FIRMWARE_SL_FW_PACKED_TIMEOUT_REQ, MT_FIRMWARE_SL_FW_CHECKSUM_TIMEOUT_REQ_INTERVAL, TIMER_ONE_SHOT);

			/*firmware data tranfer complete report */
			{
				ak_msg_t* s_msg = get_pure_msg();
				set_msg_sig(s_msg, MT_SM_SL_FW_TRANF_DATA_COMPLETED_REP);
				task_post(MT_TASK_SM_ID, s_msg);
			}
			/*request slave update firmware */
			{
				APP_DBG("Kiem tra lai firmware update---------------->>\n");
				ak_msg_t* s_msg = get_pure_msg();
				set_if_src_task_id(s_msg, MT_TASK_FIRMWARE_ID);
				set_if_des_task_id(s_msg, SL_TASK_FIRMWARE_ID);
				set_if_des_type(s_msg, IF_TYPE_CPU_SERIAL_SL);
				set_if_sig(s_msg, SL_FIRMWARE_SL_FW_INTERNAL_UPDATE_REQ);

				set_msg_sig(s_msg, MT_IF_PURE_MSG_OUT);
				set_msg_src_task_id(s_msg, MT_TASK_FIRMWARE_ID);
				task_post(MT_TASK_IF_ID, s_msg);
			}
		}
			break;

		case MT_FIRMWARE_SL_FW_CHECKSUM_INCORRECT_RES: {
			APP_DBG("MT_FIRMWARE_SL_FW_CHECKSUM_INCORRECT_RES\n");

			timer_remove_attr(MT_TASK_FIRMWARE_ID, MT_FIRMWARE_SL_FW_PACKED_TIMEOUT_REQ);

			/*firmware data tranfer failse report */
			ak_msg_t* s_msg = get_pure_msg();
			set_msg_sig(s_msg, MT_SM_SL_FW_UPDATE_FAILED_RES);
			task_post(MT_TASK_SM_ID, s_msg);
		}
			break;

		case MT_FIRMWARE_SL_FW_UPDATE_COMPLETED_REP: {
			APP_DBG("MT_FIRMWARE_SL_FW_UPDATE_COMPLETED_REP\n");

			timer_remove_attr(MT_TASK_FIRMWARE_ID, MT_FIRMWARE_SL_FW_PACKED_TIMEOUT_REQ);

			{
				ak_msg_t* s_msg = get_pure_msg();
				set_msg_sig(s_msg, MT_SM_SL_FW_UPDATE_COMPLETED_REP);
				task_post(MT_TASK_SM_ID, s_msg);
			}
		}
			break;

		case MT_FIRMWARE_SL_FW_PACKED_TIMEOUT_REQ: {
			APP_DBG("MT_FIRMWARE_SL_FW_PACKED_TIMEOUT_REQ\n");
			/*firmware data tranfer failse report */
			ak_msg_t* s_msg = get_pure_msg();
			set_msg_sig(s_msg, MT_SM_SL_FW_UPDATE_FAILED_RES);
			task_post(MT_TASK_SM_ID, s_msg);
		}
			break;

		case MT_FIRMWARE_SL_FW_OTA_REQ: {
			APP_DBG("MT_FIRMWARE_SL_FW_OTA_REQ\n");

			string outdoor_slave_firmware_name;

			get_data_common_msg(msg, (uint8_t* )outdoor_slave_firmware_name.data(), get_data_len_common_msg(msg));
			firmware_file_name = firmware_binary_path + static_cast<string>(outdoor_slave_firmware_name.data());

			APP_DBG("FILE:%s\n", firmware_file_name.data());
			if (stat(firmware_file_name.data(), &st) == -1) {
				APP_DBG("binary file:%s does not exist !\n", firmware_file_name.data());

				{
					uint32_t error_code;
					error_code = 1;

					ak_msg_t* s_msg = get_common_msg();

					set_if_src_type(s_msg, IF_TYPE_APP_MT);
					set_if_des_type(s_msg, IF_TYPE_APP_GU);
					set_if_src_task_id(s_msg,MT_TASK_FIRMWARE_ID);
					set_if_des_task_id(s_msg, GU_TASK_FIRMWARE_ID);
					set_if_sig(s_msg, GU_FIRMWARE_UPDATE_ERR);
					set_if_data_common_msg(s_msg, (uint8_t*)&error_code, sizeof(uint32_t));

					set_msg_sig(s_msg, MT_IF_COMMON_MSG_OUT);
					set_msg_src_task_id(s_msg, MT_TASK_FIRMWARE_ID);
					APP_DBG("binary file does not exist\n");
					task_post(MT_TASK_IF_ID, s_msg);
				}
			}
			else {

				ak_msg_t* s_msg = get_pure_msg();
				set_if_src_task_id(s_msg, MT_TASK_FIRMWARE_ID);
				set_if_des_task_id(s_msg, SL_TASK_FIRMWARE_ID);
				set_if_des_type(s_msg, IF_TYPE_CPU_SERIAL_SL);
				set_if_sig(s_msg, SL_FIRMWARE_FW_INFO_REQ);

				set_msg_sig(s_msg, MT_IF_PURE_MSG_OUT);
				set_msg_src_task_id(s_msg, MT_TASK_FIRMWARE_ID);
				task_post(MT_TASK_IF_ID, s_msg);
			}
		}
			break;

		default:
			break;
		}

		/* free message */
		msg_free(msg);
	}

	return (void*)0;
}
