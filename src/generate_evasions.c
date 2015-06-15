// Copyright (c) 2014-2015 by Michael Dvorkin. All Rights Reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.


//------------------------------------------------------------------------------
MoveGen *generate_evasions(MoveGen *self) {
    Position *p = self->p;
    int color = p->color;
    int enemy = p->color ^ 1;
    int square = (int)p->king[color];

    // Find out what pieces are checking the king. Usually it's a single
    // piece but double check is also a possibility.
    Bitmask checkers = maskPawn[enemy][square] & p->outposts[pawn(enemy)];
    checkers |= targets_for(p, square, knight(color)) & p->outposts[knight(enemy)];
    checkers |= targets_for(p, square, bishop(color)) & (p->outposts[bishop(enemy)] | p->outposts[queen(enemy)]);
    checkers |= targets_for(p, square, rook(color)) & (p->outposts[rook(enemy)] | p->outposts[queen(enemy)]);

    // Generate possible king retreats first, i.e. moves to squares not
    // occupied by friendly pieces and not attacked by the opponent.
    Bitmask retreats = targets(p, square) & ~all_attacks(p, enemy);

    // If the attacking piece is bishop, rook, or queen then exclude the
    // square behind the king using evasion mask. Note that knight's
    // evasion mask is full board so we only check if the attacking piece
    // is not a pawn.
    int attackSquare = pop(&checkers);
    if (p->pieces[attackSquare] != pawn(enemy)) {
        retreats &= maskEvade[square][attackSquare];
    }

    // If checkers mask is not empty then we've got double check and
    // retreat is the only option.
    if (any(checkers)) {
        attackSquare = first(checkers);
        if (p->pieces[attackSquare] != pawn(enemy)) {
            retreats &= maskEvade[square][attackSquare];
        }
        return add_piece_move(self, square, retreats);
    }

    // Generate king retreats. Since castle is not an option there is no
    // reason to use moveKing().
    add_piece_move(self, square, retreats);

    // Pawn captures: do we have any pawns available that could capture
    // the attacking piece?
    Bitmask pawns = maskPawn[color][attackSquare] & p->outposts[pawn(color)];
    while (any(pawns)) {
        Move move = new_move(p, pop(&pawns), attackSquare);
        if (attackSquare >= A8 || attackSquare <= H1) {
            move = promote(move, Queen);
        }
        append(self, move);
    }

    // Rare case when the check could be avoided by en-passant capture.
    // For example: Ke4, c5, e5 vs. Ke8, d7. Black's d7-d5+ could be
    // evaded by c5xd6 or e5xd6 en-passant captures.
    if (p->enpassant != 0) {
        int enpassant = attackSquare + eight[color];
        if (enpassant == (int)p->enpassant) {
            pawns = maskPawn[color][enpassant] & p->outposts[pawn(color)];
            while (any(pawns)) {
                append(self, new_move(p, pop(&pawns), enpassant));
            }
        }
    }

    // See if the check could be blocked or the attacked piece captured.
    Bitmask block = maskBlock[square][attackSquare] | bit[attackSquare];

    // Create masks for one-square pawn pushes and two-square jumps.
    Bitmask jumps = ~p->board;
    if (color == White) {
        pawns = (p->outposts[Pawn] << 8) & ~p->board;
        jumps &= maskRank[3] & (pawns << 8);
    } else {
        pawns = (p->outposts[BlackPawn] >> 8) & ~p->board;
        jumps &= maskRank[4] & (pawns >> 8);
    }
    pawns &= block; jumps &= block;

    // Handle one-square pawn pushes: promote to Queen if reached last rank.
    while (any(pawns)) {
        int to = pop(&pawns);
        int from = to - eight[color];
        Move move = new_move(p, from, to); // Can't cause en-passant.
        if (to >= A8 || to <= H1) {
            move = promote(move, Queen);
        }
        append(self, move);
    }

    // Handle two-square pawn jumps that can cause en-passant.
    while (any(jumps)) {
        int to = pop(&jumps);
        int from = to - 2 * eight[color];
        append(self, new_pawn_move(p, from, to));
    }

    // What's left is to generate all possible knight, bishop, rook, and
    // queen moves that evade the check.
    Bitmask outposts = p->outposts[color] & ~p->outposts[pawn(color)] & ~p->outposts[king(color)];
    while (any(outposts)) {
        int from = pop(&outposts);
        Bitmask mask = targets(p, from) & block;
        add_piece_move(self, from, mask);
    }

    return self;
}
