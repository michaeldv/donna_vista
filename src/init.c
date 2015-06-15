// Copyright (c) 2014-2015 by Michael Dvorkin. All Rights Reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.

Bitmask kingMoves[64] = {};
Bitmask knightMoves[64] = {};
Bitmask pawnMoves[2][64] = {};
Bitmask rookMagicMoves[64][4096] = {};
Bitmask bishopMagicMoves[64][512] = {};

Bitmask maskPassed[2][64] = {};
Bitmask maskInFront[2][64] = {};

// Complete file or rank mask if both squares reside on on the same file
// or rank.
Bitmask maskStraight[64][64] = {};

// Complete diagonal mask if both squares reside on on the same diagonal.
Bitmask maskDiagonal[64][64] = {};

// If a king on square [x] gets checked from square [y] it can evade the
// check from all squares except maskEvade[x][y]. For example, if white
// king on B2 gets checked by black bishop on G7 the king can't step back
// to A1 (despite not being attacked by black).
Bitmask maskEvade[64][64] = {};

// If a king on square [x] gets checked from square [y] the check can be
// evaded by moving a piece to maskBlock[x][y]. For example, if white
// king on B2 gets checked by black bishop on G7 the check can be evaded
// by moving white piece onto C3-G7 diagonal (including capture on G7).
Bitmask maskBlock[64][64] = {};

// Bitmask to indicate pawn attacks for a square. For example, C3 is being
// attacked by white pawns on B2 and D2, and black pawns on B4 and D4.
Bitmask maskPawn[2][64] = {};

// Two arrays to simplify incremental polyglot hash computation.
uint64 hashCastle[16] = {};
uint64 hashEnpassant[8] = {};

// Distance between two squares.
int distance[64][64] = {};

// Precomputed database of material imbalance scores, evaluation flags,
// and endgame handlers. I wish they all could be California girls.
MaterialEntry materialBase[2*2*3*3*3*3*3*3*9*9] = {};

//------------------------------------------------------------------------------
void setupMasks(int square, int target, int row, int col, int r, int c) {
    if (row == r) {
        if (col < c) {
            maskBlock[square][target] = fill(square, 1, bit[target], maskFull);
            maskEvade[square][target] = spot(square, -1, ~maskFile[0]);
        } else if (col > c) {
            maskBlock[square][target] = fill(square, -1, bit[target], maskFull);
            maskEvade[square][target] = spot(square, 1, ~maskFile[7]);
        }
        if (col != c) {
            maskStraight[square][target] = maskRank[r];
        }
    } else if (col == c) {
        if (row < r) {
            maskBlock[square][target] = fill(square, 8, bit[target], maskFull);
            maskEvade[square][target] = spot(square, -8, ~maskRank[0]);
        } else {
            maskBlock[square][target] = fill(square, -8, bit[target], maskFull);
            maskEvade[square][target] = spot(square, 8, ~maskRank[7]);
        }
        if (row != r) {
            maskStraight[square][target] = maskFile[c];
        }
    } else if (r + col == row + c) { // Diagonals (A1->H8).
        if (col < c) {
            maskBlock[square][target] = fill(square, 9, bit[target], maskFull);
            maskEvade[square][target] = spot(square, -9, ~maskRank[0] & ~maskFile[0]);
        } else {
            maskBlock[square][target] = fill(square, -9, bit[target], maskFull);
            maskEvade[square][target] = spot(square, 9, ~maskRank[7] & ~maskFile[7]);
        }
        int shift = (r - c) & 15;
        if (shift < 8) { // A1-A8-H8
            maskDiagonal[square][target] = maskA1H8 << (8 * shift);
        } else { // B1-H1-H7
            maskDiagonal[square][target] = maskA1H8 >> (8 * (16 - shift));
        }
    } else if (row + col == r + c) { // AntiDiagonals (H1->A8).
        if (col < c) {
            maskBlock[square][target] = fill(square, -7, bit[target], maskFull);
            maskEvade[square][target] = spot(square, 7, ~maskRank[7] & ~maskFile[0]);
        } else {
            maskBlock[square][target] = fill(square, 7, bit[target], maskFull);
            maskEvade[square][target] = spot(square, -7, ~maskRank[0] & ~maskFile[7]);
        }
        int shift = 7 ^ (r + c);
        if (shift < 8) { // A8-A1-H1
            maskDiagonal[square][target] = maskH1A8 >> (8 * shift);
        } else { // B8-H8-H2
            maskDiagonal[square][target] = maskH1A8 << (8 * (16 - shift));
        }
    }

    // Default values are all 0 for maskBlock[square][target] and all 1 for
    // maskEvade[square][target].
    if empty(maskEvade[square][target]) {
        maskEvade[square][target] = maskFull;
    }
}

//------------------------------------------------------------------------------
Bitmask createRookMask(int square) {
    int r = row(square), c = col(square);
    Bitmask bitmask = (maskRank[r] | maskFile[c]) ^ bit[square];

    return trim(bitmask, r, c);
}

//------------------------------------------------------------------------------
Bitmask createBishopMask(int square) {
    int r = row(square), c = col(square);
    Bitmask bitmask = 0ULL;

    int sq = square + 7;
    if (sq <= H8 && col(sq) == c - 1) {
        bitmask = maskDiagonal[square][sq];
    } else {
        sq = square - 7;
        if (sq >= A1 && col(sq) == c + 1) {
            bitmask = maskDiagonal[square][sq];
        }
    }

    sq = square + 9;
    if (sq <= H8 && col(sq) == c + 1) {
        bitmask |= maskDiagonal[square][sq];
    } else {
        sq = square - 9;
        if (sq >= A1 && col(sq) == c - 1) {
            bitmask |= maskDiagonal[square][sq];
        }
    }
    bitmask ^= bit[square];

    return trim(bitmask, r, c);
}

//------------------------------------------------------------------------------
Bitmask createRookAttacks(int sq, Bitmask mask) {
    Bitmask bitmask = 0ULL;
    int row = row(sq), col = col(sq);

    // North.
    for (int r = row + 1; r <= 7; r++) {
        Bitmask b = bit[square(r, col)];
        bitmask |= b;
        if any(mask & b) {
            break;
        }
    }
    // East.
    for (int c = col + 1; c <= 7; c++) {
        Bitmask b = bit[square(row, c)];
        bitmask |= b;
        if any(mask & b) {
            break;
        }
    }
    // South.
    for (int r = row - 1; r >= 0; r--) {
        Bitmask b = bit[square(r, col)];
        bitmask |= b;
        if any(mask & b) {
            break;
        }
    }
    // West
    for (int c = col - 1; c >= 0; c--) {
        Bitmask b = bit[square(row, c)];
        bitmask |= b;
        if any(mask & b) {
            break;
        }
    }
    return bitmask;
}

//------------------------------------------------------------------------------
Bitmask createBishopAttacks(int sq, Bitmask mask) {
    Bitmask bitmask = 0ULL;
    int r, c, row = row(sq), col = col(sq);

    // North East.
    for (c = col + 1, r = row + 1; c <= 7 && r <= 7; c++, r++) {
        Bitmask b = bit[square(r, c)];
        bitmask |= b;
        if any(mask & b) {
            break;
        }
    }
    // South East.
    for (c = col + 1, r = row - 1; c <= 7 && r >= 0; c++, r--) {
        Bitmask b = bit[square(r, c)];
        bitmask |= b;
        if any(mask & b) {
            break;
        }
    }
    // South West.
    for (c = col - 1, r = row - 1; c >= 0 && r >= 0; c--, r--) {
        Bitmask b = bit[square(r, c)];
        bitmask |= b;
        if any(mask & b) {
            break;
        }
    }
    // North West.
    for (c = col - 1, r = row + 1; c >= 0 && r <= 7; c--, r++) {
        Bitmask b = bit[square(r, c)];
        bitmask |= b;
        if any(mask & b) {
            break;
        }
    }
    return bitmask;
}


//------------------------------------------------------------------------------
void initMasks() {
    for (int sq = A1; sq <= H8; sq++) {
        int row = row(sq), col = col(sq);

        // Distance, Blocks, Evasions, Straight, Diagonals, Knights, and Kings.
        for (int i = A1; i <= H8; i++) {
            int r = row(i), c = col(i);

            distance[sq][i] = max(abs(row - r), abs(col - c));
            setupMasks(sq, i, row, col, r, c);

            if (i == sq || abs(i-sq) > 17) {
                continue; // No king or knight can reach that far.
            }
            if ((abs(r - row) == 2 && abs(c - col) == 1) || (abs(r - row) == 1 && abs(c - col) == 2)) {
                // knightMoves[sq] = set(knightMoves[sq], i);
                knightMoves[sq] |= bit[i];
            }
            if (abs(r - row) <= 1 && abs(c - col) <= 1) {
                // kingMoves[sq] = set(kingMoves[sq], i);
                kingMoves[sq] |= bit[i];
            }
        }

        // Rooks.
        Bitmask mask = createRookMask(sq);
        int bits = count(mask);
        for (int i = 0; i < (1 << bits); i++) {
            Bitmask bitmask = magicify(mask, i);
            int index = (bitmask * rookMagic[sq].magic) >> 52;
            rookMagicMoves[sq][index] = createRookAttacks(sq, bitmask);
        }

        // Bishops.
        mask = createBishopMask(sq);
        bits = count(mask);
        for (int i = 0; i < (1 << bits); i++) {
            Bitmask bitmask = magicify(mask, i);
            int index = (bitmask * bishopMagic[sq].magic) >> 55;
            bishopMagicMoves[sq][index] = createBishopAttacks(sq, bitmask);
        }

        // Pawns.
        if (row >= A2H2 && row <= A7H7) {
            if (col > 0) {
                pawnMoves[White][sq] |= bit[square(row + 1, col - 1)];
                pawnMoves[Black][sq] |= bit[square(row - 1, col - 1)];
            }
            if (col < 7) {
                pawnMoves[White][sq] |= bit[square(row + 1, col + 1)];
                pawnMoves[Black][sq] |= bit[square(row - 1, col + 1)];
            }
        }

        // Pawn attacks.
        if (row > 1) { // White pawns can't attack first two ranks.
            if (col != 0) {
                maskPawn[White][sq] |= bit[sq - 9];
            }
            if (col != 7) {
                maskPawn[White][sq] |= bit[sq - 7];
            }
        }
        if (row < 6) { // Black pawns can attack 7th and 8th ranks.
            if (col != 0) {
                maskPawn[Black][sq] |= bit[sq + 7];
            }
            if (col != 7) {
                maskPawn[Black][sq] |= bit[sq + 9];
            }
        }

        // Vertical squares in front of a pawn.
        maskInFront[White][sq] = (maskBlock[sq][A8+col] | bit[A8+col]) & ~bit[sq];
        maskInFront[Black][sq] = (maskBlock[A1+col][sq] | bit[A1+col]) & ~bit[sq];

        // Masks to check for passed pawns.
        if (col > 0) {
            maskPassed[White][sq]     |= maskInFront[White][sq - 1];
            maskPassed[Black][sq]     |= maskInFront[Black][sq - 1];
            maskPassed[White][sq - 1] |= maskInFront[White][sq];
            maskPassed[Black][sq - 1] |= maskInFront[Black][sq];
        }
        maskPassed[White][sq] |= maskInFront[White][sq];
        maskPassed[Black][sq] |= maskInFront[Black][sq];
    }
}

//------------------------------------------------------------------------------
void initArrays() {

    // MSB lookup table.
    for (int i = 0; i < len(msb); i++) {
        if (i > 127) {
            msb[i] = 7;
        } else if (i > 63) {
            msb[i] = 6;
        } else if (i > 31) {
            msb[i] = 5;
        } else if (i > 15) {
            msb[i] = 4;
        } else if (i > 7) {
            msb[i] = 3;
        } else if (i > 3) {
            msb[i] = 2;
        } else if (i > 1) {
            msb[i] = 1;
        }
    }

    // Castle hash values.
    for (uint8 mask = 0; mask < 16; mask++) {
        if any(mask & castleKingside[White]) {
            hashCastle[mask] ^= polyglotRandomCastle[0];
        }
        if any(mask & castleQueenside[White]) {
            hashCastle[mask] ^= polyglotRandomCastle[1];
        }
        if any(mask & castleKingside[Black]) {
            hashCastle[mask] ^= polyglotRandomCastle[2];
        }
        if any(mask & castleQueenside[Black]) {
            hashCastle[mask] ^= polyglotRandomCastle[3];
        }
    }

    // Enpassant hash values.
    for (int col = A1; col <= H1; col++) {
        hashEnpassant[col] = polyglotRandomEnpassant[col];
    }
}

//------------------------------------------------------------------------------
void initPST() {
    pieceValue[Pawn]   = pieceValue[BlackPawn]   = valuePawn.midgame;
    pieceValue[Knight] = pieceValue[BlackKnight] = valueKnight.midgame;
    pieceValue[Bishop] = pieceValue[BlackBishop] = valueBishop.midgame;
    pieceValue[Rook]   = pieceValue[BlackRook]   = valueRook.midgame;
    pieceValue[Queen]  = pieceValue[BlackQueen]  = valueQueen.midgame;

    exchangeValue[Pawn]   = exchangeValue[BlackPawn]   = valuePawn.midgame;
    exchangeValue[Knight] = exchangeValue[BlackKnight] = valueKnight.midgame;
    exchangeValue[Bishop] = exchangeValue[BlackBishop] = valueBishop.midgame;
    exchangeValue[Rook]   = exchangeValue[BlackRook]   = valueRook.midgame;
    exchangeValue[Queen]  = exchangeValue[BlackQueen]  = valueQueen.midgame;
    exchangeValue[King]   = exchangeValue[BlackKing]   = valueQueen.midgame * 8;

    penaltyCover[5] = penaltyCover[6] = penaltyCover[7] = coverMissing.midgame;

    kingBoxA[0] = bit[D1] | bit[C1] | bit[B1];
    kingBoxA[1] = bit[D8] | bit[C8] | bit[B8];
    kingBoxH[0] = bit[E1] | bit[F1] | bit[G1];
    kingBoxH[1] = bit[E8] | bit[F8] | bit[G8];
    rookBoxA[0] = bit[A1] | bit[B1] | bit[C1];
    rookBoxA[1] = bit[A8] | bit[B8] | bit[C8];
    rookBoxH[0] = bit[H1] | bit[G1] | bit[F1];
    rookBoxH[1] = bit[H8] | bit[G8] | bit[F8];

    for (int sq = A1; sq <= H8; sq++) {
        // White pieces: flip sq index since bonus points have been
        // set up from black's point of view.
        int flip = sq ^ A8;
        pst[Pawn][sq]   = add((Score){ bonusPawn  [0][flip], bonusPawn  [1][flip] }, valuePawn);
        pst[Knight][sq] = add((Score){ bonusKnight[0][flip], bonusKnight[1][flip] }, valueKnight);
        pst[Bishop][sq] = add((Score){ bonusBishop[0][flip], bonusBishop[1][flip] }, valueBishop);
        pst[Rook][sq]   = add((Score){ bonusRook  [0][flip], bonusRook  [1][flip] }, valueRook);
        pst[Queen][sq]  = add((Score){ bonusQueen [0][flip], bonusQueen [1][flip] }, valueQueen);
        pst[King][sq]   = add((Score){ bonusKing  [0][flip], bonusKing  [1][flip] }, (Score){0, 0});

        // Black pieces: use sq index as is, and assign negative
        // values so we could use white + black without extra condition.
        pst[BlackPawn][sq]   = sub((Score){ -bonusPawn  [0][sq], -bonusPawn  [1][sq] }, valuePawn);
        pst[BlackKnight][sq] = sub((Score){ -bonusKnight[0][sq], -bonusKnight[1][sq] }, valueKnight);
        pst[BlackBishop][sq] = sub((Score){ -bonusBishop[0][sq], -bonusBishop[1][sq] }, valueBishop);
        pst[BlackRook][sq]   = sub((Score){ -bonusRook  [0][sq], -bonusRook  [1][sq] }, valueRook);
        pst[BlackQueen][sq]  = sub((Score){ -bonusQueen [0][sq], -bonusQueen [1][sq] }, valueQueen);
        pst[BlackKing][sq]   = sub((Score){ -bonusKing  [0][sq], -bonusKing  [1][sq] }, (Score){0, 0});
    }
}


// Simplified second-degree polynomial material imbalance by Tord Romstad.
//------------------------------------------------------------------------------
int imbalance(int w2, int wP, int wN, int wB, int wR, int wQ, int b2, int bP, int bN, int bB, int bR, int bQ) {
    #define polynom(x, a, b, c) (a * (x * x) + (b + c) * x)

    return polynom(w2,    0, (   0                                                                                    ),  1852) +
           polynom(wP,    2, (  39*w2 +                                      37*b2                                    ),  -162) +
           polynom(wN,   -4, (  35*w2 + 271*wP +                             10*b2 +  62*bP                           ), -1122) +
           polynom(wB,    0, (   0*w2 + 105*wP +   4*wN +                    57*b2 +  64*bP +  39*bN                  ),  -183) +
           polynom(wR, -141, ( -27*w2 +  -2*wP +  46*wN + 100*wB +           50*b2 +  40*bP +  23*bN + -22*bB         ),   249) +
           polynom(wQ,    0, (-177*w2 +  25*wP + 129*wN + 142*wB + -137*wR + 98*b2 + 105*bP + -39*bN + 141*bB + 274*bR),  -154);

    #undef polynom
}

int winAgainstBareKing();
int knightAndBishopVsBareKing();
int twoBishopsVsBareKing();
int kingAndPawnVsBareKing();
int kingAndPawnsVsBareKing();
int bishopsAndPawns();
int drawishBishops();
int kingAndPawnVsKingAndPawn();
int bishopAndPawnVsBareKing();
int rookAndPawnVsRook();
int queenVsRookAndPawns();
int lastPawnLeft();
int noPawnsLeft();

//------------------------------------------------------------------------------
void endgames(int index, int wP, int wN, int wB, int wR, int wQ, int bP, int bN, int bB, int bR, int bQ) {
    int wMinor = wN + wB, wMajor = wR + wQ;
    int bMinor = bN + bB, bMajor = bR + bQ;
    int allMinor = wMinor + bMinor, allMajor = wMajor + bMajor;

    bool noPawns = (wP + bP == 0);
    bool bareKing = ((wP + wMinor + wMajor) * (bP + bMinor + bMajor) == 0); // Bare king (white, black or both).

    // Set king safety materialBase[index].flags if (the opposing side has a queen and at least one piece.
    if (wQ > 0 && (wN + wB + wR) > 0) {
        materialBase[index].flags |= blackKingSafety;
    }
    if (bQ > 0 && (bN + bB + bR) > 0) {
        materialBase[index].flags |= whiteKingSafety;
    }

    // Insufficient material endgames that don't require further evaluation:
    // 1) Two bare kings.
    if (wP + bP + allMinor + allMajor == 0) {
        materialBase[index].flags |= materialDraw;

    // 2) No pawns and king with a minor.
    } else if (noPawns && allMajor == 0 && wMinor < 2 && bMinor < 2) {
        materialBase[index].flags |= materialDraw;

    // 3) No pawns and king with two knights.
    } else if (noPawns && allMajor == 0 && allMinor == 2 && (wN == 2 || bN == 2)) {
        materialBase[index].flags |= materialDraw;

    // Known endgame: king and a pawn vs. bare king.
    } else if (wP + bP == 1 && allMinor == 0 && allMajor == 0) {
        materialBase[index].flags |= knownEndgame;
        materialBase[index].endgame = kingAndPawnVsBareKing;

    // Known endgame: king with a knight and a bishop vs. bare king.
    } else if (noPawns && allMajor == 0 && ((wN == 1 && wB == 1) || (bN == 1 && bB == 1))) {
        materialBase[index].flags |= knownEndgame;
        materialBase[index].endgame = knightAndBishopVsBareKing;

    // Known endgame: two bishops vs. bare king.
    } else if (noPawns && allMajor == 0 && ((wN == 0 && wB == 2) || (bN == 0 && bB == 2))) {
        materialBase[index].flags |= knownEndgame;
        materialBase[index].endgame = twoBishopsVsBareKing;

    // Known endgame: king with some winning material vs. bare king.
    } else if (bareKing && allMajor > 0) {
        materialBase[index].flags |= knownEndgame;
        materialBase[index].endgame = winAgainstBareKing;

    // Lesser known endgame: king and two or more pawns vs. bare king.
    } else if (bareKing && allMinor + allMajor == 0 && wP + bP > 1) {
        materialBase[index].flags |= lesserKnownEndgame;
        materialBase[index].endgame = kingAndPawnsVsBareKing;

    // Lesser known endgame: queen vs. rook with pawn(s)
    } else if ((wP + wMinor + wR == 0 && wQ == 1 && bMinor + bQ == 0 && bP > 0 && bR == 1) ||
               (bP + bMinor + bR == 0 && bQ == 1 && wMinor + wQ == 0 && wP > 0 && wR == 1)) {
        materialBase[index].flags |= lesserKnownEndgame;
        materialBase[index].endgame = queenVsRookAndPawns;

    // Lesser known endgame: king and pawn vs. king and pawn.
    } else if (allMinor + allMajor == 0 && wP == 1 && bP == 1) {
        materialBase[index].flags |= lesserKnownEndgame;
        materialBase[index].endgame = kingAndPawnVsKingAndPawn;

    // Lesser known endgame: bishop and pawn vs. bare king.
    } else if (bareKing && allMajor == 0 && allMinor == 1 && ((wB + wP == 2) || (bB + bP == 2))) {
        materialBase[index].flags |= lesserKnownEndgame;
        materialBase[index].endgame = bishopAndPawnVsBareKing;

    // Lesser known endgame: rook and pawn vs. rook.
    } else if (allMinor == 0 && wQ + bQ == 0 && ((wR + wP == 2) || (bR + bP == 2))) {
        materialBase[index].flags |= lesserKnownEndgame;
        materialBase[index].endgame = rookAndPawnVsRook;

    // Lesser known endgame: no pawns left.
    } else if ((wP == 0 || bP == 0) && wMajor - bMajor == 0 && abs(wMinor - bMinor) <= 1) {
        materialBase[index].flags |= lesserKnownEndgame;
        materialBase[index].endgame = noPawnsLeft;

    // Lesser known endgame: single pawn with not a lot of material.
    } else if ((wP == 1 || bP == 1) && wMajor - bMajor == 0 && abs(wMinor - bMinor) <= 1) {
        materialBase[index].flags |= lesserKnownEndgame;
        materialBase[index].endgame = lastPawnLeft;

    // Check for potential opposite-colored bishops.
    } else if (wB * bB == 1) {
        materialBase[index].flags |= singleBishops;
        if (allMajor == 0 && allMinor == 2) {
            materialBase[index].flags |= lesserKnownEndgame;
            materialBase[index].endgame = bishopsAndPawns;
        } else if ((materialBase[index].flags & (whiteKingSafety | blackKingSafety)) == 0) {
            materialBase[index].flags |= lesserKnownEndgame;
            materialBase[index].endgame = drawishBishops;
        }
    }
}


//------------------------------------------------------------------------------
void initMaterial() {
    int index;

    for (int wQ = 0; wQ < 2; wQ++) {
        for (int bQ = 0; bQ < 2; bQ++) {
            for (int wR = 0; wR < 3; wR++) {
                for (int bR = 0; bR < 3; bR++) {
                    for (int wB = 0; wB < 3; wB++) {
                        for (int bB = 0; bB < 3; bB++) {
                            for (int wN = 0; wN < 3; wN++) {
                                for (int bN = 0; bN < 3; bN++) {
                                    for (int wP = 0; wP < 9; wP++) {
                                        for (int bP = 0; bP < 9; bP++) {
        index = wQ * materialBalance[Queen]       +
                bQ * materialBalance[BlackQueen]  +
                wR * materialBalance[Rook]        +
                bR * materialBalance[BlackRook]   +
                wB * materialBalance[Bishop]      +
                bB * materialBalance[BlackBishop] +
                wN * materialBalance[Knight]      +
                bN * materialBalance[BlackKnight] +
                wP * materialBalance[Pawn]        +
                bP * materialBalance[BlackPawn];

        // Compute game phase and home turf values.
        materialBase[index].phase = 12 * (wN + bN + wB + bB) + 18 * (wR + bR) + 44 * (wQ + bQ);
        materialBase[index].turf = (wN + bN + wB + bB) * 36;

        // Set up evaluation flags and endgame handlers.
        endgames(index, wP, wN, wB, wR, wQ, bP, bN, bB, bR, bQ);

        // Compute material imbalance scores.
        if (wQ != bQ || wR != bR || wB != bB || wN != bN || wP != bP) {
            int white = imbalance(wB/2, wP, wN, wB, wR, wQ,  bB/2, bP, bN, bB, bR, bQ);
            int black = imbalance(bB/2, bP, bN, bB, bR, bQ,  wB/2, wP, wN, wB, wR, wQ);

            int adjustment = (white - black) / 32;
            materialBase[index].score.midgame += adjustment;
            materialBase[index].score.endgame += adjustment;
        }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}


//------------------------------------------------------------------------------
void init() {
    initMasks();
    initArrays();
    initPST();
    initMaterial();
}

