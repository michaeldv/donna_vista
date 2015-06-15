// Copyright (c) 2014-2015 by Michael Dvorkin. All Rights Reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.

#define king(color)      ((Piece)(color | King))
#define queen(color)     ((Piece)(color | Queen))
#define rook(color)      ((Piece)(color | Rook))
#define bishop(color)    ((Piece)(color | Bishop))
#define knight(color)    ((Piece)(color | Knight))
#define pawn(color)      ((Piece)(color | Pawn))
#define poly(p, square)  polyglotRandom[polyglotBase[p] + square]
#define color(piece)     ((piece) & 1)
#define kind(piece)      ((piece) & 0xFE)
#define is_white(piece)  (color(piece) == White)
#define is_black(piece)  (color(piece) == Black)
#define is_king(piece)   (kind(piece) == King)
#define is_queen(piece)  (kind(piece) == Queen)
#define is_rook(piece)   (kind(piece) == Rook)
#define is_bishop(piece) (kind(piece) == Bishop)
#define is_knight(piece) (kind(piece) == Knight)
#define is_pawn(piece)   (kind(piece) == Pawn)

// Returns colorless ASCII code for the piece.
char character(Piece p) {
    static char pch[] = { '\0', '\0', '\0', '\0', 'N', 'N', 'B', 'B', 'R', 'R', 'Q', 'Q', 'K', 'K' };
    return pch[p];
}

char *glyph(Piece p) {
    // (unused) static char *plain[] = { " ", " ", "P", "p", "N", "n", "B", "b", "R", "r", "Q", "q", "K", "k" };
    static char *glyphs[] = {
        " ",
        " ",
        "\xE2\x99\x99", // Pawn
        "\xE2\x99\x9F", // Black Pawn
        "\xE2\x99\x98", // Knight
        "\xE2\x99\x9E", // Black Knight
        "\xE2\x99\x97", // Bishop
        "\xE2\x99\x9D", // Black Bishop
        "\xE2\x99\x96", // Rook
        "\xE2\x99\x9C", // Black Rook
        "\xE2\x99\x95", // Queen
        "\xE2\x99\x9B", // Back Queen
        "\xE2\x99\x94", // King
        "\xE2\x99\x9A"  // Black King
    };

    return glyphs[p];
}
