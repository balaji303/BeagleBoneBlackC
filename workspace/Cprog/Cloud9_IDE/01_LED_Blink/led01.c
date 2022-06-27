/********************
*
*      
* USR0->LED BLINK AT TIME INTERVEL OF 1 SEC
* 
* *******************/
#include <stdio.h>
#include <unistd.h>

int main() {
       /* Opens the USR0 LED / GPIO1_21 */
	FILE * brightness = fopen("/sys/class/leds/beaglebone:green:usr0/brightness", "w");
    
	while(1) 
	{
	       /* Sets 0 */
		fprintf(brightness, "%d\n", 0);
		/* Buffers the output */
  		fflush(brightness);
  		/* sec of sleep */
		sleep(1);
		/* Sets 1 */
		fprintf(brightness, "%d\n", 1);
		/* Buffers the output */
		fflush(brightness);
		/* sec of sleep */
		sleep(1);
	}
}
