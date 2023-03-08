#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

void drukuj_alt(int *tablica, int i);

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
