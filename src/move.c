// Copyright (c) 2014-2015 by Michael Dvorkin. All Rights Reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.

#define xCapture   0x00F00000
#define xPromo     0x0F000000
#define xCastle    0x10000000
#define xEnpassant 0x20000000

#define from(m)         ((m) & 0xFF)
#define to(m)           (((m) >> 8) & 0xFF)
#define side(m)         (uint8)(((m) >> 16) & 1)
#define piece(m)        (Piece)(((m) >> 16) & 0x0F)
#define capture(m)      (Piece)(((m) >> 20) & 0x0F)
#define promo(m)        (Piece)(((m) >> 24) & 0x0F)
#define is_castle(m)    (((m) & xCastle) != 0)
#define is_capture(m)   (((m) & xCapture) != 0)
#define is_enpassant(m) (((m) & xEnpassant) != 0)
#define is_promo(m)     (((m) & xPromo) != 0)
#define is_quiet(m)     (((m) & (xCapture | xPromo)) == 0)

// Bits 00:00:00:FF => Source square (0 .. 63).
// Bits 00:00:FF:00 => Destination square (0 .. 63).
// Bits 00:0F:00:00 => Piece making the move.
// Bits 00:F0:00:00 => Captured piece if any.
// Bits 0F:00:00:00 => Promoted piece if any.
// Bits F0:00:00:00 => Castle and en-passant flags.

// Moving pianos is dangerous. Moving pianos are dangerous.
//------------------------------------------------------------------------------
Move new_move(Position *p, int from, int to) {
    Piece piece = p->pieces[from];
    Piece capture = p->pieces[to];

    if (p->enpassant != 0 && to == (int)p->enpassant && is_pawn(piece)) {
        capture = pawn(color(piece) ^ 1);
    }

    return from | (to << 8) | (piece << 16) | (capture << 20);
}

//------------------------------------------------------------------------------
Move new_enpassant(Position *p, int from, int to) {
    return from | (to << 8) | (p->pieces[from] << 16) | xEnpassant;
}


//------------------------------------------------------------------------------
Move new_pawn_move(Position *p, int square, int target) {
    if (abs(square - target) == 16) {
        // Check if pawn jump causes en-passant. This is done by verifying
        // whether enemy pawns occupy squares ajacent to the target square.
        Bitmask pawns = p->outposts[pawn(p->color ^ 1)];
        if (pawns & maskIsolated[col(target)] & maskRank[row(target)]) {
            return new_enpassant(p, square, target);
        }
    }
    return new_move(p, square, target);
}

//------------------------------------------------------------------------------
Move new_castle(Position *p, int from, int to) {
    return from | (to << 8) | (p->pieces[from] << 16) | xCastle;
}

//------------------------------------------------------------------------------
Move promote(Move m, int kind) {
    Piece piece = (Piece)(kind | color(piece(m)));
    return m | (Move)(piece << 24);
}

//------------------------------------------------------------------------------
Promo new_promotion(Position *p, int square, int target) {
    return (Promo){
        promote(new_move(p, square, target), Queen),
        promote(new_move(p, square, target), Rook),
        promote(new_move(p, square, target), Bishop),
        promote(new_move(p, square, target), Knight)
    };
}

// Capture value based on most valueable victim/least valueable attacker.
//------------------------------------------------------------------------------
int value(Move m) {
    int value = pieceValue[capture(m)] - kind(piece(m));

    if (is_enpassant(m)) {
        value += valuePawn.midgame;
    } else if (is_promo(m)) {
        value += pieceValue[promo(m)] - valuePawn.midgame;
    }

    return value;
}

// Decodes a string in coordinate notation and returns a move. The string is
// expected to be either 4 or 5 characters long (with promotion).
//------------------------------------------------------------------------------
Move new_move_from_notation(Position *self, char *e2e4) {
    int from = square(e2e4[1] - '1', e2e4[0] - 'a');
    int to = square(e2e4[3] - '1', e2e4[2] - 'a');
    Move move = (Move)0;

    // Check if this is a castle.
    if (is_king(self->pieces[from]) && abs(from - to) == 2) {
        return new_castle(self, from, to);
    }

    // Special handling for pawn pushes because they might cause en-passant
    // and result in promotion.
    if (is_pawn(self->pieces[from])) {
        move = new_pawn_move(self, from, to);
        if (strlen(e2e4) > 4) {
            switch (e2e4[4]) {
            case 'q': case 'Q':
                move = promote(move, Queen);
                break;
            case 'r': case 'R':
                move = promote(move, Rook);
                break;
            case 'b': case 'B':
                move = promote(move, Bishop);
                break;
            case 'n': case 'N':
                move = promote(move, Knight);
            }
        }
        return move;
    }

    return new_move(self, from, to);
}

// Returns string representation of the move in long coordinate notation as
// expected by UCI, ex. "g1f3", "e4d5" or "h7h8q".
//------------------------------------------------------------------------------
char *notation(Move m, char *buffer) {
    char *ch = buffer;
    int from = from(m), to = to(m);
    Piece promo = promo(m);

    *ch++ = COL(from) + 'a';
    *ch++ = ROW(from) + '1';
    *ch++ = COL(to) + 'a';
    *ch++ = ROW(to) + '1';
    if (promo) {
        *ch++ = tolower(character(promo));
    }
    *ch = '\0';

    return buffer;
}

// Returns move string in long algebraic notation, for example: "Ng1-f3",
// "e4xd5" or "h7-h8Q". This notation is used in tests, REPL, and when showing
// principal variation.
//------------------------------------------------------------------------------
char *move_to_string(Move m, char *buffer) {
    char *ch = buffer;
    int from = from(m), to = to(m);
    Piece piece = piece(m), capture = capture(m), promo = promo(m);

    if (is_castle(m)) {
        if (to > from) {
            return strcpy(buffer, "0-0");
        }
        return strcpy(buffer, "0-0-0");
    }

    if (!is_pawn(piece)) {
        *ch++ = character(piece);
    }

    *ch++ = COL(from) + 'a';
    *ch++ = ROW(from) + '1';
    *ch++ = capture ? 'x' : '-';
    *ch++ = COL(to) + 'a';
    *ch++ = ROW(to) + '1';
    if (promo) {
        *ch++ = character(promo);
    }
    *ch = '\0';

    return buffer;
}

//------------------------------------------------------------------------------
char *moves_to_string(Move moves[], int size, char *buffer) {
    char str[8];

    buffer[0] = '\0';
    for (int i = 0; i < size; i++) {
        strcat(buffer, move_to_string(moves[i], str));
        strcat(buffer, " ");
    }

    return buffer;
}
