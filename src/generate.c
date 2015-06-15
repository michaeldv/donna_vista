// Copyright (c) 2014-2015 by Michael Dvorkin. All Rights Reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.

// Returns "new" move generator for the given ply. Since move generator array
// has been pre-allocated already we simply return a pointer to the existing
// array element re-initializing all its data.
//------------------------------------------------------------------------------
MoveGen *new_gen(Position *p, int ply) {
    MoveGen *self = &moveList[ply];
    self->p = p;
    self->ply = ply;
    self->head = 0;
    self->tail = 0;
    memset(self->list, '\0', sizeof(self->list));
    self->pins = pinned_mask(p, p->king[p->color]);

    return &moveList[ply];
}

// Convenience method to return move generator for the current ply.
//------------------------------------------------------------------------------
MoveGen *new_move_gen(Position *p) {
    return new_gen(p, PLY());
}

// Returns new move generator for the initial step of iterative deepening
// (depth == 1) and existing one for subsequent iterations (depth > 1).
//------------------------------------------------------------------------------
MoveGen *new_root_gen(Position *p, int depth) {
    if (depth == 1) {
        return new_gen(p, 0); // Zero ply.
    }
    return &moveList[0];
}

//------------------------------------------------------------------------------
MoveGen *reset(MoveGen *self) {
    self->head = 0;
    return self;
}

// Removes current move from the list by copying over the ramaining moves. Head and
// tail pointers get decremented so that calling NexMove() works as expected.
//------------------------------------------------------------------------------
MoveGen *discard(MoveGen *self) {
    ScoredMove *src = &self->list[self->head];
    memcpy((src - 1), src, (self->tail - self->head) * sizeof(ScoredMove));
    self->head--;
    self->tail--;

    return self;
}

//------------------------------------------------------------------------------
bool only_move(MoveGen *self) {
    return self->tail == 1;
}

//------------------------------------------------------------------------------
Move next_move(MoveGen *self) {
    Move move = (Move)0;

    if (self->head < self->tail) {
        move = self->list[self->head].move;
        self->head++;
    }

    return move;
}

// Returns true if the move is valid in current position i.e. it can be played
// without violating chess rules.
//------------------------------------------------------------------------------
bool is_valid(MoveGen *self, Move move) {
    return is_move_valid(self->p, move, self->pins);
}

// Removes invalid moves from the generated list. We use in iterative deepening
// to avoid filtering out invalid moves on each iteration.
//------------------------------------------------------------------------------
MoveGen *valid_only(MoveGen *self) {
    for (Move move = next_move(self); move; move = next_move(self)) {
        if (!is_valid(self, move)) {
            discard(self);
        }
    }

    return reset(self);
}

// Probes a list of generated moves and returns true if it contains at least
// one valid move.
//------------------------------------------------------------------------------
bool any_valid(MoveGen *self) {
    for (Move move = next_move(self); move; move = next_move(self)) {
        if (is_valid(self, move)) {
            return true;
        }
    }

    return false;
}

// Probes valid-only list of generated moves and returns true if the given move
// is one of them.
//------------------------------------------------------------------------------
bool among_valid(MoveGen *self, Move some) {
    for (Move move = next_move(self); move; move = next_move(self)) {
        if (some == move) {
            return true;
        }
    }

    return false;
}

// Assigns given score to the last move returned by the next_move(self).
//------------------------------------------------------------------------------
MoveGen *score_move(MoveGen *self, int depth, int score) {
    ScoredMove *current = &self->list[self->head - 1];

    if (depth == 1 || current->score == score + 1) {
        current->score = score;
    } else if (score != -depth || (score == -depth && current->score != score)) {
        current->score += score; // Fix up aspiration search drop.
    }

    return self;
}

// Sort the damn thing.
//------------------------------------------------------------------------------
MoveGen *sort(MoveGen *self) {
    int total = self->tail - self->head;
    int count = total;
    bool swapping = true;
    ScoredMove pocket = {};

    while (swapping) {
        count = (count + 1) / 2;
        swapping = count > 1;
        for (int i = 0; i < total - count; i++) {
            ScoredMove *this = &self->list[i + count];
            if (this->score > self->list[i].score) {
                pocket = *this;
                self->list[i + count] = self->list[i];
                self->list[i] = pocket;
                swapping = true;
            }
        }
    }

    return self;
}

//------------------------------------------------------------------------------
MoveGen *full_rank(MoveGen *self, Move best) {
    if (self->tail < 2) {
        return self;
    }

    for (int i = self->head; i < self->tail; i++) {
        Move move = self->list[i].move;
        if (move == best) {
            self->list[i].score = 0xFFFF;
        } else if (is_capture(move) || is_enpassant(move) || is_promo(move)) {
            self->list[i].score = 8192 + value(move);
        } else if (move == game.killers[self->ply][0]) {
            self->list[i].score = 4096;
        } else if (move == game.killers[self->ply][1]) {
            self->list[i].score = 2048;
        } else {
            self->list[i].score = good(move);
        }
    }

    return sort(self);
}

//------------------------------------------------------------------------------
MoveGen *quick_rank(MoveGen *self) {
    if (self->tail < 2) {
        return self;
    }

    for (int i = self->head; i < self->tail; i++) {
        Move move = self->list[i].move;
        if (is_capture(move) || is_enpassant(move) || is_promo(move)) {
            self->list[i].score = 8192 + value(move);
        } else {
            self->list[i].score = good(move);
        }
    }

    return sort(self);
}

//------------------------------------------------------------------------------
MoveGen *root_rank(MoveGen *self, Move best) {
    if (self->tail < 2) {
        return self;
    }

    // Find the best move and assign it the highest score.
    int top = -1, killer = -1, semikiller = -1, highest = -Checkmate;

    for (int i = self->head; i < self->tail; i++) {
        ScoredMove *current = &self->list[i];
        if (current->move == best) {
            top = i;
        } else if (current->move == game.killers[self->ply][0]) {
            killer = i;
        } else if (current->move == game.killers[self->ply][1]) {
            semikiller = i;
        }
        current->score += good(current->move) >> 3;
        if (current->score > highest) {
            highest = current->score;
        }
    }
    if (top != -1) {
        self->list[top].score = highest + 10;
    }
    if (killer != -1) {
        self->list[killer].score = highest + 2;
    }
    if (semikiller != -1) {
        self->list[semikiller].score = highest + 1;
    }

    return sort(self);
}

// Returns an array of generated moves by continuously appending the next_move()
// until the list is empty.
//------------------------------------------------------------------------------
char *all_moves(MoveGen *self, char *buffer) {
    char str[8];

    strcpy(buffer, "[");
    for (Move move = next_move(self); move; move = next_move(self)) {
        strcat(buffer, move_to_string(move, str));
        strcat(buffer, " ");
    }

    if (strlen(buffer) == 1) {
        strcat(buffer, "]");
    } else {
        buffer[strlen(buffer) - 1] = ']';
    }
    reset(self);

    return buffer;
}
