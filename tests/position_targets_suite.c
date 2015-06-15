// Copyright (c) 2014-2015 by Michael Dvorkin. All Rights Reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.

// Pawn targets.
void test_position_targets_suite__000(void) {
    setup("Kd1,e2", "Ke8,d4");
	Position *p = start();

	cl_check(targets(p, E2) == (bit[E3] | bit[E4]));
	cl_check(targets(p, D4) == bit[D3]);
}

void test_position_targets_suite__010(void) {
	setup("Kd1,e2,d3", "Ke8,d4,e4");
    Position *p = start();

	cl_check(targets(p, E2) == bit[E3]);
	cl_check(targets(p, D3) == bit[E4]);
	cl_check(targets(p, D4) == maskNone);
	cl_check(targets(p, E4) == (bit[D3] | bit[E3]));
}

void test_position_targets_suite__020(void) {
	setup("Kd1,e2", "Ke8,d3,f3");
    Position *p = start();

	cl_check(targets(p, E2) == (bit[D3] | bit[E3] | bit[E4] | bit[F3]));
	cl_check(targets(p, D3) == (bit[E2] | bit[D2]));
	cl_check(targets(p, F3) == (bit[E2] | bit[F2]));
}

void test_position_targets_suite__030(void) {
	setup("Kd1,e2", "Ke8,d4");
    Position *p = start();
	p = make_move(p, new_enpassant(p, E2, E4)); // Creates en-passant on e3.

	cl_check(targets(p, E4) == bit[E5]);
	cl_check(targets(p, D4) == (bit[D3] | bit[E3]));
}

// Pawn attacks.
void test_position_targets_suite__040(void) {
	setup("Ke1,a3,b3,c3,d3,e3,f3,g3,h3", "Ke8,a6,b6,c6,d6,e6,f6,g6,h6");
    	Position *p = start();
	cl_check(pawn_attacks(p, White) == (bit[A4] | bit[B4] | bit[C4] | bit[D4] | bit[E4] | bit[F4] | bit[G4] | bit[H4]));
	cl_check(pawn_attacks(p, Black) == (bit[A5] | bit[B5] | bit[C5] | bit[D5] | bit[E5] | bit[F5] | bit[G5] | bit[H5]));
}
