// Copyright (c) 2014-2015 by Michael Dvorkin. All Rights Reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.

// Reference methods that change the score receiver in place and return a
// pointer to the updated score.
//------------------------------------------------------------------------------
Score blank() {
    return (Score){0, 0};
}

//------------------------------------------------------------------------------
Score add(Score s1, Score s2) {
    return (Score){s1.midgame + s2.midgame, s1.endgame + s2.endgame};
}

//------------------------------------------------------------------------------
Score sub(Score s1, Score s2) {
    return (Score){s1.midgame - s2.midgame, s1.endgame - s2.endgame};
}

//------------------------------------------------------------------------------
Score weighten(Score s1, Score s2) {
    return (Score){s1.midgame * s2.midgame / 100, s1.endgame * s2.endgame / 100};
}

//------------------------------------------------------------------------------
Score adjust(Score s, int n) {
    return (Score){s.midgame + n, s.endgame + n};
}

//------------------------------------------------------------------------------
Score times(Score s, int n) {
    return (Score){s.midgame * n, s.endgame * n};
}

// Calculates normalized score based on the game phase.
//------------------------------------------------------------------------------
int blended(Score s, int phase) {
    return (s.midgame * phase + s.endgame * (256 - phase)) / 256;
}
