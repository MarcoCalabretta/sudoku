// This module controls a sudoku board struct and has a bunch of functions
// relating to it made by Marco Calabretta on June 15 2023

// all functions require valid pointers

// a board is a sudoku board
struct board;

// creates a new board with size rows and columns (i.e. size = 9 is a normal 9X9
// board) and returns a pointer to it requires: size > 0 	     size is a
// square number, e.g 4, 9, 16 effects: allocates heap memory, client must call
// board_destroy time: O(n^3), where n = size
struct board *board_create(int size);

// returns the size (number of rows) of b
// time: O(1)
int board_get_size(struct board *b);

// destroys board b
// effects: frees heap memory
// time: O(n^2), where n is the size of b
void board_destroy(struct board *b);

// inserts val into b at row and column. returns 0 if val was inserted
// successfully, 1 if there was already a number at row, column or 2 if
// inserting val causes a contradition (for example if there is an identical
// number in the same row)
//
// requires: 0 < row, column, val <= board_get_size(b)
// effects: mutates b
// time: O(n), where n is the size of b
int board_insert(struct board *b, int row, int col, int val);

// prints b, with all the filled in numbers
// time: O(n^2), where n is the size of b
void board_print(struct board *b);

// solves b with a correct sudoku solve. Returns 1 if it is solved, 0 if no
// solution was possible effects: mutates b time: ???
int board_solve(struct board *b, int n);
