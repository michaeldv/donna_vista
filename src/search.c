// Copyright (c) 2014-2015 by Michael Dvorkin. All Rights Reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.

// Root node search. Basic principle is expressed by Boob's Law: you always find
// something in the last place you look.
//------------------------------------------------------------------------------
int search(Position *self, int alpha, int beta, int depth) {
    // printf("search: alpha %d beta %d depth %d\n", alpha, beta, depth);
    int score = 0;
    bool in_check = is_in_check(self, self->color);
    uint8 cacheFlags = cacheAlpha;

    // Root move generator makes sure all generated moves are valid. The
    // best move found so far is always the first one we search.
    MoveGen *gen = new_root_gen(self, depth);
    if (depth == 1) {
        generate_root_moves(gen);
    } else {
        rearrange_root_moves(gen);
        if (depth == 10) { // Skip moves that failed all iterations so far.
            cleanup_root_moves(gen, depth);
        }
    }

    int move_count = 0;
    Move best_move = (Move)0;

    for (Move move = next_move(gen); move; move = next_move(gen)) {
        Position *position = make_move(self, move);
        move_count++; game.nodes++;
        //TODO if engine.uci {
        //TODO     engine.uciMove(move, move_count, depth)
        //TODO }

        // Search depth extension.
        int new_depth = depth - 1;
        if (is_in_check(position, self->color^1)) { // Give check.
            new_depth++;
        }

        if (move_count == 1) {
            game.deepening = true;
            score = -search_tree(position, -beta, -alpha, new_depth);
        } else {
            game.deepening = false;
            score = -search_tree(position, -alpha - 1, -alpha, new_depth);
            if (score > alpha) { // && score < beta {
                score = -search_tree(position, -beta, -alpha, new_depth);
            }
        }
        undo_last_move(position);

        if (time_control(game.nodes)) {
            //Log("searchRoot: best_move %s pv[0][0] %s alpha %d\n", best_move, game.pv[0][0], alpha)
            //TODO if engine.uci { // Report alpha as score since we're returning alpha.
            //TODO     engine.uciScore(depth, alpha, alpha, beta)
            //TODO }
            return alpha;
        }

        if (move_count == 1 || score > alpha) {
            best_move = move;
            cacheFlags = cacheExact;
            save_best(0, move);
            score_move(gen, depth, score);

            if (move_count > 1) {
                game.volatility++; //\\ debug("# New move %s depth %d volatility %.2f\n", move, depth, game.volatility)
            }

            alpha = max(score, alpha);
            if (alpha >= beta) {
                cacheFlags = cacheBeta;
                break;
            }
        } else {
            score_move(gen, depth, -depth);
        }
    }


    if (move_count == 0) {
        score = (in_check ? -Checkmate : 0);
        //TODO if engine.uci {
        //TODO     engine.uciScore(depth, score, alpha, beta)
        //TODO }
        return score;
    }

    if (score >= beta && !in_check) {
        save_good(depth, best_move);
    }

    score = alpha;
    store(self, best_move, score, depth, PLY(), cacheFlags);
    //TODO if engine.uci {
    //TODO     engine.uciScore(depth, score, alpha, beta)
    //TODO }

    // printf("search: alpha %d beta %d depth %d --> score %d\n", alpha, beta, depth, score);
    return score;
}

// Testing helper method to test root search.
//------------------------------------------------------------------------------
Move solve(Position *self, int depth) {
    if (depth != 1) {
        generate_root_moves(new_root_gen(self, 1));
    }
    search(self, -Checkmate, Checkmate, depth);

    return game.pv[0].moves[0];
}

//------------------------------------------------------------------------------
int64 perft(Position *self, int depth) {
    int64 total = 0;

    if (depth == 0) {
        return 1;
    }

    MoveGen *gen = new_gen(self, depth);
    generate_all_moves(gen);

    for (Move move = next_move(gen); move != 0; move = next_move(gen)) {
        if (!is_valid(gen, move)) {
            continue;
        }
        Position *position = make_move(self, move);
        total += perft(position, depth - 1);
        undo_last_move(position);
    }
    return total;
}
