#include "game.h"
#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_timer.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static SpriteSheet_t *sheet_tetrominos = NULL;
static double PREV_TIME = 0.0;

static const int TETROMINO_MAP_I[TETROMINO_MAP_SIZE] = {1, 0, 1, 1, 1, 2, 1, 3};
static const int TETROMINO_MAP_J[TETROMINO_MAP_SIZE] = {0, 0, 1, 0, 1, 1, 1, 2};
static const int TETROMINO_MAP_L[TETROMINO_MAP_SIZE] = {0, 2, 1, 0, 1, 1, 1, 2};
static const int TETROMINO_MAP_O[TETROMINO_MAP_SIZE] = {0, 0, 0, 1, 1, 0, 1, 1};
static const int TETROMINO_MAP_S[TETROMINO_MAP_SIZE] = {0, 1, 0, 2, 1, 0, 1, 1};
static const int TETROMINO_MAP_T[TETROMINO_MAP_SIZE] = {0, 1, 1, 0, 1, 1, 1, 2};
static const int TETROMINO_MAP_Z[TETROMINO_MAP_SIZE] = {0, 0, 0, 1, 1, 1, 1, 2};

void SpriteSheet_tetrominos(SpriteSheet_t *sheet) { sheet_tetrominos = sheet; }

int *TetrominoMatrix_rotate(TetrominoMatrix const *const mat, int const deg) {
  assert(deg == 0 || deg == 90 || deg == 180 || deg == 270 && "invalid tetromino rotation");

  int *arr = malloc(sizeof(int) * TETROMINO_MAP_SIZE);

  if (deg == 0) {
    for (size_t e = 0; e < TETROMINO_MAP_SIZE; e = e + 2) {
      arr[e] = mat->map[e];
      arr[e + 1] = mat->map[e + 1];
    }
  } else if (deg == 90) {
    for (size_t e = 0; e < TETROMINO_MAP_SIZE; e = e + 2) {
      // Rotating pi radians: a[i,j] = a[j][n-i-1]
      arr[e] = mat->map[e + 1];
      arr[e + 1] = mat->size - mat->map[e] - 1;
    }
  } else if (deg == 180) {
    for (size_t e = 0; e < TETROMINO_MAP_SIZE; e = e + 2) {
      // Rotating 2pi radians: a[i,j] = a[n-i-1][n-j-1]
      arr[e] = mat->size - mat->map[e] - 1;
      arr[e + 1] = mat->size - mat->map[e + 1] - 1;
    }
  } else if (deg == 270) {
    for (size_t e = 0; e < TETROMINO_MAP_SIZE; e = e + 2) {
      // Rotating 3pi radians: a[i,j] = a[n-j-1][i]
      arr[e] = mat->size - mat->map[e + 1] - 1;
      arr[e + 1] = mat->map[e];
    }
  }
  return arr;
}

void Tetromino_rotate(Tetromino_t *const t, int const deg) {
  assert(deg == 0 || deg == 90 || deg == 180 || deg == 270 && "invalid tetromino rotation");

  t->deg_rot = (t->deg_rot + deg) % 360;
}

Tetromino_t **get_active_tetromino(TetrominoCollection_t *coll) {
  if (coll->cnt == 0) {
    return NULL;
  }

  return &coll->tetrominos[coll->cnt - 1];
}

Tetromino_t *Tetromino_init(TetrominoShapeTag const tag, int row, int col) {
  Tetromino_t *new = malloc(sizeof(Tetromino_t));
  SDL_FRect *clip = malloc(sizeof(SDL_FRect));
  TetrominoMatrix *mat = malloc(sizeof(TetrominoMatrix));

  memset(new, 0, sizeof(Tetromino_t));
  memset(clip, 0, sizeof(SDL_FRect));
  memset(mat, 0, sizeof(TetrominoMatrix));

  new->clip = clip;
  new->mat = mat;
  new->sheet = sheet_tetrominos;
  new->deg_rot = 0;
  new->state = TETROMINO_STATE_ACTIVE;

  // I shape should start horizontal in row 2 of 4
  // s.t. the origin of the bounding box would be (-1, -1)
  //
  // [-1, 0]  [-1, 1]  [-1, 2]  [-1, 3]  ...
  // ---------------- GAME BOARD BOUND ----------------------
  // [ 0, 0]  [ 0, 1]  [ 0,  2] [ 0,  3] <= This is the tetromino
  // [ 1, 0]  [ 1, 1]  [ 1,  2] [ 1,  3]       ...
  // [ 2, 0]  [ 2, 1]  [ 2,  2] [ 2,  3]
  //  ...
  //  .
  //  .  .
  //  .    .
  //
  switch (tag) {
  case TETROMINO_SHAPE_TAG_I:
    // TODO: read the data from a file....
    new->clip->x = 0;
    new->clip->y = 0;
    new->clip->h = BLOCK_SIZE_PIXELS;
    new->clip->w = BLOCK_SIZE_PIXELS;

    *new->mat = (TetrominoMatrix){.t = tag, .row0 = row - 1, .col0 = col, .map = TETROMINO_MAP_I, .size = 4};
    break;
  case TETROMINO_SHAPE_TAG_J:
    new->clip->x = 0;
    new->clip->y = 0;
    new->clip->h = 4 * BLOCK_SIZE_PIXELS;
    new->clip->w = 4 * BLOCK_SIZE_PIXELS;

    *new->mat = (TetrominoMatrix){.t = tag, .row0 = row, .col0 = col, .map = TETROMINO_MAP_J, .size = 3};
    break;
  case TETROMINO_SHAPE_TAG_L:
    new->clip->x = 0;
    new->clip->y = 0;
    new->clip->h = 4 * BLOCK_SIZE_PIXELS;
    new->clip->w = 4 * BLOCK_SIZE_PIXELS;

    *new->mat = (TetrominoMatrix){.t = tag, .row0 = row, .col0 = col - 2, .map = TETROMINO_MAP_L, .size = 3};
    break;
  case TETROMINO_SHAPE_TAG_O:
    new->clip->x = 0;
    new->clip->y = 0;
    new->clip->h = 4 * BLOCK_SIZE_PIXELS;
    new->clip->w = 4 * BLOCK_SIZE_PIXELS;

    *new->mat = (TetrominoMatrix){.t = tag, .row0 = row, .col0 = col, .map = TETROMINO_MAP_O, .size = 2};
    break;
  case TETROMINO_SHAPE_TAG_S:
    new->clip->x = 0;
    new->clip->y = 0;
    new->clip->h = 4 * BLOCK_SIZE_PIXELS;
    new->clip->w = 4 * BLOCK_SIZE_PIXELS;

    *new->mat = (TetrominoMatrix){.t = tag, .row0 = row, .col0 = col - 1, .map = TETROMINO_MAP_S, .size = 3};
    break;
  case TETROMINO_SHAPE_TAG_T:
    new->clip->x = 0;
    new->clip->y = 0;
    new->clip->h = 4 * BLOCK_SIZE_PIXELS;
    new->clip->w = 4 * BLOCK_SIZE_PIXELS;

    *new->mat = (TetrominoMatrix){.t = tag, .row0 = row, .col0 = col - 1, .map = TETROMINO_MAP_T, .size = 3};
    break;
  case TETROMINO_SHAPE_TAG_Z:
    new->clip->x = 0;
    new->clip->y = 0;
    new->clip->h = 4 * BLOCK_SIZE_PIXELS;
    new->clip->w = 4 * BLOCK_SIZE_PIXELS;

    *new->mat = (TetrominoMatrix){.t = tag, .row0 = row, .col0 = col, .map = TETROMINO_MAP_Z, .size = 3};
    break;
  }

  return new;
}

TetrominoCollection_t *TetrominoCollection_init(size_t const size) {
  TetrominoCollection_t *new = malloc(sizeof(TetrominoCollection_t));
  memset(new, 0, sizeof(TetrominoCollection_t));

  new->size = size;
  new->cnt = 0;
  new->tetrominos = malloc(sizeof(Tetromino_t) * size);
  memset(new->tetrominos, 0, sizeof(Tetromino_t) * size);

  return new;
}

void TetrominoCollection_free(TetrominoCollection_t *col) {
  for (size_t i = 0; i < col->cnt; i++) {
    Tetromino_free(col->tetrominos[i]);
  }
  free(col);
}

void TetrominoCollection_push(TetrominoCollection_t *col, Tetromino_t *const new) {

  if (col->cnt + 1 > col->size) {
    // TODO: resize and add more
    // TODO: also need to possibly add reduce size
    SDL_LogError(SDL_LOG_CATEGORY_TEST, "resize not implemented...\n");
    assert(false);
    return;
  }

  col->tetrominos[col->cnt] = new;
  col->cnt += 1;
}

SDL_FRect *Tetromino_render_position(Tetromino_t const *const o) {
  // TODO: consider rotations...
  SDL_FRect *pos_1 = malloc(sizeof(SDL_FRect));

  pos_1->x = o->mat->col0 * BLOCK_SIZE_PIXELS;
  pos_1->y = o->mat->row0 * BLOCK_SIZE_PIXELS;
  pos_1->h = BLOCK_SIZE_PIXELS;
  pos_1->w = BLOCK_SIZE_PIXELS;

  return pos_1;
}

void TetrominoCollection_render(TetrominoCollection_t *col, GameBoard_t *board, SDL_Renderer *renderer) {
  SDL_FRect *pos = malloc(sizeof(SDL_FRect));
  memset(pos, 0, sizeof(SDL_FRect));

  pos->h = BLOCK_SIZE_PIXELS;
  pos->w = BLOCK_SIZE_PIXELS;

  for (size_t i = 0; i < col->cnt; i++) {
    int *coords = GameBoard_get_tetromino_coords(board, col->tetrominos[i]);
    for (size_t e = 0; e < TETROMINO_MAP_SIZE; e = e + 2) {
      pos->x = coords[e + 1] * BLOCK_SIZE_PIXELS;
      pos->y = coords[e] * BLOCK_SIZE_PIXELS;
      SDL_RenderTexture(renderer, col->tetrominos[i]->sheet->sheet, col->tetrominos[i]->clip, pos);
    }
  }

  free(pos);
}

int SpriteSheet_init(SDL_Renderer *renderer, SpriteSheet_t *const sheet, char const *const sheet_path) {
  SDL_Surface *surface = NULL;
  char *bmp_path = NULL;

  SDL_asprintf(&bmp_path, "%sassets/%s", SDL_GetBasePath(), sheet_path);

  surface = SDL_LoadBMP(bmp_path);
  if (!surface) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Load sprite sheet %s: %s", sheet_path, SDL_GetError());
    return SDL_APP_FAILURE;
  }

  SDL_free(bmp_path);

  sheet->sheet = SDL_CreateTextureFromSurface(renderer, surface);
  sheet->h = surface->h;
  sheet->w = surface->w;

  SDL_DestroySurface(surface);

  return SDL_APP_CONTINUE;
}

void SpriteSheet_free(SpriteSheet_t *o) {
  if (o == NULL)
    return;
  SDL_DestroyTexture(o->sheet);
}

GameBoard_t *GameBoard_init(int rows, int cols) {
  GameBoard_t *new = malloc(sizeof(GameBoard_t));
  new->rows = rows;
  new->cols = cols;

  new->arr = malloc(sizeof(Tetromino_t **) * rows);
  memset(new->arr, 0, sizeof(Tetromino_t **) * rows);
  for (size_t row = 0; row < new->rows; row++) {
    new->arr[row] = malloc(sizeof(Tetromino_t *) * cols);
    memset(new->arr[row], 0, sizeof(Tetromino_t *) * cols);
  }

  return new;
}

void GameBoard_free(GameBoard_t *o) {
  if (o == NULL)
    return;

  for (size_t row = 0; row < o->rows; row++) {
    if (o->arr[row] != NULL) {
      free(o->arr[row]);
    }
  }
  free(o->arr);
  free(o);
}

void GameBoard_print_debug(GameBoard_t const *const board) {
  for (size_t row = 0; row < board->rows; row++) {
    for (size_t col = 0; col < board->cols; col++) {
      if (board->arr[row][col] != 0) {
        printf(" 1 ");
      } else {
        printf(" 0 ");
      }
    }
    printf("\n");
  }

  printf("\n");
}

GameApp_t *GameApp_init() {
  GameApp_t *new = malloc(sizeof(GameApp_t));

  SDL_Rect bounds = {0};
  SDL_GetDisplayBounds(SDL_GetPrimaryDisplay(), &bounds);
  new->display_h = bounds.h;
  new->display_w = bounds.w;
  new->tick_rate = 1000.0 / 1.0;

  new->input = USER_INPUT_NONE;
  new->tetrominos = TetrominoCollection_init(100);

  // TODO: how big should the board be?
  int rows = 10;
  int cols = 20;
  new->board = GameBoard_init(rows, cols);
  new->previous_board = GameBoard_init(rows, cols);
  return new;
}

void GameBoard_copy(GameBoard_t *const dest, GameBoard_t const *const src) {
  assert(src->cols == dest->cols && src->rows == dest->rows);

  for (size_t row = 0; row < src->rows; row++) {
    memcpy(dest->arr[row], src->arr[row], src->cols);
  }
}

void GameBoard_spawn_tetromino(GameBoard_t **board, TetrominoCollection_t *col, int spawn_row, int spawn_col) {
  // TODO: get the shape in waiting, that's the one to use
  // Meanwhile a new "shape in waiting" will be randomly selected

  assert(spawn_row >= 0 && spawn_row < (*board)->rows && "invariant: spawn row is out of bounds");
  assert(spawn_col >= 0 || spawn_col > (*board)->cols - 1 && "invariant: spawn col is out of bounds");

  Tetromino_t *new_tetromino = Tetromino_init(TETROMINO_SHAPE_TAG_I, spawn_row, spawn_col);
  TetrominoCollection_push(col, new_tetromino);

  if (GameBoard_collision(*board, new_tetromino, 0, 0)) {
    printf("GAME OVER!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
  }

  for (size_t i = 0; i < TETROMINO_MAP_SIZE; i = i + 2) {
    int r = new_tetromino->mat->map[i] + new_tetromino->mat->row0;
    int c = new_tetromino->mat->map[i + 1] + new_tetromino->mat->col0;

    (*board)->arr[r][c] = new_tetromino;
  }
}

void TetrominoMatrix_free(TetrominoMatrix *o) {
  free((int *)o->map);
  free(o);
}

void Tetromino_free(Tetromino_t *o) {
  if (o == NULL)
    return;

  SpriteSheet_free(o->sheet);
  free(o->clip);
  TetrominoMatrix_free(o->mat);

  free(o);
}

int *GameBoard_get_tetromino_coords(GameBoard_t const *const board, Tetromino_t const *const tetromino) {
  int *coords = TetrominoMatrix_rotate(tetromino->mat, tetromino->deg_rot);

  for (size_t i = 0; i < TETROMINO_MAP_SIZE; i = i + 2) {
    coords[i] += tetromino->mat->row0;
    coords[i + 1] += tetromino->mat->col0;
  }

  return coords;
}

bool GameBoard_collision(GameBoard_t const *const board, Tetromino_t *const t, int rows, int cols) {
  int *coords = GameBoard_get_tetromino_coords(board, t);
  bool did_collide = false;

  for (size_t i = 0; i < TETROMINO_MAP_SIZE; i = i + 2) {
    int curr_row = coords[i];
    int curr_col = coords[i + 1];

    if (curr_row + rows < 0 || curr_row + rows > board->rows) {
      SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "COLLISION: cannot translate vertically to row %d", coords[i]);
      did_collide = true;
      break;
    }

    if (curr_col + cols < 0 || curr_col + cols > board->cols - 1) {
      SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "COLLISION: cannot translate horizontally to col %d", coords[i + 1]);
      did_collide = true;
      break;
    }

    if (curr_row + rows == board->rows ||
        board->arr[curr_row + rows][curr_col + cols] != NULL && board->arr[curr_row + rows][curr_col + cols] != t) {
      SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "COLLISION: cannot translate new new position (%d %d)",
                   curr_row + rows, curr_col + cols);
      did_collide = true;
      t->state = TETROMINO_STATE_LOCKED;
      break;
    }
  }

  free(coords);
  return did_collide;
}

void _GameBoard_translate(GameBoard_t *const board, Tetromino_t *const tetromino, int const rows, int const cols) {
  int *coords = GameBoard_get_tetromino_coords(board, tetromino);

  if (GameBoard_collision(board, tetromino, rows, cols)) {
    return;
  }

  // The simplest solution to move the tetromino's cached board state is to clear the previous position prior to writing
  // the new position. In this way, we don't need to worry about the ordering of the writes, which may accidentally add
  // or remove a previous element..
  for (size_t i = 0; i < TETROMINO_MAP_SIZE; i = i + 2) {
    int row = coords[i];
    int col = coords[i + 1];

    board->arr[row][col] = NULL;
  }

  for (size_t i = 0; i < TETROMINO_MAP_SIZE; i = i + 2) {
    int row = coords[i];
    int col = coords[i + 1];

    board->arr[row + rows][col + cols] = tetromino;
  }

  tetromino->mat->row0 += rows;
  tetromino->mat->col0 += cols;

  free(coords);
}

void _GameBoard_rotate(GameBoard_t *const board, Tetromino_t *const tetromino, int const deg) {
  Tetromino_rotate(tetromino, deg);
  int *coords = GameBoard_get_tetromino_coords(board, tetromino);

  if (GameBoard_collision(board, tetromino, 0, 0)) {
    Tetromino_rotate(tetromino, -deg);
    printf("rotation collision!\n");
    return;
  }

  for (size_t i = 0; i < TETROMINO_MAP_SIZE; i = i + 2) {
    int row = coords[i];
    int col = coords[i + 1];

    board->arr[row][col] = NULL;
  }

  for (size_t i = 0; i < TETROMINO_MAP_SIZE; i = i + 2) {
    int row = coords[i];
    int col = coords[i + 1];

    board->arr[row][col] = tetromino;
  }

  free(coords);
}

void GameBoard_translate_left(GameBoard_t *const board, Tetromino_t *const tetromino) {
  _GameBoard_translate(board, tetromino, 0, -1);
}

void GameBoard_translate_right(GameBoard_t *const board, Tetromino_t *const tetromino) {
  _GameBoard_translate(board, tetromino, 0, 1);
}
void GameBoard_translate_down(GameBoard_t *const board, Tetromino_t *const tetromino) {
  _GameBoard_translate(board, tetromino, 1, 0);
}

void GameBoard_rotate_pi(GameBoard_t *const board, Tetromino_t *const tetromino) {
  _GameBoard_rotate(board, tetromino, 90);
}
void GameBoard_rotate_neg_pi(GameBoard_t *const board, Tetromino_t *const tetromino) {
  assert(false && "unimplemented");
}

void GameBoard_clear_full_rows(GameBoard_t *board) {
  // TODO
}

// TODO: Maybe change this to get_active, and just check for null??
bool TetrominoCollection_contains_active(TetrominoCollection_t const *const col) {
  assert(col->size > 0);

  // NOTE: It should only be the case that the last Tetromino is ever active.
  // Otherwise, we have a more general issue. However, this optimization is not
  // critical right now since we won't ever have that many tetrominos.
  for (size_t i = 0; i < col->cnt; i++) {
    if (col->tetrominos[i]->state == TETROMINO_STATE_ACTIVE) {
      assert(i == col->cnt - 1);
      return true;
    }
  }

  return false;
}

void GameBoard_update(GameBoard_t *board, GameApp_t *app_state) {
  //////////////////////////////////////////////////
  // while (not immutable)
  // tick down
  // check collision

  // while (no lines)
  // delete all lines
  // drop tetrominos down
  //////////////////////////////////////////////////

  if (!TetrominoCollection_contains_active(app_state->tetrominos)) {
    GameBoard_spawn_tetromino(&board, app_state->tetrominos, 0, 4);
  }

  // We tick the board at a different rate than the smooth updates of
  // other objects...
  if (SDL_GetTicks() - PREV_TIME < app_state->tick_rate) {
    return;
  } else {
    GameBoard_translate_down(board, app_state->tetrominos->tetrominos[app_state->tetrominos->cnt - 1]);
    // TODO: line clearing algorithm drops everything down
  }

  PREV_TIME = SDL_GetTicks();
}

void GameApp_handle_input(GameApp_t *state) {
  switch (state->input) {
  case USER_INPUT_SHOW_DEBUG:
    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Input: Show Debug\n");
    GameBoard_print_debug(state->board);
    break;
  case USER_INPUT_MOVE_RIGHT:
    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Input: Move Right\n");
    GameBoard_translate_right(state->board, *get_active_tetromino(state->tetrominos));
    break;
  case USER_INPUT_MOVE_LEFT:
    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Input: Move Left\n");
    GameBoard_translate_left(state->board, *get_active_tetromino(state->tetrominos));
    break;
  case USER_INPUT_ROTATE_RIGHT:
    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Input: Rotate Right\n");
    GameBoard_rotate_pi(state->board, *get_active_tetromino(state->tetrominos));
    break;
  case USER_INPUT_ROTATE_LEFT:
    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Input: Rotate Left\n");
    break;
  case USER_INPUT_SOFT_DROP:
    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Input: Soft Drop\n");
    GameBoard_translate_down(state->board, state->tetrominos->tetrominos[state->tetrominos->cnt - 1]);
    break;
  case USER_INPUT_HARD_DROP:
    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Input: Hard Drop\n");
    break;
  default:
    break;
  }

  state->input = USER_INPUT_NONE;
}

void GameApp_update(GameApp_t *state) {
  GameApp_handle_input(state);
  GameBoard_update(state->board, state);

  // Update Game Board
  // 1. insert tetromino?
  // 2. movement checks...

  // everything falls

  // create new tetromino if none are falling?

  // TODO:
  // everything tick down by one
  // collision detection
  // line detection for deletion (but then I need to tick down again?)
}
