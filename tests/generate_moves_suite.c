// Copyright (c) 2014-2015 by Michael Dvorkin. All Rights Reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.

void test_generate_moves_suite__000(void) {
    char buffer[256];
    new_game();
    MoveGen *gen = new_move_gen(start());
    generate_moves(gen);

    cl_assert_equal_s(all_moves(gen, buffer), "[a2-a3 a2-a4 b2-b3 b2-b4 c2-c3 c2-c4 d2-d3 d2-d4 e2-e3 e2-e4 f2-f3 f2-f4 g2-g3 g2-g4 h2-h3 h2-h4 Nb1-a3 Nb1-c3 Ng1-f3 Ng1-h3]");
}

void test_generate_moves_suite__020(void) {
    char buffer[256];
    setup("a2,b3,c4,d2,e6,f5,g4,h3", "a3,b4,c5,e7,f6,g5,h4,Kg8");
    MoveGen *gen = new_move_gen(start());
    generate_moves(gen);

    // All possible moves, left-to right, unsorted.
    cl_assert_equal_s(all_moves(gen, buffer), "[d2-d3 d2-d4]");
}

void test_generate_moves_suite__030(void) {
    char buffer[256];
    setup("a2,e4,g2", "b3,f5,f3,h3,Kg8");
    MoveGen *gen = new_move_gen(start());
    generate_moves(gen);

    // All possible moves, left-to right, unsorted.
    cl_assert_equal_s(all_moves(gen, buffer), "[a2-a3 a2xb3 a2-a4 g2xf3 g2-g3 g2xh3 g2-g4 e4-e5 e4xf5]");
}

// Castles.
void test_generate_moves_suite__031(void) {
    char buffer[256];
    setup("Ke1,Rh1,h2", "Ke8,Ra8,a7");
    Position *p = start();
    MoveGen *white = new_move_gen(p);
    generate_moves(white);
    cl_check(strstr(all_moves(white, buffer), "0-0"));

    p->color = Black;
    MoveGen *black = new_move_gen(p);
    generate_moves(black);
    cl_check(strstr(all_moves(black, buffer), "0-0-0"));
}

// Should not include castles when rook has moved.
void test_generate_moves_suite__040(void) {
    char buffer[256];
    setup("Ke1,Rf1,g2", "Ke8");
    MoveGen *white = new_move_gen(start());
    generate_moves(white);
    cl_check(!strstr(all_moves(white, buffer), "0-0"));
}

void test_generate_moves_suite__050(void) {
    char buffer[256];
    setup("Ke1,Rb1,b2", "Ke8");
    MoveGen *white = new_move_gen(start());
    generate_moves(white);
    cl_check(!strstr(all_moves(white, buffer), "0-0"));
}

// Should not include castles when king has moved.
void test_generate_moves_suite__060(void) {
    char buffer[256];
    setup("Kf1,Ra1,a2,Rh1,h2", "Ke8");
    MoveGen *white = new_move_gen(start());
    generate_moves(white);
    cl_check(!strstr(all_moves(white, buffer), "0-0"));
}

// Should not include castles when rooks are not there.
void test_generate_moves_suite__070(void) {
    char buffer[256];
    setup("Ke1", "Ke8");
    MoveGen *white = new_move_gen(start());
    generate_moves(white);
    cl_check(!strstr(all_moves(white, buffer), "0-0"));
}

// Should not include castles when king is in check.
void test_generate_moves_suite__080(void) {
    char buffer[256];
    setup("Ke1,Ra1,Rf1", "Ke8,Re7");
    MoveGen *white = new_move_gen(start());
    generate_moves(white);
    cl_check(!strstr(all_moves(white, buffer), "0-0"));
}

// Should not include castles when target square is a capture.
void test_generate_moves_suite__090(void) {
    char buffer[256];
    setup("Ke1,Ra1,Rf1", "Ke8,Nc1,Ng1");
    MoveGen *white = new_move_gen(start());
    generate_moves(white);
    cl_check(!strstr(all_moves(white, buffer), "0-0"));
}

// Should not include castles when king is to jump over attacked square.
void test_generate_moves_suite__100(void) {
    char buffer[256];
    setup("Ke1,Ra1,Rf1", "Ke8,Bc4,Bf4");
    MoveGen *white = new_move_gen(start());
    generate_moves(white);
    cl_check(!strstr(all_moves(white, buffer), "0-0"));
}

// Pawn moves that include promotions.
void test_generate_moves_suite__200(void) {
    char buffer[256];
    setup("Ka1,a6,b7", "Kh8,g3,h2");
    Position *p = start();
    MoveGen *white = new_move_gen(p);
    pawn_moves(white, White);
    cl_assert_equal_s(all_moves(white, buffer), "[a6-a7 b7-b8Q b7-b8R b7-b8B b7-b8N]");

    p->color = Black;
    MoveGen *black = new_move_gen(p);
    pawn_moves(black, Black);
    cl_assert_equal_s(all_moves(black, buffer), "[h2-h1Q h2-h1R h2-h1B h2-h1N g3-g2]");
}

// Pawn moves that include jumps.
void test_generate_moves_suite__210(void) {
    char buffer[256];
    setup("Ka1,a6", "Kh8,a7,g7,h6");
    Position *p = start();
    MoveGen *white = new_move_gen(p);
    pawn_moves(white, White);
    cl_assert_equal_s(all_moves(white, buffer), "[]");

    p->color = Black;
    MoveGen *black = new_move_gen(p);
    pawn_moves(black, Black);
    cl_assert_equal_s(all_moves(black, buffer), "[h6-h5 g7-g5 g7-g6]");
}

// Pawn captures without promotions.
void test_generate_moves_suite__220(void) {
    char buffer[256];
    setup("Ka1,a6,f6,g5", "Kh8,b7,g7,h6");
    Position *p = start();
    MoveGen *white = new_move_gen(p);
    pawn_captures(white, White);
    cl_assert_equal_s(all_moves(white, buffer), "[g5xh6 a6xb7 f6xg7]");

    p->color = Black;
    MoveGen *black = new_move_gen(p);
    pawn_captures(black, Black);
    cl_assert_equal_s(all_moves(black, buffer), "[h6xg5 b7xa6 g7xf6]");
}

// Pawn captures with Queen promotion.
void test_generate_moves_suite__230(void) {
    char buffer[256];
    setup("Ka1,Rh1,Bf1,c7", "Kh8,Nb8,Qd8,g2");
    Position *p = start();
    MoveGen *white = new_move_gen(p);
    pawn_captures(white, White);
    cl_assert_equal_s(all_moves(white, buffer), "[c7xb8Q c7xb8R c7xb8B c7xb8N c7-c8Q c7-c8R c7-c8B c7-c8N c7xd8Q c7xd8R c7xd8B c7xd8N]");

    p->color = Black;
    MoveGen *black = new_move_gen(p);
    pawn_captures(black, Black);
    cl_assert_equal_s(all_moves(black, buffer), "[g2xf1Q g2xf1R g2xf1B g2xf1N g2-g1Q g2-g1R g2-g1B g2-g1N g2xh1Q g2xh1R g2xh1B g2xh1N]");
}
