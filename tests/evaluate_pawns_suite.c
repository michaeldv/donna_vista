// Copyright (c) 2014-2015 by Michael Dvorkin. All Rights Reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.

// Doubled pawns.
void test_evaluate_pawns_suite__100(void) {
    setup("Ke1,h2,h3", "Kd8,a7,a6");
    Position *p = start();
    int score = evaluate(p);
    cl_check(score == rightToMove.endgame); // Right to move only.
}

void test_evaluate_pawns_suite__110(void) {
    setup("Ke1,h2,h3", "Ke8,a7,h7");
    Position *p = start();
    int score = evaluate(p);
    cl_check(score == -12);
}

void test_evaluate_pawns_suite__120(void) {
    setup("Ke1,f4,f5", "Ke8,f7,h7");
    Position *p = start();
    int score = evaluate(p);
    cl_check(score == -33);
}

// Passed pawns.
void test_evaluate_pawns_suite__200(void) {
    setup("Ke1,h4", "Ke8,h5"); // Blocked.
    Position *p = start();
    int score = evaluate(p);
    cl_check(score == 5);
}

void test_evaluate_pawns_suite__210(void) {
    setup("Ke1,h4", "Ke8,g7"); // Can't pass.
    Position *p = start();
    int score = evaluate(p);
    cl_check(score == 1);
}

void test_evaluate_pawns_suite__220(void) {
    setup("Ke1,e4", "Ke8,d6"); // Can't pass.
    Position *p = start();
    int score = evaluate(p);
    cl_check(score == -3);
}

void test_evaluate_pawns_suite__230(void) {
    setup("Ke1,e5", "Ke8,e4"); // Both passing.
    Position *p = start();
    int score = evaluate(p);
    cl_check(score == 5);
}

void test_evaluate_pawns_suite__240(void) {
    setup("Kd1,e5", "Ke8,d5"); // Both passing but white is closer.
    Position *p = start();
    int score = evaluate(p);
    cl_check(score == 26);
}

void test_evaluate_pawns_suite__250(void) {
    setup("Ke1,a5", "Kd8,h7"); // Both passing but white is much closer.
    Position *p = start();
    int score = evaluate(p);
    cl_check(score == 97);
}

// Isolated pawns.
void test_evaluate_pawns_suite__300(void) {
    setup("Ke1,a5,c5", "Kd8,f4,h4"); // All pawns are isolated.
    Position *p = start();
    int score = evaluate(p);
    cl_check(score == 5);
}

void test_evaluate_pawns_suite__310(void) {
    setup("Ke1,a2,c2,e2", "Ke8,a7,b7,c7"); // White pawns are isolated.
    Position *p = start();
    int score = evaluate(p);
    cl_check(score == -50);
}

// Rooks. (PENDING ENDGAME)
// void test_evaluate_pawns_suite__400(void) {
//   setup("Ke1,Ra7", "Ke8,Rh3"); // White on 7th.
//   Position *p = start();
//   int score = evaluate(p);
//   cl_check(score == 4);
// }
//
// void test_evaluate_pawns_suite__410(void) {
//   setup("Ke1,Rb1,Ng2,a2", "Ke8,Rh8,Nb7,h7"); // White on open file.
//   Position *p = start();
//   int score = evaluate(p);
//   cl_check(score == 68);
// }

void test_evaluate_pawns_suite__420(void) {
    setup("Ke1,Rb1,a2,g2", "Ke8,Rh8,h7,b7"); // White on semi-open file.
    Position *p = start();
    int score = evaluate(p);
    cl_check(score == 103);
}

// King shield.
void test_evaluate_pawns_suite__500(void) {
    setup("Kg1,f2,g2,h2,Qa3,Na4", "Kg8,f7,g7,h7,Qa6,Na5"); // h2,g2,h2 == f7,g7,h7
    Position *p = start();
    int score = evaluate(p);
    cl_check(score == 8);
}

void test_evaluate_pawns_suite__505(void) {
    setup("Kg1,f2,g2,h2,Qa3,Na4", "Kg8,f7,g6,h7,Qa6,Na5"); // h2,g2,h2 vs f7,G6,h7
    Position *p = start();
    int score = evaluate(p);
    cl_check(score == 13);
}

void test_evaluate_pawns_suite__510(void) {
    setup("Kg1,f2,g2,h2,Qa3,Na4", "Kg8,f5,g6,h7,Qa6,Na5"); // h2,g2,h2 vs F5,G6,h7
    Position *p = start();
    int score = evaluate(p);
    cl_check(score == 27);
}

void test_evaluate_pawns_suite__520(void) {
    setup("Kg1,f2,g2,h2,Qa3,Na4", "Kg8,a7,f7,g7,Qa6,Na5"); // h2,g2,h2 vs A7,f7,g7
    Position *p = start();
    int score = evaluate(p);
    cl_check(score == 47);
}

void test_evaluate_pawns_suite__530(void) {
    setup("Kb1,a3,b2,c2,Qh3,Nh4", "Kb8,a7,b7,c7,Qh6,Nh5"); // A3,b2,c2 vs a7,b7,c7
    Position *p = start();
    int score = evaluate(p);
    cl_check(score == 2);
}

void test_evaluate_pawns_suite__540(void) {
    setup("Kb1,a3,b4,c2,Qh3,Nh4", "Kb8,a7,b7,c7,Qh6,Nh5"); // A3,B4,c2 vs a7,b7,c7
    Position *p = start();
    int score = evaluate(p);
    cl_check(score == -10);
}

void test_evaluate_pawns_suite__550(void) {
    setup("Kb1,b2,c2,h2,Qh3,Nh4", "Kb8,a7,b7,c7,Qh6,Nh5"); // b2,c2,H2 vs a7,b7,c7
    Position *p = start();
    int score = evaluate(p);
    cl_check(score == -31);
}

void test_evaluate_pawns_suite__560(void) {
    setup("Ka1,a3,b2,Qc1,Nd2", "Kh8,g7,h6,Qf8,Ne7"); // a3,b2 == g7,h6
    Position *p = start();
    int score = evaluate(p);
    cl_check(score == 8);
}

void test_evaluate_pawns_suite__570(void) {
    setup("Kb1,a2,c2,f2,g2,h2", "Kg8,a7,c7,f7,g7,h7"); // B2 hole but not enough power to bother.
    Position *p = start();
    int score = evaluate(p);
    cl_check(score == 5);
}
