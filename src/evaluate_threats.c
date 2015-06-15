// Copyright (c) 2014-2015 by Michael Dvorkin. All Rights Reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.

//------------------------------------------------------------------------------
Score threats(uint8 color, Bitmask hisAttacks, Bitmask herAttacks) {
    Score score = {};
    Position *p = eval.position;

    // Find weak enemy pieces: the ones under attack and not defended by
    // pawns (excluding a king).
    Bitmask weak = p->outposts[color^1] & hisAttacks & ~eval.attacks[pawn(color^1)];
    weak &= ~p->outposts[king(color^1)];

    if (weak) {

        // Threat bonus for strongest enemy piece attacked by our pawns,
        // knights, or bishops.
        Bitmask targets = weak & (eval.attacks[pawn(color)] | eval.attacks[knight(color)] | eval.attacks[bishop(color)]);
        if (targets) {
            Piece piece = strongest_piece(p, color^1, targets);
            score = add(score, bonusMinorThreat[kind(piece) / 2]);
        }

        // Threat bonus for strongest enemy piece attacked by our rooks
        // or queen.
        targets = weak & (eval.attacks[rook(color)] | eval.attacks[queen(color)]);
        if (targets) {
            Piece piece = strongest_piece(p, color^1, targets);
            score = add(score, bonusMajorThreat[kind(piece) / 2]);
        }

        // Extra bonus when attacking enemy pieces that are hanging. Side
        // having the right to move gets bigger bonus.
        int hanging = count(weak & ~herAttacks);
        if (hanging > 0) {
            if (p->color == color) {
                hanging++;
            }
            score = add(score, times(hangingAttack, hanging));
        }
    }

    return score;
}

//------------------------------------------------------------------------------
Score center(uint8 color, Bitmask hisAttacks, Bitmask herAttacks, Bitmask herPawnAttacks) {
    Score score = {};
    Bitmask pawns = eval.position->outposts[pawn(color)];
    Bitmask safe = homeTurf[color] & ~pawns & ~herPawnAttacks & (hisAttacks | ~herAttacks);
    Bitmask turf = safe & pawns;

    if (color == White) {
        turf |= turf >> 8;   // A4..H4 -> A3..H3
        turf |= turf >> 16;  // A4..H4 | A3..H3 -> A2..H2 | A1..H1
        turf &= safe;        // Keep safe squares only.
        safe <<= 32;         // Move up to black's half of the board.
    } else {
        turf |= turf << 8;   // A5..H5 -> A6..H6
        turf |= turf << 16;  // A5..H5 | A6..H6 -> A7..H7 | A8..H8
        turf &= safe;        // Keep safe squares only.
        safe >>= 32;         // Move down to white's half of the board.
    }

    score.midgame = count(safe | turf) * eval.material->turf / 100;

    return score;
}

//------------------------------------------------------------------------------
void analyzeThreats() {
    Total total = {
        threats(White, eval.attacks[White], eval.attacks[Black]),
        threats(Black, eval.attacks[Black], eval.attacks[White])
    };
    eval.score = add(eval.score, sub(total.white, total.black));

    if (eval.material->turf && (eval.material->flags & (whiteKingSafety | blackKingSafety))) {
        total.white = center(White, eval.attacks[White], eval.attacks[Black], eval.attacks[pawn(Black)]);
        total.black = center(Black, eval.attacks[Black], eval.attacks[White], eval.attacks[pawn(White)]);
        eval.score = add(eval.score, sub(total.white, total.black));
    }
}

