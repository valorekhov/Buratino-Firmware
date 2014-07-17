/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# "Insert system clock initialization code here" comment
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
#include <asf.h>
#include <util/delay.h>

int main (void)
{
	// Insert system clock initialization code here (sysclk_init()).

	board_init();
	
	for(int i = 0; i<100 ; i++){
		LED_RED(PORT) = 0;
		LED_GREEN(PORT) = 0;
		LED_BLUE(PORT) = 0;
		
		if (i& (1<<1))
			LED_RED(PORT) = 1;
		if (i& (1<<2))
			LED_GREEN(PORT) = 1;
		if (i& (1<<3))
			LED_BLUE(PORT) =  1;
			
		_delay_ms(500);

	}

	// Insert application code here, after the board has been initialized.
}
