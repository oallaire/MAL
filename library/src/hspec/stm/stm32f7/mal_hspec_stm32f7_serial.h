/*
 * Copyright (c) 2018 Olivier Allaire
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

#ifndef HSPEC_STM_STM32F7_MAL_HSPEC_STM32F7_SERIAL_H_
#define HSPEC_STM_STM32F7_MAL_HSPEC_STM32F7_SERIAL_H_

#include "stm32f7/stm32f7xx_hal_uart.h"
#include "serial/mal_serial.h"
#include "stm32f7/stm32f7xx_hal_dma.h"

typedef struct MAL_SERIAL {
    UART_HandleTypeDef hal_serial_handle;
    DMA_HandleTypeDef hal_dma;
} mal_serial_s;

typedef struct MAL_SERIAL_INTERRUPT_STATE {

} mal_serial_interrupt_state_s;

mal_error_e mal_hspec_stm32f7_serial_get_alternate(mal_serial_port_e port, mal_gpio_port_e gpio_port, uint8_t pin,
                                                   uint32_t *alternate);

#endif /* HSPEC_STM_STM32F7_MAL_HSPEC_STM32F7_SERIAL_H_ */
