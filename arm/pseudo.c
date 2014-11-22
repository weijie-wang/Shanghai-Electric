#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <errno.h>
#include <sys/types.h>
#include <termio.h>
#include "pseudo.h"
static char buffer[512];
int baud_arr[]= { B0,B50,B75,B110,B134,B150,B200,B300,B600,B1200,B1800,B2400,B4800,B9600,B19200,B38400,B57600,B115200,B230400};
int baud_name[] = {0,50,75,110,134,150,200,300,600,1200,1800,2400,4800,9600,19200,38400,57600,115200,230400};

static int set_baud(int fd, struct termios *opt,int baud);
static int  set_databits(int fd,struct termios *opt, int databits);
static int set_stopbits(int fd,struct termios *opt,int stopbits);
static int set_parity(int fd,struct termios *opt,char parity);

//串口初始化申请设备,以及设备名.
int ReCATSerialInit(char * portname)
{
	char *ptr;
	int fdm;

	fdm = posix_openpt(O_RDWR | O_NONBLOCK);
    if (fdm < 0){
		perror("open");
        return(-1);
	}
    if (grantpt(fdm) < 0)
    {
        close(fdm);
        return(-2);
    }
    if (unlockpt(fdm) < 0)
    {
        close(fdm);
        return(-3);
    }
    if ((ptr=(char *)ptsname(fdm)) == NULL)
    {
        close(fdm);
        return(-4);
    }
	memcpy(portname ,ptr,1024);	
	portname[1023] = '\0';
	return fdm;
}
//配置串口.
int ReCATSeialConf(int fd,int baud,int databits,int stopbits,char parity)
{	int ret;
	struct termios opt;
	
	ret = tcgetattr(fd, &opt);
	opt.c_iflag  &= ~(ICANON | ECHO | ECHOE | ISIG);  /*Input*/
	opt.c_oflag  &= ~OPOST;   /*Output*/
	opt.c_cflag |= (CLOCAL | CREAD);

	ret = set_baud(fd,&opt,baud);
	if(ret < 0){
		return -1;
	}
	ret = set_databits(fd,&opt,databits);
	if(ret < 0){
		return -2;	
	}
	ret = set_stopbits(fd,&opt,stopbits);
	if(ret < 0){
		return -3;
	}
	ret = set_parity(fd,&opt,parity);
	if(ret < 0){
		return -4;	
	}

	return 0;
}

/*设置波特率*/
static int  set_baud(int fd, struct termios *opt,int baud)
{

	int   i;
    int   status;
    tcgetattr(fd, opt);
    for ( i= 0;  i < sizeof(baud_name) / sizeof(int);  i++) {
              
		if  (baud == baud_name[i]) {    
        	tcflush(fd, TCIOFLUSH);    
            cfsetispeed(opt, baud_arr[i]); 
            cfsetospeed(opt, baud_arr[i]);  
            status = tcsetattr(fd, TCSANOW, opt); 
            if  (status != 0) {       
            	perror("tcsetattr fd"); 
                return;    
            }   
            tcflush(fd,TCIOFLUSH);  
         } 
      }
	return 0;
}

/*设置数据位数*/
static int  set_databits(int fd,struct termios *opt, int databits)
{
	switch(databits)
	{
		case 7:
			opt->c_cflag |=CS7;
			break;
		case 8:
			opt->c_cflag |=CS8;
			break;
		default:
			printf("Unsupported data size\n");
			return -1;
	}
	
	return 0;
	
}
/*设置停止位数*/
static int set_stopbits(int fd,struct termios *opt,int stopbits)
{
	switch (stopbits)
	{  
       case 1:   
              opt->c_cflag &= ~CSTOPB; 
              break; 
       case 2:   
              opt->c_cflag |= CSTOPB; 
          break;
       default:   
               printf("Unsupported stop bits\n"); 
               return -1;
	} 
	return 0;
}
//设置奇偶校验
static int set_parity(int fd,struct termios *opt,char parity)
{
	switch (parity)
	{  
       case 'n':
       case 'N':   
            opt->c_cflag &= ~PARENB;   /**/
            opt->c_iflag &= ~INPCK;     
           	break; 
       case 'o':  
       case 'O':    
            opt->c_cflag |= (PARODD | PARENB); /* 设置为奇效验*/ 
            opt->c_iflag |= INPCK;             
            break; 
       case 'e': 
       case 'E':  
            opt->c_cflag |= PARENB;    
            opt->c_cflag &= ~PARODD;   /* 转换为偶效验*/    
            opt->c_iflag |= INPCK;       
            break;
       case 'S':
       case 's':    
			opt->c_cflag &= ~PARENB;   /*设置为无校验*/
            opt->c_cflag &= ~CSTOPB;break; 
       default:  
            printf("Unsupported parity\n");   
            return -1; 
       }  
	return 0;
}

size_t ReCATDataRecv( int fd, void * rbuf, int size)
{
    int ret = 0;
#ifdef DEBUG    
    printf("recv data...\n");
#endif
    ret = read(fd, rbuf, size);
    return ret;
}

int ReCATDataSend( int fd,void *data, size_t size)
{
    return write(fd,data,size);
}



