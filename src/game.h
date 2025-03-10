#ifndef GAME_H
#define GAME_H

#include <stddef.h>
#include <stdint.h>

#define MINO_COORDS_SIZE 8

typedef enum {
  TETROMINO_SHAPE_I,
  TETROMINO_SHAPE_J,
  TETROMINO_SHAPE_L,
  TETROMINO_SHAPE_O,
  TETROMINO_SHAPE_S,
  TETROMINO_SHAPE_T,
  TETROMINO_SHAPE_Z,
} ETetrominoShape;
typedef enum { TETROMINO_STATE_ACTIVE } ETetrominoState;

typedef struct {
  size_t row0, col0;
  uint32_t deg;
  size_t const *mino_coords;
  size_t mino_shift[MINO_COORDS_SIZE];
  uint8_t mino_mask;
  uint8_t bound_size;
  ETetrominoShape shape;
  ETetrominoState state;
} Tetromino;

typedef struct {
  Tetromino **arr;
  size_t cap, cnt;
} TetrominoCollection;

typedef struct {
  size_t rows, cols;
  TetrominoCollection *coll;
} TetrominoWell;

typedef struct {
  TetrominoWell *well;
} GameState;

Tetromino *Tetromino_init(ETetrominoShape const shape, size_t const row, size_t const col);
void Tetromino_free(Tetromino *t);
void Tetromino_hide_mino(Tetromino *const t, uint8_t const row);
void Tetromino_shift_mino(Tetromino *const t, size_t const mino_idx, size_t const row_shift);
void Tetromino_translate(Tetromino *const t, size_t const row_shift, size_t const col_shift);

TetrominoCollection *TetrominoCollection_init(size_t const cap);
void TetrominoCollection_free(TetrominoCollection *coll);
void TetrominoCollection_push(TetrominoCollection *const coll, Tetromino *const t);
void TetrominoCollection_resize(TetrominoCollection *const coll);

size_t *TetrominoWell_coords(Tetromino const *const t);
TetrominoWell *TetrominoWell_init(void);
void TetrominoWell_free(TetrominoWell *t);

GameState *GameState_init(void);
void GameState_free(GameState *t);

#endif
