// Copyright (c) 2014-2015 by Michael Dvorkin. All Rights Reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.

//------------------------------------------------------------------------------
MoveGen *generate_moves(MoveGen *self) {
    int color = self->p->color;

    pawn_moves(self, color);
    piece_moves(self, color);
    king_moves(self, color);

    return self;
}

//------------------------------------------------------------------------------
MoveGen *generate_all_moves(MoveGen *self) {
    if (is_in_check(self->p, self->p->color)) {
        return generate_evasions(self);
    }
    return generate_moves(self);
}

//------------------------------------------------------------------------------
MoveGen *generate_root_moves(MoveGen *self) {
    generate_all_moves(self);

    if (only_move(self)) {
        return self;
    }
    
    return quick_rank(valid_only(self));
}

// Copies last move returned by next_move() to the top of the list shifting
// remaining moves down. Head/tail pointers remain unchanged.
//------------------------------------------------------------------------------
MoveGen *rearrange_root_moves(MoveGen *self) {
    if (self->head > 0) {
        ScoredMove best = self->list[self->head - 1];
        memmove(&self->list[1], &self->list[0], (self->head - 1) * sizeof(ScoredMove));
        self->list[0] = best;
    }

    return self;
}

//------------------------------------------------------------------------------
MoveGen *cleanup_root_moves(MoveGen *self, int depth) {
    if (self->tail < 2) {
        return self;
    }

    // Always preserve first higest ranking move.
    for (int i = 1; i < self->tail; i++) {
        if (self->list[i].score == -depth + 1) {
            self->tail = i;
            break;
        }
    }

    return reset(self);
}
