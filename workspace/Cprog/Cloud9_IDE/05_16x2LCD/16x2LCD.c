/*=========================================================================================================
BBB_expansion_header_pins       GPIO number     16x2 LCD pin      Purpose 
===========================================================================================================
P8.7                             GPIO_66          4(RS)           Register selection (Character vs. Command)
P8.8                             GPIO_67          5(RW)           Read/write 
P8.9                             GPIO_69          6(EN)           Enable
P8.10                            GPIO_68          11(D4)          Data line 4
P8.11                            GPIO_45          12(D5)          Data line 5
P8.12                            GPIO_44          13(D6)          Data line 6
P8.14                            GPIO_26          14(D7)          Data line 7 
P8.16                            GPIO_46          15(BKLTA)       Backlight anode(+)
P9.15                            GPIO_48          16(BKLTK)       Backlight cathode(-)
P9.0                              ----            1(VSS/GND)      Ground
P9.7                              ----            2(sys_VDD +5V)  +5V supply 
============================================================================================================= */
/***
 * Login as Root using sudo su
 * root@beaglebone:/var/lib/cloud9/BeagleBone/Black/balaji# ./02.out balaJI
This Application prints your message on the 16x2 LCD
^C
root@beaglebone:/var/lib/cloud9/BeagleBone/Black/balaji#
 * 
 * ****/
/************************************* INCLUDES **********************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include<stdint.h>
#include <time.h>
#include <math.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>

#include <stdarg.h>

#include <sys/stat.h>

/************************************* DEFINES **********************************************/
/* BBB GPIO numbers used to connect with LCD pins */
#define GPIO_66_P8_7_RS_4       		66       /*  Register selection (Character vs. Command) */
#define GPIO_67_P8_8_RW_5      		 	67       /*  Read/write     */
#define GPIO_69_P8_9_EN_6       		69       /*  Enable */
#define GPIO_68_P8_10_D4_11      		68       /*  Data line 4    */
#define GPIO_45_P8_11_D5_12      		45       /*  Data line 5    */
#define GPIO_44_P8_12_D6_13      		44       /*  Data line 6    */
#define GPIO_26_P8_14_D7_14     		26       /*  Data line 7    */

/* BBB GPIO pins used for LCD backlight */
#define GPIO_46_P8_16_BKLTA_15      	46       /*  Backlight anode    */
#define GPIO_48_P9_15_BKLTK_16      	48        /*  Backlight cathode */


/* LCD data lines */
#define LCD_DATABIT0
#define LCD_DATABIT1
#define LCD_DATABIT2
#define LCD_DATABIT3
#define LCD_DATABIT4    GPIO_68_P8_10_D4_11
#define LCD_DATABIT5    GPIO_45_P8_11_D5_12
#define LCD_DATABIT6    GPIO_44_P8_12_D6_13
#define LCD_DATABIT7    GPIO_26_P8_14_D7_14


//Register selection (Character vs. Command)
/*The register selection pin sets the LCD to command or character mode, so it knows how to
interpret the next set of data that is transmitted via the data lines. Based on the state of this pin,
data sent to the LCD is either interpreted as a command (for example, move the cursor) or
characters (for example, the letter a). */

#define COMMAND_MODE       		 LOW_VALUE
#define USER_DATA_MODE           HIGH_VALUE


/************************************* LCD COMMAND SETS **********************************************/

/*Clears entire display and sets DDRAM address 0 in
address counter .. clears display fills up DDRAM with 20H ..
thats the ASCII value for black character or "space"*/
#define LCD_CMD_CLEAR_DISPLAY  0x01

/*Sets DDRAM address 0 in address counter. Also returns display from being shifted to original position.
DDRAM contents remain unchanged. */
#define LCD_CMD_RETURN_HOME 0x02


/* Sets cursor move direction and specifies display shift. These operations are performed during data write
and read. */
#define LCD_CMD_ENTRY_MODESET   			 0X04

#define INC_CURSOR     						( 1 << 1)
#define DEC_CURSOR    						 (LCD_CMD_ENTRY_MODESET & ~(INC_CURSOR))
#define ACCOMPANY_DISPLAY_SHIFT            	( 1 << 0)
#define DO_NOT_ACCOMPANY_DISPLAY_SHIFT      (LCD_CMD_ENTRY_MODESET & ~(ACCOMPANY_DISPLAY_SHIFT))


/*Sets entire display (D) on/off, cursor on/off (C), and blinking of cursor position character (B).*/
#define LCD_CMD_DISPLAY_CURSOR_ONOFF_CONTROL  0x08

#define DISPLAY_ON    						(1 << 2)
#define DISPLAY_OFF   						(LCD_CMD_DISPLAY_CURSOR_ONOFF_CONTROL & ~(DISPLAY_ON))

#define CURSOR_ON    						(1 << 1)
#define CURSOR_OFF  						(LCD_CMD_DISPLAY_CURSOR_ONOFF_CONTROL & ~(CURSOR_ON))

#define BLINK_CURSOR_ON 	 				(1 << 0)
#define BLINK_CURSOR_OFF  					(LCD_CMD_DISPLAY_CURSOR_ONOFF_CONTROL & ~(BLINK_CURSOR_ON))

/* Moves cursor and shifts display without changing DDRAM contents*/
#define LCD_CMD_CURSOR_DISPLAY_SHIFT_CONTROL  0x10

#define DISPLAY_SHIFT   					  ( 1 << 3)
#define CURSOR_MOVE     					  (LCD_CMD_CURSOR_DISPLAY_SHIFT_CONTROL & ~(DISPLAY_SHIFT))

#define SHIFT_TO_RIGHT  					( 1 << 2)
#define SHIFT_TO_LEFT   					 (LCD_CMD_CURSOR_DISPLAY_SHIFT_CONTROL & ~(SHIFT_TO_RIGHT))


/*Sets interface data length (DL), number of display lines (N), and character font (F). */
#define LCD_CMD_FUNCTION_SET  				0x20

#define DATA_LEN_8  						( 1 << 4)
#define DATA_LEN_4  						(LCD_CMD_FUNCTION_SET & ~(DATA_LEN_8))
#define DISPLAY_2_LINES 				 	( 1 << 3)
#define DISPLAY_1_LINE  					(LCD_CMD_FUNCTION_SET & ~(DISPLAY_2_LINES))
#define MATRIX_5_X_10 						( 1 << 2)
#define MATRIX_5_X_8 						(LCD_CMD_FUNCTION_SET & ~(MATRIX_5_X_10))


/*Sets CGRAM address. CGRAM data is sent and received after this setting. */
#define LCD_CMD_SET_CGRAM_ADDRESS  			0x40

/* Sets DDRAM address. DDRAM data is sent and received after this setting. */
#define LCD_CMD_SET_DDRAM_ADDRESS  			0x80

#define DDRAM_SECOND_LINE_BASE_ADDR         (LCD_CMD_SET_DDRAM_ADDRESS | 0x40 )
#define DDRAM_FIRST_LINE_BASE_ADDR          LCD_CMD_SET_DDRAM_ADDRESS


#define LCD_ENABLE 1
#define LCD_DISABLE 0

/* HD44780 CGRAM address start */
#define CGRAM_address_start 0x40

#define INS_WAIT_TIME ( 8 * 1000)

/* GPIO Related */
#define SOME_BYTES          100

/* This is the path corresponds to GPIOs in the 'sys' directory */
#define SYS_GPIO_PATH       "/sys/class/gpio"

#define HIGH_VALUE          1
#define LOW_VALUE           0

#define GPIO_DIR_OUT        HIGH_VALUE
#define GPIO_DIR_IN         LOW_VALUE

#define GPIO_LOW_VALUE      LOW_VALUE
#define GPIO_HIGH_VALUE     HIGH_VALUE

/************************************* Function Prototypes **********************************************/

/* LCD Related Command */
void lcd_init(void);
void lcd_set_cursor(uint8_t row, uint8_t column);
void lcd_enable(void);
void lcd_print_char(uint8_t ascii_Value);
void lcd_print_string(char *message);
void lcd_send_command(uint8_t command);
void lcd_printf(const char *fmt, ...);
void lcd_load_cgram(char tab[], uint8_t charnum);

/* GPIO Related Command */
int gpio_export(uint32_t gpio_num);
int gpio_configure_dir(uint32_t gpio_num, uint8_t dir_value);
int gpio_write_value(uint32_t gpio_num, uint8_t out_value);
int gpio_read_value(uint32_t gpio_num);
int gpio_configure_edge(uint32_t gpio_num, char *edge);
int gpio_file_open(uint32_t gpio_num);
int gpio_file_close(int fd);



/************************************* Function Definition **********************************************/
/* This function does basic initialization for your lcd  */
void lcd_init(void)
{
	uint8_t cmd=0;

	//setting function
    cmd= LCD_CMD_FUNCTION_SET | DATA_LEN_4 | DISPLAY_2_LINES | MATRIX_5_X_8;
    lcd_send_command(cmd);

    /* either you read the Busy flag and wait until BF=0, or just wait for ~5ms as per data sheet */
    usleep(INS_WAIT_TIME);

    //setting entry mode of the LCD
    cmd=LCD_CMD_ENTRY_MODESET | INC_CURSOR;
    lcd_send_command(cmd);

    usleep(INS_WAIT_TIME);

    //setting display and cursor control options
    cmd=LCD_CMD_DISPLAY_CURSOR_ONOFF_CONTROL | DISPLAY_ON | CURSOR_ON; //0x0e
    lcd_send_command(cmd);

    usleep(INS_WAIT_TIME);

}

/**
  * @brief  Set Lcd to a specified location given by row and column information
  * @param  Row Number (1 to 2)
  * @param  Column Number (1 to 16) Assuming a 2 X 16 characters display
  */
void lcd_set_cursor(uint8_t row, uint8_t column)
{
  column--;
  switch (row)
  {
    case 1:
      /* Set cursor to 1st row address and add index*/
      lcd_send_command(column |= DDRAM_FIRST_LINE_BASE_ADDR);
      break;
    case 2:
      /* Set cursor to 2nd row address and add index*/
        lcd_send_command(column |= DDRAM_SECOND_LINE_BASE_ADDR);
      break;
    default:
      break;
  }
}

//call this function in order to make LCD latch the data lines in to its internal registers.
void lcd_enable(void)
{
    gpio_write_value(GPIO_69_P8_9_EN_6,LCD_ENABLE);
    usleep(2 * 1000); //2ms delay
    gpio_write_value(GPIO_69_P8_9_EN_6,LCD_DISABLE);

}


void lcd_print_char(uint8_t ascii_Value)
{

    gpio_write_value(GPIO_66_P8_7_RS_4,USER_DATA_MODE);

    //first send the msb

    uint8_t data_msb = ((ascii_Value >> 4) & 0X0f ); // d7 d7 d5 d4

    gpio_write_value(LCD_DATABIT4,(data_msb & ( 1 << 0) ));
    gpio_write_value(LCD_DATABIT5,(data_msb & ( 1 << 1) ));
    gpio_write_value(LCD_DATABIT6,(data_msb & ( 1 << 2) ));
    gpio_write_value(LCD_DATABIT7,(data_msb & ( 1 << 3) ));

    lcd_enable();


    uint8_t data_lsb = (( ascii_Value & 0x0f )); // d3 d2 d1 d0

    gpio_write_value(LCD_DATABIT4,(data_lsb & ( 1 << 0) ));
    gpio_write_value(LCD_DATABIT5,(data_lsb & ( 1 << 1) ));
    gpio_write_value(LCD_DATABIT6,(data_lsb & ( 1 << 2) ));
    gpio_write_value(LCD_DATABIT7,(data_lsb & ( 1 << 3) ));

    lcd_enable();

    usleep(5 * 1000);
}

void lcd_print_string(char *message)
{

      do
      {
          lcd_print_char((uint8_t)*message++);
      }
      while (*message != '\0');

}


void lcd_send_command(uint8_t command)
{
/*RS: Is a Register Select Control signal.
When this signal is '1' = It accepts data to be displayed.
When this signal is '0' = It accepts instructions for
the LCD like setting font, cursor position etc. */

    //first make RS=0
    gpio_write_value(GPIO_66_P8_7_RS_4,COMMAND_MODE);

    //first send the msb

    uint8_t command_msb = ((command >> 4) & 0X0f ); // d7 d6 d5 d4

    gpio_write_value(LCD_DATABIT4,(command_msb & ( 1 << 0) ));
    gpio_write_value(LCD_DATABIT5,(command_msb & ( 1 << 1) ));
    gpio_write_value(LCD_DATABIT6,(command_msb & ( 1 << 2) ));
    gpio_write_value(LCD_DATABIT7,(command_msb & ( 1 << 3) ));

    lcd_enable();

    //now send the lsb

    uint8_t command_lsb = (( command & 0x0f )); // d3 d2 d1 d0

    gpio_write_value(LCD_DATABIT4,(command_lsb & ( 1 << 0) ));
    gpio_write_value(LCD_DATABIT5,(command_lsb & ( 1 << 1) ));
    gpio_write_value(LCD_DATABIT6,(command_lsb & ( 1 << 2) ));
    gpio_write_value(LCD_DATABIT7,(command_lsb & ( 1 << 3) ));

    lcd_enable();

}

/**
  * @brief  Initializes Character Generator CGRAM with custom characters data
  * @param  Table containing characters definition values
  * @param  Number of characters defined in the table
  * @retval None
  */
void lcd_load_cgram(char tab[], uint8_t charnum)
{
  uint8_t index;
  /* Each character contains 8 definition values*/
  charnum = charnum * 8;
  for (index = 0;index < charnum;index++)
  {
    /* Store values in LCD*/
    lcd_print_char(tab[index]);
    usleep(1*1000);
  }
}


void lcd_printf(const char *fmt, ...)
{
     int i;
      uint32_t text_size, letter;
      static char text_buffer[32];
      va_list args;

      va_start(args, fmt);
      text_size = vsprintf(text_buffer, fmt, args);

      // Process the string
      for (i = 0; i < text_size; i++)
      {
        letter = text_buffer[i];

        if (letter == 10)
          break;
        else
        {
          if ((letter > 0x1F) && (letter < 0x80))
              lcd_print_char(letter);
        }
      }
  }
 
/*
 *  GPIO export pin
 *
 */
int gpio_export(uint32_t gpio_num)
{
	int fd, len;
	char buf[SOME_BYTES];

	fd = open(SYS_GPIO_PATH "/export", O_WRONLY);
	if (fd < 0) {
		perror(" error opening export file\n");
		return fd;
	}

	len = snprintf(buf, sizeof(buf), "%d", gpio_num);
	write(fd, buf, len);
	close(fd);

	return 0;
}

/*
 *  GPIO configure direction
 *  dir_value : 1 means 'out' , 0 means "in"
 */
int gpio_configure_dir(uint32_t gpio_num, uint8_t dir_value)
{
    int fd;
    char buf[SOME_BYTES];

    snprintf(buf, sizeof(buf), SYS_GPIO_PATH "/gpio%d/direction", gpio_num);

    fd = open(buf, O_WRONLY);
    if (fd < 0) {
        perror("gpio direction configure\n");
        return fd;
    }

    if (dir_value)
        write(fd, "out", 4); //3+1  +1 for NULL character 
    else
        write(fd, "in", 3);

    close(fd);
    return 0;
}

/*
 *  GPIO write value
 *  out_value : can be either 0 or 1
 */
int gpio_write_value(uint32_t gpio_num, uint8_t out_value)
{
    int fd;
    char buf[SOME_BYTES];

    snprintf(buf, sizeof(buf), SYS_GPIO_PATH "/gpio%d/value", gpio_num);

    fd = open(buf, O_WRONLY);
    if (fd < 0) {
        perror("gpio write value\n");
        return fd;
    }

    if (out_value)
        write(fd, "1", 2);
    else
        write(fd, "0", 2);

    close(fd);
    return 0;
}

/*
 *  GPIO read value
 */
int gpio_read_value(uint32_t gpio_num)
{
    int fd;
    uint8_t read_value=0;
    char buf[SOME_BYTES];

    snprintf(buf, sizeof(buf), SYS_GPIO_PATH "/gpio%d/value", gpio_num);

    fd = open(buf, O_WRONLY);
    if (fd < 0) {
        perror("gpio read value\n");
        return fd;
    }

    read(fd, &read_value, 1);

    close(fd);
    return read_value;
}


/*
 *  GPIO configure the edge of trigger
 *  edge : rising, falling
 */
int gpio_configure_edge(uint32_t gpio_num, char *edge)
{
    int fd;
    char buf[SOME_BYTES];

    snprintf(buf, sizeof(buf), SYS_GPIO_PATH "/gpio%d/edge", gpio_num);

    fd = open(buf, O_WRONLY);
    if (fd < 0) {
        perror("gpio configure edge\n");
        return fd;
    }

    write(fd, edge, strlen(edge) + 1);
    close(fd);
    return 0;
}

/*
 *  Open the sys fs file corresponding to gpio number
 */
int gpio_file_open(uint32_t gpio_num)
{
    int fd;
    char buf[SOME_BYTES];

    snprintf(buf, sizeof(buf), SYS_GPIO_PATH "/gpio%d/value", gpio_num);

    fd = open(buf, O_RDONLY | O_NONBLOCK );
    if (fd < 0) {
        perror("gpio file open\n");
    }
    return fd;
}

/*
 *  close a file
 */
int gpio_file_close(int fd)
{
    return close(fd);
}





/* This function initializes all the gpios for this application
 *  TODO : Error handling implementation
 */
int initialize_all_gpios(void)
{
	/* first lets export all the GPIOs */
	gpio_export(GPIO_66_P8_7_RS_4);
	gpio_export(GPIO_67_P8_8_RW_5);
	gpio_export(GPIO_69_P8_9_EN_6);
	gpio_export(GPIO_68_P8_10_D4_11);
	gpio_export(GPIO_45_P8_11_D5_12);
	gpio_export(GPIO_44_P8_12_D6_13);
	gpio_export(GPIO_26_P8_14_D7_14);

    
   /*first configure the direction for LCD pins */
    gpio_configure_dir(GPIO_66_P8_7_RS_4,GPIO_DIR_OUT);
    gpio_configure_dir(GPIO_67_P8_8_RW_5,GPIO_DIR_OUT);
    gpio_configure_dir(GPIO_69_P8_9_EN_6,GPIO_DIR_OUT);
    gpio_configure_dir(GPIO_68_P8_10_D4_11,GPIO_DIR_OUT);
    gpio_configure_dir(GPIO_45_P8_11_D5_12,GPIO_DIR_OUT);
    gpio_configure_dir(GPIO_44_P8_12_D6_13,GPIO_DIR_OUT);
    gpio_configure_dir(GPIO_26_P8_14_D7_14,GPIO_DIR_OUT);

    return 0;

}


void print_time_and_date()
{
	time_t rawtime;
	struct tm * timeinfo;

	time ( &rawtime );
	timeinfo = localtime ( &rawtime );

    lcd_send_command(DDRAM_FIRST_LINE_BASE_ADDR);

	lcd_printf("%d-%d-%d",1900+timeinfo->tm_year, timeinfo->tm_mon, timeinfo->tm_mday);

   lcd_send_command(DDRAM_SECOND_LINE_BASE_ADDR);

   lcd_printf("%d:%d:%d",timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);

}

/* This function gathers the ip address of the system and prints it on LCD */
int print_ip_address()
{
    int fd;
    struct ifreq ifr;

    char iface[] = "usb0";

    fd = socket(AF_INET, SOCK_DGRAM, 0);

    //Type of address to retrieve - IPv4 IP address
    ifr.ifr_addr.sa_family = AF_INET;

    //Copy the interface name in the ifreq structure
    strncpy(ifr.ifr_name , iface , IFNAMSIZ-1);

    ioctl(fd, SIOCGIFADDR, &ifr);

    close(fd);

    //display result
    // 1. print the interface name on the first line
    lcd_send_command(DDRAM_FIRST_LINE_BASE_ADDR);
    lcd_print_string((char *)iface);
    lcd_send_command(DDRAM_SECOND_LINE_BASE_ADDR);
    lcd_print_string((char *)inet_ntoa(( (struct sockaddr_in *)&ifr.ifr_addr )->sin_addr));

 //   lcd_printf("%s - %s\n" , iface , inet_ntoa(( (struct sockaddr_in *)&ifr.ifr_addr )->sin_addr) );

    return 0;
}

/* Some silly graphics :) */
void tansition_graphics(void)
{

	 sleep(1);

    lcd_set_cursor(1,1);
    lcd_send_command(LCD_CMD_CLEAR_DISPLAY);

    for (uint8_t n =0 ; n < 2 ; n++)
    {
        for(uint8_t i=0;i<16;i++)
        {
            lcd_print_char('*');
            usleep(75*1000);

        }
        lcd_set_cursor(2,16);
        lcd_send_command(0x04);
    }
    
    lcd_set_cursor(1,1);
    lcd_send_command(0x06);
    usleep(450 * 1000);

    lcd_send_command(LCD_CMD_CLEAR_DISPLAY);

}

int main(int argc, char *argv[])
 {
    char msg_buf[16 * 2];
    
    printf("This Application prints your message on the 16x2 LCD\n");

    if ( argc != 2 ) /* argc should be 2 for correct execution */
    {

        printf( "usage: %s <message>\n", argv[0] );

    }
    else
    {

    	initialize_all_gpios();

        gpio_write_value(GPIO_66_P8_7_RS_4,GPIO_LOW_VALUE);
       
       /*The RW pin is always tied to ground in this implementation, meaning that you are only writing to
        the display and never reading from it.*/
        gpio_write_value(GPIO_67_P8_8_RW_5,GPIO_LOW_VALUE);
       
       /*• The EN pin is used to tell the LCD when data is ready*/
        gpio_write_value(GPIO_69_P8_9_EN_6,GPIO_LOW_VALUE);
        
        /*Data pins 4~7 are used for actually transmitting data, and data pins 0~3 are left unconnected*/
        gpio_write_value(GPIO_68_P8_10_D4_11,GPIO_LOW_VALUE);
        gpio_write_value(GPIO_45_P8_11_D5_12,GPIO_LOW_VALUE);
        gpio_write_value(GPIO_44_P8_12_D6_13,GPIO_LOW_VALUE);
        gpio_write_value(GPIO_26_P8_14_D7_14,GPIO_LOW_VALUE);

        /*
        You can illuminate the backlight by connecting the anode pin to 5V and the cathode pin to ground
        if you are using an LCD with a built-in resistor for the backlight. If you are not, you must put a
        current-limiting resistor in-line with the anode or cathode pin. The datasheet for your device will
        generally tell you if you need to do this. */


        lcd_init();


        lcd_send_command(LCD_CMD_CLEAR_DISPLAY);

        lcd_set_cursor(1,1);
        
        //This is the message given by user 
        strncpy(msg_buf,argv[1],32);
        
        while(1)
        {
             lcd_printf("BBB LCD Demo\n");
             tansition_graphics();
             print_ip_address();
             tansition_graphics();
             lcd_printf(msg_buf);
             tansition_graphics();
             print_time_and_date();
             tansition_graphics();
        }
    }

    return 0;
}