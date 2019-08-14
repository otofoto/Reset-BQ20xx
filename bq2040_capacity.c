/*

        Program bq2040_capacity

        Before running this program, attach your parallel port i2c plug
        to the internal CMOS Flash on your battery controller PCB.
        This is a delicate wiretapping operation.
        The internal CMOS Flash is definitely NOT accessible using
        the external connector of the battery.

        Put together by Frank Rysanek <Frantisek.Rysanek@post.cz>
        Based heavily on the "eeprom" program by Chris <chris@hedonism.cx>,
        as distributed with the lm_sensors package.

*/


/*
This program is hereby placed into the public domain.
Of course the program is provided without warranty of any kind.
*/
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <linux/i2c-dev.h>

#define DEFAULT_I2C_BUS      "/dev/i2c-0"
#define DEFAULT_EEPROM_ADDR  0x50         /* the 24C16 sits on i2c address 0x50 */
#define MAX_BYTES            8            /* max number of bytes to write in one piece */

/* write len bytes (stored in buf) to eeprom at address addr, page-offset offset */
/* if len=0 (buf may be NULL in this case) you can reposition the eeprom's read-pointer */
/* return 0 on success, -1 on failure */
int eeprom_write(int fd,
		 unsigned int addr,
		 unsigned int offset,
		 unsigned char *buf,
		 unsigned char len
){
	struct i2c_rdwr_ioctl_data msg_rdwr;
	struct i2c_msg             i2cmsg;
	int i;
	char _buf[MAX_BYTES + 1];

	if(len>MAX_BYTES){
	    fprintf(stderr,"I can only write MAX_BYTES bytes at a time!\n");
	    return -1;
	}

	if(len+offset >256){
	    fprintf(stderr,"Sorry, len(%d)+offset(%d) > 256 (page boundary)\n",
			len,offset);
	    return -1;
	}

	_buf[0]=offset;    /* _buf[0] is the offset into the eeprom page! */
	for(i=0;i<len;i++) /* copy buf[0..n] -> _buf[1..n+1] */
	    _buf[1+i]=buf[i];

	msg_rdwr.msgs = &i2cmsg;
	msg_rdwr.nmsgs = 1;

	i2cmsg.addr  = addr;
	i2cmsg.flags = 0;
	i2cmsg.len   = 1+len;
	i2cmsg.buf   = _buf;

	if((i=ioctl(fd,I2C_RDWR,&msg_rdwr))<0){
	    perror("ioctl()");
	    fprintf(stderr,"ioctl returned %d\n",i);
	    return -1;
	}

	if(len>0)
	    fprintf(stderr,"Wrote %d bytes to eeprom at 0x%02x, offset %08x\n",
		    len,addr,offset);
	else
	    fprintf(stderr,"Positioned pointer in eeprom at 0x%02x to offset %08x\n",
		    addr,offset);

	return 0;
}




int main(int argc, char **argv){

    /* filedescriptor and name of device */
    int d;
    int unlock=0;
    char i = 0;
    char *dn=DEFAULT_I2C_BUS;
    unsigned char buf[3];

    unsigned int addr=DEFAULT_EEPROM_ADDR;
    int rwmode=0;
    int capacity=1234;

    while((i=getopt(argc,argv,"hc:d:u"))>=0){
	switch(i){
	case 'h':
	    fprintf(stderr,"\n");
	    fprintf(stderr,"The purpose of this util is to help during refurbishments of battery\n");
	    fprintf(stderr,"packs equipped with the BQ2040 gas gauge chip.\n");
	    fprintf(stderr,"\n");
	    fprintf(stderr,"You'll need the primitive parallel port I2C adapter described with\n");
	    fprintf(stderr,"the i2c-pport driver. Don't use this on the internal busses such as\n");
	    fprintf(stderr,"i2c-piix4 - you'd nuke your RAM DIMMs!!!\n");
	    fprintf(stderr,"\n");
	    fprintf(stderr,"Use this util to talk to the 24C01 flash of BQ2040 - NOT to the BQ2040 itself!\n");
	    fprintf(stderr,"I.e., you need to tap the flash chip's pins directly with a soldering iron.\n");
	    fprintf(stderr,"The external SMBUS of the battery is not the right one for this purpose!\n");
	    fprintf(stderr,"\n");
	    fprintf(stderr,"### Usage: ###\n");
	    fprintf(stderr,"\n");
	    fprintf(stderr,"  %s [-c capacity] [-d /dev/i2c-whatever] [-u]\n",argv[0]);
	    fprintf(stderr,"\n");
	    fprintf(stderr,"The default capacity (in mAh) is 1234, the default bus is i2c-0.\n");
	    fprintf(stderr,"Use the -u option to turn off the write protection of BQ2040.\n");
	    fprintf(stderr,"(seems no use, really, as the reset doesn't seem to work.)\n\n");
	    exit(1);
	    break;
	case 'c':
            if(sscanf(optarg,"%d",&capacity)!=1)
            {
                fprintf(stderr,"Cannot parse '%s' as new capacity in mAh.\n",
                        optarg);
                exit(1);
            }
	    break;
        case 'd':
            dn=optarg;
            break;
        case 'u':
            unlock=1;
            break;
	default:
	    break;
	}

    }

    if((d=open(dn,O_RDWR))<0){
        fprintf(stderr,"Could not open i2c at %s\n",dn);
        perror(dn);
        exit(1);
    }

    fprintf(stderr,"i2c-devicenode is             : %s\n",dn);

    if (unlock == 1)
    {
       fprintf(stderr,"Removing the write-protect lock - writing 0x0000 at 0x3C...\n");

       buf[0] = 0x00;
       buf[1] = 0x00;
       buf[2] = 0x00;
       if(eeprom_write(d,DEFAULT_EEPROM_ADDR,0x3C,buf,2)<0)
       {
          fprintf(stderr,"Failed to write the data at 0x3C. Exiting.\n");
          exit(1);
       }
       fprintf(stderr,"Write operation successful.\n");
    }

    fprintf(stderr,"Desired new capacity: %d\n - writing it at 0x60...", capacity);

    buf[0] = capacity & 0x000000FF;
    buf[1] = (capacity & 0x0000FF00) >> 8;
    buf[2] = 0;
    
    if(eeprom_write(d,DEFAULT_EEPROM_ADDR,0x60,buf,2)<0)
    {
       fprintf(stderr,"Failed to write the new capacity at 0x60. Exiting.\n");
       exit(1);
    }
    fprintf(stderr,"Write operation successful.\n");

    close(d);

    fprintf(stderr,"The write operation appears to have succeeded.\n");
    exit(0);
}
