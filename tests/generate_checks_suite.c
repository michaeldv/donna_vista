// Copyright (c) 2014-2015 by Michael Dvorkin. All Rights Reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.

// Knight.
void test_generate_checks_suite__000(void) {
    char buffer[256];
    setup("Ka1,Nd7,Nf3,b3", "Kh7,Nd4,f6");
    Position *p = start();
    MoveGen *white = new_move_gen(p);
    generate_checks(white);
    cl_assert_equal_s(all_moves(white, buffer), "[Nf3-g5 Nd7-f8]");

    p->color = Black;
    MoveGen *black = new_move_gen(p);
    generate_checks(black);
    cl_assert_equal_s(all_moves(black, buffer), "[Nd4-c2]");
}

// Bishop->
void test_generate_checks_suite__010(void) {
    char buffer[256];
    setup("Kh2,Ba2", "Kh7,Ba7");
    Position *p = start();
    MoveGen *white = new_move_gen(p);
    generate_checks(white);
    cl_assert_equal_s(all_moves(white, buffer), "[Ba2-b1 Ba2-g8]");

    p->color = Black;
    MoveGen *black = new_move_gen(p);
    generate_checks(black);
    cl_assert_equal_s(all_moves(black, buffer), "[Ba7-g1 Ba7-b8]");
}

void test_generate_checks_suite__020(void) {
    char buffer[256];
    setup("Kf4,Bc1", "Kc5,Bf8,h6,e3");
    Position *p = start();
    MoveGen *white = new_move_gen(p);
    generate_checks(white);
    cl_assert_equal_s(all_moves(white, buffer), "[Bc1-a3]");

    p->color = Black;
    MoveGen *black = new_move_gen(p);
    generate_checks(black);
    cl_assert_equal_s(all_moves(black, buffer), "[Bf8-d6]");
}

// Bishop: discovered non-capturing check with blocked diaginal.
void test_generate_checks_suite__030(void) {
    char buffer[256];
    setup("Ka8,Ba1,Nb2,c3,f3", "Kh8,Bh1,Ng2");
    Position *p = start();
    MoveGen *white = new_move_gen(p);
    generate_checks(white);
    cl_assert_equal_s(all_moves(white, buffer), "[]");

    p->color = Black;
    MoveGen *black = new_move_gen(p);
    generate_checks(black);
    cl_assert_equal_s(all_moves(black, buffer), "[]");
}

// Bishop: discovered non-capturing check: Knight.
void test_generate_checks_suite__040(void) {
    char buffer[256];
    setup("Ka8,Ba1,Nb2,a4,h4", "Kh8,Bh1,Ng2,c4,f4");
    Position *p = start();
    MoveGen *white = new_move_gen(p);
    generate_checks(white);
    cl_assert_equal_s(all_moves(white, buffer), "[Nb2-d1 Nb2-d3]");

    p->color = Black;
    MoveGen *black = new_move_gen(p);
    generate_checks(black);
    cl_assert_equal_s(all_moves(black, buffer), "[Ng2-e1 Ng2-e3]");
}

// Bishop: discovered non-capturing check: Rook.
void test_generate_checks_suite__050(void) {
    char buffer[256];
    setup("Ka8,Qa1,Nb1,Rb2,b4,d2,e2", "Kh8,Qh1,Rg2,g4");
    Position *p = start();
    MoveGen *white = new_move_gen(p);
    generate_checks(white);
    cl_assert_equal_s(all_moves(white, buffer), "[Rb2-a2 Rb2-c2 Rb2-b3]");

    p->color = Black;
    MoveGen *black = new_move_gen(p);
    generate_checks(black);
    cl_assert_equal_s(all_moves(black, buffer), "[Rg2-g1 Rg2-f2 Rg2-h2 Rg2-g3]");
}

// Bishop: discovered non-capturing check: King.
void test_generate_checks_suite__060(void) {
    char buffer[256];
    setup("Ke5,Qc3,c4,d3,e4", "Kh8,e6");
    Position *p = start();
    MoveGen *white = new_move_gen(p);
    generate_checks(white);
    cl_assert_equal_s(all_moves(white, buffer), "[Ke5-f4 Ke5-d5 Ke5-f5 Ke5-d6]");
}

// Bishop: discovered non-capturing check: Pawn move.
void test_generate_checks_suite__070(void) {
    char buffer[256];
    setup("Ka8,Ba1,c3", "Kh8,Bg2,e4");
    Position *p = start();
    MoveGen *white = new_move_gen(p);
    generate_checks(white);
    cl_assert_equal_s(all_moves(white, buffer), "[c3-c4]");

    p->color = Black;
    MoveGen *black = new_move_gen(p);
    generate_checks(black);
    cl_assert_equal_s(all_moves(black, buffer), "[e4-e3]");
}

// Bishop: discovered non-capturing check: Pawn jump->
void test_generate_checks_suite__080(void) {
    char buffer[256];
    setup("Kh2,Bb1,c2", "Kh7,Bb8,c7");
    Position *p = start();
    MoveGen *white = new_move_gen(p);
    generate_checks(white);
    cl_assert_equal_s(all_moves(white, buffer), "[c2-c3 c2-c4]");

    p->color = Black;
    MoveGen *black = new_move_gen(p);
    generate_checks(black);
    cl_assert_equal_s(all_moves(black, buffer), "[c7-c5 c7-c6]");
}

// Bishop: discovered non-capturing check: no pawn promotions.
void test_generate_checks_suite__090(void) {
    char buffer[256];
    setup("Kh7,Bb8,c7", "Kh2,Bb1,c2");
    Position *p = start();
    MoveGen *white = new_move_gen(p);
    generate_checks(white);
    cl_assert_equal_s(all_moves(white, buffer), "[]");

    p->color = Black;
    MoveGen *black = new_move_gen(p);
    generate_checks(black);
    cl_assert_equal_s(all_moves(black, buffer), "[]");
}

// Bishop: discovered non-capturing check: no enpassant captures.
void test_generate_checks_suite__100(void) {
    char buffer[256];
    setup("Ka1,Bf4,e5", "M,Kb8,f7");
    Position *p = start();
    p = make_move(p, new_enpassant(p, F7, F5));
    MoveGen *white = new_move_gen(p);
    generate_checks(white);
    cl_assert_equal_s(all_moves(white, buffer), "[e5-e6]");

    setup("Ka1,e2", "Kb8,Be5,d4");
    p = start();
    p = make_move(p, new_enpassant(p, E2, E4));
    MoveGen *black = new_move_gen(p);
    generate_checks(black);
    cl_assert_equal_s(all_moves(black, buffer), "[d4-d3]");
}

// Bishop: extra Rook moves for Queen.
void test_generate_checks_suite__110(void) {
    char buffer[256];
    setup("Kb1,Qa1,f2,a2", "Kh1,Qa7,Nb8,a6");
    Position *p = start();
    MoveGen *white = new_move_gen(p);
    generate_checks(white);
    cl_assert_equal_s(all_moves(white, buffer), "[Qa1-h8 Kb1-b2 Kb1-c2]");

    p->color = Black;
    MoveGen *black = new_move_gen(p);
    generate_checks(black);
    cl_assert_equal_s(all_moves(black, buffer), "[Qa7-b6 Qa7-h7 Qa7-b7]");
}

// Pawns.
void test_generate_checks_suite__120(void) {
    char buffer[256];
    setup("Kb5,f2,g2,h2", "Kg4,a7,b7,c7");
    Position *p = start();
    MoveGen *white = new_move_gen(p);
    generate_checks(white);
    cl_assert_equal_s(all_moves(white, buffer), "[f2-f3 h2-h3]");

    p->color = Black;
    MoveGen *black = new_move_gen(p);
    generate_checks(black);
    cl_assert_equal_s(all_moves(black, buffer), "[a7-a6 c7-c6]");
}

void test_generate_checks_suite__130(void) {
    char buffer[256];
    setup("Kb4,f2,g2,h2", "Kg5,a7,b7,c7");
    Position *p = start();
    MoveGen *white = new_move_gen(p);
    generate_checks(white);
    cl_assert_equal_s(all_moves(white, buffer), "[f2-f4 h2-h4]");

    p->color = Black;
    MoveGen *black = new_move_gen(p);
    generate_checks(black);
    cl_assert_equal_s(all_moves(black, buffer), "[a7-a5 c7-c5]");
}

void test_generate_checks_suite__140(void) {
    char buffer[256];
    setup("Kb4,c5,f2,g2,h2", "Kg5,a7,b7,c7,h4");
    Position *p = start();
    MoveGen *white = new_move_gen(p);
    generate_checks(white);
    cl_assert_equal_s(all_moves(white, buffer), "[f2-f4]");

    p->color = Black;
    MoveGen *black = new_move_gen(p);
    generate_checks(black);
    cl_assert_equal_s(all_moves(black, buffer), "[a7-a5]");
}

// Rook with pawn on the same rank (discovered check).
void test_generate_checks_suite__150(void) {
    char buffer[256];
    setup("Ka4,Ra5,e5", "Kh5,Rh4,c4");
    Position *p = start();
    MoveGen *white = new_move_gen(p);
    generate_checks(white);
    cl_assert_equal_s(all_moves(white, buffer), "[e5-e6]");

    p->color = Black;
    MoveGen *black = new_move_gen(p);
    generate_checks(black);
    cl_assert_equal_s(all_moves(black, buffer), "[c4-c3]");
}

// Rook with pawn on the same file (no check).
void test_generate_checks_suite__160(void) {
    char buffer[256];
    setup("Kh8,Ra8,a6", "Ka3,Rh1,h5");
    Position *p = start();
    MoveGen *white = new_move_gen(p);
    generate_checks(white);
    cl_assert_equal_s(all_moves(white, buffer), "[]");

    p->color = Black;
    MoveGen *black = new_move_gen(p);
    generate_checks(black);
    cl_assert_equal_s(all_moves(black, buffer), "[]");
}

// Rook with king on the same rank (discovered check).
void test_generate_checks_suite__170(void) {
    char buffer[256];
    setup("Ke5,Ra5,d4,e4,f4", "Kh5");
    Position *p = start();
    MoveGen *white = new_move_gen(p);
    generate_checks(white);
    cl_assert_equal_s(all_moves(white, buffer), "[Ke5-d6 Ke5-e6 Ke5-f6]");
}

// Rook with king on the same file (discovered check).
void test_generate_checks_suite__180(void) {
    char buffer[256];
    setup("Kb5,Rb8,c4,c5,c6", "Kb1");
    Position *p = start();
    MoveGen *white = new_move_gen(p);
    generate_checks(white);
    cl_assert_equal_s(all_moves(white, buffer), "[Kb5-a4 Kb5-a5 Kb5-a6]");
}
