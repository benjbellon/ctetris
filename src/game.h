#ifndef GAME_H
#define GAME_H

#define BLOCK_SIZE_PIXELS 32
#define COORDS_SIZE 10

#include <SDL3/SDL_render.h>

typedef struct {
  SDL_Texture *sheet;
  int h;
  int w;
} SpriteSheet_t;

typedef enum TranslationDirection {
  TRANSLATION_DIRECTION_LEFT = -1,
  TRANSLATION_DIRECTION_RIGHT = 1,
} TranslationDirection;

typedef enum {
  USER_INPUT_NONE,
  USER_INPUT_ROTATE_LEFT,
  USER_INPUT_ROTATE_RIGHT,
  USER_INPUT_MOVE_LEFT,
  USER_INPUT_MOVE_RIGHT,
  USER_INPUT_SOFT_DROP,
  USER_INPUT_HARD_DROP,
  USER_INPUT_PAUSE,
  USER_INPUT_SHOW_DEBUG,
} UserInput_t;

typedef enum {
  TETROMINO_SHAPE_TAG_I,
  TETROMINO_SHAPE_TAG_O,
  TETROMINO_SHAPE_TAG_T,
  TETROMINO_SHAPE_TAG_S,
  TETROMINO_SHAPE_TAG_Z,
  TETROMINO_SHAPE_TAG_J,
  TETROMINO_SHAPE_TAG_L
} TetrominoShapeTag;

typedef struct {
  TetrominoShapeTag tag;
  int box_x0;
  int box_y0;
  int const *map;
} TetrominoShape;

typedef enum {
  TETROMINO_STATE_ACTIVE,
  TETROMINO_STATE_LOCKED,
  TETROMINO_STATE_EXPLODED,
  TETROMINO_STATE_INTERSECTED,
  TETROMINO_STATE_PENDING,
} TetrominoState_t;

typedef struct {
  int row;
  int col;
  int square_size;
} TetrominoBoardBound_t;

typedef struct {
  SpriteSheet_t *sheet;
  SDL_FRect *clip;
  int deg_rot;
  TetrominoShapeTag tag;
  TetrominoState_t state;
  TetrominoBoardBound_t *position;

  TetrominoShape *shape;
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
  GameBoard_t *previous_board;
  UserInput_t input;
  TetrominoCollection_t *tetrominos;
} GameApp_t;

typedef struct {
  size_t size;
  Tetromino_t **arr;
} TetrominoLockCache_t;

TetrominoLockCache_t *TetrominoLockCache_init(size_t const size);
void TetrominoLockCache_free(TetrominoLockCache_t *o);
bool TetrominoLockCache_check_and_insert(TetrominoLockCache_t *const col, Tetromino_t *const tetromino);
Tetromino_t *Tetromino_init(TetrominoShapeTag const shape, double spawn_x, double spawn_y);
void Tetromino_free(Tetromino_t *t);

TetrominoCollection_t *TetrominoCollection_init(size_t const size);
void TetrominoCollection_free(TetrominoCollection_t *col);
void TetrominoCollection_push(TetrominoCollection_t *col, Tetromino_t *const new);
bool TetrominoCollection_contains_active(TetrominoCollection_t const *const col);
void TetrominoCollection_render(TetrominoCollection_t *col, SDL_Renderer *render);

int SpriteSheet_init(SDL_Renderer *renderer, SpriteSheet_t *const sheet, char const *const sheet_path);
GameBoard_t *GameBoard_init(int cols, int rows);
void GameBoard_print_debug(GameBoard_t const *const board);

int *GameBoard_get_tetromino_coords(GameBoard_t *const board, Tetromino_t const *const tetromino);
void GameBoard_step_down_active_tetromino(GameBoard_t *board, TetrominoCollection_t *tetrominos);
void GameBoard_translate_left(GameBoard_t *const board, Tetromino_t *const tetromino);
void GameBoard_translate_right(GameBoard_t *const board, Tetromino_t *const tetromino);
void GameBoard_translate_down(GameBoard_t *const board, Tetromino_t *const tetromino);
void GameBoard_rotate_pi_radians(GameBoard_t *const board, Tetromino_t *const tetromino);
void GameBoard_rotate_neg_pi_radians(GameBoard_t *const board, Tetromino_t *const tetromino);
void GameBoard_clear_full_rows(GameBoard_t *board);
void GameBoard_collision_check(GameBoard_t const *const curr, GameBoard_t const *const prev);
void GameBoard_update(GameBoard_t *board, GameApp_t *app_state);
void GameBoard_spawn_tetromino(GameBoard_t **board, TetrominoCollection_t *col, int x, int y);
void GameBoard_copy(GameBoard_t *const dest, GameBoard_t const *const src);

GameApp_t *GameApp_init();
void GameApp_handle_input(GameApp_t *state);
void GameApp_update(GameApp_t *state);

void SpriteSheet_tetrominos(SpriteSheet_t *sheet);

// Each row contains a description of the tetromino bounding box.
// The first two elements are the bounding box origin, followed by
// the associated x and y coordinates of the bounding box.
//
// The I, O tetrominos have 4x4 bounding boxes, all others are 3x3
static const int TETROMINO_BOUNDING_BOX[][COORDS_SIZE] = {
    {0, 0, 1, 0, 1, 1, 1, 2, 1, 3}, // I_0
    /* {0, 0, 2, 0, 2, 1, 2, 2, 2, 3}, // I_90 */
    /* {0, 0, 2, 0, 2, 1, 2, 2, 2, 3}, // I_180 */
    /* {0, 0, 0, 1, 1, 1, 2, 1, 3, 1}, // I_270 */

    {0, 0, 0, 0, 1, 0, 1, 1, 1, 2}, // J_0
    /* {0, 0, 0, 1, 0, 2, 1, 1, 2, 1}, // J_90 */
    /* {0, 0, 1, 0, 1, 1, 1, 2, 2, 2}, // J_180 */
    /* {0, 0, 2, 0, 0, 1, 1, 1, 2, 1}, // J_270 */

    {0, 0, 1, 0, 1, 1, 1, 2, 0, 2}, // L_0
    /* {0, 0, 0, 1, 1, 1, 2, 1, 2, 2}, // L_90 */
    /* {0, 0, 1, 0, 1, 1, 1, 2, 2, 0}, // L_180 */
    /* {0, 0, 0, 0, 0, 1, 1, 1, 2, 1}, // L_270 */

    {0, 0, 0, 1, 0, 2, 1, 1, 1, 2}, // O_0
    /* {0, 0, 0, 1, 0, 2, 1, 1, 1, 2}, // O_90 */
    /* {0, 0, 0, 1, 0, 2, 1, 1, 1, 2}, // O_180 */
    /* {0, 0, 0, 1, 0, 2, 1, 1, 1, 2}, // O_270 */

    {0, 0, 0, 1, 0, 2, 1, 0, 1, 1}, // S_0
    /* {0, 0, 0, 1, 1, 1, 1, 2, 2, 2}, // S_90 */
    /* {0, 0, 2, 0, 2, 1, 1, 1, 1, 2}, // S_180 */
    /* {0, 0, 0, 0, 1, 0, 1, 1, 2, 1}, // S_270 */

    {0, 0, 0, 1, 1, 0, 1, 1, 1, 2}, // T_0
    /* {0, 0, 0, 1, 1, 1, 2, 1, 1, 2}, // T_90 */
    /* {0, 0, 1, 0, 1, 1, 1, 2, 2, 1}, // T_180 */
    /* {0, 0, 0, 1, 1, 0, 1, 1, 2, 1}, // T_270 */

    {0, 0, 0, 0, 0, 1, 1, 1, 1, 2}, // Z_0
                                    /* {0, 0, 0, 2, 1, 1, 1, 2, 2, 1}, // Z_90 */
                                    /* {0, 0, 1, 0, 1, 1, 2, 1, 2, 2}, // Z_180 */
                                    /* {0, 0, 0, 1, 1, 0, 1, 1, 2, 0}, // Z_270 */
};

#endif
