#include "game.h"
#define SDL_MAIN_USE_CALLBACKS 1

#include "_gen/cmake_variables.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <stdio.h>

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

void stdoutLog(void *UNUSED(userdata), int UNUSED(category), SDL_LogPriority UNUSED(priority), const char *message) {
  printf("%s\n", message);
}

SDL_AppResult SDL_AppInit(void **UNUSED(appstate), int UNUSED(argc), char *UNUSED(argv[])) {
  SDL_SetLogPriorities(SDL_LOG_PRIORITY_DEBUG);
  SDL_SetLogOutputFunction(stdoutLog, NULL);

  SDL_SetAppMetadata(CMAKE_PROJECT_NAME, CMAKE_PROJECT_VERSION, "com.rse8.tetris");

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "Init video: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  if (!SDL_Init(SDL_INIT_AUDIO)) {
    SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "Init Audio: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  // TODO: initialize GameState

  if (!SDL_CreateWindowAndRenderer(CMAKE_PROJECT_NAME, 100, 100,
                                   /* SDL_WINDOW_FULLSCREEN | SDL_WINDOW_BORDERLESS, */
                                   0, &window, &renderer)) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Init window and renderer: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *UNUSED(appstate), SDL_Event *event) {
  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS;
  }
  return SDL_APP_CONTINUE;
  ;
}

SDL_AppResult SDL_AppIterate(void *UNUSED(appstate)) {

  Tetromino *t = Tetromino_init(TETROMINO_SHAPE_I, 0, 0);
  TetrominoCollection *coll = TetrominoCollection_init(100);
  TetrominoCollection_push(coll, t);
  TetrominoCollection_free(coll);
  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *UNUSED(appstate), SDL_AppResult UNUSED(result)) { return; }
