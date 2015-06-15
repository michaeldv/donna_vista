// Copyright (c) 2014-2015 by Michael Dvorkin. All Rights Reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.


// Calculates extra bonus and penalty based on pawn structure. Specifically,
// a bonus is awarded for passed pawns, and penalty applied for isolated and
// doubled pawns.
//------------------------------------------------------------------------------
Score pawnStructure(uint8 color) {
    Score score = (Score){};
    Bitmask hisPawns = eval.position->outposts[pawn(color)];
    Bitmask herPawns = eval.position->outposts[pawn(color^1)];
    eval.pawns->passers[color] = maskNone;

    Bitmask pawns = hisPawns;
    while (any(pawns)) {
        int square = pop(&pawns);
        int row = ROW(square);
        int col = COL(square);

        // Penalty if the pawn is isolated, i.e. has no friendly pawns
        // on adjacent files. The penalty goes up if isolated pawn is
        // exposed on semi-open file.
        bool isolated = empty(maskIsolated[col] & hisPawns);
        bool exposed = empty(maskInFront[color][square] & herPawns);
        if (isolated) {
            if (exposed) {
                score = sub(score, penaltyWeakIsolatedPawn[col]);
            } else {
                score = sub(score, penaltyIsolatedPawn[col]);
            }
        }

        // Penalty if the pawn is doubled, i.e. there is another friendly
        // pawn in front of us. The penalty goes up if doubled pawns are
        // isolated.
        bool doubled = any(maskInFront[color][square] & hisPawns);
        if (doubled) {
            score = sub(score, penaltyDoubledPawn[col]);
        }

        // Bonus if the pawn is supported by friendly pawn(s) on the same
        // or previous ranks.
        bool supported = any(maskIsolated[col] & (maskRank[row] | pushed(maskRank[row], color^1)) & hisPawns);
        if (supported) {
            int f = flip(color, square);
            score = add(score, (Score){ bonusSupportedPawn[f], bonusSupportedPawn[f] });
        }

        // The pawn is passed if a) there are no enemy pawns in the same
        // and adjacent columns; and b) there are no same color pawns in
        // front of us.
        bool passed = empty(maskPassed[color][square] & herPawns) && !doubled;
        if (passed) {
            eval.pawns->passers[color] |= bit[square];
        }

        // Penalty if the pawn is backward.
        bool backward = false;
        if (!passed && !supported && !isolated) {

            // Backward pawn should not be attacking enemy pawns.
            if (empty(pawnMoves[color][square] & herPawns)) {

                // Backward pawn should not have friendly pawns behind.
                if (empty(maskPassed[color^1][square] & maskIsolated[col] & hisPawns)) {

                    // Backward pawn should face enemy pawns on the next two ranks
                    // preventing its advance.
                    Bitmask enemy = pushed(pawnMoves[color][square], color);
                    if (any((enemy | pushed(enemy, color)) & herPawns)) {
                        backward = true;
                        if (exposed) {
                            score = sub(score, penaltyWeakBackwardPawn[col]);
                        } else {
                            score = sub(score, penaltyBackwardPawn[col]);
                        }
                    }
                }
            }
        }

        // Bonus if the pawn has good chance to become a passed pawn.
        if (exposed && supported && !passed && !backward) {
            Bitmask his = maskPassed[color^1][square + eight[color]] & maskIsolated[col] & hisPawns;
            Bitmask her = maskPassed[color][square] & maskIsolated[col] & herPawns;
            if (count(his) >= count(her)) {
                score = add(score, bonusSemiPassedPawn[rank(color, square)]);
            }
        }
    }

    return score;
}

//------------------------------------------------------------------------------
Score pawnPassers(uint8 color) {
    Score score = (Score){};
    Position *p = eval.position;
    Bitmask pawns = eval.pawns->passers[color];

    while (any(pawns)) {
        int square = pop(&pawns);
        int rank = RANK(color, square);
        Score bonus = bonusPassedPawn[rank];

        if (rank > A2H2) {
            int extra = extraPassedPawn[rank];
            int nextSquare = square + eight[color];

            // Adjust endgame bonus based on how close the kings are from the
            // step forward square.
            bonus.endgame += (distance[p->king[color^1]][nextSquare] * 5 - distance[p->king[color]][nextSquare] * 2) * extra;

            // Check if the pawn can step forward.
            if (off(p->board, nextSquare)) {
                int boost = 0;

                // Assume all squares in front of the pawn are under attack.
                Bitmask attacked = maskInFront[color][square];
                Bitmask protected = attacked & eval.attacks[color];

                // Boost the bonus if squares in front of the pawn are protected.
                if (protected == attacked) {
                    boost += 6; // All squares.
                } else if on(protected, nextSquare) {
                    boost += 4; // Next square only.
                }

                // Check who is attacking the squares in front of the pawn including
                // queen and rook x-ray attacks from behind.
                Bitmask enemy = maskInFront[color^1][square] & (p->outposts[queen(color^1)] | p->outposts[rook(color^1)]);
                if (empty(enemy) || empty(enemy & rook_moves(p, square))) {

                    // Since nobody attacks the pawn from behind adjust the attacked
                    // bitmask to only include squares attacked or occupied by the enemy.
                    attacked &= (eval.attacks[color^1] | p->outposts[color^1]);
                }

                // Boost the bonus if passed pawn is free to advance to the 8th rank
                // or at least safely step forward.
                if (empty(attacked)) {
                    boost += 15; // Remaining squares are not under attack.
                } else if off(attacked, nextSquare) {
                    boost += 9;  // Next square is not under attack.
                }

                if (boost > 0) {
                    bonus = adjust(bonus, extra * boost);
                }
            }
        }
        score = add(score, bonus);
    }

    return score;
}


//------------------------------------------------------------------------------
void analyzePawns() {
    int64 id = eval.position->pawnId;
    int32 index = 0, cache_size = len(game.pawnCache);

    // Since pawn hash is fairly small we can use much faster 32-bit index.
    if (cache_size) {
        index = (uint32)id % cache_size;
    }
    eval.pawns = &game.pawnCache[index];

    // Bypass pawns cache if evaluation tracing is enabled.
    if (eval.pawns->id != id) {
        Score white = pawnStructure(White);
        Score black = pawnStructure(Black);
        // white.apply(weights[1]); black.apply(weights[1]) // <-- Pawn structure weight.
        eval.pawns->score = sub(white, black);
        eval.pawns->id = id;

        // Force full king shelter evaluation since any legit king square
        // will be viewed as if the king has moved.
        eval.pawns->king[White] = 0xFF;
        eval.pawns->king[Black] = 0xFF;
    }

    // return eval.pawns->score;
    eval.score = add(eval.score, eval.pawns->score);
}

//------------------------------------------------------------------------------
void analyzePassers() {
    Score white = pawnPassers(White);
    Score black = pawnPassers(Black);

    // white.apply(weights[2]); black.apply(weights[2]) // <-- Passed pawns weight.
    eval.score = add(eval.score, sub(white, black));
}
