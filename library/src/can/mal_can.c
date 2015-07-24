/*
 * mal_can.c
 *
 *  Created on: Jun 14, 2015
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

#include "can/mal_can.h"
#include "std/mal_error.h"
#include "hspec/mal_hspec.h"

mal_error_e mal_can_init(mal_hspec_can_init_s *init) {
	mal_error_e result;
	// Check if interface is valid
	result = mal_hspec_is_can_interface_valid(init->interface, init->tx_gpio, init->rx_gpio);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	// Initialise interface
	return mal_hspec_can_init(init);
}
