#include <sys/socket.h>
#ifdef __cplusplus
extern "C" {
#endif
void sendDataDouble(double*, const char* label, size_t elementCount);
void sendDataFloat(float*, const char* label, size_t elementCount);
int initSocket();
void socketSend(int sock, const char* string);

#ifdef __cplusplus
}
#endif