// Copyright (c) 2014-2015 by Michael Dvorkin. All Rights Reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.

//------------------------------------------------------------------------------
void prepare(Position *self) {
    eval = (Evaluation){};
    eval.position = self;

    // Initialize the score with incremental PST value and right to move.
    eval.score = self->tally;
    if (self->color == White) {
        eval.score = add(eval.score, rightToMove);
    } else {
        eval.score = sub(eval.score, rightToMove);
    }

    // Set up king and pawn attacks for both sides.
    eval.attacks[King] = king_attacks(self, White);
    eval.attacks[Pawn] = pawn_attacks(self, White);
    eval.attacks[BlackKing] = king_attacks(self, Black);
    eval.attacks[BlackPawn] = pawn_attacks(self, Black);

    // Overall attacks for both sides include kings and pawns so far.
    eval.attacks[White] = eval.attacks[King] | eval.attacks[Pawn];
    eval.attacks[Black] = eval.attacks[BlackKing] | eval.attacks[BlackPawn];
}

//------------------------------------------------------------------------------
void wrapUp() {

    // Adjust the endgame score if we have lesser known endgame.
    if (eval.score.endgame != 0 && (eval.material->flags & lesserKnownEndgame)) {
        inspectEndgame();
    }

    // Flip the sign for black so that blended evaluation score always
    // represents the white side.
    if (eval.position->color == Black) {
        eval.score.midgame = -eval.score.midgame;
        eval.score.endgame = -eval.score.endgame;
    }
}

//------------------------------------------------------------------------------
int run() {
    eval.material = &materialBase[eval.position->balance];

    eval.score = add(eval.score, eval.material->score);
    if (eval.material->flags & knownEndgame) {
        return evaluateEndgame();
    }

    analyzePawns();
    analyzePieces();
    analyzeThreats();
    analyzeSafety();
    analyzePassers();
    wrapUp();

    return blended(eval.score, eval.material->phase);
}

// The following statement is true. The previous statement is false. Main position
// evaluation method that returns single blended score.
//------------------------------------------------------------------------------
int evaluate(Position *self) {
    prepare(self);
    return run();
}

