/*
 * mal_hspec_stm32f0_timer.c
 *
 *  Created on: Jun 24, 2015
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

#include "mal_hspec_stm32f0_timer.h"
#include "std/mal_stdlib.h"
#include "std/mal_math.h"
#include "stm32f0/stm32f0xx_rcc.h"
#include "mal_hspec_stm32f0_gpio.h"
#include "timer/mal_timer.h"

#define INVOKE_TASK_CALLBACK(timer) do { \
	if (timer_callbacks[timer].task_cb != NULL) { \
		timer_callbacks[timer].task_cb(timer); \
	} \
}while(0)

typedef union {
	mal_hspec_timer_callback_t task_cb;
	mal_hspec_timer_input_capture_callback_t ic_cb;
} timer_callback_u;

static mal_error_e init_timer_rcc(mal_hspec_timer_e timer);

static uint32_t get_rcc_timer(mal_hspec_timer_e timer);

static TIM_TypeDef* get_timer_typedef(mal_hspec_timer_e timer);

static uint16_t get_timer_channel(const mal_hspec_gpio_s *gpio, mal_hspec_timer_e timer);

static void timer_input_capture_interrupt(mal_hspec_timer_e timer, TIM_TypeDef *stm_timer, uint16_t flags);

static timer_callback_u timer_callbacks[MAL_HSPEC_TIMER_SIZE];

mal_error_e mal_hspec_stm32f0_timer_init(mal_hspec_timer_e timer, float frequency, float delta, mal_hspec_timer_callback_t callback) {
	mal_error_e result;
	// Initialise peripheral clock
	result = init_timer_rcc(timer);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	// Get timer
	TIM_TypeDef *tim = get_timer_typedef(timer);
	// Get timer frequency
	uint64_t timer_frequency;
	result = mal_hspec_stm32f0_timer_get_input_clk(timer, &timer_frequency);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	// Computations will be done in integers for speed. We will use uHz for the current precision
	uint64_t uhz_frequency = frequency * 1000000.0f;
	if (uhz_frequency == 0) {
		return MAL_ERROR_CLOCK_ERROR;
	}
	uint64_t uhz_delta = delta * 1000000.0f;
	// Initialise time base timer
	TIM_TimeBaseInitTypeDef params;
	TIM_TimeBaseStructInit(&params);
	params.TIM_CounterMode = TIM_CounterMode_Up;
	// Choose longest count to get maximum precision
	uint16_t prescaler;
	uint16_t period;
	bool found = false;
	uint64_t smallest_delta = UINT64_MAX;
	for (period = UINT16_MAX; period > 0; period--) {
		for (prescaler = 1; prescaler <= UINT16_MAX; prescaler++) {
			// Resolution is in uHz to handle fractions of Hz
			uint64_t potential_frequency = (timer_frequency * 1000000) / ((uint64_t)period * (uint64_t)prescaler);
			uint64_t actual_delta;
			if (potential_frequency >= uhz_frequency) {
				actual_delta = potential_frequency - uhz_frequency;
			} else {
				actual_delta = uhz_frequency - potential_frequency;
			}
			if (actual_delta <= uhz_delta) {
				found = true;
				break;
			} else {
				if (actual_delta < smallest_delta) {
					smallest_delta = actual_delta;
				}
			}
			// If we get here and the potential frequency is smaller than the
			// target frequency, we can change the period as the potential
			// frequency is just gonna keep getting smaller if we continue
			if (potential_frequency < uhz_frequency) {
				break;
			}
		}
		if (found) {
			break;
		}
	}
	if (!found) {
		return MAL_ERROR_HARDWARE_INVALID;
	}
	params.TIM_Prescaler = prescaler - 1;
	params.TIM_Period = period;
	TIM_TimeBaseInit(tim, &params);
	// Save tick handle
	timer_callbacks[timer].task_cb = callback;
	// Enable NVIC interrupt for timer
	if (NULL != callback) {
		IRQn_Type irq = mal_hspec_stm32f0_get_timer_update_irq(timer);
		NVIC_EnableIRQ(irq);
		NVIC_SetPriority(irq, 2); // Find a way to manage priorities for interrupts
		// Enable timer interrupt
		TIM_ITConfig(tim, TIM_IT_Update, ENABLE);
	}
	// Enable timer
	TIM_Cmd(tim, ENABLE);

	return MAL_ERROR_OK;
}

static mal_error_e init_timer_rcc(mal_hspec_timer_e timer) {
switch (timer) {
case MAL_HSPEC_TIMER_1:
case MAL_HSPEC_TIMER_15:
case MAL_HSPEC_TIMER_16:
case MAL_HSPEC_TIMER_17:
	RCC_APB2PeriphClockCmd(get_rcc_timer(timer), ENABLE);
	break;
case MAL_HSPEC_TIMER_2:
case MAL_HSPEC_TIMER_3:
case MAL_HSPEC_TIMER_6:
case MAL_HSPEC_TIMER_7:
case MAL_HSPEC_TIMER_14:
default:
	RCC_APB1PeriphClockCmd(get_rcc_timer(timer), ENABLE);
	break;
}
return MAL_ERROR_OK;
}

static uint32_t get_rcc_timer(mal_hspec_timer_e timer) {
switch (timer) {
case MAL_HSPEC_TIMER_1:
	return RCC_APB2Periph_TIM1;
case MAL_HSPEC_TIMER_2:
	return RCC_APB1Periph_TIM2;
case MAL_HSPEC_TIMER_3:
	return RCC_APB1Periph_TIM3;
case MAL_HSPEC_TIMER_6:
	return RCC_APB1Periph_TIM6;
case MAL_HSPEC_TIMER_7:
	return RCC_APB1Periph_TIM7;
case MAL_HSPEC_TIMER_14:
	return RCC_APB1Periph_TIM14;
case MAL_HSPEC_TIMER_15:
	return RCC_APB2Periph_TIM15;
case MAL_HSPEC_TIMER_16:
	return RCC_APB2Periph_TIM16;
case MAL_HSPEC_TIMER_17:
default:
	return RCC_APB2Periph_TIM17;
}
}

static TIM_TypeDef* get_timer_typedef(mal_hspec_timer_e timer) {
	switch (timer) {
	case MAL_HSPEC_TIMER_1:
		return TIM1;
	case MAL_HSPEC_TIMER_2:
		return TIM2;
	case MAL_HSPEC_TIMER_3:
		return TIM3;
	case MAL_HSPEC_TIMER_6:
		return TIM6;
	case MAL_HSPEC_TIMER_7:
		return TIM7;
	case MAL_HSPEC_TIMER_14:
		return TIM14;
	case MAL_HSPEC_TIMER_15:
		return TIM15;
	case MAL_HSPEC_TIMER_16:
		return TIM16;
	case MAL_HSPEC_TIMER_17:
		return TIM17;
	default:
		return NULL;
	}
}

mal_error_e mal_hspec_stm32f0_timer_get_input_clk(mal_hspec_timer_e timer,
	uint64_t *clock) {
	// MCU clocks
	RCC_ClocksTypeDef clocks;
	RCC_GetClocksFreq(&clocks);
	// APB Prescaler
	uint32_t apb_pre = RCC->CFGR & RCC_CFGR_PPRE;

	*clock = clocks.PCLK_Frequency;
	if (apb_pre != RCC_CFGR_PPRE_DIV1) {
		// Times 2, see clock tree in datasheet
		*clock += *clock;
	}
	return MAL_ERROR_OK;
}

void TIM1_BRK_UP_TRG_COM_IRQHandler(void) {
	// Check if this is a capture interrupt
	uint16_t cc_flags = TIM1->SR & TIM_FLAG_CC1;
	cc_flags |= TIM1->SR & TIM_FLAG_CC2;
	cc_flags |= TIM1->SR & TIM_FLAG_CC3;
	cc_flags |= TIM1->SR & TIM_FLAG_CC4;
	// Clear interrupts
	TIM_ClearFlag(TIM1, TIM_FLAG_Update|TIM_FLAG_CC1|TIM_FLAG_CC2|TIM_FLAG_CC3|TIM_FLAG_CC4);
	// Handles interrupt
	if (cc_flags) {
		timer_input_capture_interrupt(MAL_HSPEC_TIMER_1, TIM1, cc_flags);
	} else {
		INVOKE_TASK_CALLBACK(MAL_HSPEC_TIMER_1);
	}
}

void TIM2_IRQHandler(void) {
	// Check if this is a capture interrupt
	uint16_t cc_flags = TIM2->SR & TIM_FLAG_CC1;
	cc_flags |= TIM2->SR & TIM_FLAG_CC2;
	cc_flags |= TIM2->SR & TIM_FLAG_CC3;
	cc_flags |= TIM2->SR & TIM_FLAG_CC4;
	// Clear interrupts
	TIM_ClearFlag(TIM2, TIM_FLAG_Update|TIM_FLAG_CC1|TIM_FLAG_CC2|TIM_FLAG_CC3|TIM_FLAG_CC4);
	// Handles interrupt
	if (cc_flags) {
		timer_input_capture_interrupt(MAL_HSPEC_TIMER_2, TIM2, cc_flags);
	} else {
		INVOKE_TASK_CALLBACK(MAL_HSPEC_TIMER_2);
	}
}

void TIM3_IRQHandler(void) {
	// Check if this is a capture interrupt
	uint16_t cc_flags = TIM3->SR & TIM_FLAG_CC1;
	cc_flags |= TIM3->SR & TIM_FLAG_CC2;
	cc_flags |= TIM3->SR & TIM_FLAG_CC3;
	cc_flags |= TIM3->SR & TIM_FLAG_CC4;
	// Clear interrupts
	TIM_ClearFlag(TIM3, TIM_FLAG_Update|TIM_FLAG_CC1|TIM_FLAG_CC2|TIM_FLAG_CC3|TIM_FLAG_CC4);
	// Handles interrupt
	if (cc_flags) {
		timer_input_capture_interrupt(MAL_HSPEC_TIMER_3, TIM3, cc_flags);
	} else {
		INVOKE_TASK_CALLBACK(MAL_HSPEC_TIMER_3);
	}
}

void TIM6_DAC_IRQHandler(void) {
	TIM_ClearFlag(TIM6, TIM_FLAG_Update);
	// Handles tick
	INVOKE_TASK_CALLBACK(MAL_HSPEC_TIMER_6);
}

void TIM7_IRQHandler(void) {
	TIM_ClearFlag(TIM7, TIM_FLAG_Update);
	// Handles tick
	INVOKE_TASK_CALLBACK(MAL_HSPEC_TIMER_7);
}

void TIM14_IRQHandler(void) {
	// Check if this is a capture interrupt
	uint16_t cc_flags = TIM14->SR & TIM_FLAG_CC1;
	// Clear interrupts
	TIM_ClearFlag(TIM14, TIM_FLAG_Update|TIM_FLAG_CC1);
	// Handles interrupt
	if (cc_flags) {
		timer_input_capture_interrupt(MAL_HSPEC_TIMER_14, TIM14, cc_flags);
	} else {
		INVOKE_TASK_CALLBACK(MAL_HSPEC_TIMER_14);
	}
}

void TIM15_IRQHandler(void) {
	// Check if this is a capture interrupt
	uint16_t cc_flags = TIM15->SR & TIM_FLAG_CC1;
	cc_flags |= TIM15->SR & TIM_FLAG_CC2;
	// Clear interrupts
	TIM_ClearFlag(TIM15, TIM_FLAG_Update|TIM_FLAG_CC1|TIM_FLAG_CC2);
	// Handles interrupt
	if (cc_flags) {
		timer_input_capture_interrupt(MAL_HSPEC_TIMER_15, TIM15, cc_flags);
	} else {
		INVOKE_TASK_CALLBACK(MAL_HSPEC_TIMER_15);
	}
}

void TIM16_IRQHandler(void) {
	// Check if this is a capture interrupt
	uint16_t cc_flags = TIM16->SR & TIM_FLAG_CC1;
	// Clear interrupts
	TIM_ClearFlag(TIM16, TIM_FLAG_Update|TIM_FLAG_CC1);
	// Handles interrupt
	if (cc_flags) {
		timer_input_capture_interrupt(MAL_HSPEC_TIMER_16, TIM16, cc_flags);
	} else {
		INVOKE_TASK_CALLBACK(MAL_HSPEC_TIMER_16);
	}
}

void TIM17_IRQHandler(void) {
	// Check if this is a capture interrupt
	uint16_t cc_flags = TIM17->SR & TIM_FLAG_CC1;
	// Clear interrupts
	TIM_ClearFlag(TIM17, TIM_FLAG_Update|TIM_FLAG_CC1);
	// Handles interrupt
	if (cc_flags) {
		timer_input_capture_interrupt(MAL_HSPEC_TIMER_17, TIM17, cc_flags);
	} else {
		INVOKE_TASK_CALLBACK(MAL_HSPEC_TIMER_17);
	}
}

mal_error_e mal_hspec_stm32f0_timer_free(mal_hspec_timer_e timer) {
	// Get timer
	TIM_TypeDef *tim = get_timer_typedef(timer);
	// Disable timer
	TIM_Cmd(tim, DISABLE);
	// Disable interrupt
	TIM_ITConfig(tim, TIM_IT_Update|TIM_FLAG_CC1|TIM_FLAG_CC2|TIM_FLAG_CC3|TIM_FLAG_CC4, DISABLE);
	NVIC_DisableIRQ(mal_hspec_stm32f0_get_timer_update_irq(timer));
	NVIC_DisableIRQ(mal_hspec_stm32f0_get_timer_compare_irq(timer));
	// Unitialise timer
	TIM_DeInit(tim);
	// Remove callback
	timer_callbacks[timer].task_cb = NULL;
	timer_callbacks[timer].ic_cb = NULL;

	return MAL_ERROR_OK;
}

bool mal_hspec_stm32f0_disable_timer_interrupt(mal_hspec_timer_e timer) {
	IRQn_Type irq = mal_hspec_stm32f0_get_timer_update_irq(timer);
	bool active = NVIC_GetActive(irq);
	NVIC_DisableIRQ(irq);
	__DSB();
	__ISB();
	return active;
}

static uint16_t get_timer_channel(const mal_hspec_gpio_s *gpio, mal_hspec_timer_e timer) {
	const mal_hspec_stm32f0_af_e (*timer_afs)[MAL_HSPEC_GPIO_PORT_SIZE][MAL_HSPEC_STM32F0_GPIO_PORT_SIZE][MAL_HSPEC_TIMER_SIZE];
	// Fetch timer alternate functions
	mal_hspec_stm32f0_get_timer_afs(&timer_afs);
	switch ((*timer_afs)[gpio->port][gpio->pin][timer]) {
		case MAL_HSPEC_STM32F0_AF_TIM1_CH1:
		case MAL_HSPEC_STM32F0_AF_TIM1_CH1N:
		case MAL_HSPEC_STM32F0_AF_TIM3_CH1:
		case MAL_HSPEC_STM32F0_AF_TIM14_CH1:
		case MAL_HSPEC_STM32F0_AF_TIM15_CH1:
		case MAL_HSPEC_STM32F0_AF_TIM15_CH1N:
		case MAL_HSPEC_STM32F0_AF_TIM16_CH1:
		case MAL_HSPEC_STM32F0_AF_TIM16_CH1N:
		case MAL_HSPEC_STM32F0_AF_TIM17_CH1:
		case MAL_HSPEC_STM32F0_AF_TIM17_CH1N:
			return TIM_Channel_1;
		case MAL_HSPEC_STM32F0_AF_TIM1_CH2:
		case MAL_HSPEC_STM32F0_AF_TIM1_CH2N:
		case MAL_HSPEC_STM32F0_AF_TIM2_CH2:
		case MAL_HSPEC_STM32F0_AF_TIM3_CH2:
		case MAL_HSPEC_STM32F0_AF_TIM15_CH2:
			return TIM_Channel_2;
		case MAL_HSPEC_STM32F0_AF_TIM1_CH3:
		case MAL_HSPEC_STM32F0_AF_TIM1_CH3N:
		case MAL_HSPEC_STM32F0_AF_TIM2_CH3:
		case MAL_HSPEC_STM32F0_AF_TIM3_CH3:
			return TIM_Channel_3;
		case MAL_HSPEC_STM32F0_AF_TIM1_CH4:
		case MAL_HSPEC_STM32F0_AF_TIM2_CH4:
		case MAL_HSPEC_STM32F0_AF_TIM3_CH4:
			return TIM_Channel_4;
		default:
			return UINT16_MAX;
	}
}

mal_error_e mal_hspec_stm32f0_timer_get_resolution(mal_hspec_timer_e timer, uint8_t *resolution) {
	switch (timer) {
		case MAL_HSPEC_TIMER_1:
		case MAL_HSPEC_TIMER_3:
		case MAL_HSPEC_TIMER_6:
		case MAL_HSPEC_TIMER_7:
		case MAL_HSPEC_TIMER_14:
		case MAL_HSPEC_TIMER_15:
		case MAL_HSPEC_TIMER_16:
		case MAL_HSPEC_TIMER_17:
			*resolution = 16;
			break;
		case MAL_HSPEC_TIMER_2:
			*resolution = 32;
			break;
		default:
			return MAL_ERROR_HARDWARE_INVALID;
	}
	return MAL_ERROR_OK;
}

mal_error_e mal_hspec_stm32f0_timer_pwm_init(mal_hspec_timer_pwm_init_s *init) {
	mal_error_e result;
	// Get timer
	TIM_TypeDef *tim = get_timer_typedef(init->timer);
	// Check if the timer is already initialized
	if (!(tim->CR1 & TIM_CR1_CEN)) {
		// Initialize timer
		result = mal_hspec_stm32f0_timer_init(init->timer, init->frequency, init->delta, NULL);
		if (MAL_ERROR_OK != result) {
			return result;
		}
	} else {
		// Timer is already initialized check if parameters match
		mal_timer_state_s state;
		result = mal_timer_get_state(init->timer, &state);
		if (MAL_ERROR_OK != result) {
			return result;
		}
		// Check frequency and delta
		if (init->frequency != state.frequency || init->delta != state.delta) {
			return MAL_ERROR_HARDWARE_UNAVAILABLE;
		}
	}
	// Enable GPIO clock
	RCC_AHBPeriphClockCmd(mal_hspec_stm32f0_get_rcc_gpio_port(init->pwm_io->port), ENABLE);
	// Configure alternate function
	uint8_t function;
	result = mal_hspec_stm32f0_get_timer_af(init->pwm_io, init->timer, &function);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	GPIO_PinAFConfig(mal_hspec_stm32f0_get_gpio_typedef(init->pwm_io->port), init->pwm_io->pin, function);
	// Configure GPIO
	GPIO_InitTypeDef gpio_init;
	gpio_init.GPIO_Pin = MAL_HSPEC_STM32F0_GET_GPIO_PIN(init->pwm_io->pin);
	gpio_init.GPIO_Mode = GPIO_Mode_AF;
	gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
	gpio_init.GPIO_OType = GPIO_OType_PP;
	gpio_init.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(mal_hspec_stm32f0_get_gpio_typedef(init->pwm_io->port), &gpio_init);
	// Disable timer while we set the PWM output
	TIM_Cmd(tim, DISABLE);
	// Configure output channel
	TIM_OCInitTypeDef channel_init;
	channel_init.TIM_OCMode = TIM_OCMode_PWM1; // Output is active up to compare
	channel_init.TIM_OutputState = TIM_OutputState_Enable;
	channel_init.TIM_OutputNState = TIM_OutputNState_Enable;
	channel_init.TIM_Pulse = 0; // Duty cycle of 0 at init.
	channel_init.TIM_OCPolarity = TIM_OCPolarity_High;
	channel_init.TIM_OCNPolarity = TIM_OCNPolarity_High;
	channel_init.TIM_OCIdleState = TIM_OCIdleState_Reset;
	channel_init.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
	// Chose the correct init function
	switch (get_timer_channel(init->pwm_io, init->timer)) {
		case TIM_Channel_1:
			TIM_OC1Init(tim, &channel_init);
			break;
		case TIM_Channel_2:
			TIM_OC2Init(tim, &channel_init);
			break;
		case TIM_Channel_3:
			TIM_OC3Init(tim, &channel_init);
			break;
		case TIM_Channel_4:
			TIM_OC4Init(tim, &channel_init);
			break;
		default:
			return MAL_ERROR_HARDWARE_INVALID;
	}
	// Enable timer
	TIM_Cmd(tim, ENABLE);
	// Check if output has to be enabled
	switch (init->timer) {
		case MAL_HSPEC_TIMER_1:
		case MAL_HSPEC_TIMER_15:
		case MAL_HSPEC_TIMER_16:
		case MAL_HSPEC_TIMER_17:
			TIM_CtrlPWMOutputs(tim, ENABLE);
			break;
		default:
			// Nothing to do for other timers
			break;
	}

	return MAL_ERROR_OK;
}

mal_error_e mal_hspec_stm32f0_timer_set_pwm_duty_cycle(mal_hspec_timer_e timer, const mal_hspec_gpio_s *gpio, float duty_cycle) {
	mal_error_e result;
	// Get timer
	TIM_TypeDef *tim = get_timer_typedef(timer);
	// We need to compute the duty cycle
	uint32_t compare_value;
	if (duty_cycle >= 1.0f) {
		// Get timer resolution to get max compare value
		uint8_t resolution;
		result = mal_hspec_stm32f0_timer_get_resolution(timer, &resolution);
		if (MAL_ERROR_OK != result) {
			return result;
		}
		uint32_t max_value = (((uint64_t)1) << ((uint64_t)resolution)) - (uint64_t)1;
		if (tim->ARR < max_value) {
			compare_value = tim->ARR + 1;
		} else {
			compare_value = tim->ARR;
		}
	} else {
		compare_value = (float)tim->ARR * duty_cycle;
	}
	// Chose the correct set function
	switch (get_timer_channel(gpio, timer)) {
		case TIM_Channel_1:
			TIM_SetCompare1(tim, compare_value);
			break;
		case TIM_Channel_2:
			TIM_SetCompare2(tim, compare_value);
			break;
		case TIM_Channel_3:
			TIM_SetCompare3(tim, compare_value);
			break;
		case TIM_Channel_4:
			TIM_SetCompare4(tim, compare_value);
			break;
		default:
			return MAL_ERROR_HARDWARE_INVALID;
	}

	return MAL_ERROR_OK;
}

mal_error_e mal_hspec_stm32f0_timer_count_init(mal_hspec_timer_e timer, float frequency) {
	mal_error_e result;
	// Initialize peripheral clock
	result = init_timer_rcc(timer);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	// Get timer
	TIM_TypeDef *tim = get_timer_typedef(timer);
	// Get timer frequency
	uint64_t timer_frequency;
	result = mal_hspec_stm32f0_timer_get_input_clk(timer, &timer_frequency);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	// Initialize time base timer
	TIM_TimeBaseInitTypeDef params;
	TIM_TimeBaseStructInit(&params);
	params.TIM_CounterMode = TIM_CounterMode_Up;
	// Compute prescaler
	if (0.0f == frequency) {
		return MAL_ERROR_CLOCK_ERROR;
	}
	uint32_t prescaler = (float)timer_frequency / frequency;
	if (prescaler > (UINT16_MAX+ 1)) {
		return MAL_ERROR_CLOCK_ERROR;
	}
	// Round up prescaler to at least 1
	if (0 == prescaler) {
		prescaler += 1;
	}
	params.TIM_Prescaler = prescaler - 1;
	// Set maximum value
	uint8_t resolution;
	result = mal_hspec_stm32f0_timer_get_resolution(timer, &resolution);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	uint32_t max_value = (((uint64_t)1) << ((uint64_t)resolution)) - (uint64_t)1;
	params.TIM_Period = max_value;

	TIM_TimeBaseInit(tim, &params);
	// Enable timer
	TIM_Cmd(tim, ENABLE);

	return MAL_ERROR_OK;
}

mal_error_e mal_hspec_stm32f0_timer_get_count_frequency(mal_hspec_timer_e timer, float *frequency) {
	mal_error_e result;
	// Get timer
	TIM_TypeDef *tim = get_timer_typedef(timer);
	if (NULL == tim) {
		return MAL_ERROR_HARDWARE_INVALID;
	}
	// Get timer frequency
	uint64_t timer_frequency;
	result = mal_hspec_stm32f0_timer_get_input_clk(timer, &timer_frequency);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	// Compute frequency
	float prescaler_value = (uint32_t)tim->PSC + 1;
	*frequency = (float)timer_frequency / prescaler_value;

	return MAL_ERROR_OK;
}

mal_error_e mal_hspec_stm32f0_timer_get_count(mal_hspec_timer_e timer, uint64_t *count) {
	// Get timer
	TIM_TypeDef *tim = get_timer_typedef(timer);
	if (NULL == tim) {
		return MAL_ERROR_HARDWARE_INVALID;
	}
	// Get value
	*count = tim->CNT;

	return MAL_ERROR_OK;
}

mal_error_e mal_hspec_stm32f0_timer_input_capture_init(mal_hspec_timer_intput_capture_init_s *init) {
	mal_error_e result;
	// Get timer
	TIM_TypeDef *tim = get_timer_typedef(init->timer);
	// Check if the timer is already initialized
	if (!(tim->CR1 & TIM_CR1_CEN)) {
		// Initialize timer
		result = mal_hspec_stm32f0_timer_count_init(init->timer, init->frequency);
		if (MAL_ERROR_OK != result) {
			return result;
		}
	} else {
		// Timer is already initialized check if parameters match
		mal_timer_state_s state;
		result = mal_timer_get_state(init->timer, &state);
		if (MAL_ERROR_OK != result) {
			return result;
		}
		// Check frequency
		if (init->frequency != state.frequency) {
			return MAL_ERROR_HARDWARE_UNAVAILABLE;
		}
	}
	// Enable GPIO clock
	RCC_AHBPeriphClockCmd(mal_hspec_stm32f0_get_rcc_gpio_port(init->input_io->port), ENABLE);
	// Configure alternate function
	uint8_t function;
	result = mal_hspec_stm32f0_get_timer_af(init->input_io, init->timer, &function);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	GPIO_PinAFConfig(mal_hspec_stm32f0_get_gpio_typedef(init->input_io->port), init->input_io->pin, function);
	// Configure GPIO
	GPIO_InitTypeDef gpio_init;
	gpio_init.GPIO_Pin = MAL_HSPEC_STM32F0_GET_GPIO_PIN(init->input_io->pin);
	gpio_init.GPIO_Mode = GPIO_Mode_AF;
	gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
	gpio_init.GPIO_OType = GPIO_OType_PP;
	gpio_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(mal_hspec_stm32f0_get_gpio_typedef(init->input_io->port), &gpio_init);
	// Disable timer while we set the PWM output
	TIM_Cmd(tim, DISABLE);
	// Get timer channel
	uint16_t timer_channel = get_timer_channel(init->input_io, init->timer);
	// Configure input channel
	TIM_ICInitTypeDef input_capture_init;
	TIM_ICStructInit(&input_capture_init);
	input_capture_init.TIM_ICSelection = TIM_ICSelection_DirectTI;
	input_capture_init.TIM_ICFilter = 0; // No filter
	// Set channel
	input_capture_init.TIM_Channel = timer_channel;
	// Set requested polarity
	switch (init->input_event) {
		case MAL_HSPEC_TIMER_INPUT_FALLING:
			input_capture_init.TIM_ICPolarity = TIM_ICPolarity_Falling;
			break;
		case MAL_HSPEC_TIMER_INPUT_RISING:
			input_capture_init.TIM_ICPolarity = TIM_ICPolarity_Rising;
			break;
		case MAL_HSPEC_TIMER_INPUT_BOTH:
		default:
			input_capture_init.TIM_ICPolarity = TIM_ICPolarity_BothEdge;
			break;
	}
	// Set input prescaler
	switch (init->input_divider) {
		case 1:
			input_capture_init.TIM_ICPrescaler = TIM_ICPSC_DIV1;
			break;
		case 2:
			input_capture_init.TIM_ICPrescaler = TIM_ICPSC_DIV2;
			break;
		case 4:
			input_capture_init.TIM_ICPrescaler = TIM_ICPSC_DIV4;
			break;
		case 8:
			input_capture_init.TIM_ICPrescaler = TIM_ICPSC_DIV8;
			break;
		default:
			return MAL_ERROR_HARDWARE_INVALID;
	}
	// Initialize input
	TIM_ICInit(tim, &input_capture_init);
	// Save callback
	timer_callbacks[init->timer].ic_cb = init->callback;
	// Enable NVIC interrupt for timer
	if (NULL != init->callback) {
		IRQn_Type irq = mal_hspec_stm32f0_get_timer_compare_irq(init->timer);
		NVIC_EnableIRQ(irq);
		NVIC_SetPriority(irq, 2); // MAL Issue #19 Find a way to manage priorities for interrupts
		// Enable timer interrupt
		uint16_t tim_interrupt;
		switch (timer_channel) {
			case TIM_Channel_1:
				tim_interrupt = TIM_IT_CC1;
				break;
			case TIM_Channel_2:
				tim_interrupt = TIM_IT_CC2;
				break;
			case TIM_Channel_3:
				tim_interrupt = TIM_IT_CC3;
				break;
			case TIM_Channel_4:
			default:
				tim_interrupt = TIM_IT_CC4;
				break;
		}
		TIM_ITConfig(tim, tim_interrupt, ENABLE);
	}
	// Enable timer
	TIM_Cmd(tim, ENABLE);

	return MAL_ERROR_OK;
}

static void timer_input_capture_interrupt(mal_hspec_timer_e timer, TIM_TypeDef *stm_timer, uint16_t flags) {
	// Check every channel and execute callbacks
	if ((flags & TIM_IT_CC1) && (stm_timer->DIER & TIM_DIER_CC1IE)) {
		if (timer_callbacks[timer].ic_cb != NULL) {
			timer_callbacks[timer].ic_cb(timer, stm_timer->CCR1);
		}
	}
	if ((flags & TIM_IT_CC2) && (stm_timer->DIER & TIM_DIER_CC2IE)) {
		if (timer_callbacks[timer].ic_cb != NULL) {
			timer_callbacks[timer].ic_cb(timer, stm_timer->CCR2);
		}
	}
	if ((flags & TIM_IT_CC3) && (stm_timer->DIER & TIM_DIER_CC3IE)) {
		if (timer_callbacks[timer].ic_cb != NULL) {
			timer_callbacks[timer].ic_cb(timer, stm_timer->CCR3);
		}
	}
	if ((flags & TIM_IT_CC4) && (stm_timer->DIER & TIM_DIER_CC4IE)) {
		if (timer_callbacks[timer].ic_cb != NULL) {
			timer_callbacks[timer].ic_cb(timer, stm_timer->CCR4);
		}
	}
}
