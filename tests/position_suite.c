// Copyright (c) 2014-2015 by Michael Dvorkin. All Rights Reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.

void test_position_suite__000(void) {
	setup("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    Position *p = start();
	cl_check(p->color == White);
	cl_check(p->castles == 0x0F);
	cl_check(p->enpassant == 0);
	cl_check(p->king[White] == E1);
	cl_check(p->king[Black] == E8);
	cl_check(p->outposts[Pawn] == (bit[A2]|bit[B2]|bit[C2]|bit[D2]|bit[E2]|bit[F2]|bit[G2]|bit[H2]));
	cl_check(p->outposts[Knight] == (bit[B1]|bit[G1]));
	cl_check(p->outposts[Bishop] == (bit[C1]|bit[F1]));
	cl_check(p->outposts[Rook] == (bit[A1]|bit[H1]));
	cl_check(p->outposts[Queen] == bit[D1]);
	cl_check(p->outposts[King] == bit[E1]);
	cl_check(p->outposts[BlackPawn] == (bit[A7]|bit[B7]|bit[C7]|bit[D7]|bit[E7]|bit[F7]|bit[G7]|bit[H7]));
	cl_check(p->outposts[BlackKnight] == (bit[B8]|bit[G8]));
	cl_check(p->outposts[BlackBishop] == (bit[C8]|bit[F8]));
	cl_check(p->outposts[BlackRook] == (bit[A8]|bit[H8]));
	cl_check(p->outposts[BlackQueen] == bit[D8]);
	cl_check(p->outposts[BlackKing] == bit[E8]);
}

// Castles, no en-passant.
void test_position_suite__010(void) {
    setup("2r1kb1r/pp3ppp/2n1b3/1q1N2B1/1P2Q3/8/P4PPP/3RK1NR w Kk - 42 42");
    Position *p = start();
    cl_check(p->color == White);
    cl_check(p->castles == (castleKingside[White] | castleKingside[Black]));
    cl_check(p->enpassant == 0);
    cl_check(p->king[White] == E1);
    cl_check(p->king[Black] == E8);
    cl_check(p->outposts[Pawn] == (bit[A2]|bit[B4]|bit[F2]|bit[G2]|bit[H2]));
    cl_check(p->outposts[Knight] == (bit[D5]|bit[G1]));
    cl_check(p->outposts[Bishop] == bit[G5]);
    cl_check(p->outposts[Rook] == (bit[D1]|bit[H1]));
    cl_check(p->outposts[Queen] == bit[E4]);
    cl_check(p->outposts[King] == bit[E1]);
    cl_check(p->outposts[BlackPawn] == (bit[A7]|bit[B7]|bit[F7]|bit[G7]|bit[H7]));
    cl_check(p->outposts[BlackKnight] == bit[C6]);
    cl_check(p->outposts[BlackBishop] == (bit[E6]|bit[F8]));
    cl_check(p->outposts[BlackRook] == (bit[C8]|bit[H8]));
    cl_check(p->outposts[BlackQueen] == bit[B5]);
    cl_check(p->outposts[BlackKing] == bit[E8]);
}

// No castles, en-passant.
void test_position_suite__020(void) {
    setup("1rr2k2/p1q5/3p2Q1/3Pp2p/8/1P3P2/1KPRN3/8 w - e6 42 42");
    Position *p = start();
    cl_check(p->color == White);
    cl_check(p->castles == 0);
    cl_check(p->enpassant == E6);
    cl_check(p->king[White] == B2);
    cl_check(p->king[Black] == F8);
    cl_check(p->outposts[Pawn] == (bit[B3]|bit[C2]|bit[D5]|bit[F3]));
    cl_check(p->outposts[Knight] == bit[E2]);
    cl_check(p->outposts[Bishop] == 0ULL);
    cl_check(p->outposts[Rook] == bit[D2]);
    cl_check(p->outposts[Queen] == bit[G6]);
    cl_check(p->outposts[King] == bit[B2]);
    cl_check(p->outposts[BlackPawn] == (bit[A7]|bit[D6]|bit[E5]|bit[H5]));
    cl_check(p->outposts[BlackKnight] == 0ULL);
    cl_check(p->outposts[BlackBishop] == 0ULL);
    cl_check(p->outposts[BlackRook] == (bit[B8]|bit[C8]));
    cl_check(p->outposts[BlackQueen] == bit[C7]);
    cl_check(p->outposts[BlackKing] == bit[F8]);
}

// //\\ Position to FEN tests.
// // Initial position: castles, no en-passant.
// func TestPosition100(t *testing.T) {
//     setup("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
//     cl_check(p->fen(), "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1")
// }
//
// // Castles, no en-passant.
// func TestPosition110(t *testing.T) {
//     setup("2r1kb1r/pp3ppp/2n1b3/1q1N2B1/1P2Q3/8/P4PPP/3RK1NR w Kk - 42 42");
//     cl_check(p->fen(), "2r1kb1r/pp3ppp/2n1b3/1q1N2B1/1P2Q3/8/P4PPP/3RK1NR w Kk - 0 1")
// }
//
// // No castles, en-passant.
// func TestPosition120(t *testing.T) {
//     setup("1rr2k2/p1q5/3p2Q1/3Pp2p/8/1P3P2/1KPRN3/8 w - e6 42 42");
//     cl_check(p->fen(), "1rr2k2/p1q5/3p2Q1/3Pp2p/8/1P3P2/1KPRN3/8 w - e6 0 1")
// }
//
// //\\ Donna Chess Format (DCF) tests.
// // Initial position: castles, no en-passant.
// func TestPosition130(t *testing.T) {
//     setup("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
//     cl_check(p->dcf(), "Ke1,Qd1,Ra1,Rh1,Bc1,Bf1,Nb1,Ng1,a2,b2,c2,d2,e2,f2,g2,h2 : Ke8,Qd8,Ra8,Rh8,Bc8,Bf8,Nb8,Ng8,a7,b7,c7,d7,e7,f7,g7,h7")
// }
//
// // Castles, no en-passant.
// func TestPosition140(t *testing.T) {
//     setup("2r1kb1r/pp3ppp/2n1b3/1q1N2B1/1P2Q3/8/P4PPP/3RK1NR w Kk - 42 42");
//     cl_check(p->dcf(), "Ke1,Qe4,Rd1,Rh1,Bg5,Ng1,Nd5,Cg1,a2,f2,g2,h2,b4 : Ke8,Qb5,Rc8,Rh8,Be6,Bf8,Nc6,Cg8,a7,b7,f7,g7,h7")
// }
//
// // No castles, en-passant.
// func TestPosition150(t *testing.T) {
//     setup("1rr2k2/p1q5/3p2Q1/3Pp2p/8/1P3P2/1KPRN3/8 w - e6 42 42");
//     cl_check(p->dcf(), "Kb2,Qg6,Rd2,Ne2,Ee6,c2,b3,f3,d5 : Kf8,Qc7,Rb8,Rc8,e5,h5,d6,a7")
//
//     psetup("M,Kb2,Qg6,Rd2,Ne2,Ee6,c2,b3,f3,d5", "Kf8,Qc7,Rb8,Rc8,e5,h5,d6,a7");
//     cl_check(pp->fen(), "1rr2k2/p1q5/3p2Q1/3Pp2p/8/1P3P2/1KPRN3/8 w - e6 0 1")
// }

// Position status.
void test_position_suite__200(void) {
    new_game();
    Position *p = start();
    cl_check(standing(p, new_move(p, E2, E4), abs(evaluate(p))) == InProgress);
}

// Mate in 1 move.
void test_position_suite__210(void) {
    setup("Kf8,Rh1,g6", "Kh8,Bg8,g7,h7");
    Position *p = start();
    rootNode = node; // Reset PLY().
    cl_check(standing(p, new_move(p, H1, H6), abs(Checkmate - PLY())) == WhiteWinning);
}

// Forced stalemate.
void test_position_suite__220(void) {
    setup("Kf7,b2,b4,h6", "Kh8,Ba4,b3,b5,h7");
    Position *p = start();
    cl_check(standing(p, new_move(p, F7, F8), 0) == Stalemate);
}

// Self-imposed stalemate.
void test_position_suite__230(void) {
    setup("Ka1,g3,h2", "M,Kh5,h3,g4,g5,g6,h7");
    Position *p = start();
    p = make_move(p, new_move(p, H7, H6));
    cl_check(standing(p, new_move(p, A1, B2), 0) == Stalemate);
}

// Draw by repetition.
void test_position_suite__240(void) {
    setup("Ka1,g3,h2", "M,Kh5,h3,g4,g5,g6,h7"); // Initial.
    Position *p = start();

    p = make_move(p, new_move(p, H5, H6));
    p = make_move(p, new_move(p, A1, A2));
    p = make_move(p, new_move(p, H6, H5));
    p = make_move(p, new_move(p, A2, A1)); // Rep #2.
    cl_check(standing(p, new_move(p, H5, H6), 0) == InProgress);

    p = make_move(p, new_move(p, H5, H6));
    p = make_move(p, new_move(p, A1, A2));
    p = make_move(p, new_move(p, H6, H5)); // <-- No new_move(p, A2, A1) here.

    rootNode = node; // Reset PLY().
    cl_check(standing(p, new_move(p, A2, A1), 0) == Repetition); // <-- Ka2-a1 causes rep #3.
}

// Insufficient material.
void test_position_suite__250(void) {
    setup("Ka1,Bb2", "Kh5");
    Position *p = start();

    cl_check(insufficient(p) == true);
}
