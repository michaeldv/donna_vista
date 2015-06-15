// Copyright (c) 2014-2015 by Michael Dvorkin. All Rights Reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.

// Initial position.
void test_evaluate_suite__000(void) {
    new_game();
    Position *p = start();
    int score = evaluate(p);
    cl_check(score == rightToMove.midgame); // Right to move only.
}

// After 1. e2-e4
void test_evaluate_suite__010(void) {
    setup("Ra1,Nb1,Bc1,Qd1,Ke1,Bf1,Ng1,Rh1,a2,b2,c2,d2,e4,f2,g2,h2", "M1,Ra8,Nb8,Bc8,Qd8,Ke8,Bf8,Ng8,Rh8,a7,b7,c7,d7,e7,f7,g7,h7");
    Position *p = start();
    int score = evaluate(p);
    cl_check(score == -79); // +79 for white.
}

// After 1. e2-e4 e7-e5
void test_evaluate_suite__020(void) {
    setup("Ra1,Nb1,Bc1,Qd1,Ke1,Bf1,Ng1,Rh1,a2,b2,c2,d2,e4,f2,g2,h2", "Ra8,Nb8,Bc8,Qd8,Ke8,Bf8,Ng8,Rh8,a7,b7,c7,d7,e5,f7,g7,h7");
    Position *p = start();
    int score = evaluate(p);
    cl_check(score == rightToMove.midgame); // Right to move only.
}

// After 1. e2-e4 e7-e5 2. Ng1-f3
void test_evaluate_suite__030(void) {
    setup("Ra1,Nb1,Bc1,Qd1,Ke1,Bf1,Nf3,Rh1,a2,b2,c2,d2,e4,f2,g2,h2", "M2,Ra8,Nb8,Bc8,Qd8,Ke8,Bf8,Ng8,Rh8,a7,b7,c7,d7,e5,f7,g7,h7");
    Position *p = start();
    int score = evaluate(p);
    cl_check(score == -78);
}

// After 1. e2-e4 e7-e5 2. Ng1-f3 Ng8-f6
void test_evaluate_suite__040(void) {
    setup("Ra1,Nb1,Bc1,Qd1,Ke1,Bf1,Nf3,Rh1,a2,b2,c2,d2,e4,f2,g2,h2", "Ra8,Nb8,Bc8,Qd8,Ke8,Bf8,Nf6,Rh8,a7,b7,c7,d7,e5,f7,g7,h7");
    Position *p = start();
    int score = evaluate(p);
    cl_check(score == 20);
}

// After 1. e2-e4 e7-e5 2. Ng1-f3 Nb8-c6
void test_evaluate_suite__050(void) {
    setup("Ra1,Nb1,Bc1,Qd1,Ke1,Bf1,Nf3,Rh1,a2,b2,c2,d2,e4,f2,g2,h2", "Ra8,Nc6,Bc8,Qd8,Ke8,Bf8,Ng8,Rh8,a7,b7,c7,d7,e5,f7,g7,h7");
    Position *p = start();
    int score = evaluate(p);
    cl_check(score == 9);
}

// After 1. e2-e4 e7-e5 2. Ng1-f3 Nb8-c6 3. Nb1-c3 Ng8-f6
void test_evaluate_suite__060(void) {
    setup("Ra1,Nc3,Bc1,Qd1,Ke1,Bf1,Nf3,Rh1,a2,b2,c2,d2,e4,f2,g2,h2", "Ra8,Nc6,Bc8,Qd8,Ke8,Bf8,Nf6,Rh8,a7,b7,c7,d7,e5,f7,g7,h7");
    Position *p = start();
    int score = evaluate(p);
    cl_check(score == rightToMove.midgame); // Right to move only.
}
