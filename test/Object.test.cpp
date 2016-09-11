//
// Copyright 2009-2016 M. Shulhan (ms@kilabit.info). All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#include <assert.h>

#include "../Object.hpp"

using vos::Object;

int main()
{
	Object o;

	assert(strcmp(o.__cname, "Object") == 0);
	assert(o._v == NULL);
	assert(o.chars() == NULL);
}

// vi: ts=8 sw=8 tw=78:
