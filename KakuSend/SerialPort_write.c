    // Thanks to https://github.com/xanthium-enterprises/Serial-Port-Programming-on-Linux for providing a
	// basic program for writing to a serial port under linux

    /*====================================================================================================*/
    /* Serial port string write app mostly intended for devices that don't deal well with                 */
    /* such writes directly form the filesystem such as my arduino digistump soft CDC                     */
    /* ---------------------------------------------------------------------------------------------------*/
    /* /dev/ttyUSBx - when using USB to Serial Converter, where x can be 0,1,2...etc                      */
    /* /dev/ttyACMx - for modems and cheap uC, where x can be 0,1,2...etc                                 */
    /*====================================================================================================*/

    /*====================================================================================================*/
    /* Running the executable                                                                             */
    /* ---------------------------------------------------------------------------------------------------*/
    /* 1) Compile the  serialport_read.c  file using gcc on the terminal (without quotes)                 */
    /*                                                                                                    */
    /*  " gcc -o AppName serialport_write.c "                                                             */
    /*                                                                                                    */
    /* 2) Linux will not allow you to access the serial port from user space,you have to be root.So use   */
    /*    "sudo" command to execute the compiled binary as super user or configure serial accces for      */
    /*     the user in the system.                                                                        */
    /*       "./AppName tty_dev "msg" timeout "                                                            */
    /*                                                                                                    */
    /*====================================================================================================*/

    /*-------------------------------------------------------------*/
    /* termios structure -  /usr/include/asm-generic/termbits.h    */
    /* use "man termios" to get more info about  termios structure */
    /*-------------------------------------------------------------*/

#include <stdio.h>
#include <fcntl.h>   /* File Control Definitions           */
#include <termios.h> /* POSIX Terminal Control Definitions */
#include <unistd.h>  /* UNIX Standard Definitions      */
#include <errno.h>   /* ERROR Number Definitions           */
#include <string.h>   /* string operations */
#include <stdlib.h>   /* atoi */

#define MAXCMDLEN (128) /* small but probably enough for most uC uart traffic*/

int main(int argc , char** argv)
{
	int retval = -1;
	int fd;/*File Descriptor*/
	int timeout = 0;
	if(argc != 4
	   && argv[1] != NULL && strlen(argv[1]) > 8
	   && argv[2] != NULL && strlen(argv[2]) < MAXCMDLEN
	   && argv[3] != NULL){/*PARAM PRE CHECKING*/
		   printf("\n  Usage %s device string timeoutMs\n maximum string length %d characters",argv[0],MAXCMDLEN);
		   return retval;
	}
	timeout = atoi(argv[3]);
	if(timeout < 0){
	   printf("\n  Usage %s device string timeoutMs\n",argv[0]);
		   return retval;
	}
	printf("\n +-----------------------------------------------------------------------------+");
	printf("\n |        %s (%d) (%s) (%s) (%s)        |",argv[0],argc, argv[1],argv[2],argv[3]);
	printf("\n +-----------------------------------------------------------------------------+\n");

	/*------------------------------- Opening the Serial Port -------------------------------*/
	fd = open(argv[1],O_RDWR|O_NOCTTY );

	if(fd == -1){                       /* Error Checking */
		   printf("\n  Error! in Opening %s\n",argv[1]);
		   return retval;
	}
	/*---------- Setting the Attributes of the serial port using termios structure --------- */
	// settings taken from arduino serial control monitor.

	struct termios SerialPortSettings;  /* Create the structure                          */
	tcgetattr(fd, &SerialPortSettings); /* Get the current attributes of the Serial port */
	cfsetispeed(&SerialPortSettings,B9600);
	cfsetospeed(&SerialPortSettings,B9600);
	SerialPortSettings.c_cflag &= ~PARENB; 
	SerialPortSettings.c_cflag &= ~CSTOPB;   
	SerialPortSettings.c_cflag &= ~CSIZE;    
	SerialPortSettings.c_cflag |=  CS8;      
	SerialPortSettings.c_cflag &= ~CRTSCTS;       
	SerialPortSettings.c_cflag |= CREAD | CLOCAL; 
	SerialPortSettings.c_iflag &= ~(IXON | IXOFF | IXANY | ICRNL);
	SerialPortSettings.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG | IEXTEN | ECHOK | ECHOCTL| ECHOKE);
	SerialPortSettings.c_oflag &= ~OPOST;
	SerialPortSettings.c_cc[VMIN] = 0; 
	SerialPortSettings.c_cc[VTIME] = 0; 
	if((tcsetattr(fd,TCSANOW,&SerialPortSettings)) != 0){
		printf("\n  ERROR ! in Setting attributes, trying send regardless!\n");
	}

	/*------------------------------- Write data to serial port -----------------------------*/
	{
		char write_buffer[MAXCMDLEN+3]; /* Buffer containing characters to write into port       */
		int  bytes_written  = 0;    /* Value for storing the number of bytes written to the port */
		int  msglen =0;                 /* Value for storing lenght of the data to transmit */

		memset(write_buffer,0,MAXCMDLEN+3);
		strncpy(write_buffer,argv[2],MAXCMDLEN);
		msglen = strlen(write_buffer);
		write_buffer[msglen] = '\r';// add \r\n as some devices might need this
		write_buffer[msglen] = '\n';

		bytes_written = write(fd,write_buffer,(msglen+2));/* use write() to send data to port                                            */
										 /* "fd"                   - file descriptor pointing to the opened serial port */
										 /* "write_buffer"         - address of the buffer containing data              */
										 /* "msglen+2    "         - No of bytes to write                               */
		if(bytes_written == (msglen+2)){ // Check if all bytes we wanted to transmit have been transmitted
			retval = 0; // Ok
		}
	}

	usleep(timeout*1000); // sleep to allow the ttyACM uC device to complete it's action. (depends on uC and application)

	close(fd);/* Close the Serial port */
	return retval;
}
