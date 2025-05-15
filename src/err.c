#include <unistd.h>

__attribute__((noreturn, cold))
void err_terminate(const char* msg) {
  const char* p = msg; 
  while(*p++ != '\0');

  write(2, msg, p - msg - 1);
  _exit(0);
};

