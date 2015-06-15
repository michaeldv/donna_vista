// Copyright (c) 2014-2015 by Michael Dvorkin. All Rights Reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.

typedef struct _DoubleScore {
  Score val, mob;
} DoubleScore, *PDoubleScore;


// Updates safety data used later on when evaluating king safety.
//------------------------------------------------------------------------------
void enemyKingThreat(Piece piece, Bitmask attacks) {
    int color = color(piece) ^ 1;

    if (attacks & eval.safety[color].fort) {
        eval.safety[color].attackers++;
        eval.safety[color].threats += bonusKingThreat[kind(piece) / 2];
        Bitmask bits = attacks & eval.attacks[king(color)];
        if (bits) {
            eval.safety[color].attacks += count(bits);
        }
    }
}

// Initializes the fort bitmask around king's square. For example, for a king on
// G1 the bitmask covers F1,F2,F3, G2,G3, and H1,H2,H3. For a king on a corner
// square, say H1, the bitmask covers F1,F2, G1,G2,G3, and H2,H3.
//------------------------------------------------------------------------------
Bitmask setupFort(uint8 color) {
    Bitmask bitmask = eval.attacks[king(color)] | pushed(eval.attacks[king(color)], color);

    switch (eval.position->king[color]) {
    case A1:
    case A8:
        bitmask |= eval.attacks[king(color)] << 1;
        break;
    case H1:
    case H8:
        bitmask |= eval.attacks[king(color)] >> 1;
        break;
    }

    return bitmask;
}

//------------------------------------------------------------------------------
DoubleScore knights(uint8 color, Bitmask maskSafe, bool isEnemyKingThreatened) {
    DoubleScore score = {};
    Position *p = eval.position;
    Bitmask outposts = p->outposts[knight(color)];

    while (any(outposts)) {
        int square = pop(&outposts);
        Bitmask attacked = attacks(p, square);

        // Bonus for knight's mobility.
        score.mob = add(score.mob, mobilityKnight[count(attacked & maskSafe)]);

        // Penalty if knight is attacked by enemy's pawn.
        if (any(maskPawn[color^1][square] & p->outposts[pawn(color^1)])) {
            score.val = sub(score.val, penaltyPawnThreat[Knight / 2]);
        }

        // Bonus if knight is behind friendly pawn.
        if (rank(color, square) < 4 && on(p->outposts[pawn(color)], square + eight[color])) {
            score.val = add(score.val, behindPawn);
        }

        // Extra bonus if knight is in the center. Increase the extra bonus
        // if the knight is supported by a pawn and can't be exchanged.
        int extra = extraKnight[flip(color, square)];
        if (extra > 0) {
            if (on(pawn_attacks(p, color), square)) {
                extra += extra / 2; // Supported by a pawn.
                if (empty(p->outposts[knight(color^1)]) && empty(same(square) & p->outposts[bishop(color^1)])) {
                    extra += extra * 2 / 3; // No knights or bishops to exchange.
                }
            }
            score.val = adjust(score.val, extra);
        }

        // Track if knight attacks squares around enemy's king.
        if (isEnemyKingThreatened) {
            enemyKingThreat(knight(color), attacked);
        }

        // Update attack bitmask for the knight.
        eval.attacks[knight(color)] |= attacked;
    }

    return score;
}

//------------------------------------------------------------------------------
DoubleScore bishops(uint8 color, Bitmask maskSafe, bool isEnemyKingThreatened) {
    DoubleScore score = {};
    Position *p = eval.position;
    Bitmask outposts = p->outposts[bishop(color)];

    while (any(outposts)) {
        int square = pop(&outposts);
        Bitmask attacked = xray_attacks(p, square);

        // Bonus for bishop's mobility
        score.mob = add(score.mob, mobilityBishop[count(attacked & maskSafe)]);

        // Penalty for light/dark-colored pawns restricting a bishop.
        int cnt = count(same(square) & p->outposts[pawn(color)]);
        if (cnt > 0) {
            score.val = sub(score.val, times(bishopPawn, cnt));
        }

        // Penalty if bishop is attacked by enemy's pawn.
        if (maskPawn[color^1][square] & p->outposts[pawn(color^1)]) {
            score.val = sub(score.val, penaltyPawnThreat[Bishop/2]);
        }

        // Bonus if bishop is behind friendly pawn.
        if (RANK(color, square) < 4 && on(p->outposts[pawn(color)], square + eight[color])) {
            score.val = add(score.val, behindPawn);
        }

        // Middle game penalty for boxed bishop.
        if (eval.material->phase > 160) {
            if (color == White) {
                if ((square == C1 && is_pawn(p->pieces[D2]) && p->pieces[D3]) ||
                    (square == F1 && is_pawn(p->pieces[E2]) && p->pieces[E3])) {
                    score.val.midgame -= bishopBoxed.midgame;
                }
            } else {
                if ((square == C8 && is_pawn(p->pieces[D7]) && p->pieces[D6]) ||
                    (square == F8 && is_pawn(p->pieces[E7]) && p->pieces[E6])) {
                    score.val.midgame -= bishopBoxed.midgame;
                }
            }
        }

        // Extra bonus if bishop is in the center. Increase the extra bonus
        // if the bishop is supported by a pawn and can't be exchanged.
        int extra = extraBishop[flip(color, square)];
        if (extra > 0) {
            if (on(pawn_attacks(p, color), square)) {
                extra += extra / 2; // Supported by a pawn.
                if (empty(p->outposts[knight(color^1)]) && empty(same(square) & p->outposts[bishop(color^1)])) {
                    extra += extra * 2 / 3; // No knights or bishops to exchange.
                }
            }
            score.val = adjust(score.val, extra);
        }

        // Track if bishop attacks squares around enemy's king.
        if (isEnemyKingThreatened) {
            enemyKingThreat(bishop(color), attacked);
        }

        // Update attack bitmask for the bishop.
        eval.attacks[bishop(color)] |= attacked;
    }

    return score;
}

//------------------------------------------------------------------------------
DoubleScore rooks(uint8 color, Bitmask maskSafe, bool isEnemyKingThreatened) {
    DoubleScore score = {};
    Position *p = eval.position;
    Bitmask hisPawns = p->outposts[pawn(color)];
    Bitmask herPawns = p->outposts[pawn(color^1)];
    Bitmask outposts = p->outposts[rook(color)];

    // Bonus if rook is on 7th rank and enemy's king trapped on 8th.
    int cnt = count(outposts & mask7th[color]);
    if (cnt > 0 && any(p->outposts[king(color^1)] & mask8th[color])) {
        score.val = add(score.val, times(rookOn7th, cnt));
    }
    while (any(outposts)) {
        int square = pop(&outposts);
        Bitmask attacked = xray_attacks(p, square);

        // Bonus for rook's mobility
        int safeSquares = count(attacked & maskSafe);
        score.mob = add(score.mob, mobilityRook[safeSquares]);

        // Penalty if rook is attacked by enemy's pawn.
        if (maskPawn[color^1][square] & herPawns) {
            score.val = sub(score.val, penaltyPawnThreat[Rook / 2]);
        }

        // Bonus if rook is attacking enemy's pawns.
        if (RANK(color, square) >= 4) {
            cnt = count(attacked & herPawns);
            if (cnt > 0) {
                score.val = add(score.val, times(rookOnPawn, cnt));
            }
        }

        // Bonuses if rook is on open or semi-open file.
        int col = COL(square);
        bool isFileAjar = empty(hisPawns & maskFile[col]);
        if (isFileAjar) {
            if empty(herPawns & maskFile[col]) {
                score.val = add(score.val, rookOnOpen);
            } else {
                score.val = add(score.val, rookOnSemiOpen);
            }
        }

        // Middle game penalty if a rook is boxed. Extra penalty if castle
        // rights have been lost.
        if (safeSquares <= 3 || !isFileAjar) {
            int kingSquare = (int)p->king[color];
            int kingColumn = COL(kingSquare);

            // Queenside box: king on D/C/B vs. rook on A/B/C files. Increase the
            // the penalty since no castle is possible.
            if (col < kingColumn && on(rookBoxA[color], square) && on(kingBoxA[color], kingSquare)) {
                score.val.midgame -= (rookBoxed.midgame - safeSquares * 10) * 2;
            }

            // Kingside box: king on E/F/G vs. rook on H/G/F files.
            if (col > kingColumn && on(rookBoxH[color], square) && on(kingBoxH[color], kingSquare)) {
                score.val.midgame -= (rookBoxed.midgame - safeSquares * 10);
                if (empty(p->castles & castleKingside[color])) {
                    score.val.midgame -= (rookBoxed.midgame - safeSquares * 10);
                }
            }
        }

        // Track if rook attacks squares around enemy's king.
        if (isEnemyKingThreatened) {
            enemyKingThreat(rook(color), attacked);
        }

        // Update attack bitmask for the rook.
        eval.attacks[rook(color)] |= attacked;
    }

    return score;
}

//------------------------------------------------------------------------------
DoubleScore queens(uint8 color, Bitmask maskSafe, bool isEnemyKingThreatened) {
    DoubleScore score = {};
    Position *p = eval.position;
    Bitmask outposts = p->outposts[queen(color)];

    // Bonus if queen is on 7th rank and enemy's king trapped on 8th.
    int cnt = count(outposts & mask7th[color]);
    if (cnt > 0 && (p->outposts[king(color^1)] & mask8th[color])) {
        score.val = add(score.val, times(queenOn7th, cnt));
    }

    while (any(outposts)) {
        int square = pop(&outposts);
        Bitmask attacked = attacks(p, square);

        // Bonus for queen's mobility.
        score.mob = add(score.mob, mobilityQueen[min(15, count(attacked & maskSafe))]);

        // Penalty if queen is attacked by enemy's pawn.
        if (maskPawn[color^1][square] & p->outposts[pawn(color^1)]) {
            score.val = sub(score.val, penaltyPawnThreat[Queen / 2]);
        }

        // Bonus if queen is out and attacking enemy's pawns.
        cnt = count(attacked & p->outposts[pawn(color^1)]);
        if (cnt > 0 && RANK(color, square) > 3) {
            score.val = add(score.val, times(queenOnPawn, cnt));
        }

        // Track if queen attacks squares around enemy's king.
        if (isEnemyKingThreatened) {
            enemyKingThreat(queen(color), attacked);
        }

        // Update attack bitmask for the queen.
        eval.attacks[queen(color)] |= attacked;
    }

    return score;
}

//------------------------------------------------------------------------------
void analyzePieces() {
    Position *p = eval.position;
    Score mobility[2] = {};
    DoubleScore knight[2] = {}, bishop[2] = {}, rook[2] = {}, queen[2] = {};

    // Mobility masks for both sides exclude a) squares attacked by enemy's
    // pawns and b) squares occupied by friendly pawns and the king.
    Bitmask maskSafeForWhite = ~(eval.attacks[BlackPawn] | p->outposts[Pawn] | p->outposts[King]);
    Bitmask maskSafeForBlack = ~(eval.attacks[Pawn] | p->outposts[BlackPawn] | p->outposts[BlackKing]);

    // Initialize king fort bitmasks only when we need them.
    bool isWhiteKingThreatened = ((eval.material->flags & whiteKingSafety) != 0);
    bool isBlackKingThreatened = ((eval.material->flags & blackKingSafety) != 0);
    if (isWhiteKingThreatened) {
        eval.safety[White].fort = setupFort(White);
    }
    if (isBlackKingThreatened) {
        eval.safety[Black].fort = setupFort(Black);
    }

    // Evaluate white pieces except the queen.
    if (any(p->outposts[Knight])) {
        knight[0] = knights(White, maskSafeForWhite, isBlackKingThreatened);
        mobility[0] = add(mobility[0], knight[0].mob);
    }
    if (any(p->outposts[Bishop])) {
        bishop[0] = bishops(White, maskSafeForWhite, isBlackKingThreatened);
        mobility[0] = add(mobility[0], bishop[0].mob);
    }
    if (any(p->outposts[Rook])) {
        rook[0] = rooks(White, maskSafeForWhite, isBlackKingThreatened);
        mobility[0] = add(mobility[0], rook[0].mob);
    }

    // Evaluate black pieces except the queen.
    if (any(p->outposts[BlackKnight])) {
      knight[1] = knights(Black, maskSafeForBlack, isWhiteKingThreatened);
      mobility[1] = add(mobility[1], knight[1].mob);
    }
    if (any(p->outposts[BlackBishop])) {
        bishop[1] = bishops(Black, maskSafeForBlack, isWhiteKingThreatened);
        mobility[1] = add(mobility[1], bishop[1].mob);
    }
    if (any(p->outposts[BlackRook])) {
        rook[1] = rooks(Black, maskSafeForBlack, isWhiteKingThreatened);
        mobility[1] = add(mobility[1], rook[1].mob);
    }

    // Now that we've built all attack bitmasks we can adjust mobility to
    // exclude attacks by enemy's knights, bishops, and rooks and evaluate
    // the queens.
    if (any(p->outposts[Queen])) {
        maskSafeForWhite &= ~(eval.attacks[BlackKnight] | eval.attacks[BlackBishop] | eval.attacks[BlackRook]);
        queen[0] = queens(White, maskSafeForWhite, isBlackKingThreatened);
        mobility[0] = add(mobility[0], queen[0].mob);
    }
    if (any(p->outposts[BlackQueen])) {
        maskSafeForBlack &= ~(eval.attacks[Knight] | eval.attacks[Bishop] | eval.attacks[Rook]);
        queen[1] = queens(Black, maskSafeForBlack, isWhiteKingThreatened);
        mobility[1] = add(mobility[1], queen[1].mob);
    }

    // Update attack bitmasks for both sides.
    eval.attacks[White] |= eval.attacks[Knight] | eval.attacks[Bishop] | eval.attacks[Rook] | eval.attacks[Queen];
    eval.attacks[Black] |= eval.attacks[BlackKnight] | eval.attacks[BlackBishop] | eval.attacks[BlackRook] | eval.attacks[BlackQueen];

    // Apply weights to the mobility scores.
    // mobility.white.apply(weights[0])
    // mobility.black.apply(weights[0])

    // Update cumulative score based on white vs. black bonuses and mobility.
    eval.score = add(eval.score, add(knight[0].val, add(bishop[0].val, add(rook[0].val, add(queen[0].val, mobility[0])))));
    eval.score = sub(eval.score, add(knight[1].val, add(bishop[1].val, add(rook[1].val, add(queen[1].val, mobility[1])))));
}
