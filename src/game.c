// Copyright (c) 2014-2015 by Michael Dvorkin. All Rights Reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.

// We have two ways to initialize the game: 1) pass FEN string, and 2) specify
// white and black pieces using regular chess notation.
//
// In latter case we need to tell who gets to move first when starting the game.
// The second option is a bit less pricise (ex. no en-passant square) but it is
// much more useful when writing tests from memory.
//------------------------------------------------------------------------------
void setup(char *args, ...) {
    va_list ap;
    va_start(ap, args);

    // Reset the game except cache data.
    Cache *cache = game.cache;
    uint32 csize = game.csize;
    memset((void *)&game, '\0', sizeof(Game));
    game.cache = cache;
    game.csize = csize;

    // Set up the cache (create new or reset existing).
    setup_cache(engine.cache_size);

    // Check if it's FEN or Donna chess format (DCF).
    if (strchr(args, '/')) {
        game.fen = args;
    } else {
        game.dcf[0] = args;
        game.dcf[1] = va_arg(ap, char *);
    }

    va_end(ap);
}

//------------------------------------------------------------------------------
void new_game() {
    return setup("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

//------------------------------------------------------------------------------
Position *start() {
    engine.clock.halt = false;

    memset((void *)&tree, '\0', sizeof(tree));
    node = 0;
    rootNode = 0;

    // Was the game started with FEN or algebraic notation?
    if (game.fen) {
        return new_position_from_fen(game.fen);
    }
    return new_position(game.dcf[0], game.dcf[1]);
}

//------------------------------------------------------------------------------
Position *position() {
    return &tree[node];
}

// Resets principal variation as well as killer moves and move history. Cache
// entries get expired by incrementing cache token. Root node gets set to the
// current tree node to match the position.
//------------------------------------------------------------------------------
static void get_ready() {
    game.rootpv.size = 0;
    memset(game.rootpv.moves, '\0', sizeof(game.rootpv.moves));
    memset(game.killers, '\0', sizeof(game.killers));
    memset(game.history, '\0', sizeof(game.history));
    game.deepening = false;
    game.improving = true;
    game.volatility = 0.0;
    game.token++; // <-- Wraps around: ...254, 255, 0, 1...

    rootNode = node;
}

//------------------------------------------------------------------------------
void print_best_move(Move move, uint64 duration) {
    char buffer[8];

    printf(ANSI_TEAL "Donna's move: %s", move_to_string(move, buffer));
	if (game.nodes == 0) {
        printf(" (book)");
	}
    printf(ANSI_NONE "\n\n");
}

// Prints principal variation. Note that in REPL advantage white is always +score
// and advantage black is -score whereas in UCI +score is advantage current side
// and -score is advantage opponent.
//------------------------------------------------------------------------------
void print_principal(int depth, int score, Status status, uint64 duration) {
    char buffer[256];
    if (position()->color == Black) {
        score = -score;
    }
	printf("%2d %s %9d %9d %9llu   ", depth, ms(duration, buffer), game.nodes, game.qnodes, nps(duration));
	switch (status) {
	case WhiteWon:
		printf("1-0 White Checkmates\n");
        break;
	case BlackWon:
		printf("0-1 Black Checkmates\n");
        break;
	case Stalemate:
		printf("1/2 Stalemate\n");
        break;
	case Repetition:
		printf("1/2 Repetition\n");
        break;
	case FiftyMoves:
		printf("1/2 Fifty Moves\n");
        break;
	case WhiteWinning:
    case BlackWinning: // Show moves till checkmate.
		printf("%6dX   %s Checkmate\n", (Checkmate - abs(score)) / 2, moves_to_string(game.rootpv.moves, game.rootpv.size, buffer));
        break;
	default:
		printf("%7.2f   %s\n", (float)score / (float)onePawn, moves_to_string(game.rootpv.moves, game.rootpv.size, buffer));
	}
}

// game.rootpv = append(game.rootpv[:0], game.pv[0]...)
//------------------------------------------------------------------------------
void update_principal() {
    memcpy(&game.rootpv.moves, &game.pv[0].moves, sizeof(Move) * game.pv[0].size);
    game.rootpv.size = game.pv[0].size;
}

// When in doubt, do what the President does ―- guess.
//------------------------------------------------------------------------------
static bool keep_thinking(int depth, Move move) {
    if (depth == 1) {
        return true;
    }
    // --------- STUB FOR FIXED DEPTH --------- 
    if (fixed_depth()) {
        return depth <= engine.options.max_depth;
    } else if (engine.clock.halt) {
        return false;
    }

    // Stop deepening if it's the only move.
    MoveGen *gen = new_root_gen(NULL, depth);
    if (only_move(gen)) {
        return false;
    }

    // Stop if the time left is not enough to gets through the next iteration.
    //TODO if (engine.varyingTime()) {
    //TODO     elapsed = engine.elapsed(time.Now());
    //TODO     remaining = engine.factor(depth, game.volatility).remaining();
    //TODO
    //TODO     //\\ debug("# Depth %02d Volatility %.2f Elapsed %s Remaining %s\n", depth, game.volatility, ms(elapsed), ms(remaining))
    //TODO     if (elapsed > remaining) {
    //TODO         //\\ debug("# Depth %02d Bailing out with %s\n", depth, move)
    //TODO         return false;
    //TODO     }
    //TODO }

    return true;
}

// "The question of whether machines can think is about as relevant as the
// question of whether submarines can swim." -- Edsger W. Dijkstra
//------------------------------------------------------------------------------
Move think() {
    Timestamp checkpoint = now();
    Position *p = position();
    game.nodes = game.qnodes = 0;

    //TODO Polyglot books TODO
    // if len(engine.bookFile) != 0 {
    //     if book, err = NewBook(engine.bookFile); err == nil {
    //         if move = book.pickMove(position); move != 0 {
    //             game.print_best_move(move, since(start))
    //             return move
    //         }
    //     } else if !engine.uci {
    //         printf("Book error: %v\n", err)
    //     }
    // }

    get_ready();
    int score = 0, alpha = -Checkmate, beta = Checkmate;
    Move move = (Move)0;
    Status status = InProgress;

    printf("Depth   Time     Nodes    QNodes   Nodes/s     Score   Best\n");

    if (!fixed_depth()) {
        start_clock();
    }

    for (int depth = 1; status == InProgress && keep_thinking(depth, move); depth++) {
        // Save previous best score in case search gets interrupted.
        int best_score = score;

        // Assume volatility decreases with each new iteration.
        game.volatility /= 2.0;

        // At low depths do the search with full alpha/beta spread.
        // Aspiration window searches kick in at depth 5 and up.
        if (depth < 5) {
            score = search(p, alpha, beta, depth);
            if (score > alpha) {
                best_score = score;
                update_principal();
            }
        } else {
            int aspiration = onePawn / 3;
            alpha = max(score - aspiration, -Checkmate);
            beta = min(score + aspiration, Checkmate);

            // Do the search with smaller alpha/beta spread based on
            // previous iteration score, and re-search with the bigger
            // window as necessary.
            while (true) {
                score = search(p, alpha, beta, depth);
                if (score > alpha) {
                    best_score = score;
                    update_principal();
                }

                if (engine.clock.halt) {
                    break;
                }

                if (score <= alpha) {
                    game.improving = false;
                    alpha = max(score - aspiration, -Checkmate);
                } else if (score >= beta) {
                    beta = min(score + aspiration, Checkmate);
                } else {
                    break;
                }

                aspiration *= 2;
            }
        }

        if (engine.clock.halt) {
            //Log("\ttimed out pv => %v\n\ttimed out rv => %v\n", game.pv[0], game.rootpv)
            score = best_score;
        }

        move = game.rootpv.moves[0];
        status = standing(p, move, abs(score));
        print_principal(depth, score, status, since(checkpoint));
    }
    if (!fixed_depth()) {
        stop_clock();
    }
    print_best_move(move, since(checkpoint));
    
    return move;
}

//------------------------------------------------------------------------------
#define game_string() position_string(position())

