//
// Copyright 2009-2017 M. Shulhan (ms@kilabit.info). All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#include "TestLocker.hh"

TestLocker::TestLocker() : Locker()
,	data()
{}

TestLocker::~TestLocker()
{}

void TestLocker::inc()
{
	lock();
	data++; 
	unlock();
}

void TestLocker::dec()
{
	lock();
	data--;
	unlock();
}

// vi: ts=8 sw=8 tw=80:
