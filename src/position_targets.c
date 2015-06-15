// Copyright (c) 2014-2015 by Michael Dvorkin. All Rights Reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.


// Returns a bitmask of possible Bishop moves from the given square wherees
// other pieces on the board are represented by the explicit parameter.
Bitmask bishop_moves_at(Position *self, int square, Bitmask board) {
    uint64 magic = ((bishopMagic[square].mask & board) * bishopMagic[square].magic) >> 55;
    return bishopMagicMoves[square][magic];
}

// Returns a bitmask of possible Rook moves from the given square wherees other
// pieces on the board are represented by the explicit parameter.
Bitmask rook_moves_at(Position *self, int square, Bitmask board) {
    uint64 magic = ((rookMagic[square].mask & board) * rookMagic[square].magic) >> 52;
    return rookMagicMoves[square][magic];
}

// Returns a bitmask of possible Bishop moves from the given square.
Bitmask bishop_moves(Position *self, int square) {
    return bishop_moves_at(self, square, self->board);
}

// Returns a bitmask of possible Rook moves from the given square.
Bitmask rook_moves(Position *self, int square) {
    return rook_moves_at(self, square, self->board);
}

Bitmask attacks_for(Position *self, int square, Piece piece) {
    int kind = kind(piece), color = color(piece);
    
    switch (kind) {
    case Pawn:
        return pawnMoves[color][square];
    case Knight:
        return knightMoves[square];
    case Bishop:
        return bishop_moves(self, square);
    case Rook:
        return rook_moves(self, square);
    case Queen:
        return bishop_moves(self, square) | rook_moves(self, square);
    case King:
        return kingMoves[square];
    }
    return (Bitmask)0;
}

Bitmask attacks(Position *self, int square) {
    return attacks_for(self, square, self->pieces[square]);
}

Bitmask targets_for(Position *self, int square, Piece piece) {
    Bitmask bitmask = (Bitmask)0;
    uint8 color = color(piece);

    if is_pawn(piece) {
        // Start with one square push, then try the second square.
        Bitmask empty = ~self->board;
        if (color == White) {
            bitmask |= (bit[square] << 8) & empty;
            bitmask |= (bitmask << 8) & empty & maskRank[3];
        } else {
            bitmask |= (bit[square] >> 8) & empty;
            bitmask |= (bitmask >> 8) & empty & maskRank[4];
        }
        bitmask |= pawnMoves[color][square] & self->outposts[color ^ 1];

        // If the last move set the en-passant square and it is diagonally adjacent
        // to the current pawn, then add en-passant to the pawn's attack targets.
        if (self->enpassant != 0 && on(maskPawn[color][self->enpassant], square)) {
            bitmask |= bit[self->enpassant];
        }
    } else {
        bitmask = attacks_for(self, square, piece) & ~self->outposts[color];
    }
    return bitmask;
}

Bitmask targets(Position *self, int square) {
    return targets_for(self, square, self->pieces[square]);
}

Bitmask xray_attacks_for(Position *self, int square, Piece piece) {
    int kind = kind(piece), color = color(piece);
    Bitmask board = (Bitmask)0;

    switch (kind) {
    case Bishop:
        board = self->board ^ self->outposts[queen(color)];
        return bishop_moves_at(self, square, board);
    case Rook:
        board = self->board ^ self->outposts[rook(color)] ^ self->outposts[queen(color)];
        return rook_moves_at(self, square, board);
    }
    return attacks_for(self, square, piece);
}

Bitmask xray_attacks(Position *self, int square) {
    return xray_attacks_for(self, square, self->pieces[square]);
}

Bitmask pawn_attacks(Position *self, uint8 color) {
    Bitmask bitmask = (Bitmask)0;

    if (color == White) {
        bitmask = (self->outposts[Pawn] & ~maskFile[0]) << 7;
        bitmask |= (self->outposts[Pawn] & ~maskFile[7]) << 9;
    } else {
        bitmask = (self->outposts[BlackPawn] & ~maskFile[0]) >> 9;
        bitmask |= (self->outposts[BlackPawn] & ~maskFile[7]) >> 7;
    }

    return bitmask;
}

Bitmask knight_attacks(Position *self, uint8 color) {
    Bitmask bitmask = (Bitmask)0;
    Bitmask outposts = self->outposts[knight(color)];

    while (any(outposts)) {
        bitmask |= knightMoves[pop(&outposts)];
    }

    return bitmask;
}

Bitmask bishop_attacks(Position *self, uint8 color) {
    Bitmask bitmask = (Bitmask)0;
    Bitmask outposts = self->outposts[bishop(color)];

    while (any(outposts)) {
        bitmask |= bishop_moves(self, pop(&outposts));
    }

    return bitmask;
}

Bitmask rook_attacks(Position *self, uint8 color) {
    Bitmask bitmask = (Bitmask)0;
    Bitmask outposts = self->outposts[rook(color)];

    while (any(outposts)) {
        bitmask |= rook_moves(self, pop(&outposts));
    }

    return bitmask;
}

Bitmask queen_attacks(Position *self, uint8 color) {
    Bitmask bitmask = (Bitmask)0;
    Bitmask outposts = self->outposts[queen(color)];

    while (any(outposts)) {
        int square = pop(&outposts);
        bitmask |= rook_moves(self, square) | bishop_moves(self, square);
    }

    return bitmask;
}

Bitmask king_attacks(Position *self, uint8 color) {
    return kingMoves[self->king[color]];
}

Bitmask all_attacks(Position *self, uint8 color) {
    Bitmask bitmask = pawn_attacks(self, color) | knight_attacks(self, color) | king_attacks(self, color);

    Bitmask outposts = self->outposts[bishop(color)] | self->outposts[queen(color)];
    while (any(outposts)) {
        bitmask |= bishop_moves(self, pop(&outposts));
    }

    outposts = self->outposts[rook(color)] | self->outposts[queen(color)];
    while (any(outposts)) {
        bitmask |= rook_moves(self, pop(&outposts));
    }

    return bitmask;
}

// Returns a bitmask of pieces that attack given square. The resulting bitmask
// only counts pieces of requested color.
//
// This method is used in static exchange evaluation so instead of using current
// board bitmask (self->board) we pass the one that gets continuously updated during
// the evaluation.
Bitmask attackers(Position *self, uint8 color, int square, Bitmask board) {
    Bitmask attackers  = knightMoves[square] & self->outposts[knight(color)];
    attackers |= maskPawn[color][square] & self->outposts[pawn(color)];
    attackers |= kingMoves[square] & self->outposts[king(color)];
    attackers |= rook_moves_at(self, square, board) & (self->outposts[rook(color)] | self->outposts[queen(color)]);
    attackers |= bishop_moves_at(self, square, board) & (self->outposts[bishop(color)] | self->outposts[queen(color)]);
    return attackers;
}

bool is_attacked(Position *self, uint8 color, int square) {
    return any(knightMoves[square] & self->outposts[knight(color)]) ||
           any(maskPawn[color][square] & self->outposts[pawn(color)]) ||
           any(kingMoves[square] & self->outposts[king(color)]) ||
           any(rook_moves(self, square) & (self->outposts[rook(color)] | self->outposts[queen(color)])) ||
           any(bishop_moves(self, square) & (self->outposts[bishop(color)] | self->outposts[queen(color)]));
}

Piece strongest_piece(Position *self, uint8 color, Bitmask targets) {
    if any(targets & self->outposts[queen(color)]) {
        return queen(color);
    }
    if any(targets & self->outposts[rook(color)]) {
        return rook(color);
    }
    if any(targets & self->outposts[bishop(color)]) {
        return bishop(color);
    }
    if any(targets & self->outposts[knight(color)]) {
        return knight(color);
    }
    if any(targets & self->outposts[pawn(color)]) {
        return pawn(color);
    }
    return (Piece)0;
}


bool oppositeBishops(Position *self) {
    Bitmask bishops = self->outposts[Bishop] | self->outposts[BlackBishop];

    return (bishops & maskDark) && (bishops & ~maskDark);
}
