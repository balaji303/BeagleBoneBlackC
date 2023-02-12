/*****
* Play with GPIOs using this C program
*
* Enter the GPIO number -> 67
* Set it as Output
* Set the output valur as High/Low/Toggle
*
*******/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
 
#define GPIO_LOCATION	"/sys/class/gpio/gpio%d/"
#define MAX_SIZE		100
 
typedef enum
{
	Input = 1,
	Output,
	High = 1,
	Low,
	Toggle,
}_operations_t;
 
int writeOperation (int pinNumber, int operation);
 
 
int writeOperation (int pinNumber, int operation)
{
	FILE *fp;
	char buffer[MAX_SIZE];
	snprintf(buffer, sizeof(buffer), GPIO_LOCATION "direction",  pinNumber);
 
	fp = fopen(buffer, "w");
	if (fp == NULL)
	{
		printf("File Cannot be open\n");
		exit(0);
	}
	fwrite((void*)"out", strlen ("out"), sizeof ("out"), fp);
	fclose(fp);
 
	memset(buffer, 0, MAX_SIZE);
	snprintf(buffer, sizeof(buffer), GPIO_LOCATION "value",  pinNumber);
 
	switch (operation)
	{
 
	case	High:
	{
		fp = fopen(buffer, "w");
		if (fp == NULL)
		{
			printf("File Cannot be open\n");
			exit(0);
		}
 
		fwrite((void*)"1", 2, 2, fp);
		fclose(fp);
 
		break;
	}
 
	case	Low:
	{
		fp = fopen(buffer, "w");
		if (fp == NULL)
		{
			printf("File Cannot be open\n");
			exit(0);
		}
 
		fwrite((void*)"0", 2, 2, fp);
		fclose(fp);
		break;
	}
 
	case	Toggle:
	{
		for (; ;)
		{
			fp = fopen(buffer, "w");
			if (fp == NULL)
			{
				printf("File Cannot be open\n");
				exit(0);
			}
 
			fwrite((void*)"1", 2, 2, fp);
			fclose(fp);
 
			sleep(1);
 
			fp = fopen(buffer, "w");
			if (fp == NULL)
			{
				printf("File Cannot be open\n");
				exit(0);
			}
 
			fwrite((void*)"0", 2, 2, fp);
			fclose(fp);
 
			sleep(1);
		}
 
		break;
	}
	}
 
	return 0;
}
 
int main(int argc, char *argv[])
{
  int gpioPinNumber = 0;
  int operationPerformed = 0;
  while(1){
	printf("\nEnter the GPIO Pin Number, you want to perform Operation: ");
	scanf("%d", &gpioPinNumber);
 
	printf("Enter the Operation you want to perform: \n"
			"1. Making Pin as Input \n"
			"2. Making Pin as Output \n");
	scanf("%d", &operationPerformed);
 
	switch (operationPerformed)
	{
       	case	Input:
       	{
       		break;
       	}
        
       	case Output:
       	{
       		int writeOperationPerformed = 0;
       		printf("Enter the Write Operation you want to perform: \n"
       				"1. Making the Pin as High \n"
       				"2. Making the Pin as Low \n"
       				"3. Making the Pin as Toggle \n");
       		scanf("%d", &writeOperationPerformed);
        
       		switch (writeOperationPerformed)
       		{
       		case	High:
       		{
       			writeOperation(gpioPinNumber, writeOperationPerformed);
       			break;
       		}
        
       		case	Low:
       		{
       			writeOperation(gpioPinNumber, writeOperationPerformed);
       			break;
       		}
        
       		case	Toggle:
       		{
       			writeOperation(gpioPinNumber, writeOperationPerformed);
       			break;
       		}
       		}
       		break;
       	}
	}
	
    }//while(1)
	return 0;
}
