#include <stdint.h>
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
  int lst_sock;  // gniazdko nasłuchujące
  int clnt_sock; // gniazdko połączone z bieżącym klientem
  int rc;        // "rc" to skrót słów "result code"
  ssize_t cnt;   // wyniki zwracane przez read() i write() są tego typu
  const uint16_t port = 20123;

  lst_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (lst_sock == -1) {
    perror("socket");
    return 1;
  }

  struct sockaddr_in addr = {.sin_family = AF_INET,
                             .sin_addr = {.s_addr = htonl(INADDR_ANY)},
                             .sin_port = htons(port)};

  rc = bind(lst_sock, (struct sockaddr *)&addr, sizeof(addr));
  if (rc == -1) {
    perror("bind");
    return 1;
  }

  rc = listen(lst_sock, 10);
  if (rc == -1) {
    perror("listen");
    return 1;
  }

  printf("server listening on port %hu\n", port);

  while (true) {

    clnt_sock = accept(lst_sock, NULL, NULL);
    if (rc == -1) {
      perror("accept");
      return 1;
    }

    char greeting[] = "Hello World!\r\n";

    cnt = write(clnt_sock, &greeting, sizeof(greeting));
    if (cnt == -1) {
      perror("write");
      return 1;
    }

    printf("wrote %zi bytes\n", cnt);

    rc = close(clnt_sock);
    if (rc == -1) {
      perror("close");
      return 1;
    }
  }

  rc = close(lst_sock);
  if (rc == -1) {
    perror("close");
    return 1;
  }

  return 0;
}
