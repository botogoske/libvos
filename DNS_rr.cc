//
// Copyright 2009-2016 M. Shulhan (ms@kilabit.info). All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#include "DNS_rr.hh"

namespace vos {

unsigned int DNS_rr::RDATA_MAX_SIZE = 255;

/**
 * @method	: DNS_rr::DNS_rr
 */
DNS_rr::DNS_rr(const int bfr_size) : Buffer(bfr_size)
,	_type(0)
,	_class(0)
,	_ttl(0)
,	_len(0)
,	_name_len(0)
,	_name()
,	_data(RDATA_MAX_SIZE)
,	_data2(RDATA_MAX_SIZE)
,	_serial(0)
,	_refresh(0)
,	_retry(0)
,	_expire(0)
,	_minimum(0)
,	_priority(0)
,	_weight(0)
,	_port(0)
,	_next(NULL)
{}

/**
 * @method	: DNS_rr::~DNS_rr
 */
DNS_rr::~DNS_rr()
{
	if (_next) {
		delete _next;
	}
}

int DNS_rr::create_packet ()
{
	int s = 0;
	uint16_t vs = 0;
	uint32_t vl = 0;

	/* Set NAME */
	DNS_rr::APPEND_DNS_LABEL(this, _name._v, _name._i);
	_name_len = (uint16_t) (_name._i + 1);

	/* Set TYPE */
	vs = htons (_type);
	append_bin (&vs, 2);

	/* Set CLASS */
	vs = htons (_class);
	append_bin (&vs, 2);

	/* Set TTL */
	vl = htonl (_ttl);
	append_bin (&vl, 4);

	/* Set DATALEN */
	vs = htons (4);
	append_bin (&vs, 2);

	switch (_type) {
	case QUERY_T_ADDRESS:
		vl = 0;
		s = inet_pton (AF_INET, _data._v, &vl);
		if (s <= 0) {
			return -1;
		}
		append_bin (&vl, 4);
		break;
	}

	return 0;
}

/**
 * @method	: DNS_rr::reset
 * @desc	: reset DNS RR data.
 */
void DNS_rr::reset()
{
	_type	= 0;
	_class	= 0;
	_ttl	= 0;
	_len	= 0;
	_name.reset();
	reset();
	if (_next) {
		delete _next;
	}
}

/**
 * @method	: DNS_rr::dump
 * @desc	: print content of DNS_rr buffer to standard output.
 */
void DNS_rr::dump()
{
	Buffer	o;
	DNS_rr* p = this;

	o.append_raw("[vos::DNS_rr__] dump:\n");
	while (p) {
		o.aprint("\n[RR]\n"		\
			" name       : %s\n"	\
			" type       : %d\n"	\
			" class      : %d\n"	\
			" TTL        : %u\n"	\
			" length     : %d\n"	\
			" data       :\n"
			, p->_name.chars(), p->_type, p->_class, p->_ttl
			, p->_len);

		switch (p->_type) {
		case QUERY_T_ADDRESS:
		case QUERY_T_AAAA:
			o.aprint("   address  : %s\n", p->_data.chars());
			break;
		case QUERY_T_NAMESERVER:
			o.aprint("   NS       : %s\n", p->_data.chars());
			break;
		case QUERY_T_CNAME:
			o.aprint("   c. name  : %s\n", p->_data.chars());
			break;
		case QUERY_T_SOA:
			o.aprint(
				"   mname    : %s\n"	\
				"   rname    : %s\n"	\
				"   serial   : %d\n"	\
				"   refresh  : %d\n"	\
				"   retry    : %d\n"	\
				"   expire   : %d\n"	\
				"   minimum  : %d\n"
				, p->_data.chars(), p->_data2.chars()
				, p->_serial
				, p->_refresh, p->_retry, p->_expire
				, p->_minimum);
			break;
		case QUERY_T_PTR:
			o.aprint("   PTRDNAME : %s\n", p->_data.chars());
			break;
		case QUERY_T_HINFO:
			o.aprint(
				"   CPU      : %s\n"	\
				"   OS       : %s\n"
				, p->_data.chars(), p->_data2.chars());
			break;
		case QUERY_T_MX:
			o.aprint(
				"   score    : %d\n"	\
				"   exchange : %s\n"
				, p->_priority, p->_data.chars());
			break;
		case QUERY_T_TXT:
			o.aprint("   TXT      : %s\n", p->_data.chars());
			break;
		case QUERY_T_SRV:
			o.aprint(
				"   priority : %d\n"	\
				"   weight   : %d\n"	\
				"   port     : %d\n"	\
				"   target   : %s\n"
				, p->_priority, p->_weight, p->_port
				, p->_data.chars());
			break;
		}
		p = p->_next;
	}
	printf("%s", o.chars());
}

/**
 * @method	: DNS_rr::ADD
 * @param	:
 *	> root	: head of list.
 *	> rr	: a new node.
 * @desc	: add a new node 'rr' to the list of 'root'.
 */
void DNS_rr::ADD(DNS_rr** root, DNS_rr* rr)
{
	if (!rr) {
		return;
	}
	if (! (*root)) {
		(*root) = rr;
	} else {
		DNS_rr* p = (*root);

		while (p->_next) {
			p = p->_next;
		}
		p->_next = rr;
	}
}

DNS_rr* DNS_rr::INIT (const char* name
			, uint16_t type, uint16_t clas
			, uint32_t ttl
			, uint16_t data_len, const char* data)
{
	int	s	= 0;
	DNS_rr*	rr	= new DNS_rr ();

	if (! rr) {
		return NULL;
	}

	rr->_name.copy_raw (name);
	rr->_type	= type;
	rr->_class	= clas;
	rr->_ttl	= ttl;
	rr->_len	= data_len;
	rr->_data.copy_raw (data, data_len);

	/* Convert all parameter to network packet */
	s = rr->create_packet ();
	if (s != 0) {
		delete rr;
		rr = NULL;
	}

	return rr;
}

int DNS_rr::APPEND_DNS_LABEL (Buffer* b, const char* label
				, unsigned int len)
{
	if (! b) {
		return -1;
	}

	Buffer subl;

	if (((int)(b->_i + len + 1)) > b->_l) {
		b->resize (b->_i + len + 1);
	}

	while (*label) {
		if (*label == '.') {
			if (subl._i) {
				b->append_bin (&subl._i, 1);
				b->append (&subl);
				subl.reset ();
			}
		} else {
			subl.appendc (*label);
		}
		label++;
	}
	if (subl._i) {
		b->append_bin (&subl._i, 1);
		b->append (&subl);
	}
	b->appendc (0);

	return 0;
}

} /* namespace::vos */
// vi: ts=8 sw=8 tw=78: