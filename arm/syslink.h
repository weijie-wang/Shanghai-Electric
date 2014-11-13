#ifndef RECAT_SYSLINK_H
#define ARM2DSP 11u

void* ReCATSyslinkRecv(size_t* len);
void ReCATSyslinkSend(void *data, size_t len);
int ReCATSyslinkInit();
void ReCATSyslinkClear();

char* ReCATSyslinkReadBuffer();
void ReCATSyslinkSendEvent(unsigned int id);
#endif
