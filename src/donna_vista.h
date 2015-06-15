// Copyright (c) 2014-2015 by Michael Dvorkin. All Rights Reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <time.h>
#include <assert.h>
#include <sys/time.h> // gettimeofday

#include "basics.c"
#include "bitmask.c"
#include "data.c"
#include "bitbase.c"
#include "piece.c"
#include "utils.c"
#include "score.c"
#include "params.c"
#include "init.c"
#include "move.c"
#include "position_targets.c"
#include "position_moves.c"
#include "position_cache.c"
#include "position_exchange.c"
#include "generate_helpers.c"
#include "generate_captures.c"
#include "generate_checks.c"
#include "generate_evasions.c"
#include "generate.c"
#include "generate_moves.c"
#include "position.c"
#include "evaluate_endgame.c"
#include "evaluate_pieces.c"
#include "evaluate_pawns.c"
#include "evaluate_safety.c"
#include "evaluate_threats.c"
#include "evaluate.c"
#include "engine.c"
#include "search_quiescence.c"
#include "search_tree.c"
#include "search.c"
#include "game.c"
#include "engine_repl.c"
