#include "game.c"
#include "game.h"
#include "unity.h"
#include <assert.h>
#include <stdlib.h>

static const int BOARD_ROWS = 30;
static const int BOARD_COLS = 15;
static GameBoard_t *BOARD_ACTUAL = NULL;
static GameBoard_t *BOARD_EXPECTED = NULL;

void stdoutLog(void *userdata, int category, SDL_LogPriority priority, const char *message) { printf("%s\n", message); }

void setUp(void) {
  SDL_SetLogPriorities(SDL_LOG_PRIORITY_DEBUG);
  SDL_SetLogOutputFunction(stdoutLog, NULL);

  BOARD_ACTUAL = GameBoard_init(BOARD_ROWS, BOARD_COLS);
  BOARD_EXPECTED = GameBoard_init(BOARD_ROWS, BOARD_COLS);
}

void tearDown(void) {
  GameBoard_free(BOARD_ACTUAL);
  GameBoard_free(BOARD_EXPECTED);
  BOARD_ACTUAL = NULL;
  BOARD_EXPECTED = NULL;
}

void _th_GameBoard_insert_tetromino(GameBoard_t *const board, Tetromino_t *const t, int const row_shift,
                                    int const col_shift) {
  int *coords = GameBoard_get_tetromino_coords(board, t);

  for (size_t e = 0; e < TETROMINO_MAP_SIZE; e = e + 2) {
    board->arr[coords[e] + row_shift][coords[e + 1] + col_shift] = t;
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
  Tetromino_t *I = Tetromino_init(TETROMINO_SHAPE_TAG_I, 0, 0);

  GameBoard_insert_tetromino(BOARD_ACTUAL, I);
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

  GameBoard_insert_tetromino(BOARD_ACTUAL, I);
  _th_GameBoard_insert_tetromino(BOARD_EXPECTED, I, 0, 1);

  GameBoard_insert_tetromino(BOARD_ACTUAL, J);
  _th_GameBoard_insert_tetromino(BOARD_EXPECTED, J, 0, 1);

  GameBoard_insert_tetromino(BOARD_ACTUAL, L);
  _th_GameBoard_insert_tetromino(BOARD_EXPECTED, L, 0, 1);

  GameBoard_insert_tetromino(BOARD_ACTUAL, O);
  _th_GameBoard_insert_tetromino(BOARD_EXPECTED, O, 0, 1);

  GameBoard_insert_tetromino(BOARD_ACTUAL, S);
  _th_GameBoard_insert_tetromino(BOARD_EXPECTED, S, 0, 1);

  GameBoard_insert_tetromino(BOARD_ACTUAL, T);
  _th_GameBoard_insert_tetromino(BOARD_EXPECTED, T, 0, 1);

  GameBoard_insert_tetromino(BOARD_ACTUAL, Z);
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
  Tetromino_t *I = Tetromino_init(TETROMINO_SHAPE_TAG_I, 2, 1);
  Tetromino_t *J = Tetromino_init(TETROMINO_SHAPE_TAG_J, 8, 1);
  Tetromino_t *L = Tetromino_init(TETROMINO_SHAPE_TAG_L, 11, 6);
  Tetromino_t *O = Tetromino_init(TETROMINO_SHAPE_TAG_O, 16, 7);
  Tetromino_t *S = Tetromino_init(TETROMINO_SHAPE_TAG_S, 20, 7);
  Tetromino_t *T = Tetromino_init(TETROMINO_SHAPE_TAG_T, 27, 2);
  Tetromino_t *Z = Tetromino_init(TETROMINO_SHAPE_TAG_Z, 27, 10);

  GameBoard_insert_tetromino(BOARD_ACTUAL, I);
  _th_GameBoard_insert_tetromino(BOARD_EXPECTED, I, 0, -1);

  GameBoard_insert_tetromino(BOARD_ACTUAL, J);
  _th_GameBoard_insert_tetromino(BOARD_EXPECTED, J, 0, -1);

  GameBoard_insert_tetromino(BOARD_ACTUAL, L);
  _th_GameBoard_insert_tetromino(BOARD_EXPECTED, L, 0, -1);

  GameBoard_insert_tetromino(BOARD_ACTUAL, O);
  _th_GameBoard_insert_tetromino(BOARD_EXPECTED, O, 0, -1);

  GameBoard_insert_tetromino(BOARD_ACTUAL, S);
  _th_GameBoard_insert_tetromino(BOARD_EXPECTED, S, 0, -1);

  GameBoard_insert_tetromino(BOARD_ACTUAL, T);
  _th_GameBoard_insert_tetromino(BOARD_EXPECTED, T, 0, -1);

  GameBoard_insert_tetromino(BOARD_ACTUAL, Z);
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

  GameBoard_insert_tetromino(BOARD_ACTUAL, I);
  _th_GameBoard_insert_tetromino(BOARD_EXPECTED, I, 1, 0);

  GameBoard_insert_tetromino(BOARD_ACTUAL, J);
  _th_GameBoard_insert_tetromino(BOARD_EXPECTED, J, 1, 0);

  GameBoard_insert_tetromino(BOARD_ACTUAL, L);
  _th_GameBoard_insert_tetromino(BOARD_EXPECTED, L, 1, 0);

  GameBoard_insert_tetromino(BOARD_ACTUAL, O);
  _th_GameBoard_insert_tetromino(BOARD_EXPECTED, O, 1, 0);

  GameBoard_insert_tetromino(BOARD_ACTUAL, S);
  _th_GameBoard_insert_tetromino(BOARD_EXPECTED, S, 1, 0);

  GameBoard_insert_tetromino(BOARD_ACTUAL, T);
  _th_GameBoard_insert_tetromino(BOARD_EXPECTED, T, 1, 0);

  GameBoard_insert_tetromino(BOARD_ACTUAL, Z);
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

void test_rotate_tetromino_matrix_90() {
  Tetromino_t *I = Tetromino_init(TETROMINO_SHAPE_TAG_I, 0, 0);
  Tetromino_t *J = Tetromino_init(TETROMINO_SHAPE_TAG_J, 0, 0);
  Tetromino_t *L = Tetromino_init(TETROMINO_SHAPE_TAG_L, 0, 0);
  Tetromino_t *O = Tetromino_init(TETROMINO_SHAPE_TAG_O, 0, 0);
  Tetromino_t *S = Tetromino_init(TETROMINO_SHAPE_TAG_S, 0, 0);
  Tetromino_t *T = Tetromino_init(TETROMINO_SHAPE_TAG_T, 0, 0);
  Tetromino_t *Z = Tetromino_init(TETROMINO_SHAPE_TAG_Z, 0, 0);

  Tetromino_rotate(I, 90);
  Tetromino_rotate(J, 90);
  Tetromino_rotate(L, 90);
  Tetromino_rotate(O, 90);
  Tetromino_rotate(S, 90);
  Tetromino_rotate(T, 90);
  Tetromino_rotate(Z, 90);

  {
    int expected[TETROMINO_MAP_SIZE] = {0, 2, 1, 2, 2, 2, 3, 2};
    int *actual = TetrominoMatrix_rotate(I->mat, I->deg_rot);
    TEST_ASSERT_EQUAL_INT_ARRAY_MESSAGE(expected, actual, TETROMINO_MAP_SIZE, "I failed 90 rotation");
    free(actual);
  }

  {
    int expected[TETROMINO_MAP_SIZE] = {0, 2, 0, 1, 1, 1, 2, 1};
    int *actual = TetrominoMatrix_rotate(J->mat, J->deg_rot);
    TEST_ASSERT_EQUAL_INT_ARRAY_MESSAGE(expected, actual, TETROMINO_MAP_SIZE, "J failed 90 rotation");
    free(actual);
  }

  {
    int expected[TETROMINO_MAP_SIZE] = {2, 2, 0, 1, 1, 1, 2, 1};
    int *actual = TetrominoMatrix_rotate(L->mat, L->deg_rot);
    TEST_ASSERT_EQUAL_INT_ARRAY_MESSAGE(expected, actual, TETROMINO_MAP_SIZE, "L failed 90 rotation");
    free(actual);
  }

  {
    int expected[TETROMINO_MAP_SIZE] = {0, 1, 1, 1, 0, 0, 1, 0};
    int *actual = TetrominoMatrix_rotate(O->mat, O->deg_rot);
    TEST_ASSERT_EQUAL_INT_ARRAY_MESSAGE(expected, actual, TETROMINO_MAP_SIZE, "O failed 90 rotation");
    free(actual);
  }

  {
    int expected[TETROMINO_MAP_SIZE] = {1, 2, 2, 2, 0, 1, 1, 1};
    int *actual = TetrominoMatrix_rotate(S->mat, S->deg_rot);
    TEST_ASSERT_EQUAL_INT_ARRAY_MESSAGE(expected, actual, TETROMINO_MAP_SIZE, "S failed 90 rotation");
    free(actual);
  }

  {
    int expected[TETROMINO_MAP_SIZE] = {1, 2, 0, 1, 1, 1, 2, 1};
    int *actual = TetrominoMatrix_rotate(T->mat, T->deg_rot);
    TEST_ASSERT_EQUAL_INT_ARRAY_MESSAGE(expected, actual, TETROMINO_MAP_SIZE, "T failed 90 rotation");
    free(actual);
  }

  {
    int expected[TETROMINO_MAP_SIZE] = {0, 2, 1, 2, 1, 1, 2, 1};
    int *actual = TetrominoMatrix_rotate(Z->mat, Z->deg_rot);
    TEST_ASSERT_EQUAL_INT_ARRAY_MESSAGE(expected, actual, TETROMINO_MAP_SIZE, "Z failed 90 rotation");
    free(actual);
  }
}

void test_rotate_tetromino_matrix_180() {
  Tetromino_t *I = Tetromino_init(TETROMINO_SHAPE_TAG_I, 0, 0);
  Tetromino_t *J = Tetromino_init(TETROMINO_SHAPE_TAG_J, 0, 0);
  Tetromino_t *L = Tetromino_init(TETROMINO_SHAPE_TAG_L, 0, 0);
  Tetromino_t *O = Tetromino_init(TETROMINO_SHAPE_TAG_O, 0, 0);
  Tetromino_t *S = Tetromino_init(TETROMINO_SHAPE_TAG_S, 0, 0);
  Tetromino_t *T = Tetromino_init(TETROMINO_SHAPE_TAG_T, 0, 0);
  Tetromino_t *Z = Tetromino_init(TETROMINO_SHAPE_TAG_Z, 0, 0);

  Tetromino_rotate(I, 180);
  Tetromino_rotate(J, 180);
  Tetromino_rotate(L, 180);
  Tetromino_rotate(O, 180);
  Tetromino_rotate(S, 180);
  Tetromino_rotate(T, 180);
  Tetromino_rotate(Z, 180);

  {
    int expected[TETROMINO_MAP_SIZE] = {2, 3, 2, 2, 2, 1, 2, 0};
    int *actual = TetrominoMatrix_rotate(I->mat, I->deg_rot);
    TEST_ASSERT_EQUAL_INT_ARRAY_MESSAGE(expected, actual, TETROMINO_MAP_SIZE, "I failed 180 rotation");
    free(actual);
  }

  {
    int expected[TETROMINO_MAP_SIZE] = {2, 2, 1, 2, 1, 1, 1, 0};
    int *actual = TetrominoMatrix_rotate(J->mat, J->deg_rot);
    TEST_ASSERT_EQUAL_INT_ARRAY_MESSAGE(expected, actual, TETROMINO_MAP_SIZE, "J failed 180 rotation");
    free(actual);
  }

  {
    int expected[TETROMINO_MAP_SIZE] = {2, 0, 1, 2, 1, 1, 1, 0};
    int *actual = TetrominoMatrix_rotate(L->mat, L->deg_rot);
    TEST_ASSERT_EQUAL_INT_ARRAY_MESSAGE(expected, actual, TETROMINO_MAP_SIZE, "L failed 180 rotation");
    free(actual);
  }

  {
    int expected[TETROMINO_MAP_SIZE] = {1, 1, 1, 0, 0, 1, 0, 0};
    int *actual = TetrominoMatrix_rotate(O->mat, O->deg_rot);
    TEST_ASSERT_EQUAL_INT_ARRAY_MESSAGE(expected, actual, TETROMINO_MAP_SIZE, "O failed 180 rotation");
    free(actual);
  }

  {
    int expected[TETROMINO_MAP_SIZE] = {2, 1, 2, 0, 1, 2, 1, 1};
    int *actual = TetrominoMatrix_rotate(S->mat, S->deg_rot);
    TEST_ASSERT_EQUAL_INT_ARRAY_MESSAGE(expected, actual, TETROMINO_MAP_SIZE, "S failed 180 rotation");
    free(actual);
  }

  {
    int expected[TETROMINO_MAP_SIZE] = {2, 1, 1, 2, 1, 1, 1, 0};
    int *actual = TetrominoMatrix_rotate(T->mat, T->deg_rot);
    TEST_ASSERT_EQUAL_INT_ARRAY_MESSAGE(expected, actual, TETROMINO_MAP_SIZE, "T failed 180 rotation");
    free(actual);
  }

  {
    int expected[TETROMINO_MAP_SIZE] = {2, 2, 2, 1, 1, 1, 1, 0};
    int *actual = TetrominoMatrix_rotate(Z->mat, Z->deg_rot);
    TEST_ASSERT_EQUAL_INT_ARRAY_MESSAGE(expected, actual, TETROMINO_MAP_SIZE, "Z failed 180 rotation");
    free(actual);
  }
}

void test_rotate_tetromino_matrix_270() {
  Tetromino_t *I = Tetromino_init(TETROMINO_SHAPE_TAG_I, 0, 0);
  Tetromino_t *J = Tetromino_init(TETROMINO_SHAPE_TAG_J, 0, 0);
  Tetromino_t *L = Tetromino_init(TETROMINO_SHAPE_TAG_L, 0, 0);
  Tetromino_t *O = Tetromino_init(TETROMINO_SHAPE_TAG_O, 0, 0);
  Tetromino_t *S = Tetromino_init(TETROMINO_SHAPE_TAG_S, 0, 0);
  Tetromino_t *T = Tetromino_init(TETROMINO_SHAPE_TAG_T, 0, 0);
  Tetromino_t *Z = Tetromino_init(TETROMINO_SHAPE_TAG_Z, 0, 0);

  Tetromino_rotate(I, 270);
  Tetromino_rotate(J, 270);
  Tetromino_rotate(L, 270);
  Tetromino_rotate(O, 270);
  Tetromino_rotate(S, 270);
  Tetromino_rotate(T, 270);

  Tetromino_rotate(Z, 180);
  Tetromino_rotate(Z, 90);

  {
    int expected[TETROMINO_MAP_SIZE] = {3, 1, 2, 1, 1, 1, 0, 1};
    int *actual = TetrominoMatrix_rotate(I->mat, I->deg_rot);
    TEST_ASSERT_EQUAL_INT_ARRAY_MESSAGE(expected, actual, TETROMINO_MAP_SIZE, "I failed 270 rotation");
    free(actual);
  }

  {
    int expected[TETROMINO_MAP_SIZE] = {2, 0, 2, 1, 1, 1, 0, 1};
    int *actual = TetrominoMatrix_rotate(J->mat, J->deg_rot);
    TEST_ASSERT_EQUAL_INT_ARRAY_MESSAGE(expected, actual, TETROMINO_MAP_SIZE, "J failed 270 rotation");
    free(actual);
  }

  {
    int expected[TETROMINO_MAP_SIZE] = {0, 0, 2, 1, 1, 1, 0, 1};
    int *actual = TetrominoMatrix_rotate(L->mat, L->deg_rot);
    TEST_ASSERT_EQUAL_INT_ARRAY_MESSAGE(expected, actual, TETROMINO_MAP_SIZE, "L failed 270 rotation");
    free(actual);
  }

  {
    int expected[TETROMINO_MAP_SIZE] = {1, 0, 0, 0, 1, 1, 0, 1};
    int *actual = TetrominoMatrix_rotate(O->mat, O->deg_rot);
    TEST_ASSERT_EQUAL_INT_ARRAY_MESSAGE(expected, actual, TETROMINO_MAP_SIZE, "O failed 270 rotation");
    free(actual);
  }

  {
    int expected[TETROMINO_MAP_SIZE] = {1, 0, 0, 0, 2, 1, 1, 1};
    int *actual = TetrominoMatrix_rotate(S->mat, S->deg_rot);
    TEST_ASSERT_EQUAL_INT_ARRAY_MESSAGE(expected, actual, TETROMINO_MAP_SIZE, "S failed 270 rotation");
    free(actual);
  }

  {
    int expected[TETROMINO_MAP_SIZE] = {1, 0, 2, 1, 1, 1, 0, 1};
    int *actual = TetrominoMatrix_rotate(T->mat, T->deg_rot);
    TEST_ASSERT_EQUAL_INT_ARRAY_MESSAGE(expected, actual, TETROMINO_MAP_SIZE, "T failed 270 rotation");
    free(actual);
  }

  {
    int expected[TETROMINO_MAP_SIZE] = {2, 0, 1, 0, 1, 1, 0, 1};
    int *actual = TetrominoMatrix_rotate(Z->mat, Z->deg_rot);
    TEST_ASSERT_EQUAL_INT_ARRAY_MESSAGE(expected, actual, TETROMINO_MAP_SIZE, "Z failed 270 rotation");
    free(actual);
  }
}

void test_rotate_tetromino_on_board() {
  Tetromino_t *I = Tetromino_init(TETROMINO_SHAPE_TAG_I, 1, 0);
  Tetromino_t *J = Tetromino_init(TETROMINO_SHAPE_TAG_J, 1, 8);
  Tetromino_t *L = Tetromino_init(TETROMINO_SHAPE_TAG_L, 9, 3);
  Tetromino_t *O = Tetromino_init(TETROMINO_SHAPE_TAG_O, 9, 10);
  Tetromino_t *S = Tetromino_init(TETROMINO_SHAPE_TAG_S, 18, 5);
  Tetromino_t *T = Tetromino_init(TETROMINO_SHAPE_TAG_T, 18, 12);
  Tetromino_t *Z = Tetromino_init(TETROMINO_SHAPE_TAG_Z, 23, 4);

  GameBoard_insert_tetromino(BOARD_ACTUAL, I);
  GameBoard_rotate_pi(BOARD_ACTUAL, I);

  GameBoard_insert_tetromino(BOARD_ACTUAL, J);
  GameBoard_rotate_pi(BOARD_ACTUAL, J);

  GameBoard_insert_tetromino(BOARD_ACTUAL, L);
  GameBoard_rotate_pi(BOARD_ACTUAL, L);

  GameBoard_insert_tetromino(BOARD_ACTUAL, O);
  GameBoard_rotate_pi(BOARD_ACTUAL, O);

  GameBoard_insert_tetromino(BOARD_ACTUAL, S);
  GameBoard_rotate_pi(BOARD_ACTUAL, S);

  GameBoard_insert_tetromino(BOARD_ACTUAL, T);
  GameBoard_rotate_pi(BOARD_ACTUAL, T);

  GameBoard_insert_tetromino(BOARD_ACTUAL, Z);
  GameBoard_rotate_pi(BOARD_ACTUAL, Z);

  I->deg_rot = 90;
  _th_GameBoard_insert_tetromino(BOARD_EXPECTED, I, 0, 0);

  J->deg_rot = 90;
  _th_GameBoard_insert_tetromino(BOARD_EXPECTED, J, 0, 0);

  L->deg_rot = 90;
  _th_GameBoard_insert_tetromino(BOARD_EXPECTED, L, 0, 0);

  O->deg_rot = 90;
  _th_GameBoard_insert_tetromino(BOARD_EXPECTED, O, 0, 0);

  S->deg_rot = 90;
  _th_GameBoard_insert_tetromino(BOARD_EXPECTED, S, 0, 0);

  T->deg_rot = 90;
  _th_GameBoard_insert_tetromino(BOARD_EXPECTED, T, 0, 0);

  Z->deg_rot = 90;
  _th_GameBoard_insert_tetromino(BOARD_EXPECTED, Z, 0, 0);

  TEST_ASSERT_BOARD_ELEMENTS_EQUAL(BOARD_EXPECTED, BOARD_ACTUAL, NULL);
}

void test_set_tetromino_hide_mask() { Tetromino_t *O = Tetromino_init(TETROMINO_SHAPE_TAG_O, 9, 10); }

int main(int argc, char *argv[]) {
  UNITY_BEGIN();

  /* RUN_TEST(test_collision_check); */
  /* RUN_TEST(test_tetromino_collection_contains_active); */
  RUN_TEST(test_get_tetromino_coords);
  RUN_TEST(test_translate_down);
  RUN_TEST(test_translate_right);
  RUN_TEST(test_translate_left);
  RUN_TEST(test_rotate_tetromino_matrix_90);
  RUN_TEST(test_rotate_tetromino_matrix_180);
  RUN_TEST(test_rotate_tetromino_matrix_270);
  RUN_TEST(test_rotate_tetromino_on_board);
  return UNITY_END();
}
