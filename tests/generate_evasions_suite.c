// Copyright (c) 2014-2015 by Michael Dvorkin. All Rights Reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.

// Check evasions (king retreats).
void test_generate_evasions_suite__270(void) {
    char buffer[256];
    setup("Kh6,g7", "M,Kf8");
    MoveGen *black = new_move_gen(start());
    generate_evasions(black);
    cl_assert_equal_s(all_moves(black, buffer), "[Kf8-e7 Kf8-f7 Kf8-e8 Kf8-g8]");
}

// Check evasions (king retreats).
void test_generate_evasions_suite__280(void) {
    char buffer[256];
    setup("Ka1", "Kf8,Nc4,b2");
    MoveGen *white = new_move_gen(start());
    generate_evasions(white);
    cl_assert_equal_s(all_moves(white, buffer), "[Ka1-b1 Ka1-a2]");
}

// Check evasions (king retreats).
void test_generate_evasions_suite__290(void) {
    char buffer[256];
    setup("Ka1,h6,g7", "M,Kf8");
    MoveGen *black = new_move_gen(start());
    generate_evasions(black);
    cl_assert_equal_s(all_moves(black, buffer), "[Kf8-e7 Kf8-f7 Kf8-e8 Kf8-g8]");
}

// Check evasions (captures/blocks by major pieces).
void test_generate_evasions_suite__300(void) {
    char buffer[256];
    setup("Ka5,Ra8", "M,Kg8,Qf6,Re6,Bc6,Bg7,Na6,Nb6,f7,g6,h5");
    MoveGen *black = new_move_gen(start());
    generate_evasions(black);
    cl_assert_equal_s(all_moves(black, buffer), "[Kg8-h7 Na6-b8 Nb6xa8 Nb6-c8 Bc6xa8 Bc6-e8 Re6-e8 Qf6-d8 Bg7-f8]");
}

// Check evasions (double check).
void test_generate_evasions_suite__310(void) {
    char buffer[256];
    setup("Ka1,Ra8,Nf6", "M,Kg8,Qc6,Bb7,Nb6,f7,g6,h7");
    MoveGen *black = new_move_gen(start());
    generate_evasions(black);
    cl_assert_equal_s(all_moves(black, buffer), "[Kg8-g7]");
}

// Check evasions (double check).
void test_generate_evasions_suite__320(void) {
    char buffer[256];
    setup("Ka1,Ra8,Be5", "M,Kh8,Qd5,Bb7,Nb6");
    MoveGen *black = new_move_gen(start());
    generate_evasions(black);
    cl_assert_equal_s(all_moves(black, buffer), "[Kh8-h7]");
}

// Check evasions (pawn captures).
void test_generate_evasions_suite__330(void) {
    char buffer[256];
    setup("Kh6,Be5", "M,Kh8,d6");
    MoveGen *black = new_move_gen(start());
    generate_evasions(black);
    cl_assert_equal_s(all_moves(black, buffer), "[Kh8-g8 d6xe5]");
}

// Check evasions (pawn captures).
void test_generate_evasions_suite__340(void) {
    char buffer[256];
    setup("Ke1,e2,f2,g2", "Kc3,Nf3");
    MoveGen *white = new_move_gen(start());
    generate_evasions(white);
    cl_assert_equal_s(all_moves(white, buffer), "[Ke1-d1 Ke1-f1 e2xf3 g2xf3]");
}

// Check evasions (pawn blocks).
void test_generate_evasions_suite__350(void) {
    char buffer[256];
    setup("Kf8,Ba1", "M,Kh8,b3,c4,d5,e6,f7");
    MoveGen *black = new_move_gen(start());
    generate_evasions(black);
    cl_assert_equal_s(all_moves(black, buffer), "[Kh8-h7 b3-b2 c4-c3 d5-d4 e6-e5 f7-f6]");
}

// Check evasions (pawn blocks).
void test_generate_evasions_suite__360(void) {
    char buffer[256];
    setup("Ka5,a4,b4,c4,d4,e4,f4,h4", "Kh8,Qh5");
    MoveGen *white = new_move_gen(start());
    generate_evasions(white);
    cl_assert_equal_s(all_moves(white, buffer), "[Ka5-a6 Ka5-b6 b4-b5 c4-c5 d4-d5 e4-e5 f4-f5]");
}

// Check evasions (pawn jumps).
void test_generate_evasions_suite__365(void) {
    char buffer[256];
    setup("Ke1,Rh4", "M,Kh6,h7");
    MoveGen *white = new_move_gen(start());
    generate_evasions(white);
    cl_assert_equal_s(all_moves(white, buffer), "[Kh6-g5 Kh6-g6 Kh6-g7]"); // No h7-h5 jump.
}

// Check evasions (en-passant pawn capture).
void test_generate_evasions_suite__370(void) {
    char buffer[256];
    setup("Kd4,d5,f5", "M,Kd8,e7");
    Position *black = start();
    black = make_move(black, new_enpassant(black, E7, E5));
    MoveGen *white = new_move_gen(black);
    generate_evasions(white);
    cl_assert_equal_s(all_moves(white, buffer), "[Kd4-c3 Kd4-d3 Kd4-e3 Kd4-c4 Kd4-e4 Kd4-c5 Kd4xe5 d5xe6 f5xe6]");
}

// Check evasions (en-passant pawn capture).
void test_generate_evasions_suite__380(void) {
    char buffer[256];
    setup("Kb1,b2", "Ka5,a4,c5,c4");
    Position *white = start();
    white = make_move(white, new_enpassant(white, B2, B4));
    MoveGen *black = new_move_gen(white);
    generate_evasions(black);
    cl_assert_equal_s(all_moves(black, buffer), "[Ka5xb4 Ka5-b5 Ka5-a6 Ka5-b6 c5xb4 a4xb3 c4xb3]");
}

// Check evasions (en-passant pawn capture).
void test_generate_evasions_suite__385(void) {
    setup("Ke4,c5,e5", "M,Ke7,d7");
    Position *black = start();
    black = make_move(black, new_enpassant(black, D7, D5));
    MoveGen *white = new_move_gen(black);
    generate_evasions(white);
    for (Move move = next_move(white); move; move = next_move(white)) {
        if (is_pawn(piece(move))) {
            cl_check(to(move) == D6);
            cl_check(color(piece(move)) == White);
            cl_check(capture(move) == (Piece)BlackPawn);
            cl_check(promo(move) == (Piece)0);
        }
    }
}

// Check evasions (en-passant pawn capture).
void test_generate_evasions_suite__386(void) {
    char buffer[256];
    setup("Kf1,Qa2", "M,Kf7,b2");
    MoveGen *black = new_move_gen(start());
    generate_evasions(black);
    cl_check(!strstr(all_moves(black, buffer), "b2-a1"));
}

// Check evasions (pawn jumps).
void test_generate_evasions_suite__390(void) {
    char buffer[256];
    setup("Kh4,a2,b2,c2,d2,e2,f2,g2", "Kd8,Ra4");
    MoveGen *white = new_move_gen(start());
    generate_evasions(white);
    cl_assert_equal_s(all_moves(white, buffer), "[Kh4-g3 Kh4-h3 Kh4-g5 Kh4-h5 b2-b4 c2-c4 d2-d4 e2-e4 f2-f4 g2-g4]");
}

// Check evasions (pawn jumps).
void test_generate_evasions_suite__400(void) {
    char buffer[256];
    setup("Kd8,Rh5", "M,Ka5,b7,c7,d7,e7,f7,g7,h7");
    MoveGen *black = new_move_gen(start());
    generate_evasions(black);
    cl_assert_equal_s(all_moves(black, buffer), "[Ka5-a4 Ka5-b4 Ka5-a6 Ka5-b6 b7-b5 c7-c5 d7-d5 e7-e5 f7-f5 g7-g5]");
}

// Check evasions (pawn jump, sets en-passant).
void test_generate_evasions_suite__405(void) {
    char buffer[256];
    setup("Ke1,Qd4,d5", "M,Kg7,e7,g6,h7");
    Position *p = start();
    MoveGen *black = new_move_gen(p);
    generate_evasions(black);
    Move e7e5 = black->list[black->head + 4].move;
    cl_assert_equal_s(all_moves(black, buffer), "[Kg7-h6 Kg7-f7 Kg7-f8 Kg7-g8 e7-e5]");
    p = make_move(p, e7e5);
    cl_check(p->enpassant == (uint8)E6);
}

// Check evasions (piece x piece captures).
void test_generate_evasions_suite__410(void) {
    char buffer[256];
    setup("Ke1,Qd1,Rc7,Bd3,Nd4,c2,f2", "M,Kh8,Nb4");
    Position *black = start();
    black = make_move(black, new_move(black, B4, C2));
    MoveGen *white = new_move_gen(black);
    generate_evasions(white);
    cl_assert_equal_s(all_moves(white, buffer), "[Ke1-f1 Ke1-d2 Ke1-e2 Qd1xc2 Bd3xc2 Nd4xc2 Rc7xc2]");
}

// Check evasions (pawn x piece captures).
void test_generate_evasions_suite__420(void) {
    char buffer[256];
    setup("Ke1,Qd1,Rd7,Bf1,Nc1,c2,d3,f2", "M,Kh8,Nb4");
    Position *black = start();
    black = make_move(black, new_move(black, B4, D3));
    MoveGen *white = new_move_gen(black);
    generate_evasions(white);
    cl_assert_equal_s(all_moves(white, buffer), "[Ke1-d2 Ke1-e2 c2xd3 Nc1xd3 Qd1xd3 Bf1xd3 Rd7xd3]");
}

// Check evasions (king x piece captures).
void test_generate_evasions_suite__430(void) {
    char buffer[256];
    setup("Ke1,Qf7,f2", "M,Kh8,Qh4");
    Position *black = start();
    black = make_move(black, new_move(black, H4, F2));
    MoveGen *white = new_move_gen(black);
    generate_evasions(white);
    cl_assert_equal_s(all_moves(white, buffer), "[Ke1-d1 Ke1xf2 Qf7xf2]");
}

// Pawn promotion to block.
void test_generate_evasions_suite__440(void) {
    char buffer[256];
    setup("Kf1,Qf3,Nf2", "Ka1,b2");
    Position *white = start();
    white = make_move(white, new_move(white, F3, D1));
    MoveGen *black = new_move_gen(white);
    generate_evasions(black);
    cl_assert_equal_s(all_moves(black, buffer), "[Ka1-a2 b2-b1Q]");
}

// Pawn promotion to block or capture.
void test_generate_evasions_suite__450(void) {
    char buffer[256];
    setup("Kf1,Qf3,Nf2", "Ka1,b2,c2");
    Position *white = start();
    white = make_move(white, new_move(white, F3, D1));
    MoveGen *black = new_move_gen(white);
    generate_evasions(black);
    cl_assert_equal_s(all_moves(black, buffer), "[Ka1-a2 c2xd1Q b2-b1Q c2-c1Q]");
}

// Pawn promotion to capture.
void test_generate_evasions_suite__460(void) {
    char buffer[256];
    setup("Kf1,Qf3,Nf2", "Kc1,c2,d2");
    Position *white = start();
    white = make_move(white, new_move(white, F3, D1));
    MoveGen *black = new_move_gen(white);
    generate_evasions(black);
    cl_assert_equal_s(all_moves(black, buffer), "[Kc1-b2 c2xd1Q]");
}
