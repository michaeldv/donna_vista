// Copyright (c) 2014-2015 by Michael Dvorkin. All Rights Reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.

//------------------------------------------------------------------------------
void save_best(int ply, Move move) {
    game.pv[ply].moves[ply] = move;
    game.pv[ply].size = ply + 1;

    int next = game.pv[ply].size;
    if (next < MaxPly && game.pv[next].size > next) {
        for (int i = next; i < game.pv[next].size; i++) {
            game.pv[ply].moves[i] = game.pv[next].moves[i];
            game.pv[ply].size++;
        }
    }
}

//------------------------------------------------------------------------------
void save_good(int depth, Move move) {
    int ply = node - rootNode;
    if (is_quiet(move) && move != game.killers[ply][0]) {
        game.killers[ply][1] = game.killers[ply][0];
        game.killers[ply][0] = move;
        game.history[piece(move)][to(move)] += depth * depth;
    }
}

// Quiescence search.
//------------------------------------------------------------------------------
int search_quiescence(Position *self, int alpha, int beta, int iteration, bool in_check) {
    int score = 0, ply = PLY();

    // Reset principal variation.
    game.pv[ply].size = 0;

    // Return if it's time to stop search.
    if (ply >= MaxPly || engine.clock.halt) {
        return evaluate(self);
    }

    // Insufficient material and repetition/perpetual check pruning.
    if (insufficient(self) || repetition(self) || fifty(self)) {
        return 0;
    }

    // If you pick up a starving dog and make him prosperous, he will not
    // bite you. This is the principal difference between a dog and a man.
    // ―- Mark Twain
    bool principal = (beta - alpha > 1);

    // Use fixed depth for caching.
    int depth = 0;
    if (!in_check && iteration > 0) {
        depth--;
    }

    // Probe cache.
    int static_score = alpha;
    CacheEntry *cached = probe(self);
    if (cached) {
        if ((int)cached->depth >= depth) {
            static_score = uncache((int)cached->score, ply);
            if ((cached->flags == cacheExact && principal) ||
                (cached->flags == cacheBeta  && static_score >= beta) ||
                (cached->flags == cacheAlpha && static_score <= alpha)) {
                return static_score;
            }
        }
    }

    if (!in_check) {
        static_score = evaluate(self);
        if (static_score >= beta) {
            store(self, (Move)0, static_score, depth, ply, cacheBeta);
            return static_score;
        }
        if (principal) {
            alpha = max(alpha, static_score);
        }
    }

    // Generate check evasions or captures.
    MoveGen *gen = new_gen(self, ply);
    if (in_check) {
        generate_evasions(gen);
    } else {
        generate_captures(gen);
        if (iteration < 1) {
            generate_checks(gen);
        }
    }
    quick_rank(gen);

    uint8 cache_flags = cacheAlpha;
    Move best_move = (Move)0;
    int move_count = 0;

    for (Move move = next_move(gen); move; move = next_move(gen)) {
        Piece cap = capture(move);
        if ((!in_check && cap && exchange(self, move) < 0) || !is_valid(gen, move)) {
            continue;
        }

        Position *position = make_move(self, move);
        move_count++; game.qnodes++;
        bool give_check = is_in_check(position, position->color);

        // Prune useless captures -- but make sure it's not a capture move that checks.
        if (!in_check && !give_check && !principal && cap && !is_promo(move) && static_score + pieceValue[cap] + 72 < alpha) {
            undo_last_move(position);
            continue;
        }
        score = -search_quiescence(position, -beta, -alpha, iteration + 1, give_check);
        undo_last_move(position);

        if (score > alpha) {
            alpha = score;
            best_move = move;
            if (alpha >= beta) {
                store(self, best_move, score, depth, ply, cacheBeta);
                return score;
            }
            cache_flags = cacheExact;
        }
        if (time_control(game.qnodes)) {
            return alpha;
        }
    }

    score = alpha;
    if (in_check && move_count == 0) {
        score = -Checkmate + ply;
    }
    store(self, best_move, score, depth, ply, cache_flags);

    return score;
}
