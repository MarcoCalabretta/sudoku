#include "board.h"
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// See board.h for documentation

// a heatmap is a bool array that counts how many 1s are in the array
static struct heatmap {
  int len;
  int count;
  bool *map;
};

// makes a heatmap with all 0s of length len
// requires: len > 0
// effects: allocates heap memory, client must call heatmap_destroy
// time: O(len)
static struct heatmap *heatmap_create(int len) {
  assert(len > 0);
  struct heatmap *h = malloc(sizeof(struct heatmap));
  h->len = len;
  h->count = 0;
  h->map = malloc(len * sizeof(bool));
  for (int i = 0; i < len; i++) {
    (h->map)[i] = 0;
  }
  return h;
}

// destroys h and frees all associated memory
// requires: h is a valid pointer
// effects: frees heap memory, invalidated h
// time: O(1)
static void heatmap_destroy(struct heatmap *h) {
  assert(h);
  free(h->map);
  free(h);
}

// flips the valth value stored in h, either from 1 to 0 or from 0 to 1
// returns the value of the bool that you flipped, i.e. if you flip from 0 to 1
// it returns 0 requires: h is a valid pointer 		0 < val <= h->len
// effects: mutates *h
// time: O(1)
static bool heatmap_flip(struct heatmap *h, int val) {
  assert(h);
  assert(val > 0);
  assert(val <= h->len);
  if ((h->map)[val - 1]) {
    (h->count)--;
    (h->map)[val - 1] = 0;
    return 1;
  } else {
    (h->count)++;
    (h->map)[val - 1] = 1;
    return 0;
  }
}

// sets the valth value stored in h to 1
// requires: h is a valid pointer
// 	0 < val <= h->len
// effects: mutates *h
// time: O(1)
static void heatmap_on(struct heatmap *h, int val) {
  assert(h);
  assert(val > 0);
  assert(val <= h->len);
  if (!(h->map)[val - 1]) {
    (h->count)++;
  }
  (h->map)[val - 1] = 1;
}

// sets the valth value stored in h to 0
// requires: h is a valid pointer
// 	0 < val <= h->len
// effects: mutates *h
// time: O(1)
static void heatmap_off(struct heatmap *h, int val) {
  assert(h);
  assert(val > 0);
  assert(val <= h->len);
  if ((h->map)[val - 1]) {
    (h->count)--;
  }
  (h->map)[val - 1] = 0;
}

// returns the valth value stored in h
// requires: h is a valid pointer
// 		0 < val <= h->len
// time: O(1)
static bool heatmap_check(struct heatmap *h, int val) {
  assert(h);
  assert(val > 0);
  assert(val <= h->len);
  return (h->map)[val - 1];
}

// copies src to dest, not making any new memory
// effects: mutates dest through pointers
// requires: src and dest have the same length
static void heatmap_copy(struct heatmap *dest, struct heatmap *src) {
  assert(dest);
  assert(src);
  assert(dest->len == src->len);
  for (int i = 1; i <= dest->len; i++) {
    heatmap_off(dest, i);
    if (heatmap_check(src, i)) {
      heatmap_flip(dest, i);
    }
  }
}

// given row, col coords in a sudoku board of size n, returns which box (indexed
// at 0) those coords are in requires: size > 0 		0 <= row, col, <
// size time: O(1)
static int box_finder(int row, int col, int size) {
  assert(size > 0);
  assert(row >= 0);
  assert(row < size);
  assert(col >= 0);
  assert(col < size);
  int root = (int)sqrt((double)size);
  return root * (row / root) + col / root;
}

struct board {
  int size;
  int **data;
  int *appearances;
  int total_appearances;
  struct heatmap **rows;
  struct heatmap **cols;
  // boxs goes left to right, then up down. i.e boxs[2] on a 9X9 board is the
  // top right box
  struct heatmap **boxs;
  struct heatmap ***notes;
};

struct board *board_create(int size) {
  assert(size > 0);
  int root = (int)sqrt((double)size);
  assert(root * root == size);
  struct board *b = malloc(sizeof(struct board));
  b->size = size;
  b->total_appearances = 0;

  b->appearances = malloc(size * sizeof(int));
  for (int i = 0; i < size; i++) {
    (b->appearances)[i] = 0;
  }

  b->rows = malloc(size * sizeof(struct heatmap *));
  for (int i = 0; i < size; i++) {
    (b->rows)[i] = heatmap_create(size);
  }

  b->cols = malloc(size * sizeof(struct heatmap *));
  for (int i = 0; i < size; i++) {
    (b->cols)[i] = heatmap_create(size);
  }

  b->boxs = malloc(size * sizeof(struct heatmap *));
  for (int i = 0; i < size; i++) {
    (b->boxs)[i] = heatmap_create(size);
  }

  b->data = malloc(size * sizeof(int *));
  for (int i = 0; i < size; i++) {
    (b->data)[i] = malloc(size * sizeof(int));
    for (int j = 0; j < size; j++) {
      (b->data)[i][j] = 0;
    }
  }

  b->notes = malloc(size * sizeof(struct heatmap **));
  for (int i = 0; i < size; i++) {
    (b->notes)[i] = malloc(size * sizeof(struct heatmap *));
    for (int j = 0; j < size; j++) {
      (b->notes)[i][j] = heatmap_create(size);
    }
  }

  return b;
}

int board_get_size(struct board *b) {
  assert(b);
  return b->size;
}

void board_destroy(struct board *b) {
  assert(b);
  for (int i = 0; i < board_get_size(b); i++) {
    free((b->data)[i]);
    heatmap_destroy((b->rows)[i]);
    heatmap_destroy((b->cols)[i]);
    heatmap_destroy((b->boxs)[i]);
    for (int j = 0; j < board_get_size(b); j++) {
      heatmap_destroy((b->notes)[i][j]);
    }
    free((b->notes)[i]);
  }
  free(b->data);
  free(b->rows);
  free(b->cols);
  free(b->boxs);
  free(b->appearances);
  free(b->notes);
  free(b);
}

int board_insert(struct board *b, int row, int col, int val) {
  assert(b);
  assert(row > 0);
  assert(row <= board_get_size(b));
  assert(col > 0);
  assert(col <= board_get_size(b));
  assert(val > 0);
  assert(val <= board_get_size(b));
  if ((b->data)[row - 1][col - 1] != 0)
    return 1;
  // horizontal check
  if (heatmap_check((b->rows)[row - 1], val))
    return 2;
  // vertical check
  if (heatmap_check((b->cols)[col - 1], val))
    return 2;
  // box check
  int box = box_finder(row - 1, col - 1, board_get_size(b));
  if (heatmap_check((b->boxs)[box], val))
    return 2;

  (b->data)[row - 1][col - 1] = val;
  (b->appearances)[val - 1]++;
  heatmap_flip((b->rows)[row - 1], val);
  heatmap_flip((b->cols)[col - 1], val);
  heatmap_flip((b->boxs)[box], val);

  // remove notes for the inserted square, and across row and column
  for (int i = 0; i < board_get_size(b); i++) {
    heatmap_off((b->notes)[row - 1][col - 1], i + 1);
    heatmap_off((b->notes)[i][col - 1], val);
    heatmap_off((b->notes)[row - 1][i], val);
  }
  // remove notes for box
  int root = (int)sqrt((double)board_get_size(b));
  int box_row = root * (box / root);
  int box_col = root * (box % root);
  for (int i = box_row; i < box_row + root; i++) {
    for (int j = box_col; j < box_col + root; j++) {
      heatmap_off((b->notes)[i][j], val);
    }
  }

  b->total_appearances++;
  return 0;
}

void board_print(struct board *b) {
  assert(b);
  for (int i = 0; i < board_get_size(b); i++) {
    for (int k = 0; k < (2 * board_get_size(b) + 1); k++) {
      putchar('-');
    }
    putchar('\n');
    for (int j = 0; j < board_get_size(b); j++) {
      printf("|%d", (b->data)[i][j]);
    }
    printf("|\n");
  }
  for (int k = 0; k < (2 * board_get_size(b) + 1); k++) {
    putchar('-');
  }
  putchar('\n');
}

// makes notes for a whole board
// effects: mutates b through pointers
static void board_make_notes(struct board *b) {
  assert(b);
  int size = board_get_size(b);
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      for (int val = 1; val <= size; val++) {
        if (!(b->data)[i][j] && !heatmap_check((b->rows)[i], val) &&
            !heatmap_check((b->cols)[j], val) &&
            !heatmap_check((b->boxs)[box_finder(i, j, size)], val)) {
          heatmap_on((b->notes)[i][j], val);
        }
      }
    }
  }
}

// board_copy copies the contents of src to dest, without creating memory
// requires: src and dest must have the same size
static void board_copy(struct board *dest, struct board *src) {
  assert(src);
  assert(dest);
  assert(board_get_size(dest) == board_get_size(src));
  int size = board_get_size(src);
  struct heatmap *empty = heatmap_create(size);
  // empty dest's data
  dest->total_appearances = 0;
  for (int i = 0; i < size; i++) {
    heatmap_copy((dest->rows)[i], empty);
    heatmap_copy((dest->cols)[i], empty);
    heatmap_copy((dest->boxs)[i], empty);
    (dest->appearances)[i] = 0;
    for (int j = 0; j < size; j++) {
      (dest->data)[i][j] = 0;
    }
  }

  // fill dest with src
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      if ((src->data)[i][j]) {
        board_insert(dest, i + 1, j + 1, (src->data)[i][j]);
      } else {
        heatmap_copy((dest->notes)[i][j], (src->notes)[i][j]);
      }
    }
  }
}

// solves b with a correct sudoku solve. Returns 0 if it is solved, 1 if no
// solution was possible effects: mutates b time: ???
int board_solve(struct board *b, int n) {
  assert(b);
  int size = board_get_size(b);
  board_make_notes(b);
  int old_total_appearances = b->total_appearances - 1;
  while (b->total_appearances < size * size &&
         b->total_appearances > old_total_appearances) {
    old_total_appearances = b->total_appearances;
    for (int i = 0; i < size; i++) {
      int row = -1;
      int col = -1;
      int val = -1;
      // check for numbers with size-1 entries
      if ((b->appearances)[i] == size - 1) {
        row = -1;
        col = -1;
        val = i + 1;
        for (int j = 0; j < size; j++) {
          if (!heatmap_check((b->rows)[j], i + 1))
            row = j;
          if (!heatmap_check((b->cols)[j], i + 1))
            col = j;
          if (row >= 0 && col >= 0) {
            board_insert(b, row + 1, col + 1, val);
            break;
          }
        }
      }
      // check for rows with size-1 entries
      if ((b->rows)[i]->count == size - 1) {
        row = i;
        col = -1;
        val = -1;
        for (int j = 0; j < size; j++) {
          if (!heatmap_check((b->rows)[i], j + 1))
            val = j + 1;
          if ((b->data)[i][j] == 0)
            col = j;
          if (col >= 0 && val >= 0) {
            board_insert(b, row + 1, col + 1, val);
            break;
          }
        }
      }
      // check for cols with size-1 entries
      if ((b->cols)[i]->count == size - 1) {
        row = -1;
        col = i;
        val = -1;
        for (int j = 0; j < size; j++) {
          if (!heatmap_check((b->cols)[i], j + 1))
            val = j + 1;
          if ((b->data)[j][i] == 0)
            row = j;
          if (row >= 0 && val >= 0) {
            board_insert(b, row + 1, col + 1, val);
            break;
          }
        }
      }
      // check for boxs with size-1 entries
      if ((b->boxs)[i]->count == size - 1) {
        int root = (int)sqrt((double)board_get_size(b));
        int box_row = root * (i / root);
        int box_col = root * (i % root);
        row = -1;
        col = -1;
        val = -1;
        for (int j = 0; j < size; j++) {
          if (!heatmap_check((b->boxs)[i], j + 1))
            val = j + 1;
        }
        for (int i = box_row; i < box_row + root; i++) {
          for (int j = box_col; j < box_col + root; j++) {
            if ((b->data)[i][j] == 0) {
              row = i;
              col = j;
              board_insert(b, row + 1, col + 1, val);
              break;
            }
          }
        }
      }

      for (int j = 0; j < size; j++) {
        struct heatmap *h = (b->notes)[i][j];
        // checks for squares with only one possible number
        if (h->count == 1) {
          for (int val = 1; val <= size; val++) {
            if (heatmap_check(h, val)) {
              board_insert(b, i + 1, j + 1, val);
            }
          }
        }
        // checks for squares with no possible numbers (meaning the board is
        // unsolvable)
        else if (h->count == 0 && (b->data)[i][j] == 0) {
          return 1;
        }
      }

      // checks for rows, cols, or boxs where numbers exist only once in the
      // notes
      for (int val = 1; val <= size; val++) {
        for (int i = 0; i < size; i++) {
          if (!heatmap_check((b->rows)[i], val)) {
            int row_count = 0;
            int col = -1;
            for (int j = 0; j < size; j++) {
              if (heatmap_check((b->notes)[i][j], val)) {
                row_count++;
                col = j;
                if (row_count > 1)
                  break;
              }
            }
            if (row_count == 1) {
              board_insert(b, i + 1, col + 1, val);
            }
          }
          if (!heatmap_check((b->cols)[i], val)) {
            int col_count = 0;
            int row = -1;
            for (int j = 0; j < size; j++) {
              if (heatmap_check((b->notes)[j][i], val)) {
                col_count++;
                row = j;
                if (col_count > 1)
                  break;
              }
            }
            if (col_count == 1) {
              board_insert(b, row + 1, i + 1, val);
            }
          }
          if (!heatmap_check((b->boxs)[i], val)) {
            int box_count = 0;
            int root = (int)sqrt((double)size);
            int box_row = root * (i / root);
            int box_col = root * (i % root);
            int row = -1;
            int col = -1;
            for (int j = box_row; j < box_row + root; j++) {
              for (int k = box_col; k < box_col + root; k++) {
                if (heatmap_check((b->notes)[j][k], val)) {
                  box_count++;
                  row = j;
                  col = k;
                }
              }
              if (box_count > 1)
                break;
            }
            if (box_count == 1) {
              board_insert(b, row + 1, col + 1, val);
            }
          }
        }
      }
    }
  }
  if (b->total_appearances >= size * size) {
    return 0;
  }
  // now we guess! as of now we try and guess the choice with the least options
  // (i.e. only 2 numbers are possible)
  struct board *new_b = board_create(size);
  bool updated = 0;
  int row = -1;
  int col = -1;
  int val = -1;
  int guess_ret = 1;
  while (guess_ret == 1) {
    board_copy(new_b, b);
    row = -1;
    col = -1;
    val = -1;
    updated = 0;

    for (int num_possible = 1; num_possible < size; num_possible++) {
      // find coords
      for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
          // fill in guess
          if (((new_b->notes)[i][j])->count == num_possible) {
            for (int k = 1; k <= size; k++) {
              if (heatmap_check((new_b->notes)[i][j], k)) {
                row = i;
                col = j;
                val = k;
                board_insert(new_b, i + 1, j + 1, k);
                updated = 1;
                break;
              }
            }
          } else if ((new_b->data)[i][j] == 0 &&
                     ((new_b->notes)[i][j])->count == 0) {
            return 1;
          }
          if (updated)
            break;
        }
        if (updated)
          break;
      }
      if (updated)
        break;
    }
    guess_ret = board_solve(new_b, n + 1);
    if (guess_ret == 0) {
      board_copy(b, new_b);
      board_destroy(new_b);
      return guess_ret;
    } else {
      heatmap_off((b->notes)[row][col], val);
    }
  }
}
