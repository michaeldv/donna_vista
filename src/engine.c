// Copyright (c) 2014-2015 by Michael Dvorkin. All Rights Reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.


// Returns remaining search time to make a move. The remaining time extends the
// soft stop estimate based on search volatility factor.
#define remaining() ((uint64)((float)engine.clock.soft_stop * engine.clock.extra))


//------------------------------------------------------------------------------
void new_engine(char *args, ...) {
    printf("new_engine()...\n");
    va_list ap;
    va_start(ap, args);

    char *key = args;
    while (key) {//} && strcmp(key, "")) {
        if (!strcmp(key, "log")) {
            engine.log = va_arg(ap, bool);
        } else if (!strcmp(key, "fancy")) {
            engine.fancy = va_arg(ap, bool);
        } else if (!strcmp(key, "uci")) {
            engine.uci = va_arg(ap, bool);
        } else if (!strcmp(key, "depth")) {
            engine.options.max_depth = va_arg(ap, int);
        } else if (!strcmp(key, "cache")) {
            engine.cache_size = (float)va_arg(ap, double);
        } else if (!strcmp(key, "logfile")) {
            engine.log_file = va_arg(ap, char *);
        } else if (!strcmp(key, "bookfile")) {
            engine.book_file = va_arg(ap, char *);
        }
        key = va_arg(ap, char *);
    }
    va_end(ap);

    printf("log:        [%s]\n", engine.log ? "true" : "false");
    printf("fancy:      [%s]\n", engine.fancy ? "true" : "false");
    printf("uci:        [%s]\n", engine.uci ? "true" : "false");
    printf("max_depth:  [%d]\n", engine.options.max_depth);
    printf("cache_size: [%f]\n", engine.cache_size);
    printf("log_file:   [%s]\n", engine.log_file);
    printf("book_file:  [%s]\n", engine.book_file);
}

// // Appends the string to log file. No flush is required as f.Write() and friends
// // are unbuffered.
// //------------------------------------------------------------------------------
// Engine *debug(e *Engine, args ...interface{}) {
//     if len(e.log_file) != 0 {
//         log_file, err = os.OpenFile(e.log_file, os.O_CREATE | os.O_WRONLY | os.O_APPEND, 0666)
//         if err == nil {
//             defer log_file.Close()
//             if len = len(args); len > 1 {
//                 log_file.WriteString(fmt.Sprintf(args[0].(string), args[1:]...))
//             } else {
//                 log_file.WriteString(args[0].(string))
//             }
//         }
//     }
//     return e
// }
//
// // Dumps the string to standard output and optionally logs it to file.
// //------------------------------------------------------------------------------
// Engine *reply(e *Engine, args ...interface{}) {
//     if len = len(args); len > 1 {
//         data = fmt.Sprintf(args[0].(string), args[1:]...)
//         engine.print(data)
//         //\\ engine.debug(data)
//     } else if len == 1 {
//         engine.print(args[0].(string))
//         //\\ engine.debug(args[0].(string))
//     }
//     return e
// }


// Sets extra time factor. For depths 5+ we take into account search volatility,
// i.e. extra time is given for uncertain positions where the best move is not clear.
//------------------------------------------------------------------------------
void factor(int depth, float volatility) {
    engine.clock.extra = 0.75;
    if (depth >= 5) {
        engine.clock.extra *= (volatility + 1.0);
    }
}

// Callback function for fixed time control (ex. 5s per move). Search gets
// terminated when we've got the move and the elapsed time approaches
// time-per-move limit.
//------------------------------------------------------------------------------
void fixed_time_ticker() {
    if (since(engine.clock.start) >= engine.options.move_time - 125000) {
        engine.clock.halt = true;
    }
}

// Callback function for the variable time control (ex. 40 moves in 5 minutes).
// Search termination depends on multiple factors with hard stop being the
// ultimate limit.
//------------------------------------------------------------------------------
void varying_time_ticker() {
    uint64 elapsed = since(engine.clock.start);

    if ((game.deepening && game.improving && elapsed > remaining() * 4 / 5) || elapsed >= engine.clock.hard_stop) {
        //\\ debug("# Halt: Flags %v Elapsed %s Remaining %s Hard stop %s\n",
        //\\    game.deepening && game.improving, ms(elapsed), ms(e.remaining() * 4 / 5), ms(e.clock.hard_stop))
        engine.clock.halt = true;
    }
}

// Every ~16K nodes calls ticker to make sure we won't exceed time limit.
//------------------------------------------------------------------------------
bool time_control(int nodes) {
    if (!fixed_depth() && engine.clock.ticker && game.rootpv.size > 0 && (nodes & 0x3FFF) == 0) {
        engine.clock.ticker();
    }

    return engine.clock.halt;
}

// Starts the clock by setting timer callback function. The callback function is
// different for fixed and variable time controls.
//------------------------------------------------------------------------------
void start_clock() {
    engine.clock.halt = false;

    if (engine.options.move_time != 0 || engine.options.time_left != 0) {
        engine.clock.start = now();

        // How long a minute is depends on which side of the bathroom door you're on.
        if (fixed_time()) {
            engine.clock.ticker = fixed_time_ticker;
        } else {
            engine.clock.ticker = varying_time_ticker;
        }
    }
}

// Stop the clock by dropping timer callback function.
//------------------------------------------------------------------------------
void stop_clock() {
    engine.clock.halt = true;
    engine.clock.ticker = NULL;
}

// // Sets fixed search limits such as maximum depth or time to make a move.
// Engine *fixedLimit(e *Engine, options Options) {
//     engine.options = options
//     return e
// }
//
// // Sets variable time control options and calculates soft and hard stop estimates.
// Engine *varyingLimits(e *Engine, options Options) {
//
//     // Note if it's a new time control before saving the options.
//     engine.options = options
//     engine.options.ponder = false
//     engine.options.infinite = false
//     engine.options.max_depth = 0
//     engine.options.max_nodes = 0
//     engine.options.move_time = 0
//
//     // Set default number of moves till the end of the game or time control.
//     // TODO: calculate based on game phase.
//     if engine.options.moves_to_go == 0 {
//         engine.options.moves_to_go = 40
//     }
//
//     // Calculate hard and soft stop estimates.
//     moves = engine.options.moves_to_go - 1
//     hard = options.time_left + options.time_inc * moves
//     soft = hard / engine.options.moves_to_go
//
//     //\\ engine.debug("#\n# Make %d moves in %s soft stop %s hard stop %s\n", engine.options.moves_to_go, ms(e.options.time_left), ms(soft), ms(hard))
//
//     // Adjust hard stop to leave enough time reserve for the remaining moves. The time
//     // reserve starts at 100% of soft stop for one remaining move, and goes down to 80%
//     // in 1% decrement for 20+ remaining moves.
//     if moves > 0 { // The last move gets all remaining time and doesn't need the reserve.
//         percent = max64(80, 100 - moves)
//         reserve = soft * moves * percent / 100
//         //\\ engine.debug("# Reserve %d%% = %s\n", percent, ms(reserve))
//         if hard - reserve > soft {
//             hard -= reserve
//         }
//         // Hard stop can't exceed optimal time to make 3 moves.
//         hard = min64(hard, soft * 3)
//         //\\ engine.debug("# Hard stop %s\n", ms(hard))
//     }
//
//     // Set the final values for soft and hard stops making sure the soft stop
//     // never exceeds the hard one.
//     if soft < hard {
//         engine.clock.soft_stop, engine.clock.hard_stop = soft, hard
//     } else {
//         engine.clock.soft_stop, engine.clock.hard_stop = hard, soft
//     }
//
//     // Keep two ping cycles available to avoid accidental time forefeit.
//     engine.clock.hard_stop -= 2 * Ping
//     if engine.clock.hard_stop < 0 {
//         engine.clock.hard_stop = options.time_left // Oh well...
//     }
//
//     //\\ engine.debug("# Final soft stop %s hard stop %s\n#\n", ms(e.clock.soft_stop), ms(e.clock.hard_stop))
//
//     return e
// }
