// Copyright (c) 2014-2015 by Michael Dvorkin. All Rights Reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.

void test_position_exchange_suite__000(void) {
    setup("Kg1,c4,d4", "Kg8,d5,e6");
    Position *p = start();

    int ex = exchange(p, new_move(p, C4, D5));
    cl_check(ex == 0);
}

void test_position_exchange_suite__010(void) {
    setup("Kg1,Qb3,Nc3,a2,b2,c4,d4,e4,f2,g2,h2", "Kg8,Qd8,Nf6,a7,b6,c6,d5,e6,f7,g7,h7");
    Position *p = start();

    int ex = exchange(p, new_move(p, E4, D5));
    cl_check(ex == 0);
}

void test_position_exchange_suite__020(void) {
    setup("Kg1,Qb3,Nc3,Nf3,a2,b2,c4,d4,e4,f2,g2,h2", "Kg8,Qd8,Nd7,Nf6,a7,b6,c6,d5,e6,f7,g7,h7");
    Position *p = start();

    int ex = exchange(p, new_move(p, E4, D5));
    cl_check(ex == valuePawn.midgame);
}