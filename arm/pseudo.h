#ifndef RECAT_PSEUDO_TEM
#define RECAT_PSEUDO_TEM

int ReCATSerialInit(char * portname);
size_t ReCATDataRecv( int fd, void* data, int size);
int    ReCATDataSend( int fd,void *data, size_t size);
int ReCATSeialConf(int fd,int baud,int databits, int stopbits,char parity);

#endif
