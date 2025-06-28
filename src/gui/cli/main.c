#include <unistd.h>

#include "frontend.h"

#define GAME_TICK_MS 16

int main(void) {
  InitializeInterface();
  bool in_menu = true;

  while (IsGameRunning()) {
    int key = getch();

    if (in_menu) {
      ShowMainMenu();
      if (key != ERR) {
        UserAction_t action = GetActionFromKey(key);
        if (action == Start) {
          userInput(Start, false);
          in_menu = false;
        } else if (action == Terminate) {
          break;
        }
      }
    } else {
      if (key != ERR) {
        UserAction_t action = GetActionFromKey(key);
        if (action != (UserAction_t)(-1)) {
          userInput(action, false);
        }
      }
      GameInfo_t state = updateCurrentState();
      DrawGame(state);
    }

    usleep(GAME_TICK_MS * 1000);
  }

  CleanupInterface();
  return 0;
}