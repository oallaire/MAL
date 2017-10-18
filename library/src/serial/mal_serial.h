/**
 * @file mal_serial.h
 * @author Olivier Allaire
 * @date May 30 2015
 * @copyright Copyright (c) 2015 Olivier Allaire
 * @par This file is part of MAL.
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
 * @brief Functions to access and configure the serial interfaces of the MCU.
 */

#ifndef SERIAL_MAL_SERIAL_H_
#define SERIAL_MAL_SERIAL_H_

#include "std/mal_error.h"
#include "std/mal_stdint.h"
#include "gpio/mal_gpio.h"

/**
 * @defgroup Serial
 * @brief @copybrief mal_serial.h
 * @{
 */

/**
 * Possible serial ports.
 */
typedef enum {
    MAL_SERIAL_PORT_1 = 0,  //!< Port 1
    MAL_SERIAL_PORT_2 = 1,  //!< Port 2
    MAL_SERIAL_PORT_3 = 2,  //!< Port 3
    MAL_SERIAL_PORT_4 = 3,  //!< Port 4
    MAL_SERIAL_PORT_SIZE = 4//!< Number of possible ports
} mal_serial_port_e;

/**
 * Possible word width.
 */
typedef enum {
    MAL_SERIAL_DATA_7_BITS,//!< 7 bit words
    MAL_SERIAL_DATA_8_BITS,//!< 8 bit words
    MAL_SERIAL_DATA_9_BITS //!< 9 bit words
} mal_serial_data_size_e;

/**
 * Possible number of stop bits.
 */
typedef enum {
    MAL_SERIAL_STOP_BITS_1,//!< 1 stop bit
    MAL_SERIAL_STOP_BITS_2 //!< 2 stop bits
} mal_serial_stop_bits_e;

/**
 * Possible parity values.
 */
typedef enum {
    MAL_SERIAL_PARITY_EVEN, //!< With parity
    MAL_SERIAL_PARITY_ODD,  //!< With parity
    MAL_SERIAL_PARITY_NONE  //!< Without parity
} mal_serial_parity_e;

/**
 * @brief Callback on byte transmitted.
 * @param data The next data to transfer.
 * @return Return a status once you executed your callback. For now, nothing is
 * done with this status.
 */
typedef mal_error_e (*mal_serial_tx_callbacl_t)(uint16_t *data);

/**
 * @brief Callback on byte received.
 * @param data The data received..
 * @return Return a status once you executed your callback. For now, nothing is
 * done with this status.
 */
typedef mal_error_e (*mal_serial_rx_callbacl_t)(uint16_t data);

/**
 * Parameters to initialize a serial port.
 */
typedef struct {
    mal_serial_port_e port; /**< The port to initialize.*/
    mal_gpio_s *rx_gpio; /**< The GPIO for the rx pin.*/
    mal_gpio_s *tx_gpio; /**< The GPIO for the tx pin.*/
    uint64_t baudrate; /**< The baudrate.*/
    mal_serial_data_size_e data_size; /**< The word size.*/
    mal_serial_stop_bits_e stop_bits; /**< Number of stop bits.*/
    mal_serial_parity_e parity; /**< The parity setting.*/
    mal_serial_tx_callbacl_t tx_callback; /**< Transmit completed callback.*/
    mal_serial_rx_callbacl_t rx_callback; /**< Receive completed callback.*/
} mal_hspec_serial_init_s;

mal_error_e mal_serial_init(mal_hspec_serial_init_s *init);

/**
 * @}
 */

#endif /* SERIAL_MAL_SERIAL_H_ */
