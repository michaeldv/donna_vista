// Copyright (c) 2014-2015 by Michael Dvorkin. All Rights Reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.


// Non-capturing checks.
//------------------------------------------------------------------------------
MoveGen *generate_checks(MoveGen *self) {
    Position *p = self->p;
    int color = p->color, enemy = p->color ^ 1;
    int square = (int)p->king[enemy];

    // Non-capturing Knight checks.
    Bitmask prohibit = maskNone;
    Bitmask checks = knightMoves[square];
    Bitmask outposts = p->outposts[knight(color)];
    while (any(outposts)) {
        int from = pop(&outposts);
        add_piece_move(self, from, knightMoves[from] & checks & ~p->board);
    }

    // Non-capturing Bishop or Queen checks.
    checks = targets_for(p, square, bishop(enemy));
    outposts = p->outposts[bishop(color)] | p->outposts[queen(color)];
    while (any(outposts)) {
        int from = pop(&outposts);
        Bitmask squares = targets_for(p, from, bishop(enemy)) & checks & ~p->outposts[enemy];
        while (any(squares)) {
            int to = pop(&squares);
            Piece piece = p->pieces[to];
            if (!piece) {
                // Empty square: simply move a bishop to check.
                append(self, new_move(p, from, to));
            } else if (color(piece) == color && any(maskDiagonal[from][square])) {
                // Non-empty square occupied by friendly piece on the same
                // diagonal: moving the piece away causes discovered check.
                switch (kind(piece)) {
                case Pawn:
                    // Block pawn promotions (since they are treated as
                    // captures) and en-passant captures.
                    prohibit = maskRank[0] | maskRank[7];
                    if (p->enpassant) {
                        prohibit |= bit[p->enpassant];
                    }
                    add_pawn_move(self, to, targets(p, to) & ~p->board & ~prohibit);
                    break;
                case King:
                    // Make sure the king steps out of attack diaginal.
                    add_king_move(self, to, targets(p, to) & ~p->board & ~maskBlock[from][square]);
                    break;
                default:
                    add_piece_move(self, to, targets(p, to) & ~p->board);
                }
            }
        }
        if (is_queen(p->pieces[from])) {
            // Queen could move straight as a rook and check diagonally as a bishop
            // or move diagonally as a bishop and check straight as a rook.
            Bitmask squares = (targets_for(p, from, rook(color)) & checks) |
                  (targets_for(p, from, bishop(color)) & targets_for(p, square, rook(color)));
            add_piece_move(self, from, squares & ~p->board);
        }
    }

    // Non-capturing Rook or Queen checks.
    checks = targets_for(p, square, rook(enemy));
    outposts = p->outposts[rook(color)] | p->outposts[queen(color)];
    while (any(outposts)) {
        int from = pop(&outposts);
        Bitmask squares = targets_for(p, from, rook(enemy)) & checks & ~p->outposts[enemy];
        while (any(squares)) {
            int to = pop(&squares);
            Piece piece = p->pieces[to];
            if (!piece) {
                // Empty square: simply move a rook to check.
                append(self, new_move(p, from, to));
            } else if (color(piece) == color) {
                if (any(maskStraight[from][square])) {
                    // Non-empty square occupied by friendly piece on the same
                    // file or rank: moving the piece away causes discovered check.
                    switch (kind(piece)) {
                    case Pawn:
                        // If pawn and rook share the same file then non-capturing
                        // discovered check is not possible since the pawn is going
                        // to stay on the same file no matter what.
                        if (col(from) == col(to)) {
                            continue;
                        }
                        // Block pawn promotions (since they are treated as captures)
                        // and en-passant captures.
                        prohibit = maskRank[0] | maskRank[7];
                        if (p->enpassant) {
                            prohibit |= bit[p->enpassant];
                        }
                        add_pawn_move(self, to, targets(p, to) & ~p->board & ~prohibit);
                        break;
                    case King:
                        // Make sure the king steps out of attack file or rank.
                        prohibit = maskNone;
                        if (row(from) == row(square)) {
                            prohibit = maskRank[row(from)];
                        } else {
                            prohibit = maskFile[col(square)];
                        }
                        add_king_move(self, to, targets(p, to) & ~p->board & ~prohibit);
                        break;
                    default:
                        add_piece_move(self, to, targets(p, to) & ~p->board);
                    }
                }
            }
        }
    }

    // Non-capturing Pawn checks.
    outposts = p->outposts[pawn(color)] & maskIsolated[col(square)];
    while (any(outposts)) {
        int from = pop(&outposts);
        Bitmask target = maskPawn[color][square] & targets(p, from);
        if (any(target)) {
            append(self, new_pawn_move(p, from, pop(&target)));
        }
    }

    return self;
}
