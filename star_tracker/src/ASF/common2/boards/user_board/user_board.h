/**
 * \file
 *
 * \brief User board definition template
 *
 */

 /* This file is intended to contain definitions and configuration details for
 * features and devices that are available on the board, e.g., frequency and
 * startup time for an external crystal, external memory devices, LED and USART
 * pins.
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#ifndef USER_BOARD_H
#define USER_BOARD_H

#include <conf_board.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \ingroup group_common_boards
 * \defgroup user_board_group User board
 *
 * @{
 */

void system_board_init(void);

/** Name string macro */
#define BOARD_NAME                "Star Tracker"

#define BUTTON_1_PIN				IOPORT_CREATE_PIN(IOPORT_PORTB, 22)
#define BUTTON_2_PIN				IOPORT_CREATE_PIN(IOPORT_PORTA, 15)

#define COIL_1_PIN					IOPORT_CREATE_PIN(IOPORT_PORTA, 1)
#define COIL_2_PIN					IOPORT_CREATE_PIN(IOPORT_PORTA, 0)
#define COIL_3_PIN					IOPORT_CREATE_PIN(IOPORT_PORTB, 3)
#define COIL_4_PIN					IOPORT_CREATE_PIN(IOPORT_PORTB, 2)
#define COIL_ACTIVE_LEVEL			IOPORT_PIN_LEVEL_HIGH

/** @} */

#ifdef __cplusplus
}
#endif

#endif // USER_BOARD_H
