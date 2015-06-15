/*
 * Copyright (c) Vicent Marti. All rights reserved.
 *
 * This file is part of clar, distributed under the ISC license.
 * For full terms see the included COPYING file.
 */

#include "clar_test.h"

/*
 * Minimal main() for clar tests.
 *
 * Modify this with any application specific setup or teardown that you need.
 * The only required line is the call to `clar_test(argc, argv)`, which will
 * execute the test suite.  If you want to check the return value of the test
 * application, main() should return the same value returned by clar_test().
 */

#include "donna_vista.h"    
#include "bitmask_suite.c"
#include "init_suite.c"
#include "move_suite.c"
#include "game_suite.c"
#include "position_cache_suite.c"
#include "position_targets_suite.c"
#include "position_moves_suite.c"
#include "position_exchange_suite.c"
#include "position_suite.c"
#include "generate_suite.c"
#include "generate_moves_suite.c"
#include "generate_captures_suite.c"
#include "generate_checks_suite.c"
#include "generate_evasions_suite.c"
#include "evaluate_suite.c"
#include "evaluate_pawns_suite.c"
#include "search_suite.c"

#ifdef _WIN32
int __cdecl main(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{
	/* Run the test suite */
    init();
    engine.cache_size = 4.0; // <-- Set cache size.
	return clar_test(argc, argv);
}
