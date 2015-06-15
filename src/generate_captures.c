// Copyright (c) 2014-2015 by Michael Dvorkin. All Rights Reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.


// Generates all pseudo-legal pawn captures and promotions.
//------------------------------------------------------------------------------
MoveGen *pawn_captures(MoveGen *self, uint8 color) {
    Position *p = self->p;
    Bitmask enemy = p->outposts[color ^ 1];
    Bitmask pawns = p->outposts[pawn(color)];

    while (any(pawns)) {
        int square = pop(&pawns);

        // For pawns on files 2-6 the moves include captures only,
        // while for pawns on the 7th file the moves include captures
        // as well as promotion on empty square in front of the pawn.
        if (rank(color, square) != 6) {
            add_pawn_move(self, square, targets(p, square) & enemy);
        } else {
            add_pawn_move(self, square, targets(p, square));
        }
    }
    return self;
}

// Generates all pseudo-legal captures by pieces other than pawn.
//------------------------------------------------------------------------------
MoveGen *piece_captures(MoveGen *self, uint8 color) {
    Position *p = self->p;
    Bitmask enemy = p->outposts[color ^ 1];
    Bitmask king = p->outposts[king(color)];
    Bitmask outposts = p->outposts[color] & ~p->outposts[pawn(color)] & ~king;

    while (any(outposts)) {
        int square = pop(&outposts);
        add_piece_move(self, square, targets(p, square) & enemy);
    }

    if (king) {
        int square = pop(&king);
        add_king_move(self, square, targets(p, square) & enemy);
    }
    return self;
}

//------------------------------------------------------------------------------
MoveGen *generate_captures(MoveGen *self) {
    int color = self->p->color;

    pawn_captures(self, color);
    piece_captures(self, color);

    return self;
}
