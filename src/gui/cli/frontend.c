#include "frontend.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BORDER_CHAR "##"
#define BLOCK_CHAR "[]"
#define EMPTY_CHAR "  "
#define SLEEP_MS 16

static bool g_game_running = true;

void InitializeInterface(void) {
  initscr();
  cbreak();
  noecho();
  nodelay(stdscr, TRUE);
  keypad(stdscr, TRUE);
  curs_set(0);
  int rows, cols;
  getmaxyx(stdscr, rows, cols);
  int min_rows = FIELD_HEIGHT + 6;
  int min_cols = (FIELD_WIDTH + 15) * 2;
  if (rows < min_rows || cols < min_cols) {
    endwin();
    printf("Terminal too small. Minimum required: %dx%d\n", min_cols, min_rows);
    exit(EXIT_FAILURE);
  }
}

void CleanupInterface(void) { endwin(); }

static void DrawBorder(void) {
  for (int j = -1; j <= FIELD_WIDTH; j++) {
    mvprintw(0, (j + 1) * 2, BORDER_CHAR);
    mvprintw(FIELD_HEIGHT + 1, (j + 1) * 2, BORDER_CHAR);
  }
  for (int i = 0; i <= FIELD_HEIGHT + 1; i++) {
    mvprintw(i, 0, BORDER_CHAR);
    mvprintw(i, (FIELD_WIDTH + 1) * 2, BORDER_CHAR);
  }
}

static void DrawField(GameInfo_t state) {
  if (!state.field) return;
  for (int i = 0; i < FIELD_HEIGHT; i++) {
    for (int j = 0; j < FIELD_WIDTH; j++) {
      mvprintw(i + 1, (j + 1) * 2, state.field[i][j] ? BLOCK_CHAR : EMPTY_CHAR);
    }
  }
}

static void DrawNextPiece(GameInfo_t state) {
  if (!state.next) return;
  int info_x = (FIELD_WIDTH + 2) * 2 + 2;
  mvprintw(2, info_x, "Next:");
  for (int i = 0; i < FIGURE_SIZE; i++) {
    for (int j = 0; j < FIGURE_SIZE; j++) {
      mvprintw(3 + i, info_x + j * 2,
               state.next[i][j] ? BLOCK_CHAR : EMPTY_CHAR);
    }
  }
}

static void DrawStatistics(GameInfo_t state) {
  int info_x = (FIELD_WIDTH + 2) * 2 + 2;
  mvprintw(8, info_x, "Level: %d", state.level);
  mvprintw(9, info_x, "Score: %d", state.score);
  mvprintw(10, info_x, "Speed: %dms", state.speed);
  if (state.high_score > 0) {
    mvprintw(11, info_x, "High: %d", state.high_score);
  }
}

static void DrawControls(void) {
  mvprintw(FIELD_HEIGHT + 3, 0, "Controls:");
  mvprintw(FIELD_HEIGHT + 4, 0,
           "Left, A  Right, D  Drop: Down, S  Rotate: Up, Space");
  mvprintw(FIELD_HEIGHT + 5, 0, "Pause: P  Quit: Q  Start: Enter");
}

static void DrawPauseMessage(void) {
  int center_y = FIELD_HEIGHT / 2 + 1;
  int center_x = FIELD_WIDTH + 1;
  mvprintw(center_y, center_x - 3, "PAUSED");
  mvprintw(center_y + 1, center_x - 7, "Press P to continue");
}

void ShowMainMenu(void) {
  clear();
  int center_y = LINES / 2;
  int center_x = COLS / 2;
  mvprintw(center_y - 3, center_x - 8, "=== TETRIS ===");
  mvprintw(center_y - 1, center_x - 10, "Press Enter to start");
  mvprintw(center_y, center_x - 6, "Press Q to quit");
  refresh();
}

void DrawGame(GameInfo_t state) {
  clear();
  DrawBorder();
  DrawField(state);
  DrawNextPiece(state);
  DrawStatistics(state);
  DrawControls();
  if (state.pause) {
    DrawPauseMessage();
  }
  refresh();
}

UserAction_t GetActionFromKey(int key) {
  switch (key) {
    case '\n':
    case '\r':
    case KEY_ENTER:
      return Start;
    case 'p':
    case 'P':
      return Pause;
    case 'q':
    case 'Q':
      g_game_running = false;
      return Terminate;
    case KEY_LEFT:
    case 'a':
    case 'A':
      return Left;
    case KEY_RIGHT:
    case 'd':
    case 'D':
      return Right;
    case KEY_DOWN:
    case 's':
    case 'S':
      return Down;
    case ' ':
    case KEY_UP:
      return Action;
    default:
      return -1;
  }
}

bool IsGameRunning(void) { return g_game_running; }