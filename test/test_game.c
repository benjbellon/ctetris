#include "game.c"
#include "game.h"
#include "unity.h"
#include <assert.h>
#include <stdlib.h>

void setUp(void) {}
void tearDown(void) {}

void ASSERT_BOARD_ELEMENTS_EQUAL(GameBoard_t *expected, GameBoard_t *actual, Tetromino_t *filter) {
  assert(expected->rows == actual->rows && expected->cols == actual->cols);

  char msg[100];
  for (size_t row = 0; row < actual->rows; row++) {
    for (size_t col = 0; col < actual->cols; col++) {
      if (filter && expected->arr[row][col] != filter) {
        continue;
      }

      sprintf(msg, "@ [row][col]: [%lu][%lu]", row, col);
      TEST_ASSERT_EQUAL_PTR_MESSAGE(expected->arr[row][col], actual->arr[row][col], msg);
    }
  }
}

void test_tetromino_collection_contains_active() {
  TetrominoCollection_t *coll = TetrominoCollection_init(3);
  Tetromino_t *t1 = Tetromino_init(TETROMINO_SHAPE_TAG_I, 0, 0);
  Tetromino_t *t2 = Tetromino_init(TETROMINO_SHAPE_TAG_J, 0, 0);
  Tetromino_t *t3 = Tetromino_init(TETROMINO_SHAPE_TAG_L, 0, 0);

  TetrominoCollection_push(coll, t1);
  TetrominoCollection_push(coll, t2);
  TetrominoCollection_push(coll, t3);

  t1->state = TETROMINO_STATE_LOCKED;
  t2->state = TETROMINO_STATE_LOCKED;

  TEST_ASSERT_EQUAL_INT(true, TetrominoCollection_contains_active(coll));
}

void test_get_tetromino_coords() {
  GameBoard_t *actual = GameBoard_init(3, 5);
  Tetromino_t *tet = Tetromino_init(TETROMINO_SHAPE_TAG_I, 0, 0);

  TetrominoCollection_t *col = TetrominoCollection_init(3);
  TetrominoCollection_push(col, tet);

  Tetromino_t *initial[3][5] = {
      {tet, tet, tet, tet, NULL}, {NULL, NULL, NULL, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL}};

  int expected[] = {0, 0, 0, 1, 0, 2, 0, 3};

  for (size_t row = 0; row < actual->rows; row++) {
    memcpy(actual->arr[row], initial[row], actual->cols * sizeof(Tetromino_t *));
  }

  int *coords = GameBoard_get_tetromino_coords(actual, tet);

  TEST_ASSERT_EQUAL_INT_ARRAY(expected, coords, TETROMINO_MAP_SIZE);
}

void test_translate_right() {
  GameBoard_t *board = GameBoard_init(3, 5);
  Tetromino_t *tet = Tetromino_init(TETROMINO_SHAPE_TAG_I, 0, 0);

  TetrominoCollection_t *col = TetrominoCollection_init(3);
  TetrominoCollection_push(col, tet);

  Tetromino_t *initial[3][5] = {
      {tet, tet, tet, tet, NULL}, {NULL, NULL, NULL, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL}};

  Tetromino_t *expected[3][5] = {
      {NULL, tet, tet, tet, tet}, {NULL, NULL, NULL, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL}};

  for (size_t row = 0; row < board->rows; row++) {
    memcpy(board->arr[row], initial[row], board->cols * sizeof(Tetromino_t *));
  }

  GameBoard_translate_right(board, tet);

  char msg[100];
  for (size_t row = 0; row < board->rows; row++) {
    for (size_t col = 0; col < board->cols; col++) {
      sprintf(msg, "row: %lu col: %lu", row, col);
      TEST_ASSERT_EQUAL_PTR_MESSAGE(expected[row][col], board->arr[row][col], msg);
    }
  }
}

void test_translate_left() {
  GameBoard_t *actual = GameBoard_init(10, 15);
  GameBoard_t *expected = GameBoard_init(10, 15);

  // TODO: create game board, insert piece, shift and check

  Tetromino_t *I = Tetromino_init(TETROMINO_SHAPE_TAG_I, 1, 1);
  Tetromino_t *J = Tetromino_init(TETROMINO_SHAPE_TAG_J, 6, 1);
  Tetromino_t *L = Tetromino_init(TETROMINO_SHAPE_TAG_L, 6, 7);
  Tetromino_t *O = Tetromino_init(TETROMINO_SHAPE_TAG_O, 4, 10);
  Tetromino_t *S = Tetromino_init(TETROMINO_SHAPE_TAG_S, 7, 13);
  Tetromino_t *T = Tetromino_init(TETROMINO_SHAPE_TAG_T, 1, 7);
  Tetromino_t *Z = Tetromino_init(TETROMINO_SHAPE_TAG_Z, 7, 8);

  TetrominoCollection_t *col = TetrominoCollection_init(7);
  TetrominoCollection_push(col, I);
  TetrominoCollection_push(col, T);
  TetrominoCollection_push(col, J);
  TetrominoCollection_push(col, L);
  TetrominoCollection_push(col, O);
  TetrominoCollection_push(col, S);
  TetrominoCollection_push(col, Z);

  // clang-format off
  Tetromino_t *initial[10][15] = {
      {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
      {NULL,    I,    I,    I,    I, NULL, NULL,    T, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
      {NULL, NULL, NULL, NULL, NULL, NULL,    T,    T,    T, NULL, NULL, NULL, NULL, NULL, NULL},
      {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
      {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,    O,    O, NULL, NULL, NULL},
      {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,    O,    O, NULL, NULL, NULL},
      {NULL,    J, NULL, NULL, NULL, NULL, NULL,    L, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
      {NULL,    J,    J,    J, NULL,    L,    L,    L,    Z,    Z, NULL, NULL, NULL,    S,    S},
      {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,    Z,    Z, NULL,    S,    S, NULL},
      {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}};
  // clang-format on

  // clang-format off
  Tetromino_t *updated[10][15] = {
      {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
      {   I,    I,    I,    I, NULL, NULL,    T, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
      {NULL, NULL, NULL, NULL, NULL,    T,    T,    T, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
      {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
      {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,    O,    O, NULL, NULL, NULL, NULL},
      {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,    O,    O, NULL, NULL, NULL, NULL},
      {   J, NULL, NULL, NULL, NULL, NULL,    L, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
      {   J,    J,    J, NULL,    L,    L,    L,    Z,    Z, NULL, NULL, NULL,    S,    S, NULL},
      {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,    Z,    Z, NULL,    S,    S, NULL, NULL},
      {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}};
  // clang-format on

  for (size_t row = 0; row < actual->rows; row++) {
    memcpy(actual->arr[row], initial[row], actual->cols * sizeof(Tetromino_t *));
    memcpy(expected->arr[row], updated[row], expected->cols * sizeof(Tetromino_t *));
  }

  GameBoard_translate_left(actual, I);
  ASSERT_BOARD_ELEMENTS_EQUAL(expected, actual, I);

  GameBoard_translate_left(actual, J);
  ASSERT_BOARD_ELEMENTS_EQUAL(expected, actual, J);

  GameBoard_translate_left(actual, L);
  ASSERT_BOARD_ELEMENTS_EQUAL(expected, actual, L);

  GameBoard_translate_left(actual, O);
  ASSERT_BOARD_ELEMENTS_EQUAL(expected, actual, O);

  GameBoard_translate_left(actual, S);
  ASSERT_BOARD_ELEMENTS_EQUAL(expected, actual, S);

  GameBoard_translate_left(actual, T);
  ASSERT_BOARD_ELEMENTS_EQUAL(expected, actual, T);

  GameBoard_translate_left(actual, Z);
  ASSERT_BOARD_ELEMENTS_EQUAL(expected, actual, Z);
}

void test_translate_down() {
  GameBoard_t *actual = GameBoard_init(10, 15);
  GameBoard_t *expected = GameBoard_init(10, 15);

  // TODO: create game board, insert piece, shift and check

  Tetromino_t *I = Tetromino_init(TETROMINO_SHAPE_TAG_I, 1, 1);
  Tetromino_t *J = Tetromino_init(TETROMINO_SHAPE_TAG_J, 6, 1);
  Tetromino_t *L = Tetromino_init(TETROMINO_SHAPE_TAG_L, 6, 7);
  Tetromino_t *O = Tetromino_init(TETROMINO_SHAPE_TAG_O, 4, 10);
  Tetromino_t *S = Tetromino_init(TETROMINO_SHAPE_TAG_S, 7, 13);
  Tetromino_t *T = Tetromino_init(TETROMINO_SHAPE_TAG_T, 1, 7);
  Tetromino_t *Z = Tetromino_init(TETROMINO_SHAPE_TAG_Z, 7, 8);

  TetrominoCollection_t *col = TetrominoCollection_init(7);
  TetrominoCollection_push(col, I);
  TetrominoCollection_push(col, T);
  TetrominoCollection_push(col, J);
  TetrominoCollection_push(col, L);
  TetrominoCollection_push(col, O);
  TetrominoCollection_push(col, S);
  TetrominoCollection_push(col, Z);

  // clang-format off
  Tetromino_t *initial[10][15] = {
      {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
      {NULL,    I,    I,    I,    I, NULL, NULL,    T, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
      {NULL, NULL, NULL, NULL, NULL, NULL,    T,    T,    T, NULL, NULL, NULL, NULL, NULL, NULL},
      {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
      {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,    O,    O, NULL, NULL, NULL},
      {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,    O,    O, NULL, NULL, NULL},
      {NULL,    J, NULL, NULL, NULL, NULL, NULL,    L, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
      {NULL,    J,    J,    J, NULL,    L,    L,    L,    Z,    Z, NULL, NULL, NULL,    S,    S},
      {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,    Z,    Z, NULL,    S,    S, NULL},
      {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}
  };
  // clang-format on

  // clang-format off
  Tetromino_t *updated[10][15] = {
      {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
      {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
      {NULL,    I,    I,    I,    I, NULL, NULL,    T, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
      {NULL, NULL, NULL, NULL, NULL, NULL,    T,    T,    T, NULL, NULL, NULL, NULL, NULL, NULL},
      {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
      {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,    O,    O, NULL, NULL, NULL},
      {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,    O,    O, NULL, NULL, NULL},
      {NULL,    J, NULL, NULL, NULL, NULL, NULL,    L, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
      {NULL,    J,    J,    J, NULL,    L,    L,    L,    Z,    Z, NULL, NULL, NULL,    S,    S},
      {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,    Z,    Z, NULL,    S,    S, NULL}
  };
  // clang-format on

  for (size_t row = 0; row < actual->rows; row++) {
    memcpy(actual->arr[row], initial[row], actual->cols * sizeof(Tetromino_t *));
    memcpy(expected->arr[row], updated[row], expected->cols * sizeof(Tetromino_t *));
  }

  GameBoard_translate_down(actual, I);
  ASSERT_BOARD_ELEMENTS_EQUAL(expected, actual, I);

  GameBoard_translate_down(actual, J);
  ASSERT_BOARD_ELEMENTS_EQUAL(expected, actual, J);

  GameBoard_translate_down(actual, L);
  ASSERT_BOARD_ELEMENTS_EQUAL(expected, actual, L);

  GameBoard_translate_down(actual, O);
  ASSERT_BOARD_ELEMENTS_EQUAL(expected, actual, O);

  GameBoard_translate_down(actual, S);
  ASSERT_BOARD_ELEMENTS_EQUAL(expected, actual, S);

  GameBoard_translate_down(actual, T);
  ASSERT_BOARD_ELEMENTS_EQUAL(expected, actual, T);

  GameBoard_translate_down(actual, Z);
  ASSERT_BOARD_ELEMENTS_EQUAL(expected, actual, Z);
}

void test_rotate_tetromino_matrix_pi() {
  int expected[TETROMINO_MAP_SIZE] = {0};

  TetrominoMatrix mat =
      (TetrominoMatrix){.row0 = 0, .col0 = 0, .t = TETROMINO_SHAPE_TAG_I, .size = 4, .map = TETROMINO_MAP_I};
  memcpy(expected, (int[]){0, 2, 1, 2, 2, 2, 3, 2}, sizeof(int) * TETROMINO_MAP_SIZE);
  int *actual = TetrominoMatrix_rotate_map_1pi(&mat);

  TEST_ASSERT_EQUAL_INT_ARRAY(expected, actual, 8);
  memset(expected, 0, sizeof(int) * TETROMINO_MAP_SIZE);
  free(actual);

  mat = (TetrominoMatrix){.row0 = 0, .col0 = 0, .t = TETROMINO_SHAPE_TAG_I, .size = 3, .map = TETROMINO_MAP_J};
  memcpy(expected, (int[]){0, 2, 0, 1, 1, 1, 2, 1}, sizeof(int) * TETROMINO_MAP_SIZE);
  actual = TetrominoMatrix_rotate_map_1pi(&mat);

  TEST_ASSERT_EQUAL_INT_ARRAY(expected, actual, 8);
  memset(expected, 0, sizeof(int) * TETROMINO_MAP_SIZE);
  free(actual);
}

int main(int argc, char *argv[]) {
  UNITY_BEGIN();

  /* RUN_TEST(test_collision_check); */
  /* RUN_TEST(test_collision_check_with_intersection); */
  RUN_TEST(test_tetromino_collection_contains_active);
  RUN_TEST(test_get_tetromino_coords);
  RUN_TEST(test_translate_down);
  RUN_TEST(test_translate_right);
  RUN_TEST(test_translate_left);
  RUN_TEST(test_rotate_tetromino_matrix_pi);

  return UNITY_END();
}
