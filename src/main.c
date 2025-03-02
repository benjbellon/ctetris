#define SDL_MAIN_USE_CALLBACKS 1
#define BLOCK_SIZE_PIXELS 32

#include "_gen/cmake_variables.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  SDL_Texture *sheet;
  int h;
  int w;
} SpriteSheet_t;

typedef enum {
  TETROMINO_SHAPE_I,
  TETROMINO_SHAPE_O,
  TETROMINO_SHAPE_T,
  TETROMINO_SHAPE_S,
  TETROMINO_SHAPE_Z,
  TETROMINO_SHAPE_J,
  TETROMINO_SHAPE_L
} TetrominoShape_t;

typedef struct {
  SpriteSheet_t *sheet;
  SDL_FRect *clip;
  SDL_FRect *pos;
  int deg_rot;
  TetrominoShape_t shape;
} Tetromino_t;

typedef struct {
  Tetromino_t **tetrominos;
  size_t size;
  size_t cnt;
} TetrominoCollection_t;

typedef struct {
  int cols;
  int rows;
  Tetromino_t ***arr;
} GameBoard_t;

typedef struct {
  int display_w;
  int display_h;
  double tick_rate;
  GameBoard_t *board;
} GameApp_t;

static GameApp_t game_app = {0};
static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static SpriteSheet_t sheet_tetromino = {0};
static TetrominoCollection_t tetrominos = {0};
static double PREV_TIME = 0.0;
static bool spawn_tetromino = true;

Tetromino_t *Tetromino_init(SpriteSheet_t *const sheet,
                            TetrominoShape_t const shape, double spawn_x,
                            double spawn_y) {
  SDL_FRect *clip = malloc(sizeof(SDL_FRect));
  SDL_FRect *pos = malloc(sizeof(SDL_FRect));
  Tetromino_t *new = malloc(sizeof(Tetromino_t));

  memset(new, 0, sizeof(Tetromino_t));
  memset(clip, 0, sizeof(SDL_FRect));
  memset(pos, 0, sizeof(SDL_FRect));

  new->pos = pos;
  new->clip = clip;

  new->sheet = sheet;
  new->deg_rot = 0;
  new->shape = shape;
  new->pos->x = spawn_x;
  new->pos->y = spawn_y;
  new->pos->h = BLOCK_SIZE_PIXELS * 4;
  new->pos->w = BLOCK_SIZE_PIXELS;

  // based on the shape, grab the proper clip bounding box
  switch (shape) {
  case TETROMINO_SHAPE_I:
    new->clip->x = BLOCK_SIZE_PIXELS;
    new->clip->y = 0;
    new->clip->h = 128;
    new->clip->w = BLOCK_SIZE_PIXELS;
    break;
  case TETROMINO_SHAPE_O:
    break;
  case TETROMINO_SHAPE_T:
    break;
  case TETROMINO_SHAPE_S:
    break;
  case TETROMINO_SHAPE_Z:
    break;
  case TETROMINO_SHAPE_J:
    break;
  case TETROMINO_SHAPE_L:
    break;
  }

  return new;
}

void TetrominoCollection_init(TetrominoCollection_t *const col,
                              size_t const size) {
  col->tetrominos = malloc(sizeof(TetrominoCollection_t) * size);
  col->size = size;
  col->cnt = 0;

  memset(col->tetrominos, 0, sizeof(TetrominoCollection_t) * size);
}

void TetrominoCollection_push(TetrominoCollection_t *const col,
                              Tetromino_t *const new) {
  if (col->cnt + 1 >= col->size) {
    // TODO: resize and add more
    // TODO: also need to possibly add reduce size
    SDL_LogError(SDL_LOG_CATEGORY_TEST, "resize not implemented...\n");
    assert(false);
    return;
  }

  col->tetrominos[col->cnt] = new;
  col->cnt += 1;
}

void TetrominoCollection_render(TetrominoCollection_t const col) {
  for (size_t i = 0; i < col.cnt; i++) {
    SDL_RenderTexture(renderer, sheet_tetromino.sheet, col.tetrominos[i]->clip,
                      col.tetrominos[i]->pos);
  }
}

int SpriteSheet_init(SpriteSheet_t *const sheet, char const *const sheet_path) {
  SDL_Surface *surface = NULL;
  char *bmp_path = NULL;

  SDL_asprintf(&bmp_path, "%sassets/%s", SDL_GetBasePath(), sheet_path);

  surface = SDL_LoadBMP(bmp_path);
  if (!surface) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Load sprite sheet %s: %s",
                 sheet_path, SDL_GetError());
    return SDL_APP_FAILURE;
  }

  SDL_free(bmp_path);

  sheet->sheet = SDL_CreateTextureFromSurface(renderer, surface);
  sheet->h = surface->h;
  sheet->w = surface->w;

  SDL_DestroySurface(surface);

  return SDL_APP_CONTINUE;
}

GameBoard_t *GameBoard_init(int cols, int rows) {
  GameBoard_t *new = malloc(sizeof(GameBoard_t));
  new->cols = cols;
  new->rows = rows;

  new->arr = malloc(sizeof(Tetromino_t **) * rows);
  memset(new->arr, 0, sizeof(Tetromino_t **) * rows);
  for (size_t row = 0; row < new->rows; row++) {
    new->arr[row] = malloc(sizeof(Tetromino_t *) * cols);
    memset(new->arr[row], 0, sizeof(Tetromino_t *) * cols);
  }

  return new;
}

void GameBoard_print_debug() {
  for (size_t row = 0; row < game_app.board->rows; row++) {
    for (size_t col = 0; col < game_app.board->cols; col++) {
      if (game_app.board->arr[row][col] != 0) {
        printf(" 1 ");
      } else {
        printf(" 0 ");
      }
    }
    printf("\n");
  }

  printf("\n");
}

void GameApp_init(GameApp_t *const app) {
  SDL_Rect bounds = {0};
  SDL_GetDisplayBounds(SDL_GetPrimaryDisplay(), &bounds);
  app->display_h = bounds.h;
  app->display_w = bounds.w;
  app->tick_rate = 1000.0 / 1.0;

  // TODO: how big should the board be?
  GameBoard_t *board = GameBoard_init(8, 20);
  app->board = board;
}

void board_spawn_tetromino() {
  // TODO: random shape + new one in waiting

  // CASE: spawn I

  Tetromino_t *new_tetromino =
      Tetromino_init(&sheet_tetromino, TETROMINO_SHAPE_I, 0,
                     game_app.board->cols / 2.0 * BLOCK_SIZE_PIXELS);
  TetrominoCollection_push(&tetrominos, new_tetromino);

  game_app.board->arr[0][game_app.board->cols / 2] = new_tetromino;
  game_app.board->arr[1][game_app.board->cols / 2] = new_tetromino;
  game_app.board->arr[2][game_app.board->cols / 2] = new_tetromino;
  game_app.board->arr[3][game_app.board->cols / 2] = new_tetromino;
}

void board_update() {
  // We tick the board at a different rate than the smooth updates of
  // other objects...
  if (SDL_GetTicks() - PREV_TIME < game_app.tick_rate) {
    return;
  }

  if (spawn_tetromino) {
    board_spawn_tetromino();
    spawn_tetromino = false; // TODO: do I need this flag?
  }

  /* move_rows_down(); */
  /* check_collision(); */
  /* while (lines) { */
  /*   delete_all_lines(); */
  /*   move_down(); */
  /* } */
  PREV_TIME = SDL_GetTicks();
}

void update_world() {

  board_update();

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

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  SDL_SetAppMetadata(CMAKE_PROJECT_NAME, CMAKE_PROJECT_VERSION,
                     "com.rse8.tetris");

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "Init video: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  if (!SDL_Init(SDL_INIT_AUDIO)) {
    SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "Init Audio: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  GameApp_init(&game_app);
  if (!SDL_CreateWindowAndRenderer(
          CMAKE_PROJECT_NAME, game_app.board->cols * BLOCK_SIZE_PIXELS,
          game_app.board->rows * BLOCK_SIZE_PIXELS,
          /* SDL_WINDOW_FULLSCREEN | SDL_WINDOW_BORDERLESS */ 0, &window,
          &renderer)) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Init window and renderer: %s",
                 SDL_GetError());
    return SDL_APP_FAILURE;
  }

  SpriteSheet_init(&sheet_tetromino, "tetronimo_I.bmp");
  TetrominoCollection_init(&tetrominos, 100);

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS;
  }

  if (event->type == SDL_EVENT_KEY_DOWN) {
    if (event->key.scancode == SDL_SCANCODE_R) {
      GameBoard_print_debug();
    }
  }

  return SDL_APP_CONTINUE;
}
SDL_AppResult SDL_AppIterate(void *appstate) {
  update_world();

  SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0);
  SDL_RenderClear(renderer);

  TetrominoCollection_render(tetrominos);

  SDL_RenderPresent(renderer);
  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {}
