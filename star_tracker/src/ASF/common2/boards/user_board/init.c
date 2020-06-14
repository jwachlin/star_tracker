/**
 * \file
 *
 * \brief User board initialization template
 *
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#include <asf.h>
#include <board.h>
#include <conf_board.h>

#if defined(__GNUC__)
void board_init(void) WEAK __attribute__((alias("system_board_init")));
#elif defined(__ICCARM__)
void board_init(void);
#  pragma weak board_init=system_board_init
#endif

void system_board_init(void)
{
	 // Set up coil control
	 ioport_set_pin_dir(COIL_1_PIN, IOPORT_DIR_OUTPUT);
	 ioport_set_pin_level(COIL_1_PIN, !COIL_ACTIVE_LEVEL);
	 ioport_set_pin_dir(COIL_2_PIN, IOPORT_DIR_OUTPUT);
	 ioport_set_pin_level(COIL_2_PIN, !COIL_ACTIVE_LEVEL);
	 ioport_set_pin_dir(COIL_3_PIN, IOPORT_DIR_OUTPUT);
	 ioport_set_pin_level(COIL_3_PIN, !COIL_ACTIVE_LEVEL);
	 ioport_set_pin_dir(COIL_4_PIN, IOPORT_DIR_OUTPUT);
	 ioport_set_pin_level(COIL_4_PIN, !COIL_ACTIVE_LEVEL);

	 // Set up buttons
	 ioport_set_pin_dir(BUTTON_1_PIN, IOPORT_DIR_INPUT);
	 ioport_set_pin_mode(BUTTON_1_PIN, IOPORT_MODE_PULLUP);
	 ioport_set_pin_dir(BUTTON_2_PIN, IOPORT_DIR_INPUT);
	 ioport_set_pin_mode(BUTTON_2_PIN, IOPORT_MODE_PULLUP);
}