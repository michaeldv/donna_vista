// Copyright (c) 2014-2015 by Michael Dvorkin. All Rights Reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.

MoveGen *append(MoveGen *self, Move move) {
    self->list[self->tail].move = move;
    self->tail++;
    return self;
}

MoveGen *add_pawn_move(MoveGen *self, int square, Bitmask targets) {
    while (any(targets)) {
        int target = pop(&targets);
        if (target > H1 && target < A8) {
            append(self, new_pawn_move(self->p, square, target));
        } else { // Promotion.
            Promo promo = new_promotion(self->p, square, target);
            append(self, promo.q);
            append(self, promo.r);
            append(self, promo.b);
            append(self, promo.n);
        }
    }
    return self;
}

MoveGen *add_king_move(MoveGen *self, int square, Bitmask targets) {
    while (any(targets)) {
        int target = pop(&targets);
        if (abs(square - target) == 2) {
            append(self, new_castle(self->p, square, target));
        } else {
            append(self, new_move(self->p, square, target));
        }
    }
    return self;
}

MoveGen *add_piece_move(MoveGen *self, int square, Bitmask targets) {
    while (any(targets)) {
        append(self, new_move(self->p, square, pop(&targets)));
    }
    return self;
}

MoveGen *pawn_moves(MoveGen *self, uint8 color) {
    Position *p = self->p;
    Bitmask outposts = p->outposts[pawn(color)];

    while (any(outposts)) {
        int square = pop(&outposts);
        add_pawn_move(self, square, targets(p, square));
    }

    return self;
}

// Go over all pieces except pawns and the king.
MoveGen *piece_moves(MoveGen *self, uint8 color) {
    Position *p = self->p;
    Bitmask outposts = p->outposts[color] & ~p->outposts[pawn(color)] & ~p->outposts[king(color)];

    while (any(outposts)) {
        int square = pop(&outposts);
        add_piece_move(self, square, targets(p, square));
    }

    return self;
}

MoveGen *king_moves(MoveGen *self, uint8 color) {
    bool kingside, queenside;
    Position *p = self->p;

    if any(p->outposts[king(color)]) {
        int square = (int)p->king[color];
        add_king_move(self, square, targets(p, square));

        can_castle(p, color, &kingside, &queenside);
        if (kingside) {
            add_king_move(self, square, bit[G1 + 56 * color]);
        }
        if (queenside) {
            add_king_move(self, square, bit[C1 + 56 * color]);
        }
    }

    return self;
}

