/**
 * @file mal_hspec_cmn.h
 * @author Olivier Allaire
 * @date May 3 2015
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
 * @brief Common definitions to prevent circular dependencies.
 */

#ifndef HSPEC_MAL_HSPEC_CMN_H_
#define HSPEC_MAL_HSPEC_CMN_H_

#include "std/mal_stdint.h"
#include "std/mal_error.h"

/**
 * @addtogroup Clock
 * @{
 */

/**
 * @brief Possible clock sources for the MCU
 */
typedef enum {
	MAL_HSPEC_SYSTEM_CLK_SRC_INTERNAL,//!< Source is an internal oscillator.
	MAL_HSPEC_SYSTEM_CLK_SRC_EXTERNAL,//!< Source is an external crystal or oscillator.
	MAL_HSPEC_SYSTEM_CLK_SRC_AUTO     //!< MAL will select the source. Note it is safer to define a known source.
} mal_hspec_system_clk_src_e;

/**
 * @brief Structure to define the system clock source.
 */
typedef struct {
	uint64_t frequency; /**< The frequency in hertz.*/
	mal_hspec_system_clk_src_e src; /**< The source of the frequency.*/
} mal_hspec_system_clk_s;

/**
 * @}
 */

/**
 * @addtogroup GPIO
 * @{
 */

/**
 * Possible GPIO ports.
 */
typedef enum {
	MAL_HSPEC_GPIO_PORT_A = 0,  //!< MAL_HSPEC_PORT_A
	MAL_HSPEC_GPIO_PORT_B = 1,  //!< MAL_HSPEC_PORT_B
	MAL_HSPEC_GPIO_PORT_C = 2,  //!< MAL_HSPEC_PORT_C
	MAL_HSPEC_GPIO_PORT_D = 3,  //!< MAL_HSPEC_PORT_D
	MAL_HSPEC_GPIO_PORT_E = 4,  //!< MAL_HSPEC_PORT_E
	MAL_HSPEC_GPIO_PORT_F = 5,  //!< MAL_HSPEC_PORT_F
	MAL_HSPEC_GPIO_PORT_SIZE = 6//!< MAL_HSPEC_PORT_SIZE
} mal_hspec_gpio_port_e;

/**
 * Defines a GPIO.
 */
typedef struct {
	mal_hspec_gpio_port_e port; /**< The port of the GPIO.*/
	uint8_t pin; /**< The pin of the GPIO.*/
} mal_hspec_gpio_s;

/**
 * Possible GPIO directions.
 */
typedef enum {
	MAL_HSPEC_GPIO_DIR_IN,//!< MAL_GPIO_DIR_IN
	MAL_HSPEC_GPIO_DIR_OUT//!< MAL_GPIO_DIR_OUT
} mal_hspec_gpio_dir_e;

/**
 * Possible GPIO types.
 */
typedef enum {
	MAL_HSPEC_GPIO_OUT_PP,//!< Push-Pull
	MAL_HSPEC_GPIO_OUT_OD //!< Open Drain
} mal_hspec_gpio_out_e;

/**
 * Possible pull-up and pull-down values.
 */
typedef enum {
	MAL_HSPEC_GPIO_PUPD_NONE,//!< No pull-up or pull-down.
	MAL_HSPEC_GPIO_PUPD_PU,  //!< Pull-up.
	MAL_HSPEC_GPIO_PUPD_PD   //!< Pull-down.
} mal_hspec_gpio_pupd_e;

/**
 * Parameters to initialise a GPIO.
 */
typedef struct {
	mal_hspec_gpio_s gpio; /**< The gpio to initialize.*/
	mal_hspec_gpio_dir_e direction; /**< The direction of the GPIO.*/
	mal_hspec_gpio_out_e output_config; /**< The output configuration (type) of the GPIO.*/
	mal_hspec_gpio_pupd_e pupd; /**< The pull-up and pull-down configuration.*/
	uint64_t speed; /**< The refresh speed of the GPIO in hertz.*/
} mal_hspec_gpio_init_s;

/**
 * Possible GPIO event triggers.
 */
typedef enum {
	MAL_HSPEC_GPIO_EVENT_RISING, //!< MAL_HSPEC_GPIO_EVENT_RISING
	MAL_HSPEC_GPIO_EVENT_FALLING,//!< MAL_HSPEC_GPIO_EVENT_FALLING
	MAL_HSPEC_GPIO_EVENT_BOTH    //!< MAL_HSPEC_GPIO_EVENT_BOTH
} mal_hspec_gpio_event_e;

/**
 * @brief GPIO event callback.
 * @return Return a status of the callback.
 */
typedef mal_error_e (*mal_hspec_gpio_event_callback_t)(mal_hspec_gpio_s *gpio);

/**
 * Parameters to initialize an event.
 */
typedef struct {
	const mal_hspec_gpio_s *gpio; /**< The gpio for the event.*/
	mal_hspec_gpio_event_e event; /**< The event that will trigger.*/
	mal_hspec_gpio_event_callback_t callback; /**< The callback to execute upon event.*/
} mal_hspec_gpio_event_init_s;

/**
 * @}
 */

/**
 * @addtogroup Timer
 * @{
 */

/**
 * Possible timers to use. Note that not all timers are supported on all
 * platforms.
 */
typedef enum {
	MAL_HSPEC_TIMER_1 = 0,    //!< Timer 1
	MAL_HSPEC_TIMER_2 = 1,    //!< Timer 2
	MAL_HSPEC_TIMER_3 = 2,    //!< Timer 3
	MAL_HSPEC_TIMER_4 = 3,    //!< Timer 4
	MAL_HSPEC_TIMER_5 = 4,    //!< Timer 5
	MAL_HSPEC_TIMER_6 = 5,    //!< Timer 6
	MAL_HSPEC_TIMER_7 = 6,    //!< Timer 7
	MAL_HSPEC_TIMER_8 = 7,    //!< Timer 8
	MAL_HSPEC_TIMER_9 = 8,    //!< Timer 9
	MAL_HSPEC_TIMER_10 = 9,   //!< Timer 10
	MAL_HSPEC_TIMER_11 = 10,  //!< Timer 11
	MAL_HSPEC_TIMER_12 = 11,  //!< Timer 12
	MAL_HSPEC_TIMER_13 = 12,  //!< Timer 13
	MAL_HSPEC_TIMER_14= 13,   //!< Timer 14
	MAL_HSPEC_TIMER_15 = 14,  //!< Timer 15
	MAL_HSPEC_TIMER_16 = 15,  //!< Timer 16
	MAL_HSPEC_TIMER_17 = 16,  //!< Timer 17
	MAL_HSPEC_TIMER_SIZE = 17,//!< Do not use.
	MAL_HSPEC_TIMER_ANY       //!< Use to pick first available timer for initialization.
} mal_hspec_timer_e;

/**
 * Possible timer modes.
 */
typedef enum {
	MAL_HSPEC_TIMER_MODE_TICK,			//!< MAL_HSPEC_TIMER_MODE_TICK
	MAL_HSPEC_TIMER_MODE_TASK,			//!< MAL_HSPEC_TIMER_MODE_TASK
	MAL_HSPEC_TIMER_MODE_PWM,			//!< MAL_HSPEC_TIMER_MODE_PWM
	MAL_HSPEC_TIMER_MODE_COUNT,			//!< MAL_HSPEC_TIMER_MODE_COUNT
	MAL_HSPEC_TIMER_MODE_INPUT_CAPTURE	//!< MAL_HSPEC_TIMER_MODE_COUNT
} mal_hspec_timer_mode_e;

/**
 * @brief Function pointer typdef for timer in task mode.
 * @param timer Will provide the timer executing the callback.
 * @return Return a status once you executed your callback. For now, nothing is
 * done with this status.
 */
typedef mal_error_e (*mal_hspec_timer_callback_t)(mal_hspec_timer_e timer);

/**
 * Initialization parameters of a PWM input.
 */
typedef struct {
	mal_hspec_timer_e timer; /**< The timer to use for the PWM output.*/
	float frequency; /**< The frequency of the PWM.*/
	float delta; /**< The acceptable frequency delta.*/
	const mal_hspec_gpio_s *pwm_io; /**< The gpio of the PWM output.*/
} mal_hspec_timer_pwm_init_s;

/**
 * Possible timer input triggers.
 */
typedef enum {
	MAL_HSPEC_TIMER_INPUT_RISING, //!< MAL_HSPEC_GPIO_EVENT_RISING
	MAL_HSPEC_TIMER_INPUT_FALLING,//!< MAL_HSPEC_GPIO_EVENT_FALLING
	MAL_HSPEC_TIMER_INPUT_BOTH    //!< MAL_HSPEC_GPIO_EVENT_BOTH
} mal_hspec_timer_input_e;

/**
 * @brief Function pointer typdef for timer in input capture mode.
 * @param timer Will provide the timer executing the callback.
 * @param count The captured count.
 * @return Return a status once you executed your callback. For now, nothing is
 * done with this status.
 */
typedef mal_error_e (*mal_hspec_timer_input_capture_callback_t)(mal_hspec_timer_e timer, uint64_t count);

/**
 * Initialization parameters of a capture input.
 */
typedef struct {
	mal_hspec_timer_e timer; /**< The timer to use for the input capture.*/
	float frequency; /**< The frequency to count to.*/
	const mal_hspec_gpio_s *input_io; /**< The gpio of the input capture.*/
	mal_hspec_timer_input_e input_event; /**< The input event to capture.*/
	uint8_t input_divider; /**< Specifies after how many events the capture happens.*/
	mal_hspec_timer_input_capture_callback_t callback; /**< The callback to be executed when capture occurs.*/
} mal_hspec_timer_intput_capture_init_s;

/**
 * @}
 */

/**
 * @addtogroup Serial
 * @{
 */

/**
 * Possible serial ports.
 */
typedef enum {
	MAL_HSPEC_SERIAL_PORT_1 = 0,  //!< MAL_HSPEC_SERIAL_PORT_1
	MAL_HSPEC_SERIAL_PORT_2 = 1,  //!< MAL_HSPEC_SERIAL_PORT_2
	MAL_HSPEC_SERIAL_PORT_SIZE = 2//!< MAL_HSPEC_SERIAL_PORT_SIZE
} mal_hspec_serial_port_e;

/**
 * Possible word width.
 */
typedef enum {
	MAL_HSPEC_SERIAL_DATA_7_BITS,//!< MAL_HSPEC_SERIAL_DATA_7_BITS
	MAL_HSPEC_SERIAL_DATA_8_BITS,//!< MAL_HSPEC_SERIAL_DATA_8_BITS
	MAL_HSPEC_SERIAL_DATA_9_BITS //!< MAL_HSPEC_SERIAL_DATA_9_BITS
} mal_hspec_serial_data_size_e;

/**
 * Possible number of stop bits.
 */
typedef enum {
	MAL_HSPEC_SERIAL_STOP_BITS_1,//!< MAL_HSPEC_SERIAL_STOP_BITS_1
	MAL_HSPEC_SERIAL_STOP_BITS_2 //!< MAL_HSPEC_SERIAL_STOP_BITS_2
} mal_hspec_serial_stop_bits_e;

/**
 * Possible parity values.
 */
typedef enum {
	MAL_HSPEC_SERIAL_PARITY,  //!< MAL_HSPEC_SERIAL_PARITY
	MAL_HSPEC_SERIAL_NO_PARITY//!< MAL_HSPEC_SERIAL_NO_PARITY
} mal_hspec_serial_parity_e;

/**
 * @brief Callback on byte transmitted.
 * @param data The next data to transfer.
 * @return Return a status once you executed your callback. For now, nothing is
 * done with this status.
 */
typedef mal_error_e (*mal_hspec_serial_tx_callbacl_t)(uint16_t *data);

/**
 * @brief Callback on byte received.
 * @param data The data received..
 * @return Return a status once you executed your callback. For now, nothing is
 * done with this status.
 */
typedef mal_error_e (*mal_hspec_serial_rx_callbacl_t)(uint16_t data);

/**
 * Parameters to initialize a serial port.
 */
typedef struct {
	mal_hspec_serial_port_e port; /**< The port to initialize.*/
	mal_hspec_gpio_s *rx_gpio; /**< The GPIO for the rx pin.*/
	mal_hspec_gpio_s *tx_gpio; /**< The GPIO for the tx pin.*/
	uint64_t baudrate; /**< The baudrate.*/
	mal_hspec_serial_data_size_e data_size; /**< The word size.*/
	mal_hspec_serial_stop_bits_e stop_bits; /**< Number of stop bits.*/
	mal_hspec_serial_parity_e parity; /**< The parity setting.*/
	mal_hspec_serial_tx_callbacl_t tx_callback; /**< Transmit completed callback.*/
	mal_hspec_serial_rx_callbacl_t rx_callback; /**< Receive completed callback.*/
} mal_hspec_serial_init_s;

/**
 * @}
 */

/**
 * @addtogroup I2C
 * @{
 */

/**
 * Possible I2C interfaces.
 */
typedef enum {
	MAL_HSPEC_I2C_1 = 0,  //!< MAL_HSPEC_I2C_1
	MAL_HSPEC_I2C_2 = 1,  //!< MAL_HSPEC_I2C_2
	MAL_HSPEC_I2C_SIZE = 2//!< MAL_HSPEC_I2C_SIZE
} mal_hspec_i2c_e;

/**
 * I2C initialization parameters.
 */
typedef struct {
	mal_hspec_i2c_e interface; /**< The I2C interface.*/
	const mal_hspec_gpio_s *scl_gpio; /**< The GPIO of the scl pin.*/
	const mal_hspec_gpio_s *sda_gpio; /**< The GPIO of the sda pin.*/
	uint64_t bitrate; /**< The bitrate of the interface.*/
} mal_hspec_i2c_init_s;

/**
 * The possible command types.
 */
typedef enum {
	MAL_HSPEC_I2C_READ,//!< MAL_HSPEC_I2C_READ
	MAL_HSPEC_I2C_WRITE//!< MAL_HSPEC_I2C_WRITE
} mal_hspec_i2c_cmd_e;

/**
 * This structure defines the core part of an I2C message.
 */
typedef struct {
	volatile uint8_t address; /**< The address of the transaction.*/
	volatile uint8_t *buffer; /**< The buffer containing the data for the transaction.*/
	volatile uint8_t packet_size; /**< The number of words in the buffer.*/
	mal_hspec_i2c_cmd_e cmd; /**< The type of transactions.*/
} mal_hspec_i2c_packet_s;

/**
 * Possible I2C transactions results.
 */
typedef enum {
	MAL_HSPEC_I2C_SUCCESS,        //!< Successful transaction.
	MAL_HSPEC_I2C_NACK_COMPLETE,  //!< Transaction completed, but device did not acknowledge.
	MAL_HSPEC_I2C_NACK_INCOMPLETE,//!< Transaction is incomplete, and the device did not acknowledge.
	MAL_HSPEC_I2C_BUS_ERROR       //!< An error occurred on the bus during the transaction.
} mal_hspec_i2c_result_e;

/// @cond SKIP
typedef struct MAL_HSPEC_I2C_MSG mal_hspec_i2c_msg_s;
/// @endcond

/**
 * @brief This callback is used when a transaction completes.
 * @param interface The interface the transaction completed on.
 * @param packet The packet received.
 * @param result The result of the transaction.
 * @param next_msg The next message to send. This message will be sent immediately.
 * @return Return a status once you executed your callback. For now, nothing is
 * done with this status.
 */
typedef mal_error_e (*mal_hspec_i2c_callback_t)(mal_hspec_i2c_e interface, mal_hspec_i2c_packet_s *packet, mal_hspec_i2c_result_e result, mal_hspec_i2c_msg_s **next_msg);

/**
 * I2C message.
 */
typedef struct /** @cond SKIP*/MAL_HSPEC_I2C_MSG /** @endcond*/ {
	mal_hspec_i2c_packet_s packet; /**< The packet containing the core of the message.*/
	mal_hspec_i2c_callback_t callback; /**< The callback to execute once the transaction is complete.*/
} mal_hspec_i2c_msg_s;

/**
 * @}
 */

/**
 * @addtogroup CAN
 * @{
 */

/**
 * The maximum number of bytes possible in a CAN message.
 */
#define MAL_HSPEC_CAN_MAX_DATA_SIZE	8

/**
 * The possible CAN interfaces.
 */
typedef enum {
	MAL_HSPEC_CAN_1 = 0,  //!< MAL_HSPEC_CAN_1
	MAL_HSPEC_CAN_2 = 1,  //!< MAL_HSPEC_CAN_2
	MAL_HSPEC_CAN_SIZE = 2//!< MAL_HSPEC_CAN_SIZE
} mal_hspec_can_e;

/**
 * The possible ID types.
 */
typedef enum {
	MAL_HSPEC_CAN_ID_STANDARD,//!< MAL_HSPEC_CAN_ID_STANDARD
	MAL_HSPEC_CAN_ID_EXTENDED //!< MAL_HSPEC_CAN_ID_EXTENDED
} mal_hspec_can_id_type_e;

/**
 * The variables related to a CAN message.
 */
typedef struct {
	uint8_t data[MAL_HSPEC_CAN_MAX_DATA_SIZE]; /**< The array containing the data of a message.*/
	uint8_t size; /**< The numbers of valid bytes in the data array.*/
	uint32_t id; /**< The id of the message.*/
	mal_hspec_can_id_type_e id_type; /**< The type of the id.*/
} mal_hspec_can_msg_s;

/**
 * @brief This function will be called when a CAN message is transmitted.
 * @param interface The interface the message was transmitted on.
 * @param next_msg The next message to send.
 * @return Return a status once you executed your callback. For now, nothing is
 * done with this status.
 */
typedef mal_error_e (*mal_hspec_can_tx_callback_t)(mal_hspec_can_e interface, mal_hspec_can_msg_s *next_msg);

/**
 * @brief This function will be called when a CAN message is received.
 * @param interface The interface the message was received on.
 * @param next_msg The message received.
 * @return Return a status once you executed your callback. For now, nothing is
 * done with this status.
 */
typedef mal_error_e (*mal_hspec_can_rx_callback_t)(mal_hspec_can_e interface, mal_hspec_can_msg_s *msg);

/**
 * The parameters to initialize a CAN interface.
 */
typedef struct {
	mal_hspec_can_e interface; /**< The CAN interface to initialize.*/
	const mal_hspec_gpio_s *tx_gpio; /**< The GPIO of the tx pin.*/
	const mal_hspec_gpio_s *rx_gpio; /**< The GPIO of the rx pin.*/
	uint64_t bitrate; /**< The bitrate of the CAN bus.*/
	mal_hspec_can_tx_callback_t tx_callback; /**< The callback to be executed when a message is transmitted.*/
	mal_hspec_can_rx_callback_t rx_callback; /**< The callback to be executed when a message is received.*/
} mal_hspec_can_init_s;

/**
 * The variables of a CAN filter.
 */
typedef struct {
	uint32_t id; /**< The id of the filter.*/
	uint32_t mask; /**< The mask of the filter. Only bits of id matching with
						bits equal to 1 of the filter will be considered.*/
	mal_hspec_can_id_type_e id_type; /**< The type of ID to filter.*/
} mal_hspec_can_filter_s;

/**
 * @}
 */

/**
 * @addtogroup ADC
 * @{
 */

/**
 * The possible ADCs.
 */
typedef enum {
	MAL_HSPEC_ADC_0 = 0,   //!< MAL_HSPEC_ADC_0
	MAL_HSPEC_ADC_1 = 1,   //!< MAL_HSPEC_ADC_1
	MAL_HSPEC_ADC_2 = 2,   //!< MAL_HSPEC_ADC_2
	MAL_HSPEC_ADC_3 = 3,   //!< MAL_HSPEC_ADC_3
	MAL_HSPEC_ADC_4 = 4,   //!< MAL_HSPEC_ADC_4
	MAL_HSPEC_ADC_5 = 5,   //!< MAL_HSPEC_ADC_5
	MAL_HSPEC_ADC_6 = 6,   //!< MAL_HSPEC_ADC_6
	MAL_HSPEC_ADC_7 = 7,   //!< MAL_HSPEC_ADC_7
	MAL_HSPEC_ADC_8 = 8,   //!< MAL_HSPEC_ADC_8
	MAL_HSPEC_ADC_9 = 9,   //!< MAL_HSPEC_ADC_9
	MAL_HSPEC_ADC_10 = 10, //!< MAL_HSPEC_ADC_10
	MAL_HSPEC_ADC_11 = 11, //!< MAL_HSPEC_ADC_11
	MAL_HSPEC_ADC_12 = 12, //!< MAL_HSPEC_ADC_12
	MAL_HSPEC_ADC_13 = 13, //!< MAL_HSPEC_ADC_13
	MAL_HSPEC_ADC_14 = 14, //!< MAL_HSPEC_ADC_14
	MAL_HSPEC_ADC_15 = 15, //!< MAL_HSPEC_ADC_15
	MAL_HSPEC_ADC_SIZE = 16//!< MAL_HSPEC_ADC_SIZE
} mal_hspec_adc_e;

/**
 * The initialization parameters of an ADC.
 */
typedef struct {
	mal_hspec_adc_e adc; /**< To ADC to initialize.*/
	const mal_hspec_gpio_s *gpio; /**< The GPIO pin of the ADC.*/
	uint8_t bit_resolution; /**< The resolution of the ADC.*/
} mal_hspec_adc_init_s;

/**
 * @brief This callback will be executed when an ADC read is complete.
 * @param adc The ADC the value was read from.
 * @param value The value read.
 * @return Return a status once you executed your callback. For now, nothing is
 * done with this status.
 */
typedef mal_error_e (*mal_hspec_adc_read_callback_t)(mal_hspec_adc_e adc, uint64_t value);

/**
 * @}
 */

/**
 * @addtogroup SPI
 * @{
 */

/**
 * The possible SPI interfaces.
 */
typedef enum {
	MAL_HSPEC_SPI_1 = 0,  //!< MAL_HSPEC_SPI_1
	MAL_HSPEC_SPI_2 = 1,  //!< MAL_HSPEC_SPI_2
	MAL_HSPEC_SPI_SIZE = 2//!< MAL_HSPEC_SPI_SIZE
} mal_hspec_spi_e;

/**
 * Possible modes for the select pin of an interface.
 */
typedef enum {
	MAL_HSPEC_SPI_SELECT_MODE_HARDWARE,//!< The select pin is controlled by hardware.
	MAL_HSPEC_SPI_SELECT_MODE_SOFTWARE,//!< The select pin is controlled by software by the driver.
	MAL_HSPEC_SPI_SELECT_MODE_USER,    //!< The select pin is controlled by software by the user code.
	MAL_HSPEC_SPI_SELECT_MODE_NONE     //!< The select pin is not controlled at all.
} mal_hspec_spi_select_mode_e;

/**
 * The possible word sizes for data transactions
 */
typedef enum {
	MAL_HSPEC_SPI_DATA_4_BITS = 4,  //!< MAL_HSPEC_SPI_DATA_4_BITS
	MAL_HSPEC_SPI_DATA_5_BITS = 5,  //!< MAL_HSPEC_SPI_DATA_5_BITS
	MAL_HSPEC_SPI_DATA_6_BITS = 6,  //!< MAL_HSPEC_SPI_DATA_6_BITS
	MAL_HSPEC_SPI_DATA_7_BITS = 7,  //!< MAL_HSPEC_SPI_DATA_7_BITS
	MAL_HSPEC_SPI_DATA_8_BITS = 8,  //!< MAL_HSPEC_SPI_DATA_8_BITS
	MAL_HSPEC_SPI_DATA_9_BITS = 9,  //!< MAL_HSPEC_SPI_DATA_9_BITS
	MAL_HSPEC_SPI_DATA_10_BITS = 10,//!< MAL_HSPEC_SPI_DATA_10_BITS
	MAL_HSPEC_SPI_DATA_11_BITS = 11,//!< MAL_HSPEC_SPI_DATA_11_BITS
	MAL_HSPEC_SPI_DATA_12_BITS = 12,//!< MAL_HSPEC_SPI_DATA_12_BITS
	MAL_HSPEC_SPI_DATA_13_BITS = 13,//!< MAL_HSPEC_SPI_DATA_13_BITS
	MAL_HSPEC_SPI_DATA_14_BITS = 14,//!< MAL_HSPEC_SPI_DATA_14_BITS
	MAL_HSPEC_SPI_DATA_15_BITS = 15,//!< MAL_HSPEC_SPI_DATA_15_BITS
	MAL_HSPEC_SPI_DATA_16_BITS = 16 //!< MAL_HSPEC_SPI_DATA_16_BITS
} mal_hspec_spi_data_size_e;

/**
 * Possible states for clock when there is no active transactions.
 */
typedef enum {
	MAL_HSPEC_SPI_CLK_IDLE_STATE_LOW,//!< MAL_HSPEC_SPI_CLK_IDLE_STATE_LOW
	MAL_HSPEC_SPI_CLK_IDLE_STATE_HIGH//!< MAL_HSPEC_SPI_CLK_IDLE_STATE_HIGH
} mal_hspec_spi_clk_idle_state_e;

/**
 * Possible values for valid data on the bus. The edge refers to the clock
 * signal.
 */
typedef enum {
	MAL_HSPEC_SPI_DATA_LATCH_EDGE_RISING,//!< MAL_HSPEC_SPI_DATA_LATCH_EDGE_RISING
	MAL_HSPEC_SPI_DATA_LATCH_EDGE_FALLING//!< MAL_HSPEC_SPI_DATA_LATCH_EDGE_FALLING
} mal_hspec_spi_data_latch_edge_e;

/**
 * Possible values of the select IO when selecting the device.
 */
typedef enum {
	MAL_HSPEC_SPI_SELECT_POLARITY_LOW,//!< The select is active low.
	MAL_HSPEC_SPI_SELECT_POLARITY_HIGH//!< The select is active high.
} mal_hspec_spi_select_polarity_e;

/**
 * Possible values for bit ordering.
 */
typedef enum {
	MAL_HSPEC_SPI_BIT_ORDER_MSB,//!< Most significant bit first
	MAL_HSPEC_SPI_BIT_ORDER_LSB //!< Least significant bit first
} mal_hspec_spi_bit_order_e;

/**
 * All parameters to initialize an SPI interface as a master.
 */
typedef struct {
	mal_hspec_spi_e interface;/**< The SPI interface to initialize.*/
	uint64_t clock_speed;/**< The clock speed to set in Hz.*/
	mal_hspec_gpio_s *mosi;/**< The master out slave in pin.*/
	mal_hspec_gpio_s *miso;/**< The master in slave out pin.*/
	mal_hspec_gpio_s *clk;/**< The clock pin.*/
	mal_hspec_gpio_s *select;/**< The select pin. This can be set to NULL if it
								  is to be omitted. This could be because the
								  mode is user or there is no global select
								  pin.*/
	mal_hspec_spi_select_mode_e select_mode;/**< The select mode.*/
	mal_hspec_spi_data_size_e data_size;/**< The size of each words.*/
	mal_hspec_spi_bit_order_e bit_order;/**< The order of each bit in a word.*/
	mal_hspec_spi_clk_idle_state_e clk_idle_state;/**< Clock idle state.*/
	mal_hspec_spi_data_latch_edge_e latch_edge;/**< Data latch on clock edge.*/
	mal_hspec_spi_select_polarity_e select_polarity;/**< Select pin polarity.*/
} mal_hspec_spi_master_init_s;

/// @cond SKIP
typedef struct MAL_HSPEC_SPI_MSG mal_hspec_spi_msg_s;
/// @endcond

typedef mal_error_e (*mal_hspec_spi_master_transaction_complete_t)(mal_hspec_spi_msg_s *msg);

typedef struct MAL_HSPEC_SPI_MSG {
	mal_hspec_gpio_s *select;
	uint16_t *data;
	uint8_t data_length;
	mal_hspec_spi_master_transaction_complete_t callback;
} mal_hspec_spi_msg_s;

/**
 * @}
 */

#endif /* HSPEC_MAL_HSPEC_CMN_H_ */
