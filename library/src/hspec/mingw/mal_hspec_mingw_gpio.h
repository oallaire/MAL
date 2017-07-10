/*
 * mal_hspec_mingw_gpio.h
 *
 *  Created on: Mar 24, 2016
 *      Author: Kevin
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

#ifndef HSPEC_MINGW_MAL_HSPEC_MINGW_GPIO_H_
#define HSPEC_MINGW_MAL_HSPEC_MINGW_GPIO_H_

#include "gpio/mal_gpio.h"

void mal_hspec_mingw_gpio_execute_callback(mal_gpio_s gpio);

mal_error_e mal_hspec_mingw_set_mocked_gpio(const mal_gpio_s *gpio, bool value);

#endif /* HSPEC_MINGW_MAL_HSPEC_MINGW_GPIO_H_ */
