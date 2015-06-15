// Copyright (c) 2014-2015 by Michael Dvorkin. All Rights Reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.

void test_init_suite__000x(void) {
    cl_check(maskBlock[C3][H8] == (bit[D4] | bit[E5] | bit[F6] | bit[G7] | bit[H8]));
    cl_check(maskBlock[C3][C8] == (bit[C4] | bit[C5] | bit[C6] | bit[C7] | bit[C8]));
    cl_check(maskBlock[C3][A5] == (bit[B4] | bit[A5]                              ));
    cl_check(maskBlock[C3][A3] == (bit[B3] | bit[A3]                              ));
    cl_check(maskBlock[C3][A1] == (bit[B2] | bit[A1]                              ));
    cl_check(maskBlock[C3][C1] == (bit[C2] | bit[C1]                              ));
    cl_check(maskBlock[C3][E1] == (bit[D2] | bit[E1]                              ));
    cl_check(maskBlock[C3][H3] == (bit[D3] | bit[E3] | bit[F3] | bit[G3] | bit[H3]));
    cl_check(maskBlock[C3][E7] == 0);
}

void test_init_suite__010x(void) {
    cl_check(maskEvade[C3][H8] == ~bit[B2]);
    cl_check(maskEvade[C3][C8] == ~bit[C2]);
    cl_check(maskEvade[C3][A5] == ~bit[D2]);
    cl_check(maskEvade[C3][A3] == ~bit[D3]);
    cl_check(maskEvade[C3][A1] == ~bit[D4]);
    cl_check(maskEvade[C3][C1] == ~bit[C4]);
    cl_check(maskEvade[C3][E1] == ~bit[B4]);
    cl_check(maskEvade[C3][H3] == ~bit[B3]);
    cl_check(maskEvade[C3][E7] == maskFull);
}

void test_init_suite__020x(void) {
    cl_check(maskPawn[White][A3] == bit[B2]);
    cl_check(maskPawn[Black][H4] == bit[G5]);
    cl_check(maskPawn[White][D5] == (bit[C4] | bit[E4]));
    cl_check(maskPawn[White][F8] == (bit[E7] | bit[G7]));
    cl_check(maskPawn[Black][C5] == (bit[B6] | bit[D6]));
    cl_check(maskPawn[Black][B1] == (bit[A2] | bit[C2]));
}

void test_init_suite__030x(void) {
    // Same file.
    cl_check(maskStraight[A2][A5] == maskFile[0]);
    cl_check(maskStraight[H6][H1] == maskFile[7]);
    // Same rank.
    cl_check(maskStraight[A2][F2] == maskRank[1]);
    cl_check(maskStraight[H6][B6] == maskRank[5]);
    // Edge cases.
    cl_check(maskStraight[A1][C5] == maskNone); // Random squares.
    cl_check(maskStraight[E4][E4] == maskNone); // Same square.
}

void test_init_suite__040x(void) {
    // Same diagonal.
    cl_check(maskDiagonal[C4][F7] == (bit[A2] | bit[B3] | bit[C4] | bit[D5] | bit[E6] | bit[F7] | bit[G8]));
    cl_check(maskDiagonal[F6][H8] == maskA1H8);
    cl_check(maskDiagonal[F1][H3] == (bit[F1] | bit[G2] | bit[H3]));
    // Same anti-diagonal.
    cl_check(maskDiagonal[C2][B3] == (bit[D1] | bit[C2] | bit[B3] | bit[A4]));
    cl_check(maskDiagonal[F3][B7] == maskH1A8);
    cl_check(maskDiagonal[H3][D7] == (bit[H3] | bit[G4] | bit[F5] | bit[E6] | bit[D7] | bit[C8]));
    // Edge cases.
    cl_check(maskDiagonal[A2][G4] == maskNone); // Random squares.
    cl_check(maskDiagonal[E4][E4] == maskNone); // Same square.
}

// Material base tests.

// Bare kings.
void test_init_suite__000(void) {
    int balance = materialBalance[King] + materialBalance[BlackKing];
    cl_check(balance == 0);
    // cl_check(materialBase[balance].flags == (uint8)materialDraw);
    // cl_check(materialBase[balance].endgame == NULL);

    setup("Ke1", "Ke8");
    Position *p = start();
    cl_check(p->balance == balance);
}

// No pawns, king with a minor.
void test_init_suite__010(void) {
    int balance = materialBalance[Bishop];
    // cl_check(materialBase[balance].flags == (uint8)materialDraw);
    // cl_check(materialBase[balance].endgame == NULL);

    setup("Ke1,Bc1", "Ke8");
    Position *p = start();
    cl_check(p->balance == balance);
}

void test_init_suite__015(void) {
    int balance = materialBalance[Bishop] + materialBalance[BlackKnight];
    // cl_check(materialBase[balance].flags == (uint8)materialDraw);
    // cl_check(materialBase[balance].endgame == NULL);

    setup("Ke1,Bc1", "Ke8,Nb8");
    Position *p = start();
    cl_check(p->balance == balance);
}

// No pawns, king with two knights.
void test_init_suite__020(void) {
    int balance = 2 * materialBalance[Knight];
    // cl_check(materialBase[balance].flags == (uint8)materialDraw);
    // cl_check(materialBase[balance].endgame == NULL);

    setup("Ke1,Ne2,Ne3", "Ke8");
    Position *p = start();
    cl_check(p->balance == balance);
}

// Known: king and a pawn vs. bare king.
void test_init_suite__030(void) {
    int balance = materialBalance[Pawn];
    // cl_check(materialBase[balance].flags == (uint8)knownEndgame);
    // cl_check(materialBase[balance].endgame == (*Evaluation).kingAndPawnVsBareKing);

    setup("Ke1,e2", "Ke8");
    Position *p = start();
    cl_check(p->balance == balance);
}

void test_init_suite__040(void) {
    int balance = materialBalance[BlackPawn];
    // cl_check(materialBase[balance].flags == (uint8)knownEndgame);
    // cl_check(materialBase[balance].endgame == (*Evaluation).kingAndPawnVsBareKing);

    setup("Ke1", "M,Ke8,e7");
    Position *p = start();
    cl_check(p->balance == balance);
}

// Known: king with a knight and a bishop vs. bare king.
void test_init_suite__050(void) {
    int balance = materialBalance[Knight] + materialBalance[Bishop];
    // cl_check(materialBase[balance].flags == (uint8)knownEndgame);
    // cl_check(materialBase[balance].endgame == (*Evaluation).knightAndBishopVsBareKing);

    setup("Ke1,Nb1,Bc1", "Ke8");
    Position *p = start();
    cl_check(p->balance == balance);
}

void test_init_suite__060(void) {
    int balance = materialBalance[BlackKnight] + materialBalance[BlackBishop];
    // cl_check(materialBase[balance].flags == (uint8)knownEndgame);
    // cl_check(materialBase[balance].endgame == (*Evaluation).knightAndBishopVsBareKing);

    setup("Ke1", "M,Ke8,Nb8,Bc8");
    Position *p = start();
    cl_check(p->balance == balance);
}

// Known endgame: two bishops vs. bare king.
void test_init_suite__070(void) {
    int balance = 2 * materialBalance[BlackBishop];
    // cl_check(materialBase[balance].flags == (uint8)knownEndgame);
    // cl_check(materialBase[balance].endgame == (*Evaluation).twoBishopsVsBareKing);

    setup("Ke1", "M,Ka8,Bg8,Bh8");
    Position *p = start();
    cl_check(p->balance == balance);
}

// Known endgame: king with some winning material vs. bare king.
void test_init_suite__080(void) {
    int balance = materialBalance[BlackRook];
    // cl_check(materialBase[balance].flags == (uint8)knownEndgame);
    // cl_check(materialBase[balance].endgame == (*Evaluation).winAgainstBareKing);

    setup("Ke1", "M,Ka8,Rh8");
    Position *p = start();
    cl_check(p->balance == balance);
}

// Lesser known endgame: king and two or more pawns vs. bare king.
void test_init_suite__090(void) {
    int balance = 2 * materialBalance[Pawn];
    // cl_check(materialBase[balance].flags == (uint8)lesserKnownEndgame);
    // cl_check(materialBase[balance].endgame == (*Evaluation).kingAndPawnsVsBareKing);

    setup("Ke1,a4,a5", "M,Ka8");
    Position *p = start();
    cl_check(p->balance == balance);
}

// Lesser known endgame: queen vs. rook with pawn(s)
void test_init_suite__100(void) {
    int balance = materialBalance[Rook] + materialBalance[Pawn] + materialBalance[BlackQueen];
    // cl_check(materialBase[balance].flags == (uint8)lesserKnownEndgame);
    // cl_check(materialBase[balance].endgame == (*Evaluation).queenVsRookAndPawns);

    setup("Ke1,Re4,e5", "M,Ka8,Qh8");
    Position *p = start();
    cl_check(p->balance == balance);
}

// Lesser known endgame: king and pawn vs. king and pawn.
void test_init_suite__110(void) {
    int balance = materialBalance[Pawn] + materialBalance[BlackPawn];
    // cl_check(materialBase[balance].flags == (uint8)lesserKnownEndgame);
    // cl_check(materialBase[balance].endgame == (*Evaluation).kingAndPawnVsKingAndPawn);

    setup("Ke1,a4", "M,Ka8,h5");
    Position *p = start();
    cl_check(p->balance == balance);
}

// Lesser known endgame: bishop and pawn vs. bare king.
void test_init_suite__120(void) {
    int balance = materialBalance[Pawn] + materialBalance[Bishop];
    // cl_check(materialBase[balance].flags == (uint8)lesserKnownEndgame);
    // cl_check(materialBase[balance].endgame == (*Evaluation).bishopAndPawnVsBareKing);

    setup("Ke1,Be2,a4", "Ka8");
    Position *p = start();
    cl_check(p->balance == balance);
}

// Lesser known endgame: rook and pawn vs. rook.
void test_init_suite__130(void) {
    int balance = materialBalance[Rook] + materialBalance[Pawn] + materialBalance[BlackRook];
    // cl_check(materialBase[balance].flags == (uint8)lesserKnownEndgame);
    // cl_check(materialBase[balance].endgame == (*Evaluation).rookAndPawnVsRook);

    setup("Ke1,Re2,a4", "Ka8,Rh8");
    Position *p = start();
    cl_check(p->balance == balance);
}

// Single bishops (midgame).
void test_init_suite__140(void) {
    int balance = materialBalance[Pawn] * 2 + materialBalance[Bishop] + materialBalance[Knight] + materialBalance[Rook] + materialBalance[BlackPawn] * 2 + materialBalance[BlackBishop] + materialBalance[BlackKnight] + materialBalance[BlackRook];
    // cl_check(materialBase[balance].flags == uint8(singleBishops | lesserKnownEndgame));
    // cl_check(materialBase[balance].endgame == (*Evaluation).drawishBishops);

    setup("Ke1,Ra1,Bc1,Nb1,d2,e2", "Ke8,Rh8,Bf8,Ng8,d7,e7");
    Position *p = start();
    cl_check(p->balance == balance);
}

// Single bishops (endgame).
void test_init_suite__150(void) {
    int balance = materialBalance[Bishop] + 4 * materialBalance[Pawn] + materialBalance[BlackBishop] + 3 * materialBalance[BlackPawn];
    // cl_check(materialBase[balance].flags == uint8(singleBishops | lesserKnownEndgame));
    // cl_check(materialBase[balance].endgame == (*Evaluation).bishopsAndPawns);

    setup("Ke1,Bc1,a2,b2,c2,d4", "Ke8,Bf8,f7,g7,h7");
    Position *p = start();
    cl_check(p->balance == balance);
}
