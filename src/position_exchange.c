// Copyright (c) 2014-2015 by Michael Dvorkin. All Rights Reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.

// Recursive helper method for the static exchange evaluation.
//------------------------------------------------------------------------------
static int exchangeScore(Position *self, uint8 color, int to, int score, int extra, Bitmask board) {
    Bitmask attacked = attackers(self, color, to, board) & board;

    if (empty(attacked)) {
        return score;
    }

    int from = 0, best = Checkmate;

    while (attacked) {
        int square = pop(&attacked);
        int index = (int)self->pieces[square];
        if (exchangeValue[index] < best) {
            from = square;
            best = exchangeValue[index];
        }
    }

    if (best != Checkmate) {
        board ^= bit[from];
    }

    return max(score, -exchangeScore(self, color^1, to, -(score + extra), best, board));
}

// Static exchange evaluation.
//------------------------------------------------------------------------------
int exchange(Position *self, Move move) {
    int from = from(move), to = to(move), piece = piece(move), capture = capture(move), promo = promo(move);
    int score = exchangeValue[capture];

    if (promo) {
        score += exchangeValue[promo] - exchangeValue[Pawn];
        piece = promo;
    }

    Bitmask board = self->board ^ bit[from];

    return -exchangeScore(self, color(piece)^1, to, -score, exchangeValue[piece], board);
}

