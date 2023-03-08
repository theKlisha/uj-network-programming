#include <stdio.h>
#include <stdlib.h>

void drukuj(int tablica[], int i);

int main() {
  int liczby[50];
  int i = 0;

  while (1) {
    scanf("%d", &liczby[i]);
    i += 1;

    if (liczby[i - 1] == 0 || i >= 50)
      break;
  }

  drukuj(liczby, i);
}

void drukuj(int tablica[], int i) {
  for (int j = 0; j < i; j++) {
    if (tablica[j] >= 10 && tablica[j] <= 100)
      printf("%d\n", tablica[j]);
  }
}
