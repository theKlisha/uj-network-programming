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
  ssize_t cnt; // na wyniki zwracane przez recvfrom() i sendto()

  sock = socket(AF_INET, SOCK_DGRAM, 0);
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

  cnt = sendto(sock, NULL, 0, 0, (struct sockaddr *)&addr, sizeof(addr));
  if (cnt == -1) {
    perror("sendto");
    return 1;
  }

  while (true) {
    const int buffsize = 4;
    unsigned char buf[buffsize];

    cnt = recvfrom(sock, buf, buffsize, 0, NULL, NULL);
    if (cnt == -1) {
      perror("recvfrom");
      return 1;
    }

    /* printf("recived %ld\n", cnt); */
    printf("%.*s", (int)cnt, buf);

    if (cnt < buffsize) {
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
