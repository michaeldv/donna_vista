// Copyright (c) 2014-2015 by Michael Dvorkin. All Rights Reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.

// PxQ, NxQ, BxQ, RxQ, QxQ, KxQ
void test_move_suite__000(void) {
    setup("Kd6,Qd1,Ra5,Nc3,Bc4,e4", "Kh8,Qd5");
    Position *p = start();
    cl_check(value(new_move(p, E4, D5)) == 1258); // PxQ
    cl_check(value(new_move(p, C3, D5)) == 1256); // NxQ
    cl_check(value(new_move(p, C4, D5)) == 1254); // BxQ
    cl_check(value(new_move(p, A5, D5)) == 1252); // RxQ
    cl_check(value(new_move(p, D1, D5)) == 1250); // QxQ
    cl_check(value(new_move(p, D6, D5)) == 1248); // KxQ
}

// PxR, NxR, BxR, RxR, QxR, KxR
void test_move_suite__010(void) {
    setup("Kd6,Qd1,Ra5,Nc3,Bc4,e4", "Kh8,Rd5");
    Position *p = start();
    cl_check(value(new_move(p, E4, D5)) == 633); // PxR
    cl_check(value(new_move(p, C3, D5)) == 631); // NxR
    cl_check(value(new_move(p, C4, D5)) == 629); // BxR
    cl_check(value(new_move(p, A5, D5)) == 627); // RxR
    cl_check(value(new_move(p, D1, D5)) == 625); // QxR
    cl_check(value(new_move(p, D6, D5)) == 623); // KxR
}

// PxB, NxB, BxB, RxB, QxB, KxB
void test_move_suite__020(void) {
    setup("Kd6,Qd1,Ra5,Nc3,Bc4,e4", "Kh8,Bd5");
    Position *p = start();
    cl_check(value(new_move(p, E4, D5)) == 416); // PxB
    cl_check(value(new_move(p, C3, D5)) == 414); // NxB
    cl_check(value(new_move(p, C4, D5)) == 412); // BxB
    cl_check(value(new_move(p, A5, D5)) == 410); // RxB
    cl_check(value(new_move(p, D1, D5)) == 408); // QxB
    cl_check(value(new_move(p, D6, D5)) == 406); // KxB
}

// PxN, NxN, BxN, RxN, QxN, KxN
void test_move_suite__030(void) {
    setup("Kd6,Qd1,Ra5,Nc3,Bc4,e4", "Kh8,Nd5");
    Position *p = start();
    cl_check(value(new_move(p, E4, D5)) == 406); // PxN
    cl_check(value(new_move(p, C3, D5)) == 404); // NxN
    cl_check(value(new_move(p, C4, D5)) == 402); // BxN
    cl_check(value(new_move(p, A5, D5)) == 400); // RxN
    cl_check(value(new_move(p, D1, D5)) == 398); // QxN
    cl_check(value(new_move(p, D6, D5)) == 396); // KxN
}

// PxP, NxP, BxP, RxP, QxP, KxP
void test_move_suite__040(void) {
    setup("Kd6,Qd1,Ra5,Nc3,Bc4,e4", "Kh8,d5");
    Position *p = start();
    cl_check(value(new_move(p, E4, D5)) == 98); // PxP
    cl_check(value(new_move(p, C3, D5)) == 96); // NxP
    cl_check(value(new_move(p, C4, D5)) == 94); // BxP
    cl_check(value(new_move(p, A5, D5)) == 92); // RxP
    cl_check(value(new_move(p, D1, D5)) == 90); // QxP
    cl_check(value(new_move(p, D6, D5)) == 88); // KxP
}

// new_move_from_string: move from algebraic notation.
void test_move_suite__100(void) {
    new_game();
    Position *p = start();
    Move m1 = new_move(p, E2, E4);
    Move m2 = new_move(p, G1, F3);
    Move move[5] = {
        new_move_from_string(p, "e2e4"),
        new_move_from_string(p, "e2-e4"),
        new_move_from_string(p, "Ng1f3"),
        new_move_from_string(p, "Ng1-f3"),
        new_move_from_string(p, "Rg1-f3")
    };

    cl_check(move[0] == m1);
    cl_check(move[1] == m1);
    cl_check(move[2] == m2);
    cl_check(move[3] == m2);
    cl_check(move[4] == (Move)0);
}

void test_move_suite__110(void) {
    setup("Ke1,g7,a7", "Ke8,Rh8,e2");
    Position *p = start();
    Move m1 = new_move(p, E1, E2); // Capture.
    Move m2 = promote(new_move(p, A7, A8), Rook);  // Promo without capture.
    Move m3 = promote(new_move(p, G7, H8), Queen); // Promo with capture.

    Move move[7] = {
        new_move_from_string(p, "Ke1e2"),
        new_move_from_string(p, "Ke1xe2"),
        new_move_from_string(p, "a7a8R"),
        new_move_from_string(p, "a7-a8R"),
        new_move_from_string(p, "g7h8Q"),
        new_move_from_string(p, "g7xh8Q"),
        new_move_from_string(p, "Bh1h8")
    };

    cl_check(move[0] == m1);
    cl_check(move[1] == m1);
    cl_check(move[2] == m2);
    cl_check(move[3] == m2);
    cl_check(move[4] == m3);
    cl_check(move[5] == m3);
    cl_check(move[6] == (Move)0);
}

void test_move_suite__120(void) {
    setup("Ke1", "M,Ke8,Ra8");
    Position *p = start();
    Move move = new_move_from_string(p, "0-0-0");
    cl_check(move == new_castle(p, E8, C8));

    setup("Ke1", "M,Ke8,Rh8");
    p = start();
    move = new_move_from_string(p, "0-0");
    cl_check(move == new_castle(p, E8, G8));
}

void test_move_suite__130(void) {
    new_game();
    Position *p = start();
    p = make_move(p, new_pawn_move(p, E2, E4));
    p = make_move(p, new_pawn_move(p, E7, E6));
    p = make_move(p, new_pawn_move(p, E4, E5));
    Move move = new_pawn_move(p, D7, D5); // Causes en-passant on D6.

    Move m1 = new_move_from_string(p, "d7d5");
    Move m2 = new_move_from_string(p, "d7-d5");
    cl_check(m1 == move);
    cl_check(is_enpassant(m2) == true);
}

// Move to UCI coordinate notation.
void test_move_suite__200(void) {
    char buffer[8];
    new_game();
    Position *p = start();
    Move m1 = new_move(p, E2, E4);
    Move m2 = new_move(p, G1, F3);

    cl_assert_equal_s(notation(m1, buffer), "e2e4"); // Pawn.
    cl_assert_equal_s(notation(m2, buffer), "g1f3"); // Knight.
}

void test_move_suite__210(void) {
    char buffer[8];
    setup("Ke1,g7,a7", "Ke8,Rh8,e2");
    Position *p = start();
    Move m1 = new_move(p, E1, E2); // Capture.
    Move m2 = promote(new_move(p, A7, A8), Rook);  // Promo without capture.
    Move m3 = promote(new_move(p, G7, H8), Queen); // Promo with capture.

    cl_assert_equal_s(notation(m1, buffer), "e1e2");
    cl_assert_equal_s(notation(m2, buffer), "a7a8r");
    cl_assert_equal_s(notation(m3, buffer), "g7h8q");
}

void test_move_suite__220(void) {
    char buffer[8];
    setup("Ke1", "M,Ke8,Ra8");
    Position *p = start();
    Move m = new_castle(p, E8, C8); // 0-0-0
    cl_assert_equal_s(notation(m, buffer), "e8c8");

    setup("Ke1", "M,Ke8,Rh8");
    p = start();
    m = new_castle(p, E8, G8); // 0-0
    cl_assert_equal_s(notation(m, buffer), "e8g8");
}

// Move from UCI coordinate notation.
void test_move_suite__300(void) {
    new_game();
    Position *p = start();
    Move m1 = new_move(p, E2, E4);
    Move m2 = new_move(p, G1, F3);

    cl_check(new_move_from_notation(p, "e2e4") == m1); // Pawn.
    cl_check(new_move_from_notation(p, "g1f3") == m2); // Knight.
}

void test_move_suite__310(void) {
    setup("Ke1,g7,a7", "Ke8,Rh8,e2");
    Position *p = start();
    Move m1 = new_move(p, E1, E2); // Capture.
    Move m2 = promote(new_move(p, A7, A8), Rook);  // Promo without capture.
    Move m3 = promote(new_move(p, G7, H8), Queen); // Promo with capture.

    cl_check(new_move_from_notation(p, "e1e2") == m1);
    cl_check(new_move_from_notation(p, "a7a8r") == m2);
    cl_check(new_move_from_notation(p, "g7h8q") == m3);
}

void test_move_suite__320(void) {
    setup("Ke1", "M,Ke8,Ra8");
    Position *p = start();
    Move m = new_castle(p, E8, C8); // 0-0-0
    cl_check(new_move_from_notation(p, "e8c8") == m);

    setup("Ke1", "M,Ke8,Rh8");
    p = start();
    m = new_castle(p, E8, G8); // 0-0
    cl_check(new_move_from_notation(p, "e8g8") == m);
}

void test_move_suite__330(void) {
    new_game();
    Position *p = start();
    p = make_move(p, new_pawn_move(p, E2, E4));
    p = make_move(p, new_pawn_move(p, E7, E6));
    p = make_move(p, new_pawn_move(p, E4, E5));
    Move m = new_pawn_move(p, D7, D5); // Causes en-passant on D6.

    cl_check(new_move_from_notation(p, "d7d5") == m);
    cl_check(is_enpassant(new_move_from_notation(p, "d7d5")));
}

// Only pawns can do en-passant capture.
void test_move_suite__340(void) {
    setup("Kg1,d2", "Kc2,Qa3,Rh3,Be4,Nc1,c4");
    Position *p = start();
    p = make_move(p, new_enpassant(p, D2, D4)); // Causes en-passant on D3.
    Move bQ = new_move(p, A3, D3);
    Move bR = new_move(p, H3, D3);
    Move bB = new_move(p, E4, D3);
    Move bN = new_move(p, C1, D3);
    Move bK = new_move(p, C2, D3);
    Move bP = new_move(p, C4, D3);

    cl_check(capture(bQ) == (Piece)0);
    cl_check(capture(bR) == (Piece)0);
    cl_check(capture(bB) == (Piece)0);
    cl_check(capture(bN) == (Piece)0);
    cl_check(capture(bK) == (Piece)0);
    cl_check(capture(bP) == (Piece)Pawn);

    cl_check(is_capture(bQ) == false);
    cl_check(is_capture(bR) == false);
    cl_check(is_capture(bB) == false);
    cl_check(is_capture(bN) == false);
    cl_check(is_capture(bK) == false);
    cl_check(is_capture(bP) == true);
}
