// Copyright (c) 2014-2015 by Michael Dvorkin. All Rights Reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.

// Piece captures.
void test_generate_captures_suite__000(void) {
    char buffer[256];
    setup("Ka1,Qd1,Rh1,Bb3,Ne5", "Ka8,Qd8,Rh8,Be6,Ng6");
    Position *p = start();
    MoveGen *white = new_move_gen(p);
    generate_captures(white);
    cl_assert_equal_s(all_moves(white, buffer), "[Qd1xd8 Rh1xh8 Bb3xe6 Ne5xg6]");

    p->color = Black;
    MoveGen *black = new_move_gen(p);
    generate_captures(black);
    cl_assert_equal_s(all_moves(black, buffer), "[Be6xb3 Ng6xe5 Qd8xd1 Rh8xh1]");
}

// Pawn captures on rows 2-6 (no promotions).
void test_generate_captures_suite__010(void) {
    char buffer[256];
    setup("Ka1,b3,c4,d5", "Ka8,a4,b5,c6,e6");
    Position *p = start();
    MoveGen *white = new_move_gen(p);
    generate_captures(white);
    cl_assert_equal_s(all_moves(white, buffer), "[b3xa4 c4xb5 d5xc6 d5xe6]");

    p->color = Black;
    MoveGen *black = new_move_gen(p);
    generate_captures(black);
    cl_assert_equal_s(all_moves(black, buffer), "[a4xb3 b5xc4 c6xd5 e6xd5]");
}

// Pawn captures with promotion, rows 1-7.
void test_generate_captures_suite__020(void) {
    char buffer[256];
    setup("Ka1,Bh1,Ng1,a2,b7,e7", "Kb8,Rd8,Be8,Rf8,h2");
    Position *p = start();
    MoveGen *white = new_move_gen(p);
    generate_captures(white);
    cl_assert_equal_s(all_moves(white, buffer), "[e7xd8Q e7xd8R e7xd8B e7xd8N e7xf8Q e7xf8R e7xf8B e7xf8N]");

    p->color = Black;
    MoveGen *black = new_move_gen(p);
    generate_captures(black);
    cl_assert_equal_s(all_moves(black, buffer), "[h2xg1Q h2xg1R h2xg1B h2xg1N Kb8xb7]");
}

// Pawn promotions without capture, rows 1-7.
void test_generate_captures_suite__030(void) {
    char buffer[256];
    setup("Ka1,a2,e7", "Ka8,Rd8,a7,h2");
    Position *p = start();
    MoveGen *white = new_move_gen(p);
    generate_captures(white);
    cl_assert_equal_s(all_moves(white, buffer), "[e7xd8Q e7xd8R e7xd8B e7xd8N e7-e8Q e7-e8R e7-e8B e7-e8N]");

    p->color = Black;
    MoveGen *black = new_move_gen(p);
    generate_captures(black);
    cl_assert_equal_s(all_moves(black, buffer), "[h2-h1Q h2-h1R h2-h1B h2-h1N]");
}

// Captures/promotions sort order.
void test_generate_captures_suite__100(void) {
    char buffer[256];
    setup("Kg1,Qh4,Rg2,Rf1,Nd6,f7", "Kh8,Qc3,Rd8,Rd7,Ne6,a3,h7");
    MoveGen *gen = new_move_gen(start());
    generate_captures(gen);
    cl_assert_equal_s(all_moves(gen, buffer), "[f7-f8Q f7-f8R f7-f8B f7-f8N Qh4xh7 Qh4xd8]");
}

void test_generate_captures_suite__110(void) {
    char buffer[256];
    setup("Kg1,Qh4,Rg2,Rf1,Nd6,f7", "Kh8,Qc3,Rd8,Rd7,Ne6,a3,h7");
    MoveGen *gen = new_move_gen(start());
    generate_captures(gen);
    full_rank(gen, (Move)0);
    cl_assert_equal_s(all_moves(gen, buffer), "[f7-f8Q Qh4xd8 f7-f8R f7-f8B f7-f8N Qh4xh7]");
}

void test_generate_captures_suite__120(void) {
    char buffer[256];
    setup("Kg1,Qh4,Rg2,Rf1,Nd6,f7", "Kh8,Qc3,Rd8,Rd7,Ne6,a3,h7");
    MoveGen *gen = new_move_gen(start());
    generate_captures(gen);
    quick_rank(gen);
    cl_assert_equal_s(all_moves(gen, buffer), "[f7-f8Q Qh4xd8 f7-f8R f7-f8B f7-f8N Qh4xh7]");
}
