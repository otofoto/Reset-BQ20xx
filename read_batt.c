/*

        Program read_batt

	Use this to read the standard registers of a Smart Battery.
	Originally, this was written for a battery based on the
	Benchmarq / Unitrode / Texas Instruments chip called BQ2092,
        later tested with BQ2040.

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

  /* __u8 reg = 0x08; */ /* Device register to access */
  /* Using SMBus commands */

  for (reg=0; reg <= 0x3f; reg++)
  /*for (reg=0; reg <= 0xFE; reg++)       // there are a number of undocumented registers.*/
  {
     res = i2c_smbus_read_word_data(file,reg);
     if (res < 0) {
        printf("I2C transaction failed at reg %d\n", reg);
       /* ERROR HANDLING: i2c transaction failed */
     } else {
       /* res contains the read word */
       printf("Reg 0x%02X: Read 0x%04X %5d\n", reg, res, res);
       /*
       *(int*)res_buf = res;
       res_buf[3] = 0;
       printf("Reg 0x%02X: Read 0x%04X %5d %s\n", reg, res, res, res_buf);
       */
     }
  }

   values_read = i2c_smbus_read_i2c_block_data(file,0x20,buf);
   if (values_read > 0)
   {
      buf[values_read+1] = 0;
      printf("Manufacturer name: %s\n",buf);
   }
   else
   {
      printf("Block read failed: %d.\n", values_read);
   }

   values_read = i2c_smbus_read_i2c_block_data(file,0x21,buf);
   if (values_read > 0)
   {
      buf[values_read+1] = 0;
      printf("Device name: %s\n",buf);
   }
   else
   {
      printf("Block read failed: %d.\n", values_read);
   }

   values_read = i2c_smbus_read_i2c_block_data(file,0x22,buf);
   if (values_read > 0)
   {
      buf[values_read+1] = 0;
      printf("Device chemistry: %s\n",buf);
   }
   else
   {
      printf("Block read failed: %d.\n", values_read);
   }
   
   values_read = i2c_smbus_read_i2c_block_data(file,0x23,buf);
   if (values_read > 0)
   {
      buf[values_read+1] = 0;
      printf("Manufacturer data: %s\n",buf);
   }
   else
   {
      printf("Block read failed: %d.\n", values_read);
   }

   exit(0);
}
/*
IMPORTANT: because of the use of inline functions, you *have* to use
'-O' or some variation when you compile your program!
*/
