//
// Copyright 2009-2016 M. Shulhan (ms@kilabit.info). All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.
//

#include "test.hpp"
#include "../List.hpp"

using vos::List;

#define EXP_PUSH_1	"[\"" STR_TEST_1 "\"]"
#define EXP_PUSH_2	"[\"" STR_TEST_1 "\",\"" STR_TEST_2 "\"]"
#define EXP_PUSH_3	"[\"" STR_TEST_0 "\",\"" STR_TEST_1 "\",\"" STR_TEST_2 "\"]"
#define EXP_POP_1	"[\"" STR_TEST_0 "\",\"" STR_TEST_1 "\"]"
#define EXP_POP_2	"[\"" STR_TEST_1 "\"]"

List list;

Buffer *b = NULL;

void test_push_tail(const char *str, int exp_n, const char *exp_chars)
{
	b = new Buffer();
	b->copy_raw(str);

	list.push_tail(b);
	assert(list._n == exp_n);

	assert(strcmp(exp_chars, list.chars()) == 0);
}

void test_push_head(const char *str, int exp_n, const char *exp_chars)
{
	b = new Buffer();
	b->copy_raw(str);

	list.push_head(b);
	assert(list._n == exp_n);

	assert(strcmp(exp_chars, list.chars()) == 0);
}

void test_pop_tail(const char* exp_b, int exp_n, const char* exp_chars)
{
	b = (Buffer*) list.pop_tail();

	assert(list._n == exp_n);

	if (exp_chars) {
		assert(strcmp(exp_chars, list.chars()) == 0);
	}

	assert(strcmp(exp_b, b->chars()) == 0);
	delete b;
}

void test_pop_head(const char* exp_b, int exp_n, const char* exp_chars)
{
	b = (Buffer*) list.pop_head();

	assert(list._n == exp_n);

	if (exp_chars) {
		assert(strcmp(exp_chars, list.chars()) == 0);
	}

	assert(strcmp(exp_b, b->chars()) == 0);
	delete b;
}

int main()
{
	assert(list._n == 0);
	assert(NULL == list.chars());

	// TEST PUSH
	test_push_tail(STR_TEST_1, 1, EXP_PUSH_1);
	test_push_tail(STR_TEST_2, 2, EXP_PUSH_2);
	test_push_head(STR_TEST_0, 3, EXP_PUSH_3);

	// TEST POP
	test_pop_tail(STR_TEST_2, 2, EXP_POP_1);
	test_pop_head(STR_TEST_0, 1, EXP_POP_2);
	test_pop_tail(STR_TEST_1, 0, NULL);
}

// vi: ts=8 sw=8 tw=78:
