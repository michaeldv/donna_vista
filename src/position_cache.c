// Copyright (c) 2014-2015 by Michael Dvorkin. All Rights Reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.

//------------------------------------------------------------------------------
int cache_usage() {
    register int hits = 0;
    register int entries = game.csize / sizeof(CacheEntry);

    for (int i = 0; i < entries; i++) {
        if (game.cache[i].id != 0) {
            hits++;
        }
    }

    return hits;
}

// Creates new or resets existing game cache (aka transposition table).
//------------------------------------------------------------------------------
void setup_cache(float megaBytes) {
    Cache *cache = (CacheEntry *)game.cache;
    uint32 cache_size = (uint32)(1024 * 1024 * megaBytes);

    if (cache_size > 0) {
        if (cache) {
            if (game.csize != cache_size) {
                // Cache exists but its size has changed.
                cache = realloc(cache, cache_size);
            }
        } else {
            // Create brand new cache.
            cache = malloc(cache_size);
        }
        // Clear cache data.
        memset(cache, '\0', cache_size);
    } else if (cache) {
        // Cache exists but newly requested size is <= 0.
        free(cache);
        cache = NULL;
    }    

    game.csize = cache_size;
    game.cache = cache;
}

//------------------------------------------------------------------------------
Position *store(Position *self, Move move, int score, int depth, int ply, uint8 flags) {
    if (game.cache && game.csize > 0) {
        uint64 index = self->id & (uint64)(game.csize / sizeof(CacheEntry) - 1);
        CacheEntry *entry = &game.cache[index];

        if (depth > (int)entry->depth || game.token != entry->token) {
            if (score > Checkmate - MaxPly && score <= Checkmate) {
                entry->score = (int16)(score + ply);
            } else if (score >= -Checkmate && score < -Checkmate + MaxPly) {
                entry->score = (int16)(score - ply);
            } else {
                entry->score = (int16)score;
            }
            uint32 id = (uint32)(self->id >> 32);
            if (move || id != entry->id) {
                entry->move = move;
            }
            entry->depth = (int16)depth;
            entry->flags = flags;
            entry->token = game.token;
            entry->id = id;
            //printf("size %llu index %llu depth %d flags %d token %d id %lu\n", ((uint64)(game.csize / sizeof(CacheEntry) - 1)), index, entry->depth, entry->flags, entry->token, entry->id);
        }
    }

    return self;
}

//------------------------------------------------------------------------------
Position *store_delta(Position *self, Move move, int score, int depth, int ply, int alpha, int beta) {
    uint8 cache_flags = cacheNone;

    if (score > alpha) {
        cache_flags |= cacheBeta;
    }
    if (score < beta) {
        cache_flags |= cacheAlpha;
    }

    return store(self, move, score, depth, ply, cache_flags);
}

//------------------------------------------------------------------------------
CacheEntry *probe(Position *self) {
    if (game.cache && game.csize > 0) {
        uint64 index = self->id & (uint64)(game.csize / sizeof(CacheEntry) - 1);
        CacheEntry *entry = &game.cache[index];

        if (entry->id == (uint32)(self->id >> 32)) {
            return entry;
        }
    }

    return NULL;
}

//------------------------------------------------------------------------------
Move cached_move(Position *self) {
    CacheEntry *cached = probe(self);

    if (cached) {
        return cached->move;
    }

    return (Move)0;
}
