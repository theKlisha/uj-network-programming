#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

void drukuj_alt(int *tablica, int i);
bool printable_buf(const void *buf, int len);
bool printable_str(const char *buf, int len);

int main() {
  int liczby[50];
  int i = 0;

  while (1) {
    scanf("%d", &liczby[i]);
    i += 1;

    if (liczby[i - 1] == 0 || i >= 50)
      break;
  }

  drukuj_alt(liczby, i);
}

void drukuj_alt(int *tablica, int i) {
  for (int j = 0; j < i; j++) {
    int t = *(tablica + j);
    if (t >= 10 && t <= 100)
      printf("%d\n", t);
  }
}

bool printable_buf(const void *buf, int len) {
  const void *p = buf;

  while (p < buf + len) {
    if (*(char *)p < 32 || *(char *)p > 126)
      return false;
    p++;
  }

  return true;
}

bool printable_str(const char *buf, int len) {
  const char *p = buf;

  while (p < buf + len) {
    if (*p < 32 || *p > 126 || *p == 0)
      return false;
    p++;
  }

  return true;
}
