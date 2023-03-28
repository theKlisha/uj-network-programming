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

enum operator{ ADD, SUB };

bool parse_expr(uint8_t const *stop, uint8_t const **pos, int16_t *result);
bool parse_operator(uint8_t const *stop, uint8_t const **pos,
                    enum operator* result);
bool parse_endl(uint8_t const *stop, uint8_t const **pos);
bool parse_number(uint8_t const *stop, uint8_t const **pos, int16_t *result);

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

    printf("request recived\n");

    uint8_t response_buf[32];
    int16_t result;
    const uint8_t *pos = request_buf;
    const uint8_t *end = request_buf + request_len;
    bool r = parse_expr(end, &pos, &result);
    parse_endl(end, &pos);

    if (r && pos == end) {
      printf("request parsed\n");
      sprintf((char *)response_buf, "%d", result);
    } else {
      sprintf((char *)response_buf, "ERROR");
      printf("error: invalid request\n");
    }

    ssize_t response_bytes_sent =
        sendto(sock, &response_buf, strlen((char *)response_buf), 0,
               (struct sockaddr *)&address, address_size);

    if (response_bytes_sent == -1) {
      perror("sendto");
      exit(EXIT_FAILURE);
    }

    printf("response sent\n");
  }

  rc = close(sock);
  if (rc == -1) {
    perror("close");
    exit(EXIT_FAILURE);
  }

  return 0;
}

bool parse_operator(uint8_t const *stop, uint8_t const **pos,
                    enum operator* result) {
  if (*pos >= stop)
    return false;

  char op = **(uint8_t **)pos;

  if (op == '+') {
    *result = ADD;
    *pos += 1;
    return true;
  } else if (op == '-') {
    *result = SUB;
    *pos += 1;
    return true;
  } else {
    return false;
  }
}

bool parse_endl(uint8_t const *stop, uint8_t const **pos) {
  if (*pos >= stop)
    return false;

  if (**pos == '\n') {
    *pos += 1;
    return true;
  } else if (**pos == '\r' && *pos - stop < 2 && *(*pos + 1) == '\n') {
    *pos += 2;
    return true;
  } else {
    return false;
  }
}

bool parse_number(uint8_t const *stop, uint8_t const **pos, int16_t *result) {
  if (*pos >= stop)
    return false;

  // ingore leading zeros
  bool leading_zeros = false;
  uint8_t const *offset = *pos;
  while (*offset == '0' && offset < stop) {
    offset += 1;
    leading_zeros = true;
  }

  int len = 0;
  uint8_t const *ptr = offset;
  while (*ptr >= '0' && *ptr <= '9' && ptr < stop) {
    ptr += 1;
    len += 1;
  }

  if (len == 0 && leading_zeros) {
    *result = 0;
    *pos = ptr;
    return true;
  } else if (len == 0 || len >= 8) {
    return false;
  }

  uint8_t num_str[8];
  num_str[len] = '\0';
  strncpy((char *)num_str, (char *)offset, len);
  int tmp;
  int rc = sscanf((char *)num_str, "%d", &tmp);

  if (rc == -1 || tmp > INT16_MAX || tmp < INT16_MIN) {
    return false;
  }

  *result = tmp;
  *pos = ptr;

  return true;
}

bool parse_expr(uint8_t const *stop, uint8_t const **pos, int16_t *result) {
  if (*pos >= stop)
    return false;

  uint8_t const *ptr = *pos;
  int16_t sum = 0;

  if (!parse_number(stop, &ptr, &sum)) {
    return false;
  };

  while (true) {
    enum operator op;
    int16_t num;
    uint8_t const *p = ptr;

    if (!parse_operator(stop, &p, &op))
      break;

    if (!parse_number(stop, &p, &num))
      break;

    ptr = p;

    if (op == SUB) {
      if (num < 0 && sum > INT16_MAX + num) {
        return false;
      }
      if (num > 0 && sum < INT16_MIN + num) {
        return false;
      }

      sum -= num;
    } else if (op == ADD) {
      if (num > 0 && sum > INT16_MAX - num) {
        return false;
      }
      if (num < 0 && sum < INT16_MIN - num) {
        return false;
      }

      sum += num;
    } else {
      // should never happen
      return false;
    }
  }

  *result = sum;
  *pos = ptr;
  return true;
}
