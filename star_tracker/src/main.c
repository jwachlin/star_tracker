/**
 * \file main.c
 *
 * \brief Main file for star tracker firmware
 *
 */


#include <asf.h>
#include <stdint.h>
#include <math.h>
#include <stdbool.h>

#define NUMBER_STEPS			(4)
#define NUMBER_COILS			(4)
#define COIL_DELAY_US			(1500)
#define STEPS_PER_REV           (2037.8864)
#define STEP_INCREMENT          (20)

#define MM_PER_ROTATION         (0.8)    // 5mm, 0.8mm pitch
#define MM_PER_STEP             (MM_PER_ROTATION / STEPS_PER_REV)

#define DEG_PER_SECOND          (0.0041666)  // 15 deg per hour, 3600 seconds/hour
#define DEG_TO_RAD              (0.0174533)
#define RAD_TO_DEG              (57.2958)

#define L_MM                    (89.0)   // TODO tune

static const ioport_pin_t coil_pins[NUMBER_COILS] = {COIL_1_PIN, COIL_3_PIN, COIL_2_PIN, COIL_4_PIN};
static int32_t current_step = 0;
static bool controlled_movement = false;
static uint32_t start_movement_time = 0;

static struct rtc_module module;

static uint32_t get_current_time_ms(void)
{
	// Actually ticks at 1024Hz.
	uint32_t count_faster = rtc_count_get_count(&module);
	uint32_t count_corrected = (count_faster * 1000) / 1024;
	return rtc_count_get_count(&module);
}

static void drive_phase(uint8_t step)
{
	if(step >= NUMBER_STEPS)
	{
		return;
	}

	/* Arduino Stepper library: The sequence of control signals for 4 control wires is as follows:
	*
	* Step C0 C1 C2 C3
	*    1  1  0  1  0
	*    2  0  1  1  0
	*    3  0  1  0  1
	*    4  1  0  0  1
	*
	* BUT Pins entered in sequence IN1-IN3-IN2-IN4 for proper step sequence
	*/

	if(step == 0)
	{
		ioport_set_pin_level(coil_pins[0], COIL_ACTIVE_LEVEL); // Blue
		ioport_set_pin_level(coil_pins[1], !COIL_ACTIVE_LEVEL); // Pink
		ioport_set_pin_level(coil_pins[2], COIL_ACTIVE_LEVEL); // Yellow
		ioport_set_pin_level(coil_pins[3], !COIL_ACTIVE_LEVEL); // Orange
	}
	else if(step == 1)
	{
		ioport_set_pin_level(coil_pins[0], !COIL_ACTIVE_LEVEL); // Blue
		ioport_set_pin_level(coil_pins[1], COIL_ACTIVE_LEVEL); // Pink
		ioport_set_pin_level(coil_pins[2], COIL_ACTIVE_LEVEL); // Yellow
		ioport_set_pin_level(coil_pins[3], !COIL_ACTIVE_LEVEL); // Orange
	}
	else if(step == 2)
	{
		ioport_set_pin_level(coil_pins[0], !COIL_ACTIVE_LEVEL); // Blue
		ioport_set_pin_level(coil_pins[1], COIL_ACTIVE_LEVEL); // Pink
		ioport_set_pin_level(coil_pins[2], !COIL_ACTIVE_LEVEL); // Yellow
		ioport_set_pin_level(coil_pins[3], COIL_ACTIVE_LEVEL); // Orange
	}
	else if(step == 3)
	{
		ioport_set_pin_level(coil_pins[0], COIL_ACTIVE_LEVEL); // Blue
		ioport_set_pin_level(coil_pins[1], !COIL_ACTIVE_LEVEL); // Pink
		ioport_set_pin_level(coil_pins[2], !COIL_ACTIVE_LEVEL); // Yellow
		ioport_set_pin_level(coil_pins[3], COIL_ACTIVE_LEVEL); // Orange
	}

	delay_us(COIL_DELAY_US);
}

static void step_motor(int32_t steps)
{
	int32_t i;
	static int32_t current_step_stage;

	current_step += steps;

	if(steps > 0)
	{
		for(i = 0; i < steps; i++)
		{
			// Number of steps
			drive_phase(current_step_stage);
			current_step_stage++;
			if(current_step_stage >= NUMBER_STEPS)
			{
				current_step_stage = 0;
			}
			if(current_step_stage < 0)
			{
				current_step_stage = (NUMBER_STEPS-1);
			}
		}
	}
	else if(steps < 0)
	{
		for(i = 0; i > steps; i--)
		{
			// Number of steps
			drive_phase(current_step_stage);
			current_step_stage--;
			if(current_step_stage >= NUMBER_STEPS)
			{
				current_step_stage = 0;
			}
			if(current_step_stage < 0)
			{
				current_step_stage = (NUMBER_STEPS-1);
			}
		}
	}

	// Turn off all coils
	for (i = 0; i < NUMBER_COILS; i++)
	{
		ioport_set_pin_level(coil_pins[i], !COIL_ACTIVE_LEVEL);
	}
}

static float calculate_current_angle_deg(void)
{
	float position_mm = MM_PER_STEP * current_step;
	float this_arg = 1.0 - ((position_mm * position_mm)/(2.0 * L_MM * L_MM));
	float theta = RAD_TO_DEG * acos( this_arg );
	return theta;
}

int main (void)
{
	system_init();
	delay_init();

	// Set up RTC for timing
	struct rtc_count_config rtc_config;
	rtc_count_get_config_defaults(&rtc_config);
	rtc_config.mode = RTC_COUNT_MODE_32BIT;
	rtc_config.prescaler = RTC_COUNT_PRESCALER_DIV_32; // Already 1.024Hz

	rtc_count_init(&module, RTC, &rtc_config);
	rtc_count_reset(&module);
	rtc_count_set_count(&module, 0);
	rtc_count_enable(&module);


	delay_ms(100);

	while(1)
	{
		
		delay_ms(2);
		if(!controlled_movement)
		{
			delay_ms(200);
		}
		if(ioport_get_pin_level(BUTTON_1_PIN) == IOPORT_PIN_LEVEL_LOW && ioport_get_pin_level(BUTTON_2_PIN) == IOPORT_PIN_LEVEL_HIGH)
		{
			step_motor(200);
			controlled_movement = false;
			delay_ms(50);
		}
		else if(ioport_get_pin_level(BUTTON_2_PIN) == IOPORT_PIN_LEVEL_LOW && ioport_get_pin_level(BUTTON_1_PIN) == IOPORT_PIN_LEVEL_HIGH)
		{
			step_motor(-200);
			controlled_movement = false;
			delay_ms(50);
		}
		
		if(ioport_get_pin_level(BUTTON_1_PIN) == IOPORT_PIN_LEVEL_LOW && ioport_get_pin_level(BUTTON_2_PIN) == IOPORT_PIN_LEVEL_LOW)
		{
			delay_ms(1000); // debounce
			start_movement_time = get_current_time_ms();
			current_step = 0;
			controlled_movement = true;
		}

		if(controlled_movement)
		{
			uint32_t time_into_control_ms = get_current_time_ms() - start_movement_time;
			float time_into_control_s = (time_into_control_ms * 0.001);
			float desired_angle_deg = time_into_control_s * DEG_PER_SECOND;

			// Simple P controller
			float current_angle_deg = calculate_current_angle_deg();
			int32_t steps_to_move = (int32_t) 50.0 * (desired_angle_deg - current_angle_deg); // move out is positive steps
			step_motor(steps_to_move);

			static uint32_t last_cycle_t = 0;
			if((get_current_time_ms() - last_cycle_t) > 5000)
			{
				last_cycle_t = get_current_time_ms();
			}
		}
	}
}
