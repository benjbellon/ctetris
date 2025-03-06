#include "game.c"
#include "game.h"
#include "unity.h"
#include <assert.h>
#include <stdlib.h>

static const int BOARD_ROWS = 30;
static const int BOARD_COLS = 15;
static GameBoard_t *BOARD_ACTUAL = NULL;
static GameBoard_t *BOARD_EXPECTED = NULL;

void setUp(void) {
  BOARD_ACTUAL = GameBoard_init(BOARD_ROWS, BOARD_COLS);
  BOARD_EXPECTED = GameBoard_init(BOARD_ROWS, BOARD_COLS);
}

void tearDown(void) {
  GameBoard_free(BOARD_ACTUAL);
  GameBoard_free(BOARD_EXPECTED);
  BOARD_ACTUAL = NULL;
  BOARD_EXPECTED = NULL;
}

void _th_GameBoard_insert_tetromino(GameBoard_t *board, Tetromino_t *t, int const row_shift, int const col_shift) {

  if (GameBoard_collision(board, t, 0, 0)) {
    assert(false && "cannot insert on top of existing tetromino");
  }

  for (size_t i = 0; i < TETROMINO_MAP_SIZE; i = i + 2) {
    int r = t->mat->map[i] + t->mat->row0;
    int c = t->mat->map[i + 1] + t->mat->col0;

    board->arr[r + row_shift][c + col_shift] = t;
  }
}

void TEST_ASSERT_BOARD_ELEMENTS_EQUAL(GameBoard_t *expected, GameBoard_t *actual, Tetromino_t *filter) {
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
  TetrominoCollection_t *coll = TetrominoCollection_init(3);
  Tetromino_t *I = Tetromino_init(TETROMINO_SHAPE_TAG_I, 0, 0);
  TetrominoCollection_push(coll, I);

  GameBoard_spawn_tetromino(&BOARD_ACTUAL, coll, 0, 0);
  int expected[] = {0, 0, 0, 1, 0, 2, 0, 3};

  int *actual = GameBoard_get_tetromino_coords(BOARD_ACTUAL, I);
  TEST_ASSERT_EQUAL_INT_ARRAY(expected, actual, TETROMINO_MAP_SIZE);
}

void test_translate_right() {
  Tetromino_t *I = Tetromino_init(TETROMINO_SHAPE_TAG_I, 5, 1);
  Tetromino_t *J = Tetromino_init(TETROMINO_SHAPE_TAG_J, 8, 1);
  Tetromino_t *L = Tetromino_init(TETROMINO_SHAPE_TAG_L, 11, 6);
  Tetromino_t *O = Tetromino_init(TETROMINO_SHAPE_TAG_O, 16, 7);
  Tetromino_t *S = Tetromino_init(TETROMINO_SHAPE_TAG_S, 20, 7);
  Tetromino_t *T = Tetromino_init(TETROMINO_SHAPE_TAG_T, 27, 2);
  Tetromino_t *Z = Tetromino_init(TETROMINO_SHAPE_TAG_Z, 27, 10);

  _th_GameBoard_insert_tetromino(BOARD_ACTUAL, I, 0, 0);
  _th_GameBoard_insert_tetromino(BOARD_EXPECTED, I, 0, 1);

  _th_GameBoard_insert_tetromino(BOARD_ACTUAL, J, 0, 0);
  _th_GameBoard_insert_tetromino(BOARD_EXPECTED, J, 0, 1);

  _th_GameBoard_insert_tetromino(BOARD_ACTUAL, L, 0, 0);
  _th_GameBoard_insert_tetromino(BOARD_EXPECTED, L, 0, 1);

  _th_GameBoard_insert_tetromino(BOARD_ACTUAL, O, 0, 0);
  _th_GameBoard_insert_tetromino(BOARD_EXPECTED, O, 0, 1);

  _th_GameBoard_insert_tetromino(BOARD_ACTUAL, S, 0, 0);
  _th_GameBoard_insert_tetromino(BOARD_EXPECTED, S, 0, 1);

  _th_GameBoard_insert_tetromino(BOARD_ACTUAL, T, 0, 0);
  _th_GameBoard_insert_tetromino(BOARD_EXPECTED, T, 0, 1);

  _th_GameBoard_insert_tetromino(BOARD_ACTUAL, Z, 0, 0);
  _th_GameBoard_insert_tetromino(BOARD_EXPECTED, Z, 0, 1);

  GameBoard_translate_right(BOARD_ACTUAL, I);
  TEST_ASSERT_BOARD_ELEMENTS_EQUAL(BOARD_EXPECTED, BOARD_ACTUAL, I);

  GameBoard_translate_right(BOARD_ACTUAL, J);
  TEST_ASSERT_BOARD_ELEMENTS_EQUAL(BOARD_EXPECTED, BOARD_ACTUAL, J);

  GameBoard_translate_right(BOARD_ACTUAL, L);
  TEST_ASSERT_BOARD_ELEMENTS_EQUAL(BOARD_EXPECTED, BOARD_ACTUAL, L);

  GameBoard_translate_right(BOARD_ACTUAL, O);
  TEST_ASSERT_BOARD_ELEMENTS_EQUAL(BOARD_EXPECTED, BOARD_ACTUAL, O);

  GameBoard_translate_right(BOARD_ACTUAL, S);
  TEST_ASSERT_BOARD_ELEMENTS_EQUAL(BOARD_EXPECTED, BOARD_ACTUAL, S);

  GameBoard_translate_right(BOARD_ACTUAL, T);
  TEST_ASSERT_BOARD_ELEMENTS_EQUAL(BOARD_EXPECTED, BOARD_ACTUAL, T);

  GameBoard_translate_right(BOARD_ACTUAL, Z);
  TEST_ASSERT_BOARD_ELEMENTS_EQUAL(BOARD_EXPECTED, BOARD_ACTUAL, Z);
}

void test_translate_left() {
  Tetromino_t *I = Tetromino_init(TETROMINO_SHAPE_TAG_I, 5, 1);
  Tetromino_t *J = Tetromino_init(TETROMINO_SHAPE_TAG_J, 8, 1);
  Tetromino_t *L = Tetromino_init(TETROMINO_SHAPE_TAG_L, 11, 6);
  Tetromino_t *O = Tetromino_init(TETROMINO_SHAPE_TAG_O, 16, 7);
  Tetromino_t *S = Tetromino_init(TETROMINO_SHAPE_TAG_S, 20, 7);
  Tetromino_t *T = Tetromino_init(TETROMINO_SHAPE_TAG_T, 27, 2);
  Tetromino_t *Z = Tetromino_init(TETROMINO_SHAPE_TAG_Z, 27, 10);

  _th_GameBoard_insert_tetromino(BOARD_ACTUAL, I, 0, 0);
  _th_GameBoard_insert_tetromino(BOARD_EXPECTED, I, 0, -1);

  _th_GameBoard_insert_tetromino(BOARD_ACTUAL, J, 0, 0);
  _th_GameBoard_insert_tetromino(BOARD_EXPECTED, J, 0, -1);

  _th_GameBoard_insert_tetromino(BOARD_ACTUAL, L, 0, 0);
  _th_GameBoard_insert_tetromino(BOARD_EXPECTED, L, 0, -1);

  _th_GameBoard_insert_tetromino(BOARD_ACTUAL, O, 0, 0);
  _th_GameBoard_insert_tetromino(BOARD_EXPECTED, O, 0, -1);

  _th_GameBoard_insert_tetromino(BOARD_ACTUAL, S, 0, 0);
  _th_GameBoard_insert_tetromino(BOARD_EXPECTED, S, 0, -1);

  _th_GameBoard_insert_tetromino(BOARD_ACTUAL, T, 0, 0);
  _th_GameBoard_insert_tetromino(BOARD_EXPECTED, T, 0, -1);

  _th_GameBoard_insert_tetromino(BOARD_ACTUAL, Z, 0, 0);
  _th_GameBoard_insert_tetromino(BOARD_EXPECTED, Z, 0, -1);

  GameBoard_translate_left(BOARD_ACTUAL, I);
  TEST_ASSERT_BOARD_ELEMENTS_EQUAL(BOARD_EXPECTED, BOARD_ACTUAL, I);

  GameBoard_translate_left(BOARD_ACTUAL, J);
  TEST_ASSERT_BOARD_ELEMENTS_EQUAL(BOARD_EXPECTED, BOARD_ACTUAL, J);

  GameBoard_translate_left(BOARD_ACTUAL, L);
  TEST_ASSERT_BOARD_ELEMENTS_EQUAL(BOARD_EXPECTED, BOARD_ACTUAL, L);

  GameBoard_translate_left(BOARD_ACTUAL, O);
  TEST_ASSERT_BOARD_ELEMENTS_EQUAL(BOARD_EXPECTED, BOARD_ACTUAL, O);

  GameBoard_translate_left(BOARD_ACTUAL, S);
  TEST_ASSERT_BOARD_ELEMENTS_EQUAL(BOARD_EXPECTED, BOARD_ACTUAL, S);

  GameBoard_translate_left(BOARD_ACTUAL, T);
  TEST_ASSERT_BOARD_ELEMENTS_EQUAL(BOARD_EXPECTED, BOARD_ACTUAL, T);

  GameBoard_translate_left(BOARD_ACTUAL, Z);
  TEST_ASSERT_BOARD_ELEMENTS_EQUAL(BOARD_EXPECTED, BOARD_ACTUAL, Z);
}

void test_translate_down() {
  Tetromino_t *I = Tetromino_init(TETROMINO_SHAPE_TAG_I, 5, 1);
  Tetromino_t *J = Tetromino_init(TETROMINO_SHAPE_TAG_J, 8, 1);
  Tetromino_t *L = Tetromino_init(TETROMINO_SHAPE_TAG_L, 11, 6);
  Tetromino_t *O = Tetromino_init(TETROMINO_SHAPE_TAG_O, 16, 7);
  Tetromino_t *S = Tetromino_init(TETROMINO_SHAPE_TAG_S, 20, 7);
  Tetromino_t *T = Tetromino_init(TETROMINO_SHAPE_TAG_T, 27, 2);
  Tetromino_t *Z = Tetromino_init(TETROMINO_SHAPE_TAG_Z, 27, 10);

  _th_GameBoard_insert_tetromino(BOARD_ACTUAL, I, 0, 0);
  _th_GameBoard_insert_tetromino(BOARD_EXPECTED, I, 1, 0);

  _th_GameBoard_insert_tetromino(BOARD_ACTUAL, J, 0, 0);
  _th_GameBoard_insert_tetromino(BOARD_EXPECTED, J, 1, 0);

  _th_GameBoard_insert_tetromino(BOARD_ACTUAL, L, 0, 0);
  _th_GameBoard_insert_tetromino(BOARD_EXPECTED, L, 1, 0);

  _th_GameBoard_insert_tetromino(BOARD_ACTUAL, O, 0, 0);
  _th_GameBoard_insert_tetromino(BOARD_EXPECTED, O, 1, 0);

  _th_GameBoard_insert_tetromino(BOARD_ACTUAL, S, 0, 0);
  _th_GameBoard_insert_tetromino(BOARD_EXPECTED, S, 1, 0);

  _th_GameBoard_insert_tetromino(BOARD_ACTUAL, T, 0, 0);
  _th_GameBoard_insert_tetromino(BOARD_EXPECTED, T, 1, 0);

  _th_GameBoard_insert_tetromino(BOARD_ACTUAL, Z, 0, 0);
  _th_GameBoard_insert_tetromino(BOARD_EXPECTED, Z, 1, 0);

  GameBoard_translate_down(BOARD_ACTUAL, I);
  TEST_ASSERT_BOARD_ELEMENTS_EQUAL(BOARD_EXPECTED, BOARD_ACTUAL, I);

  GameBoard_translate_down(BOARD_ACTUAL, J);
  TEST_ASSERT_BOARD_ELEMENTS_EQUAL(BOARD_EXPECTED, BOARD_ACTUAL, J);

  GameBoard_translate_down(BOARD_ACTUAL, L);
  TEST_ASSERT_BOARD_ELEMENTS_EQUAL(BOARD_EXPECTED, BOARD_ACTUAL, L);

  GameBoard_translate_down(BOARD_ACTUAL, O);
  TEST_ASSERT_BOARD_ELEMENTS_EQUAL(BOARD_EXPECTED, BOARD_ACTUAL, O);

  GameBoard_translate_down(BOARD_ACTUAL, S);
  TEST_ASSERT_BOARD_ELEMENTS_EQUAL(BOARD_EXPECTED, BOARD_ACTUAL, S);

  GameBoard_translate_down(BOARD_ACTUAL, T);
  TEST_ASSERT_BOARD_ELEMENTS_EQUAL(BOARD_EXPECTED, BOARD_ACTUAL, T);

  GameBoard_translate_down(BOARD_ACTUAL, Z);
  TEST_ASSERT_BOARD_ELEMENTS_EQUAL(BOARD_EXPECTED, BOARD_ACTUAL, Z);
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
  RUN_TEST(test_tetromino_collection_contains_active);
  RUN_TEST(test_get_tetromino_coords);
  RUN_TEST(test_translate_down);
  RUN_TEST(test_translate_right);
  RUN_TEST(test_translate_left);
  /* RUN_TEST(test_rotate_tetromino_matrix_pi); */

  return UNITY_END();
}
