#ifndef GUI_CLI_FRONTEND_H_
#define GUI_CLI_FRONTEND_H_

#include <ncurses.h>

#include "../../brick_game/tetris/tetris.h"

void InitializeInterface(void);
void CleanupInterface(void);
void ShowMainMenu(void);
void DrawGame(GameInfo_t state);
UserAction_t GetActionFromKey(int key);
bool IsGameRunning(void);

#endif  // GUI_CLI_FRONTEND_H_