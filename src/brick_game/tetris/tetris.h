#ifndef BRICK_GAME_TETRIS_TETRIS_H_
#define BRICK_GAME_TETRIS_TETRIS_H_

#include <stdbool.h>

#define FIELD_WIDTH 10
#define FIELD_HEIGHT 20
#define FIGURE_SIZE 4
#define TETRIS_FIGURES_COUNT 7

#define SPAWN_X 3
#define SPAWN_Y 0

#define INITIAL_SPEED 500
#define MIN_SPEED 50
#define SPEED_DECREASE 50

typedef enum {
  Start,
  Pause,
  Terminate,
  Left,
  Right,
  Up,
  Down,
  Action
} UserAction_t;

typedef struct {
  int **field;
  int **next;
  int score;
  int high_score;
  int level;
  int speed;
  bool pause;
} GameInfo_t;

void userInput(UserAction_t action, bool hold);
GameInfo_t updateCurrentState();

#endif  // BRICK_GAME_TETRIS_TETRIS_H_