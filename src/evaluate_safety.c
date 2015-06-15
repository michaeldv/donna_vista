// Copyright (c) 2014-2015 by Michael Dvorkin. All Rights Reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.

//------------------------------------------------------------------------------
int king_cover_bonus(uint8 color, int square, int flipped) {
    int row = ROW(flipped), col = COL(flipped);
    int from = max(0, col - 1);
    int to = min(7, col + 1);
    int bonus = onePawn + onePawn / 3;

    // Get friendly pawns adjacent and in front of the king.
    Bitmask adjacent = maskIsolated[col] & maskRank[ROW(square)];
    Bitmask pawns = eval.position->outposts[pawn(color)] & (adjacent | maskPassed[color][square]);

    // For each of the cover files find the closest friendly pawn. The penalty
    // is carried if the pawn is missing or is too far from the king (more than
    // one rank apart).
    for (int c = from; c <= to; c++) {
        Bitmask cover = (pawns & maskFile[c]);
        if (cover) {
            int nearby = RANK(color, closest(cover, color));
            bonus -= penaltyCover[nearby - row];
        } else {
            bonus -= coverMissing.midgame;
        }
    }

    return bonus;
}

// Calculates endgame penalty to encourage a king stay closer to friendly pawns.
//------------------------------------------------------------------------------
int king_pawn_proximity(uint8 color) {
    int penalty = 0;
    Bitmask pawns = eval.position->outposts[pawn(color)];

    if (pawns && empty(pawns & eval.attacks[king(color)])) {
        int proximity = 8;
        int king = eval.position->king[color];

        while (any(pawns)) {
            int sq = pop(&pawns);
            proximity = min(proximity, distance[king][sq]);
            // proximity = min(proximity, distance[king][pop(&pawns)]);
        }
        penalty = -kingByPawn.endgame * (proximity - 1);
    }

    return penalty;
}

//------------------------------------------------------------------------------
Score king_cover_score(uint8 color) {
    Score score = {};
    Position *p = eval.position;    
    int square = (int)p->king[color];

    // Calculate relative square for the king so we could treat black king
    // as white. Don't bother with the cover if the king is too far.
    int flipped = flip(color^1, square);
    if (flipped > H3) {
        return score;
    }

    // If we still have castle rights encourage castle pawns to stay intact
    // by scoring least safe castle.
    score.midgame = king_cover_bonus(color, square, flipped);
    if (p->castles & castleKingside[color]) {
        score.midgame = max(score.midgame, king_cover_bonus(color, homeKing[color] + 2, G1));
    }
    if (p->castles & castleQueenside[color]) {
        score.midgame = max(score.midgame, king_cover_bonus(color, homeKing[color] - 2, C1));
    }

    return score;
}

//------------------------------------------------------------------------------
Score king_safety_score(uint8 color) {
    Score score = {};
    Position *p = eval.position;

    if (eval.safety[color].threats > 0) {
        int square = (int)p->king[color];
        int safetyIndex = 0;

        // Find squares around the king that are being attacked by the
        // enemy and defended by our king only.
        Bitmask defended = eval.attacks[pawn(color)] | eval.attacks[knight(color)] |
                           eval.attacks[bishop(color)] | eval.attacks[rook(color)] |
                           eval.attacks[queen(color)];
        Bitmask weak = eval.attacks[king(color)] & eval.attacks[color^1] & ~defended;

        // Find possible queen checks on weak squares around the king.
        // We only consider squares where the queen is protected and
        // can't be captured by the king.
        Bitmask protected = eval.attacks[pawn(color^1)] | eval.attacks[knight(color^1)] |
                            eval.attacks[bishop(color^1)] | eval.attacks[rook(color^1)] |
                            eval.attacks[king(color^1)];
        Bitmask checks = weak & eval.attacks[queen(color^1)] & protected & ~p->outposts[color^1];
        if (checks) {
            safetyIndex += bonusCloseCheck[Queen / 2] * count(checks);
        }

        // Find possible rook checks within king's home zone. Unlike
        // queen we must only consider squares where the rook actually
        // gives a check.
        protected = eval.attacks[pawn(color^1)] | eval.attacks[knight(color^1)] |
                    eval.attacks[bishop(color^1)] | eval.attacks[queen(color^1)] |
                    eval.attacks[king(color^1)];
        checks = weak & eval.attacks[rook(color^1)] & protected & ~p->outposts[color^1];
        checks &= rookMagicMoves[square][0];
        if (checks) {
            safetyIndex += bonusCloseCheck[Rook / 2] * count(checks);
        }

        // Double safety index if the enemy has right to move.
        if (p->color == (color^1)) {
            safetyIndex *= 2;
        }

        // Out of all squares available for enemy pieces select the ones
        // that are not under our attack.
        Bitmask safe = ~(eval.attacks[color] | p->outposts[color^1]);

        // Are there any safe squares from where enemy Knight could give
        // us a check?
        checks = knightMoves[square] & safe & eval.attacks[knight(color^1)];
        if (checks) {
            safetyIndex += bonusDistanceCheck[Knight / 2] * count(checks);
        }

        // Are there any safe squares from where enemy Bishop could give
        // us a check?
        Bitmask safeBishopMoves = bishop_moves(p, square) & safe;
        checks = safeBishopMoves & eval.attacks[bishop(color^1)];
        if (checks) {
            safetyIndex += bonusDistanceCheck[Bishop / 2] * count(checks);
        }

        // Are there any safe squares from where enemy Rook could give
        // us a check?
        Bitmask safeRookMoves = rook_moves(p, square) & safe;
        checks = safeRookMoves & eval.attacks[rook(color^1)];
        if (checks) {
            safetyIndex += bonusDistanceCheck[Rook / 2] * count(checks);
        }

        // Are there any safe squares from where enemy Queen could give
        // us a check?
        checks = (safeBishopMoves | safeRookMoves) & eval.attacks[queen(color^1)];
        if (checks) {
            safetyIndex += bonusDistanceCheck[Queen / 2] * count(checks);
        }

        int threatIndex = min(12, eval.safety[color].attackers * eval.safety[color].threats / 3) +
                          (eval.safety[color].attacks + count(weak)) * 2;
        safetyIndex = min(63, safetyIndex + threatIndex);

        score.midgame -= kingSafety[safetyIndex];
    }

    return score;
}

//------------------------------------------------------------------------------
void analyzeSafety() {
    Total cover = {}, safety = {};
    Position *p = eval.position;
    bool opposite = oppositeBishops(p);

    // Any pawn move invalidates king's square in the pawns hash so that we
    // could detect it here.
    bool whiteKingMoved = (p->king[White] != eval.pawns->king[White]);
    bool blackKingMoved = (p->king[Black] != eval.pawns->king[Black]);

    // If the king has moved then recalculate king/pawn proximity and update
    // cover score and king square in the pawn cache.
    if (whiteKingMoved) {
        eval.pawns->cover[White] = king_cover_score(White);
        eval.pawns->cover[White].endgame += king_pawn_proximity(White);
        eval.pawns->king[White] = p->king[White];
    }
    if (blackKingMoved) {
        eval.pawns->cover[Black] = king_cover_score(Black);
        eval.pawns->cover[Black].endgame += king_pawn_proximity(Black);
        eval.pawns->king[Black] = p->king[Black];
    }

    // Fetch king cover score from the pawn cache.
    cover.white = add(cover.white, eval.pawns->cover[White]);
    cover.black = add(cover.black, eval.pawns->cover[Black]);

    // Compute king's safety for both sides.
    safety.white = king_safety_score(White);
    if (opposite && (eval.material->flags & whiteKingSafety) && (safety.white.midgame < -onePawn / 10 * 8)) {
        safety.white.midgame -= bishopDanger.midgame;
    }
    safety.black = king_safety_score(Black);
    if (opposite && (eval.material->flags & blackKingSafety) && (safety.black.midgame < -onePawn / 10 * 8)) {
        safety.black.midgame -= bishopDanger.midgame;
    }

    // Apply weights by mapping Black to our king safety index [3], and White
    // to enemy's king safety index [4].
    // cover.white.apply(weights[3+color])
    // cover.black.apply(weights[4-color])
    // safety.white.apply(weights[3+color])
    // safety.black.apply(weights[4-color])
    eval.score = add(eval.score, add(sub(cover.white, cover.black), sub(safety.white, safety.black)));
}
