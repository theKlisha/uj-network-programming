#define _POSIX_C_SOURCE 200809L

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

enum operator{ ADD, SUBTRACT };

int parse(void const *stop, void const **pos, long int *result);
int parse_operator(void const *stop, void const **pos, enum operator* result);
int parse_endl(void const *stop, void const **pos);
int parse_number(void const *stop, void const **pos, long int *result);

int main(void) {
  int sock;
  int rc;
  const uint16_t port = 2020;

  sock = socket(AF_INET, SOCK_DGRAM, 0);

  if (sock == -1) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in addr = {.sin_family = AF_INET,
                             .sin_addr = {.s_addr = htonl(INADDR_ANY)},
                             .sin_port = htons(port)};

  rc = bind(sock, (struct sockaddr *)&addr, sizeof(addr));

  if (rc == -1) {
    perror("bind");
    exit(EXIT_FAILURE);
  }

  printf("server listening on port %hu\n", port);

  while (true) {
    struct sockaddr_in address;
    socklen_t address_size = sizeof(address);
    const int request_buf_size = 1024 * 64;
    uint8_t request_buf[request_buf_size];

    ssize_t request_len = recvfrom(sock, &request_buf, request_buf_size, 0,
                                   (struct sockaddr *)&address, &address_size);

    if (request_len == -1) {
      perror("recvfrom");
      exit(EXIT_FAILURE);
    }

    char response_buf[32];
    long int result;
    const void *pos = request_buf;
    rc = parse(request_buf + request_len, &pos, &result);

    if (rc) {
      sprintf(response_buf, "ERROR\n");
    } else {
      sprintf(response_buf, "%ld\n", result);
    }

    ssize_t response_bytes_sent =
        sendto(sock, &response_buf, strlen(response_buf), 0,
               (struct sockaddr *)&address, address_size);

    if (response_bytes_sent == -1) {
      perror("sendto");
      exit(EXIT_FAILURE);
    }
  }

  rc = close(sock);
  if (rc == -1) {
    perror("close");
    exit(EXIT_FAILURE);
  }

  return 0;
}

int parse_operator(void const *stop, void const **pos, enum operator* result) {
  if (*pos >= stop)
    return -1;

  if (**(char **)pos == '+') {
    *result = ADD;
    *pos += 1;

    printf("parse_operator: done (+)\n");
    return 0;
  } else if (**(char **)pos == '-') {
    *result = SUBTRACT;
    *pos += 1;

    printf("parse_operator: done (-)\n");
    return 0;
  }

  return -1;
};

int parse_endl(void const *stop, void const **pos) {
  if (*pos >= stop || **(char **)pos != '\r')
    return -1;

  return 0;
}

int parse_number(void const *stop, void const **pos, long int *result) {
  if (*pos >= stop)
    return -1;

  char const *ptr = *pos;
  int len = 0;
  while (*ptr >= '0' && *ptr <= '9') {
    ptr += 1;
    len = (void *)ptr - *pos;
  }

  if (len == 0 || len >= 16)
    return -1;

  char num_str[16];
  num_str[len] = '\0';
  strncpy(num_str, *pos, len);
  sscanf(num_str, "%ld", result);

  *pos += len;

  return 0;
};

int parse(void const *stop, void const **pos, long int *result) {
  printf("parse\n");

  long int rolling_sum = 0;
  enum operator op = ADD;

  parse_operator(stop, pos, &op);

  if (!parse_number(stop, pos, &rolling_sum)) {
    if (op == SUBTRACT) {
      printf("parse: leading minus\n");
      rolling_sum *= -1;
    }
  } else {
    return -1;
  };

  while (true) {
    printf("parse: %p, %p\n", *pos, stop);
    enum operator op;
    long int num;

    if (parse_operator(stop, pos, &op))
      break;

    if (parse_number(stop, pos, &num))
      break;

    if (op == SUBTRACT) {
      rolling_sum -= num;
    } else if (op == ADD) {
      rolling_sum += num;
    } else {
      return -1;
    }
  }

  if (!parse_endl(stop, pos) && *pos != stop) {
    return -1;
  }

  *result = rolling_sum;

  return 0;
};
