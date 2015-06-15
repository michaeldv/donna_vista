// Copyright (c) 2014-2015 by Michael Dvorkin. All Rights Reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.

//------------------------------------------------------------------------------
Position *repl_setup(Position *self) {
    if (!self) {
        char buffer[512];
        new_game();
        self = start();
        printf("%s\n", position_string(self, buffer));
    }
    return self;
}

//------------------------------------------------------------------------------
Position *repl_think(Position *self) {
    Move move = think();
    if (move) {
        char buffer[512];
        self = make_move(self, move);
        printf("%s\n", position_string(self, buffer));
    }
    return self;
}

//------------------------------------------------------------------------------
Position *repl_undo(Position *self) {
    if (self) {
        char buffer[512];
        self = undo_last_move(self);
        printf("%s\n", position_string(self, buffer));
    }
    return self;
}

//------------------------------------------------------------------------------
void repl_book(char *file_name) {
    printf("book -- not implemented yet (%s)\n", file_name);
    // book = func(fileName string) {
    //     if e.bookFile = fileName; e.bookFile == "" {
    //         fmt.Println("Using no opening book")
    //     } else {
    //         fmt.Printf("Using opening book %s\n", fileName)
    //     }
    // }
}

//------------------------------------------------------------------------------
void repl_bench(char *file_name) {
    char line[256], buffer[512];
    int total = 0, solved = 0;
    char *white, *black, *solution, *best;

	FILE *file = fopen(file_name, "rt");
	if (!file) {
        printf("Could not open file: [%s]\n", file_name);
		return;
	}

    // Save engine settings.
    int64 max_depth = engine.options.max_depth;
    int64 move_time = engine.options.move_time;

    // Set fixed time of 10s per position.
    engine.options.max_depth = 0;
    engine.options.move_time = 10 * 1000 * 1000;

    // Read benchmarks line by line, turn them into position, and solve.
    while (fgets(line, sizeof(line), file)) {
        if (strlen(line) > 0 && line[0] != '#' && line[0] != '\n') {
            total++;
            line[strlen(line) - 1] = '\0'; // Strip trailing \n.
            // Split into white, black, and solution move(s).
            white = strtok(line, " : ");
            if (white) {
                black = strtok(white + strlen(white) + 3, " # ");
                if (black) {
                    solution = black + strlen(black) + 3;
                }
            }
            if (white && black && solution) {
                setup(white, black);
                Position *p = start();
                printf(ANSI_TEAL "%d) %s for %s" ANSI_NONE "\n%s\n", total, solution, p->color ? "Black" : "White", position_string(p, buffer));
                Move move = think();

                // Solution might space-separated list of best moves.
                best = strtok(solution, " ");
                while (best) {
                    // Strip extra characters leaving the actual move only.
                    for (char *ch = best + strlen(best) - 1; strchr(" +?!", *ch); ch--) {
                        *ch = '\0';
                    }
                    if (!strcmp(move_to_string(move, buffer), best)) {
                        solved++;
                        printf(ANSI_GREEN "%d) Solved (%d/%d %2.1f%%)\n\n\n" ANSI_NONE, total, solved, total - solved, (float)solved * 100.0 / (float)total);
                        goto read_next_line;
                    }
                    best = strtok(NULL, " ");
                }
                printf(ANSI_RED "%d) Not solved (%d/%d %2.1f%%)\n\n\n" ANSI_NONE, total, solved, total - solved, (float)solved * 100.0 / (float)total);
            }
        }
        read_next_line:;
    }
    fclose(file);

    // Restore engine settings.
    engine.options.max_depth = max_depth;
    engine.options.move_time = move_time;
}

//------------------------------------------------------------------------------
void repl_perft(char *arg) {
    int depth = arg ? atoi(arg) : 5;
    if (depth <= 0) {
        depth = 5;
    }

    Timestamp checkpoint = now();
    new_game();
    int64 nodes = perft(start(), depth);
    uint64 elapsed = since(checkpoint);
    uint64 nps = nodes * 1000000 / elapsed; // Elapsed is in microseconds.

    printf("  Depth: %d\n", depth);
    printf("  Nodes: %lld\n", nodes);
    printf("Elapsed: %llu.%06llus\n", elapsed / 1000000, elapsed % 1000000);
    printf("Nodes/s: %lldK\n\n", nps / 1000);
}

//------------------------------------------------------------------------------
void repl_help() {
    printf("The commands are:\n\n"
    "  bench <file>   Run benchmarks\n"
    "  book <file>    Use opening book\n"
    "  exit           Exit the program\n"
    "  go             Take side and make a move\n"
    "  help           Display this help\n"
    "  new            Start new game\n"
    "  perft [depth]  Run perft test\n"
    "  undo           Undo last move\n\n"
    "To make a move use algebraic notation, for example e2e4, Ng1f3, or e7e8Q\n");
}

// Before returning the move make sure it is valid in current position.
//------------------------------------------------------------------------------
Move validate(Position *self, Move move) {
    if (move) {
        MoveGen *gen = new_move_gen(self);
        valid_only(generate_all_moves(gen));
        if (!among_valid(gen, move)) {
            return (Move)0;
        }
    }

    return move;
}

// Decodes a string in long algebraic notation and returns a move. If the
// notation is not recognized the move is returned as (Move)0.
//------------------------------------------------------------------------------
Move new_move_from_string(Position *self, char *e2e4) {
    bool queen_side_castle = !strcmp(e2e4, "0-0-0");
    bool king_side_castle = !strcmp(e2e4, "0-0") && !queen_side_castle;

    if (king_side_castle || queen_side_castle) {
        bool king_side_ok = false, queen_side_ok = false;
        can_castle(self, self->color, &king_side_ok, &queen_side_ok);
        if (king_side_castle && king_side_ok) {
            int from = self->king[self->color];
            int to = G1 + self->color * A8;
            return validate(self, new_castle(self, from, to));
        }
        if (queen_side_castle && queen_side_ok) {
            int from = self->king[self->color];
            int to = C1 + self->color * A8;
            return validate(self, new_castle(self, from, to));
        }
        return (Move)0;
    }

    char notation[strlen(e2e4)];
    char *src = e2e4, *dst = notation;

    // Strip all optional characters.
    while (*src) {
        if (strchr("KkQqRrBbNn12345678acdefgh", *src)) {
            *dst++ = *src;            
        }
        src++;
    }
    *dst = '\0';

    if (strlen(notation) >= 4 && strlen(notation) <= 5) {
        // Validate optional piece character to make sure the actual piece it
        // represents is there.
        Piece piece = pawn(self->color);
        switch (notation[0]) {
        case 'K':
            piece = king(self->color);
            break;
        case 'Q':
            piece = queen(self->color);
            break;
        case 'R':
            piece = rook(self->color);
            break;
        case 'B':
            piece = bishop(self->color);
            break;
        case 'N':
            piece = knight(self->color);
        }

        int sq;
        if (is_pawn(piece)) {
            sq = square(notation[1] - '1', notation[0] - 'a');
        } else {
            sq = square(notation[2] - '1', notation[1] - 'a');
        }

        if (sq >= A1 && sq <= H8) {
            if (self->pieces[sq] == piece) {
                return validate(self, new_move_from_notation(self, is_pawn(piece) ? notation : notation + 1));
            }
        }
    }

    return (Move)0;
}

// There are two types of command interfaces in the world of computing: good
// interfaces and user interfaces. -- Daniel J. Bernstein
//------------------------------------------------------------------------------
void repl() {
    Position *position = NULL;
    char command[128] = "";

    printf("Donna Vista v%s Copyright (c) 2014-2015 by Michael Dvorkin. All Rights Reserved.\nType ? for help.\n\n", VERSION);
    while (true) {
        printf("donna_vista> ");
        if (fgets(command, sizeof(command), stdin)) {
            // Strip trailing newline returned by fgets().
            int len = strlen(command);
            if (len > 0 && command[len - 1] == '\n') {
                command[len - 1] = '\0';
            }
            if (strlen(command)) {
                if (!strncmp(command, "bench", 5)) {
                    repl_bench(strtok(&command[5], " "));

                } else if (!strncmp(command, "book", 4)) {
                    repl_book(strtok(&command[4], " "));

                } else if (!strncmp(command, "perft", 5)) {
                    repl_perft(strtok(&command[5], " "));

                } else if (!strcmp(command, "go")) {
                    position = repl_think(repl_setup(position));

                } else if (!strcmp(command, "help") || !strcmp(command, "?")) {
                    repl_help();

                } else if (!strcmp(command, "new")) {
                    position = repl_setup(NULL);

                } else if (!strcmp(command, "undo")) {
                    position = repl_undo(position);

                } else if (!strcmp(command, "exit") || !strcmp(command, "quit")) {
                    break;

                } else {
                    Move move = new_move_from_string(position, command);
                    if (move) {
                        position = repl_think(make_move(repl_setup(position), move));
                    } else {
                        char buffer[256];
                        MoveGen *gen = new_move_gen(position);
                        valid_only(generate_all_moves(gen));
                        printf("%s appears to be an invalid move; valid moves are %s\n", command, all_moves(gen, buffer));
                    }
                }
            }
        } else {
            break; // Exit if fgets() errors out.
        }
    }
}
