#define _POSIX_C_SOURCE 200809L
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  if (argc != 3) {
    return 1;
  }

  int sock;
  int rc;      // "rc" to skrót słów "result code"
  ssize_t cnt; // wyniki zwracane przez read() i write() są tego typu

  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock == -1) {
    perror("socket");
    return 1;
  }

  in_addr_t ip = inet_addr(argv[1]);
  if (ip == 0) {
    return 1;
  }

  uint16_t port;
  rc = sscanf(argv[2], "%hu", &port);
  if (rc == EOF) {
    perror("socket");
    return 1;
  }

  struct sockaddr_in addr = {.sin_family = AF_INET,
                             .sin_addr = {.s_addr = ip},
                             .sin_port = htons(port)};

  rc = connect(sock, (struct sockaddr *)&addr, sizeof(addr));
  if (rc == -1) {
    perror("connect");
    return 1;
  }

  while (true) {
    unsigned char buf[8];
    cnt = read(sock, buf, 8);
    if (cnt == -1) {
      perror("read");
      return 1;
    }

    printf("%.*s", (int)cnt, buf);

    if (cnt < 8) {
      break;
    }
  }

  rc = close(sock);
  if (rc == -1) {
    perror("close");
    return 1;
  }

  return 0;
}
