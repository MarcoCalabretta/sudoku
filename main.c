#include "board.h"
#include <stdio.h>

int main() {
  int size = 9;
    printf("Insert size: ");
	scanf("%d", &size);

  struct board *b = board_create(size);
  int val = size + 1;
  int row = 0;
  int col = 0;
  while (val != 0) {
    printf("Insert val (insert 0 to stop): ");
    while (scanf("%d", &val) != 1 || val < 0 || val > size) {
      printf("Error. Must be a whole number between 0 and %d.\n", size);
    }
    if (val == 0)
      break;
    printf("Insert row: ");
    while (scanf("%d", &row) != 1 || row < 0 || row > size)
      printf("Error. Must be a whole number between 1 and %d.\n", size);
    printf("Insert column: ");
    while (scanf("%d", &col) != 1 || col < 0 || col > size)
      printf("Error. Must be a whole number between 1 and %d.\n", size);
    printf("%d\n", board_insert(b, row, col, val));
  }
  putchar('\n');
  int result = board_solve(b, 1);
  board_print(b);
  board_destroy(b);
  printf("%d\n", result);
}
