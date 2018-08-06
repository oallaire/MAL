/*
 * mal_hspec_stm32f7_gpio.c
 *
 *  Created on: May 7, 2018
 *      Author: olivi
 */
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

#include "mal_hspec_stm32f7_gpio.h"
#include "flash/mal_flash.h"
#include "stm32f7/stm32f7xx_hal_rcc.h"
#include "power/mal_power.h"

static mal_error_e mal_hspec_stm32f7_gpio_get_speed(uint64_t speed, uint32_t *speed_setting);

GPIO_TypeDef* mal_hspec_stm32f7_gpio_get_hal_port(mal_gpio_port_e port) {
    switch (port) {
        case MAL_GPIO_PORT_A:
            return GPIOA;
        case MAL_GPIO_PORT_B:
            return GPIOB;
        case MAL_GPIO_PORT_C:
            return GPIOC;
        case MAL_GPIO_PORT_D:
            return GPIOD;
        case MAL_GPIO_PORT_E:
            return GPIOE;
        case MAL_GPIO_PORT_F:
            return GPIOF;
        case MAL_GPIO_PORT_G:
            return GPIOG;
        case MAL_GPIO_PORT_H:
        default:
            return GPIOH;
    }
}

static mal_error_e mal_hspec_stm32f7_gpio_get_speed(uint64_t speed, uint32_t *speed_setting) {
    mal_volts_t vdd;
    mal_error_e mal_result = mal_power_get_rail_voltage(MAL_POWER_RAIL_VDD, &vdd);
    if (MAL_ERROR_OK != mal_result) {
        return mal_result;
    }
    float vdd_value = MAL_TYPES_MAL_VOLTS_TO_VOLTS(vdd);
    // Assuming 10 pF
    if (vdd_value <= 1.7f) {
        if (speed >= 72500000ULL) {
            *speed_setting = GPIO_SPEED_FREQ_VERY_HIGH;
        } else if (speed >= 42500000ULL) {
            *speed_setting = GPIO_SPEED_FREQ_HIGH;
        } else if (speed >= 12500000ULL) {
            *speed_setting = GPIO_SPEED_FREQ_MEDIUM;
        } else {
            *speed_setting = GPIO_SPEED_FREQ_LOW;
        }
    } else if (vdd_value <= 1.8f) {
        if (speed >= 100000000ULL) {
            *speed_setting = GPIO_SPEED_FREQ_VERY_HIGH;
        } else if (speed >= 50000000ULL) {
            *speed_setting = GPIO_SPEED_FREQ_HIGH;
        } else if (speed >= 20000000ULL) {
            *speed_setting = GPIO_SPEED_FREQ_MEDIUM;
        } else {
            *speed_setting = GPIO_SPEED_FREQ_LOW;
        }
    } else {
        if (speed >= 180000000ULL) {
            *speed_setting = GPIO_SPEED_FREQ_VERY_HIGH;
        } else if (speed >= 100000000ULL) {
            *speed_setting = GPIO_SPEED_FREQ_HIGH;
        } else if (speed >= 50000000ULL) {
            *speed_setting = GPIO_SPEED_FREQ_MEDIUM;
        } else {
            *speed_setting = GPIO_SPEED_FREQ_LOW;
        }
    }

    return MAL_ERROR_OK;
}

mal_error_e mal_hspec_stm32f7_gpio_enable_clock(mal_gpio_port_e port) {
    switch (port) {
        case MAL_GPIO_PORT_A:
            __HAL_RCC_GPIOA_CLK_ENABLE();
            break;
        case MAL_GPIO_PORT_B:
            __HAL_RCC_GPIOB_CLK_ENABLE();
            break;
        case MAL_GPIO_PORT_C:
            __HAL_RCC_GPIOC_CLK_ENABLE();
            break;
        case MAL_GPIO_PORT_D:
            __HAL_RCC_GPIOD_CLK_ENABLE();
            break;
        case MAL_GPIO_PORT_E:
            __HAL_RCC_GPIOE_CLK_ENABLE();
            break;
        case MAL_GPIO_PORT_F:
            __HAL_RCC_GPIOF_CLK_ENABLE();
            break;
        case MAL_GPIO_PORT_G:
            __HAL_RCC_GPIOG_CLK_ENABLE();
            break;
        case MAL_GPIO_PORT_H:
            __HAL_RCC_GPIOH_CLK_ENABLE();
            break;
        default:
            return MAL_ERROR_HARDWARE_INVALID;
    }
    return MAL_ERROR_OK;
}

mal_error_e mal_gpio_init(mal_gpio_init_s *init, mal_gpio_s *handle) {
    mal_error_e mal_result;
    // Enable clock and select port
    mal_result = mal_hspec_stm32f7_gpio_enable_clock(init->port);
    if (MAL_ERROR_OK != mal_result) {
        return mal_result;
    }
    // Get port and pin
    GPIO_TypeDef *hal_port = mal_hspec_stm32f7_gpio_get_hal_port(init->port);
    uint32_t hal_pin = MAL_HSPEC_STM32F7_GPIO_GET_HAL_PIN(init->pin);
    // Configure GPIO
    GPIO_InitTypeDef hal_gpio_init;
    hal_gpio_init.Alternate = 0;
    hal_gpio_init.Pin = hal_pin;
    if (MAL_GPIO_DIRECTION_IN == init->direction) {
        hal_gpio_init.Mode = GPIO_MODE_INPUT;
    } else {
        // Select type
        if (MAL_GPIO_OUT_PP == init->output_config) {
            hal_gpio_init.Mode = GPIO_MODE_OUTPUT_PP;
        } else {
            hal_gpio_init.Mode = GPIO_MODE_OUTPUT_OD;
        }
        // Select speed. Assuming 10 pF capacitance.
        mal_result = mal_hspec_stm32f7_gpio_get_speed(init->speed, &hal_gpio_init.Speed);
        if (MAL_ERROR_OK != mal_result) {
            return mal_result;
        }
    }
    if (MAL_GPIO_PUPD_PU == init->pupd) {
        hal_gpio_init.Pull = GPIO_PULLUP;
    } else if (MAL_GPIO_PUPD_PD == init->pupd) {
        hal_gpio_init.Pull = GPIO_PULLDOWN;
    } else {
        hal_gpio_init.Pull = GPIO_NOPULL;
    }
    HAL_GPIO_Init(hal_port, &hal_gpio_init);
    // Save handle
    handle->hal_port = hal_port;
    handle->hal_pin = hal_pin;

    return MAL_ERROR_OK;
}

mal_error_e mal_gpio_set(mal_gpio_s *gpio, bool value) {
    HAL_GPIO_WritePin(gpio->hal_port, (uint16_t)gpio->hal_pin, (GPIO_PinState)value);
    return MAL_ERROR_OK;
}

mal_error_e mal_gpio_get(mal_gpio_s *gpio, bool *value) {
    GPIO_PinState hal_pin_state = HAL_GPIO_ReadPin(gpio->hal_port, (uint16_t)gpio->hal_pin);
    if (GPIO_PIN_SET == hal_pin_state) {
        *value = true;
    } else {
        *value = false;
    }
    return MAL_ERROR_OK;
}

mal_error_e mal_gpio_toggle(mal_gpio_s *gpio) {
    HAL_GPIO_TogglePin(gpio->hal_port, (uint16_t)gpio->hal_pin);
    return MAL_ERROR_OK;
}

mal_error_e mal_gpio_deinit(mal_gpio_s *gpio) {
    HAL_GPIO_DeInit(gpio->hal_port, gpio->hal_pin);
    return MAL_ERROR_OK;
}

mal_error_e mal_hspec_stm32f7_gpio_init_alternate(mal_gpio_port_e port, uint8_t pin, uint32_t alternate) {
    mal_error_e mal_result;
    // Enable IO clock
    mal_result = mal_hspec_stm32f7_gpio_enable_clock(port);
    if (MAL_ERROR_OK != mal_result) {
        return mal_result;
    }
    // Set IO in alternate mode
    GPIO_TypeDef *hal_port = mal_hspec_stm32f7_gpio_get_hal_port(port);
    GPIO_InitTypeDef gpio_init;
    gpio_init.Mode = GPIO_MODE_AF_PP;
    gpio_init.Pull = GPIO_NOPULL;
    gpio_init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    gpio_init.Pin = MAL_HSPEC_STM32F7_GPIO_GET_HAL_PIN(pin);
    gpio_init.Alternate = alternate;
    HAL_GPIO_Init(hal_port, &gpio_init);

    return MAL_ERROR_OK;
}
