/*
 * mal_hspec_cmn.h
 *
 *  Created on: May 3, 2015
 *      Author: Olivier
 */
/*
 * Copyright (c) 2015 Olivier Allaire
 *
 * This file is part of MAL.
 *
 * MAL is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MAL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MAL.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef HSPEC_MAL_HSPEC_CMN_H_
#define HSPEC_MAL_HSPEC_CMN_H_

#include "std/mal_stdint.h"
#include "std/mal_error.h"

// Clock

typedef enum {
	MAL_HSPEC_SYSTEM_CLK_SRC_INTERNAL,
	MAL_HSPEC_SYSTEM_CLK_SRC_EXTERNAL,
	MAL_HSPEC_SYSTEM_CLK_SRC_AUTO
} mal_hspec_system_clk_src_e;

typedef struct {
	uint64_t frequency;
	mal_hspec_system_clk_src_e src;
} mal_hspec_system_clk_s;

// GPIO

typedef enum {
	MAL_HSPEC_PORT_A = 0,
	MAL_HSPEC_PORT_B = 1,
	MAL_HSPEC_PORT_C = 2,
	MAL_HSPEC_PORT_D = 3,
	MAL_HSPEC_PORT_E = 4,
	MAL_HSPEC_PORT_F = 5,
	MAL_HSPEC_PORT_SIZE = 6
} mal_hspec_port_e;

typedef struct {
	mal_hspec_port_e port;
	uint8_t pin;
} mal_hspec_gpio_s;

typedef enum {
	MAL_GPIO_DIR_IN,
	MAL_GPIO_DIR_OUT
} mal_hspec_gpio_dir_e;

typedef enum {
	MAL_GPIO_OUT_PP,
	MAL_GPIO_OUT_OD
} mal_hspec_gpio_out_e;

typedef enum {
	MAL_GPIO_PUPD_NONE,
	MAL_GPIO_PUPD_PU,
	MAL_GPIO_PUPD_PD
} mal_hspec_gpio_pupd_e;

typedef struct {
	mal_hspec_gpio_s gpio;
	mal_hspec_gpio_dir_e direction;
	mal_hspec_gpio_out_e output_config;
	mal_hspec_gpio_pupd_e pupd;
	uint64_t speed;
} mal_hpsec_gpio_init_s;

// TIMER

typedef enum {
	MAL_HSPEC_TIMER_1 = 0,
	MAL_HSPEC_TIMER_2 = 1,
	MAL_HSPEC_TIMER_3 = 2,
	MAL_HSPEC_TIMER_4 = 3,
	MAL_HSPEC_TIMER_5 = 4,
	MAL_HSPEC_TIMER_6 = 5,
	MAL_HSPEC_TIMER_7 = 6,
	MAL_HSPEC_TIMER_8 = 7,
	MAL_HSPEC_TIMER_9 = 8,
	MAL_HSPEC_TIMER_10 = 9,
	MAL_HSPEC_TIMER_11 = 10,
	MAL_HSPEC_TIMER_12 = 11,
	MAL_HSPEC_TIMER_13 = 12,
	MAL_HSPEC_TIMER_14= 13,
	MAL_HSPEC_TIMER_15 = 14,
	MAL_HSPEC_TIMER_16 = 15,
	MAL_HSPEC_TIMER_17 = 16,
	MAL_HSPEC_TIMER_SIZE = 17,
	MAL_HSPEC_TIMER_ANY
} mal_hspec_timer_e;

typedef enum {
	MAL_HSPEC_TIMER_MODE_TICK
} mal_hspec_timer_mode_e;

// Serial

typedef enum {
	MAL_HSPEC_SERIAL_PORT_1 = 0,
	MAL_HSPEC_SERIAL_PORT_2 = 1,
	MAL_HSPEC_SERIAL_PORT_SIZE = 2
} mal_hspec_serial_port_e;

// I2C

typedef enum {
	MAL_HSPEC_I2C_1 = 0,
	MAL_HSPEC_I2C_2 = 1,
	MAL_HSPEC_I2C_SIZE = 2
} mal_hspec_i2c_e;

typedef struct {
	mal_hspec_i2c_e interface;
	mal_hspec_gpio_s *scl_gpio;
	mal_hspec_gpio_s *sda_gpio;
	uint64_t bitrate;
} mal_hspec_i2c_init_s;

typedef enum {
	MAL_HSPEC_I2C_READ,
	MAL_HSPEC_I2C_WRITE
} mal_hspec_i2c_cmd_e;

typedef struct {
	volatile uint8_t address;
	volatile uint8_t *buffer;
	volatile uint8_t packet_size;
	mal_hspec_i2c_cmd_e cmd;
} mal_hspec_i2c_packet_t;

typedef enum {
	MAL_HSPEC_I2C_SUCCESS,
	MAL_HSPEC_I2C_NACK_COMPLETE,
	MAL_HSPEC_I2C_NACK_INCOMPLETE,
	MAL_HSPEC_I2C_BUS_ERROR
} mal_hspec_i2c_result_e;

typedef struct MAL_HSPEC_I2C_MSG mal_hspec_i2c_msg_t;

typedef mal_error_e (*mal_hspec_i2c_callback_t)(mal_hspec_i2c_packet_t *packet, mal_hspec_i2c_result_e result, mal_hspec_i2c_msg_t **next_msg);

typedef struct MAL_HSPEC_I2C_MSG {
	mal_hspec_i2c_packet_t packet;
	mal_hspec_i2c_callback_t callback;
} mal_hspec_i2c_msg_t;

#endif /* HSPEC_MAL_HSPEC_CMN_H_ */
