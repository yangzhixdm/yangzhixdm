#include <stdio.h>
#include <arpa/inet.h>

int main (int argc, char *argv[]) {
  char *addr = "1.2.3.4";

  struct sockaddr_in addr_inet;

  if (!inet_aton(addr, &addr_inet.sin_addr)) {
    printf("conversion error");
  } else {
    printf("newwork ordered integer addr: %#x \n", addr_inet.sin_addr.s_addr);
  }
  return 0;
}
