// Copyright (c) 2014-2015 by Michael Dvorkin. All Rights Reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.

void test_position_moves_suite__010(void) {
    setup("Ke1,e2", "Kg8,d7,f7");
    Position *p = start();
    cl_check(p->enpassant == (uint8)0);

    p = make_move(p, new_move(p, E2, E4));
    cl_check(p->enpassant == (uint8)0);

    p = make_move(p, new_move(p, D7, D5));
    cl_check(p->enpassant == (uint8)0);

    p = make_move(p, new_move(p, E4, E5));
    cl_check(p->enpassant == (uint8)0);

    p = make_move(p, new_enpassant(p, F7, F5));
    cl_check(p->enpassant == (uint8)F6);
}

// Castle tests.
void test_position_moves_suite__030(void) { // Everything is OK.
    setup("Ke1,Ra1,Rh1", "Ke8");
    Position *p = start();
    bool kingside, queenside;

    can_castle(p, p->color, &kingside, &queenside);
    cl_check(kingside);
    cl_check(queenside);

    setup("Ke1", "M,Ke8,Ra8,Rh8");
    p = start();
    can_castle(p, p->color, &kingside, &queenside);
    cl_check(kingside);
    cl_check(queenside);
}

void test_position_moves_suite__040(void) { // King checked.
    setup("Ke1,Ra1,Rh1", "Ke8,Bg3");
    Position *p = start();
    bool kingside, queenside;

    can_castle(p, p->color, &kingside, &queenside);
    cl_check(kingside == false);
    cl_check(queenside == false);

    setup("Ke1,Bg6", "M,Ke8,Ra8,Rh8");
    p = start();
    can_castle(p, p->color, &kingside, &queenside);
    cl_check(kingside == false);
    cl_check(queenside == false);
}

void test_position_moves_suite__050(void) { // Attacked square.
    setup("Ke1,Ra1,Rh1", "Ke8,Bb3,Bh3");
    Position *p = start();
    bool kingside, queenside;

    can_castle(p, p->color, &kingside, &queenside);
    cl_check(kingside == false);
    cl_check(queenside == false);

    setup("Ke1,Bb6,Bh6", "M,Ke8,Ra8,Rh8");
    p = start();
    can_castle(p, p->color, &kingside, &queenside);
    cl_check(kingside == false);
    cl_check(queenside == false);
}

void test_position_moves_suite__060(void) { // Wrong square.
    setup("Ke1,Ra8,Rh8", "Ke5");
    Position *p = start();
    bool kingside, queenside;

    can_castle(p, p->color, &kingside, &queenside);
    cl_check(kingside == false);
    cl_check(queenside == false);

    setup("Ke2,Ra1,Rh1", "Ke8");
    p = start();
    can_castle(p, p->color, &kingside, &queenside);
    cl_check(kingside == false);
    cl_check(queenside == false);

    setup("Ke4", "M,Ke8,Ra1,Rh1");
    p = start();
    can_castle(p, p->color, &kingside, &queenside);
    cl_check(kingside == false);
    cl_check(queenside == false);

    setup("Ke4", "M,Ke7,Ra8,Rh8");
    p = start();
    can_castle(p, p->color, &kingside, &queenside);
    cl_check(kingside == false);
    cl_check(queenside == false);
}

void test_position_moves_suite__070(void) { // Missing rooks.
    setup("Ke1", "Ke8");
    Position *p = start();
    bool kingside, queenside;

    can_castle(p, p->color, &kingside, &queenside);
    cl_check(kingside == false);
    cl_check(queenside == false);

    setup("Ke1", "M,Ke8");
    p = start();
    can_castle(p, p->color, &kingside, &queenside);
    cl_check(kingside == false);
    cl_check(queenside == false);
}

void test_position_moves_suite__080(void) { // Rooks on wrong squares.
    setup("Ke1,Rb1", "Ke8");
    Position *p = start();
    bool kingside, queenside;

    can_castle(p, p->color, &kingside, &queenside);
    cl_check(kingside == false);
    cl_check(queenside == false);

    setup("Ke1,Rb1,Rh1", "Ke8");
    p = start();
    can_castle(p, p->color, &kingside, &queenside);
    cl_check(kingside == true);
    cl_check(queenside == false);

    setup("Ke1,Ra1,Rf1", "Ke8");
    p = start();
    can_castle(p, p->color, &kingside, &queenside);
    cl_check(kingside == false);
    cl_check(queenside == true);
}

void test_position_moves_suite__081(void) { // Rook has moved.
    setup("Ke1,Ra1,Rh1", "Ke8");
    Position *p = start();
    p = make_move(p, new_move(p, A1, A2));
    p = make_move(p, new_move(p, E8, E7));
    p = make_move(p, new_move(p, A2, A1));

    bool kingside, queenside;
    can_castle(p, White, &kingside, &queenside);
    cl_check(kingside == true);
    cl_check(queenside == false);
}

void test_position_moves_suite__082(void) { // King has moved.
    setup("Ke1", "M,Ke8,Ra8,Rh8");
    Position *p = start();
    p = make_move(p, new_move(p, E8, E7));
    p = make_move(p, new_move(p, E1, E2));
    p = make_move(p, new_move(p, E7, E8));

    bool kingside, queenside;
    can_castle(p, Black, &kingside, &queenside);
    cl_check(kingside == false);
    cl_check(queenside == false);
}

void test_position_moves_suite__083(void) { // Rook is taken.
    setup("Ke1,Nb6", "Ke8,Ra8,Rh8");
    Position *p = start();
    p = make_move(p, new_move(p, B6, A8));

    bool kingside, queenside;
    can_castle(p, Black, &kingside, &queenside);
    cl_check(kingside == true);
    cl_check(queenside == false);
}

// Blocking kingside knight.
void test_position_moves_suite__084(void) {
    setup("Ke1", "M,Ke8,Ra8,Rh8,Ng8");
    Position *p = start();

    bool kingside, queenside;
    can_castle(p, Black, &kingside, &queenside);
    cl_check(kingside == false);
    cl_check(queenside == true);
}

// Blocking queenside knight.
void test_position_moves_suite__085(void) {
    setup("Ke1", "M,Ke8,Ra8,Rh8,Nb8");
    Position *p = start();

    bool kingside, queenside;
    can_castle(p, Black, &kingside, &queenside);
    cl_check(kingside == true);
    cl_check(queenside == false);
}

// Straight repetition.
void test_position_moves_suite__100(void) {
    new_game();
    Position *p = start();                 // Initial 1.
    p = make_move(p, new_move(p, G1, F3));
    p = make_move(p, new_move(p, G8, F6)); // 1.
    cl_check(repetition(p) == false);
    cl_check(third_repetition(p) == false);

    p = make_move(p, new_move(p, F3, G1));
    p = make_move(p, new_move(p, F6, G8)); // Initial 2.
    cl_check(repetition(p));
    cl_check(third_repetition(p) == false);

    p = make_move(p, new_move(p, G1, F3));
    p = make_move(p, new_move(p, G8, F6)); // 2.
    cl_check(repetition(p));
    cl_check(third_repetition(p) == false);

    p = make_move(p, new_move(p, F3, G1));
    p = make_move(p, new_move(p, F6, G8)); // Initial 3.
    cl_check(repetition(p));
    cl_check(third_repetition(p));

    p = make_move(p, new_move(p, G1, F3));
    p = make_move(p, new_move(p, G8, F6)); // 3.
    cl_check(repetition(p));
    cl_check(third_repetition(p));
}

// Repetition with some moves in between.
void test_position_moves_suite__110(void) {
    new_game();
    Position *p = start();
    p = make_move(p, new_move(p, E2, E4));
    p = make_move(p, new_move(p, E7, E5));
    p = make_move(p, new_move(p, G1, F3));
    p = make_move(p, new_move(p, G8, F6)); // 1.
    p = make_move(p, new_move(p, B1, C3));
    p = make_move(p, new_move(p, B8, C6));
    p = make_move(p, new_move(p, F1, C4));
    p = make_move(p, new_move(p, F8, C5));
    p = make_move(p, new_move(p, C3, B1));
    p = make_move(p, new_move(p, C6, B8));
    p = make_move(p, new_move(p, C4, F1));
    p = make_move(p, new_move(p, C5, F8)); // 2.
    cl_check(repetition(p));
    cl_check(third_repetition(p) == false);

    p = make_move(p, new_move(p, F1, C4));
    p = make_move(p, new_move(p, F8, C5));
    p = make_move(p, new_move(p, B1, C3));
    p = make_move(p, new_move(p, B8, C6));
    p = make_move(p, new_move(p, C4, F1));
    p = make_move(p, new_move(p, C5, F8));
    p = make_move(p, new_move(p, C3, B1));
    p = make_move(p, new_move(p, C6, B8)); // 3.
    cl_check(repetition(p));
    cl_check(third_repetition(p));
}

// Irreversible 0-0.
void test_position_moves_suite__120(void) {
    setup("Ke1,Rh1,h2", "Ke8,Ra8,a7");
    Position *p = start();
    p = make_move(p, new_move(p, H2, H4));
    p = make_move(p, new_move(p, A7, A5)); // 1.
    p = make_move(p, new_move(p, E1, E2));
    p = make_move(p, new_move(p, E8, E7)); // King has moved.
    p = make_move(p, new_move(p, E2, E1));
    p = make_move(p, new_move(p, E7, E8)); // 2.
    p = make_move(p, new_move(p, E1, E2));
    p = make_move(p, new_move(p, E8, E7)); // King has moved again.
    p = make_move(p, new_move(p, E2, E1));
    p = make_move(p, new_move(p, E7, E8)); // 3.
    cl_check(repetition(p));
    cl_check(third_repetition(p) == false); // <-- Lost 0-0 right.

    p = make_move(p, new_move(p, E1, E2));
    p = make_move(p, new_move(p, E8, E7)); // King has moved again.
    p = make_move(p, new_move(p, E2, E1));
    p = make_move(p, new_move(p, E7, E8)); // 4.
    cl_check(repetition(p));
    cl_check(third_repetition(p)); // <-- 3 time repetioion with lost 0-0 right.
}

// 50 moves draw (no captures, no pawn moves).
void test_position_moves_suite__130(void) {
    setup("Kh8,Ra1", "Ka8,a7,b7");
    Position *p = start();
    int squares[64] = {
        A1, B1, C1, D1, E1, F1, G1, H1,
        H2, G2, F2, E2, D2, C2, B2, A2,
        A3, B3, C3, D3, E3, F3, G3, H3,
        H4, G4, F4, E4, D4, C4, B4, A4,
        A5, B5, C5, D5, E5, F5, G5, H5,
        H6, G6, F6, E6, D6, C6, B6, A6,
        A5, B5, C5, D5, E5, F5, G5, H5,
        H4, G4, F4, E4, D4, C4, B4, A4
    };

    // White rook is zigzaging while black king bounces back and forth.
    for (int move = 1; move < len(squares); move++) {
        p = make_move(p, new_move(p, squares[move - 1], squares[move]));
        if (p->king[Black] == A8) {
            p = make_move(p, new_move(p, A8, B8));
        } else {
            p = make_move(p, new_move(p, B8, A8));
        }

        cl_check(fifty(p) == (move >= 50));
    }
}

// Incremental id recalculation tests (see book_test.go));
void test_position_moves_suite__200(void) { // 1. e4
    new_game();
    Position *p = start();
    p = make_move(p, new_move(p, E2, E4));

    cl_check(p->id == (uint64)0x823C9B50FD114196);
    cl_check(p->pawnId == (uint64)0x0B2D6B38C0B92E91);

    cl_check(p->balance == len(materialBase) - 1);
    cl_check(p->enpassant == (uint8)0);
    cl_check(p->castles == (uint8)0x0F);
}

void test_position_moves_suite__210(void) { // 1. e4 d5
    new_game();
    Position *p = start();
    p = make_move(p, new_move(p, E2, E4));
    p = make_move(p, new_move(p, D7, D5));

    cl_check(p->id == (uint64)0x0756B94461C50FB0);
    cl_check(p->pawnId == (uint64)0x76916F86F34AE5BE);

    cl_check(p->balance == len(materialBase) - 1);
    cl_check(p->enpassant == (uint8)0);
    cl_check(p->castles == (uint8)0x0F);
}

// 1. e4 d5 2. e5
void test_position_moves_suite__220(void) {
    new_game();
    Position *p = start();
    p = make_move(p, new_move(p, E2, E4));
    p = make_move(p, new_move(p, D7, D5));
    p = make_move(p, new_move(p, E4, E5));

    cl_check(p->id == (uint64)0x662FAFB965DB29D4);
    cl_check(p->pawnId == (uint64)0xEF3E5FD1587346D3);

    cl_check(p->balance == len(materialBase) - 1);
    cl_check(p->enpassant == (uint8)0);
    cl_check(p->castles == (uint8)0x0F);
}

// 1. e4 d5 2. e5 f5 <-- Enpassant
void test_position_moves_suite__230(void) {
    new_game();
    Position *p = start();
    p = make_move(p, new_move(p, E2, E4));
    p = make_move(p, new_move(p, D7, D5));
    p = make_move(p, new_move(p, E4, E5));
    p = make_move(p, new_enpassant(p, F7, F5));

    cl_check(p->id == (uint64)0x22A48B5A8E47FF78);
    cl_check(p->pawnId == (uint64)0x83871FE249DCEE04);

    cl_check(p->balance == len(materialBase) - 1);
    cl_check(p->enpassant == (uint8)F6);
    cl_check(p->castles == (uint8)0x0F);
}

// 1. e4 d5 2. e5 f5 3. Ke2 <-- White Castle
void test_position_moves_suite__240(void) {
    new_game();
    Position *p = start();
    p = make_move(p, new_move(p, E2, E4));
    p = make_move(p, new_move(p, D7, D5));
    p = make_move(p, new_move(p, E4, E5));
    p = make_move(p, new_move(p, F7, F5));
    p = make_move(p, new_move(p, E1, E2));

    cl_check(p->id == (uint64)0x652A607CA3F242C1);
    cl_check(p->pawnId == (uint64)0x83871FE249DCEE04);

    cl_check(p->balance == len(materialBase) - 1);
    cl_check(p->enpassant == (uint8)0);
    cl_check(p->castles == (castleKingside[Black] | castleQueenside[Black]));
}

// 1. e4 d5 2. e5 f5 3. Ke2 Kf7 <-- Black Castle
void test_position_moves_suite__250(void) {
    new_game();
    Position *p = start();
    p = make_move(p, new_move(p, E2, E4));
    p = make_move(p, new_move(p, D7, D5));
    p = make_move(p, new_move(p, E4, E5));
    p = make_move(p, new_move(p, F7, F5));
    p = make_move(p, new_move(p, E1, E2));
    p = make_move(p, new_move(p, E8, F7));

    cl_check(p->id == (uint64)0x00FDD303C946BDD9);
    cl_check(p->pawnId == (uint64)0x83871FE249DCEE04);

    cl_check(p->balance == len(materialBase) - 1);
    cl_check(p->enpassant == (uint8)0);
    cl_check(p->castles == (uint8)0);
}

// 1. a2a4 b7b5 2. h2h4 b5b4 3. c2c4 <-- Enpassant
void test_position_moves_suite__260(void) {
    new_game();
    Position *p = start();
    p = make_move(p, new_move(p, A2, A4));
    p = make_move(p, new_move(p, B7, B5));
    p = make_move(p, new_move(p, H2, H4));
    p = make_move(p, new_move(p, B5, B4));
    p = make_move(p, new_enpassant(p, C2, C4));

    cl_check(p->id == (uint64)0x3C8123EA7B067637);
    cl_check(p->pawnId == (uint64)0xB5AA405AF42E7052);

    cl_check(p->balance == len(materialBase) - 1);
    cl_check(p->enpassant == (uint8)C3);
    cl_check(p->castles == (uint8)0x0F);
}

// 1. a2a4 b7b5 2. h2h4 b5b4 3. c2c4 b4xc3 4. Ra1a3 <-- Enpassant/Castle
void test_position_moves_suite__270(void) {
    new_game();
    Position *p = start();
    p = make_move(p, new_move(p, A2, A4));
    p = make_move(p, new_move(p, B7, B5));
    p = make_move(p, new_move(p, H2, H4));
    p = make_move(p, new_move(p, B5, B4));
    p = make_move(p, new_enpassant(p, C2, C4));
    p = make_move(p, new_move(p, B4, C3));
    p = make_move(p, new_move(p, A1, A3));

    cl_check(p->id == (uint64)0x5C3F9B829B279560);
    cl_check(p->pawnId == (uint64)0xE214F040EAA135A0);

    cl_check(p->balance == len(materialBase) - 1 - materialBalance[Pawn]);
    cl_check(p->enpassant == (uint8)0);
    cl_check(p->castles == (castleKingside[White] | castleKingside[Black] | castleQueenside[Black]));
}

// Incremental material id calculation.

// 1. e4 d5 2. e4xd5
void test_position_moves_suite__280(void) {
    new_game();
    Position *p = start();
    p = make_move(p, new_move(p, E2, E4)); p = make_move(p, new_move(p, D7, D5));
    p = make_move(p, new_move(p, E4, D5));

    cl_check(p->balance == len(materialBase) - 1 - materialBalance[BlackPawn]);
}

// 1. e4 d5 2. e4xd5 Ng8-f6 3. Nb1-c3 Nf6xd5
void test_position_moves_suite__281(void) {
    new_game();
    Position *p = start();
    p = make_move(p, new_move(p, E2, E4)); p = make_move(p, new_move(p, D7, D5));
    p = make_move(p, new_move(p, E4, D5)); p = make_move(p, new_move(p, G8, F6));
    p = make_move(p, new_move(p, B1, C3)); p = make_move(p, new_move(p, F6, D5));

    cl_check(p->balance == len(materialBase) - 1 - materialBalance[Pawn] - materialBalance[BlackPawn]);
}

// 1. e4 d5 2. e4xd5 Ng8-f6 3. Nb1-c3 Nf6xd5 4. Nc3xd5 Qd8xd5
void test_position_moves_suite__282(void) {
    new_game();
    Position *p = start();
    p = make_move(p, new_move(p, E2, E4)); p = make_move(p, new_move(p, D7, D5));
    p = make_move(p, new_move(p, E4, D5)); p = make_move(p, new_move(p, G8, F6));
    p = make_move(p, new_move(p, B1, C3)); p = make_move(p, new_move(p, F6, D5));
    p = make_move(p, new_move(p, C3, D5)); p = make_move(p, new_move(p, D8, D5));

    cl_check(p->balance == len(materialBase) - 1 - materialBalance[Pawn] - materialBalance[Knight] - materialBalance[BlackPawn] - materialBalance[BlackKnight]);
}

// Pawn promotion.
void test_position_moves_suite__283(void) {
    setup("Kh1", "M,Ka8,a2,b7");
    Position *p = start();
    cl_check(p->balance == 2 * materialBalance[BlackPawn]);
    p = make_move(p, promote(new_move(p, A2, A1), Rook));
    cl_check(p->balance == materialBalance[BlackPawn] + materialBalance[BlackRook]);
}

// Last pawn promotion.
void test_position_moves_suite__284(void) {
    setup("Kh1", "M,Ka8,a2");
    Position *p = start();
    cl_check(p->balance == materialBalance[BlackPawn]);
    p = make_move(p, promote(new_move(p, A2, A1), Rook));
    cl_check(p->balance == materialBalance[BlackRook]);
}

// Pawn promotion with capture.
void test_position_moves_suite__285(void) {
    setup("Kh1,Nb1,Ng1", "M,Ka8,a2,b7");
    Position *p = start();
    cl_check(p->balance == 2 * materialBalance[Knight] + 2 * materialBalance[BlackPawn]);
    p = make_move(p, promote(new_move(p, A2, B1), Queen));
    cl_check(p->balance == materialBalance[Knight] + materialBalance[BlackPawn] + materialBalance[BlackQueen]);
}

// Pawn promotion with last piece capture.
void test_position_moves_suite__286(void) {
    setup("Kh1,Nb1", "M,Ka8,a2,b7");
    Position *p = start();
    cl_check(p->balance == materialBalance[Knight] + 2 * materialBalance[BlackPawn]);
    p = make_move(p, promote(new_move(p, A2, B1), Queen));
    cl_check(p->balance == materialBalance[BlackPawn] + materialBalance[BlackQueen]);
}

// Last pawn promotion with capture.
void test_position_moves_suite__287(void) {
    setup("Kh1,Nb1,Ng1", "M,Ka8,a2");
    Position *p = start();
    cl_check(p->balance == 2 * materialBalance[Knight] + materialBalance[BlackPawn]);
    p = make_move(p, promote(new_move(p, A2, B1), Queen));
    cl_check(p->balance == materialBalance[Knight] + materialBalance[BlackQueen]);
}

// Last pawn promotion with last piece capture.
void test_position_moves_suite__288(void) {
    setup("Kh1,Nb1", "M,Ka8,a2");
    Position *p = start();
    cl_check(p->balance == materialBalance[Knight] + materialBalance[BlackPawn]);
    p = make_move(p, promote(new_move(p, A2, B1), Queen));
    cl_check(p->balance == materialBalance[BlackQueen]);
}

// Capture.
void test_position_moves_suite__289(void) {
    setup("Kh1,Nc3,Nf3", "M,Ka8,d4,e4");
    Position *p = start();
    cl_check(p->balance == 2 * materialBalance[Knight] + 2 * materialBalance[BlackPawn]);
    p = make_move(p, new_move(p, D4, C3));
    cl_check(p->balance == materialBalance[Knight] + 2 * materialBalance[BlackPawn]);
}

// Last piece capture.
void test_position_moves_suite__290(void) {
    setup("Kh1,Nc3", "M,Ka8,d4,e4");
    Position *p = start();
    cl_check(p->balance == materialBalance[Knight] + 2 * materialBalance[BlackPawn]);
    p = make_move(p, new_move(p, D4, C3));
    cl_check(p->balance == 2 * materialBalance[BlackPawn]);
}

// En-passant capture: 1. e2-e4 e7-e6 2. e4-e5 d7-d5 3. e4xd5
void test_position_moves_suite__291(void) {
    new_game();
    Position *p = start();
    cl_check(p->balance == len(materialBase) - 1);
    p = make_move(p, new_move(p, E2, E4)); p = make_move(p, new_move(p, E7, E6));
    p = make_move(p, new_move(p, E4, E5)); p = make_move(p, new_enpassant(p, D7, D5));
    p = make_move(p, new_move(p, E5, D6));
    cl_check(p->balance == len(materialBase) - 1 - materialBalance[BlackPawn]);
}

// Last pawn en-passant capture.
void test_position_moves_suite__292(void) {
    setup("Kh1,c2", "Ka8,d4");
    Position *p = start();
    cl_check(p->balance == materialBalance[Pawn] + materialBalance[BlackPawn]);
    p = make_move(p, new_enpassant(p, C2, C4)); p = make_move(p, new_move(p, D4, C3));
    cl_check(p->balance == materialBalance[BlackPawn]);
}


// Unobstructed pins.
void test_position_moves_suite__300(void) {
    setup("Ka1,Qe1,Ra8,Rh8,Bb5", "Ke8,Re7,Bc8,Bf8,Nc6");
    Position *p = start();
    Bitmask pinned = pinned_mask(p, E8);

    cl_check(pinned == (bit[C6] | bit[C8] | bit[E7] | bit[F8]));
}

void test_position_moves_suite__310(void) {
    setup("Ke4,Qe5,Rd5,Nd4,Nf4", "M,Ka7,Qe8,Ra4,Rh4,Ba8");
    Position *p = start();
    Bitmask pinned = pinned_mask(p, E4);

    cl_check(pinned == (bit[D5] | bit[E5] | bit[D4] | bit[F4]));
}

// Not a pin (friendly blockers));
void test_position_moves_suite__320(void) {
    setup("Ka1,Qe1,Ra8,Rh8,Bb5,Nb8,Ng8,e4", "Ke8,Re7,Bc8,Bf8,Nc6");
    Position *p = start();
    Bitmask pinned = pinned_mask(p, E8);

    cl_check(pinned == bit[C6]);
}

void test_position_moves_suite__330(void) {
    setup("Ke4,Qe7,Rc6,Nb4,Ng4", "M,Ka7,Qe8,Ra4,Rh4,Ba8,c4,e6,f4");
    Position *p = start();
    Bitmask pinned = pinned_mask(p, E4);

    cl_check(pinned == bit[C6]);
}

// Not a pin (enemy blockers));
void test_position_moves_suite__340(void) {
    setup("Ka1,Qe1,Ra8,Rh8,Bb5", "Ke8,Re7,Rg8,Bc8,Bf8,Nc6,Nb8,e4");
    Position *p = start();
    Bitmask pinned = pinned_mask(p, E8);

    cl_check(pinned == bit[C6]);
}

void test_position_moves_suite__350(void) {
    setup("Ke4,Qe7,Rc6,Nb4,Ng4,c4,e5,f4", "M,Ka7,Qe8,Ra4,Rh4,Ba8");
    Position *p = start();
    Bitmask pinned = pinned_mask(p, E4);

    cl_check(pinned == bit[C6]);
}

// Position after null move.
void test_position_moves_suite__400(void) {
    setup("Ke1,Qd1,d2,e2", "Kg8,Qf8,f7,g7");
    Position *p = start();

    p = make_null_move(p);
    cl_check(is_null(p) == true);

    p = undo_null_move(p);
    p = make_move(p, new_move(p, E2, E4));
    cl_check(is_null(p) == false);
}

// is_in_check
void test_position_moves_suite__410(void) {
    new_game();
    Position *p = start();
    p = make_move(p, new_move(p, E2, E4));
    p = make_move(p, new_move(p, F7, F6));
    Position *position = make_move(p, new_move(p, D1, H5));

    cl_check(is_in_check(position, position->color) == true);
    cl_check(is_in_check(position, p->color ^ 1) == true);
}
