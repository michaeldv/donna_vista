// Copyright (c) 2014-2015 by Michael Dvorkin. All Rights Reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.

void test_position_cache_suite__010(void) {
    int save = engine.cache_size;
    engine.cache_size = 1.0; // <-- Set smaller cache size.
    new_game();
    Position *p = start();
    Move move = new_move(p, E2, E4);
    p = make_move(p, move);
    store(p, move, 42, 1, 0, cacheExact);

    CacheEntry *cached = probe(p);
    cl_check(cached->move == move);
    cl_check(cached->score == 42);
    cl_check(cached->depth == 1);
    cl_check(cached->flags == cacheExact);
    cl_check(cached->id == (uint32)(p->id >> 32));

    cl_check(cached_move(p) == move);
    engine.cache_size = save;
}

void test_position_cache_suite__020(void) {
    int save = engine.cache_size;
    engine.cache_size = 5.0; // <-- Set bigger cache size.
    new_game();
    Position *p = start();
    Move move = new_move(p, E2, E4);
    p = make_move(p, move);
    store(p, move, 42, 1, 0, cacheExact);

    CacheEntry *cached = probe(p);
    cl_check(cached->move == move);
    cl_check(cached->score == 42);
    cl_check(cached->depth == 1);
    cl_check(cached->flags == cacheExact);
    cl_check(cached->id == (uint32)(p->id >> 32));

    cl_check(cached_move(p) == move);
    engine.cache_size = save;
}
