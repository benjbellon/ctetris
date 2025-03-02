#include "tetromino.h"
#include <SDL3/SDL_rect.h>
#include <stdlib.h>

Tetromino_t *tetromino_init(SpriteSheet_t *const sheet,
                            TetrominoShape_t const shape,
                            SDL_FRect *const pos) {
  SDL_FRect *clip = NULL;
  Tetromino_t *new = malloc(sizeof(Tetromino_t));

  new->sheet = sheet;
  new->deg_rot = 0;
  new->shape = shape;
  new->pos = pos;

  // based on the shape, grab the proper clip bounding box
  switch (shape) {
  case TETROMINO_SHAPE_I:
    new->clip->x = 64;
    new->clip->y = 0;
    new->clip->h = 128;
    new->clip->w = 64;
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

  new->clip;

  return new;
}
