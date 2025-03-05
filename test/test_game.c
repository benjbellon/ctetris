#include "game.h"
#include "unity.h"
#include <assert.h>

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

void test_step_matrix_down() {

  GameBoard_t *board = GameBoard_init(3, 5);
  Tetromino_t *dummy = Tetromino_init(TETROMINO_SHAPE_TAG_T, 0, 0);

  TetrominoCollection_t *col = TetrominoCollection_init(1);
  TetrominoCollection_push(col, dummy);

  Tetromino_t *under_test[3][5] = {
      {NULL, NULL, dummy, dummy, dummy}, {NULL, NULL, NULL, dummy, NULL}, {NULL, NULL, NULL, NULL, NULL}};

  Tetromino_t *expected[3][5] = {
      {NULL, NULL, NULL, NULL, NULL},
      {NULL, NULL, dummy, dummy, dummy},
      {NULL, NULL, NULL, dummy, NULL},
  };

  for (size_t row = 0; row < board->rows; row++) {
    memcpy(board->arr[row], under_test[row], board->cols * sizeof(Tetromino_t *));
  }

  GameBoard_step_down_active_tetromino(board, col);

  for (size_t row = 0; row < board->rows; row++) {
    for (size_t col = 0; col < board->cols; col++) {
      TEST_ASSERT_EQUAL_PTR_MESSAGE(expected[row][col], board->arr[row][col],
                                    "Elements should be equal after stepdown");
    }
  }
}

void test_collision_check() {
  GameBoard_t *board = GameBoard_init(3, 5);
  GameBoard_t *prev = GameBoard_init(3, 5);
  Tetromino_t *not_locked = Tetromino_init(TETROMINO_SHAPE_TAG_T, 0, 0);
  Tetromino_t *locked = Tetromino_init(TETROMINO_SHAPE_TAG_I, 0, 0);

  TetrominoCollection_t *col = TetrominoCollection_init(2);
  TetrominoCollection_push(col, locked);
  TetrominoCollection_push(col, not_locked);

  Tetromino_t *board_in[3][5] = {{locked, NULL, not_locked, not_locked, not_locked},
                                 {locked, NULL, NULL, not_locked, NULL},
                                 {locked, NULL, NULL, NULL, NULL}};

  Tetromino_t *prev_board_in[3][5] = {{NULL, NULL, not_locked, not_locked, not_locked},
                                      {NULL, NULL, NULL, not_locked, NULL},
                                      {NULL, NULL, NULL, NULL, NULL}};

  for (size_t row = 0; row < board->rows; row++) {
    memcpy(board->arr[row], board_in[row], board->cols * sizeof(Tetromino_t *));
  }

  for (size_t row = 0; row < board->rows; row++) {
    memcpy(prev->arr[row], prev_board_in[row], board->cols * sizeof(Tetromino_t *));
  }

  GameBoard_collision_check(board, prev);

  TEST_ASSERT_EQUAL_INT_MESSAGE(TETROMINO_STATE_ACTIVE, not_locked->state, "Expected not_locked to be active");
  TEST_ASSERT_EQUAL_INT_MESSAGE(TETROMINO_STATE_LOCKED, locked->state, "Expected locked to be locked");
}

void test_collision_check_with_intersection() {
  GameBoard_t *board = GameBoard_init(3, 5);
  GameBoard_t *prev = GameBoard_init(3, 5);
  Tetromino_t *not_locked = Tetromino_init(TETROMINO_SHAPE_TAG_T, 0, 0);
  Tetromino_t *locked = Tetromino_init(TETROMINO_SHAPE_TAG_I, 0, 0);
  Tetromino_t *another = Tetromino_init(TETROMINO_SHAPE_TAG_I, 0, 0);

  TetrominoCollection_t *col = TetrominoCollection_init(3);
  TetrominoCollection_push(col, locked);
  TetrominoCollection_push(col, not_locked);
  TetrominoCollection_push(col, another);

  Tetromino_t *board_in[3][5] = {{locked, NULL, not_locked, not_locked, not_locked},
                                 {locked, NULL, NULL, not_locked, NULL},
                                 {locked, NULL, NULL, NULL, NULL}};

  Tetromino_t *prev_board_in[3][5] = {{another, NULL, not_locked, not_locked, not_locked},
                                      {another, NULL, NULL, not_locked, NULL},
                                      {another, NULL, NULL, NULL, NULL}};

  for (size_t row = 0; row < board->rows; row++) {
    memcpy(board->arr[row], board_in[row], board->cols * sizeof(Tetromino_t *));
  }

  for (size_t row = 0; row < board->rows; row++) {
    memcpy(prev->arr[row], prev_board_in[row], board->cols * sizeof(Tetromino_t *));
  }

  GameBoard_collision_check(board, prev);

  TEST_ASSERT_EQUAL_INT_MESSAGE(TETROMINO_STATE_ACTIVE, not_locked->state, "Expected not_locked to be active");
  TEST_ASSERT_EQUAL_INT_MESSAGE(TETROMINO_STATE_INTERSECTED, another->state, "Expected another to be intersected");
  TEST_ASSERT_EQUAL_INT_MESSAGE(TETROMINO_STATE_INTERSECTED, locked->state, "Expected locked to be intersected");
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

void test_rotate_right_pi_radians() {
  GameBoard_t *board = GameBoard_init(3, 5);
  Tetromino_t *tet = Tetromino_init(TETROMINO_SHAPE_TAG_I, 0, 0);

  TetrominoCollection_t *col = TetrominoCollection_init(3);
  TetrominoCollection_push(col, tet);

  Tetromino_t *initial[3][5] = {
      {NULL, NULL, tet, NULL, NULL}, {NULL, NULL, tet, NULL, NULL}, {NULL, NULL, tet, NULL, NULL}};

  Tetromino_t *expected[3][5] = {
      {NULL, NULL, NULL, NULL, NULL}, {NULL, tet, tet, tet, NULL}, {NULL, NULL, NULL, NULL, NULL}};

  for (size_t row = 0; row < board->rows; row++) {
    memcpy(board->arr[row], initial[row], board->cols * sizeof(Tetromino_t *));
  }

  GameBoard_rotate_pi_radians(board, tet);
}

void test_get_tetromino_coords() {
  GameBoard_t *actual = GameBoard_init(3, 5);
  Tetromino_t *tet = Tetromino_init(TETROMINO_SHAPE_TAG_I, 0, 0);

  TetrominoCollection_t *col = TetrominoCollection_init(3);
  TetrominoCollection_push(col, tet);

  Tetromino_t *initial[3][5] = {
      {tet, tet, tet, tet, NULL}, {NULL, NULL, NULL, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL}};

  int expected[] = {-1, 0, 0, 0, 0, 1, 0, 2, 0, 3};

  for (size_t row = 0; row < actual->rows; row++) {
    memcpy(actual->arr[row], initial[row], actual->cols * sizeof(Tetromino_t *));
  }

  int *coords = GameBoard_get_tetromino_coords(actual, tet);

  TEST_ASSERT_EQUAL_INT_ARRAY(expected, coords, 10);
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

  Tetromino_t *I = Tetromino_init(TETROMINO_SHAPE_TAG_I, 0, 0);
  Tetromino_t *T = Tetromino_init(TETROMINO_SHAPE_TAG_T, 0, 0);
  Tetromino_t *J = Tetromino_init(TETROMINO_SHAPE_TAG_J, 0, 0);
  Tetromino_t *L = Tetromino_init(TETROMINO_SHAPE_TAG_L, 0, 0);
  Tetromino_t *O = Tetromino_init(TETROMINO_SHAPE_TAG_O, 0, 0);
  Tetromino_t *S = Tetromino_init(TETROMINO_SHAPE_TAG_S, 0, 0);
  Tetromino_t *Z = Tetromino_init(TETROMINO_SHAPE_TAG_Z, 0, 0);

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

int main(int argc, char *argv[]) {
  UNITY_BEGIN();

  RUN_TEST(test_step_matrix_down);
  RUN_TEST(test_collision_check);
  RUN_TEST(test_collision_check_with_intersection);
  RUN_TEST(test_tetromino_collection_contains_active);
  RUN_TEST(test_get_tetromino_coords);
  RUN_TEST(test_translate_right);
  RUN_TEST(test_translate_left);
  /* RUN_TEST(test_rotate_right_pi_radians); */

  return UNITY_END();
}
