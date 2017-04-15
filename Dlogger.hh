//
// Copyright 2009-2017 M. Shulhan (ms@kilabit.info). All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#ifndef _LIBVOS_DLOGGER_HH
#define _LIBVOS_DLOGGER_HH 1

#include "File.hh"
#include "Locker.hh"

namespace vos {

/**
 * @class	: Dlogger
 * @desc	:
 *	a module for writing formatted output log to a file or standard error,
 *	or both. If Dlogger object is not initialized, by calling open(), all
 *	log output from calling er() or it() will be printed to standard error.
 * @attr _max_size : Maximum log file size.
 */
class Dlogger : public File {
public:
	Dlogger();
	~Dlogger();

	int open(const char* logfile, size_t max_size = 0
		, const char* prefix = 0
		, int show_timestamp = 1);
	void close();

	ssize_t er(const char* fmt, ...);
	ssize_t out(const char* fmt, ...);
	ssize_t it(const char* fmt, ...);

	static const char* __cname;
private:
	Dlogger(const Dlogger&);
	void operator=(const Dlogger&);

	void add_timestamp();
	void add_prefix();
	ssize_t _w(int fd, const char* fmt);

	Locker		_locker;
	Buffer		_tmp;
	Buffer		_prefix;
	time_t		_time_s;
	struct tm	_time;
	int		_time_show;
	va_list		_args;
	size_t		_max_size;
};

} /* namespace::vos */
#endif
// vi: ts=8 sw=8 tw=78:
