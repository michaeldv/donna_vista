// Copyright (c) 2014-2015 by Michael Dvorkin. All Rights Reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.

// Default move ordering.
void test_generate_suite__010(void) {
    char buffer[256];
    setup("Ka1,a2,b3,c4,d2,e6,f5,g4,h3", "Kc1");
    MoveGen *gen = new_move_gen(start());
    generate_moves(gen);
    full_rank(gen, (Move)0);

    cl_assert_equal_s(all_moves(gen, buffer), "[a2-a3 a2-a4 d2-d3 d2-d4 b3-b4 h3-h4 c4-c5 g4-g5 f5-f6 e6-e7 Ka1-b1 Ka1-b2]");
}

// LVA/MVV capture ordering.
void test_generate_suite__110(void) {
    char buffer[256];
    setup("Kd4,e4,Nf4,Bc4,Ra5,Qh5", "Kd8,Qd5");
    Position *p = start();
    MoveGen *gen = new_move_gen(p);
    generate_captures(gen);
    full_rank(gen, (Move)0);

    cl_assert_equal_s(all_moves(gen, buffer), "[e4xd5 Nf4xd5 Bc4xd5 Ra5xd5 Qh5xd5 Kd4xd5]");
}

void test_generate_suite__120(void) {
    char buffer[256];
    setup("Kd4,e4,Nf4,Bc4,Ra5,Qh5", "Kd8,Qd5,Rf5");
    Position *p = start();
    MoveGen *gen = new_move_gen(p);
    generate_captures(gen);
    full_rank(gen, (Move)0);

    cl_assert_equal_s(all_moves(gen, buffer), "[e4xd5 Nf4xd5 Bc4xd5 Ra5xd5 Kd4xd5 e4xf5 Qh5xf5]");
}

void test_generate_suite__130(void) {
    char buffer[256];
    setup("Kd4,e4,Nf4,Bc4,Ra5,Qh5", "Kd8,Qd5,Rf5,Bg6");
    Position *p = start();
    MoveGen *gen = new_move_gen(p);
    generate_captures(gen);
    full_rank(gen, (Move)0);

    cl_assert_equal_s(all_moves(gen, buffer), "[e4xd5 Nf4xd5 Bc4xd5 Ra5xd5 Kd4xd5 e4xf5 Qh5xf5 Nf4xg6 Qh5xg6]");
}

void test_generate_suite__140(void) {
    char buffer[256];
    setup("Kd4,e4,Nf4,Bc4,Ra5,Qh5", "Kd8,Qd5,Rf5,Bg6,Nh3");
    Position *p = start();
    MoveGen *gen = new_move_gen(p);
    generate_captures(gen);
    full_rank(gen, (Move)0);

    cl_assert_equal_s(all_moves(gen, buffer), "[e4xd5 Nf4xd5 Bc4xd5 Ra5xd5 Kd4xd5 e4xf5 Qh5xf5 Nf4xg6 Qh5xg6 Nf4xh3 Qh5xh3]");
}

void test_generate_suite__150(void) {
    char buffer[256];
    setup("Kd4,e4,Nf4,Bc4,Ra5,Qh5", "Kd8,Qd5,Rf5,Bg6,Nh3,e2");
    Position *p = start();
    MoveGen *gen = new_move_gen(p);
    generate_captures(gen);
    full_rank(gen, (Move)0);

    cl_assert_equal_s(all_moves(gen, buffer), "[e4xd5 Nf4xd5 Bc4xd5 Ra5xd5 Kd4xd5 e4xf5 Qh5xf5 Nf4xg6 Qh5xg6 Nf4xh3 Qh5xh3 Nf4xe2 Bc4xe2 Qh5xe2]");
}

// Vaditaing generated moves.
void test_generate_suite__200(void) {
    char buffer[256];
    setup("Ke1,Qe2,d2", "Ke8,e4");
    Position *p = start();
    p = make_move(p, new_enpassant(p, D2, D4));

    // No e4xd3 en-passant capture.
    MoveGen *gen = new_move_gen(p);
    generate_moves(gen);
    valid_only(gen);

    cl_assert_equal_s(all_moves(gen, buffer), "[e4-e3 Ke8-d7 Ke8-e7 Ke8-f7 Ke8-d8 Ke8-f8]");
}

void test_generate_suite__210(void) {
    char buffer[256];
    setup("Ke1,Qg2,d2", "Ka8,e4");
    Position *p = start();
    p = make_move(p, new_enpassant(p, D2, D4));

    // Neither e4-e3 nor e4xd3 en-passant capture.
    MoveGen *gen = new_move_gen(p);
    generate_moves(gen);
    valid_only(gen);

    cl_assert_equal_s(all_moves(gen, buffer), "[Ka8-a7 Ka8-b7 Ka8-b8]");
}
