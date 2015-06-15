// Copyright (c) 2014-2015 by Michael Dvorkin. All Rights Reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.

#define razoring_margin(depth) (512 + 64 * (depth - 1))
#define futility_margin(depth) (256 * depth)

//------------------------------------------------------------------------------
int search_tree(Position *self, int alpha, int beta, int depth) {
    //printf("search_tree: alpha %d beta %d depth %d\n%s\n", alpha, beta, depth, position_string(self));
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

    // Checkmate distance pruning.
    int checkmate_score = abs(ply - Checkmate);
    if (checkmate_score < beta) {
        if (checkmate_score <= alpha) {
            return alpha;
        }
        beta = checkmate_score;
    }

    // Initialize node search conditions.
    bool null = is_null(self);
    bool in_check = is_in_check(self, self->color);
    bool principal = (beta - alpha > 1);

    // Probe cache.
    Move cached_move = (Move)0;
    int static_score = UnknownScore;
    CacheEntry *cached = probe(self);
    if (cached) {
        cached_move = cached->move;
        if ((int)cached->depth >= depth) {
            static_score = uncache((int)cached->score, ply);
            if ((cached->flags == cacheExact && principal) ||
                (cached->flags == cacheBeta  && static_score >= beta) ||
                (cached->flags == cacheAlpha && static_score <= alpha)) {
                if (static_score >= beta && !in_check && cached_move && is_quiet(cached_move)) {
                    save_good(depth, cached_move);
                }
                return static_score;
            }
        }
    }

    // Quiescence search.
    if (!in_check && depth < 1) {
        return search_quiescence(self, alpha, beta, 0, in_check);
    }

    if (static_score == UnknownScore) {
        static_score = evaluate(self);
    }

    // Razoring and futility margin pruning.
    if (!in_check && !principal) {

        // No razoring if pawns are on 7th rank.
        if (!cached_move && depth < 8 && empty(self->outposts[pawn(self->color)] & mask7th[self->color])) {

            // Special case for razoring at low depths.
            if (depth <= 2 && static_score <= alpha - razoring_margin(5)) {
                return search_quiescence(self, alpha, beta, 0, in_check);
            }

            int quiescence_score = search_quiescence(self, alpha, beta + 1, 0, in_check);
            if (quiescence_score <= alpha - razoring_margin(depth)) {
                return quiescence_score;
            }
        }

        // Futility pruning is only applicable if we don't have winning score
        // yet and there are pieces other than pawns.
        Bitmask pieces_left = self->outposts[self->color] & ~(self->outposts[king(self->color)] | self->outposts[pawn(self->color)]);
        if (!null && depth < 14 && abs(beta) < Checkmate - MaxPly && pieces_left) {

            // Largest conceivable positional gain.
            int gain = static_score - futility_margin(depth);
            if (gain >= beta) {
                return gain;
            }
        }

        // Null move pruning.
        if (!null && depth > 1 && count(self->outposts[self->color]) > 5) {
            Position *position = make_null_move(self);
            game.nodes++;
            int null_score = -search_tree(position, -beta, -beta + 1, depth - 1 - 3);
            undo_null_move(position);

            if (null_score >= beta) {
                if (abs(null_score) >= Checkmate - MaxPly) {
                    return beta;
                }
                return null_score;
            }
        }
    }

    // Internal iterative deepening.
    int new_depth = depth;
    if (!in_check && !cached_move && depth > 4) {
        new_depth = depth / 2;
        if (principal) {
            new_depth = depth - 2;
        }
        search_tree(self, alpha, beta, new_depth);
        cached = probe(self); 
        if (cached) {
            cached_move = cached->move;
        }
    }

    MoveGen *gen = new_gen(self, ply);
    if (in_check) {
        quick_rank(generate_evasions(gen));
    } else {
        full_rank(generate_moves(gen), cached_move);
    }

    uint8 cache_flags = cacheAlpha;
    Move best_move = (Move)0;
    int move_count = 0, quiet_move_count = 0;

    for (Move move = next_move(gen); move; move = next_move(gen)) {
        if (!is_valid(gen, move)) {
            continue;
        }

        Position *position = make_move(self, move);
        move_count++; game.nodes++;
        new_depth = depth - 1;

        // Search depth extension.
        bool give_check = is_in_check(position, position->color);
        if (give_check) {
            new_depth++;
        }

        // Late move reduction.
        bool late_move_reduction = false;
        if (depth >= 3 && !principal && !in_check && !give_check && is_quiet(move)) {
            quiet_move_count++;
            if (new_depth > 0 && quiet_move_count >= 8) {
                new_depth--;
                late_move_reduction = true;
                if (quiet_move_count >= 16) {
                    new_depth--;
                    if (quiet_move_count >= 24) {
                        new_depth--;
                    }
                }
            }
        }

        // Start search with full window.
        if (move_count == 1) {
            score = -search_tree(position, -beta, -alpha, new_depth);
        } else if (late_move_reduction) {
            score = -search_tree(position, -alpha - 1, -alpha, new_depth);

            // Verify late move reduction and re-run the search if necessary.
            if (score > alpha) {
                score = -search_tree(position, -alpha - 1, -alpha, new_depth + 1);
            }
        } else {
            if (new_depth < 1) {
                score = -search_quiescence(position, -alpha - 1, -alpha, 0, give_check);
            } else {
                score = -search_tree(position, -alpha - 1, -alpha, new_depth);
            }

            // If zero window failed try full window.
            if (score > alpha && score < beta) {
                score = -search_tree(position, -beta, -alpha, new_depth);
            }
        }
        undo_last_move(position);

        if (time_control(game.nodes)) {
            return alpha;
        }

        if (score > alpha) {
            alpha = score;
            best_move = move;
            cache_flags = cacheExact;
            save_best(ply, move);

            if (alpha >= beta) {
                cache_flags = cacheBeta;
                break; // Stop searching. Happiness is right next to you.
            }
        }
    }

    if (move_count == 0) {
        alpha = (in_check ? -Checkmate + ply : 0);
    } else if (score >= beta && !in_check) {
        save_good(depth, best_move);
    }

    score = alpha;
    store(self, best_move, score, depth, ply, cache_flags);

    //printf("search_tree: alpha %d beta %d depth %d --> score %d\n", alpha, beta, depth, score);
    return score;
}
