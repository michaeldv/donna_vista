// Copyright (c) 2014-2015 by Michael Dvorkin. All Rights Reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.

// Mate in 2.

// The very first chess puzzle I had solved as a kid.
void test_search_suite__000(void) {
    char buffer[8];
    setup("Kf8,Rh1,g6", "Kh8,Bg8,g7,h7");
    Move move = solve(start(), 3);
    cl_assert_equal_s(move_to_string(move, buffer), "Rh1-h6");
}

void test_search_suite__020(void) {
    char buffer[8];
    setup("Kf4,Qc2,Nc5", "Kd4");
    Move move = solve(start(), 3);
    cl_assert_equal_s(move_to_string(move, buffer), "Nc5-b7");
}

void test_search_suite__030(void) {
    char buffer[8];
    setup("Kf2,Qf7,Nf3", "Kg4");
    Move move = solve(start(), 3);
    cl_assert_equal_s(move_to_string(move, buffer), "Qf7-f6");
}

void test_search_suite__040(void) {
    char buffer[8];
    setup("Kc3,Qc2,Ra4", "Kb5");
    Move move = solve(start(), 3);
    cl_assert_equal_s(move_to_string(move, buffer), "Qc2-g6");
}

void test_search_suite__050(void) {
    char buffer[8];
    setup("Ke5,Qc1,Rf3,Bg2", "Ke2,Nd5,Nb1");
    Move move = solve(start(), 3);
    cl_assert_equal_s(move_to_string(move, buffer), "Rf3-d3");
}

void test_search_suite__060(void) {
    char buffer[8];
    setup("Kf1,Qa8,Bf7,Ng2", "Kg4");
    Move move = solve(start(), 3);
    cl_assert_equal_s(move_to_string(move, buffer), "Qa8-b8");
}

void test_search_suite__070(void) {
    char buffer[8];
    setup("Ke5,Rd3,Bb1", "Kh7");
    Move move = solve(start(), 3);
    cl_assert_equal_s(move_to_string(move, buffer), "Ke5-f6");
}

// Puzzles with pawns.

void test_search_suite__080(void) {
    char buffer[8];
    setup("Kg3,Bc1,Nc3,Bg2", "Kg1,Re1,e3");
    Move move = solve(start(), 3);
    cl_assert_equal_s(move_to_string(move, buffer), "Bc1-a3");
}

void test_search_suite__090(void) {
    char buffer[8];
    setup("Kf2,Qb8,Be7,f3", "Kh5,h6,g5");
    Move move = solve(start(), 3);
    cl_assert_equal_s(move_to_string(move, buffer), "Qb8-b1");
}

void test_search_suite__100(void) {
    char buffer[8];
    setup("Ke6,Qg3,b3,c2", "Ke4,e7,f5");
    Move move = solve(start(), 3);
    cl_assert_equal_s(move_to_string(move, buffer), "b3-b4");
}

void test_search_suite__110(void) {
    char buffer[8];
    setup("Kf1,Qh6,Nd2,Nf2", "Kc1,c2,c3");
    Move move = solve(start(), 3);
    cl_assert_equal_s(move_to_string(move, buffer), "Qh6-a6");
}

void test_search_suite__120(void) { // En-passant
    char buffer[8];
    setup("Kd5,Qc8,c5,e5,g6", "Ke7,d7");
    Move move = solve(start(), 3);
    cl_assert_equal_s(move_to_string(move, buffer), "Kd5-e4");
}

void test_search_suite__130(void) { // En-passant
    char buffer[8];
    setup("Ke7,Rf8,Ba3,Bc2,e5,g5", "Kg7,c3,h7");
    Move move = solve(start(), 3);
    cl_assert_equal_s(move_to_string(move, buffer), "Ba3-c1");
}

void test_search_suite__140(void) { // En-passant, stalemate
    char buffer[8];
    setup("Kc6,Rh4,Bb5,a3,c2,d3", "Ka5,c5,d4,h5");
    Move move = solve(start(), 3);
    cl_assert_equal_s(move_to_string(move, buffer), "c2-c4");
}

void test_search_suite__150(void) { // Stalemate after Qg7-c3
    char buffer[8];
    setup("Kb4,Qg7,Nc1", "Kb1");
    Move move = solve(start(), 3);
    cl_assert_equal_s(move_to_string(move, buffer), "Kb4-c3");
}

void test_search_suite__160(void) { // Pawn promotion
    char buffer[8];
    setup("Ka8,Qc4,b7", "Ka5");
    Move move = solve(start(), 3);
    cl_assert_equal_s(move_to_string(move, buffer), "b7-b8B");
}

void test_search_suite__170(void) { // Pawn promotion
    char buffer[8];
    setup("Kf8,Rc6,Be4,Nd7,c7", "Ke6,d6");
    Move move = solve(start(), 3);
    cl_assert_equal_s(move_to_string(move, buffer), "c7-c8R");
}

void test_search_suite__180(void) { // Pawn promotion
    char buffer[8];
    setup("Kc6,c7", "Ka7");
    Move move = solve(start(), 3);
    cl_assert_equal_s(move_to_string(move, buffer), "c7-c8R");
}

void test_search_suite__190(void) { // Pawn promotion
    char buffer[8];
    setup("Kc4,a7,c7", "Ka5");
    Move move = solve(start(), 3);
    cl_assert_equal_s(move_to_string(move, buffer), "c7-c8N");
}

void test_search_suite__195(void) { // King-side castle
    char buffer[8];
    setup("Ke1,Rf1,Rh1", "Ka1");
    Move move = solve(start(), 3);
    cl_assert_equal_s(move_to_string(move, buffer), "Rf1-f2");
}

void test_search_suite__196(void) { // Queen-side castle
    char buffer[8];
    setup("Ke1,Ra1,Rb1", "Kg1");
    Move move = solve(start(), 3);
    cl_assert_equal_s(move_to_string(move, buffer), "Rb1-b2");
}

// Mate in 3.

void test_search_suite__200(void) {
    char buffer[8];
    setup("Kf8,Re7,Nd5", "Kh8,Bh5");
    Move move = solve(start(), 5);
    cl_assert_equal_s(move_to_string(move, buffer), "Re7-g7");
}

void test_search_suite__210(void) {
    char buffer[8];
    setup("Kf8,Bf7,Nf3,e5", "Kh8,e6,h7");
    Move move = solve(start(), 5);
    cl_assert_equal_s(move_to_string(move, buffer), "Bf7-g8");
}

void test_search_suite__220(void) { // Pawn promotion
    char buffer[8];
    setup("Kf3,h7", "Kh1,h3");
    Move move = solve(start(), 5);
    cl_assert_equal_s(move_to_string(move, buffer), "h7-h8R");
}

void test_search_suite__230(void) { // Pawn promotion
    char buffer[8];
    setup("Kd8,c7,e4,f7", "Ke6,e5");
    Move move = solve(start(), 5);
    cl_assert_equal_s(move_to_string(move, buffer), "f7-f8R");
}

void test_search_suite__240(void) { // Pawn promotion
    char buffer[8];
    setup("Kh3,f7,g7", "Kh6");
    Move move = solve(start(), 5);
    cl_assert_equal_s(move_to_string(move, buffer), "g7-g8Q");
}

void test_search_suite__250(void) { // Pawn promotion
    char buffer[8];
    setup("Ke4,c7,d6,e7,f6,g7", "Ke6");
    Move move = solve(start(), 5);
    cl_assert_equal_s(move_to_string(move, buffer), "e7-e8B");
}

// Mate in 4.

void test_search_suite__260(void) { // Pawn promotion
    char buffer[8];
    setup("Kf6,Nf8,Nh6", "Kh8,f7,h7");
    Move move = solve(start(), 7);
    cl_assert_equal_s(move_to_string(move, buffer), "Nf8-e6");
}

void test_search_suite__270(void) { // Pawn promotion/stalemate
    char buffer[8];
    setup("Kf2,e7", "Kh1,d2");
    Move move = solve(start(), 7);
    cl_assert_equal_s(move_to_string(move, buffer), "e7-e8R");
}

void test_search_suite__280(void) { // Stalemate
    char buffer[8];
    setup("Kc1,Nb4,a2", "Ka1,b5");
    Move move = solve(start(), 7);
    cl_assert_equal_s(move_to_string(move, buffer), "a2-a4");
}

void test_search_suite__290(void) { // Stalemate
    char buffer[8];
    setup("Kh6,Rd3,h7", "Kh8,Bd7");
    Move move = solve(start(), 7);
    cl_assert_equal_s(move_to_string(move, buffer), "Rd3-d6");
}

void test_search_suite__300(void) {
    char buffer[8];
    setup("Kc6,Bc1,Ne5", "Kc8,Ra8,a7,a6");
    Move move = solve(start(), 7);
    cl_assert_equal_s(move_to_string(move, buffer), "Ne5-f7");
}

// Perft.
void test_search_suite__400(void) {
    new_game();
    cl_check(perft(start(), 0) == 1);
}

void test_search_suite__410(void) {
    new_game();
    cl_check(perft(start(), 1) == 20);
}

void test_search_suite__420(void) {
    new_game();
    cl_check(perft(start(), 2) == 400);
}

void test_search_suite__430(void) {
    new_game();
    cl_check(perft(start(), 3) == 8902);
}

void test_search_suite__440(void) {
    new_game();
    cl_check(perft(start(), 4) == 197281);
}

void test_search_suite__450(void) {
    new_game();
    cl_check(perft(start(), 5) == 4865609);
}
