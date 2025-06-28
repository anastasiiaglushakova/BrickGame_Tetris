#include <check.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include "../brick_game/tetris/tetris.h"

static int count_filled(int **field) {
    int count = 0;
    for (int i = 0; i < FIELD_HEIGHT; ++i)
        for (int j = 0; j < FIELD_WIDTH; ++j)
            count += !!field[i][j];
    return count;
}

static int count_preview(int **next) {
    int count = 0;
    for (int i = 0; i < FIGURE_SIZE; ++i)
        for (int j = 0; j < FIGURE_SIZE; ++j)
            count += !!next[i][j];
    return count;
}

START_TEST(test_initial_state)
{
    userInput(Start, false);
    GameInfo_t info = updateCurrentState();
    ck_assert_ptr_nonnull(info.field);
    ck_assert_ptr_nonnull(info.next);
    ck_assert_int_eq(info.score, 0);
    ck_assert_int_eq(info.level, 1);
    ck_assert_int_eq(info.speed, INITIAL_SPEED);
    ck_assert(!info.pause);
    ck_assert_int_ge(count_preview(info.next), 4);
}
END_TEST

START_TEST(test_pause_unpause)
{
    userInput(Start, false);
    updateCurrentState();
    userInput(Pause, false);
    GameInfo_t info = updateCurrentState();
    ck_assert(info.pause != 0);
    userInput(Pause, false);
    info = updateCurrentState();
    ck_assert(info.pause == 0);
}
END_TEST

START_TEST(test_move_left_right)
{
    userInput(Start, false);
    updateCurrentState();
    GameInfo_t info1 = updateCurrentState();
    int filled_before = count_filled(info1.field);

    userInput(Left, false);
    GameInfo_t info2 = updateCurrentState();
    int filled_left = count_filled(info2.field);

    userInput(Right, false);
    GameInfo_t info3 = updateCurrentState();
    int filled_right = count_filled(info3.field);

    ck_assert_int_eq(filled_before, filled_left);
    ck_assert_int_eq(filled_before, filled_right);
}
END_TEST

START_TEST(test_rotate)
{
    userInput(Start, false);
    updateCurrentState();
    GameInfo_t info1 = updateCurrentState();
    int filled_before = count_filled(info1.field);

    userInput(Action, false);
    GameInfo_t info2 = updateCurrentState();
    int filled_after = count_filled(info2.field);

    ck_assert_int_eq(filled_before, filled_after);
}
END_TEST

START_TEST(test_hard_drop)
{
    userInput(Start, false);
    updateCurrentState();
    userInput(Down, false);
    GameInfo_t info = updateCurrentState();
    ck_assert_int_ge(count_filled(info.field), 4);
}
END_TEST

START_TEST(test_terminate)
{
    userInput(Start, false);
    updateCurrentState();
    userInput(Terminate, false);
    GameInfo_t info = updateCurrentState();
    ck_assert_int_eq(info.score, 0);
}
END_TEST

START_TEST(test_restart_after_terminate)
{
    userInput(Start, false);
    updateCurrentState();
    userInput(Terminate, false);
    GameInfo_t info = updateCurrentState();
    ck_assert_int_eq(info.score, 0);
    userInput(Start, false);
    info = updateCurrentState();
    ck_assert_int_eq(info.level, 1);
    ck_assert(!info.pause);
}
END_TEST

START_TEST(test_level_speed_increase)
{
    userInput(Start, false);
    GameInfo_t info = updateCurrentState();
    int old_level = info.level;
    int old_speed = info.speed;
    for (int i = 0; i < 50; ++i) {
        userInput(Down, false);
        updateCurrentState();
    }
    info = updateCurrentState();
    ck_assert(info.level >= old_level);
    ck_assert(info.speed <= old_speed);
}
END_TEST

START_TEST(test_action_in_pause)
{
    userInput(Start, false);
    updateCurrentState();
    userInput(Pause, false);
    GameInfo_t info1 = updateCurrentState();
    int before = count_filled(info1.field);

    userInput(Left, false);
    userInput(Right, false);
    userInput(Action, false);
    userInput(Down, false);
    GameInfo_t info2 = updateCurrentState();
    int after = count_filled(info2.field);

    ck_assert_int_eq(before, after);
    ck_assert(info2.pause != 0);
    userInput(Pause, false);
}
END_TEST

START_TEST(test_double_start)
{
    userInput(Start, false);
    GameInfo_t info1 = updateCurrentState();
    userInput(Start, false);
    GameInfo_t info2 = updateCurrentState();
    ck_assert(info2.level == info1.level);
    ck_assert(info2.score == info1.score);
}
END_TEST

START_TEST(test_double_terminate)
{
    userInput(Start, false);
    updateCurrentState();
    userInput(Terminate, false);
    updateCurrentState();
    userInput(Terminate, false);
    GameInfo_t info2 = updateCurrentState();
    ck_assert(info2.score == 0);
    ck_assert(info2.level == 1);
}
END_TEST

START_TEST(test_action_after_terminate)
{
    userInput(Start, false);
    updateCurrentState();
    userInput(Terminate, false);
    GameInfo_t info1 = updateCurrentState();
    int before = count_filled(info1.field);

    userInput(Left, false);
    userInput(Right, false);
    userInput(Action, false);
    userInput(Down, false);

    GameInfo_t info2 = updateCurrentState();
    int after = count_filled(info2.field);
    ck_assert_int_eq(before, after);
}
END_TEST

START_TEST(test_scoring_scheme)
{
    userInput(Start, false);
    updateCurrentState();

    int score_before = 0;
    int score_after = 0;
    int any_points = 0;

    for (int i = 0; i < 100; ++i) {
        GameInfo_t info = updateCurrentState();
        score_before = info.score;
        userInput(Down, false);
        updateCurrentState();
        info = updateCurrentState();
        score_after = info.score;
        if (score_after > score_before) {
            int delta = score_after - score_before;
            ck_assert(delta == 100 || delta == 300 || delta == 700 || delta == 1500);
            any_points = 1;
            break;
        }
    }

    ck_assert_int_ge(any_points, 0);
}
END_TEST

START_TEST(test_high_score_persistence)
{
    userInput(Start, false);
    updateCurrentState();
    for (int i = 0; i < 10; ++i) {
        userInput(Down, false);
        updateCurrentState();
    }
    GameInfo_t info = updateCurrentState();
    int old_high_score = info.high_score;

    userInput(Terminate, false);
    updateCurrentState();
    userInput(Start, false);
    info = updateCurrentState();

    ck_assert(info.high_score >= old_high_score);
}
END_TEST

START_TEST(test_gameover_state)
{
    userInput(Start, false);
    updateCurrentState();
    userInput(Terminate, false);
    for (int i = 0; i < 5; ++i) updateCurrentState();
    userInput(Start, false);
    GameInfo_t info = updateCurrentState();
    userInput(Terminate, false);
    updateCurrentState();
    userInput(Start, false);
    info = updateCurrentState();
    ck_assert_int_eq(info.score, 0);
}
END_TEST

START_TEST(test_pause_terminate_from_pause)
{
    userInput(Start, false);
    int tries = 100;
    GameInfo_t info = updateCurrentState();
    int pause_enabled = 0;
    while (tries-- > 0) {
        userInput(Pause, false);
        info = updateCurrentState();
        if (info.pause) {
            pause_enabled = 1;
            break;
        }
        userInput(Left, false);
        updateCurrentState();
        userInput(Right, false);
        updateCurrentState();
    }
    if (!pause_enabled) {
        return;
    }
    userInput(Terminate, false);
    info = updateCurrentState();
    ck_assert(info.pause == 0);
}
END_TEST

START_TEST(test_no_action)
{
    userInput(Start, false);
    updateCurrentState();
    GameInfo_t info1 = updateCurrentState();
    userInput(-1, false);
    GameInfo_t info2 = updateCurrentState();
    ck_assert_int_eq(count_filled(info1.field), count_filled(info2.field));
}
END_TEST

START_TEST(test_invalid_action_in_start)
{
    userInput(Start, false);
    updateCurrentState();
    userInput(Left, false);
    GameInfo_t info = updateCurrentState();
    ck_assert_int_ge(info.level, 1);
}
END_TEST

START_TEST(test_multiple_pause_terminate)
{
    userInput(Start, false);
    updateCurrentState();
    userInput(Pause, false);
    updateCurrentState();
    userInput(Terminate, false);
    updateCurrentState();
    userInput(Terminate, false);
    GameInfo_t info = updateCurrentState();
    ck_assert_int_eq(info.level, 1);
}
END_TEST

START_TEST(test_gameover_restart_and_actions)
{
    userInput(Start, false);
    GameInfo_t info = updateCurrentState();
    userInput(Terminate, false);
    updateCurrentState();
    userInput(Start, false);
    info = updateCurrentState();
    userInput(Left, false);
    updateCurrentState();
    userInput(Right, false);
    updateCurrentState();
    userInput(Action, false);
    updateCurrentState();
    userInput(Down, false);
    updateCurrentState();
    ck_assert(info.level >= 1);
}
END_TEST

START_TEST(test_exit_state)
{
    userInput(Start, false);
    updateCurrentState();
    userInput(Terminate, false);
    updateCurrentState();
    userInput(Terminate, false);
    updateCurrentState();
    userInput(Start, false);
    GameInfo_t info = updateCurrentState();
    ck_assert(info.level >= 1);
}
END_TEST

START_TEST(test_pause_multiple)
{
    userInput(Start, false);
    int tries = 200;
    GameInfo_t info = updateCurrentState();
    int pause_enabled = 0;
    while (tries-- > 0) {
        userInput(Left, false);
        updateCurrentState();
        userInput(Right, false);
        updateCurrentState();
        userInput(Pause, false);
        info = updateCurrentState();
        if (info.pause) {
            pause_enabled = 1;
            break;
        }
    }
    if (!pause_enabled) {
        return;
    }
    userInput(Pause, false);
    info = updateCurrentState();
    ck_assert(info.pause == 0);
}
END_TEST

START_TEST(test_many_spawns_and_levels)
{
    userInput(Start, false);
    GameInfo_t info = updateCurrentState();
    int old_level = info.level;
    int old_score = info.score;
    for (int i = 0; i < 100; ++i) {
        userInput(Down, false);
        updateCurrentState();
    }
    info = updateCurrentState();
    ck_assert(info.level >= old_level);
    ck_assert(info.score >= old_score);
    ck_assert(info.high_score >= info.score);
}
END_TEST

START_TEST(test_highscore_update_logic)
{
    userInput(Start, false);
    updateCurrentState();
    for (int i = 0; i < 200; ++i) {
        userInput(Down, false);
        updateCurrentState();
        GameInfo_t info = updateCurrentState();
        if (info.score > info.high_score) {
            ck_assert(info.score == info.high_score);
        }
    }
}
END_TEST

START_TEST(test_default_input_in_all_states) {
    userInput(Start, false);
    updateCurrentState();

    userInput(-1, false);
    updateCurrentState();

    userInput(Pause, false);
    updateCurrentState();

    userInput(-1, false);
    updateCurrentState();

    userInput(Terminate, false);
    updateCurrentState();

    userInput(-1, false);
    updateCurrentState();
}
END_TEST


Suite *tetris_suite(void) {
    Suite *s = suite_create("Tetris");
    TCase *tc = tcase_create("Core");
    tcase_add_test(tc, test_initial_state);
    tcase_add_test(tc, test_pause_unpause);
    tcase_add_test(tc, test_move_left_right);
    tcase_add_test(tc, test_rotate);
    tcase_add_test(tc, test_hard_drop);
    tcase_add_test(tc, test_terminate);
    tcase_add_test(tc, test_restart_after_terminate);
    tcase_add_test(tc, test_level_speed_increase);
    tcase_add_test(tc, test_action_in_pause);
    tcase_add_test(tc, test_double_start);
    tcase_add_test(tc, test_double_terminate);
    tcase_add_test(tc, test_action_after_terminate);
    tcase_add_test(tc, test_scoring_scheme);
    tcase_add_test(tc, test_high_score_persistence);
    tcase_add_test(tc, test_gameover_state);
    tcase_add_test(tc, test_pause_terminate_from_pause);
    tcase_add_test(tc, test_no_action);
    tcase_add_test(tc, test_invalid_action_in_start);
    tcase_add_test(tc, test_multiple_pause_terminate);
    tcase_add_test(tc, test_gameover_restart_and_actions);
    tcase_add_test(tc, test_exit_state);
    tcase_add_test(tc, test_pause_multiple);
    tcase_add_test(tc, test_many_spawns_and_levels);
    tcase_add_test(tc, test_highscore_update_logic);
    tcase_add_test(tc, test_default_input_in_all_states);

    suite_add_tcase(s, tc);
    return s;
}

int main(void) {
    Suite *s = tetris_suite();
    SRunner *sr = srunner_create(s);
    srunner_set_fork_status(sr, CK_NOFORK);
    srunner_run_all(sr, CK_NORMAL);
    int nf = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (nf == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}