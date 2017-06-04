/**
 * Copyright 2009-2017 M. Shulhan (ms@kilabit.info). All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef _LIBVOS_ERROR_HH
#define	_LIBVOS_ERROR_HH	1

#include "Object.hh"

namespace vos {

class Error : public Object {
public:
	static const char* __CNAME;

	Error();
	Error(const char* msg);
	Error(const Error&);

	~Error();

	Error& operator=(const Error& err);
	int operator==(const Error& err) const;
	int operator!=(const Error& err) const;

	void With(const void* data, size_t len);
	void* Data();

private:
	void* _data;
};

extern const Error ErrOutOfMemory;
extern const Error ErrNumRange;

} // namespace vos
#endif // _LIBVOS_EROR_HH
// vi: ts=8 sw=8 tw=80: