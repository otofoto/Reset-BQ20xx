/*

	Program reset_bq2092

	Using this code, I have succeeded to reset the BQ2040, based on
	the soft reset procedure described in the BQ2092 datasheet.
	I have succeeded to reset the IC despite the fact that, according
	to the hints from the BQ2092 datasheet, the IC was write-protected.

        When you run this program, the indicator LEDs on your battery
	should go somewhat nuts (if there are any). To complete the reset,
	you have to disconnect and reconnect the controller PCB from the
	battery cells - to power-cycle the IC.

	Put together by Frank Rysanek <Frantisek.Rysanek@post.cz>
	Heavily based on the example C code that came with i2c 2.6.4.

*/



#include <stdio.h>
#undef __NFDBITS
#undef __FDMASK
#undef off_t
#include "/usr/local/include/linux/i2c.h"
#include "/usr/local/include/linux/i2c-dev.h"
#define O_RDWR 0x0002
/* #include <sys/types.h> */
/* #include <sys/stat.h> */
/* #include <fcntl.h> */


  int file;
  int adapter_nr = 0; /* probably dynamically determined */
  char filename[20];

int main(void)
{
  int addr = 0x0b; /* 11(dec) - The I2C address */
  /*int addr = 0x50; nah, not accessible */ /* 80(dec), 01010000(bin) */
  __u8 reg = 0x08; /* i2c command - just an example - temperature, in 0.1 K */
  __s32 res;
  char buf[30];
  int values_read = 0;
  char res_buf[3];
  sprintf(filename,"/dev/i2c%d",adapter_nr);
  
  if ((file = open(filename,O_RDWR)) < 0) {
    /* ERROR HANDLING; you can check errno to see what went wrong */
    printf("Something went wrong - couldn't open file.\n");
    exit(1);
  }

  /* int addr = 0x16; */ /* 22(dec) - The I2C address */
  if (ioctl(file,I2C_SLAVE,addr) < 0) {
    /* ERROR HANDLING; you can check errno to see what went wrong */
    printf("Something went wrong - ioctl failed.\n");
    exit(1);
  }

/*
Well, you are all set up now. You can now use SMBus commands or plain
I2C to communicate with your device. SMBus commands are preferred if
the device supports them. Both are illustrated below.
*/

  /* Using SMBus commands */

   /* RESET */
   /* Please check manually that WRINH is set to 0. */
   /* Now we can set MaxError: */
   reg = 0x0c;
   res = i2c_smbus_write_word_data(file,reg,0x0000);
   res = 0;
   if (res < 0) {
      printf("I2C transaction failed at reg %d\n", reg);
      /* ERROR HANDLING: i2c transaction failed */
      exit(1);
   }
   else
   {
      printf("Reg %d - MaxError - written successfully.\n", reg);
   }
   
   /* Now we can write the special value into the reset register: */
   reg = 0x44;
   res = i2c_smbus_write_word_data(file,reg,8009);
   if (res < 0) {
      printf("I2C transaction failed at reg %d\n", reg);
      /* ERROR HANDLING: i2c transaction failed */
      exit(1);
   }
   else
   {
      printf("Reg %d - RESET - written successfully.\n", reg);
   }

   exit(0);
}
/*
IMPORTANT: because of the use of inline functions, you *have* to use
'-O' or some variation when you compile your program!
*/
