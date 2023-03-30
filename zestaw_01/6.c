#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  int from_fd = open(argv[1], O_RDONLY);
  int to_fd = open(argv[2], O_WRONLY | O_CREAT, S_IRWXU);
  char buf[1024];

  while (true) {
    size_t bytes_red = 0;
    size_t bytes_written = 0;

    bytes_red = read(from_fd, &buf, 1024);
    if (bytes_red < 1)
      break;

    bytes_written = write(to_fd, &buf, bytes_red);
    if (bytes_written < 1)
      break;
  }

  close(from_fd);
  close(to_fd);
}
