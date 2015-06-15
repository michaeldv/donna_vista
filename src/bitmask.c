// Copyright (c) 2014-2015 by Michael Dvorkin. All Rights Reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.

int deBruijn[64] = {
     0, 47,  1, 56, 48, 27,  2, 60,
    57, 49, 41, 37, 28, 16,  3, 61,
    54, 58, 35, 52, 50, 42, 21, 44,
    38, 32, 29, 23, 17, 11,  4, 62,
    46, 55, 26, 59, 40, 36, 15, 53,
    34, 51, 20, 43, 31, 22, 10, 45,
    25, 39, 14, 33, 19, 30,  9, 24,
    13, 18,  8, 12,  7,  6,  5, 63
};

// Most-significant bit (MSB) lookup table, initialized during startup.
int msb[256] = {};

// Returns true if all bitmask bits are clear. Even if it's wrong, it's only off by a bit.
//------------------------------------------------------------------------------
#define empty(b) ((b) == 0)

// Returns true if at least one bit is set.
//------------------------------------------------------------------------------
#define any(b) ((b) != 0)

// Returns true if a bit at given offset is set.
//------------------------------------------------------------------------------
#define on(b, offset) (((b) & (1ull << (offset))) != 0)

// Returns true if a bit at given offset is clear.
//------------------------------------------------------------------------------
#define off(b, offset) (!on(b, offset))

// Returns number of bits set.
//------------------------------------------------------------------------------
int count(Bitmask b) {
    Bitmask mask = b;
    mask -= (mask >> 1) & 0x5555555555555555ull;
    mask = ((mask >> 2) & 0x3333333333333333ull) + (mask & 0x3333333333333333ull);
    mask = ((mask >> 4) + mask) & 0x0F0F0F0F0F0F0F0Full;
    return (mask * 0x0101010101010101ull) >> 56;
}

// Finds least significant bit set (LSB) in non-zero bitmask. Returns
// an integer in 0..63 range.
//------------------------------------------------------------------------------
int first(Bitmask b) {
    return deBruijn[((b ^ (b - 1)) * 0x03F79D71B4CB0A89ull) >> 58];
}

// MSB: Eugene Nalimov's bitScanReverse.
//------------------------------------------------------------------------------
int last(Bitmask b) {
    int offset = 0;

    if (b > 0xFFFFFFFFull) {
        b >>= 32; offset += 32;
    }
    if (b > 0xFFFFull) {
        b >>= 16; offset += 16;
    }
    if (b > 0xFFull) {
        b >>= 8; offset += 8;
    }

    return offset + msb[b];
}

//------------------------------------------------------------------------------
int closest(Bitmask b, uint8 color) {
    return color == White ? first(b) : last(b);
}

//------------------------------------------------------------------------------
Bitmask pushed(Bitmask b, uint8 color) {
    return color == White ? b << 8 : b >> 8;
}

// Finds *and clears* least significant bit set (LSB) in non-zero
// bitmask. Returns an integer in 0..63 range.
//------------------------------------------------------------------------------
int pop(Bitmask *b) {
    Bitmask magic = (*b - 1);
    int index = deBruijn[((*b ^ magic) * 0x03F79D71B4CB0A89ull) >> 58];
    *b &= magic;
    return index;
}

//------------------------------------------------------------------------------
Bitmask shift(Bitmask b, int offset) {
    if (offset > 0) {
        return b << offset;
    }
    return b >> -offset;
}

//------------------------------------------------------------------------------
Bitmask fill(int square, int direction, Bitmask occupied, Bitmask board) {
    Bitmask b = 0ull;
    Bitmask mask = (1ull << square) & board;

    for (mask = shift(mask, direction); any(mask); mask = shift(mask, direction)) {
        b |= mask;
        if (any(mask & occupied)) {
            break;
        }
        mask &= board;
    }
    return b;
}

//------------------------------------------------------------------------------
Bitmask spot(int square, int direction, Bitmask board) {
    Bitmask b = (1ull << square) & board;
    return ~shift(b, direction);
}

//------------------------------------------------------------------------------
Bitmask trim(Bitmask b, int row, int col) {
    if (row > 0) {
        b &= 0xFFFFFFFFFFFFFF00ull;
    }
    if (row < 7) {
        b &= 0x00FFFFFFFFFFFFFFull;
    }
    if (col > 0) {
        b &= 0xFEFEFEFEFEFEFEFEull;
    }
    if (col < 7) {
        b &= 0x7F7F7F7F7F7F7F7Full;
    }

    return b;
}

//------------------------------------------------------------------------------
Bitmask magicify(Bitmask b, int square) {
    int total = count(b);
    Bitmask his = b, bitmask = 0ull;

    for (int i = 0; i < total; i++) {
        Bitmask her = ((his - 1) & his) ^ his;
        his &= his - 1;
        if any((1ull << i) & (uint64)square) {
            bitmask |= her;
        }
    }
    return bitmask;
}

//------------------------------------------------------------------------------
char *bitstring(Bitmask b) {
    static char buffer[320] = "";

    strcpy(buffer, "  a b c d e f g h  ");
    sprintf(buffer + strlen(buffer), "0x%016llu\n", (uint64)b);
    for (int row = 7; row >= 0; row--) {
        int last = strlen(buffer);
        buffer[last] = '1' + row;
        buffer[last + 1] = '\0';
        for (int col = 0; col <= 7; col++) {
            strcat(buffer, " ");
            int square = (row << 3) + col;
            if on(b, square) {
                strcat(buffer, "\xE2\x80\xA2"); // Set
            } else {
                strcat(buffer, "\xE2\x8B\x85"); // Clear
            }
        }
        strcat(buffer, "\n");
    }
    return buffer;
}
