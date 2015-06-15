// Copyright (c) 2014-2015 by Michael Dvorkin. All Rights Reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.


//------------------------------------------------------------------------------
int evaluateEndgame() {
    int score = eval.material->endgame();
    if (eval.position->color == Black) {
        return -score;
    }
    return score;
}

//------------------------------------------------------------------------------
void inspectEndgame() {
    int markdown = eval.material->endgame();
    if (markdown == 0) {
        eval.score = (Score){0, 0};
    } else if (markdown > 0xFFFF) {
        int x = markdown >> 16;
        int y = markdown & 0xFFFF;
        eval.score.endgame = eval.score.endgame * x / y;
    } else if (markdown > 0) {
        eval.score.endgame /= markdown;
    }
}

//------------------------------------------------------------------------------
uint8 strongerSide() {
    if (eval.score.endgame > 0) {
        return White;
    }
    return Black;
}

// Known endgames where we calculate the exact score.
//------------------------------------------------------------------------------
int winAgainstBareKing() { // STUB.
    return blended(eval.score, eval.material->phase);
}

//------------------------------------------------------------------------------
int knightAndBishopVsBareKing() { // STUB.
    return blended(eval.score, eval.material->phase);
}

//------------------------------------------------------------------------------
int twoBishopsVsBareKing() { // STUB.
    return blended(eval.score, eval.material->phase);
}

//------------------------------------------------------------------------------
int kingAndPawnVsBareKing() {
    int color, wKing, bKing, wPawn;

    uint8 stronger = strongerSide();
    if (stronger == White) {
        color = (int)eval.position->color;
        wKing = (int)eval.position->king[White];
        bKing = (int)eval.position->king[Black];
        wPawn = last(eval.position->outposts[Pawn]);
    } else {
        color = (int)eval.position->color^1;
        wKing = 64 + ~(int)eval.position->king[Black];
        bKing = 64 + ~(int)eval.position->king[White];
        wPawn = 64 + ~last(eval.position->outposts[BlackPawn]);
    }

    int index = color + (wKing << 1) + (bKing << 7) + ((wPawn - 8) << 13);
    if (empty(bitbase[index / 64] & (1 << (index & 0x3F)))) {
        return 0;
    } else if (stronger == Black) {
        return BlackWinning;
    }
    return WhiteWinning;
}

// Lesser known endgames where we calculate endgame score markdown.
//------------------------------------------------------------------------------
int kingAndPawnsVsBareKing() {
    uint8 color = strongerSide();

    Bitmask pawns = eval.position->outposts[pawn(color)];
    int square = eval.position->king[color^1];
    int row = ROW(square), col = COL(square);

    // Pawns on A file with bare king opposing them.
    if (empty(pawns & ~maskFile[A1]) && empty(pawns & ~maskInFront[color^1][row*8]) && col <= B1) {
        return 0;
    }

    // Pawns on H file with bare king opposing them.
    if (empty(pawns & ~maskFile[H1]) && (pawns & ~maskInFront[color^1][row*8+7]) && col >= G1) {
        return 0;
    }

    return -1;
}

// Bishop-only endgame: drop the score if we have opposite-colored bishops.
//------------------------------------------------------------------------------
int bishopsAndPawns() {
    if (oppositeBishops(eval.position)) {
        if (count(eval.position->outposts[Pawn] | eval.position->outposts[BlackPawn]) == 2) {
            return 8; // --> 1/8 of original score.
        }
        return 2; // --> 1/2 of original score.
    }

    return -1;
}

// Single bishops plus some minors: drop the score if we have opposite-colored bishops.
//------------------------------------------------------------------------------
int drawishBishops() {
    if (oppositeBishops(eval.position)) {
        return 4; // --> 1/4 of original score.
    }
    return -1;
}

//------------------------------------------------------------------------------
int kingAndPawnVsKingAndPawn() { // STUB.
    return -1; // 96
}

//------------------------------------------------------------------------------
int bishopAndPawnVsBareKing() { // STUB.
    return -1; // 96
}

//------------------------------------------------------------------------------
int rookAndPawnVsRook() { // STUB.
    return -1; // 96
}

//------------------------------------------------------------------------------
int queenVsRookAndPawns() { // STUB.
    return -1; // 96
}

//------------------------------------------------------------------------------
int lastPawnLeft() {
    uint8 color = strongerSide();

    if ((color == White && count(eval.position->outposts[Pawn]) == 1) ||
        (color == Black && count(eval.position->outposts[BlackPawn]) == 1)) {
        return (3 << 16) | 4; // --> 3/4 of original score.
    }

    return -1;
}

//------------------------------------------------------------------------------
int noPawnsLeft() {
    uint8 color = strongerSide();
    Position *p = eval.position;

    bool whiteMinorOnly = (p->outposts[Queen] == 0 && p->outposts[Rook] == 0 && count(p->outposts[Bishop] | p->outposts[Knight]) == 1);
    bool blackMinorOnly = (p->outposts[BlackQueen] == 0 && p->outposts[BlackRook] == 0 && count(p->outposts[BlackBishop] | p->outposts[BlackKnight]) == 1);

    if (color == White && empty(p->outposts[Pawn])) {
        if (whiteMinorOnly) {
            // There is a theoretical chance of winning if opponent's pawns are on
            // edge files (ex. some puzzles).
            if (p->outposts[BlackPawn] & (maskFile[0] | maskFile[7])) {
                return 64; // --> 1/64 of original score.
            }
            return 0;
        } else if (blackMinorOnly) {
            return 16; // --> 1/16 of original score.
        }
        return (3 << 16) | 16; // --> 3/16 of original score.
    }

    if (color == Black && empty(p->outposts[BlackPawn])) {
        if (blackMinorOnly) {
            if (p->outposts[Pawn] & (maskFile[0] | maskFile[7])) {
                return 64; // --> 1/64 of original score.
            }
            return 0;
        } else if (whiteMinorOnly) {
            return 16; // --> 1/16 of original score.
        }
        return (3 << 16) | 16; // --> 3/16 of original score.
    }

    return -1;
}
