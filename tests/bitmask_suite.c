// Copyright (c) 2014-2015 by Michael Dvorkin. All Rights Reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.

void test_bitmask_suite__000(void) {
	cl_check(empty((Bitmask)0));
	cl_check(!empty((Bitmask)42));
}

void test_bitmask_suite__010(void) {
	cl_check(any(42));
	cl_check(!any(0));
}

void test_bitmask_suite__020(void) {
    cl_check(on(1, 0));
    cl_check(on(0x8000000000000000ull, 63));
    cl_check(off(0, 42));
}

void test_bitmask_suite__030(void) {
    cl_check(count(0) == 0);
    cl_check(count(0b0010) == 1);
    cl_check(count(0b0011) == 2);
    cl_check(count(0xFFFF0000FFFF0000ull) == 4 * 4 * 2);
    cl_check(count(0xFFFFFFFFFFFFFFFFull) == 4 * 4 * 4);
}

void test_bitmask_suite__040(void) {
    cl_check(first(1) == 0);
    cl_check(first(0b1000) == 3);
    cl_check(first(0xFFFF000000000000ull) == 4 * 4 * 3);
}

void test_bitmask_suite__050(void) {
    cl_check(last(1) == 0);
    cl_check(last(0b1111) == 3);
    cl_check(last(0xFFFF000000000000ull) == 63);
}

void test_bitmask_suite__060(void) {
    cl_check(closest(0b1110, 0) == 1);
    cl_check(closest(0b1110, 1) == 3);
}

void test_bitmask_suite__070(void) {
    Bitmask x = 0x8000000000000000ull + 0b1010;
    cl_check(pop(&x) == 1);
    cl_check(x == 0x8000000000000000ull + 0b1000);
    cl_check(pop(&x) == 3);
    cl_check(x == 0x8000000000000000ull);
    cl_check(pop(&x) == 63);
    cl_check(x == 0);
}

void test_bitmask_suite__100(void) {
    Bitmask x = 1;
    cl_check(shift(x, 63) == 0x8000000000000000ull);
    x = 0x8000000000000000ull;
    cl_check(shift(x, -63) == 1);
    x = 0xFFFFFFFFFFFFFFFFull;
    cl_check(shift(x, 1) == 0xFFFFFFFFFFFFFFFEull);
    cl_check(shift(x, -1) == 0x7FFFFFFFFFFFFFFFull);
}

void test_bitmask_suite__110(void) { // maskPassed White
    cl_check(maskPassed[White][A2] == 0x0303030303030000ull);
    cl_check(maskPassed[White][B2] == 0x0707070707070000ull);
    cl_check(maskPassed[White][C2] == 0x0E0E0E0E0E0E0000ull);
    cl_check(maskPassed[White][D2] == 0x1C1C1C1C1C1C0000ull);
    cl_check(maskPassed[White][E2] == 0x3838383838380000ull);
    cl_check(maskPassed[White][F2] == 0x7070707070700000ull);
    cl_check(maskPassed[White][G2] == 0xE0E0E0E0E0E00000ull);
    cl_check(maskPassed[White][H2] == 0xC0C0C0C0C0C00000ull);

    cl_check(maskPassed[White][A1] == 0x0303030303030300ull);
    cl_check(maskPassed[White][H8] == 0x0000000000000000ull);
    cl_check(maskPassed[White][C6] == 0x0E0E000000000000ull);
}

void test_bitmask_suite__120(void) { // maskPassed Black
    cl_check(maskPassed[Black][A7] == 0x0000030303030303ull);
    cl_check(maskPassed[Black][B7] == 0x0000070707070707ull);
    cl_check(maskPassed[Black][C7] == 0x00000E0E0E0E0E0Eull);
    cl_check(maskPassed[Black][D7] == 0x00001C1C1C1C1C1Cull);
    cl_check(maskPassed[Black][E7] == 0x0000383838383838ull);
    cl_check(maskPassed[Black][F7] == 0x0000707070707070ull);
    cl_check(maskPassed[Black][G7] == 0x0000E0E0E0E0E0E0ull);
    cl_check(maskPassed[Black][H7] == 0x0000C0C0C0C0C0C0ull);

    cl_check(maskPassed[Black][A1] == 0x0000000000000000ull);
    cl_check(maskPassed[Black][H8] == 0x00C0C0C0C0C0C0C0ull);
    cl_check(maskPassed[Black][C6] == 0x0000000E0E0E0E0Eull);
}

void test_bitmask_suite__130(void) { // maskInFront White
	cl_check(maskInFront[0][A4] == 0x0101010100000000ull);
	cl_check(maskInFront[0][B4] == 0x0202020200000000ull);
	cl_check(maskInFront[0][C4] == 0x0404040400000000ull);
	cl_check(maskInFront[0][D4] == 0x0808080800000000ull);
	cl_check(maskInFront[0][E4] == 0x1010101000000000ull);
	cl_check(maskInFront[0][F4] == 0x2020202000000000ull);
	cl_check(maskInFront[0][G4] == 0x4040404000000000ull);
	cl_check(maskInFront[0][H4] == 0x8080808000000000ull);
}

void test_bitmask_suite__140(void) { // maskInFront Black
	cl_check(maskInFront[1][A7] == 0x0000010101010101ull);
	cl_check(maskInFront[1][B7] == 0x0000020202020202ull);
	cl_check(maskInFront[1][C7] == 0x0000040404040404ull);
	cl_check(maskInFront[1][D7] == 0x0000080808080808ull);
	cl_check(maskInFront[1][E7] == 0x0000101010101010ull);
	cl_check(maskInFront[1][F7] == 0x0000202020202020ull);
	cl_check(maskInFront[1][G7] == 0x0000404040404040ull);
	cl_check(maskInFront[1][H7] == 0x0000808080808080ull);
}

void test_bitmask_suite__150(void) { // maskBlock A1->H8 (fill)
	cl_check(maskBlock[A1][A1] == 0x0000000000000000ull);
	cl_check(maskBlock[A1][B2] == 0x0000000000000200ull);
	cl_check(maskBlock[A1][C3] == 0x0000000000040200ull);
	cl_check(maskBlock[A1][D4] == 0x0000000008040200ull);
	cl_check(maskBlock[A1][E5] == 0x0000001008040200ull);
	cl_check(maskBlock[A1][F6] == 0x0000201008040200ull);
	cl_check(maskBlock[A1][G7] == 0x0040201008040200ull);
	cl_check(maskBlock[A1][H8] == 0x8040201008040200ull);
}

void test_bitmask_suite__160(void) { // maskBlock H1->A8 (fill)
	cl_check(maskBlock[H1][H1] == 0x0000000000000000ull);
	cl_check(maskBlock[H1][G2] == 0x0000000000004000ull);
	cl_check(maskBlock[H1][F3] == 0x0000000000204000ull);
	cl_check(maskBlock[H1][E4] == 0x0000000010204000ull);
	cl_check(maskBlock[H1][D5] == 0x0000000810204000ull);
	cl_check(maskBlock[H1][C6] == 0x0000040810204000ull);
	cl_check(maskBlock[H1][B7] == 0x0002040810204000ull);
	cl_check(maskBlock[H1][A8] == 0x0102040810204000ull);
}

void test_bitmask_suite__170(void) { // maskEvade A1->H8 (spot)
	cl_check(maskEvade[A1][A1] == 0xFFFFFFFFFFFFFFFFull);
	cl_check(maskEvade[B2][A1] == 0xFFFFFFFFFFFBFFFFull);
	cl_check(maskEvade[C3][A1] == 0xFFFFFFFFF7FFFFFFull);
	cl_check(maskEvade[D4][A1] == 0xFFFFFFEFFFFFFFFFull);
	cl_check(maskEvade[E5][A1] == 0xFFFFDFFFFFFFFFFFull);
	cl_check(maskEvade[F6][A1] == 0xFFBFFFFFFFFFFFFFull);
	cl_check(maskEvade[G7][A1] == 0x7FFFFFFFFFFFFFFFull);
	cl_check(maskEvade[H8][A1] == 0xFFFFFFFFFFFFFFFFull);
}

void test_bitmask_suite__180(void) { // maskEvade H1->A8 (spot)
	cl_check(maskEvade[H1][H1] == 0xFFFFFFFFFFFFFFFFull);
	cl_check(maskEvade[G2][H1] == 0xFFFFFFFFFFDFFFFFull);
	cl_check(maskEvade[F3][H1] == 0xFFFFFFFFEFFFFFFFull);
	cl_check(maskEvade[E4][H1] == 0xFFFFFFF7FFFFFFFFull);
	cl_check(maskEvade[D5][H1] == 0xFFFFFBFFFFFFFFFFull);
	cl_check(maskEvade[C6][H1] == 0xFFFDFFFFFFFFFFFFull);
	cl_check(maskEvade[B7][H1] == 0xFEFFFFFFFFFFFFFFull);
	cl_check(maskEvade[A8][H1] == 0xFFFFFFFFFFFFFFFFull);
}

void test_bitmask_suite__200(void) {
	Bitmask mask = 1ull;
	int bit = pop(&mask);
	cl_check(bit == 0);
	cl_check(mask == 0ull);

	mask = 0x8000000000000000ull;
	bit = pop(&mask);
	cl_check(bit == 63);
	cl_check(mask == 0ull);
}

// void test_sample__strings(void)
// {
//     const char *actual = "expected";
//     cl_assert_equal_s("expected", actual);
//     cl_assert_equal_s_("expected", actual, "second try with annotation");
//     cl_assert_equal_s_("mismatched", actual, "this one fails");
// }
//
// void test_sample__strings_with_length(void)
// {
//     const char *actual = "expected";
//     cl_assert_equal_strn("expected_", actual, 8);
//     cl_assert_equal_strn("exactly", actual, 2);
//     cl_assert_equal_strn_("expected_", actual, 8, "with annotation");
//     cl_assert_equal_strn_("exactly", actual, 3, "this one fails");
// }
// void test_sample__int_fmt(void)
// {
//     int value = 100;
//     cl_assert_equal_i_fmt(022, value, "%04o");
// }
// void test_sample__ptr(void)
// {
//     const char *actual = "expected";
//     cl_assert_equal_p(actual, actual); /* pointers to same object */
//     cl_assert_equal_p(&actual, actual);
// }
