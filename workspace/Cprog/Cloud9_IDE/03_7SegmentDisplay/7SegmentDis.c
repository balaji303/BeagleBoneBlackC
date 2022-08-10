/**************************
 * 
 * 
 * 
 * 7-Segment LED's counter with delay
 * 
 * Need to run the .out file with arguments
 * 
 * example: ./a.out up 1000
 * 
 * Increments the Numbers in LED display at a delay of 1000millisec
 * 
 * Code by @balaji303 using Beagle Bone Black
 * 
 * BBB P8.7 ->   GPIO_66	-> 7SD a	
 * BBB P8.8 ->   GPIO_67	-> 7SD b
 * BBB P8.9 ->   GPIO_69	-> 7SD c
 * BBB P8.10 ->  GPIO_68	-> 7SD dp
 * BBB P8.11 ->  GPIO_45	-> 7SD d
 * BBB P8.12 ->  GPIO_44	-> 7SD e
 * BBB P8.13 ->  GPIO_23	-> 7SD f 
 * BBB P8.14 ->  GPIO_26	-> 7SD g 
 * 
 * ***********************/
 
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

/********************
 * 
 * MACRO Definition
 * 
 * ******************/
 
 
/* Beagle Bone PinOut */
#define GPIO_66     66
#define GPIO_67     67
#define GPIO_69     69
#define GPIO_68     68
#define GPIO_45     45
#define GPIO_44     44
#define GPIO_23     23
#define GPIO_26     26


/* 7SD PinOut */
#define GPIO_66_P8_7_SEGA       GPIO_66       
#define GPIO_67_P8_8_SEGB       GPIO_67       
#define GPIO_69_P8_9_SEGC       GPIO_69       
#define GPIO_68_P8_10_DP        GPIO_68      
#define GPIO_45_P8_11_SEGD      GPIO_45       
#define GPIO_44_P8_12_SEGE      GPIO_44      
#define GPIO_23_P8_13_SEGF      GPIO_23
#define GPIO_26_P8_14_SEGG      GPIO_26      


/* LED controls */
#define HIGH_VALUE  		1
#define LOW_VALUE   		0

#define GPIO_DIR_OUT        HIGH_VALUE
#define GPIO_DIR_IN         LOW_VALUE

#define GPIO_LOW_VALUE      LOW_VALUE
#define GPIO_HIGH_VALUE     HIGH_VALUE

#define SEGMENT_ON          HIGH_VALUE
#define SEGMENT_OFF         LOW_VALUE

/* Path to control GPIO */
#define SYS_GPIO_PATH 		"/sys/class/gpio"

#define INIT_ARRAY_SIZE 	100

/* Function Declaration */
int GPIO_File_Open(uint32_t GPIO_num);
int GPIO_File_Close(int fd);
int GPIO_Configure_DIR( uint32_t GPIO_num, uint8_t dir_value );
int GPIO_Write_Value( uint32_t GPIO_num, uint8_t out_value );
int GPIO_Read_Value( uint32_t GPIO_num );
int GPIO_Configure_Edge( uint32_t GPIO_num, char *edge );
int Initialize_All_GPIOs(void);
void Write_Number_7SegDisplay(uint8_t numberToDisplay);
void Start_Upcounting(int delay_value_ms);
void Start_Downcounting(int delay_value_ms);
void Start_UpDowncounting(int delay_value_ms);
void Start_RandomCounting(int delay_value_ms);

/* Function Definition */

/*
 *  GPIO_File_Open
 *  Open the sys fs file corresponding to GPIO number
 */
int GPIO_File_Open(uint32_t GPIO_num)
{
    int fd;
    char Buffer[INIT_ARRAY_SIZE];

    snprintf(Buffer, sizeof(Buffer), SYS_GPIO_PATH "/gpio%d/value", GPIO_num);

    fd = open(Buffer, O_RDONLY | O_NONBLOCK );
    if (fd < 0) {
        perror("GPIO file open\n");
    }
    return fd;
}

/*
 *  GPIO_File_Close
 *  close a file
 */
int GPIO_File_Close(int fd)
{
    return close(fd);
}

/*
 *  GPIO_Configure_DIR
 *  Configures the dir_value : 1 means 'out' , 0 means "in"
 */
int GPIO_Configure_DIR( uint32_t GPIO_num, uint8_t dir_value )
{
    int fd;
    char Buffer[ INIT_ARRAY_SIZE ];
    /* Buffer has the Path  /sys/class/gpio/gpio64/direction */
    snprintf( Buffer, sizeof(Buffer), SYS_GPIO_PATH "/gpio%d/direction", GPIO_num );
    /* Opens in Write only */
    fd = open( Buffer, O_WRONLY );
    /* FD is the file handle */
    if ( fd < 0 )
    {
         /* Error occured */
        perror( "GPIO direction configure\n" );
        return fd;
    }
    /* If dir_value is ONE writes out */
    if ( dir_value )
    {
        write( fd, "out", 4 ); //3+1  +1 for NULL character 
    }
    else
    {
        write( fd, "in", 3 );
    }

    close( fd );
    return 0;
}

/*
 *  GPIO_Write_Value
 *  Sets the out_value, can be either 0 or 1
 */
int GPIO_Write_Value(uint32_t GPIO_num, uint8_t out_value)
{
    int fd;
    char Buffer[INIT_ARRAY_SIZE];
    /* Buffer has the Path  /sys/class/gpio/gpio64/value */
    snprintf(Buffer, sizeof(Buffer), SYS_GPIO_PATH "/gpio%d/value", GPIO_num);
    /* Opens in Write only */
    fd = open(Buffer, O_WRONLY);
    /* FD is the file handle */
    if (fd < 0) 
    {
    	/* Error occured */
        perror("GPIO write value\n");
        return fd;
    }
    /* If out_value is ONE writes 1 */
    if (out_value)
        write(fd, "1", 2);
    else
        write(fd, "0", 2);

    close(fd);
    return 0;
}

/*
 *  GPIO_Read_Value
 *	Returns the GPIO value for the given number
 */
int GPIO_Read_Value(uint32_t GPIO_num)
{
    int fd;
    uint8_t read_value=0;
    char buf[INIT_ARRAY_SIZE];
    /* Buffer has the Path  /sys/class/gpio/gpio64/value */
    snprintf(buf, sizeof(buf), SYS_GPIO_PATH "/gpio%d/value", GPIO_num);
    /* FD is the file handle */
    fd = open(buf, O_WRONLY);
    if (fd < 0) {
        /* Error occured */
        perror("GPIO read value\n");
        return fd;
    }
    /* Reads and stores the value at read_value */
    read(fd, &read_value, 1);
    /* Error occured */
    close(fd);
    return read_value;
}


/*
*   GPIO_Configure_Edge
 *  	GPIO configure the edge of trigger
 *  	edge : rising, falling , none
 */
int GPIO_Configure_Edge(uint32_t GPIO_num, char *edge)
{
    int fd;
    char buf[INIT_ARRAY_SIZE];
    /* Buffer has the Path  /sys/class/gpio/gpio64/edge */
    snprintf(buf, sizeof(buf), SYS_GPIO_PATH "/gpio%d/edge", GPIO_num);
    /* FD is the file handle */
    fd = open(buf, O_WRONLY);
    if (fd < 0) {
        /* Error occured */
        perror("GPIO configure edge\n");
        return fd;
    }
    /* Writes the value from edge */
    write(fd, edge, strlen(edge) + 1);
    /* Error occured */
    close(fd);
    return 0;
}

	
/* 
 *  Initialize_All_GPIOs
 *  This function initializes all the GPIOs for this application
 *  
 */
int Initialize_All_GPIOs(void)
{
    /*  we are driving the leds , so,
     * 1. first make the GPIO which drives the led to output mode , that means set the direction as "out"
     * 2. then write the value "0" to turn off all the leds or segments.
     */
    
    GPIO_Configure_DIR(GPIO_66,GPIO_DIR_OUT);
    GPIO_Write_Value(GPIO_66, GPIO_LOW_VALUE );
    
    GPIO_Configure_DIR(GPIO_67,GPIO_DIR_OUT);
    GPIO_Write_Value(GPIO_67, GPIO_LOW_VALUE );
    
    GPIO_Configure_DIR(GPIO_69,GPIO_DIR_OUT);
    GPIO_Write_Value(GPIO_69, GPIO_LOW_VALUE );
    
    GPIO_Configure_DIR(GPIO_68,GPIO_DIR_OUT);
    GPIO_Write_Value(GPIO_68, GPIO_LOW_VALUE );
    
    GPIO_Configure_DIR(GPIO_45,GPIO_DIR_OUT);
    GPIO_Write_Value(GPIO_45, GPIO_LOW_VALUE );
    
    GPIO_Configure_DIR(GPIO_44,GPIO_DIR_OUT);
    GPIO_Write_Value(GPIO_44, GPIO_LOW_VALUE );
    
    GPIO_Configure_DIR(GPIO_23,GPIO_DIR_OUT);
    GPIO_Write_Value(GPIO_23, GPIO_LOW_VALUE );
    
    GPIO_Configure_DIR(GPIO_26,GPIO_DIR_OUT);
    GPIO_Write_Value(GPIO_26, GPIO_LOW_VALUE );

    return 0;

}

/* 
 * Write_Number_7SegDisplay
 * This function displays number on the 7segment display 
 */
void Write_Number_7SegDisplay(uint8_t numberToDisplay)
{

    switch (numberToDisplay){

    case 0:
        GPIO_Write_Value(GPIO_66_P8_7_SEGA, SEGMENT_ON);
        GPIO_Write_Value(GPIO_67_P8_8_SEGB, SEGMENT_ON);
        GPIO_Write_Value(GPIO_69_P8_9_SEGC, SEGMENT_ON);
        GPIO_Write_Value(GPIO_45_P8_11_SEGD, SEGMENT_ON);
        GPIO_Write_Value(GPIO_44_P8_12_SEGE, SEGMENT_ON);
        GPIO_Write_Value(GPIO_23_P8_13_SEGF, SEGMENT_ON);
        GPIO_Write_Value(GPIO_26_P8_14_SEGG, SEGMENT_OFF);
    break;

    case 1:
        GPIO_Write_Value(GPIO_66_P8_7_SEGA, SEGMENT_OFF);
        GPIO_Write_Value(GPIO_67_P8_8_SEGB, SEGMENT_ON);
        GPIO_Write_Value(GPIO_69_P8_9_SEGC, SEGMENT_ON);
        GPIO_Write_Value(GPIO_45_P8_11_SEGD, SEGMENT_OFF);
        GPIO_Write_Value(GPIO_44_P8_12_SEGE, SEGMENT_OFF);
        GPIO_Write_Value(GPIO_23_P8_13_SEGF, SEGMENT_OFF);
        GPIO_Write_Value(GPIO_26_P8_14_SEGG, SEGMENT_OFF);
    break;

    case 2:
        GPIO_Write_Value(GPIO_66_P8_7_SEGA, SEGMENT_ON);
        GPIO_Write_Value(GPIO_67_P8_8_SEGB, SEGMENT_ON);
        GPIO_Write_Value(GPIO_69_P8_9_SEGC, SEGMENT_OFF);
        GPIO_Write_Value(GPIO_45_P8_11_SEGD, SEGMENT_ON);
        GPIO_Write_Value(GPIO_44_P8_12_SEGE, SEGMENT_ON);
        GPIO_Write_Value(GPIO_23_P8_13_SEGF, SEGMENT_OFF);
        GPIO_Write_Value(GPIO_26_P8_14_SEGG, SEGMENT_ON);
    break;

    case 3:
        GPIO_Write_Value(GPIO_66_P8_7_SEGA, SEGMENT_ON);
        GPIO_Write_Value(GPIO_67_P8_8_SEGB, SEGMENT_ON);
        GPIO_Write_Value(GPIO_69_P8_9_SEGC, SEGMENT_ON);
        GPIO_Write_Value(GPIO_45_P8_11_SEGD, SEGMENT_ON);
        GPIO_Write_Value(GPIO_44_P8_12_SEGE, SEGMENT_OFF);
        GPIO_Write_Value(GPIO_23_P8_13_SEGF, SEGMENT_OFF);
        GPIO_Write_Value(GPIO_26_P8_14_SEGG, SEGMENT_ON);
    break;

    case 4:
        GPIO_Write_Value(GPIO_66_P8_7_SEGA, SEGMENT_OFF);
        GPIO_Write_Value(GPIO_67_P8_8_SEGB, SEGMENT_ON);
        GPIO_Write_Value(GPIO_69_P8_9_SEGC, SEGMENT_ON);
        GPIO_Write_Value(GPIO_45_P8_11_SEGD, SEGMENT_OFF);
        GPIO_Write_Value(GPIO_44_P8_12_SEGE, SEGMENT_OFF);
        GPIO_Write_Value(GPIO_23_P8_13_SEGF, SEGMENT_ON);
        GPIO_Write_Value(GPIO_26_P8_14_SEGG, SEGMENT_ON);
    break;

    case 5:
        GPIO_Write_Value(GPIO_66_P8_7_SEGA, SEGMENT_ON);
        GPIO_Write_Value(GPIO_67_P8_8_SEGB, SEGMENT_OFF);
        GPIO_Write_Value(GPIO_69_P8_9_SEGC, SEGMENT_ON);
        GPIO_Write_Value(GPIO_45_P8_11_SEGD, SEGMENT_ON);
        GPIO_Write_Value(GPIO_44_P8_12_SEGE, SEGMENT_OFF);
        GPIO_Write_Value(GPIO_23_P8_13_SEGF, SEGMENT_ON);
        GPIO_Write_Value(GPIO_26_P8_14_SEGG, SEGMENT_ON);
    break;

    case 6:
        GPIO_Write_Value(GPIO_66_P8_7_SEGA, SEGMENT_ON);
        GPIO_Write_Value(GPIO_67_P8_8_SEGB, SEGMENT_OFF);
        GPIO_Write_Value(GPIO_69_P8_9_SEGC, SEGMENT_ON);
        GPIO_Write_Value(GPIO_45_P8_11_SEGD, SEGMENT_ON);
        GPIO_Write_Value(GPIO_44_P8_12_SEGE, SEGMENT_ON);
        GPIO_Write_Value(GPIO_23_P8_13_SEGF, SEGMENT_ON);
        GPIO_Write_Value(GPIO_26_P8_14_SEGG, SEGMENT_ON);
    break;

    case 7:
        GPIO_Write_Value(GPIO_66_P8_7_SEGA, SEGMENT_ON);
        GPIO_Write_Value(GPIO_67_P8_8_SEGB, SEGMENT_ON);
        GPIO_Write_Value(GPIO_69_P8_9_SEGC, SEGMENT_ON);
        GPIO_Write_Value(GPIO_45_P8_11_SEGD, SEGMENT_OFF);
        GPIO_Write_Value(GPIO_44_P8_12_SEGE, SEGMENT_OFF);
        GPIO_Write_Value(GPIO_23_P8_13_SEGF, SEGMENT_OFF);
        GPIO_Write_Value(GPIO_26_P8_14_SEGG, SEGMENT_OFF);
    break;

    case 8:
        GPIO_Write_Value(GPIO_66_P8_7_SEGA, SEGMENT_ON);
        GPIO_Write_Value(GPIO_67_P8_8_SEGB, SEGMENT_ON);
        GPIO_Write_Value(GPIO_69_P8_9_SEGC, SEGMENT_ON);
        GPIO_Write_Value(GPIO_45_P8_11_SEGD, SEGMENT_ON);
        GPIO_Write_Value(GPIO_44_P8_12_SEGE, SEGMENT_ON);
        GPIO_Write_Value(GPIO_23_P8_13_SEGF, SEGMENT_ON);
        GPIO_Write_Value(GPIO_26_P8_14_SEGG, SEGMENT_ON);
    break;

    case 9:
        GPIO_Write_Value(GPIO_66_P8_7_SEGA, SEGMENT_ON);
        GPIO_Write_Value(GPIO_67_P8_8_SEGB, SEGMENT_ON);
        GPIO_Write_Value(GPIO_69_P8_9_SEGC, SEGMENT_ON);
        GPIO_Write_Value(GPIO_45_P8_11_SEGD, SEGMENT_ON);
        GPIO_Write_Value(GPIO_44_P8_12_SEGE, SEGMENT_OFF);
        GPIO_Write_Value(GPIO_23_P8_13_SEGF, SEGMENT_ON);
        GPIO_Write_Value(GPIO_26_P8_14_SEGG, SEGMENT_ON);
    break;

    case 10:
        GPIO_Write_Value(GPIO_66_P8_7_SEGA, SEGMENT_OFF);
        GPIO_Write_Value(GPIO_67_P8_8_SEGB, SEGMENT_OFF);
        GPIO_Write_Value(GPIO_69_P8_9_SEGC, SEGMENT_OFF);
        GPIO_Write_Value(GPIO_45_P8_11_SEGD, SEGMENT_OFF);
        GPIO_Write_Value(GPIO_44_P8_12_SEGE, SEGMENT_OFF);
        GPIO_Write_Value(GPIO_23_P8_13_SEGF, SEGMENT_OFF);
        GPIO_Write_Value(GPIO_26_P8_14_SEGG, SEGMENT_OFF);
    break;
    }



}

/* 
 * Start_Upcounting
 * implements the up counting from 0 to 9 
 *
 */
void Start_Upcounting(int delay_value_ms)
{
    uint8_t i =0;
    
    if ( Initialize_All_GPIOs() < 0)
    {
        printf("Error: GPIO init failed !\n");
    }
    else
    {
        printf("UP COUNTING.......\n");
        while(1)
       {
  
            for(;i<10;i++)
            {
                Write_Number_7SegDisplay(i);
                /*suspend execution for microsecond intervals*/
                usleep(delay_value_ms * 1000); //converting ms to micro
            }

       }

    }

}

/*
 * Start_Downcounting
 * implements the down counting from 9 to 0 
 */
void Start_Downcounting(int delay_value_ms)
{
    uint8_t i=9;
    if ( Initialize_All_GPIOs() < 0)
    {
        printf("Error: GPIO init failed !\n");
    }
    else
    {
        printf("DOWN COUNTING.......\n");
        while(1)
       {
            for(;i >= 0 ;i--)
            {
                Write_Number_7SegDisplay(i);
                /*suspend execution for microsecond intervals*/
                usleep(delay_value_ms * 1000); //converting ms to micro
            }
       }
    }
}

void Start_UpDowncounting(int delay_value_ms)
{
    uint8_t i =0;
    
    if ( Initialize_All_GPIOs() < 0)
    {
        printf("Error: GPIO init failed !\n");
    }
    else
    {
    	while(1)
    	{
        printf("UP COUNTING.......\n");
            for(;i<10;i++)
            {
                Write_Number_7SegDisplay(i);
                /*suspend execution for microsecond intervals*/
                usleep(delay_value_ms * 1000); //converting ms to micro
            }
 
        printf("DOWN COUNTING.......\n"); 
            for(;i >= 0 ;i--)
            {
                Write_Number_7SegDisplay(i);
                /*suspend execution for microsecond intervals*/
                usleep(delay_value_ms * 1000); //converting ms to micro
            }
       }
    }
}

void Start_RandomCounting(int delay_value_ms)
{
    uint8_t i=0;
    if ( Initialize_All_GPIOs() < 0)
    {
        printf("Error: GPIO init failed !\n");
    }
    else
    {
        printf("Random COUNTING.......\n");
        while(1)
       {
       	/* Random number between 0 to 9 */
		i = rand() % 10;
                Write_Number_7SegDisplay(i);
                /*suspend execution for microsecond intervals*/
                usleep(delay_value_ms * 1000); //converting ms to micro
       }
    }
}

int main(int argc, char *argv[]) {
    
    printf("Application for up/down/random counter on 7 seg display\n");

    if ( argc != 3 ) /* argc should be 3 for correct execution */
    {

        printf( "usage: %s <direction> <delay>\n", argv[0] );
        printf( "valid direction : up, down, updown, random\n");
        printf ("recommended delay range in ms : 0 to 1000\n");
    }
    else
    {
    	/* convert the 'delay' value , which argv[2]  in to integer */
        int delay_value = atoi(argv[2]);
        
        /* argc is correct , lets check argv */
        if (! strcmp(argv[1], "up") )
        {
            Start_Upcounting(delay_value);
        }
        else if (strcmp(argv[1], "down") == 0)
        {
            Start_Downcounting(delay_value);
        }
        else if (strcmp(argv[1], "updown") == 0)
        {
            Start_UpDowncounting(delay_value);
        }
        else if (strcmp(argv[1], "random") == 0)
        {
            Start_RandomCounting(delay_value);
        }
        else /* default: */
        {
            printf("Invalid direction values\n");
            printf( "valid direction values : up, down,updown,random\n");
        }
    }
}
