/*
 * mal_hspec_mingw_timer.c
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

#include "mal_hspec_mingw_timer.h"
#include "std/mal_bool.h"
#include "std/mal_stdlib.h"
#include "std/mal_stdint.h"
#include <windows.h> // Leave this include last, windows defines the word interface and it creates conflicts.

typedef struct {
	mal_timer_e timer;
	float frequency;
	mal_timer_callback_t callack;
	HANDLE thread;
	volatile bool active;
	volatile uint32_t count;
	mal_timer_input_capture_callback_t intput_capture_cb[MAL_GPIO_PORT_SIZE][PORT_SIZE];
} mingw_timer_s;

static DWORD WINAPI timer_thread(LPVOID lpParameter);
static mal_error_e count_timer_callback(mal_timer_e timer);

static mal_timer_e available_timers[MAL_TIMER_SIZE];
static mingw_timer_s mingw_timers[MAL_TIMER_SIZE];

mal_error_e mal_timer_get_valid_timers(const mal_timer_e **timers, uint8_t *size) {
	// Initialize timers
	for (int i = 0; i < MAL_TIMER_SIZE; i++) {
		available_timers[i] = i;
	}
	// Set values
	*timers = available_timers;
	*size = MAL_TIMER_SIZE;
	return MAL_ERROR_OK;
}

mal_error_e mal_timer_init(mal_timer_init_s *init) {
	// Save timer parameters
	mingw_timers[init->timer].timer = init->timer;
	mingw_timers[init->timer].frequency = MAL_TYPES_MAL_HERTZ_TO_HERTZ(init->frequency);
	mingw_timers[init->timer].callack = init->callback;
	// Set timer active
	mingw_timers[init->timer].active = true;
	// Create thread
	mingw_timers[init->timer].thread = CreateThread(NULL, /*Default security settings*/
											        0, /*Default stack size*/
											        &timer_thread,
											        &mingw_timers[init->timer],
											        0 /*Thread starts now*/,
											        NULL /*No thread identifier*/);
	if (NULL == mingw_timers[init->timer].thread) {
		return MAL_ERROR_INIT_FAILED;
	}

	return MAL_ERROR_OK;
}

static DWORD WINAPI timer_thread(LPVOID lpParameter) {
	// Save timer
	mingw_timer_s *timer = (mingw_timer_s*)lpParameter;
	// Compute sleep time
	DWORD sleep_time;
	float period = 1.0f / timer->frequency;
	sleep_time = (long)(1000.0f * period);
	// Run loop
	while (timer->active) {
		// Sleep
		Sleep(sleep_time);
		// Execute callback
		if (timer->active && NULL != timer->callack) {
			timer->callack(timer->timer);
		}
	}
	return 0;
}

mal_error_e mal_timer_free_unmanaged(mal_timer_e timer) {
	// Set timer as not active
	mingw_timers[timer].active = false;
	// Join thread
	WaitForSingleObject(mingw_timers[timer].thread, INFINITE);

	return MAL_ERROR_OK;
}

mal_error_e mal_timer_init_count_unmanaged(mal_timer_e timer, mal_hertz_t frequency) {
    mal_timer_init_s init;
    init.timer = timer;
    init.frequency = frequency;
    init.delta = 0;
    init.callback = &count_timer_callback;
	return mal_timer_init(&init);
}

static mal_error_e count_timer_callback(mal_timer_e timer) {
	mingw_timers[timer].count++;
	return MAL_ERROR_OK;
}

mal_error_e mal_timer_get_resolution(mal_timer_e timer, uint8_t *resolution) {
	*resolution = 32;
	return MAL_ERROR_OK;
}

mal_error_e mal_timer_get_count_frequency(mal_timer_e timer, mal_hertz_t *frequency) {
	*frequency = MAL_TYPES_HERTZ_TO_MAL_HERTZ(mingw_timers[timer].frequency);
	return MAL_ERROR_OK;
}

mal_error_e mal_timer_get_count(mal_timer_e timer, uint64_t *count) {
	*count = mingw_timers[timer].count;
	return MAL_ERROR_OK;
}

mal_error_e mal_timer_init_input_capture_unmanaged(mal_timer_intput_capture_init_s *init) {
	// Save timer parameters
	mingw_timers[init->timer].timer = init->timer;
	mingw_timers[init->timer].frequency = init->frequency;
	mingw_timers[init->timer].intput_capture_cb[init->input_io->port][init->input_io->pin] = init->callback;
	// Set timer active
	mingw_timers[init->timer].active = true;

	return MAL_ERROR_OK;
}

mal_error_e mal_hspec_mingw_timer_do_input_capture_callback(mal_timer_e timer, const mal_gpio_s *io, uint64_t value) {
	return mingw_timers[timer].intput_capture_cb[io->port][io->pin](timer, value);
}
