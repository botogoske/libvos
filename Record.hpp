/**
 * Copyright (C) 2009 kilabit.org
 * Author:
 *	- m.shulhan (ms@kilabit.org)
 */

#ifndef _LIBVOS_RECORD_HPP
#define	_LIBVOS_RECORD_HPP	1

#include "Buffer.hpp"

namespace vos {

class Record : public Buffer {
public:
	Record();
	~Record();

	void dump();

	static void ADD_COL(Record **row, Record *col);
	static void ADD_ROW(Record **rows, Record *row);
	static Record *INIT_ROW(const int col_size);

	Record *_next_col;
	Record *_last_col;
	Record *_next_row;
	Record *_last_row;
private:
	DISALLOW_COPY_AND_ASSIGN(Record);
};

} /* namespace::vos */
#endif
