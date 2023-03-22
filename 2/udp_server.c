#define _POSIX_C_SOURCE 200809L
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main(void) {
  int sock;
  int rc;      // "rc" to skrót słów "result code"
  ssize_t cnt; // na wyniki zwracane przez recvfrom() i sendto()
  const uint16_t port = 20123;

  sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock == -1) {
    perror("socket");
    return 1;
  }

  struct sockaddr_in addr = {.sin_family = AF_INET,
                             .sin_addr = {.s_addr = htonl(INADDR_ANY)},
                             .sin_port = htons(port)};

  rc = bind(sock, (struct sockaddr *)&addr, sizeof(addr));
  if (rc == -1) {
    perror("bind");
    return 1;
  }

  printf("server listening on port %hu\n", port);

  while (true) {
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_len;

    clnt_addr_len = sizeof(clnt_addr);
    cnt = recvfrom(sock, NULL, 1, 0, (struct sockaddr *)&clnt_addr,
                   &clnt_addr_len);
    if (cnt == -1) {
      perror("recvfrom");
      return 1;
    }
    printf("received %zi bytes\n", cnt);

    char greeting[] = "Hello World!\r\n";

    cnt = sendto(sock, greeting, sizeof(greeting), 0,
                 (struct sockaddr *)&clnt_addr, clnt_addr_len);
    if (cnt == -1) {
      perror("sendto");
      return 1;
    }

    printf("sent %zi bytes\n", cnt);
  }

  rc = close(sock);
  if (rc == -1) {
    perror("close");
    return 1;
  }

  return 0;
}
