// Copyright (c) 2014-2015 by Michael Dvorkin. All Rights Reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.

//------------------------------------------------------------------------------
Position *move_piece(Position *self, Piece piece, int from, int to) {
    self->pieces[from] = (Piece)0;
    self->pieces[to] = piece;
    self->outposts[piece] ^= (bit[from] | bit[to]);
    self->outposts[color(piece)] ^= (bit[from] | bit[to]);

    // Update position's hash values.
    uint64 random = poly(piece, from) ^ poly(piece, to);
    self->id ^= random;
    if (is_pawn(piece)) {
        self->pawnId ^= random;
    }

    // Update positional score.
    self->tally = add(sub(self->tally, pst[piece][from]), pst[piece][to]);

    return self;
}

//------------------------------------------------------------------------------
Position *promote_pawn(Position *self, Piece pawn, int from, int to, Piece promo) {
    self->pieces[from] = (Piece)0;
    self->pieces[to] = promo;
    self->outposts[pawn] ^= bit[from];
    self->outposts[promo] ^= bit[to];
    self->outposts[color(pawn)] ^= (bit[from] | bit[to]);

    // Update position's hash values and material balance.
    uint64 random = poly(pawn, from);
    self->id ^= (random ^ poly(promo, to));
    self->pawnId ^= random;
    self->balance += materialBalance[promo] - materialBalance[pawn];

    // Update positional score.
    self->tally = add(sub(self->tally, pst[pawn][from]), pst[promo][to]);

    return self;
}

//------------------------------------------------------------------------------
Position *capture_piece(Position *self, Piece capture, int from, int to) {
    self->outposts[capture] ^= bit[to];
    self->outposts[color(capture)] ^= bit[to];

    // Update position's hash values and material balance.
    uint64 random = poly(capture, to);
    self->id ^= random;
    if (is_pawn(capture)) {
        self->pawnId ^= random;
    }
    self->balance -= materialBalance[capture];

    // Update positional score.
    self->tally = sub(self->tally, pst[capture][to]);

    return self;
}

//------------------------------------------------------------------------------
Position *capture_enpassant(Position *self, Piece capture, int from, int to) {
    int enpassant = to - eight[color(capture) ^ 1];

    self->pieces[enpassant] = 0;
    self->outposts[capture] ^= bit[enpassant];
    self->outposts[color(capture)] ^= bit[enpassant];

    // Update position's hash values and material balance.
    uint64 random = poly(capture, enpassant);
    self->id ^= random;
    self->pawnId ^= random;
    self->balance -= materialBalance[capture];

    // Update positional score.
    self->tally = sub(self->tally, pst[capture][enpassant]);

    return self;
}

//------------------------------------------------------------------------------
Position *make_move(Position *self, Move move) {
    int from = from(move), to = to(move);
    Piece piece = piece(move), capture = capture(move), promo = promo(move);
    uint8 color = color(piece);

    // Copy over the contents of previous tree node to the current one.
    node++;
    tree[node] = *self; // => tree[node] = tree[node - 1]
    Position *p = &tree[node];

    p->enpassant = 0;
    p->reversible = true;

    if (capture != (Piece)0) {
        p->reversible = false;
        if (to != 0 && to == (int)self->enpassant) {
            capture_enpassant(p, pawn(color ^ 1), from, to);
            p->id ^= hashEnpassant[self->enpassant & 7]; // self->enpassant column.
        } else {
            capture_piece(p, capture, from, to);
        }
    }

    if (promo == (Piece)0) {
        move_piece(p, piece, from, to);

        if (is_king(piece)) {
            p->king[color] = (uint8)to;
            if (is_castle(move)) {
                p->reversible = false;
                switch (to) {
                case G1:
                    move_piece(p, Rook, H1, F1);
                    break;
                case C1:
                    move_piece(p, Rook, A1, D1);
                    break;
                case G8:
                    move_piece(p, BlackRook, H8, F8);
                    break;
                case C8:
                    move_piece(p, BlackRook, A8, D8);
                    break;
                }
            }
        } else if (is_pawn(piece)) {
            p->reversible = false;
            if (is_enpassant(move)) {
                p->enpassant = (uint8)(from + eight[color]); // Save the en-passant square.
                p->id ^= hashEnpassant[p->enpassant & 7];
            }
        }
    } else {
        p->reversible = false;
        promote_pawn(p, piece, from, to, promo);
    }

    // Set up the board bitmask, update castle rights, finish off incremental
    // hash value, and flip the color.
    p->board = p->outposts[White] | p->outposts[Black];
    p->castles &= (castleRights[from] & castleRights[to]);
    p->id ^= (hashCastle[self->castles] ^ hashCastle[p->castles]);
    p->id ^= polyglotRandomWhite;
    p->color ^= 1; // <-- Flip side to move.

    return &tree[node]; // p
}

// Makes "null" move by copying over previous node position (i.e. preserving all pieces
// intact) and flipping the color.
Position *make_null_move(Position *self) {
    node++;
    tree[node] = *self; // => tree[node] = tree[node - 1]
    Position *p = &tree[node];

    // Flipping side to move obviously invalidates the enpassant square.
    if (p->enpassant) {
        p->id ^= hashEnpassant[p->enpassant & 7];
        p->enpassant = 0;
    }
    p->id ^= polyglotRandomWhite;
    p->color ^= 1; // <-- Flip side to move.

    return &tree[node]; // p
}


// Restores previous position effectively taking back the last move made.
//------------------------------------------------------------------------------
Position *undo_last_move(Position *self) {
    if (node > 0) {
        node--;
    }

    return &tree[node];
}

//------------------------------------------------------------------------------
Position *undo_null_move(Position *self) {
    self->id ^= polyglotRandomWhite;
    self->color ^= 1;

    return undo_last_move(self);
}

//------------------------------------------------------------------------------
bool is_in_check(Position *self, uint8 color) {
    return is_attacked(self, color ^ 1, (int)self->king[color]);
}

//------------------------------------------------------------------------------
bool is_null(Position *self) {
    return node > 0 && tree[node].board == tree[node - 1].board;
}

//------------------------------------------------------------------------------
bool fifty(Position *self) {
    if (node < 100) {
        return false;
    }

    int count = 0;

    for (int previous = node - 1; previous >= 0 && count < 100; previous--) {
        if (!tree[previous].reversible) {
            break;
        }
        count++;
    }

    return count >= 100;
}

//------------------------------------------------------------------------------
bool repetition(Position *self) {
    if (!self->reversible || node < 1) {
        return false;
    }

    for (int previous = node - 1; previous >= 0; previous--) {
        if (!tree[previous].reversible) {
            return false;
        }
        if (tree[previous].id == self->id) {
            return true;
        }
    }

    return false;
}

//------------------------------------------------------------------------------
bool third_repetition(Position *self) {
    if (!self->reversible || node < 4) {
        return false;
    }

    int repetitions = 1;

    for (int previous = node - 2; previous >= 0; previous -= 2) {
        if (!tree[previous].reversible || !tree[previous + 1].reversible) {
            return false;
        }
        if (tree[previous].id == self->id) {
            repetitions++;
            if (repetitions == 3) {
                return true;
            }
        }
    }

    return false;
}


//------------------------------------------------------------------------------
void can_castle(Position *self, uint8 color, bool *kingside, bool *queenside) {
    // Start off with simple checks.
    *kingside = ((self->castles & castleKingside[color]) != 0) && ((gapKing[color] & self->board) == 0);
    *queenside = ((self->castles & castleQueenside[color]) != 0) && ((gapQueen[color] & self->board) == 0);

    // If it still looks like the castles are possible perform more expensive
    // final check.
    if (*kingside || *queenside) {
        Bitmask attacks = all_attacks(self, color ^ 1);
        *kingside = *kingside && empty(castleKing[color] & attacks);
        *queenside = *queenside && empty(castleQueen[color] & attacks);
    }
}

// Returns true if (*non-evasion* move is valid, i.e. it is possible to make
// the move in current position without violating chess rules. If the king is
// in check the generator is expected to generate valid evasions where extra
// validation is not needed.
//------------------------------------------------------------------------------
bool is_move_valid(Position *self, Move move, Bitmask pins) {
    int from = from(move), to = to(move);
    Piece piece = piece(move), capture = capture(move);
    uint8 color = color(piece);

    // For rare en-passant pawn captures we validate the move by actually
    // making it, and then taking it back.
    if (self->enpassant != 0 && to == (int)self->enpassant && is_pawn(capture)) {
        Position *position = make_move(self, move);
        bool inCheck = is_in_check(position, color);
        undo_last_move(position);
        return !inCheck;
    }

    // King's move is valid when a) the move is a castle or b) the destination
    // square is not being attacked by the opponent.
    if (is_king(piece)) {
        return is_castle(move) || !is_attacked(self, color ^ 1, to);
    }

    // For all other peices the move is valid when it doesn't cause a
    // check. For pinned sliders this includes moves along the pinning
    // file, rank, or diagonal.
    bool between = on(maskStraight[from][to] | maskDiagonal[from][to], (int)self->king[color]);
    
    return pins == 0 || off(pins, from) || between;
}

// Returns a bitmask of all pinned pieces preventing a check for the king on
// given square. The color of the pieces match the color of the king.
//------------------------------------------------------------------------------
Bitmask pinned_mask(Position *self, uint8 square) {
    Bitmask mask = (Bitmask)0;
    uint8 color = color(self->pieces[square]);
    uint8 enemy = color ^ 1;

    Bitmask attackers = (self->outposts[bishop(enemy)] | self->outposts[queen(enemy)]) & bishopMagicMoves[square][0];
    attackers |= (self->outposts[rook(enemy)] | self->outposts[queen(enemy)]) & rookMagicMoves[square][0];

    while (any(attackers)) {
        int attackSquare = pop(&attackers);
        Bitmask blockers = maskBlock[square][attackSquare] & ~bit[attackSquare] & self->board;

        if (count(blockers) == 1) {
            mask |= blockers & self->outposts[color]; // Only friendly pieces are pinned.
        }
    }
    return mask;
}
