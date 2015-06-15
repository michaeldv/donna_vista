// Copyright (c) 2014-2015 by Michael Dvorkin. All Rights Reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.

void test_game_suite__000(void) {
    new_game();
    Position *p = start();

    Move m0 = new_move(p, E2, E4);
    Move m1 = new_move(p, E7, E5);
    Move m2 = new_move(p, G1, F3);
    Move m3 = new_move(p, B8, C6);
    Move m4 = new_move(p, F1, B5);
    save_best(0, m0);
    save_best(1, m1);
    save_best(2, m2);
    save_best(3, m3);
    save_best(4, m4);

    cl_check(game.pv[0].size == 1);
    cl_check(game.pv[0].moves[0] == m0);
    cl_check(game.pv[1].size == 2);
    cl_check(game.pv[1].moves[0] == (Move)0);
    cl_check(game.pv[1].moves[1] == m1);
    cl_check(game.pv[2].size == 3);
    cl_check(game.pv[2].moves[0] == (Move)0);
    cl_check(game.pv[2].moves[1] == (Move)0);
    cl_check(game.pv[2].moves[2] == m2);
    cl_check(game.pv[3].size == 4);
    cl_check(game.pv[3].moves[0] == (Move)0);
    cl_check(game.pv[3].moves[1] == (Move)0);
    cl_check(game.pv[3].moves[2] == (Move)0);
    cl_check(game.pv[3].moves[3] == m3);
    cl_check(game.pv[4].size == 5);
    cl_check(game.pv[4].moves[0] == (Move)0);
    cl_check(game.pv[4].moves[1] == (Move)0);
    cl_check(game.pv[4].moves[2] == (Move)0);
    cl_check(game.pv[4].moves[3] == (Move)0);
    cl_check(game.pv[4].moves[4] == m4);

    Move m5 = new_move(p, A7, A6);
    save_best(1, m5);

    cl_check(game.pv[0].size == 1);
    cl_check(game.pv[0].moves[0] == m0);
    cl_check(game.pv[1].size == 3);                       // <--
    cl_check(game.pv[1].moves[0] == (Move)0);
    cl_check(game.pv[1].moves[1] == m5);                  // <--
    cl_check(game.pv[1].moves[2] == game.pv[2].moves[2]); // <--
    cl_check(game.pv[2].size == 3);
    cl_check(game.pv[2].moves[0] == (Move)0);
    cl_check(game.pv[2].moves[1] == (Move)0);
    cl_check(game.pv[2].moves[2] == m2);
    cl_check(game.pv[3].size == 4);
    cl_check(game.pv[3].moves[0] == (Move)0);
    cl_check(game.pv[3].moves[1] == (Move)0);
    cl_check(game.pv[3].moves[2] == (Move)0);
    cl_check(game.pv[3].moves[3] == m3);
    cl_check(game.pv[4].size == 5);
    cl_check(game.pv[4].moves[0] == (Move)0);
    cl_check(game.pv[4].moves[1] == (Move)0);
    cl_check(game.pv[4].moves[2] == (Move)0);
    cl_check(game.pv[4].moves[3] == (Move)0);
    cl_check(game.pv[4].moves[4] == m4);
}
