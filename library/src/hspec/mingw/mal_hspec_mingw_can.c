/*
 * mal_hspec_mingw_can.c
 *
 *  Created on: Mar 24, 2016
 *      Author: Olivier
 */
/*
 * Copyright (c) 2015 Olivier Allaire
 *
 * This file is part of MAL.
 *
 * MAL is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MAL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with MAL.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "mal_hspec_mingw_can.h"
#include "mal_hspec_mingw_cmn.h"
#include "utils/mal_circular_buffer.h"

#define MESSAGE_BUFFER_SIZE	3

typedef struct {
	mal_hspec_can_init_s init;
	mal_circular_buffer_s tx_circular_buffer;
	mal_hspec_can_msg_s message_buffer[MESSAGE_BUFFER_SIZE];
} mingw_can_interface_s;

static mingw_can_interface_s can_interfaces[MAL_HSPEC_CAN_SIZE];

mal_error_e mal_hspec_mingw_get_valid_can_ios(mal_hspec_can_e interface, const mal_hspec_gpio_s **txs, uint8_t *txs_size, const mal_hspec_gpio_s **rxs, uint8_t *rxs_size) {
	// Fetch IOs
	mal_hspec_mingw_cmn_valid_ios(txs, txs_size);
	*rxs = *txs;
	*rxs_size = *txs_size;

	return MAL_ERROR_OK;
}

mal_error_e mal_hspec_mingw_can_init(mal_hspec_can_init_s *init) {
	// Save init
	can_interfaces[init->interface].init = *init;
	// Initialise circular buffer
	mal_circular_buffer_init((void*)can_interfaces[init->interface].message_buffer,
							 sizeof(mal_hspec_can_msg_s),
							 sizeof(mal_hspec_can_msg_s) * MESSAGE_BUFFER_SIZE,
							 &can_interfaces[init->interface].tx_circular_buffer);
	return MAL_ERROR_OK;
}

mal_error_e mal_hspec_mingw_can_transmit(mal_hspec_can_e interface, mal_hspec_can_msg_s *msg) {
	mal_error_e result;
	// Write to buffer
	result = mal_circular_buffer_write(&can_interfaces[interface].tx_circular_buffer, msg);
	if (MAL_ERROR_OK != result) {
		return MAL_ERROR_HARDWARE_UNAVAILABLE;
	}
	return result;
}

mal_error_e mal_hspec_mingw_can_get_tx_msg(mal_hspec_can_e interface, mal_hspec_can_msg_s *msg) {
	mal_error_e result;
	// Remove message from buffer
	result = mal_circular_buffer_read(&can_interfaces[interface].tx_circular_buffer, msg);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	// Execute tx callback
	mal_hspec_can_msg_s next_msg;
	result = can_interfaces[interface].init.tx_callback(interface, &next_msg);
	if (MAL_ERROR_OK == result) {
		mal_hspec_mingw_can_transmit(interface, &next_msg);
	}

	return MAL_ERROR_OK;
}

mal_error_e mal_hspec_mingw_can_add_filter(mal_hspec_can_e interface, mal_hspec_can_filter_s *filter) {
	return MAL_ERROR_OK;
}

mal_error_e mal_hspec_mingw_can_remove_filter(mal_hspec_can_e interface, mal_hspec_can_filter_s *filter) {
	return MAL_ERROR_OK;
}

mal_error_e mal_hspec_mingw_can_push_rx_msg(mal_hspec_can_e interface, mal_hspec_can_msg_s *msg) {
	return can_interfaces[interface].init.rx_callback(interface, msg);
}
