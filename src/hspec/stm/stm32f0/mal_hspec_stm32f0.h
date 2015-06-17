/*
 * mal_hspec_stm32.h
 *
 *  Created on: May 2, 2015
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

#ifndef HSPEC_STM_MAL_HSPEC_STM32F0_H_
#define HSPEC_STM_MAL_HSPEC_STM32F0_H_

#include "std/mal_error.h"
#include "hspec/mal_hspec_cmn.h"
#include "gpio/mal_gpio.h"
#include "std/mal_bool.h"
#include "stm32f0/stm32f0xx.h"
#include "cmsis/core_cmInstr.h"

#define MAL_HSPEC_STM32F0_GPIO_PORT_SIZE	16
#define MAL_HSPEC_STM32F0_GPIO_PORT_AF_SIZE	8
#define MAL_HSPEC_STM32F0_GPIO_PIN_AF_SIZE	2

typedef enum {
	MAL_HSPEC_STM32F0_AF_NONE = 0,
	MAL_HSPEC_STM32F0_AF_USART2_CTS,
	MAL_HSPEC_STM32F0_AF_TIM2_CH1_ETR,
	MAL_HSPEC_STM32F0_AF_TSC_G1_IO1,
	MAL_HSPEC_STM32F0_AF_COMP1_OUT,
	MAL_HSPEC_STM32F0_AF_EVENTOUT,
	MAL_HSPEC_STM32F0_AF_USART2_RTS,
	MAL_HSPEC_STM32F0_AF_TIM2_CH2,
	MAL_HSPEC_STM32F0_AF_TSC_G1_IO2,
	MAL_HSPEC_STM32F0_AF_TIM15_CH1,
	MAL_HSPEC_STM32F0_AF_USART2_TX,
	MAL_HSPEC_STM32F0_AF_TIM2_CH3,
	MAL_HSPEC_STM32F0_AF_TSC_G1_IO3,
	MAL_HSPEC_STM32F0_AF_COMP2_OUT,
	MAL_HSPEC_STM32F0_AF_TIM15_CH2,
	MAL_HSPEC_STM32F0_AF_USART2_RX,
	MAL_HSPEC_STM32F0_AF_TIM2_CH4,
	MAL_HSPEC_STM32F0_AF_TSC_G1_IO4,
	MAL_HSPEC_STM32F0_AF_SPI1_NSS,
	MAL_HSPEC_STM32F0_AF_I2S1_WS,
	MAL_HSPEC_STM32F0_AF_USART2_CK,
	MAL_HSPEC_STM32F0_AF_TSC_G2_IO1,
	MAL_HSPEC_STM32F0_AF_TIM14_CH1,
	MAL_HSPEC_STM32F0_AF_SPI1_SCK,
	MAL_HSPEC_STM32F0_AF_I2S1_CK,
	MAL_HSPEC_STM32F0_AF_CEC,
	MAL_HSPEC_STM32F0_AF_TSC_G2_IO2,
	MAL_HSPEC_STM32F0_AF_SPI1_MISO,
	MAL_HSPEC_STM32F0_AF_I2S1_MCK,
	MAL_HSPEC_STM32F0_AF_TIM3_CH1,
	MAL_HSPEC_STM32F0_AF_TIM1_BKIN,
	MAL_HSPEC_STM32F0_AF_TSC_G2_IO3,
	MAL_HSPEC_STM32F0_AF_TIM16_CH1,
	MAL_HSPEC_STM32F0_AF_SPI1_MOSI,
	MAL_HSPEC_STM32F0_AF_I2S1_SD,
	MAL_HSPEC_STM32F0_AF_TIM3_CH2,
	MAL_HSPEC_STM32F0_AF_TIM1_CH1N,
	MAL_HSPEC_STM32F0_AF_TSC_G2_IO4,
	MAL_HSPEC_STM32F0_AF_TIM17_CH1,
	MAL_HSPEC_STM32F0_AF_MCO,
	MAL_HSPEC_STM32F0_AF_USART1_CK,
	MAL_HSPEC_STM32F0_AF_TIM1_CH1,
	MAL_HSPEC_STM32F0_AF_TIM15_BKIN,
	MAL_HSPEC_STM32F0_AF_USART1_TX,
	MAL_HSPEC_STM32F0_AF_TIM1_CH2,
	MAL_HSPEC_STM32F0_AF_TSC_G4_IO1,
	MAL_HSPEC_STM32F0_AF_TIM17_BKIN,
	MAL_HSPEC_STM32F0_AF_USART1_RX,
	MAL_HSPEC_STM32F0_AF_TIM1_CH3,
	MAL_HSPEC_STM32F0_AF_TSC_G4_IO2,
	MAL_HSPEC_STM32F0_AF_USART1_CTS,
	MAL_HSPEC_STM32F0_AF_TIM1_CH4,
	MAL_HSPEC_STM32F0_AF_TSC_G4_IO3,
	MAL_HSPEC_STM32F0_AF_USART1_RTS,
	MAL_HSPEC_STM32F0_AF_TIM1_ETR,
	MAL_HSPEC_STM32F0_AF_TSC_G4_IO4,
	MAL_HSPEC_STM32F0_AF_SWDIO,
	MAL_HSPEC_STM32F0_AF_IR_OUT,
	MAL_HSPEC_STM32F0_AF_SWCLK,
	MAL_HSPEC_STM32F0_AF_TIM3_CH3,
	MAL_HSPEC_STM32F0_AF_TIM1_CH2N,
	MAL_HSPEC_STM32F0_AF_TSC_G3_IO2,
	MAL_HSPEC_STM32F0_AF_TIM3_CH4,
	MAL_HSPEC_STM32F0_AF_TIM1_CH3N,
	MAL_HSPEC_STM32F0_AF_TSC_G3_IO3,
	MAL_HSPEC_STM32F0_AF_TSC_G3_IO4,
	MAL_HSPEC_STM32F0_AF_TSC_G5_IO1,
	MAL_HSPEC_STM32F0_AF_TSC_G5_IO2,
	MAL_HSPEC_STM32F0_AF_TIM16_BKIN,
	MAL_HSPEC_STM32F0_AF_I2C1_SMBA,
	MAL_HSPEC_STM32F0_AF_I2C1_SCL,
	MAL_HSPEC_STM32F0_AF_TIM16_CH1N,
	MAL_HSPEC_STM32F0_AF_TSC_G5_IO3,
	MAL_HSPEC_STM32F0_AF_I2C1_SDA,
	MAL_HSPEC_STM32F0_AF_TIM17_CH1N,
	MAL_HSPEC_STM32F0_AF_TSC_G5_IO4,
	MAL_HSPEC_STM32F0_AF_TSC_SYNC,
	MAL_HSPEC_STM32F0_AF_I2C2_SCL,
	MAL_HSPEC_STM32F0_AF_I2C2_SDA,
	MAL_HSPEC_STM32F0_AF_TSC_G6_IO1,
	MAL_HSPEC_STM32F0_AF_SPI2_NSS,
	MAL_HSPEC_STM32F0_AF_TSC_G6_IO2,
	MAL_HSPEC_STM32F0_AF_SPI2_SCK,
	MAL_HSPEC_STM32F0_AF_TSC_G6_IO3,
	MAL_HSPEC_STM32F0_AF_SPI2_MISO,
	MAL_HSPEC_STM32F0_AF_TSC_G6_IO4,
	MAL_HSPEC_STM32F0_AF_SPI2_MOSI,
	MAL_HSPEC_STM32F0_AF_TIM15_CH1N,
	MAL_HSPEC_STM32F0_AF_USART4_TX,
	MAL_HSPEC_STM32F0_AF_USART4_RX,
	MAL_HSPEC_STM32F0_AF_USART3_CTS,
	MAL_HSPEC_STM32F0_AF_CRS_SYNC,
	MAL_HSPEC_STM32F0_AF_CAN_RX,
	MAL_HSPEC_STM32F0_AF_CAN_TX,
	MAL_HSPEC_STM32F0_AF_USB_NOE,
	MAL_HSPEC_STM32F0_AF_USART4_RTS,
	MAL_HSPEC_STM32F0_AF_USART3_CK,
	MAL_HSPEC_STM32F0_AF_USART3_RTS,
	MAL_HSPEC_STM32F0_AF_USART4_CTS,
	MAL_HSPEC_STM32F0_AF_I2S2_WS,
	MAL_HSPEC_STM32F0_AF_USART3_TX,
	MAL_HSPEC_STM32F0_AF_I2S2_CK,
	MAL_HSPEC_STM32F0_AF_USART3_RX,
	MAL_HSPEC_STM32F0_AF_I2S2_MCK,
	MAL_HSPEC_STM32F0_AF_I2S2_SD,
	MAL_HSPEC_STM32F0_AF_TSC_G3_IO1,
	MAL_HSPEC_STM32F0_AF_USART4_CK,
	MAL_HSPEC_STM32F0_AF_TIM3_ETR,
	MAL_HSPEC_STM32F0_AF_TSC_G8_IO1,
	MAL_HSPEC_STM32F0_AF_TSC_G8_IO2,
	MAL_HSPEC_STM32F0_AF_TSC_G8_IO3,
	MAL_HSPEC_STM32F0_AF_TSC_G8_IO4,
	MAL_HSPEC_STM32F0_AF_TSC_G7_IO1,
	MAL_HSPEC_STM32F0_AF_TSC_G7_IO2,
	MAL_HSPEC_STM32F0_AF_TSC_G7_IO3,
	MAL_HSPEC_STM32F0_AF_TSC_G7_IO4
} mal_hspec_stm32f0_af_e;

// Device specific function mapping section.
#ifdef MAL_STM32F030R8

#include "hspec/stm/mal_hspec_stm32f030r8.h"

#define MAL_HSPEC_STM32F0_MAX_FREQUENCY	48000000

#define mal_hspec_stm32f0_get_valid_ports(ports, size) mal_hspec_stm32f030r8_get_valid_ports(ports, size)

#define mal_hspec_stm32f0_get_valid_pins(pins) mal_hspec_stm32f030r8_get_valid_pins(pins)

#elif defined(MAL_STM32F051R8)

#include "stm32f051r8/mal_hspec_stm32f051r8.h"

#define MAL_HSPEC_STM32F0_MAX_FREQUENCY	48000000

#define mal_hspec_stm32f0_get_valid_ports(ports, size) mal_hspec_stm32f051r8_get_valid_ports(ports, size)

#define mal_hspec_stm32f0_get_valid_pins(pins) mal_hspec_stm32f051r8_get_valid_pins(pins)

#define mal_hspec_stm32f0_get_valid_timers(timers, size) mal_hspec_stm32f051r8_get_valid_timers(timers, size)

#define mal_hspec_stm32f0_get_timer_update_irq(timer) mal_hspec_stm32f051r8_get_timer_update_irq(timer)

#define mal_hspec_stm32f0_get_valid_i2c_interfaces(interfaces, size) mal_hspec_stm32f051r8_get_valid_i2c_interfaces(interfaces, size)

#define mal_hspec_stm32f0_get_valid_i2c_ios(interface, scls, scls_size, sdas, sdas_size) mal_hspec_stm32f051r8_get_valid_i2c_ios(interface, scls, scls_size, sdas, sdas_size)

#define mal_hspec_stm32f0_get_port_afs(port, afs) mal_hspec_stm32f051r8_get_port_afs(port, afs)

#elif defined(MAL_STM32F072RB)

#include "stm32f072rb/mal_hspec_stm32f072rb.h"

#define MAL_HSPEC_STM32F0_MAX_FREQUENCY	48000000

#define mal_hspec_stm32f0_get_valid_ports(ports, size) mal_hspec_stm32f072rb_get_valid_ports(ports, size)

#define mal_hspec_stm32f0_get_valid_pins(pins) mal_hspec_stm32f072rb_get_valid_pins(pins)

#define mal_hspec_stm32f0_get_valid_timers(timers, size) mal_hspec_stm32f072rb_get_valid_timers(timers, size)

#define mal_hspec_stm32f0_get_timer_update_irq(timer) mal_hspec_stm32f072rb_get_timer_update_irq(timer)

#define mal_hspec_stm32f0_get_valid_i2c_interfaces(interfaces, size) mal_hspec_stm32f072rb_get_valid_i2c_interfaces(interfaces, size)

#define mal_hspec_stm32f0_get_valid_i2c_ios(interface, scls, scls_size, sdas, sdas_size) mal_hspec_stm32f072rb_get_valid_i2c_ios(interface, scls, scls_size, sdas, sdas_size)

#define mal_hspec_stm32f0_get_port_afs(port, afs) mal_hspec_stm32f072rb_get_port_afs(port, afs)

#define mal_hspec_stm32f0_get_valid_can_ios(interface, txs, txs_size, rxs, rxs_size) mal_hspec_stm32f072rb_get_valid_can_ios(interface, txs, txs_size, rxs, rxs_size)

#elif defined(MAL_STM32F030K6)
#else
#error No valid hardware specfic device symbol specified...
#endif

#define mal_hspec_stm32f0_disable_timer_interrupt(timer) do {\
	NVIC_DisableIRQ(mal_hspec_stm32f0_get_timer_update_irq(timer)); \
	__DSB(); \
	__ISB(); \
} while(0)

#define mal_hspec_stm32f0_enable_timer_interrupt(timer) do {\
	NVIC_EnableIRQ(mal_hspec_stm32f0_get_timer_update_irq(timer)); \
} while(0)

/**
 * Specific implementation of setting direction of gpio for STM.
 * \param gpio [in] The desired GPIO.
 * \param dir  [in] The desired direction.
 * \return Returns an error code.
 */
mal_error_e mal_hspec_stm32f0_gpio_init(mal_hpsec_gpio_init_s *gpio_init);

mal_error_e mal_hspec_stm32f0_set_gpio(mal_hspec_gpio_s *gpio, bool value);

mal_error_e mal_hspec_stm32f0_toggle_gpio(mal_hspec_gpio_s *gpio);

bool mal_hspec_stm32f0_get_gpio(mal_hspec_gpio_s *gpio);

void mal_hspec_stm32f0_startup();

mal_error_e mal_hspec_stm32f0_set_system_clk(const mal_hspec_system_clk_s *clk);

mal_error_e mal_hspec_stm32f0_init_tick(mal_hspec_timer_e timer, float frequency, uint8_t precision, volatile uint64_t *tick_handle);

mal_error_e mal_hspec_stm32f0_get_timer_input_clk(mal_hspec_timer_e timer, uint64_t *clock);

mal_error_e mal_hspec_stm32f0_i2c_master_init(mal_hspec_i2c_init_s *init);

mal_error_e mal_hspec_stm32f0_i2c_transfer(mal_hspec_i2c_e interface, mal_hspec_i2c_msg_s *msg);


#endif /* HSPEC_STM_MAL_HSPEC_STM32F0_H_ */
