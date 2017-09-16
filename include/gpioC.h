#ifndef GPIOC_H_
#define GPIOC_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include "timeStampC.h"


/*
#include <limits.h>
#include <sys/time.h>

#include <dirent.h>

#include <string.h>
#include <sys/resource.h>
#include <unistd.h>
*/

using namespace std;


class gpioC {

	public:
		gpioC(unsigned int);
		virtual ~gpioC();

		int gpioPWMInit(timeStampC*, double, unsigned int);
		void gpioPWMStart();
 

	private:

		int mem_fd;
		void *gpio_map;

		/* For I/O direct access */
		volatile unsigned *gpio;

		/* Choosen Pin */
		unsigned int pin;

		/* Measure Pointer */
		timeStampC *measurePtr;
		/* Frequency */
		double freq;
		/* PWM Duty */
		unsigned int duty;	

		int gpioDirectAccessInit(void);

		void gpioSet();
		void gpioClear();
		void gpioAsInput();
		void gpioAsOutput(); 
		int gpioReadPin(unsigned int);
		int gpioExportPin(unsigned int);
		int gpioUnExportPin(unsigned int);



};


#endif
